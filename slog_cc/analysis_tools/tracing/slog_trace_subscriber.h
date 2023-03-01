// Copyright 2023 Woven Planet Holdings
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

#ifndef slog_cc_analysis_tools_tracing_slog_trace_subscriber
#define slog_cc_analysis_tools_tracing_slog_trace_subscriber

#include <cstring>
#include <string>
#include <fstream>
#include <map>
#include <memory>

#include "slog_cc/context/context.h"

namespace slog {

struct SlogTraceSubscriberState {
  std::ofstream file;
  std::map<int64_t, std::string> scope_id_to_name;
  int64_t min_ts_ns = -1;

  ~SlogTraceSubscriberState() { file << "\n]}\n"; }
};

struct SlogTraceSubscriber {
  std::shared_ptr<SlogTraceSubscriberState> state;
  SlogSubscriber slog_subscriber;
};

SlogTraceSubscriber CreateSlogTraceSubscriber(const std::string& slog_trace_json_filepath);

}  // namespace slog

#endif
