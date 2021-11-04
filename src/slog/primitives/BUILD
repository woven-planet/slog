package(default_visibility = ["//visibility:public"])

cc_library(
    name = "primitives_cc",
    srcs = [
        "record.cc",
        "tag.cc",
    ],
    hdrs = [
        "call_site.h",
        "record.h",
        "tag.h",
        "timestamps.h",
    ],
    deps = [
        "//src/slog/util",
    ],
)

# TODO(vsbus): import pybind11
# load("//bazel:pybind11_build.bzl", "avs_pybind11_library")
# 
# avs_pybind11_library(
#     name = "primitives_py",
#     srcs = ["primitives_pybind.cc"],
#     module_name = "primitives",
#     deps = [
#         ":primitives_cc",
#         "//src/slog/printer",
#     ],
# )
