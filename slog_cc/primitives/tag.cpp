#include "slog_cc/primitives/tag.h"

#include <sstream>
#include <string>

#include "slog_cc/util/inline_macro.h"

namespace slog {

template <size_t actual, size_t expect>
struct assert_size_SlogTag {
  static_assert(actual == expect,
                "sizeof(SlogTag) changed that may significantly affect "
                "performance, please run "
                "slog_benchmark before committing this change to master.");
};
assert_size_SlogTag<80, sizeof(SlogTag)> do_assert_size_SlogTag;

}  // namespace slog
