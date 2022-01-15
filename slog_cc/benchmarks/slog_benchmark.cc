#include "slog_cc/slog.h"

#include <benchmark/benchmark.h>
#include <glog/logging.h>

// clang-format off
// Logs emit data to std streams, to run benchmark not printing lots of logs run
// $> bazelisk build slog_cc/benchmarks:slog_benchmark && bazel-bin/slog_cc/benchmarks/slog_benchmark 2>/dev/null
// ---------------------------------------------------------------------------------------
// Benchmark                                             Time             CPU   Iterations
// ---------------------------------------------------------------------------------------
// SlogBenchmark/basic                                 303 ns          303 ns      2301294
// SlogBenchmark/tag_no_value_silent                   353 ns          353 ns      1990034
// SlogBenchmark/tag_int_silent                        350 ns          350 ns      2004105
// SlogBenchmark/tag_double_silent                     351 ns          351 ns      1996570
// SlogBenchmark/tag_string_silent                     371 ns          371 ns      1890267
// SlogBenchmark/two_tag_silent                        399 ns          399 ns      1753460
// SlogBenchmark/scope                                 848 ns          848 ns       822531
// SlogBenchmark/msg_and_10_tags_silent                543 ns          543 ns      1281703
// SlogBenchmark/10_tags_jkey_char_silent              493 ns          493 ns      1408597
// SlogBenchmark/10_tags_jkey_rvalue_str_silent        522 ns          522 ns      1353928
// SlogBenchmark/10_tags_jkey_lvalue_str_silent        518 ns          518 ns      1349733
// SlogBenchmark/10_tags_kv_char_silent                590 ns          590 ns      1189719
// SlogBenchmark/10_tags_kv_rvalue_str_silent          571 ns          571 ns      1228567
// SlogBenchmark/10_tags_kv_lvalue_str_silent          556 ns          555 ns      1248910
// SlogBenchmark/msg_and_10_tags_noisy                5277 ns         5277 ns       133200
// SlogBenchmark/stream_10_chars                      4346 ns         4346 ns       161008
// SlogBenchmark/stream_10_lvalue_strings             5353 ns         5352 ns       130986
// SlogBenchmark/stream_10_rvalue_strings             5318 ns         5318 ns       131863
// SlogBenchmark/10_tags_noisy                        4438 ns         4438 ns       158008
// SlogBenchmark/dummy_str                            4145 ns         4145 ns       168601
// SlogBenchmark/glog_dummy_str                       2430 ns         2430 ns       286799
// SlogBenchmark/slow_callback                         927 ns          927 ns       744835
// clang-format on

namespace slog {

class SlogBenchmark : public benchmark::Fixture {
 public:
  void SetUp(const ::benchmark::State& state) {}

  void TearDown(const ::benchmark::State& state) {
    SlogContext::getInstance()->resetAsyncNotificationQueue();
  }
};

BENCHMARK_F(SlogBenchmark, basic)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO);
  }
}

BENCHMARK_F(SlogBenchmark, tag_no_value_silent)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO).addTag("tag");
  }
}

BENCHMARK_F(SlogBenchmark, tag_int_silent)(benchmark::State& state) {
  const int64_t value = 42;
  for (auto _ : state) {
    SLOG(INFO).addTag("tag", value);
  }
}

BENCHMARK_F(SlogBenchmark, tag_double_silent)(benchmark::State& state) {
  const double value = 42.73;
  for (auto _ : state) {
    SLOG(INFO).addTag("tag", value);
  }
}

BENCHMARK_F(SlogBenchmark, tag_string_silent)(benchmark::State& state) {
  const std::string str = "dummy_message";
  for (auto _ : state) {
    SLOG(INFO).addTag("tag", str);
  }
}

// T(BM_two_tags_noisy) - T(BM_tag_noisy) == time to add extra silent tag.
BENCHMARK_F(SlogBenchmark, two_tag_silent)(benchmark::State& state) {
  const std::string str = "dummy_message";
  for (auto _ : state) {
    SLOG(INFO).addTag("tag1", str).addTag("tag2", str);
  }
}

BENCHMARK_F(SlogBenchmark, scope)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG_SCOPE("benchmark_scope");
  }
}

BENCHMARK_F(SlogBenchmark, msg_and_10_tags_silent)(benchmark::State& state) {
  const std::string str = "Neo";
  for (auto _ : state) {
    SLOG(INFO)
        .addTag("", "Hi ")
        .addTag("username", str)
        .addTag("", ". Follow the ")
        .addTag("s1", std::string("rabbit"))
        .addTag("", " number ")
        .addTag(std::string("i1"), 4)
        .addTag("i2", 2)
        .addTag("", " in ")
        .addTag("f", 0.5)
        .addTag(str, " hours.");
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_jkey_char_silent)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO)
        .addTag("a")
        .addTag("username")
        .addTag("b")
        .addTag("s1")
        .addTag("c")
        .addTag("i1")
        .addTag("i2")
        .addTag("d")
        .addTag("f")
        .addTag("hello");
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_jkey_rvalue_str_silent)
(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO)
        .addTag(std::string("a"))
        .addTag(std::string("username"))
        .addTag(std::string("b"))
        .addTag(std::string("s1"))
        .addTag(std::string("c"))
        .addTag(std::string("i1"))
        .addTag(std::string("i2"))
        .addTag(std::string("d"))
        .addTag(std::string("f"))
        .addTag(std::string("hello"));
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_jkey_lvalue_str_silent)
(benchmark::State& state) {
  std::string a("a");
  std::string username("username");
  std::string b("b");
  std::string s1("s1");
  std::string c("c");
  std::string i1("i1");
  std::string i2("i2");
  std::string d("d");
  std::string f("f");
  std::string hello("hello");
  for (auto _ : state) {
    SLOG(INFO)
        .addTag(a)
        .addTag(username)
        .addTag(b)
        .addTag(s1)
        .addTag(c)
        .addTag(i1)
        .addTag(i2)
        .addTag(d)
        .addTag(f)
        .addTag(hello);
  }
  a = a + username + b + s1 + c + i1 + i2 + d + f + hello;
  a.size();
}

