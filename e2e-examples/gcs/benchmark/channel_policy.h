#ifndef GCS_BENCHMARK_CHANNEL_POLICY_H_
#define GCS_BENCHMARK_CHANNEL_POLICY_H_

#include <memory>

#include <grpcpp/channel.h>

#include "absl/strings/string_view.h"
#include "absl/time/time.h"

#include "google/storage/v1/storage.grpc.pb.h"

class StorageStubProvider {
 public:
  struct StubHolder {
    std::unique_ptr<google::storage::v1::Storage::Stub> stub;
    void* handle;
  };

 public:
  virtual ~StorageStubProvider() = default;

  // Returns a stub holder holding
  // - stub for calling the RPC
  // - handle for reporing the result
  virtual StubHolder GetStorageStub() = 0;

  // Reports result
  // - Returns if it's okay to keep going
  virtual bool ReportResult(void* handle, const grpc::Status& status,
                            const grpc::ClientContext& context,
                            absl::Duration elapsed_time, int64_t bytes) = 0;
};

std::shared_ptr<StorageStubProvider> CreateConstChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator);

std::shared_ptr<StorageStubProvider> CreateCreateNewChannelStubProvider(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator);

std::shared_ptr<StorageStubProvider> CreateRoundRobinChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size);

std::shared_ptr<StorageStubProvider> CreateRoundRobinPlusChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size);

std::shared_ptr<StorageStubProvider> CreateSmartRoundRobinChannelPool(
    std::function<std::shared_ptr<grpc::Channel>()> channel_creator, int size);

#endif  // GCS_BENCHMARK_CHANNEL_POLICY_H_
