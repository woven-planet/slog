#include "slog_cc/buffer/buffer.h"

#include <future>
#include <vector>

#include <gtest/gtest.h>

#include "slog_cc/context/context.h"
#include "slog_cc/slog.h"

namespace slog {

class SlogBufferTest : public ::testing::Test {
 public:
  void SetUp() override {
    slog_records_.clear();
    slog_subscribers_.clear();
    slog_subscribers_.push_back(
        slog_context_->createAsyncSubscriber([this](const SlogRecord& record) {
          std::unique_lock<std::mutex> lock(slog_records_mutex_);
          slog_records_.push_back(record);
        }));
    slog_context_->resetCallSites();
  }

  void TearDown() override { slog_context_->resetAsyncNotificationQueue(); }

  void waitSlog() { slog_context_->waitAsyncSubscribers(); }

 protected:
  std::shared_ptr<SlogContext> slog_context_ = SlogContext::getInstance();
  std::mutex slog_records_mutex_;
  std::vector<SlogRecord> slog_records_;
  std::vector<SlogSubscriber> slog_subscribers_;
};

TEST_F(SlogBufferTest, slog_buffer) {
  SLOG(INFO) << "foo";
  waitSlog();
  ASSERT_EQ(1, slog_records_.size());
  {
    SlogBuffer slog_buffer(slog_context_);
    {
      auto buffer_data = slog_buffer.flush();
      EXPECT_EQ(0, buffer_data.records.size());
      EXPECT_EQ(2, buffer_data.call_sites.size());
    }

    SLOG(INFO) << "bar";
    waitSlog();
    ASSERT_EQ(2, slog_records_.size());
    {
      auto buffer_data = slog_buffer.flush();
      EXPECT_EQ(1, buffer_data.records.size());
      EXPECT_EQ(3, buffer_data.call_sites.size());
    }
    {
      auto buffer_data = slog_buffer.flush();
      EXPECT_EQ(0, buffer_data.records.size());
      EXPECT_EQ(3, buffer_data.call_sites.size());
    }
  }
  SLOG(INFO) << "no segfaults";
}

TEST_F(SlogBufferTest, races) {
  std::vector<std::future<void>> tasks;
  for (int i = 0; i < 1000; ++i) {
    tasks.push_back(std::async(std::launch::async, [this] {
      SlogBuffer b(slog_context_);
      for (int j = 0; j < 10; ++j) {
        SLOG_SCOPE("foo");
        usleep(rand() % 4 * 1000);
      }
    }));
  }
  for (auto& task : tasks) {
    task.get();
  }
  SLOG(INFO) << "no segfaults";
}

}  // namespace slog
