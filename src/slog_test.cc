#include "src/slog.h"

#include <future>
#include <vector>

#include <gtest/gtest.h>

#include "src/context/context.h"
#include "src/primitives/call_site.h"
#include "src/util/string_util.h"

namespace avsoftware {

class SlogTest : public ::testing::Test {
 public:
  void SetUp() override {
    slog_records_.clear();
    slog_subscribers_.clear();
    slog_subscribers_.push_back(
        SlogContext::getInstance().createAsyncSubscriber(
            [this](const SlogRecord& record) {
              std::unique_lock<std::mutex> lock(slog_records_mutex_);
              slog_records_.push_back(record);
            }));
    SlogContext::getInstance().resetCallSites();
  }

  void TearDown() override {
    SlogContext::getInstance().resetAsyncNotificationQueue();
  }

  template <class T>
  static SlogTag getTag(const T& tags, const std::string& key) {
    for (const auto& tag : tags) {
      if (tag.key() == key) {
        return tag;
      }
    }
    LOG(FATAL) << "Tag " << key << " not found.";
    return SlogTag("", "");
  }

  template <class T>
  static size_t countTags(const T& tags, const std::string& key) {
    size_t res = 0;
    for (const auto& tag : tags) {
      if (tag.key() == key) {
        res += 1;
      }
    }
    return res;
  }

  static std::pair<bool, std::string> isAscending(
      const std::vector<SlogRecord>& slog_records) {
    for (size_t i = 1; i < slog_records.size(); ++i) {
      const auto& prev = slog_records[i - 1];
      const auto& cur = slog_records[i];
      if (prev.time().elapsed_ns > cur.time().elapsed_ns) {
        return {false,
                util::stringPrintf(
                    "time_elapsed_ns[%d] = %lld, time_elapsed_ns[%d] = "
                    "%lld\nprev: %s\n cur: %s\n\n",
                    i - 1, prev.time().elapsed_ns, i, cur.time().elapsed_ns,
                    SlogPrinter::debugString(prev).c_str(),
                    SlogPrinter::debugString(cur).c_str())};
      }
    }
    return {true, ""};
  }

  void waitSlog() { SlogContext::getInstance().waitAsyncSubscribers(); }

