#pragma once

#include <string>
#include <utility>
#include <vector>

#include "src/slog/primitives/tag.h"
#include "src/slog/primitives/timestamps.h"
#include "src/slog/util/inline_macro.h"

namespace avsoftware {

class SlogRecord {
 public:
  SLOG_INLINE
  SlogRecord(int32_t thread_id, int32_t call_site_id, int8_t severity)
      : thread_id_(thread_id),
        call_site_id_(call_site_id),
        severity_(severity) {
    tags_.reserve(16);
  }

  SLOG_INLINE int32_t thread_id() const { return thread_id_; }
  SLOG_INLINE int32_t call_site_id() const { return call_site_id_; }
  SLOG_INLINE const SlogTimestamps& time() const { return time_; }
  SLOG_INLINE void set_time(const SlogTimestamps& time) { time_ = time; }
  SLOG_INLINE int8_t severity() const { return severity_; }
  SLOG_INLINE const std::vector<SlogTag>& tags() const { return tags_; }
  SLOG_INLINE const SlogTag* find_tag(const std::string& key) const {
    for (auto& tag : tags_) {
      if (tag.key() == key) {
        return &tag;
      }
    }
    return nullptr;
  };

  template <class... Args>
  SLOG_INLINE void addTag(Args&&... args) {
    tags_.emplace_back(std::forward<Args>(args)...);
  }

  bool isNoisy() const;

 private:
  int32_t thread_id_ = -1;
  int32_t call_site_id_ = -1;
  SlogTimestamps time_;
  int8_t severity_ = -1;
  std::vector<SlogTag> tags_;
};

}  // namespace avsoftware
