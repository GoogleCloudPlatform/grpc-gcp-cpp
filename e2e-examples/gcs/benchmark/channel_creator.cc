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

#include "channel_creator.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

#include <fstream>

#include "absl/strings/str_format.h"

static std::string LoadStringFromFile(std::string path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    std::cout << "Failed to open " << path << std::endl;
    abort();
  }
  std::stringstream sstr;
  sstr << file.rdbuf();
  return sstr.str();
}

std::shared_ptr<grpc::Channel> CreateGrpcChannel(absl::string_view host,
                                                 absl::string_view access_token,
                                                 absl::string_view network,
                                                 absl::string_view cred,
                                                 absl::string_view ssl_cert,
                                                 bool use_rr, bool use_td,
                                                 bool use_tx_zerocopy) {
  std::string target = std::string(host);
  if (target.empty()) {
    target = "storage.googleapis.com";
  }
  if (use_td) {
    target = "google-c2p:///" + target;
  }
  if (access_token.empty()) {
    std::shared_ptr<grpc::ChannelCredentials> channel_cred;
    grpc::ChannelArguments channel_args;
    if (!use_td) {
      const char* policy = use_rr ? "round_robin" : "pick_first";
      channel_args.SetServiceConfigJSON(
          absl::StrFormat("{\"loadBalancingConfig\":[{\"grpclb\":{"
                          "\"childPolicy\":[{\"%s\":{}}]}}]"
                          "}",
                          policy));
    }
    if (network == "cfe") {
      if (!use_td) {
        channel_args.SetInt("grpc.dns_enable_srv_queries",
                            0);  // Disable DirectPath
      }
    } else if (network == "dp") {
      grpc::experimental::AltsCredentialsOptions alts_opts;
      channel_cred = grpc::CompositeChannelCredentials(
          grpc::experimental::AltsCredentials(alts_opts),
          grpc::GoogleComputeEngineCredentials());
      if (!use_td) {
        channel_args.SetInt("grpc.dns_enable_srv_queries",
                            1);  // Enable DirectPath
      }
    } else if (network == "dp2") {
      channel_cred = grpc::GoogleDefaultCredentials();
      if (!use_td) {
        channel_args.SetInt("grpc.dns_enable_srv_queries",
                            1);  // Enable DirectPath
      }
    } else {
      if (!cred.empty()) {
        if (cred == "insecure") {
          channel_cred = grpc::InsecureChannelCredentials();
        } else if (cred == "ssl") {
          if (ssl_cert == "") {
            channel_cred = grpc::SslCredentials(grpc::SslCredentialsOptions());
          } else {
            grpc::SslCredentialsOptions ssl_options;
            ssl_options.pem_root_certs =
                LoadStringFromFile(std::string(ssl_cert));
            channel_cred = grpc::SslCredentials(ssl_options);
          }
        } else if (cred == "alts") {
          channel_cred = grpc::experimental::AltsCredentials(
              grpc::experimental::AltsCredentialsOptions());
        }
      }
    }

    // Use a local subchannel pool to avoid contention in gRPC.
    channel_args.SetInt(GRPC_ARG_USE_LOCAL_SUBCHANNEL_POOL, 1);
    // Effectively disable keepalive messages.
    auto constexpr kDisableKeepaliveTime =
        std::chrono::milliseconds(std::chrono::hours(24));
    channel_args.SetInt(GRPC_ARG_KEEPALIVE_TIME_MS,
                        static_cast<int>(kDisableKeepaliveTime.count()));
    // Make gRPC set the TCP_USER_TIMEOUT socket option to a value that detects
    // broken servers more quickly.
    auto constexpr kKeepaliveTimeout =
        std::chrono::milliseconds(std::chrono::seconds(30));
    channel_args.SetInt(GRPC_ARG_KEEPALIVE_TIMEOUT_MS,
                        static_cast<int>(kKeepaliveTimeout.count()));

    if (use_tx_zerocopy) {
      channel_args.SetInt(GRPC_ARG_TCP_TX_ZEROCOPY_ENABLED, 1);
    }

    if (channel_cred == nullptr) {
      channel_cred = grpc::GoogleDefaultCredentials();
    }
    std::shared_ptr<grpc::Channel> channel =
        grpc::CreateCustomChannel(target, channel_cred, channel_args);
    return channel;
  } else {
    std::shared_ptr<grpc::ChannelCredentials> credentials;
    std::shared_ptr<grpc::ChannelCredentials> channel_credentials =
        grpc::SslCredentials(grpc::SslCredentialsOptions());
    if (access_token == "-") {
      credentials = channel_credentials;
    } else {
      std::shared_ptr<grpc::CallCredentials> call_credentials =
          grpc::AccessTokenCredentials(std::string(access_token));
      credentials = grpc::CompositeChannelCredentials(channel_credentials,
                                                      call_credentials);
    }
    std::shared_ptr<grpc::Channel> channel =
        grpc::CreateChannel(target, credentials);
    return channel;
  }
}
