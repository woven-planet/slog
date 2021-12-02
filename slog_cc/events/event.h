#pragma once

#include <sys/syscall.h>
#include <unistd.h>

#include "slog_cc/context/context.h"
#include "slog_cc/primitives/record.h"
#include "slog_cc/primitives/tag.h"
#include "slog_cc/util/inline_macro.h"
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
              thread_local int32_t thread_id = syscall(SYS_gettid);
              return thread_id;
            }(),
            call_site_id, severity) {}

  SLOG_INLINE ~SlogEvent() {
    record_.set_time(SlogContext::getInstance().getTimestamps());
    SlogContext::getInstance().notifySubscribers(std::move(this->record_));
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
