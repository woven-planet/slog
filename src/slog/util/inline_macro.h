#pragma once

namespace avsoftware {

// We need to specify this long instruction to ensure inlining, using short
// macro to remove boiler plate.
#define SLOG_INLINE inline __attribute__((always_inline))

}  // namespace avsoftware
