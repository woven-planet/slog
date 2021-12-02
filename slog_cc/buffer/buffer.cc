#include "slog_cc/buffer/buffer.h"

#include <mutex>
#include <vector>

#include "slog_cc/events/event.h"

namespace slog {

SlogBuffer::SlogBuffer()
    : slog_subscriber_(SlogContext::getInstance().createAsyncSubscriber(
          [this](const SlogRecord& record) {
            std::unique_lock<std::mutex> lock(mutex_);
            buffer_.emplace_back(record);
          })) {}

SlogBufferData SlogBuffer::flush() {
  SlogBufferData res;
  {
    std::unique_lock<std::mutex> lock(mutex_);
    res.records.swap(buffer_);
  }
  const size_t num_call_sites = SlogContext::getInstance().numCallSites();
  for (size_t i = 0; i < num_call_sites; ++i) {
    res.call_sites.emplace_back(SlogContext::getInstance().getCallSite(i));
  }
  return res;
}

size_t SlogBuffer::size() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return buffer_.size();
}

void SlogBuffer::waitSlogQueue() const {
  SlogContext::getInstance().waitAsyncSubscribers();
}

}  // namespace slog
