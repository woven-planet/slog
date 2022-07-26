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

#ifndef slog_cc_primitives_record
#define slog_cc_primitives_record

#include <string>
#include <utility>
#include <vector>

#include "slog_cc/primitives/tag.h"
#include "slog_cc/primitives/timestamps.h"
#include "slog_cc/util/inline_macro.h"

namespace slog {

constexpr int8_t INFO = 0;
constexpr int8_t WARNING = 1;
constexpr int8_t ERROR = 2;
constexpr int8_t FATAL = 3;

// TODO(vsbus): now having slog namespace, does it worth removing Slog prefix
// everywhere to avoid slog::SlogFoo repititions?
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
  // TODO(vsbus): move severity to CallSite?
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

}  // namespace slog

#endif
