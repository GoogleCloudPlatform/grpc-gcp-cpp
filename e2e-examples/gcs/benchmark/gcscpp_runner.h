#ifndef GCS_BENCHMARK_GCSCPP_RUNNER_H_
#define GCS_BENCHMARK_GCSCPP_RUNNER_H_

#include "object_resolver.h"
#include "parameters.h"
#include "runner.h"
#include "runner_watcher.h"

#include "google/cloud/storage/client.h"

#include <functional>
#include <memory>

class GcscppRunner : public Runner {
 public:
  GcscppRunner(Parameters parameters, std::shared_ptr<RunnerWatcher> watcher);
  virtual bool Run() override;

 private:
  bool DoOperation(int thread_id, google::cloud::storage::Client storage_client);
  bool DoRead(int thread_id, google::cloud::storage::Client storage_client);
  bool DoRandomRead(int thread_id, google::cloud::storage::Client storage_client);
  bool DoWrite(int thread_id, google::cloud::storage::Client storage_client);

 private:
  Parameters parameters_;
  ObjectResolver object_resolver_;
  std::shared_ptr<RunnerWatcher> watcher_;
};

#endif  // GCS_BENCHMARK_GCSCPP_RUNNER_H_
