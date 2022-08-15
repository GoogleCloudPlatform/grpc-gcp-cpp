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
    absl::MutexLock lock(&mu_);
    shutdown_ = true;
    cq_.Shutdown();
  }
  thread_->join();
}

void ChannelPoller::StartWatch() {
  grpc_connectivity_state last_observed = channel_->GetState(true);
  channel_->NotifyOnStateChange(
      last_observed, std::chrono::system_clock::now() + std::chrono::seconds(1),
      &cq_, nullptr);
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
