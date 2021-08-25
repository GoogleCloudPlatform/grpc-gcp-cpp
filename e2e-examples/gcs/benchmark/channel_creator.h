#ifndef GCS_BENCHMARK_CHANNEL_CREATOR_H_
#define GCS_BENCHMARK_CHANNEL_CREATOR_H_

#include <memory>

#include <grpcpp/channel.h>

#include "absl/strings/string_view.h"

std::shared_ptr<grpc::Channel> CreateGrpcChannel(absl::string_view host,
                                                 absl::string_view access_token,
                                                 absl::string_view network,
                                                 bool use_td);

#endif  // GCS_BENCHMARK_CHANNEL_CREATOR_H_
