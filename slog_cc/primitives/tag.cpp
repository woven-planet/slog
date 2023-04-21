// Copyright 2022 Woven Planet Holdings
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

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
assert_size_SlogTag<2 * sizeof(std::string) + 16, sizeof(SlogTag)>
    do_assert_size_SlogTag;

}  // namespace slog
