#include "../thread_id.h"

#include <process.h>
#include <pthread.h>

namespace slog {
namespace util {
namespace os {

int32_t get_thread_id() { return gettid(); }

std::string get_thread_name(int32_t thread_id) {
  constexpr int max_size = 16;
  char buffer[max_size] = "";
  pthread_getname_np(thread_id, buffer, max_size);
  return buffer;
}

}  // namespace os
}  // namespace util
}  // namespace slog
