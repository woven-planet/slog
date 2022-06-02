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

#ifndef slog_cc_slog
#define slog_cc_slog

#include "slog_cc/context/context.h"
#include "slog_cc/events/event.h"
#include "slog_cc/events/scope.h"

#define SLOG(severity)                                                \
  slog::SlogEvent(slog::severity, [func = __FUNCTION__] {             \
    static int32_t slog_call_site_id =                                \
        slog::SlogContext::getInstance()->addCallSite(func, __FILE__, \
                                                      __LINE__);      \
    return slog_call_site_id;                                         \
  }())

// C++ standard 12.2.3 guarantees that the SlogEvent is not destroyed before
// SlogScope's constructor finishes.
#define SLOG_SCOPE(scope_name) \
  slog::SlogScope slog_scope = \
      SLOG(INFO).addTag(slog::kSlogTagKeyScopeName, scope_name)

#define SLOG_FUNC_BLOCK_START(func_block_name) \
  SLOG(INFO).addTag(slog::kSlogTagKeyFuncBlockStart, func_block_name)

#endif
