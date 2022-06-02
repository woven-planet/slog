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

#include <benchmark/benchmark.h>

// clang-format off
// Running bazel-bin/slog_cc/benchmarks/clock_benchmark
// Run on (16 X 3187.74 MHz CPU s)
// CPU Caches:
//   L1 Data 32 KiB (x8)
//   L1 Instruction 32 KiB (x8)
//   L2 Unified 1024 KiB (x8)
//   L3 Unified 36608 KiB (x1)
// Load Average: 1.15, 0.95, 0.56
// --------------------------------------------------------------------
// Benchmark                          Time             CPU   Iterations
// --------------------------------------------------------------------
// BM_clock_monotonic_raw          22.8 ns         22.8 ns     30791459
// BM_clock_monotonic              22.7 ns         22.7 ns     30843160
// BM_clock_monotonic_coarse       5.57 ns         5.57 ns    126397539
// clang-format on

// TODO(vsbus): check return value == 0
// CHECK_EQ(0, clock_getres(clock_id, &ts));
// TODO(vsbus): check result != -1
// CHECK_NE(-1, clock_gettime(clock_id, &ts));
// LOG(INFO) << "Clock precision (ns) of " << #clock_id << ": " << ts.tv_nsec;

#define BENCH_CLOCK(clock_id)     \
  struct timespec ts;             \
  static auto once = [&ts] {      \
    clock_getres(clock_id, &ts);  \
    return 0;                     \
  }();                            \
  (void)once;                     \
  int sum = 0;                    \
  for (auto _ : state) {          \
    clock_gettime(clock_id, &ts); \
    sum += ts.tv_nsec;            \
  }                               \
  (void)sum;

static void BM_clock_monotonic_raw(benchmark::State& state) {
  BENCH_CLOCK(CLOCK_MONOTONIC_RAW);
}
BENCHMARK(BM_clock_monotonic_raw);

static void BM_clock_monotonic(benchmark::State& state) {
  BENCH_CLOCK(CLOCK_MONOTONIC);
}
BENCHMARK(BM_clock_monotonic);

static void BM_clock_monotonic_coarse(benchmark::State& state) {
  BENCH_CLOCK(CLOCK_MONOTONIC_COARSE);
}
BENCHMARK(BM_clock_monotonic_coarse);

BENCHMARK_MAIN();
