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

#ifndef slog_cc_primitives_timestamps
#define slog_cc_primitives_timestamps

namespace slog {

enum class SlogGlobalClockTypeId {
  // A regiular pc realtime clock.
  kWallTimeClock,

  // A user-defined clock producing timestamps in GPS epoch.
  kGpsEpochClock
};

struct SlogTimestamps {
  // Monotonic timestamp in nanoseconds. Used to compute latencies by
  // substracting these timestamps from different events.
  int64_t elapsed_ns = -1;

  // Timestamp in nanoseconds that is used to map event to global time. What
  // clock and epoch to use stays up to a user definition. For our usecase we
  // are using GPS timestamp sourced by TimingBox.
  int64_t global_ns = -1;

  // Global timestamp might be generated via different clock type (e.g. PC clock
  // or GPS clock).
  SlogGlobalClockTypeId global_clock_type_id =
      SlogGlobalClockTypeId::kWallTimeClock;
};

}  // namespace slog

#endif
