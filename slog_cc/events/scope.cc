#include "slog_cc/events/scope.h"

namespace slog {

thread_local int SlogScope::thread_local_scopes_counter_ = 0;
thread_local int SlogScope::thread_local_scopes_depth_ = 0;

}  // namespace slog
