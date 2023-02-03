workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "3ea49a7d97421b88a8c48a0de16c16048e17725c7ec0f1d3ea2683a2a75adc21",
    strip_prefix = "abseil-cpp-20230125.0",
    url = "https://github.com/abseil/abseil-cpp/archive/20230125.0.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "7f42363711eb483a0501239fd5522467b31d8fe98d70d7867c6ca7b52440d828",
    strip_prefix = "grpc-1.51.0",
    url = "https://github.com/grpc/grpc/archive/v1.51.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "d55bdab0dcdbad8bf3c2cf913a26e1d02780e6cea2ba789009959e58e966bbef",
    strip_prefix = "googleapis-b6b751420f6b055d9445e81d35f90c0a2ee16b05",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
    url = "https://github.com/googleapis/googleapis/archive/b6b751420f6b055d9445e81d35f90c0a2ee16b05.tar.gz",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "23b8ad92efed546139550853bd1ead2b9dbd93320c8e793c29fcb3858a0c2f6c",
    strip_prefix = "google-cloud-cpp-2.7.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.7.0.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@com_github_grpc_grpc//third_party:zlib.BUILD",
    sha256 = "1525952a0a567581792613a9723333d7f8cc20b87a81f920fb8bc7e3f2251428",
    strip_prefix = "zlib-1.2.13",
    url = "https://github.com/madler/zlib/archive/refs/tags/v1.2.13.tar.gz",
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
