#include <gtest/gtest.h>
#include <slog_cc/buffer/buffer.h>
#include <slog_cc/slog.h>

TEST(SlogTest, basic) {
  SLOG(INFO) << "foo";
  slog::SlogContext::getInstance()->waitAsyncSubscribers();
  {
    slog::SlogBuffer slog_buffer(slog::SlogContext::getInstance());
    {
      auto buffer_data = slog_buffer.flush();
      EXPECT_EQ(0, buffer_data.records.size());
      EXPECT_EQ(2, buffer_data.call_sites.size());
    }

    SLOG(INFO) << "bar";
    slog::SlogContext::getInstance()->waitAsyncSubscribers();
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
