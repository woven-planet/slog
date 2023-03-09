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

#ifndef slog_cc_context_subscribers
#define slog_cc_context_subscribers

#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "slog_cc/primitives/record.h"
#include "slog_cc/util/inline_macro.h"

namespace slog {

using SlogCallback = std::function<void(const SlogRecord&)>;
using SlogCallbackId = SlogCallback*;
using SlogSubscriber = std::shared_ptr<SlogCallbackId>;

class SlogContextSubscribers {
 public:
  SlogSubscriber create(const SlogCallback& callback);

  SLOG_INLINE void notify(const SlogRecord& record) {
    std::unique_lock<std::mutex> low_priority_access_lock(
        low_priority_access_mutex_);
    std::unique_lock<std::mutex> next_access_lock(next_access_mutex_);
    std::unique_lock<std::mutex> data_lock(data_mutex_);
    next_access_lock.unlock();

    for (const auto& callback : *callbacks_) {
      (*callback)(record);
    }
    if (record.severity() == FATAL) {
      abort();
    }
  }

 private:
  SlogCallbackId addCallback(const SlogCallback& callback);
  void removeCallback(SlogCallbackId callback_id);

  // SlogContextSubscribers class manages SlogSubscriber resources. Only
  // SlogSubscriber shared pointer can be exposed to the external users via
  // create() interface. Internal details or their copies like callbacks_ should
  // never be exposed. This is required to guarantee the thread-safety of
  // ContextSubscribers.
  std::shared_ptr<std::vector<std::shared_ptr<SlogCallback>>> callbacks_{
      new std::vector<std::shared_ptr<SlogCallback>>()};

  // Using "triple mutex" pattern from
  // https://stackoverflow.com/questions/11666610/how-to-give-priority-to-privileged-thread-in-mutex-locking
  // to allow addCallback() and removeCallback() to run with higher priority
  // than notify().
  std::mutex data_mutex_;
  std::mutex next_access_mutex_;
  std::mutex low_priority_access_mutex_;
};

}  // namespace slog

#endif
