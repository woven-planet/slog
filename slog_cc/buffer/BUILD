package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
    "//slog_py:__pkg__",
])

cc_library(
    name = "buffer_cc",
    srcs = [
        "buffer.cc",
    ],
    hdrs = [
        "buffer.h",
        "buffer_data.h",
    ],
    deps = [
        "//slog_cc/context",
        "//slog_cc/events:events_cc",
        "//slog_cc/primitives:primitives_cc",
    ],
)

cc_test(
    name = "buffer_cc_test",
    srcs = ["buffer_test.cc"],
    deps = [
        ":buffer_cc",
        "//slog_cc",
        "//slog_cc/context",
        "@com_github_google_googletest//:gtest_main",
    ],
)
