#pragma once

#include <mutex>
#include <vector>

#include "src/slog/buffer/buffer_data.h"
#include "src/slog/context/subscribers.h"

namespace avsoftware {

class SlogRecord;

// SlogBuffer instance subscribes on Slog events stream and copies them to
// internal buffer. User can flush the buffer when needed, E.g. they can flush
// buffer periodically and serialize to a file. This is a building block for the
// Hermes / Lyftbag loggers.
class SlogBuffer {
 public:
  // Create buffer and subscribes to Slog events stream.
  SlogBuffer();

  // Returns a list of all accumulated slog events.
  SlogBufferData flush();

  // Returns the current number of events in the buffer.
  size_t size() const;

  // Blocks current thread execution until all slog messages emitted by the
  // moment of this call are processed by all async subscribers.
  void waitSlogQueue() const;

 private:
  mutable std::mutex mutex_;
  std::vector<SlogRecord> buffer_;
  SlogSubscriber slog_subscriber_;
};

}  // namespace avsoftware
