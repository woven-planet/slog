#pragma once

#include <iostream>

namespace slog {

#define SLOG_ASSERT(e)                                                       \
  if (!(e)) {                                                                \
    std::cerr << __FILE__ << ":" << __LINE__ << "] Assertion failed: " << #e \
              << std::endl;                                                  \
    abort();                                                                 \
  }

}  // namespace slog
