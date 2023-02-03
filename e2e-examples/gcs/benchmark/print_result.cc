// Copyright 2022 gRPC authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "print_result.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sys/resource.h>

#include "absl/strings/str_format.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_replace.h"
#include "absl/time/time.h"

constexpr double kMB = 1024.0 * 1024.0;
constexpr double kSevenPercentiles[] = {0.001, 0.01, 0.10, 0.50,
                                        0.90,  0.99, 0.999};

std::string ShortFormatTime(absl::Time time) {
  return absl::FormatTime("%Y-%m-%dT%H:%M:%S", time, absl::LocalTimeZone());
}

std::string FullFormatTime(absl::Time time) {
  return absl::FormatTime("%Y-%m-%dT%H:%M:%E*S", time, absl::LocalTimeZone());
}

std::vector<size_t> GetSortedOperationIndex(
    std::vector<RunnerWatcher::Operation>& operations) {
  std::vector<size_t> indexes;
  for (size_t i = 0, i_max = operations.size(); i < i_max; i++) {
    if (operations[i].status.ok()) {
      indexes.push_back(i);
    }
  }
  // Sorted by throughput in an ascending order
  std::sort(indexes.begin(), indexes.end(), [&operations](size_t a, size_t b) {
    return operations[a].elapsed_time > operations[b].elapsed_time;
  });
  return indexes;
}

struct PeerValue {
  PeerValue() : total_count(0), total_size(0), throughput(0) {}
  std::string peer;
  size_t total_count;
  size_t total_size;
  absl::Duration total_time;
  double throughput;
  std::vector<size_t> indexes;
};

std::vector<PeerValue> GetSortedPeers(
    std::vector<RunnerWatcher::Operation>& operations) {
  std::unordered_map<std::string, PeerValue> peer_map;
  for (size_t i = 0, i_max = operations.size(); i < i_max; i++) {
    const auto& op = operations[i];
    if (operations[i].status.ok()) {
      auto& value = peer_map[op.peer];
      value.total_count += 1;
      value.total_size += op.bytes;
      value.total_time += op.elapsed_time;
      value.indexes.push_back(i);
    }
  }
  std::vector<PeerValue> peers;
  for (auto& i : peer_map) {
    i.second.throughput =
        i.second.total_size / absl::ToDoubleSeconds(i.second.total_time);
    peers.push_back(std::move(i.second));
    peers.back().peer = i.first;
  }
  // Sorted by throughput in an ascending order
  std::sort(peers.begin(), peers.end(),
            [](const PeerValue& a, const PeerValue& b) {
              return a.throughput < b.throughput;
            });
  return peers;
}

void PrintResult(const RunnerWatcher& watcher) {
  auto operations = watcher.GetNonWarmupsOperations();
  if (operations.empty()) {
    return;
  }

  auto elapsed_time = absl::ToDoubleSeconds(watcher.GetNonWarmupsDuration());

  // Total throughput

  int64_t total_bytes = 0;
  for (auto& op : operations) {
    total_bytes += op.bytes;
  }
  std::cout
      << absl::StrFormat(
             "Elapsed: %.1fs Count: %d Bytes: %.1fMB Throughput: %.2fMB/s",
             elapsed_time, operations.size(), total_bytes / kMB,
             total_bytes / kMB / elapsed_time)
      << std::endl;

  // Resource usage

  struct rusage ru;
  if (getrusage(RUSAGE_SELF, &ru) == 0) {
    std::cout << "Resource [ ";
    std::cout << "utime: " << absl::DurationFromTimeval(ru.ru_utime) << " ";
    std::cout << "sime: " << absl::DurationFromTimeval(ru.ru_stime) << " ";
    std::cout << "maxrss: " << ru.ru_maxrss << "KB ";
    std::cout << "]" << std::endl;
  }

  // Percentile for each file

  std::cout << std::endl << "Operation percentiles" << std::endl;
  std::vector<size_t> indexes = GetSortedOperationIndex(operations);
  for (auto p : kSevenPercentiles) {
    size_t index = indexes[size_t(p * indexes.size())];
    auto op = operations[index];
    auto sec = absl::ToDoubleSeconds(op.elapsed_time);
    std::cout << absl::StrFormat(
                     " [p%04.1f] Throughput: %.2fMB/s Time: %.1fs, Chunks: %d "
                     "Peer: %s",
                     p * 100, op.bytes / sec / kMB, sec, op.chunks.size(),
                     op.peer)
              << std::endl;
  }

  // Percentile for each peer

  std::cout << std::endl << "Peer percentiles" << std::endl;
  std::vector<PeerValue> peers = GetSortedPeers(operations);
  for (auto p : kSevenPercentiles) {
    const PeerValue& peer = peers[size_t(p * peers.size())];
    auto indexes = peer.indexes;
    // Sorted by throughput in an ascending order
    std::sort(indexes.begin(), indexes.end(),
              [&operations](size_t a, size_t b) {
                return operations[a].elapsed_time > operations[b].elapsed_time;
              });
    std::vector<double> subt;
    for (auto p2 : kSevenPercentiles) {
      size_t index = indexes[indexes.size() * p2];
      auto op = operations[index];
      subt.push_back(op.bytes / absl::ToDoubleSeconds(op.elapsed_time));
    }
    std::cout << absl::StrFormat(
                     " [p%04.1f] Throughput: %.2fMB/s (p01:%.1f p50:%.1f "
                     "p99:%.1f) Count: %d Peer: %s",
                     p * 100, peer.throughput / kMB, subt[0] / kMB,
                     subt[2] / kMB, subt[4] / kMB, peer.total_count, peer.peer)
              << std::endl;
  }
}

