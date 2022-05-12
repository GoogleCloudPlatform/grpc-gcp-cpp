#ifndef GCS_BENCHMARK_GCSCPP_RUNNER_H_
#define GCS_BENCHMARK_GCSCPP_RUNNER_H_

#include "object_resolver.h"
#include "parameters.h"
#include "runner.h"
#include "runner_watcher.h"

#include <functional>
#include <memory>

class GcscppRunner : public Runner {
 public:
  GcscppRunner(Parameters parameters, std::shared_ptr<RunnerWatcher> watcher);
  virtual bool Run() override;

 private:
  bool DoOperation(int thread_id);
  bool DoRead(int thread_id);
  bool DoRandomRead(int thread_id);
  bool DoWrite(int thread_id);

 private:
  Parameters parameters_;
  ObjectResolver object_resolver_;
  std::shared_ptr<RunnerWatcher> watcher_;
};

#endif  // GCS_BENCHMARK_GCSCPP_RUNNER_H_
