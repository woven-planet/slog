#ifndef slog_cc_util_inline_macro
#define slog_cc_util_inline_macro

namespace slog {

// We need to specify this long instruction to ensure inlining, using short
// macro to remove boiler plate.
#define SLOG_INLINE inline __attribute__((always_inline))

}  // namespace slog

#endif