 protected:
  std::mutex slog_records_mutex_;
  std::vector<SlogRecord> slog_records_;
  std::vector<SlogSubscriber> slog_subscribers_;
};

#define ASSERT_ASCENDING(expected, slog_records) \
  {                                              \
    const auto r = isAscending(slog_records);    \
    ASSERT_EQ(expected, r.first) << r.second;    \
  }

TEST_F(SlogTest, basic) {
  ASSERT_EQ(0, slog_records_.size());
  SLOG(INFO);

  waitSlog();
  ASSERT_EQ(1, slog_records_.size());

  EXPECT_EQ("", SlogPrinter::slogText(slog_records_.back()));
  {
    const int32_t call_site_id = 1;
    EXPECT_EQ(call_site_id, slog_records_.back().call_site_id());
    const SlogCallSite call_site =
        SlogContext::getInstance().getCallSite(call_site_id);
    EXPECT_EQ("src/slog_test.cc", call_site.file());
    EXPECT_EQ(__LINE__ - 12, call_site.line());
    EXPECT_EQ("TestBody", call_site.function());
  }
  SLOG(INFO) << "basic message";

  waitSlog();
  ASSERT_EQ(2, slog_records_.size());

  EXPECT_EQ("basic message", SlogPrinter::slogText(slog_records_.back()));
  EXPECT_EQ("basic message", SlogPrinter::flatText(slog_records_.back()));
  {
    const int32_t call_site_id = 2;
    EXPECT_EQ(call_site_id, slog_records_.back().call_site_id());
    const SlogCallSite call_site =
        SlogContext::getInstance().getCallSite(call_site_id);
    EXPECT_EQ("src/slog_test.cc", call_site.file());
    EXPECT_EQ(__LINE__ - 13, call_site.line());
    EXPECT_EQ("TestBody", call_site.function());
  }
}

TEST_F(SlogTest, scope_id) {
  ASSERT_EQ(0, slog_records_.size());
  auto f = [] { SLOG(INFO); };
  f();

  waitSlog();
  ASSERT_EQ(1, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().call_site_id());
  SLOG(INFO);

  waitSlog();
  ASSERT_EQ(2, slog_records_.size());

  EXPECT_EQ(2, slog_records_.back().call_site_id());
  f();

  waitSlog();
  ASSERT_EQ(3, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().call_site_id());
}

TEST_F(SlogTest, empty_tags) {
  SLOG(INFO).addTag("t1");

  waitSlog();
  ASSERT_EQ(1, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().tags().size());
  EXPECT_EQ("t1", slog_records_.back().tags().back().key());
  EXPECT_EQ(SlogTagValueType::kNone,
            slog_records_.back().tags().back().valueType());
  EXPECT_EQ("", slog_records_.back().tags().back().valueString());
  EXPECT_EQ(0, slog_records_.back().tags().back().valueInt());
  EXPECT_EQ(0.0, slog_records_.back().tags().back().valueDouble());

  SLOG(INFO).addTag("t2", "");

  waitSlog();
  ASSERT_EQ(2, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().tags().size());
  EXPECT_EQ("t2", slog_records_.back().tags().back().key());
  EXPECT_EQ(SlogTagValueType::kString,
            slog_records_.back().tags().back().valueType());
  EXPECT_EQ("", slog_records_.back().tags().back().valueString());
  EXPECT_EQ(0, slog_records_.back().tags().back().valueInt());
  EXPECT_EQ(0.0, slog_records_.back().tags().back().valueDouble());

  SLOG(INFO).addTag("t3", 0);

  waitSlog();
  ASSERT_EQ(3, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().tags().size());
  EXPECT_EQ("t3", slog_records_.back().tags().back().key());
  EXPECT_EQ(SlogTagValueType::kInt,
            slog_records_.back().tags().back().valueType());
  EXPECT_EQ("", slog_records_.back().tags().back().valueString());
  EXPECT_EQ(0, slog_records_.back().tags().back().valueInt());
  EXPECT_EQ(0.0, slog_records_.back().tags().back().valueDouble());

  SLOG(INFO).addTag("t4", 0.0);

  waitSlog();
  ASSERT_EQ(4, slog_records_.size());

  EXPECT_EQ(1, slog_records_.back().tags().size());
  EXPECT_EQ("t4", slog_records_.back().tags().back().key());
  EXPECT_EQ(SlogTagValueType::kDouble,
            slog_records_.back().tags().back().valueType());
  EXPECT_EQ("", slog_records_.back().tags().back().valueString());
  EXPECT_EQ(0, slog_records_.back().tags().back().valueInt());
  EXPECT_EQ(0.0, slog_records_.back().tags().back().valueDouble());
}

TEST_F(SlogTest, silent_tag) {
  ASSERT_EQ(0, slog_records_.size());
  SLOG(INFO)
      .addTag("silent_tag_1")
      .addTag("silent_tag_2", "silent_tag_2_value");

  waitSlog();
  ASSERT_EQ(1, slog_records_.size());

  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "silent_tag_1"));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "silent_tag_2"));
  EXPECT_EQ(0, countTags(slog_records_.back().tags(), "silent_tag_3"));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag("silent_tag_1", SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(
                getTag(slog_records_.back().tags(), "silent_tag_1")));
  EXPECT_EQ(
      SlogPrinter::debugString(SlogTag("silent_tag_2", "silent_tag_2_value",
                                       SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(
          getTag(slog_records_.back().tags(), "silent_tag_2")));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_.back()));
  EXPECT_EQ("", SlogPrinter::flatText(slog_records_.back()));
}

