#include "parameters.h"

#include <iostream>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"

ABSL_FLAG(std::string, client, "grpc",
          "Client (grpc, gcscpp-json, gcscpp-grpc)");
ABSL_FLAG(std::string, operation, "read",
          "Operation type (read, random-read, write)");
ABSL_FLAG(std::string, bucket, "gcs-grpc-team-veblush1",
          "Bucket to fetch object from");
ABSL_FLAG(std::string, object, "1G.txt", "Object to download");
ABSL_FLAG(std::string, object_format, "",
          "Format std::string to resolve the object (format: {t}=thread-id, "
          "{o}=object-id)");
ABSL_FLAG(
    int, object_start, 0,
    "An integer number specifying at which position to start. Default is 0");
ABSL_FLAG(int, object_stop, 0,
          "An integer number specifying at which position to end.");
ABSL_FLAG(int64_t, chunk_size, -1, "Chunk size for random-read and write");
ABSL_FLAG(int64_t, read_offset, -1, "Read offset for read");
ABSL_FLAG(int64_t, read_limit, -1, "Read limit for read");
ABSL_FLAG(int64_t, write_size, 0, "Write size");
ABSL_FLAG(absl::Duration, timeout, absl::InfiniteDuration(),
          "Timeout for the call. (Default: none)");
ABSL_FLAG(int, runs, 1, "The number of times to run the download");
ABSL_FLAG(int, warmups, 0,
          "The number of warm-up calls to be excluded for the report");
ABSL_FLAG(int, threads, 1, "The number of threads running downloding objects");
ABSL_FLAG(bool, crc32c, false, "Check CRC32C check for received content");
ABSL_FLAG(bool, resumable, false, "Use resumable-write for writing");
ABSL_FLAG(bool, trying, false, "Keep trying the same operation if failed");
ABSL_FLAG(bool, verbose, false, "Show debug output and progress updates");

ABSL_FLAG(std::string, report_tag, "",
          "The user-defined tag to be inserted in the report");
ABSL_FLAG(std::string, report_file, "",
          "The file to append the line for the run");
ABSL_FLAG(std::string, data_file, "", "The data file to dump the all data");

ABSL_FLAG(std::string, host, "dns:///storage.googleapis.com:443",
          "Host to reach");
ABSL_FLAG(std::string, access_token, "", "Access token for auth");
ABSL_FLAG(std::string, network, "default", "Network path (default, cfe, dp)");
ABSL_FLAG(bool, rr, false,
          "Use round_robin grpclb policy (otherwise pick_first)");
ABSL_FLAG(bool, td, false, "Use Traffic Director");
ABSL_FLAG(std::string, cpolicy, "perthread",
          "Channel Policy (perthread, percall, const, pool, bpool, spool)");
ABSL_FLAG(
    int, carg, 0,
    "Parameter for cpolicy (e.g. pool uses this as the number of channels)");
ABSL_FLAG(int, ctest, 0, "Test to get a list of peers from grpclb");

const char* ToOperationTypeString(OperationType operationType) {
  switch (operationType) {
    case OperationType::Read:
      return "Read";
    case OperationType::RandomRead:
      return "Random-Read";
    case OperationType::Write:
      return "Write";
    default:
      return "None";
  }
}

absl::optional<Parameters> GetParameters() {
  Parameters p;
  p.client = absl::GetFlag(FLAGS_client);
  p.operation = absl::GetFlag(FLAGS_operation);
  if (p.operation == "read") {
    p.operation_type = OperationType::Read;
  } else if (p.operation == "random-read") {
    p.operation_type = OperationType::RandomRead;
  } else if (p.operation == "write") {
    p.operation_type = OperationType::Write;
  } else {
    std::cerr << "Invalid operation: " << p.operation << std::endl;
    return {};
  }
  p.bucket = absl::GetFlag(FLAGS_bucket);
  p.object = absl::GetFlag(FLAGS_object);
  p.object_format = absl::GetFlag(FLAGS_object_format);
  p.object_start = absl::GetFlag(FLAGS_object_start);
  p.object_stop = absl::GetFlag(FLAGS_object_stop);
  p.chunk_size = absl::GetFlag(FLAGS_chunk_size);
  p.read_offset = absl::GetFlag(FLAGS_read_offset);
  p.read_limit = absl::GetFlag(FLAGS_read_limit);
  p.write_size = absl::GetFlag(FLAGS_write_size);
  p.timeout = absl::GetFlag(FLAGS_timeout);
  p.runs = absl::GetFlag(FLAGS_runs);
  p.warmups = absl::GetFlag(FLAGS_warmups);
  p.threads = absl::GetFlag(FLAGS_threads);
  p.crc32c = absl::GetFlag(FLAGS_crc32c);
  p.resumable = absl::GetFlag(FLAGS_resumable);
  p.trying = absl::GetFlag(FLAGS_trying);
  p.verbose = absl::GetFlag(FLAGS_verbose);
  p.report_tag = absl::GetFlag(FLAGS_report_tag);
  p.report_file = absl::GetFlag(FLAGS_report_file);
  p.data_file = absl::GetFlag(FLAGS_data_file);
  p.host = absl::GetFlag(FLAGS_host);
  p.access_token = absl::GetFlag(FLAGS_access_token);
  p.network = absl::GetFlag(FLAGS_network);
  p.rr = absl::GetFlag(FLAGS_rr);
  p.td = absl::GetFlag(FLAGS_td);
  p.cpolicy = absl::GetFlag(FLAGS_cpolicy);
  if (p.cpolicy != "perthread" && p.cpolicy != "percall" &&
      p.cpolicy != "const" && p.cpolicy != "pool" && p.cpolicy != "bpool" &&
      p.cpolicy != "spool") {
    std::cerr << "Invalid cpolicy: " << p.cpolicy << std::endl;
    return {};
  }
  p.carg = absl::GetFlag(FLAGS_carg);
  p.ctest = absl::GetFlag(FLAGS_ctest);
  return p;
}
