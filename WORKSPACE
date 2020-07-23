workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "f368a8476f4e2e0eccf8a7318b98dafbe30b2600f4e3cf52636e5eb145aba06a",
    strip_prefix = "abseil-cpp-df3ea785d8c30a9503321a3d35ee7d35808f190d",
    url = "https://github.com/abseil/abseil-cpp/archive/df3ea785d8c30a9503321a3d35ee7d35808f190d.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "68b61f80d85d026068e0a35a56a19248480b5eaf785d8f71b1ba5d2c49739b85",
    strip_prefix = "grpc-786ebf69aa73eba005634e356ae78081133c855f",
    url = "https://github.com/grpc/grpc/archive/786ebf69aa73eba005634e356ae78081133c855f.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "136e333508337030e112afe4974e2e595a8f4751e9a1aefc598b7aa7282740db",
    strip_prefix = "googleapis-a9a9950dc472e7036e05df8dd29597cd19235649",
    urls = [
        "https://github.com/googleapis/googleapis/archive/a9a9950dc472e7036e05df8dd29597cd19235649.tar.gz",
    ],
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
)

load('@com_github_grpc_grpc//bazel:grpc_deps.bzl', 'grpc_deps')
grpc_deps()

load('@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl', 'grpc_extra_deps')
grpc_extra_deps()
