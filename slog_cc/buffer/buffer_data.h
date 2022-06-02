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

#ifndef slog_cc_buffer_buffer_data
#define slog_cc_buffer_buffer_data

#include <memory>
#include <vector>

#include "slog_cc/primitives/call_site.h"
#include "slog_cc/primitives/record.h"

namespace slog {

// Encapsulation of a list of slog records and call sites map at the moment of
// flushing the buffer.
struct SlogBufferData {
  std::vector<SlogRecord> records;
  std::vector<SlogCallSite> call_sites;
};

}  // namespace slog

#endif
