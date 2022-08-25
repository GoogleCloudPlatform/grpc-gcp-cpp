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

#include "object_resolver.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_replace.h"

ObjectResolver::ObjectResolver(std::string object, std::string object_format,
                               int object_start, int object_stop)
    : object_(object),
      object_format_(object_format),
      object_start_(object_start),
      object_stop_(object_stop) {}

std::string ObjectResolver::Resolve(int thread_id, int object_id) {
  if (object_format_.empty()) {
    return object_;
  }

  int oid = object_stop_ == 0
                ? object_id + object_start_
                : (object_id % (object_stop_ - object_start_)) + object_start_;
  return absl::StrReplaceAll(object_format_, {{"{t}", absl::StrCat(thread_id)},
                                              {"{o}", absl::StrCat(oid)}});
}