TEST_F(SlogTest, noisy_tag) {
  SLOG(INFO) << "tag: " << SlogTag("noisy_tag", "noisy_tag_value");

  waitSlog();
  ASSERT_EQ(1, slog_records_.size());

  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "noisy_tag"));
  EXPECT_EQ(SlogPrinter::debugString(SlogTag("noisy_tag", "noisy_tag_value",
                                             SlogTagVerbosity::kNoisy)),
            SlogPrinter::debugString(
                getTag(slog_records_.back().tags(), "noisy_tag")));
  EXPECT_EQ("tag: <noisy_tag>", SlogPrinter::slogText(slog_records_.back()));
  EXPECT_EQ("tag: noisy_tag_value",
            SlogPrinter::flatText(slog_records_.back()));
  SLOG(INFO) << "Hi " << SlogTag("username", "Neo") << ". Follow the "
             << std::string("rabbit") << " number " << 4 << 2 << " in " << 0.5
             << " hours.";

  waitSlog();
  ASSERT_EQ(2, slog_records_.size());

  EXPECT_EQ(
      "Hi <username>. Follow the <_t0_s> number <_t1_i><_t2_i> in <_t3_f> "
      "hours.",
      SlogPrinter::slogText(slog_records_.back()));
  EXPECT_EQ("Hi Neo. Follow the rabbit number 42 in 0.500000 hours.",
            SlogPrinter::flatText(slog_records_.back()));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "username"));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "_t0_s"));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "_t1_i"));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "_t3_f"));
  EXPECT_EQ(1, countTags(slog_records_.back().tags(), "_t2_i"));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag("username", "Neo", SlogTagVerbosity::kNoisy)),
            SlogPrinter::debugString(
                getTag(slog_records_.back().tags(), "username")));
  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag("_t0_s", "rabbit", SlogTagVerbosity::kNoisy)),
      SlogPrinter::debugString(getTag(slog_records_.back().tags(), "_t0_s")));
  EXPECT_EQ(
      SlogPrinter::debugString(SlogTag("_t1_i", 4, SlogTagVerbosity::kNoisy)),
      SlogPrinter::debugString(getTag(slog_records_.back().tags(), "_t1_i")));
  EXPECT_EQ(
      SlogPrinter::debugString(SlogTag("_t2_i", 2, SlogTagVerbosity::kNoisy)),
      SlogPrinter::debugString(getTag(slog_records_.back().tags(), "_t2_i")));
  EXPECT_EQ(
      SlogPrinter::debugString(SlogTag("_t3_f", 0.5, SlogTagVerbosity::kNoisy)),
      SlogPrinter::debugString(getTag(slog_records_.back().tags(), "_t3_f")));
}

