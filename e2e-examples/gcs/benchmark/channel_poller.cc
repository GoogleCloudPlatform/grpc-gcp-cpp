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

#include "channel_poller.h"

#include <chrono>

ChannelPoller::ChannelPoller(std::shared_ptr<grpc::Channel> channel)
    : channel_(channel) {
  StartWatch();
  thread_ = std::unique_ptr<std::thread>(
      new std::thread([this]() { this->ThreadRun(); }));
}

ChannelPoller::~ChannelPoller() {
  {
    std::shared_ptr<grpc::Channel> tmp_channel;
    {
      absl::MutexLock lock(&mu_);
      tmp_channel.swap(channel_);
    }
    // Give ChannelPoller a chance to handle remaining events
    // while shutting down the channel.
    tmp_channel.reset();
    {
      absl::MutexLock lock(&mu_);
      shutdown_ = true;
      cq_.Shutdown();
    }
  }
  thread_->join();
}

void ChannelPoller::StartWatch() {
  if (channel_ != nullptr) {
    grpc_connectivity_state last_observed = channel_->GetState(true);
    channel_->NotifyOnStateChange(last_observed,
                                  std::chrono::system_clock::time_point::max(),
                                  &cq_, nullptr);
  }
}

void ChannelPoller::ThreadRun() {
  // Keep calling Next in order to poll a channel.
  bool ok = false;
  void* tag = nullptr;
  while (cq_.Next(&tag, &ok)) {
    absl::MutexLock lock(&mu_);
    if (shutdown_) {
      break;
    }
    StartWatch();
  }
}
