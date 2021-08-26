#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>

#include <fstream>
#include <memory>
#include <streambuf>
#include <string>
#include <thread>
#include <unordered_map>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "channel_manager.h"

using ReadObjectRequest = google::storage::v2::ReadObjectRequest;
using ReadObjectResponse = google::storage::v2::ReadObjectResponse;

ABSL_FLAG(bool, directpath, true, "Whether to allow DirectPath");
ABSL_FLAG(std::string, access_token, "", "Access token for auth");
ABSL_FLAG(std::string, host, "dns:///storage.googleapis.com:443",
          "Host to reach");
ABSL_FLAG(std::string, bucket, "gcs-grpc-team-veblush1",
          "Bucket to fetch object from");
ABSL_FLAG(std::string, object, "1MB.bin", "Object to download");
ABSL_FLAG(int, runs, 10, "Number of times to run the download");
ABSL_FLAG(int, threads, 8, "The number of threads running downloding objects");
ABSL_FLAG(int, channels, 4, "The max number of gRPC channels");
ABSL_FLAG(int, retries, 10, "The max number of gRPC retries");

std::shared_ptr<grpc::Channel> CreateBenchmarkGrpcChannel() {
  if (absl::GetFlag(FLAGS_access_token).empty()) {
    grpc::ChannelArguments channel_args;
    channel_args.SetServiceConfigJSON(
        "{\"loadBalancingConfig\":[{\"grpclb\":{"
        "\"childPolicy\":[{\"pick_first\":{}}]}}]"
        "}");
    if (!absl::GetFlag(FLAGS_directpath)) {
      channel_args.SetInt("grpc.dns_enable_srv_queries",
                          0);  // Disable DirectPath
    }
    std::shared_ptr<grpc::Channel> channel = grpc::CreateCustomChannel(
        std::string(absl::GetFlag(FLAGS_host)),
        grpc::GoogleDefaultCredentials(), channel_args);
    return channel;
  } else {
    std::shared_ptr<grpc::ChannelCredentials> credentials;
    std::shared_ptr<grpc::ChannelCredentials> channel_credentials =
        grpc::SslCredentials(grpc::SslCredentialsOptions());
    if (absl::GetFlag(FLAGS_access_token) == "-") {
      credentials = channel_credentials;
    } else {
      std::shared_ptr<grpc::CallCredentials> call_credentials =
          grpc::AccessTokenCredentials(
              std::string(absl::GetFlag(FLAGS_access_token)));
      credentials = grpc::CompositeChannelCredentials(channel_credentials,
                                                      call_credentials);
    }
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(
        std::string(absl::GetFlag(FLAGS_host)), credentials);
    return channel;
  }

  grpc::ChannelArguments channel_args;
  channel_args.SetServiceConfigJSON(
      "{\"loadBalancingConfig\":[{\"grpclb\":{"
      "\"childPolicy\":[{\"pick_first\":{}}]}}]"
      "}");
  std::shared_ptr<grpc::Channel> channel =
      grpc::CreateCustomChannel(std::string(absl::GetFlag(FLAGS_host)),
                                grpc::GoogleDefaultCredentials(), channel_args);
  return channel;
}

void worker(ChannelManager& channel_manager, std::atomic_size_t& read_bytes) {
  // Downloads a given file N times.
  for (int i = 0; i < absl::GetFlag(FLAGS_runs); i++) {
    for (int j = 0; j < absl::GetFlag(FLAGS_retries); j++) {
      auto channel_handle = channel_manager.GetHandle();

      ReadObjectRequest request;
      request.set_bucket("projects/_/buckets/" + absl::GetFlag(FLAGS_bucket));
      request.set_object(absl::GetFlag(FLAGS_object));

      grpc::ClientContext context;
      std::unique_ptr<grpc::ClientReader<ReadObjectResponse>> reader =
          channel_handle.GetStub<google::storage::v2::Storage::Stub>()
              ->ReadObject(&context, request);

      int64_t total_bytes = 0;
      ReadObjectResponse response;
      while (reader->Read(&response)) {
        int64_t content_size = response.checksummed_data().content().size();
        total_bytes += content_size;
      }
      read_bytes += total_bytes;

      auto status = reader->Finish();
      if (!status.ok()) {
        std::cerr
            << absl::StrFormat(
                   "Download Error: Code=%d Message=%s Retries=%d from %s",
                   status.error_code(), status.error_message(), j,
                   context.peer())
            << std::endl;
      }
      channel_handle.OnRpcDone(status);

      // In case of retriable error, it's going to retry it
      if (status.error_code() != grpc::StatusCode::CANCELLED &&
          status.error_code() != grpc::StatusCode::DEADLINE_EXCEEDED) {
        break;
      }
    }
  }
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  ChannelManager channel_manager(absl::GetFlag(FLAGS_channels),
                                 &CreateBenchmarkGrpcChannel);

  // Spawns benchmark runners and waits until they're done.
  absl::Time run_start = absl::Now();
  std::atomic_size_t total_size(0);
  std::vector<std::thread> runner_threads;
  for (int i = 0; i < absl::GetFlag(FLAGS_threads); i++) {
    runner_threads.emplace_back([&channel_manager, &total_size]() {
      worker(channel_manager, total_size);
    });
  }
  std::for_each(runner_threads.begin(), runner_threads.end(),
                [](std::thread& t) { t.join(); });
  absl::Time run_end = absl::Now();

  // Shows the result.
  double elapsed = absl::ToDoubleSeconds(run_end - run_start);
  std::cout << "Data: " << total_size << " bytes" << std::endl;
  std::cout << "Elapsed: " << elapsed << " sec" << std::endl;
  std::cout << "Throughput: " << total_size / elapsed / 1024 / 1024 << " MB/s"
            << std::endl;
  return 0;
}
