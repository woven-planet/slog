#ifndef slog_cc_util_os_thread_id
#define slog_cc_util_os_thread_id

#include <cstdint>
#include <string>

namespace slog {
namespace util {
namespace os {

// Return an integer ID of a current thread.
int32_t get_thread_id();

// Return an string name of a current thread.
std::string get_thread_name(int32_t thread_id);

}  // namespace os
}  // namespace util
}  // namespace slog

#endif
