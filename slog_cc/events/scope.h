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

#ifndef slog_cc_events_scope
#define slog_cc_events_scope

#include <string>

#include "slog_cc/events/event.h"
#include "slog_cc/util/inline_macro.h"

namespace slog {

constexpr char kSlogTagKeyFuncBlockStart[] = ".func_block_b";
constexpr char kSlogTagKeyScopeClose[] = ".scope_close";
constexpr char kSlogTagKeyScopeId[] = ".scope_id";
constexpr char kSlogTagKeyScopeDepth[] = ".scope_depth";
constexpr char kSlogTagKeyScopeName[] = ".scope_name";
constexpr char kSlogTagKeyScopeOpen[] = ".scope_open";

}  // namespace slog

#define _SLOG_SCOPE_CLOSE(scope_id)        \
  slog::SlogEvent(slog::INFO)              \
      .addTag(slog::kSlogTagKeyScopeClose) \
      .addTag(slog::kSlogTagKeyScopeId, scope_id)

namespace slog {

class SlogScope {
 public:
  // Passing a SlogEvent by reference here because we don't want to make any
  // copies (this would trigger event logging). See SLOG_SCOPE macro in slog.h
  SlogScope(SlogEvent& log_event) {
    scope_id_ = ++thread_local_scopes_counter_;
    ++thread_local_scopes_depth_;
    log_event.addTag(kSlogTagKeyScopeOpen)
        .addTag(kSlogTagKeyScopeId, scope_id_)
        .addTag(kSlogTagKeyScopeDepth, thread_local_scopes_depth_);
  }

  SLOG_INLINE ~SlogScope() {
    _SLOG_SCOPE_CLOSE(scope_id_);
    --thread_local_scopes_depth_;
  }
  static int currentScopeDepth() { return thread_local_scopes_depth_; }

 private:
  SlogScope() = delete;
  thread_local static int thread_local_scopes_counter_;
  thread_local static int thread_local_scopes_depth_;
  int scope_id_ = -1;
};

}  // namespace slog

#endif
