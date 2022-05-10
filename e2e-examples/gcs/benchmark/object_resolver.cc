#include "object_resolver.h"

#include "absl/strings/str_cat.h"
#include "absl/strings/str_replace.h"

ObjectResolver::ObjectResolver(std::string object, std::string object_format,
                               int object_start, int object_stop)
    : object_(object),
      object_format_(object_format),
      object_start_(object_start),
      object_stop_(object_stop) {}

std::string ObjectResolver::Resolve(int thread_id, int object_id) {
  if (object_format_.empty()) {
    return object_;
  }

  int oid = object_stop_ == 0
                ? object_id
                : (object_id % (object_stop_ - object_start_)) + object_start_;
  return absl::StrReplaceAll(object_format_, {{"{t}", absl::StrCat(thread_id)},
                                              {"{o}", absl::StrCat(oid)}});
}
