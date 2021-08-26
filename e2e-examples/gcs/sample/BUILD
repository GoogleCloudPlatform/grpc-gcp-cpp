cc_binary(
    name = "sample",
    srcs = [
        "channel_manager.cc",
        "channel_manager.h",
        "main.cc",
    ],
    deps = [
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings:str_format",
    ],
)
