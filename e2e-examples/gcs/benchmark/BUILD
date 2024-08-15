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
    srcs = [
        "channel_creator.cc",
    ],
    hdrs = [
        "channel_creator.h",
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
    ],
)

cc_library(
    name = "channel_policy",
    srcs = [
        "channel_policy.cc",
    ],
    hdrs = [
        "channel_policy.h",
    ],
    deps = [
        "channel_poller",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
    ],
)

cc_library(
    name = "channel_poller",
    srcs = [
        "channel_poller.cc",
    ],
    hdrs = [
        "channel_poller.h",
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "gcscpp_runner",
    srcs = [
        "gcscpp_runner.cc",
    ],
    hdrs = [
        "gcscpp_runner.h",
    ],
    deps = [
        "object_resolver",
        "parameters",
        "random_data",
        "runner",
        "runner_watcher",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@google_cloud_cpp//:experimental-storage_grpc",
        "@google_cloud_cpp//:storage",
    ],
)

cc_library(
    name = "grpc_admin",
    srcs = [
        "grpc_admin.cc",
    ],
    hdrs = [
        "grpc_admin.h",
    ],
    deps = [
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpcpp_admin",
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "grpc_runner",
    srcs = [
        "grpc_runner.cc",
        "grpc_xtra.cc",
    ],
    hdrs = [
        "grpc_runner.h",
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
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
        "@com_google_absl//absl/crc:crc32c",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
    ],
)

cc_library(
    name = "object_resolver",
    srcs = [
        "object_resolver.cc",
    ],
    hdrs = [
        "object_resolver.h",
    ],
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_library(
    name = "work_queue",
    srcs = [
        "work_queue.cc",
    ],
    hdrs = [
        "work_queue.h",
    ],
    deps = [
        "@com_google_absl//absl/synchronization",
    ],
)

cc_library(
    name = "parameters",
    srcs = [
        "parameters.cc",
    ],
    hdrs = [
        "parameters.h",
    ],
    deps = [
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
    ],
)

cc_library(
    name = "random_data",
    srcs = [
        "random_data.cc",
    ],
    hdrs = [
        "random_data.h",
    ],
    deps = [
        "@com_google_absl//absl/random",
        "@com_google_absl//absl/strings:cord",
    ],
)

cc_library(
    name = "runner_watcher",
    srcs = [
        "runner_watcher.cc",
    ],
    hdrs = [
        "runner_watcher.h",
    ],
    deps = [
        "parameters",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
    ],
)

cc_library(
    name = "runner",
    srcs = [
        "runner.cc",
    ],
    hdrs = [
        "runner.h",
    ],
    deps = [
        "channel_policy",
        "object_resolver",
        "parameters",
        "runner_watcher",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
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
        "gcscpp_runner",
        "grpc_admin",
        "grpc_runner",
        "parameters",
        "runner",
        "runner_watcher",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//test/core/test_util:stack_tracer",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@io_opentelemetry_cpp//exporters/prometheus:prometheus_exporter",
        "@io_opentelemetry_cpp//sdk/src/metrics",
    ],
)