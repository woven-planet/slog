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

#ifndef slog_cc_context_notification_queue
#define slog_cc_context_notification_queue

#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

#include "slog_cc/primitives/record.h"
#include "slog_cc/printer/printer.h"

namespace slog {

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
      const std::function<void()>& thread_init, size_t buffer_size);

  ~SlogAsyncNotificationQueue();

  SLOG_INLINE void add(SlogRecord&& record) {
    std::unique_lock<std::mutex> lock(mu_);
    buffer_.emplace_back(std::move(record));
    num_records_added_ += 1;
    // buffer_ was pre-allocated for buffer_size_ elements. When buffer size is
    // approaching buffer_size_ it is a good time to notify flush thread to
    // process the buffer. Benchmarks showed that we need to do it before we
    // get buffer_size_ messages in the buffer but not too early. buffer_size_/2
    // appeared a good value providing good benchmarks. Maybe the best value is
    // a bit bigger or smaller but small changes of this parameter don't provide
    // visible benchmark improvements.
    if (buffer_.size() % (buffer_size_ / 2) == 0) {
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
  const size_t buffer_size_;

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

}  // namespace slog

#endif
