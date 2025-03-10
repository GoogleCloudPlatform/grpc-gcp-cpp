/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include <algorithm>
#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "e2e-examples/gcs/dummy_server/gcs_util.h"
#include "google/storage/v2/storage.grpc.pb.h"

using grpc::CallbackServerContext;
using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerUnaryReactor;
using grpc::Status;
using grpc::StatusCode;

using ::google::storage::v2::GetObjectRequest;
using ::google::storage::v2::Object;
using ::google::storage::v2::ReadObjectRequest;
using ::google::storage::v2::ReadObjectResponse;
using ::google::storage::v2::StartResumableWriteRequest;
using ::google::storage::v2::StartResumableWriteResponse;
using ::google::storage::v2::WriteObjectRequest;
using ::google::storage::v2::WriteObjectResponse;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

// Logic and data behind the server's behavior.
class StorageServiceImpl final
    : public google::storage::v2::Storage::CallbackService {
  ServerUnaryReactor* GetObject(CallbackServerContext* context,
                                const GetObjectRequest* request,
                                Object* reply) override {
    Status status;
    const int64_t object_size =
        GcsUtil::GetObjectSize(request->bucket(), request->object());
    if (object_size < 0) {
      status = Status(StatusCode::NOT_FOUND, "Object is not found");
    } else {
      status = Status::OK;
      reply->set_size(object_size);
    }
    ServerUnaryReactor* reactor = context->DefaultReactor();
    reactor->Finish(status);
    return reactor;
  }

  grpc::ServerWriteReactor<ReadObjectResponse>* ReadObject(
      CallbackServerContext* context,
      const ReadObjectRequest* request) override {
    class Reactor : public grpc::ServerWriteReactor<ReadObjectResponse> {
     public:
      Reactor(const ReadObjectRequest* request) {
        const int64_t object_size =
            GcsUtil::GetObjectSize(request->bucket(), request->object());
        if (object_size < 0) {
          Finish(Status(StatusCode::NOT_FOUND, "Object is not found"));
          return;
        }
        left_size_ = object_size;
        if (request->read_limit() > 0) {
          left_size_ = std::min(left_size_, request->read_limit());
        }
        chunk_data_ = GcsUtil::GetObjectDataChunk(
            google::storage::v2::ServiceConstants::MAX_READ_CHUNK_BYTES);
        MaybeWriteNext();
      }

      void OnWriteDone(bool ok) override {
        if (ok) {
          MaybeWriteNext();
        } else {
          Finish(grpc::Status(grpc::StatusCode::UNKNOWN, "Unexpected failure"));
        }
      }

      void OnDone() override { delete this; }

     private:
      void MaybeWriteNext() {
        if (left_size_ == 0) {
          Finish(grpc::Status::OK);
          return;
        }
        auto data_size =
            std::min(left_size_, static_cast<int64_t>(chunk_data_.size()));
        response_.mutable_checksummed_data()->set_content(
            absl::string_view(chunk_data_.c_str(), data_size));
        left_size_ -= data_size;
        StartWrite(&response_);
      }

     private:
      int64_t left_size_;
      std::string chunk_data_;
      ReadObjectResponse response_;
    };

    return new Reactor(request);
  }

  /*
    ServerUnaryReactor* StartResumableWrite(
        CallbackServerContext* context, const StartResumableWriteRequest*
    request, StartResumableWriteResponse* reply) override { ServerUnaryReactor*
    reactor = context->DefaultReactor(); reactor->Finish(Status::OK); return
    reactor;
    }

    grpc::ServerReadReactor<WriteObjectRequest>* WriteObject(
        CallbackServerContext* context, WriteObjectResponse* summary) override {
      return nullptr;
    }
  */
};

void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  StorageServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}
