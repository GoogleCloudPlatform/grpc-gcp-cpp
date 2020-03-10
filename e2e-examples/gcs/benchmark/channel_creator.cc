#include "channel_creator.h"

#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>

std::shared_ptr<grpc::Channel> CreateGrpcChannel(absl::string_view host,
                                                 absl::string_view access_token,
                                                 absl::string_view network) {
  if (access_token.empty()) {
    std::shared_ptr<grpc_impl::ChannelCredentials> channel_cred;
    grpc::ChannelArguments channel_args;
    channel_args.SetServiceConfigJSON(
        "{\"loadBalancingConfig\":[{\"grpclb\":{"
        "\"childPolicy\":[{\"pick_first\":{}}]}}]"
        "}");
    if (network == "cfe") {
      channel_args.SetInt("grpc.dns_enable_srv_queries",
                          0);  // Disable DirectPath
    } else if (network == "dp") {
      grpc::experimental::AltsCredentialsOptions alts_opts;
      channel_cred = grpc::CompositeChannelCredentials(
          grpc::experimental::AltsCredentials(alts_opts),
          grpc::GoogleComputeEngineCredentials());
      channel_args.SetInt("grpc.dns_enable_srv_queries",
                          1);  // Enable DirectPath
    } else if (network == "dp2") {
      channel_cred = grpc::GoogleDefaultCredentials();
      channel_args.SetInt("grpc.dns_enable_srv_queries",
                          1);  // Enable DirectPath
    }

    if (channel_cred == nullptr) {
      channel_cred = grpc::GoogleDefaultCredentials();
    }
    std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(
        std::string(host), channel_cred, channel_args);
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
        grpc::CreateChannel(std::string(host), credentials);
    return channel;
  }
}
