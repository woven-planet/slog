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

#ifndef slog_cc_buffer_buffer
#define slog_cc_buffer_buffer

#include <mutex>
#include <vector>

#include "slog_cc/buffer/buffer_data.h"
#include "slog_cc/context/subscribers.h"

namespace slog {

class SlogContext;
class SlogRecord;

// SlogBuffer instance subscribes on Slog events stream and copies them to
// internal buffer. User can flush the buffer when needed, E.g. they can flush
// buffer periodically and serialize to a file. This is a building block for the
// Hermes / Lyftbag loggers.
class SlogBuffer {
 public:
  // Create buffer and subscribes to Slog events stream.
  SlogBuffer(std::shared_ptr<SlogContext> slog_context);

  // Returns a list of all accumulated slog events.
  SlogBufferData flush();

  // Returns the current number of events in the buffer.
  size_t size() const;

  // Blocks current thread execution until all slog messages emitted by the
  // moment of this call are processed by all async subscribers.
  void waitSlogQueue() const;

 private:
  mutable std::mutex mutex_;
  std::shared_ptr<SlogContext> slog_context_;
  std::vector<SlogRecord> buffer_;
  SlogSubscriber slog_subscriber_;
};

}  // namespace slog

#endif
