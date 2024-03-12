workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "3c743204df78366ad2eaf236d6631d83f6bc928d1705dd0000b872e53b73dc6a",
    strip_prefix = "abseil-cpp-20240116.1",
    url = "https://github.com/abseil/abseil-cpp/archive/20240116.1.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "c9f9ae6e4d6f40464ee9958be4068087881ed6aa37e30d0e64d40ed7be39dd01",
    strip_prefix = "grpc-1.62.1",
    url = "https://github.com/grpc/grpc/archive/v1.62.1.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "c88131915fe4510280af0296974ba6c31ccd5d0fa7cbe242baa43d894abbae53",
    strip_prefix = "googleapis-9868a57470a969ffa1d21194a5c05d7a6e4e98cc",
    url = "https://github.com/googleapis/googleapis/archive/9868a57470a969ffa1d21194a5c05d7a6e4e98cc.tar.gz",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "0c68782e57959c82e0c81def805c01460a042c1aae0c2feee905acaa2a2dc9bf",
    strip_prefix = "google-cloud-cpp-2.22.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.22.0.tar.gz",
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
