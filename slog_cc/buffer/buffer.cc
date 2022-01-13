#include "slog_cc/buffer/buffer.h"

#include <mutex>
#include <vector>

#include "slog_cc/events/event.h"

namespace slog {

SlogBuffer::SlogBuffer(std::shared_ptr<SlogContext> slog_context)
    : slog_context_(slog_context),
      slog_subscriber_(slog_context_->createAsyncSubscriber(
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
  const size_t num_call_sites = slog_context_->numCallSites();
  for (size_t i = 0; i < num_call_sites; ++i) {
    res.call_sites.emplace_back(slog_context_->getCallSite(i));
  }
  return res;
}

size_t SlogBuffer::size() const {
  std::unique_lock<std::mutex> lock(mutex_);
  return buffer_.size();
}

void SlogBuffer::waitSlogQueue() const {
  slog_context_->waitAsyncSubscribers();
}

}  // namespace slog
