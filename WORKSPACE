workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "9a2b5752d7bfade0bdeee2701de17c9480620f8b237e1964c1b9967c75374906",
    strip_prefix = "abseil-cpp-20230125.2",
    url = "https://github.com/abseil/abseil-cpp/archive/20230125.2.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "9717ffc52120861136e478155c2ff3a9c21740e2244de52fa966f376d7471adf",
    strip_prefix = "grpc-1.53.0",
    url = "https://github.com/grpc/grpc/archive/v1.53.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "40695e9cd84b4e62462803de90eb3e9b2cac8ec36f21f185f6fb5be9fbf6b830",
    strip_prefix = "googleapis-0f35802cb7b4bd752c4adaa9dbdcaaa1ae6bd40a",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
    url = "https://github.com/googleapis/googleapis/archive/0f35802cb7b4bd752c4adaa9dbdcaaa1ae6bd40a.tar.gz",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "640fd1d0e1136b323985b9fd472d52619796c0c76a7b6a4d561ba4132539295c",
    strip_prefix = "google-cloud-cpp-2.9.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.9.0.tar.gz",
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
