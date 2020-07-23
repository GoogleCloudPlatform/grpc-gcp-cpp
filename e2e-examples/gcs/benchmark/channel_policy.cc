#include "channel_policy.h"

#include <unordered_map>

#include "absl/synchronization/mutex.h"

class ConstChannelPool : public StorageStubProvider {
 public:
  ConstChannelPool(
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator)
      : channel_creator_(channel_creator) {
    channel_ = channel_creator();
  }

  StorageStubProvider::StubHolder GetStorageStub() override {
    StorageStubProvider::StubHolder holder = {
        google::storage::v1::Storage::NewStub(channel_), (void*)channel_.get()};
    return holder;
  }

  bool ReportResult(void* handle, const grpc::Status& status,
                    const grpc::ClientContext& context,
                    absl::Duration elapsed_time, int64_t bytes) override {
    if (status.ok()) {
      return true;
    } else if (status.error_code() == grpc::StatusCode::CANCELLED) {
      channel_ = channel_creator_();
      return true;
    }
    return false;
  }

 private:
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
  std::shared_ptr<grpc::Channel> channel_;
};

std::shared_ptr<StorageStubProvider> CreateConstChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator) {
  return std::make_shared<ConstChannelPool>(channel_creator);
}

class CreateNewChannelStubProvider : public StorageStubProvider {
 public:
  CreateNewChannelStubProvider(
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator)
      : channel_creator_(channel_creator) {}

  StorageStubProvider::StubHolder GetStorageStub() override {
    auto channel = channel_creator_();
    StorageStubProvider::StubHolder holder = {
        google::storage::v1::Storage::NewStub(channel), (void*)channel.get()};
    return holder;
  }

  bool ReportResult(void* handle, const grpc::Status& status,
                    const grpc::ClientContext& context,
                    absl::Duration elapsed_time, int64_t bytes) override {
    return (status.ok() || status.error_code() == grpc::StatusCode::CANCELLED);
  }

 private:
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
};

std::shared_ptr<StorageStubProvider> CreateCreateNewChannelStubProvider(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator) {
  return std::make_shared<CreateNewChannelStubProvider>(channel_creator);
}

class RoundRobinChannelPool : public StorageStubProvider {
 public:
  RoundRobinChannelPool(
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
      int size) {
    channel_creator_ = channel_creator;
    for (int i = 0; i < size; i++) {
      channels_.push_back(channel_creator());
    }
  }

  StorageStubProvider::StubHolder GetStorageStub() override {
    absl::MutexLock l(&lock_);
    cursor_ = (cursor_ + 1) % channels_.size();
    StorageStubProvider::StubHolder holder = {
        google::storage::v1::Storage::NewStub(channels_[cursor_]),
        (void*)channels_[cursor_].get()};
    return holder;
  }

  bool ReportResult(void* handle, const grpc::Status& status,
                    const grpc::ClientContext& context,
                    absl::Duration elapsed_time, int64_t bytes) override {
    if (status.ok()) {
      return true;
    } else if (status.error_code() == grpc::StatusCode::CANCELLED ||
               status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
      auto i = std::find_if(channels_.begin(), channels_.end(),
                            [handle](std::shared_ptr<grpc::Channel> val) {
                              return (void*)val.get() == handle;
                            });
      if (i != channels_.end()) {
        std::cout << "Evict the channel (peer=" << context.peer()
                  << ") due to error:" << status.error_code() << std::endl;
        *i = channel_creator_();
        return true;
      }
    }
    return false;
  }

 private:
  absl::Mutex lock_;
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
  std::vector<std::shared_ptr<grpc::Channel>> channels_;
  int cursor_ = 0;
};

std::shared_ptr<StorageStubProvider> CreateRoundRobinChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size) {
  return std::make_shared<RoundRobinChannelPool>(channel_creator, size);
}

class RoundRobinPlusChannelPool : public StorageStubProvider {
 public:
  RoundRobinPlusChannelPool(
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
      int size) {
    channel_creator_ = channel_creator;
    for (int i = 0; i < size; i++) {
      channel_states_.push_back(ChannelState{channel_creator(), 0});
    }
  }

  StorageStubProvider::StubHolder GetStorageStub() override {
    absl::MutexLock l(&lock_);

    // Finds the channel with the least number of use.
    auto least = channel_states_.begin();
    for (auto i = channel_states_.begin(); i != channel_states_.end(); ++i) {
      if (i->in_use_count < least->in_use_count) {
        least = i;
      }
    }

    // Increases in-use count for the channel to be returned.
    least->in_use_count += 1;
    StorageStubProvider::StubHolder holder = {
        google::storage::v1::Storage::NewStub(least->channel),
        (void*)least->channel.get()};
    return holder;
  }

