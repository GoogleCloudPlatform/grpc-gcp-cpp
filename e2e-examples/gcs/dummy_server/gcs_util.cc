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

#include "gcs_util.h"

#include <iostream>
#include <regex>
#include <string>

#include "absl/random/random.h"
#include "absl/strings/ascii.h"
#include "absl/strings/numbers.h"

static absl::string_view GetBasenameWithoutExt(absl::string_view object) {
  absl::string_view name = object;

  // Strips directory part
  size_t path = name.find_last_of('/');
  if (path != name.npos) name.remove_prefix(path + 1);

  // Strips extension part
  size_t ext = name.find_last_of('.');
  if (ext != name.npos) name.remove_suffix(name.size() - ext);

  return name;
}

static int64_t GetUnitMultiplier(absl::string_view unit) {
  const int64_t k = int64_t{1024};
  std::string s = absl::AsciiStrToLower(unit);
  if (s.length() == 0) return 1;
  if (s == "k" || s == "kib") return k;
  if (s == "m" || s == "mib") return k * k;
  if (s == "g" || s == "gib") return k * k * k;
  if (s == "t" || s == "tib") return k * k * k * k;
  return 0;
}

std::regex kSizeRegex(R"((\d+)([A-Za-z]*))");

int64_t GcsUtil::GetObjectSize(absl::string_view bucket,
                               absl::string_view object) {
  std::string name = std::string(GetBasenameWithoutExt(object));

  // Parse the object name into num and unit (e.g. 100KB => 100 and KB)
  std::smatch match;
  if (!std::regex_match(name, match, kSizeRegex)) {
    return -1;
  }

  // Calculate the byte size with num and unit
  std::string num_part = match.str(1);
  std::string unit_part = match.str(2);
  int64_t num;
  if (!absl::SimpleAtoi(num_part, &num)) return -1;
  int64_t multiplier = GetUnitMultiplier(unit_part);
  if (multiplier == 0) return -1;
  return num * multiplier;
}

std::string GcsUtil::GetObjectDataChunk(int size) {
  if (size <= 0) {
    return "";
  }
  std::string chunk(size, '\0');
  absl::InsecureBitGen gen;
  for (int i = 0; i < size; ++i) {
    chunk[i] = static_cast<char>(absl::Uniform(gen, 0, 255));
  }
  return chunk;
}
