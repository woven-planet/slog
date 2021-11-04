package(default_visibility = ["//visibility:public"])

cc_library(
    name = "events_cc",
    srcs = ["scope.cc"],
    hdrs = [
        "event.h",
        "scope.h",
    ],
    deps = [
        "//src/context",
        "//src/primitives:primitives_cc",
        "//src/util",
        "//src/util:string_util",
    ],
)

# TODO(vsbus): import pybind11
# load("//bazel:pybind11_build.bzl", "avs_pybind11_library")
# 
# avs_pybind11_library(
#     name = "events_py",
#     srcs = ["events_pybind.cc"],
#     module_name = "events",
#     deps = [":events_cc"],
# )
