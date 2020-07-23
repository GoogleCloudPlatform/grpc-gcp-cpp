#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <unordered_map>

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include "absl/memory/memory.h"

#include "src/proto/grpc/health/v1/health.grpc.pb.h"

int run_ctest(std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
              int size) {
  struct ChannelState {
    std::shared_ptr<grpc::Channel> channel;
    std::unique_ptr<std::thread> thread;
    std::string peer;
  };
  std::vector<ChannelState> states;
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
  std::cout << "### Summary: Total " << peer_count_map.size() << " peers" << std::endl;
  for (const auto& p : peer_count_map) {
      std::cout << p.first << " -> " << p.second << " times." << std::endl;
  }
  return 0;
}
