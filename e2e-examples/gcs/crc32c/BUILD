package(
    default_visibility = ["//visibility:public"],
)

# Bazel version of https://github.com/google/crc32c
cc_library(
    name = "crc32c",
    hdrs = [
        "crc32c.h", 
    ],
    copts = [
        "-msse4.2"
    ],
    srcs = [
        "crc32c_config.h",
        "crc32c_internal.h",
        "crc32c_portable.cc",
        "crc32c_prefetch.h",
        "crc32c_read_le.h",
        "crc32c_round_up.h",
        "crc32c_sse42_check.h",
        "crc32c_sse42.cc",
        "crc32c_sse42.h",
        "crc32c.cc",
    ]
)
