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

#ifndef GCS_BENCHMARK_OBJECT_RESOLVER_H_
#define GCS_BENCHMARK_OBJECT_RESOLVER_H_

#include <string>

class ObjectResolver {
 public:
  ObjectResolver(std::string object, std::string object_format,
                 int object_start, int object_stop);
  std::string Resolve(int thread_id, int object_id);

 private:
  std::string object_;
  std::string object_format_;
  int object_start_;
  int object_stop_;
};

#endif  // GCS_BENCHMARK_OBJECT_RESOLVER_H_
