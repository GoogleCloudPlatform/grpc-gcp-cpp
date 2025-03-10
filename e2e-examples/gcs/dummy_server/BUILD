# Copyright 2025 gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cc_library(
    name = "gcs_util",
    hdrs = [
        "gcs_util.h",
    ],
    srcs = [
        "gcs_util.cc",
    ],
    deps = [
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/random",
    ],
)

cc_binary(
    name = "dummy_server",
    srcs = [
        "main.cc",
    ],
    deps = [
        "gcs_util",
        "@com_github_grpc_grpc//:grpc++",
        "@com_github_grpc_grpc//:grpc++_reflection",
        "@com_github_grpc_grpc//:grpcpp_admin",
        "@com_google_absl//absl/flags:flag",
        "@com_google_absl//absl/flags:parse",
        "@com_google_absl//absl/strings",
        "@com_google_googleapis//google/storage/v2:storage_cc_grpc",
    ],
)
