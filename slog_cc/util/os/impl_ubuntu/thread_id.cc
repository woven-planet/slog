#include "../thread_id.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <sys/prctl.h>

namespace slog {
namespace util {
namespace os {

int32_t get_thread_id() { return syscall(SYS_gettid); }

std::string get_thread_name(int32_t thread_id) {
  constexpr int max_size = 16;
  char buffer[max_size] = "";
  prctl(PR_GET_NAME, buffer);
  return buffer;
}

}  // namespace os
}  // namespace util
}  // namespace slog
