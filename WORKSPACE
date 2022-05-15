workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "dcf71b9cba8dc0ca9940c4b316a0c796be8fab42b070bb6b7cab62b48f0e66c4",
    strip_prefix = "abseil-cpp-20211102.0",
    url = "https://github.com/abseil/abseil-cpp/archive/refs/tags/20211102.0.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "67423a4cd706ce16a88d1549297023f0f9f0d695a96dd684adc21e67b021f9bc",
    strip_prefix = "grpc-1.46.0",
    url = "https://github.com/grpc/grpc/archive/refs/tags/v1.46.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "523e5986bdb7f001807a96373f1f6a05833cebb98314bc34d0defd9126772285",
    strip_prefix = "googleapis-a0d4c5c2a714a7317714ba6f0066a74ec8a9c10b",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
    url = "https://github.com/googleapis/googleapis/archive/a0d4c5c2a714a7317714ba6f0066a74ec8a9c10b.tar.gz",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "fb62f0e7dc964c5d3cd0d85977b85f3e0e7dce97e9029abf9c32ecc29db07043",
    strip_prefix = "google-cloud-cpp-1.40.1",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v1.40.1.tar.gz",
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
