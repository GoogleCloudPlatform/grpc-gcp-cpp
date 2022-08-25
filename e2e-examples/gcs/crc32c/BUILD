# Copyright 2022 gRPC authors.
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

package(
    default_visibility = ["//visibility:public"],
)

# Bazel version of https://github.com/google/crc32c
cc_library(
    name = "crc32c",
    hdrs = [
        "crc32c.h", 
    ],
    copts = [
        "-msse4.2"
    ],
    srcs = [
        "crc32c_config.h",
        "crc32c_internal.h",
        "crc32c_portable.cc",
        "crc32c_prefetch.h",
        "crc32c_read_le.h",
        "crc32c_round_up.h",
        "crc32c_sse42_check.h",
        "crc32c_sse42.cc",
        "crc32c_sse42.h",
        "crc32c.cc",
    ]
)
