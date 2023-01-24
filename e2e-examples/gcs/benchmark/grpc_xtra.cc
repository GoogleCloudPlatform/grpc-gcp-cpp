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

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>
#include <vector>

#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "google/storage/v2/storage.grpc.pb.h"
#include "parameters.h"
#include "src/proto/grpc/health/v1/health.grpc.pb.h"

int run_ctest(std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
              const Parameters& parameters) {
  struct ChannelState {
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<std::thread> thread;
    std::string peer;
  };
  std::vector<ChannelState> states;
  const int size = parameters.ctest;
  for (int i = 0; i < size; i++) {
    states.push_back(ChannelState{channel_creator()});
  }
  for (int i = 0; i < size; i++) {
    ChannelState& cur_state = states[i];
    cur_state.thread = absl::make_unique<std::thread>([&cur_state]() {
      grpc::health::v1::HealthCheckRequest request;
      request.set_service("google.storage.v1.Storage");
      grpc::health::v1::HealthCheckResponse response;
      grpc::ClientContext context;
      auto stub = grpc::health::v1::Health::NewStub(cur_state.channel);
      auto s = stub->Check(&context, request, &response);
      cur_state.peer = context.peer();
    });
  }
  std::for_each(states.begin(), states.end(),
                [](ChannelState& state) { state.thread->join(); });
  std::unordered_map<std::string, int> peer_count_map;
  std::cout << "### Result: Requests for " << size << " channels" << std::endl;
  for (int i = 0; i < size; i++) {
    std::cout << i << ": " << states[i].peer << std::endl;
    peer_count_map[states[i].peer] += 1;
  }
  std::cout << std::endl;
  std::cout << "### Summary: Total " << peer_count_map.size() << " peers"
            << std::endl;
  for (const auto& p : peer_count_map) {
    std::cout << p.first << " -> " << p.second << " times." << std::endl;
  }
  return 0;
}

int run_mtest(std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
              const Parameters& parameters) {
  auto channel = channel_creator();
  auto stub = google::storage::v2::Storage::NewStub(channel);

  google::storage::v2::GetObjectRequest request;
  request.set_bucket(absl::StrCat("projects/_/buckets/", parameters.bucket));
  request.set_object(parameters.object);

  grpc::ClientContext context;
  google::storage::v2::Object reply;
  grpc::Status status = stub->GetObject(&context, request, &reply);

  if (status.ok()) {
    std::cerr << "Status: OK" << std::endl;
  } else {
    std::cerr << "Status:" << std::endl;
    std::cerr << "- Code:    " << status.error_code() << std::endl;
    std::cerr << "- Message: " << status.error_message() << std::endl;
    std::cerr << "- Details: " << status.error_details() << std::endl;
  }

  std::cerr << "ServerInitialMetadata:" << std::endl;
  auto imd = context.GetServerInitialMetadata();
  for (auto v : imd) {
    std::cerr << "- " << v.first << " = " << v.second << std::endl;
  }

  std::cerr << "ServerTrailingMetadata:" << std::endl;
  auto tmd = context.GetServerTrailingMetadata();
  for (auto v : tmd) {
    std::cerr << "- " << v.first << " = " << v.second << std::endl;
  }

  return 0;
}
