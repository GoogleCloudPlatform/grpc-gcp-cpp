# Copyright 2022 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cc_library(
    name = "channel_creator",
    hdrs = [
        "channel_creator.h",
    ],
    srcs = [
        "channel_creator.cc",
    ],
    deps = [
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "channel_policy",
    hdrs = [
        "channel_policy.h",
    ],
    srcs = [
        "channel_policy.cc",
    ],
    deps = [
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "channel_poller",
    ],
)

cc_library(
    name = "channel_poller",
    hdrs = [
        "channel_poller.h",
    ],
    srcs = [
        "channel_poller.cc",
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "gcscpp_runner",
    hdrs = [
        "gcscpp_runner.h",
    ],
    srcs = [
        "gcscpp_runner.cc",
    ],
    deps = [
        "object_resolver",
        "parameters",
        "random_data",
        "runner",
        "runner_watcher",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@google_cloud_cpp//:storage",
        "@google_cloud_cpp//:experimental-storage_grpc",
    ],
)

cc_library(
    name = "grpc_admin",
    hdrs = [
        "grpc_admin.h",
    ],
    srcs = [
        "grpc_admin.cc",
    ],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpcpp_admin",
    ],
)

cc_library(
    name = "grpc_otel",
    hdrs = [
        "grpc_otel.h",
    ],
    srcs = [
        "grpc_otel.cc",
    ],
    deps = [
        "@com_google_absl//absl/status",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpcpp_otel_plugin",
        "@io_opentelemetry_cpp//exporters/prometheus:prometheus_exporter",
        "@io_opentelemetry_cpp//sdk/src/metrics",
    ],
)

cc_library(
    name = "grpc_runner",
    hdrs = [
        "grpc_runner.h",
    ],
    srcs = [
        "grpc_runner.cc",
        "grpc_xtra.cc",
    ],
    deps = [
        "channel_creator",
        "channel_policy",
        "object_resolver",
        "parameters",
        "random_data",
        "runner",
        "runner_watcher",
        "work_queue",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
        "@com_google_absl//absl/crc:crc32c",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "object_resolver",
    hdrs = [
        "object_resolver.h",
    ],
    srcs = [
        "object_resolver.cc",
    ],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "work_queue",
    hdrs = [
        "work_queue.h",
    ],
    srcs = [
        "work_queue.cc",
    ],
    deps = [
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "parameters",
    hdrs = [
        "parameters.h",
    ],
    srcs = [
        "parameters.cc",
    ],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
    ],    
)

cc_library(
    name = "random_data",
    hdrs = [
        "random_data.h",
    ],
    srcs = [
        "random_data.cc",
    ],
    deps = [
        "@com_google_absl//absl/strings:cord",
        "@com_google_absl//absl/random",
    ],
)

cc_library(
    name = "runner_watcher",
    hdrs = [
        "runner_watcher.h",
    ],
    srcs = [
        "runner_watcher.cc",
    ],
    deps = [
        "parameters",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "runner",
    hdrs = [
        "runner.h",
    ],
    srcs = [
        "runner.cc",
    ],
    deps = [
        "parameters",
        "channel_policy",
        "object_resolver",
        "runner_watcher",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_binary(
    name = "benchmark",
    srcs = [
        "main.cc",
        "print_result.cc",
        "print_result.h",
    ],
    deps = [
        "channel_creator",
        "channel_policy",
        "parameters",
        "runner",
        "runner_watcher",
        "gcscpp_runner",
        "grpc_admin",
        "grpc_otel",
        "grpc_runner",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//test/core/test_util:stack_tracer",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
    ],
)
