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

#include "work_queue.h"

WorkQueue::WorkQueue(int thread_count, int work_count_per_thread,
                     bool work_stealing_enabled)
    : thread_count_(thread_count),
      work_count_per_thread_(work_count_per_thread),
      work_stealing_enabled_(work_stealing_enabled) {
  thread_works_.assign(thread_count, 0);
}

std::tuple<int, int> WorkQueue::pop(int thread_id) {
  if (thread_id < 1 || thread_id > thread_count_) {
    return std::make_tuple(0, 0);
  }

  absl::MutexLock l(&mu_);

  // Pop the next work if the current thread stil has remaining works
  int& cur_thread_work = thread_works_[thread_id - 1];
  if (cur_thread_work < work_count_per_thread_) {
    cur_thread_work += 1;
    return std::make_tuple(thread_id, cur_thread_work);
  }

  // Try to steal a job from other threads if it's enabled
  if (work_stealing_enabled_) {
    for (int t = 0; t < thread_count_; t++) {
      int& t_work = thread_works_[t];
      if (t_work < work_count_per_thread_) {
        t_work += 1;
        return std::make_tuple(t + 1, t_work);
      }
    }
  }

  // Otherwise nothing to do
  return std::make_tuple(0, 0);
}