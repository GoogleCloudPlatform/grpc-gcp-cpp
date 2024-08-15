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

#include "opentelemetry/exporters/prometheus/exporter_factory.h"
#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/meter_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/view_factory.h"

#include <grpcpp/ext/otel_plugin.h>
#include <grpcpp/grpcpp.h>

#include "channel_creator.h"
#include "channel_policy.h"
#include "gcscpp_runner.h"
#include "grpc_admin.h"
#include "grpc_runner.h"
#include "parameters.h"
#include "print_result.h"
#include "runner.h"

#include "test/core/test_util/stack_tracer.h"

using ::opentelemetry::sdk::metrics::InstrumentSelectorFactory;
using ::opentelemetry::sdk::metrics::InstrumentType;
using ::opentelemetry::sdk::metrics::MeterSelectorFactory;
using ::opentelemetry::sdk::metrics::ViewFactory;

void AddLatencyView(opentelemetry::sdk::metrics::MeterProvider *provider,
                    const std::string &name, const std::string &unit) {
  auto histogram_config = std::make_shared<
      opentelemetry::sdk::metrics::HistogramAggregationConfig>();
  histogram_config->boundaries_ = {
      0,     0.00001, 0.00005, 0.0001, 0.0003, 0.0006, 0.0008, 0.001, 0.002,
      0.003, 0.004,   0.005,   0.006,  0.008,  0.01,   0.013,  0.016, 0.02,
      0.025, 0.03,    0.04,    0.05,   0.065,  0.08,   0.1,    0.13,  0.16,
      0.2,   0.25,    0.3,     0.4,    0.5,    0.65,   0.8,    1,     2,
      5,     10,      20,      50,     100};
  provider->AddView(
      InstrumentSelectorFactory::Create(InstrumentType::kHistogram, name, unit),
      MeterSelectorFactory::Create("grpc-c++", grpc::Version(), ""),
      ViewFactory::Create(
          name, "", unit,
          opentelemetry::sdk::metrics::AggregationType::kHistogram,
          std::move(histogram_config)));
}

int main(int argc, char **argv) {
  grpc_core::testing::InitializeStackTracer(argv[0]);
  absl::ParseCommandLine(argc, argv);
  absl::optional<Parameters> parameters = GetParameters();
  if (!parameters.has_value()) {
    return 1;
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

  if (parameters->prometheus_port != 0) {
    opentelemetry::exporter::metrics::PrometheusExporterOptions opts;
    opts.url = absl::StrCat("0.0.0.0:", parameters->prometheus_port);
    auto prometheus_exporter =
        opentelemetry::exporter::metrics::PrometheusExporterFactory::Create(
            opts);
    auto meter_provider =
        std::make_shared<opentelemetry::sdk::metrics::MeterProvider>();
    // The default histogram boundaries are not granular enough for RPCs.
    // Override the "grpc.client.attempt.duration" view as recommended by
    // https://github.com/grpc/proposal/blob/master/A66-otel-stats.md.
    AddLatencyView(meter_provider.get(), "grpc.client.attempt.duration", "s");
    AddLatencyView(meter_provider.get(),
                   "grpc.client.attempt.rcvd_total_compressed_message_size",
                   "B");
    AddLatencyView(meter_provider.get(),
                   "grpc.client.attempt.sent_total_compressed_message_size",
                   "B");
    meter_provider->AddMetricReader(std::move(prometheus_exporter));
    auto status = grpc::OpenTelemetryPluginBuilder()
                      .SetMeterProvider(std::move(meter_provider))
                      .BuildAndRegisterGlobal();
    if (!status.ok()) {
      std::cerr << "Failed to register gRPC OpenTelemetry Plugin: "
                << status.ToString() << std::endl;
      return static_cast<int>(status.code());
    }
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
