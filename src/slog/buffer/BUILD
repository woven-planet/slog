package(default_visibility = ["//visibility:public"])

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
        "//src/slog/context",
        "//src/slog/events:events_cc",
        "//src/slog/primitives:primitives_cc",
    ],
)

cc_test(
    name = "buffer_cc_test",
    srcs = ["buffer_test.cc"],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        ":buffer_cc",
        "//src/slog:slog_cc",
        "//src/slog/context",
    ],
)

# TODO(vsbus): import pybind
# load("//bazel:pybind11_build.bzl", "avs_pybind11_library")
# 
# avs_pybind11_library(
#     name = "buffer_py",
#     srcs = ["buffer_pybind.cc"],
#     module_name = "buffer",
#     deps = [
#         ":buffer_cc",
#     ],
# )
