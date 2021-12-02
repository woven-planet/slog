#pragma once

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
