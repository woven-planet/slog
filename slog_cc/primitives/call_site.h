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

#ifndef slog_cc_primitives_call_site
#define slog_cc_primitives_call_site

#include <string>

#include "slog_cc/util/inline_macro.h"

namespace slog {

class SlogCallSite {
 public:
  SLOG_INLINE SlogCallSite(const std::string& function, const std::string& file,
                           int32_t line)
      : function_(function), file_(file), line_(line) {}

  SLOG_INLINE const std::string& function() const { return function_; }
  SLOG_INLINE const std::string& file() const { return file_; }
  SLOG_INLINE int32_t line() const { return line_; }

 private:
  std::string function_;
  std::string file_;
  int32_t line_;
};

}  // namespace slog

#endif
