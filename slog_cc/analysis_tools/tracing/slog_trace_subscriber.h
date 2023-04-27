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
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <string>

#include "slog_cc/context/context.h"

namespace slog {

struct GlobalScopeId {
  int32_t thread_id;
  int64_t scope_id;

  bool operator<(const GlobalScopeId& other) const {
    if (thread_id != other.thread_id) {
      return thread_id < other.thread_id;
    }
    if (scope_id != other.scope_id) {
      return scope_id < other.scope_id;
    }
    return false;
  }
};

struct SlogTraceSubscriberState {
  std::ofstream file;
  std::map<GlobalScopeId, std::string> scope_id_to_name;
  std::set<int32_t> logged_thread_names;
  int64_t min_ts_ns = -1;

  ~SlogTraceSubscriberState() { file << "\n]}\n"; }
};

struct SlogTraceSubscriber {
  std::shared_ptr<SlogTraceSubscriberState> state;
  SlogSubscriber slog_subscriber;
};

SlogTraceSubscriber CreateSlogTraceSubscriber(
    const std::string& slog_trace_json_filepath);

}  // namespace slog

#endif
