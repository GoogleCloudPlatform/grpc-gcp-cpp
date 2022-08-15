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
