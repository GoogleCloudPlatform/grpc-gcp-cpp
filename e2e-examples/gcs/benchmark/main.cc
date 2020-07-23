#include <grpc/status.h>
#include <grpcpp/channel.h>
#include <grpcpp/client_context.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/credentials.h>
#include <stdio.h>

#include <fstream>
#include <memory>
#include <streambuf>
#include <string>
#include <thread>
#include <unordered_map>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"

#include "channel_creator.h"
#include "channel_policy.h"
#include "print_result.h"
#include "runner.h"

using std::cerr;
using std::cout;
using std::endl;
using std::string;

ABSL_FLAG(string, access_token, "", "Access token for auth");
ABSL_FLAG(string, network, "default", "Network path (default, cfe, dp)");
ABSL_FLAG(string, operation, "read", "Operation type (read, write)");
ABSL_FLAG(string, host, "dns:///storage.googleapis.com:443", "Host to reach");
ABSL_FLAG(string, bucket, "gcs-grpc-team-veblush1",
          "Bucket to fetch object from");
ABSL_FLAG(string, object, "1G.txt", "Object to download");
ABSL_FLAG(string, object_format, "", "Format string to resolve the object");
ABSL_FLAG(
    int, object_start, 0,
    "An integer number specifying at which position to start. Default is 0");
ABSL_FLAG(int, object_stop, 0,
          "An integer number specifying at which position to end.");
ABSL_FLAG(int64_t, read_offset, -1, "Read offset for read");
ABSL_FLAG(int64_t, read_limit, -1, "Read limit for read");
ABSL_FLAG(int64_t, write_size, 0, "Write size");
ABSL_FLAG(int, runs, 1, "Number of times to run the download");
ABSL_FLAG(bool, verbose, false, "Show debug output and progress updates");
ABSL_FLAG(bool, crc32c, false, "Check CRC32C check for received content");
ABSL_FLAG(bool, resumable, false, "Use resumable-write for writing");
ABSL_FLAG(int, ctest, 0, "Test to get a list of peers from grpclb");
ABSL_FLAG(int, threads, 1, "The number of threads running downloding objects");
ABSL_FLAG(string, cpolicy, "perthread",
          "Channel Policy (perthread, percall, pool, bpool, spool)");
ABSL_FLAG(
    int, carg, 0,
    "Parameter for cpolicy (e.g. pool uses this as the number of channels)");
ABSL_FLAG(string, report_tag, "",
          "The user-defined tag to be inserted in the report");
ABSL_FLAG(string, report_file, "", "The file to append the line for the run");
ABSL_FLAG(string, data_file, "", "The data file to dump the all data");

int run_ctest(std::function<std::shared_ptr<grpc::Channel>()> channel_creator,
              int size);

std::shared_ptr<grpc::Channel> CreateBenchmarkGrpcChannel() {
  return CreateGrpcChannel(absl::GetFlag(FLAGS_host),
                           absl::GetFlag(FLAGS_access_token),
                           absl::GetFlag(FLAGS_network));
}

