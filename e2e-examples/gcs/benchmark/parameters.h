// Copyright 2022 gRPC authors.
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

#ifndef GCS_BENCHMARK_PARAMETERS_
#define GCS_BENCHMARK_PARAMETERS_

#include <cstdint>
#include <string>

#include "absl/time/time.h"
#include "absl/types/optional.h"

enum class OperationType { None, Read, RandomRead, Write };

const char* ToOperationTypeString(OperationType operationType);

struct Parameters {
  std::string client;
  std::string operation;
  OperationType operation_type;
  std::string bucket;
  std::string object;
  std::string object_format;
  int object_start;
  int object_stop;
  int64_t chunk_size;
  int64_t read_offset;
  int64_t read_limit;
  int64_t write_size;
  absl::Duration timeout;
  int runs;
  int warmups;
  int threads;
  bool crc32c;
  bool resumable;
  bool trying;
  bool wait_threads;
  bool verbose;

  std::string report_tag;
  std::string report_file;
  std::string data_file;

  std::string host;
  std::string target_api_version;
  std::string access_token;
  std::string network;
  bool rr;
  bool td;
  std::string cpolicy;
  int carg;
  int ctest;
};

absl::optional<Parameters> GetParameters();

#endif  // GCS_BENCHMARK_PARAMETERS_
