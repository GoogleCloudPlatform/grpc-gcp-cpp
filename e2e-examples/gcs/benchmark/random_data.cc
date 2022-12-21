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

#include "random_data.h"

#include "absl/random/random.h"

namespace {

absl::Cord CreateRandomData(size_t size) {
  std::vector<char> content(size);
  absl::InsecureBitGen gen;
  int* const s = reinterpret_cast<int*>(&(*content.begin()));
  int* const e = reinterpret_cast<int*>(&(*content.rbegin()));
  for (int* c = s; c < e; c += 1) {
    *c = static_cast<char>(absl::Uniform(gen, 0, 256));
  }
  return absl::Cord(absl::string_view(content.data(), content.size()));
}

}  // namespace

absl::Cord GetRandomData(size_t size) {
  // This is optimized to use pregenerated random data
  static absl::Cord random_cord = CreateRandomData(1048576);
  absl::Cord ret;
  while (ret.size() < size) {
    size_t remain = size - ret.size();
    ret.Append(random_cord.Subcord(0, std::min(remain, random_cord.size())));
  }
  return ret;
}
