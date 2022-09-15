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

#ifndef GCS_BENCHMARK_WORK_QUEUE_H_
#define GCS_BENCHMARK_WORK_QUEUE_H_

#include <tuple>
#include <vector>

#include "absl/synchronization/mutex.h"

class WorkQueue {
 public:
  WorkQueue(int thread_count, int work_count_per_thread,
            bool work_stealing_enabled);

  // Returns tuple<thread_id, work_id> if there is job.
  // Otherwise it returns tuple<0, 0>
  std::tuple<int, int> pop(int thread_id);

 private:
  absl::Mutex mu_;
  int thread_count_;
  int work_count_per_thread_;
  bool work_stealing_enabled_;
  std::vector<int> thread_works_;
};

#endif  // GCS_BENCHMARK_WORK_QUEUE_H_
