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

#include "grpc_admin.h"

#include "absl/strings/str_cat.h"

#include <grpcpp/ext/admin_services.h>
#include <grpcpp/grpcpp.h>

#include <thread>

namespace {

std::unique_ptr<grpc::Server> g_admin_server;
std::unique_ptr<std::thread> g_admin_thread;

}  // namespace

void StartGrpcAdmin(int port) {
  if (g_admin_thread.get() != nullptr) {
    return;
  }
  g_admin_thread.reset(new std::thread([port]() {
    grpc::ServerBuilder builder;
    grpc::AddAdminServices(&builder);
    builder.AddListeningPort(absl::StrCat("0.0.0.0:", port),
                             grpc::InsecureServerCredentials());
    g_admin_server = builder.BuildAndStart();
    g_admin_server->Wait();
    g_admin_server = nullptr;
  }));
}

void StopGrpcAdmin() {
  if (g_admin_thread.get() == nullptr) {
    return;
  }
  g_admin_server->Shutdown();
  g_admin_thread->join();
  g_admin_thread.reset(nullptr);
}
