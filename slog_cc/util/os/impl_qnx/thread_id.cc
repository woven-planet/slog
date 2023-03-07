#include "../thread_id.h"

#include <process.h>

namespace slog {
namespace util {
namespace os {

int32_t get_thread_id() {
  return gettid();
}

}  // namespace os
}  // namespace util
}  // namespace slog
