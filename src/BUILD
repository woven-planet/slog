package(default_visibility = ["//visibility:public"])

# TODO(vsbus): import pybind
# load("//bazel:pybind11_build.bzl", "avs_pybind11_library")

cc_library(
    name = "slog_cc",
    hdrs = [
        "slog.h",
    ],
    deps = [
        "//src/context",
        "//src/events:events_cc",
    ],
)

cc_test(
    name = "slog_test",
    srcs = ["slog_test.cc"],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        ":slog_cc",
        "//src/context",
        "//src/primitives:primitives_cc",
        "//src/util:string_util",
    ],
)

cc_test(
    name = "slog_benchmark",
    srcs = ["slog_benchmark.cc"],
    # Benchmark generates lots of logs that make results hard to read, it
    # should be run in a following way to hide all printed logs:
    # bazel build src:slog_benchmark && \
    #   ./bazel-bin/src/slog_benchmark 2>/dev/null
    tags = ["manual"],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        ":slog_cc",
    ],
)

cc_test(
    name = "slog_benchmark_concurrency",
    srcs = ["slog_benchmark_concurrency.cc"],
    tags = ["manual"],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        ":slog_cc",
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
#         "//src/common/util:init",
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
#         "//src/avs/libs/resource_management:cpu_allocation",
#         "//src/common/system:runtime_metrics",
#     ],
# )

# TODO(vsbus): import pybind11
# py_library(
#     name = "slog_py",
#     srcs = ["slog.py"],
#     visibility = ["//visibility:public"],
#     deps = [
#         "//src/events:events_py",
#     ],
# )
# 
# py_test(
#     name = "slog_py_test",
#     srcs = ["slog_test.py"],
#     main = "slog_test.py",
#     deps = [
#         ":slog_py",
#         "//src/buffer:buffer_py",
#         "//src/primitives:primitives_py",
#     ],
# )
