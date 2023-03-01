package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "slog_trace_subscriber",
    hdrs = ["slog_trace_subscriber.h"],
    srcs = ["slog_trace_subscriber.cc"],
    deps = [
        "//slog_cc/util:string_util",
        "//slog_cc/context",
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
