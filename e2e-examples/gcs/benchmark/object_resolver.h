#ifndef GCS_BENCHMARK_OBJECT_RESOLVER_H_
#define GCS_BENCHMARK_OBJECT_RESOLVER_H_

#include <string>

class ObjectResolver {
 public:
  ObjectResolver(std::string object, std::string object_format,
                 int object_start, int object_stop);
  std::string Resolve(int thread_id, int object_id);

 private:
  std::string object_;
  std::string object_format_;
  int object_start_;
  int object_stop_;
};

#endif  // GCS_BENCHMARK_OBJECT_RESOLVER_H_
