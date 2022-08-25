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

#ifndef GCS_BENCHMARK_CHANNEL_POLLER_H_
#define GCS_BENCHMARK_CHANNEL_POLLER_H_

#include <grpcpp/channel.h>

#include <memory>
#include <thread>

#include "absl/synchronization/mutex.h"

class ChannelPoller {
 public:
  ChannelPoller(std::shared_ptr<grpc::Channel> channel);
  ~ChannelPoller();

 private:
  void StartWatch();
  void ThreadRun();

 private:
  std::shared_ptr<grpc::Channel> channel_;
  grpc::CompletionQueue cq_;
  std::unique_ptr<std::thread> thread_;
  absl::Mutex mu_;
  bool shutdown_ = false;
};

#endif  // GCS_BENCHMARK_CHANNEL_POLLER_H_
