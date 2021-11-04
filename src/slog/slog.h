#pragma once

#include "src/slog/context/context.h"
#include "src/slog/events/event.h"
#include "src/slog/events/scope.h"

namespace avsoftware {

#define SLOG(severity)                                                    \
  SlogEvent(google::severity, [func = __FUNCTION__] {                     \
    static int32_t slog_call_site_id =                                    \
        SlogContext::getInstance().addCallSite(func, __FILE__, __LINE__); \
    return slog_call_site_id;                                             \
  }())

// C++ standard 12.2.3 guarantees that the SlogEvent is not destroyed before
// SlogScope's constructor finishes.
#define SLOG_SCOPE(scope_name) \
  SlogScope slog_scope = SLOG(INFO).addTag(kSlogTagKeyScopeName, scope_name)

#define SLOG_FUNC_BLOCK_START(func_block_name) \
  SLOG(INFO).addTag(kSlogTagKeyFuncBlockStart, func_block_name)

}  // namespace avsoftware
