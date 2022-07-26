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

#ifndef slog_cc_util_inline_macro
#define slog_cc_util_inline_macro

namespace slog {

// We need to specify this long instruction to ensure inlining, using short
// macro to remove boiler plate.
#define SLOG_INLINE inline __attribute__((always_inline))

}  // namespace slog

#endif
