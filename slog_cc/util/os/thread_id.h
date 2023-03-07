#ifndef slog_cc_util_os_thread_id
#define slog_cc_util_od_thread_id

#include <cstdint>

namespace slog {
namespace util {
namespace os {

// Return an integer ID of a current thread.
int32_t get_thread_id();

}  // namespace os
}  // namespace util
}  // namespace slog

#endif
