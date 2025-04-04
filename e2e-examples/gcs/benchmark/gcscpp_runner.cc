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

#include "gcscpp_runner.h"

#include <string>
#include <thread>

#include "absl/random/random.h"
#include "absl/strings/cord.h"
#include "absl/time/time.h"
#include "e2e-examples/gcs/benchmark/random_data.h"
#include "google/cloud/grpc_options.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/grpc_plugin.h"

GcscppRunner::GcscppRunner(Parameters parameters,
                           std::shared_ptr<RunnerWatcher> watcher)
    : parameters_(parameters),
      object_resolver_(parameters_.object, parameters_.object_format,
                       parameters_.object_start, parameters_.object_stop),
      watcher_(watcher) {}

static google::cloud::storage::Client CreateClient(
    const Parameters& parameters) {
  auto opts = google::cloud::Options{};
  if (parameters.write_size > 0) {
    // Make a upload buffer big enough to make it done in a single rpc call if
    // chunk_size is not specified explicitly.
    std::size_t upload_buffer_size =
        (std::size_t)((parameters.chunk_size < 0) ? parameters.write_size
                                                  : parameters.chunk_size);
    opts.set<google::cloud::storage::UploadBufferSizeOption>(
        upload_buffer_size);
  }
  if (parameters.client == "gcscpp-grpc") {
    std::string target = parameters.host;
    if (target.empty()) {
      target = "storage.googleapis.com";
    }
    if (parameters.td) {
      target = "google-c2p:///" + target;
    }
    if (parameters.carg != 0) {
      opts.set<google::cloud::GrpcNumChannelsOption>(parameters.carg);
    }
    if (parameters.tx_zerocopy) {
      grpc::ChannelArguments channel_arguments;
      channel_arguments.SetInt(GRPC_ARG_TCP_TX_ZEROCOPY_ENABLED, 1);
      opts.set<google::cloud::GrpcChannelArgumentsNativeOption>(
          channel_arguments);
    }
    return ::google::cloud::storage::MakeGrpcClient(
        opts.set<google::cloud::EndpointOption>(target));
  } else {
    if (!parameters.host.empty()) {
      opts.set<google::cloud::storage::RestEndpointOption>(parameters.host);
    }
    if (!parameters.target_api_version.empty()) {
      opts.set<google::cloud::storage::internal::TargetApiVersionOption>(
          parameters.target_api_version);
    }
    return ::google::cloud::storage::Client(std::move(opts));
  }
}

