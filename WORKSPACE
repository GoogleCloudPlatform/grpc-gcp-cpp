workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "f50e5ac311a81382da7fa75b97310e4b9006474f9560ac46f54a9967f07d4ae3",
    strip_prefix = "abseil-cpp-20240722.0",
    url = "https://github.com/abseil/abseil-cpp/archive/20240722.0.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "79ed4ab72fa9589b20f8b0b76c16e353e4cfec1d773d33afad605d97b5682c61",
    strip_prefix = "grpc-1.66.1",
    url = "https://github.com/grpc/grpc/archive/v1.66.1.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "435ae615a71ab2718d5e4b348b2442d896a03f9c1ea98182dc2ea927f383acd2",
    strip_prefix = "googleapis-6a474b31c53cc1797710206824a17b364a835d2d",
    url = "https://github.com/googleapis/googleapis/archive/6a474b31c53cc1797710206824a17b364a835d2d.tar.gz",
    build_file = "@google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "google_cloud_cpp",
    sha256 = "758e1eca8186b962516c0659b34ce1768ba1c9769cfd998c5bbffb084ad901ff",
    strip_prefix = "google-cloud-cpp-2.29.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.29.0.tar.gz",
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
