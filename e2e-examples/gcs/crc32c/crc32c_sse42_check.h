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

#ifndef CRC32C_CRC32C_SSE42_CHECK_H_
#define CRC32C_CRC32C_SSE42_CHECK_H_

// X86-specific code checking the availability of SSE4.2 instructions.

#include <cstddef>
#include <cstdint>

#include "./crc32c_config.h"

#if HAVE_SSE42 && (defined(_M_X64) || defined(__x86_64__))

// If the compiler supports SSE4.2, it definitely supports X86.

#if defined(_MSC_VER)
#include <intrin.h>

namespace crc32c {

inline bool CanUseSse42() {
  int cpu_info[4];
  __cpuid(cpu_info, 1);
  return (cpu_info[2] & (1 << 20)) != 0;
}

}  // namespace crc32c

#else  // !defined(_MSC_VER)
#include <cpuid.h>

namespace crc32c {

inline bool CanUseSse42() {
  unsigned int eax, ebx, ecx, edx;
  return __get_cpuid(1, &eax, &ebx, &ecx, &edx) && ((ecx & (1 << 20)) != 0);
}

}  // namespace crc32c

#endif  // defined(_MSC_VER)

#endif  // HAVE_SSE42 && (defined(_M_X64) || defined(__x86_64__))

#endif  // CRC32C_CRC32C_SSE42_CHECK_H_
