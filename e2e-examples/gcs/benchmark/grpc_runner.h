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
#include "work_queue.h"

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
  std::shared_ptr<WorkQueue> work_queue_;
  std::shared_ptr<RunnerWatcher> watcher_;
};

#endif  // GCS_BENCHMARK_GRPC_RUNNER_H_
