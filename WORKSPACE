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
    sha256 = "ec19657a677d49af59aa806ec299c070c882986c9fcc022b1c22c2a3caf01bcd",
    strip_prefix = "grpc-1.45.0",
    url = "https://github.com/grpc/grpc/archive/refs/tags/v1.45.0.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "09baebb48905d27f047625e2ed05331ec43562afffe9e90f103e36c0a7a6e40d",
    strip_prefix = "googleapis-cdc9747a73f6af2393ab51490fc3a3c11ff3620f",
    url = "https://github.com/googleapis/googleapis/archive/cdc9747a73f6af2393ab51490fc3a3c11ff3620f.tar.gz",
)

http_archive(
    name = "zlib",
    build_file = "@com_github_grpc_grpc//third_party:zlib.BUILD",
    sha256 = "6d4d6640ca3121620995ee255945161821218752b551a1a180f4215f7d124d45",
    strip_prefix = "zlib-cacf7f1d4e3d44d871b605da3b647f07d718623f",
    url = "https://github.com/madler/zlib/archive/cacf7f1d4e3d44d871b605da3b647f07d718623f.tar.gz",
)

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
