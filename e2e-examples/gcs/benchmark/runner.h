#ifndef GCS_BENCHMARK_RUNNER_H_
#define GCS_BENCHMARK_RUNNER_H_

#include <memory>
#include <string>
#include <vector>

#include "absl/time/time.h"
#include "absl/types/optional.h"

#include "channel_policy.h"
#include "object_resolver.h"
#include "runner_watcher.h"

class Runner {
 public:
  struct Parameter {
    OperationType operation_type;
    int id;
    std::string bucket;
    std::string object;
    std::string object_format;
    int object_start;
    int object_stop;
    int64_t chunk_size;
    int64_t read_offset;
    int64_t read_limit;
    int64_t write_size;
    absl::Duration timeout;
    int runs;
    bool verbose;
    bool check_crc32c;
    bool resumable_write;
    bool keep_trying;
    std::shared_ptr<StorageStubProvider> storage_stub_provider;
  };

  Runner(Parameter parameter, std::shared_ptr<RunnerWatcher> watcher);

  void Run();

  absl::optional<bool> GetReturnCode();

 private:
  bool RunRead();
  bool RunRandomRead();
  bool RunWrite();

 private:
  Parameter parameter_;
  ObjectResolver objectResolver_;
  std::shared_ptr<RunnerWatcher> watcher_;
  bool return_code_;
};

#endif  // GCS_BENCHMARK_RUNNER_H_
