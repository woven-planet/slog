#include "src/slog/context/subscribers.h"

#include <unistd.h>

namespace avsoftware {

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
  for (auto it = callbacks_->begin(); it != callbacks_->end(); ++it) {
    if (it->get() != callback_id) {
      new_callbacks->push_back(*it);
    }
  }
  callbacks_ = new_callbacks;
}

}  // namespace avsoftware