  bool ReportResult(void* handle, const grpc::Status& status,
                    const grpc::ClientContext& context,
                    absl::Duration elapsed_time, int64_t bytes) override {
    absl::MutexLock l(&lock_);

    // Decreases in-use count for the channel
    auto i = std::find_if(channel_states_.begin(), channel_states_.end(),
                          [handle](const ChannelState& val) {
                            return (void*)val.channel.get() == handle;
                          });
    if (i != channel_states_.end()) {
      i->in_use_count -= 1;
    }

    // If the error indicates that the channel is hopeless,
    // replace it with the newly created one.
    if (status.ok()) {
      return true;
    } else if (status.error_code() == grpc::StatusCode::CANCELLED ||
               status.error_code() == grpc::StatusCode::DEADLINE_EXCEEDED) {
      auto i = std::find_if(channel_states_.begin(), channel_states_.end(),
                            [handle](const ChannelState& val) {
                              return (void*)val.channel.get() == handle;
                            });
      if (i != channel_states_.end()) {
        std::cerr << "Evict the channel (peer=" << context.peer()
                  << ") due to error:" << status.error_code() << std::endl;
        i->channel = channel_creator_();
        i->in_use_count = 0;
      }
      return true;
    } else {
      return false;
    }
  }

 private:
  absl::Mutex lock_;
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;

  struct ChannelState {
    std::shared_ptr<grpc::Channel> channel;
    int32_t in_use_count;
  };
  std::vector<ChannelState> channel_states_;
};

std::shared_ptr<StorageStubProvider> CreateRoundRobinPlusChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size) {
  return std::make_shared<RoundRobinPlusChannelPool>(channel_creator, size);
}

class SmartRoundRobinChannelPool : public StorageStubProvider {
 public:
  SmartRoundRobinChannelPool(
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
      int size) {
    channel_creator_ = channel_creator;
    for (int i = 0; i < size; i++) {
      channels_.push_back(channel_creator());
    }
    InitChannelStateMap();
  }

  StorageStubProvider::StubHolder GetStorageStub() override {
    absl::MutexLock l(&lock_);
    cursor_ = (cursor_ + 1) % channels_.size();
    StorageStubProvider::StubHolder holder = {
        google::storage::v1::Storage::NewStub(channels_[cursor_]),
        (void*)channels_[cursor_].get()};
    return holder;
  }

  bool ReportResult(void* handle, const grpc::Status& status,
                    const grpc::ClientContext& context,
                    absl::Duration elapsed_time, int64_t bytes) override {
    absl::MutexLock l(&lock_);

    // if error, evict it right away.
    if (!status.ok()) {
      if (status.error_code() == grpc::CANCELLED ||
          status.error_code() == grpc::DEADLINE_EXCEEDED) {
        auto i = std::find_if(channels_.begin(), channels_.end(),
                              [handle](std::shared_ptr<grpc::Channel> val) {
                                return (void*)val.get() == handle;
                              });
        if (i != channels_.end()) {
          std::cout << "Evict the channel (peer=" << context.peer()
                    << ") due to error:" << status.error_code() << std::endl;
          *i = channel_creator_();
        }
        return true;
      } else {
        return false;
      }
    }

    // Update the state of the corresponding channel.
    auto iter = channep_state_map_.find(handle);
    if (iter != channep_state_map_.end()) {
      iter->second.count += 1;
      iter->second.total_bytes += bytes;
      iter->second.total_time += elapsed_time;
      iter->second.peer = context.peer();
    }

    // once the update counter exceeds the threadhold, it evaluates
    // the last performer to be evicted.
    score_count += 1;
    if (score_count > (int)channep_state_map_.size() * 3) {
      void* least_key = 0;
      int valid_count = 0;
      int64_t least_score = 0xffffffffffffl;
      std::string least_peer;
      int64_t best_score = 0;
      for (auto s : channep_state_map_) {
        if (s.second.count == 0) {
          continue;
        }
        valid_count += 1;
        int64_t score = s.second.total_bytes /
                        absl::ToInt64Milliseconds(s.second.total_time);
        if (score < least_score) {
          least_key = s.first;
          least_score = score;
          least_peer = s.second.peer;
        }
        if (score > best_score) {
          best_score = score;
        }
      }

      if (valid_count < int(channep_state_map_.size()) / 2) {
        std::cerr << "No quorum: " << valid_count << " of "
                  << channep_state_map_.size() / 2 << std::endl;
      } else if (least_score >= best_score / 3) {
        std::cerr << "No least to evict: least_score " << least_score
                  << ", best_score: " << best_score << std::endl;
        score_count = 0;
        InitChannelStateMap();
      } else {
        auto i = std::find_if(channels_.begin(), channels_.end(),
                              [least_key](std::shared_ptr<grpc::Channel> val) {
                                return (void*)val.get() == least_key;
                              });
        if (i != channels_.end()) {
          std::cout << "Evict the channel (peer=" << least_peer
                    << ") because it underperformed score: " << least_score
                    << ", best_score: " << best_score << std::endl;
          *i = channel_creator_();
        }
        score_count = 0;
        InitChannelStateMap();
      }
    }

    return true;
  }

 private:
  void InitChannelStateMap() {
    channep_state_map_.clear();
    for (auto c : channels_) {
      channep_state_map_[(void*)c.get()] =
          ChannelState{0, 0, absl::Duration(), ""};
    }
  }

 private:
  absl::Mutex lock_;
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
  std::vector<std::shared_ptr<grpc::Channel>> channels_;
  int cursor_ = 0;

  struct ChannelState {
    int count;
    int64_t total_bytes;
    absl::Duration total_time;
    std::string peer;
  };
  std::unordered_map<void*, ChannelState> channep_state_map_;
  int score_count = 0;
};

std::shared_ptr<StorageStubProvider> CreateSmartRoundRobinChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size) {
  return std::make_shared<SmartRoundRobinChannelPool>(channel_creator, size);
}
