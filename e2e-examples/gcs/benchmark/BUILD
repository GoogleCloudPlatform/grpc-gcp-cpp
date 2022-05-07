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
    ],
)

cc_library(
    name = "common",
    hdrs = [
        "common.h",
    ],
    srcs = [
        "common.cc",
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
    name = "runner_watcher",
    hdrs = [
        "runner_watcher.h",
    ],
    srcs = [
        "runner_watcher.cc",
    ],
    deps = [
        "common",
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
        "common",
        "channel_policy",
        "object_resolver",
        "runner_watcher",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/synchronization",
        "//e2e-examples/gcs/crc32c",
    ],
)

cc_binary(
    name = "benchmark",
    srcs = [
        "ctest.cc",
        "main.cc",
        "print_result.cc",
        "print_result.h",
    ],
    deps = [
        "channel_creator",
        "channel_policy",
        "runner",
        "runner_watcher",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//src/proto/grpc/health/v1:health_proto",
        "@com_github_grpc_grpc//test/core/util:stack_tracer",

        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
    ],
)