int main(int argc, char** argv) {
  absl::ParseCommandLine(argc, argv);

  const int ctest = absl::GetFlag(FLAGS_ctest);
  if (ctest > 0) {
    return run_ctest(&CreateBenchmarkGrpcChannel, ctest);
  }

  OperationType operation_type = OperationType::None;
  const std::string operation = absl::GetFlag(FLAGS_operation);
  if (operation == "read") {
    operation_type = OperationType::Read;
  } else if (operation == "write") {
    operation_type = OperationType::Write;
  } else {
    std::cerr << "Invalid operation: " << operation << std::endl;
    return 1;
  }

  const std::string cpolicy = absl::GetFlag(FLAGS_cpolicy);
  if (cpolicy != "perthread" && cpolicy != "percall" && cpolicy != "pool" &&
      cpolicy != "bpool" && cpolicy != "spool") {
    std::cerr << "Invalid cpolicy: " << cpolicy << std::endl;
    return 1;
  }

  const int carg = absl::GetFlag(FLAGS_carg);

  // Initializes a gRPC channel pool.
  std::shared_ptr<StorageStubProvider> stub_pool;
  if (cpolicy == "pool") {
    if (carg <= 0) {
      std::cerr << "Invalid carg: " << carg << std::endl;
      return 1;
    }
    stub_pool = CreateRoundRobinChannelPool(&CreateBenchmarkGrpcChannel, carg);
  } else if (cpolicy == "bpool") {
    if (carg <= 0) {
      std::cerr << "Invalid carg: " << carg << std::endl;
      return 1;
    }
    stub_pool =
        CreateRoundRobinPlusChannelPool(&CreateBenchmarkGrpcChannel, carg);
  } else if (cpolicy == "spool") {
    if (carg <= 0) {
      std::cerr << "Invalid carg: " << carg << std::endl;
      return 1;
    }
    stub_pool =
        CreateSmartRoundRobinChannelPool(&CreateBenchmarkGrpcChannel, carg);
  }

  auto watcher = std::make_shared<RunnerWatcher>(absl::GetFlag(FLAGS_verbose));

  // Creates runners per thread.
  std::vector<Runner> runners;
  for (int i = 1; i <= absl::GetFlag(FLAGS_threads); i++) {
    Runner::Parameter param;
    param.operation_type = operation_type;
    param.id = i;
    param.bucket = absl::GetFlag(FLAGS_bucket);
    param.object = absl::GetFlag(FLAGS_object);
    param.object_format = absl::GetFlag(FLAGS_object_format);
    param.object_start = absl::GetFlag(FLAGS_object_start);
    param.object_stop = absl::GetFlag(FLAGS_object_stop);
    param.read_offset = absl::GetFlag(FLAGS_read_offset);
    param.read_limit = absl::GetFlag(FLAGS_read_limit);
    param.write_size = absl::GetFlag(FLAGS_write_size);
    param.runs = absl::GetFlag(FLAGS_runs);
    param.verbose = absl::GetFlag(FLAGS_verbose);
    param.check_crc32c = absl::GetFlag(FLAGS_crc32c);
    param.resumable_write = absl::GetFlag(FLAGS_resumable);
    // If the number of channels is given, it uses the intialized
    // channel pool. Otherwise each thread is going to get a dedicated
    // channel.
    if (stub_pool != nullptr) {
      param.storage_stub_provider = stub_pool;
    } else if (cpolicy == "perthread") {
      param.storage_stub_provider =
          CreateConstChannelPool(&CreateBenchmarkGrpcChannel);
    } else if (cpolicy == "percall") {
      param.storage_stub_provider =
          CreateCreateNewChannelStubProvider(&CreateBenchmarkGrpcChannel);
    }
    runners.emplace_back(param, watcher);
  }

  // Spawns benchmark runners and waits until they're done.
  absl::Time run_start = absl::Now();
  watcher->SetStartTime(run_start);
  std::vector<std::thread> runner_threads;
  for (int i = 0; i < absl::GetFlag(FLAGS_threads); i++) {
    Runner& runner = runners[i];
    runner_threads.emplace_back([&runner]() { runner.Run(); });
  }
  std::for_each(runner_threads.begin(), runner_threads.end(),
                [](std::thread& t) { t.join(); });
  absl::Time run_end = absl::Now();
  watcher->SetDuration(run_end - run_start);

  // Results
  std::string report_tag = absl::GetFlag(FLAGS_report_tag);
  std::string report_file = absl::GetFlag(FLAGS_report_file);
  std::string data_file = absl::GetFlag(FLAGS_data_file);
  PrintResult(*watcher);
  if (!report_file.empty()) {
    WriteReport(*watcher, report_file, report_tag);
  }
  if (!data_file.empty()) {
    WriteData(*watcher, data_file, report_tag);
  }

  return 0;
}
