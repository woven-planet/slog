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

#ifndef slog_cc_context_context
#define slog_cc_context_context

#include <cassert>
#include <functional>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "slog_cc/context/notification_queue.h"
#include "slog_cc/context/subscribers.h"
#include "slog_cc/primitives/call_site.h"
#include "slog_cc/primitives/record.h"
#include "slog_cc/printer/printer.h"
#include "slog_cc/util/assert_macro.h"

namespace slog {

class SlogContext {
 public:
  static std::shared_ptr<SlogContext> getInstance() noexcept;

  SlogSubscriber createAsyncSubscriber(const SlogCallback& callback) {
    return async_subscribers_.create(callback);
  }

  SlogSubscriber createSyncSubscriber(const SlogCallback& callback) {
    return sync_subscribers_.create(callback);
  }

  SLOG_INLINE void notifySyncSubscribers(const SlogRecord& record) noexcept {
    sync_subscribers_.notify(record);
  }

  SLOG_INLINE void notifyAsyncSubscribers(SlogRecord&& record) noexcept {
    std::shared_lock<std::shared_timed_mutex> lock(
        async_notification_queue_mutex_);
    SLOG_ASSERT(async_notification_queue_.get());
    async_notification_queue_.get()->add(std::move(record));
  }

  SLOG_INLINE void waitAsyncSubscribers() {
    std::shared_lock<std::shared_timed_mutex> lock(
        async_notification_queue_mutex_);
    SLOG_ASSERT(async_notification_queue_.get());
    async_notification_queue_.get()->waitRecordsFlush();
  }

  void resetAsyncNotificationQueue(const std::function<void()>& thread_init =
                                       [] {}) {
    std::unique_lock<std::shared_timed_mutex> lock(
        async_notification_queue_mutex_);
    async_notification_queue_.reset(new SlogAsyncNotificationQueue(
        [this](const SlogRecord& record) { async_subscribers_.notify(record); },
        thread_init));
  }

  SLOG_INLINE size_t numCallSites() {
    std::shared_lock<std::shared_timed_mutex> lock(call_sites_mutex_);
    return call_sites_.size();
  }

  // We are using vector<unique_ptr<SlogCallSite>> to guarantee valid references
  // on call_sites_ vector resize. But if resetCallSites was called it will
  // invalidate result reference.
  SLOG_INLINE const SlogCallSite& getCallSite(size_t call_site_id) {
    std::shared_lock<std::shared_timed_mutex> lock(call_sites_mutex_);
    SLOG_ASSERT(call_site_id < call_sites_.size());
    return *call_sites_.at(call_site_id);
  }

  SLOG_INLINE int addCallSite(const std::string& function,
                              const std::string& file, int32_t line) {
    std::unique_lock<std::shared_timed_mutex> lock(call_sites_mutex_);
    return addCallSiteUnsafe(function, file, line);
  }

  // This function finds an existing call site ID or adds a new one for a given
  // <function, file, line> tuple. Its implementation is naively scanning a list
  // of registered call sites thus it could be very slow. A more efficient
  // implementation could use a hashtable from a tuple to ID for a faster
  // lookup.
  int addOrReuseCallSiteVerySlow(const std::string& function,
                                 const std::string& file, int32_t line);

  // Use resetCallSites() ONLY for testing. It invalidates CallSite references
  // returned by getCallSite().
  SLOG_INLINE void resetCallSites() {
    std::unique_lock<std::shared_timed_mutex> lock(call_sites_mutex_);
    call_sites_.clear();
    call_sites_.emplace_back(std::make_unique<SlogCallSite>("", "", 0));
  }

  // NOTE: time methods are not thread-safe.
  /// Captures current timestamps using default of user-defined implementation.
  SlogTimestamps getTimestamps() const noexcept;
  /// Overrides implementation of getTimestamps().
  void setGetTimestampsFunc(const std::function<SlogTimestamps()>& func);
  /// A default implementation of getTimestamps().
  static const std::function<SlogTimestamps()> kDefaultGetTimestampsFunc;

 private:
  SlogContext();

  // Thread unsafe implementation of add-call-site logic as a private method as
  // all public methods must be thread safe.
  SLOG_INLINE int addCallSiteUnsafe(const std::string& function,
                                    const std::string& file, int32_t line) {
    SLOG_ASSERT(call_sites_.size() > 0U &&
                "call_sites_ isn't initialized with resetCallSites() call.");
    call_sites_.emplace_back(
        std::make_unique<SlogCallSite>(function, file, line));
    return call_sites_.size() - 1;
  }

  SLOG_INLINE void emitStderrLine(const SlogRecord& record) {
    if (record.severity() == FATAL || record.isNoisy()) {
      slog_printer_.emitStderrLine(record, getCallSite(record.call_site_id()));
    }
  }

  SlogContextSubscribers async_subscribers_;
  SlogContextSubscribers sync_subscribers_;
  SlogSubscriber echo_to_glog_;

  std::unique_ptr<SlogAsyncNotificationQueue> async_notification_queue_;
  std::shared_timed_mutex async_notification_queue_mutex_;

  std::vector<std::unique_ptr<SlogCallSite>> call_sites_;
  std::shared_timed_mutex call_sites_mutex_;

  std::function<SlogTimestamps()> get_timestamps_func_;
  SlogPrinter slog_printer_;
};

}  // namespace slog

#endif
