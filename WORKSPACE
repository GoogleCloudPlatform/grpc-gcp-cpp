workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "987ce98f02eefbaf930d6e38ab16aa05737234d7afbab2d5c4ea7adbe50c28ed",
    strip_prefix = "abseil-cpp-20230802.1",
    url = "https://github.com/abseil/abseil-cpp/archive/20230802.1.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "a3a65f0129c4922c5d7f4c11dcd40083a12ca54076fd3a927bcd63c53b7e44a5",
    strip_prefix = "grpc-1.59.2",
    url = "https://github.com/grpc/grpc/archive/v1.59.2.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "f192fc1ff3bccf8a02fd1f9115d01d154aab875da43276d55989fca65fbb07c8",
    strip_prefix = "googleapis-4a94b9e4403f958f65077f43863302c4ba4597da",
    url = "https://github.com/googleapis/googleapis/archive/4a94b9e4403f958f65077f43863302c4ba4597da.tar.gz",
    build_file = "@com_github_googleapis_google_cloud_cpp//bazel:googleapis.BUILD",
)

http_archive(
    name = "com_github_googleapis_google_cloud_cpp",
    sha256 = "374efd3d85144f373cc8efcaea30712c0b1afacfc35574e2f4fd512360fd3d01",
    strip_prefix = "google-cloud-cpp-2.18.0",
    url = "https://github.com/googleapis/google-cloud-cpp/archive/v2.18.0.tar.gz",
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