inline bool FileExists(const std::string& name) {
  std::ifstream f(name);
  return f.good();
}

void WriteReport(const RunnerWatcher& watcher, std::string report_file,
                 std::string tag) {
  auto operations = watcher.GetNonWarmupsOperations();
  if (operations.empty()) {
    return;
  }

  bool column_need = !FileExists(report_file);

  std::ofstream f;
  f.open(report_file, std::ios::out | std::ios::app);

  if (column_need) {
    //
    std::vector<std::string> c = {"Time",         "Tag",
                                  "Elapsed",      "Bytes",
                                  "Throughput",   "Success",
                                  "Failure",      "ChannelCount",
                                  "PeerCount",    "MaxChannelPerPeer",
                                  "File-P00.1-T", "File-P01-T",
                                  "File-P10-T",   "File-P50-T",
                                  "File-P90-T",   "File-P99-T",
                                  "File-P99.9-T", "Peer-P00.1-T",
                                  "Peer-P00.1-C", "Peer-P00.1-IP",
                                  "Peer-P01-T",   "Peer-P01-C",
                                  "Peer-P01-IP",  "Peer-P10-T",
                                  "Peer-P10-C",   "Peer-P10-IP",
                                  "Peer-P50-T",   "Peer-P50-C",
                                  "Peer-P50-IP",  "Peer-P90-T",
                                  "Peer-P90-C",   "Peer-P90-IP",
                                  "Peer-P99-T",   "Peer-P99-C",
                                  "Peer-P99-IP",  "Peer-P99.9-T",
                                  "Peer-P99.9-C", "Peer-P99.9-IP"};
    f << absl::StrJoin(c, "\t") << std::endl;
  }

  auto elapsed_time = absl::ToDoubleSeconds(watcher.GetNonWarmupsDuration());

  // Calculates stats

  int64_t total_bytes = 0;
  int32_t success_count = 0;
  int32_t failure_count = 0;
  std::unordered_set<int32_t> channel_id_set;
  std::unordered_map<std::string, std::unordered_set<int32_t>>
      peer_channel_ids_map;
  for (const auto& op : operations) {
    total_bytes += op.bytes;
    if (op.status.ok()) {
      success_count += 1;
    } else {
      failure_count += 1;
    }
    channel_id_set.insert(op.channel_id);
    peer_channel_ids_map[op.peer].insert(op.channel_id);
  }

  int32_t max_channel_per_peer = 0;
  for (const auto& i : peer_channel_ids_map) {
    max_channel_per_peer =
        std::max(max_channel_per_peer, (int32_t)i.second.size());
  }

  std::vector<std::string> v;
  v.push_back(FullFormatTime(watcher.GetStartTime()));
  v.push_back(tag);
  v.push_back(std::to_string(elapsed_time));
  v.push_back(std::to_string(total_bytes));
  v.push_back(std::to_string(total_bytes / elapsed_time));
  v.push_back(std::to_string(success_count));
  v.push_back(std::to_string(failure_count));
  v.push_back(std::to_string(channel_id_set.size()));
  v.push_back(std::to_string(peer_channel_ids_map.size()));
  v.push_back(std::to_string(max_channel_per_peer));

  // Percentile for each file
  std::vector<size_t> indexes = GetSortedOperationIndex(operations);
  for (auto p : kSevenPercentiles) {
    size_t index = indexes[size_t(p * indexes.size())];
    auto op = operations[index];
    auto sec = absl::ToDoubleSeconds(op.elapsed_time);
    v.push_back(std::to_string(op.bytes / sec));
  }

  // Percentile for each peer
  std::vector<PeerValue> peers = GetSortedPeers(operations);
  for (auto p : kSevenPercentiles) {
    const PeerValue& peer = peers[size_t(p * peers.size())];
    auto sec = absl::ToDoubleSeconds(peer.total_time);
    v.push_back(std::to_string(peer.total_size / sec));
    v.push_back(std::to_string(peer.total_count));
    v.push_back(peer.peer);
  }

  f << absl::StrJoin(v, "\t") << std::endl;
}

