#include "src/context/notification_queue.h"

#include <unistd.h>

namespace avsoftware {

const std::chrono::milliseconds kWaitBetweenCheck(100);
const std::chrono::milliseconds kSleepBetweenFlushes(1000);

SlogAsyncNotificationQueue::SlogAsyncNotificationQueue(
    const std::function<void(const SlogRecord&)>& notify,
    const std::function<void()>& thread_init) {
  // Reserve buffer_ before initializing the thread.
  // Still in single threaded mode, no lock is required.
  buffer_.reserve(kBufferSize);

  // The worker thread.
  process_loop_ = std::thread([this, notify, thread_init] {
    thread_init();

    std::vector<SlogRecord> batch;
    batch.reserve(kBufferSize);
    auto last_check_time = std::chrono::steady_clock::now();
    while (true) {
      {
        std::unique_lock<std::mutex> lock(mu_);
        if (done_) {
          return;
        }
        batch.swap(buffer_);
        if (batch.empty()) {
          cv_batch_ready_.wait_for(lock, kSleepBetweenFlushes);
        }
      }
      // TODO(vsbus): This leads to big inefficiencies in the implementation of
      // the callbacks. (mutex locking...). We should change the callback to
      // accept a complete batch instead of individual events.
      for (const SlogRecord& record : batch) {
        const auto now = std::chrono::steady_clock::now();
        // If batch has many tasks that take too long time to handle we
        // don't want to wait them all to finish. That's why we
        // periodically check status of done_ variable to not block program
        // exit.
        if (now - last_check_time > kWaitBetweenCheck) {
          last_check_time = now;
          std::unique_lock<std::mutex> lock(mu_);
          if (done_) {
            return;
          }
        }
        notify(record);
      }
      {
        std::unique_lock<std::mutex> lock(mu_);
        num_records_flushed_ += batch.size();
      }
      batch.clear();
      cv_batch_flushed_.notify_all();
    }
  });
}

SlogAsyncNotificationQueue::~SlogAsyncNotificationQueue() {
  {
    std::unique_lock<std::mutex> lock(mu_);
    done_ = true;
    cv_batch_ready_.notify_all();
  }
  process_loop_.join();
}

}  // namespace avsoftware
