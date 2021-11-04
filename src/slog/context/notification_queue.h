#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "src/slog/primitives/record.h"
#include "src/slog/printer/printer.h"

namespace avsoftware {

// Asynchronous thread-safe queue of Slog events. Allows to add events to queue
// from multiple threads and eventually handles them in a single background
// thread. Ordering is FIFO. When handling next event from the queue calls
// notify(record) on it. notify() is a lambda passed to constructor that is
// supposed to trigger callbacks according to user choice.
class SlogAsyncNotificationQueue {
 public:
  // notify -- a lambda that is being run in a background thread to send the
  // notification. thread_init -- a lambda that is run in the beginning of
  // background thread, e.g. it could set the thread name.
  SlogAsyncNotificationQueue(
      const std::function<void(const SlogRecord&)>& notify,
      const std::function<void()>& thread_init);

  ~SlogAsyncNotificationQueue();

  SLOG_INLINE void add(SlogRecord&& record) {
    std::unique_lock<std::mutex> lock(mu_);
    buffer_.emplace_back(std::move(record));
    num_records_added_ += 1;
    // buffer_ was pre-allocated for kBufferSize elements. When buffer size is
    // approaching kBufferSize it is a good time to notify flush thread to
    // process the buffer. Benchmarks showed that we need to do it before we
    // get kBufferSize messages in the buffer but not too early. kBufferSize/2
    // appeared a good value providing good benchmarks. Maybe the best value is
    // a bit bigger or smaller but small changes of this parameter don't provide
    // visible benchmark improvements.
    if (buffer_.size() % (kBufferSize / 2) == 0) {
      cv_batch_ready_.notify_all();
    }
  }

  SLOG_INLINE void waitRecordsFlush() {
    cv_batch_ready_.notify_all();
    std::unique_lock<std::mutex> lock(mu_);
    const size_t num_added = num_records_added_;
    while (num_added > num_records_flushed_) {
      cv_batch_flushed_.wait(lock);
    }
  }

 private:
  static constexpr size_t kBufferSize = 8192;

  std::mutex mu_;

  // A batch-flush background thread (process_loop_) is waiting to be notified
  // when batch is ready using cv_batch_ready_ condition variable.
  std::condition_variable cv_batch_ready_;

  // waitRecordsFlush() is waiting when exiting buffer is flushed using
  // cv_batch_flushed_ condition variable.
  std::condition_variable cv_batch_flushed_;

  std::vector<SlogRecord> buffer_;
  std::chrono::steady_clock::time_point last_flush_time_ =
      std::chrono::steady_clock::now();

  // Two counters to track number of slog messages added to notifiction queue
  // and processed by subscribers. They start with 0 and always growing. We
  // assume one process can't generate more than 1B slogs per second. So these
  // counters can work for (size_t_max / 1e9) seconds before overflowing:
  // 2**64 / 1e9 ~= 580+ years.
  size_t num_records_added_ = 0;
  size_t num_records_flushed_ = 0;

  bool done_ = false;
  std::thread process_loop_;
};

}  // namespace avsoftware
