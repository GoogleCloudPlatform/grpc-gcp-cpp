#include "runner_watcher.h"

RunnerWatcher::RunnerWatcher(bool verbose) { verbose_ = verbose; }

absl::Time RunnerWatcher::GetStartTime() const { return start_time_; }

void RunnerWatcher::SetStartTime(absl::Time start_time) {
  start_time_ = start_time;
}

absl::Duration RunnerWatcher::GetDuration() const { return duration_; }

void RunnerWatcher::SetDuration(absl::Duration duration) {
  duration_ = duration;
}

void RunnerWatcher::NotifyCompleted(OperationType operationType,
                                    int32_t runner_id, int64_t channel_id,
                                    std::string peer, std::string bucket,
                                    std::string object, grpc::Status status,
                                    int64_t bytes, absl::Time time,
                                    absl::Duration elapsed_time,
                                    std::vector<Chunk> chunks) {
  Operation op;
  op.type = operationType;
  op.runner_id = runner_id;
  op.channel_id = channel_id;
  op.peer = peer;
  op.bucket = bucket;
  op.object = object;
  op.status = status;
  op.bytes = bytes;
  op.time = time;
  op.elapsed_time = elapsed_time;
  op.chunks = std::move(chunks);

  // Insert records
  int ord;
  {
    absl::MutexLock l(&lock_);
    operation_.push_back(std::move(op));
    ord = operation_.size();
  }

  if (verbose_) {
    auto sec = absl::ToDoubleSeconds(op.elapsed_time);
    printf(
        "### %sCompleted: ord=%d peer=%s bucket=%s object=%s bytes=%lld "
        "elapsed=%.2fs\n",
        ToOperationTypeString(operationType), ord, peer.c_str(), bucket.c_str(),
        object.c_str(), (long long)bytes, sec);
    fflush(stdout);
  }
}

const std::vector<RunnerWatcher::Operation>& RunnerWatcher::GetOperations()
    const {
  return operation_;
}
