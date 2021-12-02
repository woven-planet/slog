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

# TODO(vsbus): fix benchmark
cc_test(
    name = "slog_benchmark",
    srcs = ["slog_benchmark.cc"],
    # Benchmark generates lots of logs that make results hard to read, it
    # should be run in a following way to hide all printed logs:
    # bazel build slog_cc:slog_benchmark && \
    #   ./bazel-bin/slog_cc/slog_benchmark 2>/dev/null
    tags = ["manual"],
    deps = [
        ":slog_cc",
        "@com_github_google_benchmark//:benchmark",
        "@com_github_google_glog//:glog",
        "@com_github_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "slog_benchmark_concurrency",
    srcs = ["slog_benchmark_concurrency.cc"],
    tags = ["manual"],
    deps = [
        ":slog_cc",
        "@com_github_google_googletest//:gtest_main",
    ],
)

cc_test(
    name = "clock_benchmark",
    srcs = ["clock_benchmark.cc"],
    tags = ["manual"],
)

# TODO(vsbus): recover stress tests and resolve deps.
# cc_binary(
#     name = "slog_concurrency_stress_test",
#     srcs = ["slog_concurrency_stress_test.cc"],
#     deps = [
#         ":slog_cc",
#         "//slog_cc/common/util:init",
#         "//third_party/cartographer:common",
#     ],
# )
#
# cc_binary(
#     name = "slog_system_perf_test",
#     srcs = ["slog_system_perf_test.cc"],
#     tags = ["manual"],
#     deps = [
#         ":slog_cc",
#         "//slog_cc/avs/libs/resource_management:cpu_allocation",
#         "//slog_cc/common/system:runtime_metrics",
#     ],
# )
