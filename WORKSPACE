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
    sha256 = "0ff2e0a6abf195cf23b4ce808570bcbb2ff4b5bee453af0b45afd496e661f2c0",
    strip_prefix = "grpc-1.65.2",
    url = "https://github.com/grpc/grpc/archive/v1.65.2.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "33c62c03f9479728bdaa1a6553d8b35fa273d010706c75ea85cd8dfe1687586c",
    strip_prefix = "googleapis-622e10a1e8b2b6908e0ac7448d347a0c1b4130de",
    url = "https://github.com/googleapis/googleapis/archive/622e10a1e8b2b6908e0ac7448d347a0c1b4130de.tar.gz",
    build_file = "@google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "google_cloud_cpp",
    sha256 = "91cd0552c68d85c0c07f9500771367034ea78f6814603275dcf8664472f8f37f",
    strip_prefix = "google-cloud-cpp-2.26.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.26.0.tar.gz",
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
