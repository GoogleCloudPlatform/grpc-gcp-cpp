#include "gcscpp_runner.h"

GcscppRunner::GcscppRunner(Parameters parameters,
                           std::shared_ptr<RunnerWatcher> watcher)
    : parameters_(parameters),
      object_resolver_(parameters_.object, parameters_.object_format,
                       parameters_.object_start, parameters_.object_stop),
      watcher_(watcher) {}

bool GcscppRunner::Run() { return false; }

bool GcscppRunner::DoOperation(int thread_id) {
  switch (parameters_.operation_type) {
    case OperationType::Read:
      return DoRead(thread_id);
    case OperationType::RandomRead:
      return DoRandomRead(thread_id);
    case OperationType::Write:
      return DoWrite(thread_id);
    default:
      return false;
  }
}

bool GcscppRunner::DoRead(int thread_id) { return true; }

bool GcscppRunner::DoRandomRead(int thread_id) { return true; }

bool GcscppRunner::DoWrite(int thread_id) { return true; }
