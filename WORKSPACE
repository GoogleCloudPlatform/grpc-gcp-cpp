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
    sha256 = "8393767af531b2d0549a4c26cf8ba1f665b16c16fb6c9238a7755e45444881dd",
    strip_prefix = "grpc-1.57.0",
    url = "https://github.com/grpc/grpc/archive/v1.57.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "b15cee2e21e26e8ef31c55881af249baf24b1df63f691368d788989c14c18570",
    strip_prefix = "googleapis-6a6fd29a79fe2846001d90d93e79a19fcc303b85",
    url = "https://github.com/googleapis/googleapis/archive/6a6fd29a79fe2846001d90d93e79a19fcc303b85.tar.gz",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "47a5c6beff48625fa1b65b1ddc575247def80c88d29062c66d463172280d3959",
    strip_prefix = "google-cloud-cpp-2.15.1",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.15.1.tar.gz",
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
