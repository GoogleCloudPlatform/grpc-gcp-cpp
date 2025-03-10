// Copyright 2025 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef GCS_BENCHMARK_DUMMY_SERVER_GCS_UTIL_H_
#define GCS_BENCHMARK_DUMMY_SERVER_GCS_UTIL_H_

#include <string>

#include "absl/strings/string_view.h"

class GcsUtil {
 public:
  // Returns the size of object by given bucket and object
  static int64_t GetObjectSize(absl::string_view bucket,
                               absl::string_view object);

  // Returns the random chunk
  static std::string GetObjectDataChunk(int size);
};

#endif  // GCS_BENCHMARK_DUMMY_SERVER_GCS_UTIL_H_