TEST_F(SlogTest, scope) {
  int scope_a_line = -1, scope_b_line = -1;
  const char* kMyTag = "mytag";
  EXPECT_EQ(0, SlogScope::currentScopeDepth());
  {
    ASSERT_EQ(0, slog_records_.size());
    SLOG_SCOPE("scope_a");
    scope_a_line = __LINE__ - 1;
    EXPECT_EQ(1, SlogScope::currentScopeDepth());
    waitSlog();
    ASSERT_EQ(1, slog_records_.size());

    {
      SLOG_SCOPE("scope_b");
      scope_b_line = __LINE__ - 1;
      EXPECT_EQ(2, SlogScope::currentScopeDepth());
      waitSlog();
      ASSERT_EQ(2, slog_records_.size());
    }
    EXPECT_EQ(1, SlogScope::currentScopeDepth());

    {
      SLOG_SCOPE("scope_c").addTag(kMyTag, "My tag value");
      EXPECT_EQ(2, SlogScope::currentScopeDepth());
      waitSlog();
      ASSERT_EQ(4, slog_records_.size());
    }
    EXPECT_EQ(1, SlogScope::currentScopeDepth());

    waitSlog();
    ASSERT_EQ(5, slog_records_.size());
  }
  EXPECT_EQ(0, SlogScope::currentScopeDepth());

  waitSlog();
  ASSERT_EQ(6, slog_records_.size());

  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_name", "scope_a", SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[0].tags(), ".scope_name")));
  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_id", 1, SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[0].tags(), ".scope_id")));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag(kSlogTagKeyScopeDepth, 1, SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(
                getTag(slog_records_[0].tags(), kSlogTagKeyScopeDepth)));
  EXPECT_EQ(1, countTags(slog_records_[0].tags(), ".scope_open"));
  EXPECT_EQ(0, countTags(slog_records_[0].tags(), ".scope_close"));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_[0]));
  {
    const int32_t call_site_id = 1;
    EXPECT_EQ(call_site_id, slog_records_[0].call_site_id());
    const SlogCallSite call_site =
        SlogContext::getInstance().getCallSite(call_site_id);
    EXPECT_EQ("src/slog_test.cc", call_site.file());
    EXPECT_EQ(scope_a_line, call_site.line());
    EXPECT_EQ("TestBody", call_site.function());
  }

  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_name", "scope_b", SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[1].tags(), ".scope_name")));
  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_id", 2, SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[1].tags(), ".scope_id")));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag(kSlogTagKeyScopeDepth, 2, SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(
                getTag(slog_records_[1].tags(), kSlogTagKeyScopeDepth)));
  EXPECT_EQ(1, countTags(slog_records_[1].tags(), ".scope_open"));
  EXPECT_EQ(0, countTags(slog_records_[1].tags(), ".scope_close"));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_[1]));
  EXPECT_EQ(2, slog_records_[1].call_site_id());
  {
    const int32_t call_site_id = 2;
    EXPECT_EQ(call_site_id, slog_records_[1].call_site_id());
    const SlogCallSite call_site =
        SlogContext::getInstance().getCallSite(call_site_id);
    EXPECT_EQ("src/slog_test.cc", call_site.file());
    EXPECT_EQ(scope_b_line, call_site.line());
    EXPECT_EQ("TestBody", call_site.function());
  }

  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag(kMyTag, "My tag value", SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(getTag(slog_records_[3].tags(), kMyTag)));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag(kSlogTagKeyScopeId, 3, SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(
                getTag(slog_records_[3].tags(), kSlogTagKeyScopeId)));
  EXPECT_EQ(SlogPrinter::debugString(
                SlogTag(kSlogTagKeyScopeDepth, 2, SlogTagVerbosity::kSilent)),
            SlogPrinter::debugString(
                getTag(slog_records_[3].tags(), kSlogTagKeyScopeDepth)));

  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_id", 3, SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[4].tags(), ".scope_id")));
  EXPECT_EQ(0, countTags(slog_records_[4].tags(), ".scope_name"));
  EXPECT_EQ(0, countTags(slog_records_[4].tags(), ".scope_open"));
  EXPECT_EQ(1, countTags(slog_records_[4].tags(), ".scope_close"));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_[4]));
  EXPECT_EQ(0, slog_records_[4].call_site_id());
  EXPECT_EQ("", SlogContext::getInstance().getCallSite(0).file());
  EXPECT_EQ(0, SlogContext::getInstance().getCallSite(0).line());
  EXPECT_EQ("", SlogContext::getInstance().getCallSite(0).function());

  EXPECT_EQ(
      SlogPrinter::debugString(
          SlogTag(".scope_id", 1, SlogTagVerbosity::kSilent)),
      SlogPrinter::debugString(getTag(slog_records_[5].tags(), ".scope_id")));
  EXPECT_EQ(0, countTags(slog_records_[5].tags(), ".scope_name"));
  EXPECT_EQ(0, countTags(slog_records_[5].tags(), ".scope_open"));
  EXPECT_EQ(1, countTags(slog_records_[5].tags(), ".scope_close"));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_[5]));
  EXPECT_EQ("", SlogPrinter::slogText(slog_records_[5]));
  EXPECT_EQ(0, slog_records_[5].call_site_id());
  EXPECT_EQ("", SlogContext::getInstance().getCallSite(0).file());
  EXPECT_EQ(0, SlogContext::getInstance().getCallSite(0).line());
  EXPECT_EQ("", SlogContext::getInstance().getCallSite(0).function());

  ASSERT_ASCENDING(true, slog_records_);
}