bool GcscppRunner::Run() {
  auto client = CreateClient(parameters_);

  // Spawns benchmark threads and waits until they're done.
  std::vector<std::thread> threads;
  std::vector<bool> returns(parameters_.threads);
  for (int i = 1; i <= parameters_.threads; i++) {
    int thread_id = i;
    threads.emplace_back([thread_id, client, &returns, this]() {
      bool r = this->DoOperation(thread_id, client);
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

bool GcscppRunner::DoOperation(int thread_id,
                               google::cloud::storage::Client storage_client) {
  switch (parameters_.operation_type) {
    case OperationType::Read:
      return DoRead(thread_id, storage_client);
    case OperationType::RandomRead:
      return DoRandomRead(thread_id, storage_client);
    case OperationType::Write:
      return DoWrite(thread_id, storage_client);
    default:
      return false;
  }
}

std::string ExtractPeer(
    std::multimap<std::string, std::string> const& headers) {
  auto p = headers.find(":grpc-context-peer");
  if (p == headers.end()) {
    p = headers.find(":curl-peer");
  }
  return p == headers.end() ? "" : p->second;
}

bool GcscppRunner::DoRead(int thread_id,
                          google::cloud::storage::Client storage_client) {
  std::vector<char> buffer(4 * 1024 * 1024);
  auto const buffer_size = static_cast<std::streamsize>(buffer.size());
  for (int run = 0; run < parameters_.runs; run++) {
    std::string object = object_resolver_.Resolve(thread_id, run);

    absl::Time run_start = absl::Now();
    auto reader = storage_client.ReadObject(parameters_.bucket, object);
    if (!reader) {
      std::cerr << "Error reading object: " << reader.status() << "\n";
      return false;
    }
    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);
    while (!reader.eof()) {
      reader.read(buffer.data(), buffer_size);
      int64_t content_size = reader.gcount();
      RunnerWatcher::Chunk chunk = {absl::Now(), content_size};
      chunks.push_back(chunk);
      total_bytes += content_size;
    }
    reader.Close();
    absl::Time run_end = absl::Now();

    watcher_->NotifyCompleted(OperationType::Read, thread_id, 0,
                              ExtractPeer(reader.headers()), parameters_.bucket,
                              object, grpc::Status::OK, total_bytes, run_start,
                              run_end - run_start, chunks);
  }
  return true;
}

bool GcscppRunner::DoRandomRead(int thread_id,
                                google::cloud::storage::Client storage_client) {
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

  std::string object = object_resolver_.Resolve(thread_id, 0);
  absl::BitGen gen;
  std::vector<char> buffer(4 * 1024 * 1024);
  for (int run = 0; run < parameters_.runs; run++) {
    int64_t offset = absl::Uniform(gen, 0, chunks) * parameters_.chunk_size;
    absl::Time run_start = absl::Now();
    auto reader =
        storage_client.ReadObject(parameters_.bucket, object,
                                  google::cloud::storage::ReadRange(
                                      offset, offset + parameters_.chunk_size));
    if (!reader) {
      std::cerr << "Error reading object: " << reader.status() << "\n";
      return false;
    }
    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);
    while (total_bytes < parameters_.chunk_size) {
      reader.read(buffer.data(),
                  std::min(buffer.size(), (size_t)parameters_.chunk_size));
      int64_t content_size = reader.gcount();
      RunnerWatcher::Chunk chunk = {absl::Now(), content_size};
      chunks.push_back(chunk);
      total_bytes += content_size;
    }
    reader.Close();
    absl::Time run_end = absl::Now();

    watcher_->NotifyCompleted(OperationType::Read, thread_id, 0,
                              ExtractPeer(reader.headers()), parameters_.bucket,
                              object, grpc::Status::OK, total_bytes, run_start,
                              run_end - run_start, chunks);
  }

  return true;
}

bool GcscppRunner::DoWrite(int thread_id,
                           google::cloud::storage::Client storage_client) {
  const int64_t max_chunk_size = (parameters_.chunk_size < 0)
                                     ? parameters_.write_size
                                     : parameters_.chunk_size;
  absl::Cord content = GetRandomData(max_chunk_size);
  absl::string_view content_data = content.Flatten();

  if (parameters_.object_stop > 0) {
    std::cerr << "write doesn't support object_stop" << std::endl;
    return false;
  }
  if (parameters_.write_size <= 0) {
    std::cerr << "write_size should be greater than 0." << std::endl;
    return false;
  }

  for (int run = 0; run < parameters_.runs; run++) {
    std::string object = object_resolver_.Resolve(thread_id, run);
    absl::Time run_start = absl::Now();

    int64_t total_bytes = 0;
    std::vector<RunnerWatcher::Chunk> chunks;
    chunks.reserve(256);

    auto writer = storage_client.WriteObject(parameters_.bucket, object);
    if (!writer) {
      std::cerr << "Error writing object: " << writer.last_status() << "\n";
      return false;
    }

    for (int64_t o = 0; o < parameters_.write_size; o += max_chunk_size) {
      int64_t chunk_size = std::min(max_chunk_size, parameters_.write_size - o);
      writer.write(content_data.data(), chunk_size);

      RunnerWatcher::Chunk chunk = {absl::Now(), chunk_size};
      chunks.push_back(chunk);
      total_bytes += chunk_size;
    }
    writer.Close();
    absl::Time run_end = absl::Now();

    watcher_->NotifyCompleted(OperationType::Write, thread_id, 0,
                              ExtractPeer(writer.headers()), parameters_.bucket,
                              object, grpc::Status::OK, total_bytes, run_start,
                              run_end - run_start, std::move(chunks));
  }

  return true;
}
