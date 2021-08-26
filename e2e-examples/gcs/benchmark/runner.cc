#include "runner.h"

#include "absl/strings/string_view.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_cat.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "e2e-examples/gcs/crc32c/crc32c.h"

using ::google::storage::v2::Object;
using ::google::storage::v2::ReadObjectRequest;
using ::google::storage::v2::ReadObjectResponse;
using ::google::storage::v2::StartResumableWriteRequest;
using ::google::storage::v2::StartResumableWriteResponse;
using ::google::storage::v2::WriteObjectRequest;
using ::google::storage::v2::WriteObjectResponse;

int32_t GetChannelId(void* stub_handle) {
  static std::unordered_map<void*, int32_t> handle_to_id_map;
  static absl::Mutex lock;

  absl::MutexLock l(&lock);
  auto i = handle_to_id_map.find(stub_handle);
  if (i != handle_to_id_map.end()) {
    return i->second;
  }

  auto new_id = int32_t(handle_to_id_map.size() + 1);
  handle_to_id_map[stub_handle] = new_id;
  return new_id;
}

std::string ToV2BucketName(absl::string_view bucket_name) {
  static const absl::string_view V2_BUCKET_NAME_PREFIX = "projects/_/buckets/";
  return absl::StrCat(V2_BUCKET_NAME_PREFIX, bucket_name);
}

Runner::Runner(Runner::Parameter parameter,
               std::shared_ptr<RunnerWatcher> watcher)
    : parameter_(parameter), watcher_(watcher) {}

void Runner::Run() {
  switch (parameter_.operation_type) {
    case OperationType::Read:
      return_code_ = RunRead();
      break;
    case OperationType::Write:
      return_code_ = RunWrite();
      break;
    default:
      return_code_ = false;
      break;
  }
}

bool Runner::RunRead() {
  for (int run = 0; run < parameter_.runs; run++) {
    auto storage = parameter_.storage_stub_provider->GetStorageStub();

    std::string object;
    if (parameter_.object_stop > 0) {
      int offset = run + ((parameter_.id - 1) * parameter_.runs);
      int index =
          (offset % (parameter_.object_stop - parameter_.object_start)) +
          parameter_.object_start;
      char real_object[256];
      int cx = snprintf(real_object, sizeof(real_object),
                        parameter_.object_format.c_str(), index);
      if (cx < 0) {
        std::cerr << "snprintf(object, index) failed" << std::endl;
        return false;
      }
      object = real_object;
    } else {
      object = parameter_.object;
    }

    ReadObjectRequest request;
    request.set_bucket(ToV2BucketName(parameter_.bucket));
    request.set_object(object);
    if (parameter_.read_offset >= 0) {
      request.set_read_offset(parameter_.read_offset);
    }
    if (parameter_.read_limit >= 0) {
      request.set_read_limit(parameter_.read_limit);
    }

    absl::Time run_start = absl::Now();
    grpc::ClientContext context;
    std::unique_ptr<grpc::ClientReader<ReadObjectResponse>> reader =
        storage.stub->ReadObject(&context, request);

    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);

    ReadObjectResponse response;
    while (reader->Read(&response)) {
      const auto& content = response.checksummed_data().content();
      int64_t content_size = content.size();

      if (parameter_.check_crc32c) {
        uint32_t content_crc = response.checksummed_data().crc32c();
        uint32_t calculated_crc =
            crc32c_value((const uint8_t*)content.c_str(), content_size);
        if (content_crc != calculated_crc) {
          std::cerr << "CRC32 is not identical. " << content_crc << " vs "
                    << calculated_crc << std::endl;
          break;
        }
      }

      RunnerWatcher::Chunk chunk = {absl::Now(), content_size};
      chunks.push_back(chunk);
      total_bytes += content_size;
    }

    auto status = reader->Finish();
    absl::Time run_end = absl::Now();

    if (!status.ok()) {
      std::cerr << "Download Failure!" << std::endl;
      std::cerr << "Peer: " << context.peer() << std::endl;
      std::cerr << "Status: " << std::endl;
      std::cerr << "- Code: " << status.error_code() << std::endl;
      std::cerr << "- Message: " << status.error_message() << std::endl;
      std::cerr << "- Details: " << status.error_details() << std::endl;
    }

    auto keep_going = parameter_.storage_stub_provider->ReportResult(
        storage.handle, status, context, run_end - run_start, total_bytes);

    watcher_->NotifyCompleted(
        OperationType::Read, parameter_.id, GetChannelId(storage.handle),
        context.peer(), parameter_.bucket, object, status, total_bytes,
        run_start, run_end - run_start, std::move(chunks));

    /*
    bool used_directpath = context.peer().rfind("ipv6:[2001:") == 0;
    if (used_directpath ^ absl::GetFlag(FLAGS_directpath)) {
      std::cerr << "DirectPath flag is " << absl::GetFlag(FLAGS_directpath)
           << ", but using directpath is " << used_directpath << std::endl;
      std::cerr << "Peer was " << context.peer() << endl;
      return;
    }
    */

    if (keep_going == false) {
      return false;
    }
  }

  return true;
}

