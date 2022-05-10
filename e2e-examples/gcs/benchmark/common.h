#ifndef GCS_BENCHMARK_COMMON_H_
#define GCS_BENCHMARK_COMMON_H_

enum class OperationType { None, Read, RandomRead, Write };

const char* ToOperationTypeString(OperationType operationType);

#endif  // GCS_BENCHMARK_COMMON_H_
