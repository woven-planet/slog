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