BENCHMARK_F(SlogBenchmark, 10_tags_kv_char_silent)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO)
        .addTag("dummy", "a")
        .addTag("dummy", "username")
        .addTag("dummy", "b")
        .addTag("dummy", "s1")
        .addTag("dummy", "c")
        .addTag("dummy", "i1")
        .addTag("dummy", "i2")
        .addTag("dummy", "d")
        .addTag("dummy", "f")
        .addTag("dummy", "hello");
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_kv_rvalue_str_silent)
(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO)
        .addTag("dummy", std::string("a"))
        .addTag("dummy", std::string("username"))
        .addTag("dummy", std::string("b"))
        .addTag("dummy", std::string("s1"))
        .addTag("dummy", std::string("c"))
        .addTag("dummy", std::string("i1"))
        .addTag("dummy", std::string("i2"))
        .addTag("dummy", std::string("d"))
        .addTag("dummy", std::string("f"))
        .addTag("dummy", std::string("hello"));
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_kv_lvalue_str_silent)
(benchmark::State& state) {
  std::string a("a");
  std::string username("username");
  std::string b("b");
  std::string s1("s1");
  std::string c("c");
  std::string i1("i1");
  std::string i2("i2");
  std::string d("d");
  std::string f("f");
  std::string hello("hello");
  for (auto _ : state) {
    SLOG(INFO)
        .addTag("dummy", a)
        .addTag("dummy", username)
        .addTag("dummy", b)
        .addTag("dummy", s1)
        .addTag("dummy", c)
        .addTag("dummy", i1)
        .addTag("dummy", i2)
        .addTag("dummy", d)
        .addTag("dummy", f)
        .addTag("dummy", hello);
  }
  a = a + username + b + s1 + c + i1 + i2 + d + f + hello;
  a.size();
}

BENCHMARK_F(SlogBenchmark, msg_and_10_tags_noisy)(benchmark::State& state) {
  const std::string str = "Neo";
  for (auto _ : state) {
    SLOG(INFO) << "Hi " << SlogTag("username", str) << ". Follow the "
               << std::string("rabbit") << " number " << 4 << 2 << " in " << 0.5
               << " hours.";
  }
}

BENCHMARK_F(SlogBenchmark, stream_10_chars)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO) << " 0"
               << " 1"
               << " 2"
               << " 3"
               << " 4"
               << " 5"
               << " 6"
               << " 7"
               << " 8"
               << " 9";
  }
}

BENCHMARK_F(SlogBenchmark, stream_10_lvalue_strings)(benchmark::State& state) {
  std::string n0 = " 0";
  std::string n1 = " 1";
  std::string n2 = " 2";
  std::string n3 = " 3";
  std::string n4 = " 4";
  std::string n5 = " 5";
  std::string n6 = " 6";
  std::string n7 = " 7";
  std::string n8 = " 8";
  std::string n9 = " 9";
  for (auto _ : state) {
    SLOG(INFO) << n0 << n1 << n2 << n3 << n4 << n5 << n6 << n7 << n8 << n9;
  }
}

BENCHMARK_F(SlogBenchmark, stream_10_rvalue_strings)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO) << std::string(" 0") << std::string(" 1") << std::string(" 2")
               << std::string(" 3") << std::string(" 4") << std::string(" 5")
               << std::string(" 6") << std::string(" 7") << std::string(" 8")
               << std::string(" 9");
  }
}

BENCHMARK_F(SlogBenchmark, 10_tags_noisy)(benchmark::State& state) {
  for (auto _ : state) {
    SLOG(INFO) << SlogTag("t0", "n0") << SlogTag("t1", "n1")
               << SlogTag("t2", "n2") << SlogTag("t3", "n3")
               << SlogTag("t4", "n4") << SlogTag("t5", "n5")
               << SlogTag("t6", "n6") << SlogTag("t7", "n7")
               << SlogTag("t8", "n8") << SlogTag("t9", "n9");
  }
}

BENCHMARK_F(SlogBenchmark, dummy_str)(benchmark::State& state) {
  const std::string str = "dummy_message";
  for (auto _ : state) {
    SLOG(INFO) << str;
  }
}

BENCHMARK_F(SlogBenchmark, glog_dummy_str)(benchmark::State& state) {
  const std::string str = "dummy_message";
  for (auto _ : state) {
    LOG(INFO) << str;
  }
}

BENCHMARK_F(SlogBenchmark, slow_callback)(benchmark::State& state) {
  auto sbs3 = SlogContext::getInstance()->createAsyncSubscriber(
      [](const SlogRecord&) { usleep(100 * 1000); });
  int count = 0;
  for (auto _ : state) {
    SLOG(INFO);
    count += 1;
  }
  LOG(INFO) << "Done, " << count << " slogs sent";
}

}  // namespace slog
