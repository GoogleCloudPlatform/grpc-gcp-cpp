workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "4208129b49006089ba1d6710845a45e31c59b0ab6bff9e5788a87f55c5abd602",
    strip_prefix = "abseil-cpp-20220623.0",
    url = "https://github.com/abseil/abseil-cpp/archive/20220623.0.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "9b1f348b15a7637f5191e4e673194549384f2eccf01fcef7cc1515864d71b424",
    strip_prefix = "grpc-1.48.0",
    url = "https://github.com/grpc/grpc/archive/refs/tags/v1.48.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "b0989259d95eddabf9684ecb43d3e558e5a18acd3d4a78a372f8152af95bbe71",
    strip_prefix = "googleapis-ff8159ec8694fe07a10142c8c8bf9f740bf0a93e",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
    url = "https://github.com/googleapis/googleapis/archive/ff8159ec8694fe07a10142c8c8bf9f740bf0a93e.tar.gz",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "a3951678c2413574fa21bea52d4653a508cde12c06f59042a5d04cc23ff33543",
    strip_prefix = "google-cloud-cpp-2.0.1",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.0.1.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@com_github_grpc_grpc//third_party:zlib.BUILD",
    sha256 = "6d4d6640ca3121620995ee255945161821218752b551a1a180f4215f7d124d45",
    strip_prefix = "zlib-cacf7f1d4e3d44d871b605da3b647f07d718623f",
    url = "https://github.com/madler/zlib/archive/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
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
