// Copyright 2024 gRPC authors.
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

#include "grpc_otel.h"

#include "opentelemetry/exporters/prometheus/exporter_factory.h"
#include "opentelemetry/exporters/prometheus/exporter_options.h"
#include "opentelemetry/sdk/metrics/meter_provider.h"
#include "opentelemetry/sdk/metrics/view/instrument_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/meter_selector_factory.h"
#include "opentelemetry/sdk/metrics/view/view_factory.h"

#include <grpcpp/ext/otel_plugin.h>
#include <grpcpp/grpcpp.h>

namespace {

void AddLatencyView(opentelemetry::sdk::metrics::MeterProvider* provider,
                    const std::string& name, const std::string& unit) {
  auto histogram_config = std::make_shared<
      opentelemetry::sdk::metrics::HistogramAggregationConfig>();
  histogram_config->boundaries_ = {
      0,     0.00001, 0.00005, 0.0001, 0.0003, 0.0006, 0.0008, 0.001, 0.002,
      0.003, 0.004,   0.005,   0.006,  0.008,  0.01,   0.013,  0.016, 0.02,
      0.025, 0.03,    0.04,    0.05,   0.065,  0.08,   0.1,    0.13,  0.16,
      0.2,   0.25,    0.3,     0.4,    0.5,    0.65,   0.8,    1,     2,
      5,     10,      20,      50,     100};
  provider->AddView(
      opentelemetry::sdk::metrics::InstrumentSelectorFactory::Create(
          opentelemetry::sdk::metrics::InstrumentType::kHistogram, name, unit),
      opentelemetry::sdk::metrics::MeterSelectorFactory::Create(
          "grpc-c++", grpc::Version(), ""),
      opentelemetry::sdk::metrics::ViewFactory::Create(
          name, "", unit,
          opentelemetry::sdk::metrics::AggregationType::kHistogram,
          std::move(histogram_config)));
}

}  // namespace

absl::Status StartGrpcOpenTelemetry(std::string prometheus_endpoint) {
  // Register a global gRPC OpenTelemetry plugin configured with a prometheus
  // exporter.
  opentelemetry::exporter::metrics::PrometheusExporterOptions opts;
  opts.url = prometheus_endpoint;
  auto prometheus_exporter =
      opentelemetry::exporter::metrics::PrometheusExporterFactory::Create(opts);
  auto meter_provider =
      std::make_shared<opentelemetry::sdk::metrics::MeterProvider>();
  // The default histogram boundaries are not granular enough for RPCs. Override
  // the "grpc.client.attempt.duration" view as recommended by
  // https://github.com/grpc/proposal/blob/master/A66-otel-stats.md.
  AddLatencyView(meter_provider.get(), "grpc.client.attempt.duration", "s");
  meter_provider->AddMetricReader(std::move(prometheus_exporter));
  return grpc::OpenTelemetryPluginBuilder()
      .SetMeterProvider(std::move(meter_provider))
      .AddOptionalLabel("grpc.lb.locality")
      .BuildAndRegisterGlobal();
}
