py_binary(
    name = "slog_trace",
    srcs = ["slog_trace.py"],
    main = "slog_trace.py",
    deps = ["//:slog_py"],
)

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
