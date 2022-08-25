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

#include "channel_manager.h"

#include <cassert>

ChannelHandle::ChannelHandle(ChannelManager* manager,
                             std::shared_ptr<grpc::Channel> channel)
    : manager_(manager),
      channel_(std::move(channel)),
      on_rpc_done_called_(false) {}

ChannelHandle::~ChannelHandle() {
  manager_->OnChannelHandleDestroyed(channel_.get());
}

void ChannelHandle::OnRpcDone(grpc::Status status) {
  assert(!on_rpc_done_called_);
  manager_->OnRpcDone(channel_.get(), status);
  on_rpc_done_called_ = true;
}

ChannelManager::ChannelManager(
    size_t max_channel_count,
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator)
    : max_channel_count_(max_channel_count),
      channel_handle_count_(0),
      channel_creator_(channel_creator) {}

ChannelManager::~ChannelManager() {
  absl::MutexLock l(&lock_);
  assert(channel_handle_count_ == 0);
}

ChannelHandle ChannelManager::GetHandle() {
  absl::MutexLock l(&lock_);

  // Finds the channel with the least in-use count.
  ChannelState* candidate = nullptr;
  size_t min_in_use_count = SIZE_MAX;
  for (auto& cs : channel_states_) {
    if (cs.in_use_count < min_in_use_count) {
      candidate = &cs;
      min_in_use_count = cs.in_use_count;
    }
  }

  // Should we need to create a new channel? It would create
  // a new one when there is no idle channel and a pool is not full yet.
  // This lazy channel creation is helpful to avoid underutilized channels.
  if (candidate == nullptr || (candidate->in_use_count > 0 &&
                               channel_states_.size() < max_channel_count_)) {
    channel_states_.push_back(ChannelState{channel_creator_(), 0});
    candidate = &channel_states_.back();
  }

  candidate->in_use_count += 1;
  channel_handle_count_ += 1;
  return ChannelHandle(this, candidate->channel);
}

void ChannelManager::OnRpcDone(grpc::Channel* channel, grpc::Status status) {
  absl::MutexLock l(&lock_);
  for (auto& cs : channel_states_) {
    if (cs.channel.get() == channel) {
      cs.in_use_count -= 1;
      if (status.error_code() == grpc::StatusCode::CANCELLED ||
          status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
        // Replace the unhealthy channel with the newly created one.
        cs.channel == channel_creator_();
        cs.in_use_count = 0;
      }
      break;
    }
  }
}

void ChannelManager::OnChannelHandleDestroyed(grpc::Channel* channel) {
  absl::MutexLock l(&lock_);
  channel_handle_count_ -= 1;
}
