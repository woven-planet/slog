// Copyright 2022 Woven Planet Holdings
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

// This is for testing the impact of SLOG background thread(s) on other threads.
// The background threads in SLOG can wakeup and steal CPU cycles from other
// running threads, thus causes scheduling latency in the system.
//
// In this setup, we spawn 8 processes and force them to run in 2 cores. SLOG
// context is activated in all these processes, while there is no SLOG
// activities. Each process will burn fix number of cpu cycles periodically, and
// scheduling latency was measured. During that period, the thread can be
// preempted and placed in the ready queue of a CPU. The time to wait on ready
// queue will be captured as queue_time or scheduling latency.
//
// clang-format off
// command:
/*
 $> ./avctl build //slog:slog_system_perf_test && \
    ./bazel-bin/slog/slog_system_perf_test
*/
//
// 2020-11-11
//
// Start 8 processes in 2 cores
// Scheduling latency is calculated in milliseconds
// Process: 0 Samples:5000 AVG: 0.0664472 Median:0  Percentiles 90:0 99:0.099271 99.9:10.1586
// Process: 1 Samples:5000 AVG: 0.038565 Median:0  Percentiles 90:0 99:0 99.9:10.0569
// Process: 2 Samples:5000 AVG: 0.0416313 Median:0  Percentiles 90:0 99:0.003861 99.9:10.059
// Process: 3 Samples:5000 AVG: 0.0867195 Median:0  Percentiles 90:0 99:3.40124 99.9:10.1975
// Process: 4 Samples:5000 AVG: 0.0472081 Median:0  Percentiles 90:0 99:0.004911 99.9:10.0986
// Process: 5 Samples:5000 AVG: 0.0857969 Median:0  Percentiles 90:0 99:3.40527 99.9:10.1792
// Process: 6 Samples:5000 AVG: 0.108975 Median:0  Percentiles 90:0 99:6.7296 99.9:10.1821
// Process: 7 Samples:5000 AVG: 0.118479 Median:0  Percentiles 90:0 99:6.74494 99.9:10.1821
// clang-format on

#include "slog_cc/slog.h"

#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "slog_cc/avs/libs/resource_management/cpu_allocation.h"
#include "slog_cc/common/system/runtime_metrics.h"

namespace slog {

constexpr char kSlogCpuset[] = "slog";
const Duration kWaitToStart = Duration::fromSec(2);
const Duration kWaitBetweenWork = Duration::fromMSec(1);
const constexpr uint64_t kNsInMs = 1000000;
const constexpr uint64_t kIters = 5000;
const constexpr uint64_t kProcesses = 8;

// Burn CPU with the fix number of instructions
// Without optimizations from compiler
static void __attribute__((optimize(0))) burn_some_cpu() {
  double sum = 0.0;
  for (int i = 0; i < 424560; ++i) {
    sum += (static_cast<double>(i) * 123456.7) / 23456.9;
  }
}

void test(uint64_t id) {
  std::vector<int64_t> results(kIters, 0);
  int tid = get_tid();
  int64_t sum = 0;
  for (auto i = 0u; i < kIters; ++i) {
    auto start_metrics = RuntimeMetrics::getRuntimeMetricsInfo(tid);
    burn_some_cpu();
    auto delta = RuntimeMetrics::getRuntimeMetricsInfo(tid) - start_metrics;
    results[i] = delta.queue_time().time_ns();
    sum += results[i];

    sleepFor(kWaitBetweenWork);
  }
  std::sort(results.begin(), results.end());
  const double avg = static_cast<double>(sum) / kIters / kNsInMs;
  const int64_t median = results[static_cast<int>(results.size() * 0.5)];
  const int64_t perc90 = results[static_cast<int>(results.size() * 0.90)];
  const int64_t perc99 = results[static_cast<int>(results.size() * 0.99)];
  const int64_t perc999 = results[static_cast<int>(results.size() * 0.999)];
  std::cout << "Process: " << id << " Samples:" << kIters << " AVG: " << avg
            << " Median:" << static_cast<double>(median) / kNsInMs
            << "  Percentiles 90:" << static_cast<double>(perc90) / kNsInMs
            << " 99:" << static_cast<double>(perc99) / kNsInMs
            << " 99.9:" << static_cast<double>(perc999) / kNsInMs << std::endl;
}

void test_new_process(uint64_t id, slog::avs::CpuAllocator* cpu_alloc) {
  pid_t pid = fork();

  if (pid == 0) {
    // Child
    //
    // create slog context and everything
    SLOG(INFO);
    sleepFor(kWaitToStart);

    // test body
    test(id);

    exit(0);
  } else if (pid > 0) {
    // move this process to a cpuset
    cpu_alloc->assign(pid, kSlogCpuset);
  }
}

}  // namespace slog

using namespace slog;

int main() {
  slog::avs::CpusetMap cpuset_map;
  cpuset_map["non_avs"] = "2-11";
  cpuset_map[kSlogCpuset] = "0-1";
  auto cpu_allocator = slog::avs::createCpuAllocator(cpuset_map);

  std::cout << "Start " << kProcesses << " processes in 2 cores" << std::endl;
  std::cout << "Scheduling latency is calculated in milliseconds" << std::endl;
  for (auto i = 0u; i < kProcesses; ++i) {
    test_new_process(i, cpu_allocator.get());
  }

  // wait for all child processes
  pid_t wpid;
  int status = 0;
  while ((wpid = wait(&status)) > 0) {
  }
  return 0;
}
