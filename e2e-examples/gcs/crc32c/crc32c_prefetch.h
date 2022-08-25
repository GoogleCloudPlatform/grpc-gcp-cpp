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

#ifndef CRC32C_CRC32C_PREFETCH_H_
#define CRC32C_CRC32C_PREFETCH_H_

#include <cstddef>
#include <cstdint>

#include "./crc32c_config.h"

#if HAVE_MM_PREFETCH

#if defined(_MSC_VER)
#include <intrin.h>
#else  // !defined(_MSC_VER)
#include <xmmintrin.h>
#endif  // defined(_MSC_VER)

#endif  // HAVE_MM_PREFETCH

namespace crc32c {

// Ask the hardware to prefetch the data at the given address into the L1 cache.
inline void RequestPrefetch(const uint8_t* address) {
#if HAVE_BUILTIN_PREFETCH
  // Clang and GCC implement the __builtin_prefetch non-standard extension,
  // which maps to the best instruction on the target architecture.
  __builtin_prefetch(reinterpret_cast<const char*>(address), 0 /* Read only. */,
                     0 /* No temporal locality. */);
#elif HAVE_MM_PREFETCH
  // Visual Studio doesn't implement __builtin_prefetch, but exposes the
  // PREFETCHNTA instruction via the _mm_prefetch intrinsic.
  _mm_prefetch(reinterpret_cast<const char*>(address), _MM_HINT_NTA);
#else
  // No prefetch support. Silence compiler warnings.
  (void)address;
#endif  // HAVE_BUILTIN_PREFETCH
}

}  // namespace crc32c

#endif  // CRC32C_CRC32C_ROUND_UP_H_
