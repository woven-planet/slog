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
