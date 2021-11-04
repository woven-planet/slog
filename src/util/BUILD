package(default_visibility = ["//visibility:public"])

cc_library(
    name = "util",
    hdrs = [
        "inline_macro.h",
    ],
)

cc_library(
    name = "string_util",
    hdrs = ["string_util.h"],
    srcs = ["string_util.cc"],
)

cc_test(
    name = "string_util_test",
    srcs = ["string_util_test.cc"],
    deps = [
        "@com_github_google_googletest//:gtest_main",
        ":string_util",
    ],
)
