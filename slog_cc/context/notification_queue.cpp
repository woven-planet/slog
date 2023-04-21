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

#include "slog_cc/context/notification_queue.h"

#include <unistd.h>

namespace slog {

const std::chrono::milliseconds kWaitBetweenCheck(100);
const std::chrono::milliseconds kSleepBetweenFlushes(1000);

SlogAsyncNotificationQueue::SlogAsyncNotificationQueue(
    const std::function<void(const SlogRecord&)>& notify,
    const std::function<void()>& thread_init,
    size_t buffer_size) : buffer_size_(buffer_size) {
  // Reserve buffer_ before initializing the thread.
  // Still in single threaded mode, no lock is required.
  buffer_.reserve(buffer_size);

  // The worker thread.
  process_loop_ = std::thread([this, notify, thread_init] {
    thread_init();

    std::vector<SlogRecord> batch;
    batch.reserve(buffer_size_);
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

}  // namespace slog
