#include <benchmark/benchmark.h>

#include <glog/logging.h>

// clang-format off
// -----------------------------------------------------------------
// Benchmark                          Time           CPU Iterations
// -----------------------------------------------------------------
// I0111 17:24:12.616287    15 clock_benchmark.cc:41] Clock precision (ns) of CLOCK_MONOTONIC_RAW: 1
// BM_clock_monotonic_raw            69 ns         69 ns   10160674
// I0111 17:24:13.392776    15 clock_benchmark.cc:60] Clock precision (ns) of CLOCK_MONOTONIC: 1
// BM_clock_monotonic                14 ns         14 ns   48555769
// I0111 17:24:14.250128    15 clock_benchmark.cc:79] Clock precision (ns) of
// CLOCK_MONOTONIC_COARSE: 4000000 BM_clock_monotonic_coarse          3 ns          3 ns  201819204
// clang-format om

namespace avsoftware {

#define BENCH_CLOCK(clock_id)                                                   \
  struct timespec ts;                                                           \
  static auto once = [&ts] {                                                    \
    CHECK_EQ(0, clock_getres(clock_id, &ts));                                   \
    LOG(INFO) << "Clock precision (ns) of " << #clock_id << ": " << ts.tv_nsec; \
    return 0;                                                                   \
  }();                                                                          \
  (void)once;                                                                   \
  int sum = 0;                                                                  \
  for (auto _ : state) {                                                        \
    CHECK_NE(-1, clock_gettime(clock_id, &ts));                                 \
    sum += ts.tv_nsec;                                                          \
  }                                                                             \
  VLOG(5) << sum;

static void BM_clock_monotonic_raw(benchmark::State& state) { BENCH_CLOCK(CLOCK_MONOTONIC_RAW); }
BENCHMARK(BM_clock_monotonic_raw);

static void BM_clock_monotonic(benchmark::State& state) { BENCH_CLOCK(CLOCK_MONOTONIC); }
BENCHMARK(BM_clock_monotonic);

static void BM_clock_monotonic_coarse(benchmark::State& state) {
  BENCH_CLOCK(CLOCK_MONOTONIC_COARSE);
}
BENCHMARK(BM_clock_monotonic_coarse);

#undef BENCH_CLOCK

}  // namespace avsoftware