TEST_F(SlogTest, func_block) {
  {
    ASSERT_EQ(0, slog_records_.size());
    SLOG_SCOPE("scope_a");

    waitSlog();
    ASSERT_EQ(1, slog_records_.size());

    EXPECT_EQ(1, slog_records_.back().call_site_id());
    SLOG_FUNC_BLOCK_START("f1");

    waitSlog();
    ASSERT_EQ(2, slog_records_.size());

    EXPECT_EQ(2, slog_records_.back().call_site_id());
  }

  waitSlog();
  ASSERT_EQ(3, slog_records_.size());

  EXPECT_EQ(0, slog_records_.back().call_site_id());
}

TEST_F(SlogTest, races) {
  ASSERT_EQ(0, slog_records_.size());
  constexpr int kNumThreads = 100;
  constexpr int kNumScopes = 100;
  std::vector<std::future<void>> futures;
  for (int i = 0; i < kNumThreads; ++i) {
    futures.emplace_back(std::async(std::launch::async, [i] {
      for (int j = 0; j < kNumScopes; ++j) {
        SLOG_SCOPE("thread_" + std::to_string(i) + ".scope_" +
                   std::to_string(j));
        usleep(10);
      }
    }));
  }
  for (auto& f : futures) {
    f.get();
  }

  waitSlog();
  ASSERT_EQ(2 * kNumThreads * kNumScopes, slog_records_.size());

  ASSERT_ASCENDING(false, slog_records_);
  std::map<int64_t, std::vector<SlogRecord>> thread_slog_records;
  for (const auto& record : slog_records_) {
    thread_slog_records[record.thread_id()].push_back(record);
  }
  EXPECT_EQ(kNumThreads, thread_slog_records.size());
  for (const auto& item : thread_slog_records) {
    EXPECT_EQ(2 * kNumScopes, item.second.size());
    ASSERT_ASCENDING(true, item.second);
  }
}

TEST_F(SlogTest, severity) {
  SLOG(INFO) << "info";
  waitSlog();
  ASSERT_EQ(1, slog_records_.size());
  EXPECT_EQ(google::INFO, slog_records_.back().severity());

  SLOG(WARNING) << "warning";
  waitSlog();
  ASSERT_EQ(2, slog_records_.size());
  EXPECT_EQ(google::WARNING, slog_records_.back().severity());

  SLOG(ERROR) << "error";
  waitSlog();
  ASSERT_EQ(3, slog_records_.size());
  EXPECT_EQ(google::ERROR, slog_records_.back().severity());

  ASSERT_DEATH(SLOG(FATAL) << "fatal", "fatal");
  // ASSERT_DEATH runs in a forked process so the generated log message is
  // handled with another SlogContext instance and in current process no new
  // messages are added.
  ASSERT_EQ(3, slog_records_.size());

  ASSERT_DEATH(SLOG(FATAL), "");
  // silent SLOG(FATAL) should trigget LOG(FATAL) as well, while non-fatal
  // silent calls skip GLOG to avoid extra latencies.
  ASSERT_EQ(3, slog_records_.size());
}

TEST_F(SlogTest, fast_callback) {
  // This test emits 1M of slog messages and registers a subscriber with a fast
  // callback. This test should take around 1 second of time but may hang and
  // fail with TIMEOUT if we have a bug.
  int cnt = 0;
  auto slow_callback_subscriber =
      SlogContext::getInstance().createAsyncSubscriber(
          [&cnt](const SlogRecord&) { cnt += 1; });
  for (int i = 0; i < 1000 * 1000; ++i) {
    SLOG(INFO).addTag("test-fast-callback");
  }
  waitSlog();
  ASSERT_EQ(1000000, cnt);
}

TEST_F(SlogTest, slow_callback) {
  // This test emits 1M of slog messages and registers a subscriber with a slow
  // callback (1 second per message). If subscriber is not handled correctly
  // this test will take very long time and will fail with TIMOUT.
  auto slow_callback_subscriber =
      SlogContext::getInstance().createAsyncSubscriber(
          [](const SlogRecord&) { usleep(1000 * 1000); });
  for (int i = 0; i < 1000 * 1000; ++i) {
    SLOG(INFO).addTag("test-slow-callback");
  }
  // No need to wait for all slog messages to be processed.
}

}  // namespace avsoftware
