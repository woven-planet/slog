package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "slog_trace_subscriber",
    srcs = ["slog_trace_subscriber.cpp"],
    hdrs = ["slog_trace_subscriber.h"],
    deps = [
        "//slog_cc/context",
        "//slog_cc/events:events_cc",
        "//slog_cc/util:string_util",
    ],
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
