workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "5366d7e7fa7ba0d915014d387b66d0d002c03236448e1ba9ef98122c13b35c36",
    strip_prefix = "abseil-cpp-20230125.3",
    url = "https://github.com/abseil/abseil-cpp/archive/20230125.3.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "9cf1a69a921534ac0b760dcbefb900f3c2f735f56070bf0536506913bb5bfd74",
    strip_prefix = "grpc-1.55.0",
    url = "https://github.com/grpc/grpc/archive/v1.55.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "8d0476bf689d25c212744f38646d44a41ff36282722bb92160f536d9377aba3b",
    strip_prefix = "googleapis-117be9dfdf65ff766a794c8b85d5d7480a1fd83d",
    url = "https://github.com/googleapis/googleapis/archive/117be9dfdf65ff766a794c8b85d5d7480a1fd83d.tar.gz",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "7564ec1883240b5a19a5b30c827553e459ca9df90fdc0424ba8b607516ffbf40",
    strip_prefix = "google-cloud-cpp-12600479a816a1a74757ff94f2343774f4e44c4f",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/12600479a816a1a74757ff94f2343774f4e44c4f.tar.gz"
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
