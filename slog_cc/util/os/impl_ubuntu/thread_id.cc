#include "../thread_id.h"

#include <sys/syscall.h>
#include <unistd.h>

namespace slog {
namespace util {
namespace os {

int32_t get_thread_id() { return syscall(SYS_gettid); }

}  // namespace os
}  // namespace util
}  // namespace slog
