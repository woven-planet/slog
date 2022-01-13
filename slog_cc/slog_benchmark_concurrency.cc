#include "slog_cc/slog.h"

#include <benchmark/benchmark.h>

// clang-format off
// Logs emit data to std streams, to run benchmark not printing lots of logs run
/*
  $> ./avctl build slog:slog_benchmark_concurrency && \
     ./bazel-bin/slog/slog_benchmark_concurrency \
         --benchmark_counters_tabular \
         --benchmark_min_time 10
*/
// 2020-11-11 09:28:00
// Run on (36 X 1265.09 MHz CPU s)
// CPU Caches:
//   L1 Data 32K (x18)
//   L1 Instruction 32K (x18)
//   L2 Unified 1024K (x18)
//   L3 Unified 25344K (x1)
// -----------------------------------------------------------------------------------------
// Benchmark                            Time           CPU Iterations avg_time_s max_time_s
// -----------------------------------------------------------------------------------------
// SlogLoad/sleep/threads:1        169613 ns      15505 ns     910835   852.403n    34.714u
// SlogLoad/sleep/threads:2         84817 ns      15632 ns     854094    1.2292u     49.78u
// SlogLoad/sleep/threads:4         42650 ns      16614 ns     923280   1.65051u   54.7103u
// SlogLoad/sleep/threads:6         28181 ns      15116 ns     932316   1.40715u   36.0755u
// SlogLoad/sleep/threads:8         21093 ns      14887 ns     913312   1.50141u   54.4045u
// SlogLoad/sleep/threads:10        16869 ns      14835 ns     885220   1.57035u   47.3925u
// SlogLoad/sleep/threads:12        13988 ns      14116 ns    1003128    1.4794u   55.2782u
// SlogLoad/sleep/threads:14        11972 ns      13923 ns     986860   1.53649u   49.8313u
// SlogLoad/sleep/threads:16        10427 ns      13335 ns    1011856     1.485u   54.5588u
// SlogLoad/sleep/threads:18         9242 ns      12967 ns    1115316   1.45163u   65.3246u
// SlogLoad/sleep/threads:20         8290 ns      12465 ns    1122000   1.43695u   258.888u
// SlogLoad/sleep/threads:22         7512 ns      12045 ns    1125432   1.39088u   47.2895u
// SlogLoad/sleep/threads:24         6859 ns      11531 ns    1238280    1.3633u   81.3648u
// SlogLoad/sleep/threads:26         6313 ns      11157 ns    1284270    1.3384u   58.6688u
// SlogLoad/sleep/threads:28         5841 ns      10681 ns    1355256   1.30322u   63.7678u
// SlogLoad/sleep/threads:30         5439 ns      10350 ns    1328580   1.28469u    58.365u
// SlogLoad/sleep/threads:32         5079 ns       9848 ns    1455584   1.25301u   66.9319u
// SlogLoad/sleep/threads:34         4764 ns       9384 ns    1430108   1.20351u   81.4014u
// SlogLoad/sleep/threads:36         4491 ns       9158 ns    1557900   1.22961u   70.3267u
// SlogLoad/nosleep/threads:1         314 ns        313 ns   44734150   280.027n    46.014u
// SlogLoad/nosleep/threads:2         593 ns       1183 ns   11665822   1.14636u   73.4725u
// SlogLoad/nosleep/threads:4         560 ns       2181 ns    6495264    2.1982u   60.5828u
// SlogLoad/nosleep/threads:6         573 ns       3282 ns    4334946   3.39634u     47.54u
// SlogLoad/nosleep/threads:8         578 ns       4376 ns    3183944   4.58381u     61.88u
// SlogLoad/nosleep/threads:10        592 ns       5565 ns    2555350   5.88199u   77.5387u
// SlogLoad/nosleep/threads:12        585 ns       6613 ns    2105832   6.98384u   59.2148u
// SlogLoad/nosleep/threads:14        592 ns       7860 ns    1782830   8.24583u   75.4184u
// SlogLoad/nosleep/threads:16        593 ns       9077 ns    1560816   9.44387u   105.722u
// SlogLoad/nosleep/threads:18        590 ns      10229 ns    1368792   10.5713u    118.08u
// SlogLoad/nosleep/threads:20        594 ns      11480 ns    1245280   11.8262u   143.381u
// SlogLoad/nosleep/threads:22        589 ns      12576 ns    1115268   12.9211u   159.988u
// SlogLoad/nosleep/threads:24        608 ns      14181 ns    1011648   14.5509u   226.739u
// SlogLoad/nosleep/threads:26        605 ns      15334 ns     906126   15.6953u   218.368u
// SlogLoad/nosleep/threads:28        620 ns      16961 ns     830844   17.3167u   239.835u
// SlogLoad/nosleep/threads:30        639 ns      18742 ns     757170    19.125u   264.443u
// SlogLoad/nosleep/threads:32        657 ns      20607 ns     690496   20.9665u   285.448u
// SlogLoad/nosleep/threads:34        660 ns      21994 ns     632060   22.3997u   325.425u
// SlogLoad/nosleep/threads:36        683 ns      23835 ns     593424   24.5363u   788.258u
// clang-format on

namespace slog {

class SlogLoad : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {
    SlogContext::getInstance()->resetAsyncNotificationQueue();
  }
};

void slogLoadTest(const Duration sleep, benchmark::State* state) {
  // This test makes a very basic SLOG call from N threands with sleeps of S
  // seconds between them. We iterate a number of <N, S> combinations to
  // evaluate different load scenarios.
  Duration total_time, max_time;
  int num_iterations = 0;
  for (auto _ : *state) {
    const auto start_time = MonotonicTime::now();
    SLOG(INFO);
    const Duration duration = MonotonicTime::now() - start_time;

    if (sleep.toSec() > 0) {
      sleepFor(sleep);
    }

    state->SetIterationTime(duration.toSec());

    num_iterations += 1;
    total_time += duration;
    max_time = std::max(max_time, duration);
  }

  state->counters["max_time_s"] =
      benchmark::Counter(max_time.toSec(), benchmark::Counter::kAvgThreads);
  state->counters["avg_time_s"] = benchmark::Counter(
      total_time.toSec() / num_iterations, benchmark::Counter::kAvgThreads);
}

BENCHMARK_DEFINE_F(SlogLoad, sleep)(benchmark::State& state) {
  slogLoadTest(Duration::fromUSec(100), &state);
}
BENCHMARK_REGISTER_F(SlogLoad, sleep)
    // To have fewer tests we do DenseThreadRange with step 2 for even numers.
    // But we also want a test with a single core, that's why we have
    // Threads(1).
    ->Threads(1)
    ->DenseThreadRange(2, std::thread::hardware_concurrency(), 2);

BENCHMARK_DEFINE_F(SlogLoad, nosleep)(benchmark::State& state) {
  slogLoadTest(Duration(), &state);
}
BENCHMARK_REGISTER_F(SlogLoad, nosleep)
    // To have fewer tests we do DenseThreadRange with step 2 for even numers.
    // But we also want a test with a single core, that's why we have
    // Threads(1).
    ->Threads(1)
    ->DenseThreadRange(2, std::thread::hardware_concurrency(), 2);

}  // namespace slog
