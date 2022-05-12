#include <stdio.h>

#include <memory>

#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "channel_creator.h"
#include "channel_policy.h"
#include "gcscpp_runner.h"
#include "grpc_runner.h"
#include "parameters.h"
#include "print_result.h"
#include "runner.h"
#include "test/core/util/stack_tracer.h"

int main(int argc, char **argv) {
  grpc_core::testing::InitializeStackTracer(argv[0]);

  absl::ParseCommandLine(argc, argv);
  absl::optional<Parameters> parameters = GetParameters();
  if (!parameters.has_value()) {
    return 1;
  }

  // Create a runner based on a client
  auto watcher = std::make_shared<RunnerWatcher>(
      parameters->warmups * parameters->threads, parameters->verbose);
  std::unique_ptr<Runner> runner;
  if (parameters->client == "grpc") {
    runner.reset(new GrpcRunner(*parameters, watcher));
  } else if (parameters->client == "gcscpp") {
    runner.reset(new GcscppRunner(*parameters, watcher));
  } else {
    std::cerr << "Invalid client: " << parameters->client << std::endl;
    return 1;
  }

  // Let's run!
  absl::Time run_start = absl::Now();
  watcher->SetStartTime(run_start);
  if (!runner->Run()) {
    std::cerr << "Runner failed to complete a run." << std::endl;
    return 1;
  }
  watcher->SetDuration(absl::Now() - run_start);

  // Results
  if (!parameters->report_file.empty()) {
    WriteReport(*watcher, parameters->report_file, parameters->report_tag);
  }
  if (!parameters->data_file.empty()) {
    WriteData(*watcher, parameters->data_file, parameters->report_tag);
  }

  return 0;
}
