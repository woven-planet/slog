cc_library(
  name = "slog",
  hdrs = ["slog.h"],
)

cc_test(
  name = "slog_test",
  srcs = ["slog_test.cc"],
  deps = [
    "@com_github_google_googletest//:gtest_main",
    ":slog",
  ],
)
