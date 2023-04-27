#include <unistd.h>

#include "slog_cc/analysis_tools/tracing/slog_trace_subscriber.h"
#include "slog_cc/context/context.h"
#include "slog_cc/slog.h"

int fib(int n) {
  SLOG_SCOPE("fib-" + std::to_string(n));
  int result = n <= 1 ? 1 : fib(n - 1) + fib(n - 2);
  SLOG(INFO) << "fib(" << n << ") = " << result;
  return result;
}

int main() {
  // While slog trace subscriber object exists it will append chrome tracing
  // records to the provided json file. On destruction it will finalize the
  // file.
  auto slog_trace_subscriber = slog::CreateSlogTraceSubscriber(
      "/tmp/slog-trace.json", slog::SlogTraceConfig::kTrackScopesAndLogs);

  // Actual code with slogs body.
  {
    SLOG(INFO).addTag(slog::kSlogTagTraceThreadName, "example-t");

    SLOG_SCOPE("Whole test scope");
    constexpr int kNumIterations = 4;
    for (int i = 0; i < kNumIterations; ++i) {
      SLOG_SCOPE("iteration-" + std::to_string(i) + "-of-" +
                 std::to_string(kNumIterations));
      const int sleep_us = (i + 1) * 100;
      SLOG(INFO) << "iteration #" << i << ", sleeping "
                 << slog::SlogTag("sleep_us", sleep_us) << " microseconds...";
      SLOG_SCOPE("Sleep scope").addTag("sleep_us", sleep_us);
      usleep(sleep_us);
    }

    fib(7);
  }

  // Need to wait all slog records to be processed to make sure all records were
  // flushed and written to the slog json file.
  slog::SlogContext::getInstance()->waitAsyncSubscribers();
}
