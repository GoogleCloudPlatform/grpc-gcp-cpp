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

// Copyright 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef CRC32C_CRC32C_INTERNAL_H_
#define CRC32C_CRC32C_INTERNAL_H_

// Internal functions that may change between releases.

#include <cstddef>
#include <cstdint>

namespace crc32c {

// Un-accelerated implementation that works on all CPUs.
uint32_t ExtendPortable(uint32_t crc, const uint8_t* data, size_t count);

// CRCs are pre- and post- conditioned by xoring with all ones.
static constexpr const uint32_t kCRC32Xor = static_cast<uint32_t>(0xffffffffU);

}  // namespace crc32c

#endif  // CRC32C_CRC32C_INTERNAL_H_
