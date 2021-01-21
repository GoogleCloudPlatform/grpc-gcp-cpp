workspace(name = "grpc_gcp_cpp")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_google_absl",
    sha256 = "3d74cdc98b42fd4257d91f652575206de195e2c824fcd8d6e6d227f85cb143ef",
    strip_prefix = "abseil-cpp-0f3bb466b868b523cf1dc9b2aaaed65c77b28862",
    url = "https://github.com/abseil/abseil-cpp/archive/0f3bb466b868b523cf1dc9b2aaaed65c77b28862.tar.gz",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "bab63277fe856cea0a53085464c26a905265dc268e8603d5a29b0e1ba69024aa",
    strip_prefix = "grpc-257d0045ab958eb767a3591c88e9d0c2bdf4b916",
    url = "https://github.com/grpc/grpc/archive/257d0045ab958eb767a3591c88e9d0c2bdf4b916.tar.gz",
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
