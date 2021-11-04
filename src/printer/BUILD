package(default_visibility = ["//visibility:public"])

cc_library(
    name = "printer",
    srcs = [
        "printer.cc",
    ],
    hdrs = [
        "printer.h",
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "//src/primitives:primitives_cc",
        "//src/util:string_util",
    ],
)

cc_test(
    name = "printer_test",
    srcs = ["printer_test.cc"],
    deps = [
	"@com_github_google_googletest//:gtest_main",
        ":printer",
    ],
)
