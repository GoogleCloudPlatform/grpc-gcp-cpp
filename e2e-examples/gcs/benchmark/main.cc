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

#include <stdio.h>

#include <memory>

#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"
#include "absl/time/time.h"
#include "channel_creator.h"
#include "channel_policy.h"
#include "gcscpp_runner.h"
#include "grpc_admin.h"
#include "grpc_otel.h"
#include "grpc_runner.h"
#include "parameters.h"
#include "print_result.h"
#include "runner.h"
#include "test/core/test_util/stack_tracer.h"

int main(int argc, char **argv) {
  grpc_core::testing::InitializeStackTracer(argv[0]);

  absl::ParseCommandLine(argc, argv);
  absl::optional<Parameters> parameters = GetParameters();
  if (!parameters.has_value()) {
    return 1;
  }

  if (parameters->prometheus_endpoint != "") {
    absl::Status s = StartGrpcOpenTelemetry(parameters->prometheus_endpoint);
    if (!s.ok()) {
      std::cerr << "OpenTelemetry failure: " << s.ToString().c_str()
                << std::endl;
      return 1;
    }
  }

  if (parameters->grpc_admin > 0) {
    StartGrpcAdmin(parameters->grpc_admin);
  }

  // Create a runner based on a client
  auto watcher = std::make_shared<RunnerWatcher>(
      parameters->warmups * parameters->threads, parameters->verbose);
  std::unique_ptr<Runner> runner;
  if (parameters->client == "grpc") {
    runner.reset(new GrpcRunner(*parameters, watcher));
  } else if (parameters->client == "gcscpp-json" ||
             parameters->client == "gcscpp-grpc") {
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

  StopGrpcAdmin();

  // Results
  PrintResult(*watcher);
  if (!parameters->report_file.empty()) {
    WriteReport(*watcher, parameters->report_file, parameters->report_tag);
  }
  if (!parameters->data_file.empty()) {
    WriteData(*watcher, parameters->data_file, parameters->report_tag);
  }

  return 0;
}
