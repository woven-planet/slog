#ifndef slog_cc_util_assert_macro
#define slog_cc_util_assert_macro

#include <iostream>

namespace slog {

#define SLOG_ASSERT(e)                                                       \
  if (!(e)) {                                                                \
    std::cerr << __FILE__ << ":" << __LINE__ << "] Assertion failed: " << #e \
              << std::endl;                                                  \
    abort();                                                                 \
  }

}  // namespace slog

#endif
