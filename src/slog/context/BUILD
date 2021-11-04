package(default_visibility = ["//visibility:public"])

cc_library(
    name = "context",
    srcs = [
        "context.cc",
        "notification_queue.cc",
        "subscribers.cc",
    ],
    hdrs = [
        "context.h",
        "notification_queue.h",
        "subscribers.h",
    ],
    deps = [
        "@com_github_google_glog//:glog",
        "//src/slog/primitives:primitives_cc",
        "//src/slog/printer",
        "//src/slog/util",
    ],
)
