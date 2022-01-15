package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "slog_cc",
    hdrs = [
        "slog.h",
    ],
    deps = [
        "//slog_cc/context",
        "//slog_cc/events:events_cc",
    ],
)

cc_test(
    name = "slog_test",
    srcs = ["slog_test.cc"],
    deps = [
        ":slog_cc",
        "//slog_cc/context",
        "//slog_cc/primitives:primitives_cc",
        "//slog_cc/util:string_util",
        "@com_github_google_googletest//:gtest_main",
    ],
)
