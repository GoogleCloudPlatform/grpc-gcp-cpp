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
    sha256 = "e034992a0b464042021f6d440f2090acc2422c103a322b0844e3921ccea981dc",
    strip_prefix = "grpc-1.56.0",
    url = "https://github.com/grpc/grpc/archive/v1.56.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "edc901180a3ebdd4b3b3086e7df2ca71f947433ebeb827796447c57491fb334e",
    strip_prefix = "googleapis-a3f983b38c357a1e7a7810d9ad795756b77d4332",
    url = "https://github.com/googleapis/googleapis/archive/a3f983b38c357a1e7a7810d9ad795756b77d4332.tar.gz",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "8cda870803925c62de8716a765e03eb9d34249977e5cdb7d0d20367e997a55e2",
    strip_prefix = "google-cloud-cpp-2.12.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.12.0.tar.gz",
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
