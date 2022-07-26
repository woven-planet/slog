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

#include "slog_cc/context/subscribers.h"

#include <unistd.h>

namespace slog {

SlogSubscriber SlogContextSubscribers::create(const SlogCallback& callback) {
  return SlogSubscriber(new SlogCallbackId(addCallback(callback)),
                        [this](SlogCallbackId* p) {
                          removeCallback(*p);
                          delete p;
                        });
}

SlogCallbackId SlogContextSubscribers::addCallback(
    const SlogCallback& callback) {
  std::unique_lock<std::mutex> next_access_lock(next_access_mutex_);
  std::unique_lock<std::mutex> data_lock(data_mutex_);
  next_access_lock.unlock();

  auto new_callbacks =
      std::make_shared<std::vector<std::shared_ptr<SlogCallback>>>(*callbacks_);
  new_callbacks->emplace_back(new SlogCallback(callback));
  callbacks_ = new_callbacks;
  return callbacks_->back().get();
}

void SlogContextSubscribers::removeCallback(SlogCallbackId callback_id) {
  std::unique_lock<std::mutex> next_access_lock(next_access_mutex_);
  std::unique_lock<std::mutex> data_lock(data_mutex_);
  next_access_lock.unlock();

  auto new_callbacks =
      std::make_shared<std::vector<std::shared_ptr<SlogCallback>>>();
  for (const std::shared_ptr<SlogCallback>& item : *callbacks_) {
    if (item.get() != callback_id) {
      new_callbacks->push_back(item);
    }
  }
  callbacks_ = new_callbacks;
}

}  // namespace slog
