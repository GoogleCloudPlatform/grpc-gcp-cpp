// Copyright 2022 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "grpc_runner.h"

#include <grpc/status.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <stdlib.h>

#include <functional>
#include <thread>

#include "absl/random/random.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/string_view.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "channel_creator.h"
#include "channel_policy.h"
#include "e2e-examples/gcs/crc32c/crc32c.h"
#include "e2e-examples/gcs/benchmark/random_data.h"
#include "google/storage/v2/storage.grpc.pb.h"

using ::google::storage::v2::Object;
using ::google::storage::v2::ReadObjectRequest;
using ::google::storage::v2::ReadObjectResponse;
using ::google::storage::v2::StartResumableWriteRequest;
using ::google::storage::v2::StartResumableWriteResponse;
using ::google::storage::v2::WriteObjectRequest;
using ::google::storage::v2::WriteObjectResponse;

extern int run_ctest(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size);
extern int run_mtest(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator);

static std::shared_ptr<grpc::Channel> CreateBenchmarkGrpcChannel(
    const Parameters& parameters) {
  return CreateGrpcChannel(parameters.host, parameters.access_token,
                           parameters.network, parameters.rr, parameters.td,
                           parameters.tx_zerocopy);
}

static int32_t GetChannelId(void* stub_handle) {
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

static std::string ToV2BucketName(absl::string_view bucket_name) {
  static const absl::string_view V2_BUCKET_NAME_PREFIX = "projects/_/buckets/";
  return absl::StrCat(V2_BUCKET_NAME_PREFIX, bucket_name);
}

static void ApplyCallTimeout(grpc::ClientContext* context,
                             absl::Duration timeout) {
  if (timeout != absl::InfiniteDuration()) {
    context->set_deadline(
        std::chrono::system_clock::now() +
        std::chrono::milliseconds(absl::ToInt64Milliseconds(timeout)));
  }
}

GrpcRunner::GrpcRunner(Parameters parameters,
                       std::shared_ptr<RunnerWatcher> watcher)
    : parameters_(parameters),
      object_resolver_(parameters_.object, parameters_.object_format,
                       parameters_.object_start, parameters_.object_stop),
      watcher_(watcher) {}

bool GrpcRunner::Run() {
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator = [&]() {
    return CreateBenchmarkGrpcChannel(parameters_);
  };
  if (parameters_.ctest > 0) {
    return run_ctest(channel_creator, parameters_.ctest);
  }
  if (parameters_.mtest > 0) {
    return run_mtest(channel_creator);
  }

  // Initializes a gRPC channel pool.
  std::shared_ptr<StorageStubProvider> stub_pool;
  if (parameters_.cpolicy == "const") {
    stub_pool = CreateConstChannelPool(channel_creator);
  } else if (parameters_.cpolicy == "pool") {
    if (parameters_.carg <= 0) {
      std::cerr << "Invalid carg: " << parameters_.carg << std::endl;
      return false;
    }
    stub_pool = CreateRoundRobinChannelPool(channel_creator, parameters_.carg);
  } else if (parameters_.cpolicy == "bpool") {
    if (parameters_.carg <= 0) {
      std::cerr << "Invalid carg: " << parameters_.carg << std::endl;
      return false;
    }
    stub_pool =
        CreateRoundRobinPlusChannelPool(channel_creator, parameters_.carg);
  } else if (parameters_.cpolicy == "spool") {
    if (parameters_.carg <= 0) {
      std::cerr << "Invalid carg: " << parameters_.carg << std::endl;
      return false;
    }
    stub_pool =
        CreateSmartRoundRobinChannelPool(channel_creator, parameters_.carg);
  }

  // Spawns benchmark threads and waits until they're done.
  std::vector<std::thread> threads;
  std::vector<bool> returns(parameters_.threads);
  work_queue_.reset(new WorkQueue(parameters_.threads, parameters_.runs,
                                  parameters_.steal_work));
  for (int i = 1; i <= parameters_.threads; i++) {
    int thread_id = i;
    std::shared_ptr<StorageStubProvider> storage_stub_provider;
    if (stub_pool != nullptr) {
      storage_stub_provider = stub_pool;
    } else if (parameters_.cpolicy == "perthread") {
      storage_stub_provider = CreateConstChannelPool(channel_creator);
    } else if (parameters_.cpolicy == "percall") {
      storage_stub_provider =
          CreateCreateNewChannelStubProvider(channel_creator);
    }
    threads.emplace_back([thread_id, storage_stub_provider, &returns, this]() {
      bool r = this->DoOperation(thread_id, storage_stub_provider);
      if (!r && !parameters_.wait_threads) {
        std::cerr << "Thread id=" << thread_id << " stopped." << std::endl;
        exit(1);
      }
      returns[thread_id - 1] = r;
    });
  }
  std::for_each(threads.begin(), threads.end(),
                [](std::thread& t) { t.join(); });
  return std::all_of(returns.begin(), returns.end(), [](bool v) { return v; });
}

bool GrpcRunner::DoOperation(
    int thread_id, std::shared_ptr<StorageStubProvider> storage_stub_provider) {
  switch (parameters_.operation_type) {
    case OperationType::Read:
      return DoRead(thread_id, storage_stub_provider);
    case OperationType::RandomRead:
      return DoRandomRead(thread_id, storage_stub_provider);
    case OperationType::Write:
      return DoWrite(thread_id, storage_stub_provider);
    default:
      return false;
  }
}

bool GrpcRunner::DoRead(
    int thread_id, std::shared_ptr<StorageStubProvider> storage_stub_provider) {
  std::tuple<int, int> work_queue;
  while (true) {
    auto work = work_queue_->pop(thread_id);
    auto work_tid = std::get<0>(work);
    auto work_run = std::get<1>(work);
    if (work_run == 0) {
      break;
    }
    while (true) {
      auto storage = storage_stub_provider->GetStorageStub();

      std::string object = object_resolver_.Resolve(work_tid, work_run);
      ReadObjectRequest request;
      request.set_bucket(ToV2BucketName(parameters_.bucket));
      request.set_object(object);
      if (parameters_.read_offset >= 0) {
        request.set_read_offset(parameters_.read_offset);
      }
      if (parameters_.read_limit >= 0) {
        request.set_read_limit(parameters_.read_limit);
      }

      absl::Time run_start = absl::Now();
      grpc::ClientContext context;
      ApplyCallTimeout(&context, parameters_.timeout);
      std::unique_ptr<grpc::ClientReader<ReadObjectResponse>> reader =
          storage.stub->ReadObject(&context, request);

      int64_t total_bytes = 0;
      std::vector<RunnerWatcher::Chunk> chunks;
      chunks.reserve(256);

      ReadObjectResponse response;
      while (reader->Read(&response)) {
        const auto& content = response.checksummed_data().content();
        int64_t content_size = content.size();

        if (parameters_.crc32c) {
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
        std::cerr << "Peer:   " << context.peer() << std::endl;
        std::cerr << "Start:  " << run_start << std::endl;
        std::cerr << "End:    " << run_end << std::endl;
        std::cerr << "Elased: " << (run_end - run_start) << std::endl;
        std::cerr << "Bucket: " << parameters_.bucket.c_str() << std::endl;
        std::cerr << "Object: " << object.c_str() << std::endl;
        std::cerr << "Bytes:  " << total_bytes << std::endl;
        std::cerr << "Status: " << std::endl;
        std::cerr << "- Code:    " << status.error_code() << std::endl;
        std::cerr << "- Message: " << status.error_message() << std::endl;
        std::cerr << "- Details: " << status.error_details() << std::endl;
      }

      storage_stub_provider->ReportResult(storage.handle, status, context,
                                          run_end - run_start, total_bytes);

      watcher_->NotifyCompleted(
          OperationType::Read, work_tid, GetChannelId(storage.handle),
          context.peer(), parameters_.bucket, object, status, total_bytes,
          run_start, run_end - run_start, std::move(chunks));

      if (status.ok()) {
        break;
      } else if (parameters_.trying) {
        // let's try the same if keep_trying is set and it failed
        continue;
      } else {
        return false;
      }
    }
  }

  return true;
}

bool GrpcRunner::DoRandomRead(
    int thread_id, std::shared_ptr<StorageStubProvider> storage_stub_provider) {
  if (parameters_.read_limit <= 0) {
    std::cerr << "read_limit should be greater than 0." << std::endl;
    return false;
  }
  int64_t read_span =
      parameters_.read_limit - std::max(int64_t(0), parameters_.read_offset);
  if (read_span <= 0) {
    std::cerr << "read_limit should be greater than read_offset." << std::endl;
    return false;
  }
  if (parameters_.chunk_size == 0) {
    std::cerr << "chunk_size should be greater than 0." << std::endl;
    return false;
  }
  int64_t chunks = read_span / parameters_.chunk_size;
  if (chunks <= 0) {
    std::cerr
        << "read_limit should be greater than or equal to readable window."
        << std::endl;
    return false;
  }

  auto storage = storage_stub_provider->GetStorageStub();
  std::string object = object_resolver_.Resolve(thread_id, 0);
  absl::BitGen gen;
  for (int run = 0; run < parameters_.runs; run++) {
    int64_t offset = absl::Uniform(gen, 0, chunks) * parameters_.chunk_size;
    ReadObjectRequest request;
    request.set_bucket(ToV2BucketName(parameters_.bucket));
    request.set_object(object);
    request.set_read_offset(offset);
    request.set_read_limit(parameters_.chunk_size);

    absl::Time run_start = absl::Now();
    grpc::ClientContext context;
    ApplyCallTimeout(&context, parameters_.timeout);
    std::unique_ptr<grpc::ClientReader<ReadObjectResponse>> reader =
        storage.stub->ReadObject(&context, request);

    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);

    ReadObjectResponse response;
    while (reader->Read(&response)) {
      const auto& content = response.checksummed_data().content();
      int64_t content_size = content.size();

      if (parameters_.crc32c) {
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
      std::cerr << "Peer:   " << context.peer() << std::endl;
      std::cerr << "Start:  " << run_start << std::endl;
      std::cerr << "End:    " << run_end << std::endl;
      std::cerr << "Elased: " << (run_end - run_start) << std::endl;
      std::cerr << "Bucket: " << parameters_.bucket.c_str() << std::endl;
      std::cerr << "Object: " << object.c_str() << std::endl;
      std::cerr << "Bytes:  " << total_bytes << std::endl;
      std::cerr << "Status: " << std::endl;
      std::cerr << "- Code:    " << status.error_code() << std::endl;
      std::cerr << "- Message: " << status.error_message() << std::endl;
      std::cerr << "- Details: " << status.error_details() << std::endl;
    }

    storage_stub_provider->ReportResult(storage.handle, status, context,
                                        run_end - run_start, total_bytes);

    watcher_->NotifyCompleted(
        OperationType::Read, thread_id, GetChannelId(storage.handle),
        context.peer(), parameters_.bucket, object, status, total_bytes,
        run_start, run_end - run_start, std::move(chunks));

    if (status.ok()) {
      ;
    } else if (parameters_.trying) {
      // let's try the same if keep_trying is set and it failed
      run -= 1;
    } else {
      return false;
    }
  }

  return true;
}

bool GrpcRunner::DoWrite(
    int thread_id, std::shared_ptr<StorageStubProvider> storage_stub_provider) {
  const int64_t max_chunk_size =
      (parameters_.chunk_size < 0) ? 2097152 : parameters_.chunk_size;

  if (parameters_.object_stop > 0) {
    std::cerr << "write doesn't support object_stop" << std::endl;
    return false;
  }
  if (parameters_.write_size <= 0) {
    std::cerr << "write_size should be greater than 0." << std::endl;
    return false;
  }

  std::tuple<int, int> work_queue;
  while (true) {
    auto work = work_queue_->pop(thread_id);
    auto work_tid = std::get<0>(work);
    auto work_run = std::get<1>(work);
    if (work_run == 0) {
      break;
    }
    while (true) {
      auto storage = storage_stub_provider->GetStorageStub();

      std::string object = object_resolver_.Resolve(work_tid, work_run);
      absl::Time run_start = absl::Now();
      uint32_t object_crc32c = 0;

      std::string upload_id;
      if (parameters_.resumable) {
        grpc::ClientContext context;
        ApplyCallTimeout(&context, parameters_.timeout);
        StartResumableWriteRequest start_request;
        auto resource =
            start_request.mutable_write_object_spec()->mutable_resource();
        resource->set_bucket(ToV2BucketName(parameters_.bucket));
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
      ApplyCallTimeout(&context, parameters_.timeout);
      WriteObjectResponse reply;
      std::unique_ptr<grpc::ClientWriter<WriteObjectRequest>> writer(
          storage.stub->WriteObject(&context, &reply));

      int64_t total_bytes = 0;
      std::vector<RunnerWatcher::Chunk> chunks;
      chunks.reserve(256);

      for (int64_t o = 0; o < parameters_.write_size; o += max_chunk_size) {
        bool first_request = o == 0;
        bool last_request = (o + max_chunk_size) >= parameters_.write_size;
        int64_t chunk_size =
            std::min(max_chunk_size, parameters_.write_size - o);

        WriteObjectRequest request;
        if (first_request) {
          if (parameters_.resumable) {
            request.set_upload_id(upload_id);
          } else {
            auto resource =
                request.mutable_write_object_spec()->mutable_resource();
            resource->set_bucket(ToV2BucketName(parameters_.bucket));
            resource->set_name(object);
          }
        }

        absl::Cord content = GetRandomData(chunk_size);
        absl::CopyCordToString(content, request.mutable_checksummed_data()->mutable_content());

        if (parameters_.crc32c) {
          const char* buf = request.mutable_checksummed_data()->content().c_str();
          uint32_t crc32c =
              crc32c_value((const uint8_t*)buf, chunk_size);
          request.mutable_checksummed_data()->set_crc32c(crc32c);

          object_crc32c = crc32c_extend(
              object_crc32c, (const uint8_t*)buf, chunk_size);
        }

        request.set_write_offset(o);
        if (last_request) {
          request.set_finish_write(true);
          if (parameters_.crc32c) {
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
        std::cerr << "Peer:   " << context.peer() << std::endl;
        std::cerr << "Start:  " << run_start << std::endl;
        std::cerr << "End:    " << run_end << std::endl;
        std::cerr << "Elased: " << (run_end - run_start) << std::endl;
        std::cerr << "Bucket: " << parameters_.bucket.c_str() << std::endl;
        std::cerr << "Object: " << object.c_str() << std::endl;
        std::cerr << "Bytes:  " << total_bytes << std::endl;
        std::cerr << "Status: " << std::endl;
        std::cerr << "- Code:    " << status.error_code() << std::endl;
        std::cerr << "- Message: " << status.error_message() << std::endl;
        std::cerr << "- Details: " << status.error_details() << std::endl;
      }

      storage_stub_provider->ReportResult(storage.handle, status, context,
                                          run_end - run_start, total_bytes);

      watcher_->NotifyCompleted(
          OperationType::Write, work_tid, GetChannelId(storage.handle),
          context.peer(), parameters_.bucket, object, status, total_bytes,
          run_start, run_end - run_start, std::move(chunks));

      if (status.ok()) {
        break;
      } else if (parameters_.trying) {
        // let's try the same if keep_trying is set and it failed
        continue;
      } else {
        return false;
      }
    }
  }

  return true;
}