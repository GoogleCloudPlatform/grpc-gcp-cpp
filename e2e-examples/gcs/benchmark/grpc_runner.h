#ifndef GCS_BENCHMARK_GRPC_RUNNER_H_
#define GCS_BENCHMARK_GRPC_RUNNER_H_

#include <grpcpp/channel.h>

#include <functional>
#include <memory>

#include "channel_policy.h"
#include "object_resolver.h"
#include "parameters.h"
#include "runner.h"
#include "runner_watcher.h"

class GrpcRunner : public Runner {
 public:
  GrpcRunner(Parameters parameters, std::shared_ptr<RunnerWatcher> watcher);
  virtual bool Run() override;

 private:
  bool DoOperation(int thread_id,
                   std::shared_ptr<StorageStubProvider> storage_stub_provider);
  bool DoRead(int thread_id,
              std::shared_ptr<StorageStubProvider> storage_stub_provider);
  bool DoRandomRead(int thread_id,
                    std::shared_ptr<StorageStubProvider> storage_stub_provider);
  bool DoWrite(int thread_id,
               std::shared_ptr<StorageStubProvider> storage_stub_provider);

 private:
  Parameters parameters_;
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
  ObjectResolver object_resolver_;
  std::shared_ptr<RunnerWatcher> watcher_;
};

#endif  // GCS_BENCHMARK_GRPC_RUNNER_H_
