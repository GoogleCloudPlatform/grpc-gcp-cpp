package(
    default_visibility = ["//visibility:public"],
)

load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

proto_library(
    name = "storage_resources_proto",
    srcs = ["storage_resources.proto"],
    deps = [
        "@com_google_protobuf//:field_mask_proto",
        "@com_google_protobuf//:timestamp_proto",
        "@com_google_protobuf//:wrappers_proto",
    ],
)

cc_proto_library(
    name = "storage_resources_cc_proto",
    deps = [":storage_resources_proto"],
)

cc_grpc_library(
    name = "storage_resources_cc_grpc",
    srcs = [":storage_resources_proto"],
    grpc_only = True,
    deps = [":storage_resources_cc_proto"],
)

proto_library(
    name = "storage_proto",
    srcs = ["storage.proto"],
    deps = [
        ":storage_resources_proto",
        "@com_google_googleapis//google/api:annotations_proto",
        "@com_google_googleapis//google/api:client_proto",
        "@com_google_googleapis//google/iam/v1:iam_policy_proto",
        "@com_google_googleapis//google/iam/v1:policy_proto",
        "@com_google_protobuf//:empty_proto",
        "@com_google_protobuf//:field_mask_proto",
        "@com_google_protobuf//:wrappers_proto",
    ],
)

cc_proto_library(
    name = "storage_cc_proto",
    deps = [":storage_proto"],
)

cc_grpc_library(
    name = "storage_cc_grpc",
    srcs = [":storage_proto"],
    grpc_only = True,
    deps = [":storage_cc_proto"],
)
