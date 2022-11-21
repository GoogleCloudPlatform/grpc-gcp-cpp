workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "91ac87d30cc6d79f9ab974c51874a704de9c2647c40f6932597329a282217ba8",
    strip_prefix = "abseil-cpp-20220623.1",
    url = "https://github.com/abseil/abseil-cpp/archive/20220623.1.tar.gz",
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
    sha256 = "245e198e29c4ec19734cc99ef631daaefbdb874307fc3743e22514ee8bcb36c4",
    strip_prefix = "google-cloud-cpp-2.4.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.4.0.tar.gz",
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
