#ifndef GCS_BENCHMARK_PRINT_RESULT_H
#define GCS_BENCHMARK_PRINT_RESULT_H

#include <memory>

#include "absl/strings/string_view.h"
#include "absl/time/time.h"

#include "runner_watcher.h"

void PrintResult(const RunnerWatcher& watcher);

void WriteReport(const RunnerWatcher& watcher, std::string file, std::string tag);

void WriteData(const RunnerWatcher& watcher, std::string file, std::string tag);

#endif  // GCS_BENCHMARK_PRINT_RESULT_H
