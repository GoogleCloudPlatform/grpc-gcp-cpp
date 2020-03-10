#ifndef GCS_SAMPLE_CHANNEL_MANAGER_H_
#define GCS_SAMPLE_CHANNEL_MANAGER_H_

#include <memory>

#include <grpcpp/channel.h>

#include "absl/memory/memory.h"
#include "absl/strings/string_view.h"
#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"

#include "google/storage/v1/storage.grpc.pb.h"

class ChannelManager;

// This is to track a lifetime of rpc call. This is supposed to
// be used for a single RPC. ChannelManager assumes that users
// will use this to make a single RPC call and call OnRpcDone
// with its call status. This instance should be short-lived.
//
// For example:
//    auto handle = channel_manager.GetHandle();
//    handle.GetStub<RpcStub>().RpcCall(...)
//    handle.OnRpcDone(status);
class ChannelHandle {
 private:
  ChannelHandle(ChannelManager* manager,
                std::shared_ptr<grpc::Channel> channel);

 public:
  ~ChannelHandle();

  // Move-only object
  ChannelHandle(ChannelHandle&&) = default;
  ChannelHandle& operator=(ChannelHandle&&) = default;
  ChannelHandle(const ChannelHandle&) = delete;
  ChannelHandle& operator=(const ChannelHandle&) = delete;

  // Returns a stub instance of given type.
  template <typename T>
  std::unique_ptr<T> GetStub() {
    assert(!on_rpc_done_called_);
    return absl::make_unique<T>(channel_);
  }

  // When a RPC gets response, this function needs to be called so that
  // a channel manager can notice this call finished.
  void OnRpcDone(grpc::Status status);

 private:
  ChannelManager* manager_;
  std::shared_ptr<grpc::Channel> channel_;
  bool on_rpc_done_called_;

  friend class ChannelManager;
};

// This manages a channel pool and tracks the usage of each channel.
// Using this usage information, a manage can manage to pick the best
// channel for the next rpc call. When this instance destroys, all
// ChannelHandle instances created from this should be destroyed.
class ChannelManager {
 public:
  ChannelManager(
      // The maximum number of channels in the pool
      size_t max_channel_count,
      // A function to create a gRPC channel
      std::function<std::shared_ptr<grpc::Channel>()> channel_creator);

  ~ChannelManager();

  // Returns the ChannelHandle to be used to make a rpc call.
  ChannelHandle GetHandle();

 private:
  void OnRpcDone(grpc::Channel* channel, grpc::Status status);
  void OnChannelHandleDestroyed(grpc::Channel* channel);

 private:
  struct ChannelState {
    std::shared_ptr<grpc::Channel> channel;
    // The number of in-progress RPC.
    size_t in_use_count;
  };

  absl::Mutex lock_;
  size_t max_channel_count_;
  size_t channel_handle_count_;
  std::function<std::shared_ptr<grpc::Channel>()> channel_creator_;
  std::vector<ChannelState> channel_states_;

  friend class ChannelHandle;
};

#endif  // GCS_SAMPLE_CHANNEL_MANAGER_H_
