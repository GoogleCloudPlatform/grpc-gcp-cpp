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

#ifndef GCS_BENCHMARK_CHANNEL_CREATOR_H_
#define GCS_BENCHMARK_CHANNEL_CREATOR_H_

#include <memory>

#include <grpcpp/channel.h>

#include "absl/strings/string_view.h"

std::shared_ptr<grpc::Channel> CreateGrpcChannel(absl::string_view host,
                                                 absl::string_view access_token,
                                                 absl::string_view network,
                                                 bool use_rr,
                                                 bool use_td);

#endif  // GCS_BENCHMARK_CHANNEL_CREATOR_H_
