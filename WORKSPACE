workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "40cee67604060a7c8794d931538cb55f4d444073e556980c88b6c49bb9b19bb7",
    strip_prefix = "abseil-cpp-20240722.1",
    url = "https://github.com/abseil/abseil-cpp/archive/20240722.1.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "cd256d91781911d46a57506978b3979bfee45d5086a1b6668a3ae19c5e77f8dc",
    strip_prefix = "grpc-1.69.0",
    url = "https://github.com/grpc/grpc/archive/v1.69.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "5a9450cf1ad1187c82a2b5cdff53e4d584b6d45292b5f71b504083acb03ad7d0",
    strip_prefix = "googleapis-280725e991516d4a0f136268faf5aa6d32d21b54",
    url = "https://github.com/googleapis/googleapis/archive/280725e991516d4a0f136268faf5aa6d32d21b54.tar.gz",
    build_file = "@google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "google_cloud_cpp",
    sha256 = "81ea28cf9e5bb032d356b0187409f30b1035f8ea5b530675ea248c8a6c0070aa",
    strip_prefix = "google-cloud-cpp-2.35.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.35.0.tar.gz",
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

load("@io_opentelemetry_cpp//bazel:repository.bzl", "opentelemetry_cpp_deps")

opentelemetry_cpp_deps()

load("@io_opentelemetry_cpp//bazel:extra_deps.bzl", "opentelemetry_extra_deps")

opentelemetry_extra_deps()