void WriteData(const RunnerWatcher& watcher, std::string file,
               std::string tag) {
  absl::StrReplaceAll(
      {{"@tag@", tag}, {"@time@", ShortFormatTime(watcher.GetStartTime())}},
      &file);

  std::ofstream f;
  f.open(file, std::ios::out);

  // Total throughput

  f << "{" << std::endl;
  f << absl::StrFormat("\t\"time\": \"%s\",",
                       FullFormatTime(watcher.GetStartTime()))
    << std::endl;
  f << absl::StrFormat("\t\"tag\": \"%s\",", tag) << std::endl;
  f << absl::StrFormat("\t\"duration\": %f,",
                       absl::ToDoubleSeconds(watcher.GetNonWarmupsDuration()))
    << std::endl;

  // All operations

  f << "\t\"operations\": [" << std::endl;
  for (const auto& op : watcher.GetNonWarmupsOperations()) {
    f << "\t\t{" << std::endl;
    f << absl::StrFormat("\t\t\t\"type\": %d,", int(op.type)) << std::endl;
    f << absl::StrFormat("\t\t\t\"runner_id\": %d,", op.runner_id) << std::endl;
    f << absl::StrFormat("\t\t\t\"channel_id\": %d,", op.channel_id)
      << std::endl;
    f << absl::StrFormat("\t\t\t\"peer\": \"%s\",", op.peer) << std::endl;
    f << absl::StrFormat("\t\t\t\"bucket\": \"%s\",", op.bucket) << std::endl;
    f << absl::StrFormat("\t\t\t\"object\": \"%s\",", op.object) << std::endl;
    f << absl::StrFormat("\t\t\t\"status\": %d,", int(op.status.error_code()))
      << std::endl;
    f << absl::StrFormat("\t\t\t\"bytes\": %d,", op.bytes) << std::endl;
    f << absl::StrFormat("\t\t\t\"time\": \"%s\",", FullFormatTime(op.time))
      << std::endl;
    f << absl::StrFormat("\t\t\t\"elapsed_time\": %f,",
                         absl::ToDoubleSeconds(op.elapsed_time))
      << std::endl;
    f << "\t\t\t\"chunks\": [" << std::endl;
    for (const auto& chunk : op.chunks) {
      f << "\t\t\t\t{" << std::endl;
      f << absl::StrFormat("\t\t\t\t\t\"time\": \"%s\",",
                           FullFormatTime(chunk.time))
        << std::endl;
      f << absl::StrFormat("\t\t\t\t\t\"bytes\": %d,", chunk.bytes)
        << std::endl;
      f << "\t\t\t\t}," << std::endl;
    }
    f << "\t\t\t]" << std::endl;
    f << "\t\t}," << std::endl;
  }
  f << "\t]" << std::endl;
  f << "}" << std::endl;
}
