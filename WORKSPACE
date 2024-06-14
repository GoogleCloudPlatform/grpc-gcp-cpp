workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "733726b8c3a6d39a4120d7e45ea8b41a434cdacde401cba500f14236c49b39dc",
    strip_prefix = "abseil-cpp-20240116.2",
    url = "https://github.com/abseil/abseil-cpp/archive/20240116.2.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "c5ad277fc21d4899f0e23f6f0337d8a2190d3c66c57ca868378be7c7bfa59fec",
    strip_prefix = "grpc-1.64.1",
    url = "https://github.com/grpc/grpc/archive/v1.64.1.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "bf49dfee4daecfbcd38bd07cd9b0fd9e303452ed7f54dda56171b500c1234e2e",
    strip_prefix = "googleapis-30717c0b0c9966906880703208a4c820411565c4",
    url = "https://github.com/googleapis/googleapis/archive/30717c0b0c9966906880703208a4c820411565c4.tar.gz",
    build_file = "@google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "google_cloud_cpp",
    sha256 = "6f58213e2af16326392da84cd8a52af78cb80bc47338eb87e87d14c14c0e6bad",
    strip_prefix = "google-cloud-cpp-2.25.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.25.0.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@com_github_grpc_grpc//third_party:zlib.BUILD",
    sha256 = "1525952a0a567581792613a9723333d7f8cc20b87a81f920fb8bc7e3f2251428",
    strip_prefix = "zlib-1.2.13",
    url = "https://github.com/madler/zlib/archive/refs/tags/v1.2.13.tar.gz",
)

load("@google_cloud_cpp//bazel:google_cloud_cpp_deps.bzl", "google_cloud_cpp_deps")

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
