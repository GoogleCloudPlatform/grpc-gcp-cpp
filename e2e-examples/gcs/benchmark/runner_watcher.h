#ifndef GCS_BENCHMARK_RUNNER_WATCHER_H_
#define GCS_BENCHMARK_RUNNER_WATCHER_H_

#include <grpcpp/impl/codegen/status.h>

#include <string>
#include <vector>

#include "absl/synchronization/mutex.h"
#include "absl/time/time.h"
#include "common.h"

class RunnerWatcher {
 public:
  struct Chunk {
    absl::Time time;
    int64_t bytes;
  };

  struct Operation {
    OperationType type;
    int32_t runner_id;
    int64_t channel_id;
    std::string peer;
    std::string bucket;
    std::string object;
    grpc::Status status;
    int64_t bytes;
    absl::Time time;
    absl::Duration elapsed_time;
    std::vector<Chunk> chunks;
  };

 public:
  RunnerWatcher(size_t warmups = 0, bool verbose = false);

  absl::Time GetStartTime() const;

  void SetStartTime(absl::Time start_time);

  absl::Duration GetDuration() const;

  void SetDuration(absl::Duration duration);

  void NotifyCompleted(OperationType operationType, int32_t runner_id,
                       int64_t channel_id, std::string peer, std::string bucket,
                       std::string object, grpc::Status status, int64_t bytes,
                       absl::Time time, absl::Duration elapsed_time,
                       std::vector<Chunk> chunks);

  std::vector<Operation> GetNonWarmupsOperations() const;

 private:
  size_t warmups_;
  bool verbose_;
  absl::Time start_time_;
  absl::Duration duration_;
  std::vector<Operation> operations_;
  mutable absl::Mutex lock_;
};

#endif  // GCS_BENCHMARK_RUNNER_WATCHER_H_
