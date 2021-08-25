workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "35f22ef5cb286f09954b7cc4c85b5a3f6221c9d4df6b8c4a1e9d399555b366ee",
    strip_prefix = "abseil-cpp-997aaf3a28308eba1b9156aa35ab7bca9688e9f6",
    url = "https://github.com/abseil/abseil-cpp/archive/997aaf3a28308eba1b9156aa35ab7bca9688e9f6.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "1b9b77f894d4d2cca73f534f4bee574588e5ed7f31794715073e4957fb790cee",
    strip_prefix = "grpc-58602e20a3f3e48f24a4114c757099b25b947f7b",
    url = "https://github.com/grpc/grpc/archive/58602e20a3f3e48f24a4114c757099b25b947f7b.tar.gz",
)

http_archive(
    name = "com_google_googleapis",
    sha256 = "09baebb48905d27f047625e2ed05331ec43562afffe9e90f103e36c0a7a6e40d",
    strip_prefix = "googleapis-cdc9747a73f6af2393ab51490fc3a3c11ff3620f",
    urls = [
        "https://github.com/googleapis/googleapis/archive/cdc9747a73f6af2393ab51490fc3a3c11ff3620f.tar.gz",
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
