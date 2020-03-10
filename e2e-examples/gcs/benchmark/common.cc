#include "common.h"

const char* ToOperationTypeString(OperationType operationType) {
  switch (operationType) {
    case OperationType::Read:
      return "Read";
    case OperationType::Write:
      return "Write";
    default:
      return "None";
  }
}
