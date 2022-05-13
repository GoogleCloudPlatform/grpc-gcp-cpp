#include "gcscpp_runner.h"

#include <string>
#include <thread>

#include "absl/time/time.h"
#include "google/cloud/storage/client.h"
#include "google/cloud/storage/grpc_plugin.h"

GcscppRunner::GcscppRunner(Parameters parameters,
                           std::shared_ptr<RunnerWatcher> watcher)
    : parameters_(parameters),
      object_resolver_(parameters_.object, parameters_.object_format,
                       parameters_.object_start, parameters_.object_stop),
      watcher_(watcher) {}

bool GcscppRunner::Run() {
  auto client = ::google::cloud::storage::Client();
  if (parameters_.client == "gcscpp-grpc") {
    client = ::google::cloud::storage_experimental::DefaultGrpcClient();
  }

  // Spawns benchmark threads and waits until they're done.
  std::vector<std::thread> threads;
  std::vector<bool> returns(parameters_.threads);
  for (int i = 1; i <= parameters_.threads; i++) {
    int thread_id = i;
    threads.emplace_back([thread_id, client, &returns, this]() {
      returns[thread_id - 1] = this->DoOperation(thread_id, client);
    });
  }
  std::for_each(threads.begin(), threads.end(),
                [](std::thread& t) { t.join(); });
  return std::all_of(returns.begin(), returns.end(), [](bool v) { return v; });
}

bool GcscppRunner::DoOperation(int thread_id, google::cloud::storage::Client storage_client) {
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

bool GcscppRunner::DoRead(int thread_id, google::cloud::storage::Client storage_client) {
  for (int run = 0; run < parameters_.runs; run++) {
    std::string object = object_resolver_.Resolve(thread_id, run);

    absl::Time run_start = absl::Now();
    auto reader = storage_client.ReadObject(parameters_.bucket, object);
    if (!reader) {
      std::cerr << "Error reading object: " << reader.status() << "\n";
      return false;
    }

    std::string contents{std::istreambuf_iterator<char>{reader}, {}};
    int64_t total_bytes = contents.size();
    absl::Time run_end = absl::Now();

    watcher_->NotifyCompleted(OperationType::Read, thread_id, 0, "",
                              parameters_.bucket, object, grpc::Status::OK,
                              total_bytes, run_start, run_end - run_start, {});
  }

  return true;
}

bool GcscppRunner::DoRandomRead(int thread_id, google::cloud::storage::Client storage_client) { return true; }

bool GcscppRunner::DoWrite(int thread_id, google::cloud::storage::Client storage_client) { return true; }
