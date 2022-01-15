// This test compares the latency jitter of code using SLOG and code not using
// SLOG.
//
// We have a busyLoop function that mostly does computation and sometimes logs
// the time it takes to do the computation. We implement 2 versions:
// - busyLoopNoSlog: this version uses ad-hoc time logging with data structures
// local to the thread.
// - busyLoopSlog  : this version uses SLOG to do the time logging.
//
// The test starts N concurrent threads. N/2 threads run busyLoopSlog, N/2 run
// busyLoopNoSlog. Each time we run the functions, we record the execution time.
// At the end of the test, we print the distribution of those execution times
// for the 2 different versions. We expect the two distributions to be very
// similar. This test verifies that using SLOG does not introduce any
// significant latency jitter to the application being instrumented.
//
// clang-format off
// Run test with the following command:
/*
  $> ./avctl build slog:slog_concurrency_stress_test && \
     ./bazel-bin/slog/slog_concurrency_stress_test
*/
// 2020-11-11
//
// 36 threads, totaliters=1047124, nloops=100
// ADHOC Samples: 10298 Median:0.017 percentiles 90:0.017 99:0.018 99.9:0.032 99.99:0.040 maxv:0.041
// SLOG  Samples: 10116 Median:0.017 percentiles 90:0.017 99:0.019 99.9:0.038 99.99:0.051 maxv:0.058
// clang-format on

#include <gflags/gflags.h>
#include <glog/logging.h>
#include <math.h>
#include "slog_cc/slog.h"
#include "slog_cc/common/util/init.h"

namespace slog {

DEFINE_int32(test_duration, 10, "Number of seconds to test.");
DEFINE_int32(threads, std::thread::hardware_concurrency(),
             "Number of threads.");
DEFINE_int32(nloops, 100,
             "Number of times we log time per function execution.");
DEFINE_int32(target_function_time_ms, 10,
             "Number of ms that the reference function we are testing should "
             "take to execute with no load.");

// An ad-hoc structure to record time for the "no-SLOG" case.
struct TimeRecord {
  std::string name;
  std::chrono::steady_clock::time_point start, end;
};

// A computation loop that uses ad-hoc time keeping.
double busyLoopNoSlog(int64_t nloops, int64_t loopiters) {
  double x = 1.0;
  std::vector<TimeRecord> recorder;
  for (int64_t i = 0; i < nloops; ++i) {
    TimeRecord rec;
    rec.name = "myscope";
    rec.start = std::chrono::steady_clock::now();
    for (int64_t j = 0; j < loopiters; ++j) {
      x = x + sqrt(x) - exp(x);
    }
    rec.end = std::chrono::steady_clock::now();
    recorder.push_back(rec);
  }
  return x;
}

// A computation loop that uses SLOG for time keeping.
double busyLoopSlog(int64_t nloops, int64_t loopiters) {
  double x = 1.0;
  for (int64_t i = 0; i < nloops; ++i) {
    SLOG_SCOPE("myscope");
    for (int64_t j = 0; j < loopiters; ++j) {
      x = x + sqrt(x) - exp(x);
    }
  }
  return x;
}

// Run the computation functions in a loop for a duration of "duration" and keep
// track of the execution times in the "durations" vector.
void threadLauncher(int nloops, int loopiters,
                    std::chrono::nanoseconds duration, bool use_slog,
                    std::vector<std::chrono::nanoseconds>* durations) {
  std::chrono::steady_clock::time_point deadline =
      std::chrono::steady_clock::now() + duration;
  while (true) {
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    if (use_slog) {
      busyLoopSlog(nloops, loopiters);
    } else {
      busyLoopNoSlog(nloops, loopiters);
    }
    std::chrono::steady_clock::time_point end =
        std::chrono::steady_clock::now();
    durations->push_back(end - start);
    if (end > deadline) {
      break;
    }
  }
}

// Find an approximate number of iterations that make one execution of
// busyLoopXXXX last "target_execution_duration".
int64_t guessTotalIters(std::chrono::nanoseconds target_execution_duration) {
  int64_t iters =
      1e8 * std::chrono::duration<double>(target_execution_duration).count();
  for (int i = 0; i < 3; ++i) {
    std::chrono::steady_clock::time_point start =
        std::chrono::steady_clock::now();
    busyLoopNoSlog(1, iters);
    std::chrono::nanoseconds actual_duration =
        std::chrono::steady_clock::now() - start;
    iters = iters *
            std::chrono::duration<double>(target_execution_duration).count() /
            std::chrono::duration<double>(actual_duration).count();
  }
  return iters;
}

void test_slog() {
  const int64_t totaliters =
      guessTotalIters(std::chrono::milliseconds(FLAGS_target_function_time_ms));
  const int64_t nloops = FLAGS_nloops;
  const int nthreads = FLAGS_threads;
  const std::chrono::nanoseconds test_duration =
      std::chrono::seconds(FLAGS_test_duration);
  std::vector<std::vector<std::chrono::nanoseconds>> durations(nthreads);
  std::vector<std::thread> threads(nthreads);
  printf("%d threads, totaliters=%" PRId64 ", nloops=%" PRId64 "\n", nthreads,
         totaliters, nloops);
  for (int i = 0; i < nthreads; ++i) {
    // 1/2 of the thread will use slog, the other 1/2 does not.
    int use_slog = i & 1;
    durations[i].reserve(100000);
    threads[i] = std::thread(threadLauncher, nloops, totaliters / nloops,
                             test_duration, use_slog, &durations[i]);
  }

  for (int i = 0; i < nthreads; ++i) {
    threads[i].join();
  }
  // Aggregate the results from all the slog threads and non-slog threads
  // independently.
  std::vector<double> all_stats[2];
  for (int i = 0; i < nthreads; ++i) {
    for (size_t j = 0; j < durations[i].size(); ++j) {
      all_stats[i & 1].push_back(
          std::chrono::duration<double>(durations[i][j]).count());
    }
  }
  // Compute distribution metrics on all the samples.
  for (int i = 0; i < 2; ++i) {
    std::vector<double>& data = all_stats[i];
    std::sort(data.begin(), data.end());
    const double median = data[static_cast<int>(data.size() * 0.5)];
    const double perc90 = data[static_cast<int>(data.size() * 0.90)];
    const double perc99 = data[static_cast<int>(data.size() * 0.99)];
    const double perc999 = data[static_cast<int>(data.size() * 0.999)];
    const double perc9999 = data[static_cast<int>(data.size() * 0.9999)];
    const double maxv = data.back();
    printf(
        "%s Samples: %d Median:%f percentiles 90:%f 99:%f 99.9:%f 99.99:%f "
        "maxv:%f\n",
        i ? "SLOG " : "ADHOC", (int)data.size(), median, perc90, perc99,
        perc999, perc9999, maxv);
  }
}

}  // namespace slog

int main(int argc, char** argv) {
  slog::init(&argc, &argv);
  slog::test_slog();
}
