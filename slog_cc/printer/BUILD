package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "printer",
    srcs = [
        "printer.cpp",
    ],
    hdrs = [
        "printer.h",
    ],
    copts = [
        "-DNDEBUG",
        "-g0",
        "-O3",
    ],
    deps = [
        "//slog_cc/primitives:primitives_cc",
        "//slog_cc/util:string_util",
    ],
)

cc_test(
    name = "printer_test",
    srcs = ["printer_test.cpp"],
    deps = [
        ":printer",
        "@com_github_google_googletest//:gtest_main",
    ],
)