static std::vector<char> GetRandomData(size_t size) {
  std::vector<char> content(size);
  int* const s = reinterpret_cast<int*>(&(*content.begin()));
  int* const e = reinterpret_cast<int*>(&(*content.rbegin()));
  for (int* c = s; c < e; c += 1) {
    *c = rand();
  }
  return content;
}

bool Runner::RunWrite() {
  const int64_t max_chunk_size = 2097152;
  const std::vector<char> content = GetRandomData(max_chunk_size);

  for (int run = 0; run < parameter_.runs; run++) {
    auto storage = parameter_.storage_stub_provider->GetStorageStub();

    std::string object;
    if (parameter_.object_stop > 0) {
      std::cerr << "write doesn't support object_stop" << std::endl;
      return false;
    } else {
      char real_object[256];
      int cx = snprintf(real_object, sizeof(real_object), "%s_%d_%d",
                        parameter_.object.c_str(), parameter_.id, run);
      if (cx < 0) {
        std::cerr << "snprintf(object, index) failed" << std::endl;
        return false;
      }
      object = real_object;
    }

    absl::Time run_start = absl::Now();
    uint32_t object_crc32c = 0;

    std::string upload_id;
    if (parameter_.resumable_write) {
      grpc::ClientContext context;
      StartResumableWriteRequest start_request;
      auto resource =
          start_request.mutable_write_object_spec()->mutable_resource();
      resource->set_bucket(parameter_.bucket);
      resource->set_name(object);
      StartResumableWriteResponse start_response;
      auto status = storage.stub->StartResumableWrite(&context, start_request,
                                                      &start_response);
      if (!status.ok()) {
        std::cerr << "StartResumableWrite failed code=" << status.error_code()
                  << std::endl;
        return false;
      }
      upload_id = start_response.upload_id();
    }

    grpc::ClientContext context;
    WriteObjectResponse reply;
    std::unique_ptr<grpc::ClientWriter<WriteObjectRequest>> writer(
        storage.stub->WriteObject(&context, &reply));

    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);

    for (int64_t o = 0; o < parameter_.write_size; o += max_chunk_size) {
      bool first_request = o == 0;
      bool last_request = (o + max_chunk_size) >= parameter_.write_size;
      int64_t chunk_size = std::min(max_chunk_size, parameter_.write_size - o);

      WriteObjectRequest request;
      if (first_request) {
        if (parameter_.resumable_write) {
          request.set_upload_id(upload_id);
        } else {
          auto resource =
              request.mutable_write_object_spec()->mutable_resource();
          resource->set_bucket(parameter_.bucket);
          resource->set_name(object);
        }
      }

      request.mutable_checksummed_data()->set_content(&content[0], chunk_size);
      if (parameter_.check_crc32c) {
        uint32_t crc32c = crc32c_value((const uint8_t*)&content[0], chunk_size);
        request.mutable_checksummed_data()->set_crc32c(crc32c);

        object_crc32c = crc32c_extend(object_crc32c,
                                      (const uint8_t*)&content[0], chunk_size);
      }

      request.set_write_offset(o);
      if (last_request) {
        request.set_finish_write(true);
        if (parameter_.check_crc32c) {
          request.mutable_object_checksums()->set_crc32c(object_crc32c);
        }
      }

      if (!writer->Write(request)) break;

      RunnerWatcher::Chunk chunk = {absl::Now(), chunk_size};
      chunks.push_back(chunk);
      total_bytes += chunk_size;
    }
    writer->WritesDone();

    auto status = writer->Finish();
    absl::Time run_end = absl::Now();

    if (!status.ok()) {
      std::cerr << "Upload Failure!" << std::endl;
      std::cerr << "Peer: " << context.peer() << std::endl;
      std::cerr << "Status: " << std::endl;
      std::cerr << "- Code: " << status.error_code() << std::endl;
      std::cerr << "- Message: " << status.error_message() << std::endl;
      std::cerr << "- Details: " << status.error_details() << std::endl;
    }

    auto keep_going = parameter_.storage_stub_provider->ReportResult(
        storage.handle, status, context, run_end - run_start, total_bytes);

    watcher_->NotifyCompleted(
        OperationType::Write, parameter_.id, GetChannelId(storage.handle),
        context.peer(), parameter_.bucket, object, status, total_bytes,
        run_start, run_end - run_start, std::move(chunks));

    if (keep_going == false) {
      return false;
    }
  }

  return true;
}
