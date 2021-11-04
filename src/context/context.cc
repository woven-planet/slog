#include "src/context/context.h"

namespace avsoftware {

SlogContext::SlogContext() : get_timestamps_func_(kDefaultGetTimestampsFunc) {
  resetAsyncNotificationQueue();
  resetCallSites();
  echo_to_glog_ = createSyncSubscriber(
      [this](const SlogRecord& record) { echoToGlog(record); });
}

SlogTimestamps SlogContext::getTimestamps() const {
  return get_timestamps_func_();
}

void SlogContext::setGetTimestampsFunc(
    const std::function<SlogTimestamps()>& func) {
  get_timestamps_func_ = func;
}

const std::function<SlogTimestamps()> SlogContext::kDefaultGetTimestampsFunc =
    [] {
      // Different clock have different time to measure and different precision
      // CLOCK_MONOTONIC_RAW takes ~66ns to get time with X ns precision,
      // CLOCK_MONOTONIC takes ~12ns with Y ns precision, and
      // CLOCK_MONOTONIC_COARSE ~2ns with 1 ms precision.
      // Consider overloading with proper implementation to match your
      // performance and precision expectations. Overloading could be done in a
      // following way: SlogContext::getInstance().setGetTimestampsFunc([] {
      //   const int64_t elapsed_ns = myElapsedNs();
      //   const int64_t global_ns = myGloabalNs();
      //   SlogTimestamps{elapsed_ns, global_ns,
      //   SlogGlobalClockTypeId::kWallTimeClock};
      // });
      // The defauls implementation is using CLOCK_MONOTONIC as an option
      // providing average precisions with average cost.
      thread_local struct timespec ts;

      CHECK_NE(-1, clock_gettime(CLOCK_MONOTONIC, &ts));
      const int64_t elapsed_ns = ts.tv_sec * 1000000000 + ts.tv_nsec;

      CHECK_NE(-1, clock_gettime(CLOCK_REALTIME, &ts));
      const int64_t global_ns = ts.tv_sec * 1000000000 + ts.tv_nsec;

      return SlogTimestamps{elapsed_ns, global_ns,
                            SlogGlobalClockTypeId::kWallTimeClock};
    };

int SlogContext::addOrReuseCallSiteVerySlow(const std::string& function,
                                            const std::string& file,
                                            int32_t line) {
  std::unique_lock<std::shared_timed_mutex> lock(call_sites_mutex_);
  for (size_t i = 0; i < call_sites_.size(); ++i) {
    if (line == call_sites_[i]->line() &&
        function == call_sites_[i]->function() &&
        file == call_sites_[i]->file()) {
      return i;
    }
  }
  return addCallSiteUnsafe(function, file, line);
}
}  // namespace avsoftware
