# TODO(vsbus): recover stress tests and resolve deps.
# cc_binary(
#     name = "clock",
#     srcs = ["clock.cpp"],
#     copts = [
#         "-DNDEBUG",
#         "-g0",
#         "-O3",
#     ],
#     linkopts = ["-lpthread"],
#     deps = [
#         "@com_github_google_benchmark//:benchmark",
#     ],
# )
#
# cc_binary(
#     name = "slog_benchmark",
#     srcs = ["slog_benchmark.cpp"],
#     copts = [
#         "-DNDEBUG",
#         "-g0",
#         "-O3",
#     ],
#     linkopts = [
#         "-lpthread",
#         # Note, a very big contributor to Slog performance is a memory allocator
#         # used by target binary b/c slog makes a few alloc/dealloc under the
#         # hood. E.g. `SlogBenchmark/basic` takes 230ns with tcmalloc, but it
#         # takes 1100ns with a default library.
#         "-ltcmalloc",
#     ],
#     deps = [
#         "//slog_cc",
#         "@com_github_google_benchmark//:benchmark",
#         "@com_github_google_glog//:glog",
#     ],
# )
#
# # TODO(vsbus): fix benchmark
# cc_test(
#     name = "slog_benchmark_concurrency",
#     srcs = ["slog_benchmark_concurrency.cpp"],
#     tags = ["manual"],
#     deps = [
#         ":slog_cc",
#         "@com_github_google_googletest//:gtest_main",
#     ],
# )
#
# cc_binary(
#     name = "slog_concurrency_stress_test",
#     srcs = ["slog_concurrency_stress_test.cpp"],
#     deps = [
#         ":slog_cc",
#         "//slog_cc/common/util:init",
#         "//third_party/cartographer:common",
#     ],
# )
#
# cc_binary(
#     name = "slog_system_perf_test",
#     srcs = ["slog_system_perf_test.cpp"],
#     tags = ["manual"],
#     deps = [
#         ":slog_cc",
#         "//slog_cc/avs/libs/resource_management:cpu_allocation",
#         "//slog_cc/common/system:runtime_metrics",
#     ],
# )
