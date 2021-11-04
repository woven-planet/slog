#include "src/primitives/tag.h"

#include <sstream>
#include <string>

#include "src/util/inline_macro.h"

namespace avsoftware {

template <size_t actual, size_t expect>
struct assert_size_SlogTag {
  static_assert(actual == expect,
                "sizeof(SlogTag) changed that may significantly affect "
                "performance, please run "
                "slog_benchmark before committing this change to master.");
};
assert_size_SlogTag<80, sizeof(SlogTag)> do_assert_size_SlogTag;

}  // namespace avsoftware
