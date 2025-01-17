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

#ifndef slog_cc_events_event
#define slog_cc_events_event

#include <cstdint>

#include "slog_cc/context/context.h"
#include "slog_cc/primitives/record.h"
#include "slog_cc/primitives/tag.h"
#include "slog_cc/util/inline_macro.h"
#include "slog_cc/util/os/thread_id.h"
#include "slog_cc/util/string_util.h"

namespace slog {

class SlogEvent {
 public:
  SlogEvent(const SlogEvent& other) = delete;
  SlogEvent(SlogEvent&& other) = default;

  SlogEvent& operator=(const SlogEvent& other) = delete;
  SlogEvent& operator=(SlogEvent&& other) = delete;

  SLOG_INLINE SlogEvent(const int8_t severity, const int32_t call_site_id = 0)
      : record_(
            [] {
              thread_local int32_t thread_id = util::os::get_thread_id();
              return thread_id;
            }(),
            call_site_id, severity) {}

  SLOG_INLINE ~SlogEvent() {
    record_.set_time(SlogContext::getInstance()->getTimestamps());
    SlogContext::getInstance()->notifySyncSubscribers(this->record_);
    SlogContext::getInstance()->notifyAsyncSubscribers(
        std::move(this->record_));
  }

  SLOG_INLINE const SlogRecord& record() const { return record_; }

  template <class... Args>
  SLOG_INLINE SlogEvent& addTag(Args&&... args) {
    record_.addTag(std::forward<Args>(args)..., SlogTagVerbosity::kSilent);
    return *this;
  }

  SLOG_INLINE SlogEvent& operator<<(const char* s) {
    record_.addTag("", s);
    return *this;
  }
  SLOG_INLINE SlogEvent& operator<<(std::string&& s) {
    record_.addTag(util::stringPrintf("_t%d_s", stream_term_tag_id_++),
                   std::move(s));
    return *this;
  }
  SLOG_INLINE SlogEvent& operator<<(const std::string& s) {
    record_.addTag(util::stringPrintf("_t%d_s", stream_term_tag_id_++), s);
    return *this;
  }
  SLOG_INLINE SlogEvent& operator<<(SlogTag&& tag) {
    record_.addTag(std::move(tag));
    return *this;
  }

  template <class T, typename std::enable_if<
                         std::is_floating_point<T>::value>::type* = nullptr>
  SLOG_INLINE SlogEvent& operator<<(const T value) {
    record_.addTag(util::stringPrintf("_t%d_f", stream_term_tag_id_++), value);
    return *this;
  }

  template <class T, typename std::enable_if<
                         std::is_integral<T>::value>::type* = nullptr>
  SLOG_INLINE SlogEvent& operator<<(const T value) {
    record_.addTag(util::stringPrintf("_t%d_i", stream_term_tag_id_++), value);
    return *this;
  }

 private:
  SlogRecord record_;
  int16_t stream_term_tag_id_ = 0;
};

}  // namespace slog

#endif
