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

#include <gtest/gtest.h>
#include <slog_cc/analysis_tools/tracing/slog_trace_subscriber.h>
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

TEST(SlogTest, chrome_tracing_build) {
  // While slog trace subscriber object exists it will append chrome tracing
  // records to the provided json file. On destruction it will finalize the
  // file.
  auto slog_trace_subscriber =
      slog::CreateSlogTraceSubscriber("/tmp/slog-trace.json");

  // Actual code with slogs body.
  {
    SLOG_SCOPE("foo_scope");
    { SLOG_SCOPE("bar_scope"); }
  }

  // Need to wait all slog records to be processed to make sure all records were
  // flushed and written to the slog json file.
  slog::SlogContext::getInstance()->waitAsyncSubscribers();
}
