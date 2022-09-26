workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "4208129b49006089ba1d6710845a45e31c59b0ab6bff9e5788a87f55c5abd602",
    strip_prefix = "abseil-cpp-20220623.0",
    url = "https://github.com/abseil/abseil-cpp/archive/20220623.0.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "5071b630e2a14707ad060636990f1b25b0984bd168c7ea1ab95e48a3bdc0081f",
    strip_prefix = "grpc-1.49.1",
    url = "https://github.com/grpc/grpc/archive/v1.49.1.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "c164c4cbafd829e11aa958d47c63e24daa939638f04d4adb91dfa2c58e49d2c6",
    strip_prefix = "googleapis-a0093c4b69a84d0021a247f21ab5f5f7b6dfe9d2",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
    url = "https://github.com/googleapis/googleapis/archive/a0093c4b69a84d0021a247f21ab5f5f7b6dfe9d2.tar.gz",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "351b7f332cc14b0d384bb24c6ea5a6ad5926b87c1bf25e33e514cbe515659237",
    strip_prefix = "google-cloud-cpp-2.2.1",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.2.1.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@com_github_grpc_grpc//third_party:zlib.BUILD",
    sha256 = "d8688496ea40fb61787500e863cc63c9afcbc524468cedeb478068924eb54932",
    strip_prefix = "zlib-1.2.12",
    url = "https://github.com/madler/zlib/archive/refs/tags/v1.2.12.tar.gz",
)

load("@com_github_googleapis_google_cloud_cpp//bazel:google_cloud_cpp_deps.bzl", "google_cloud_cpp_deps")

google_cloud_cpp_deps()

load("@com_google_googleapis//:repository_rules.bzl", "switched_rules_by_language")

switched_rules_by_language(
    name = "com_google_googleapis_imports",
    cc = True,
    grpc = True,
)

load('@com_github_grpc_grpc//bazel:grpc_deps.bzl', 'grpc_deps')

grpc_deps()

load('@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl', 'grpc_extra_deps')

grpc_extra_deps()
