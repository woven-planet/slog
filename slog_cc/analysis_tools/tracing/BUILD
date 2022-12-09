package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "slog_trace_subscriber",
    hdrs = ["slog_trace_subscriber.h"],
)

cc_binary(
    name = "example_slog_trace",
    srcs = ["example_slog_trace.cpp"],
    linkopts = ["-lpthread"],
    deps = [
        ":slog_trace_subscriber",
        "//:slog_cc",
    ],
)
