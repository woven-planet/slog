package(default_visibility = ["//slog_cc:__subpackages__"])

cc_library(
    name = "util",
    hdrs = [
        "assert_macro.h",
        "inline_macro.h",
    ],
)

cc_library(
    name = "string_util",
    srcs = ["string_util.cc"],
    hdrs = ["string_util.h"],
)

cc_test(
    name = "string_util_test",
    srcs = ["string_util_test.cc"],
    deps = [
        ":string_util",
        "@com_github_google_googletest//:gtest_main",
    ],
)
