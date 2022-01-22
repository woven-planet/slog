package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

cc_library(
    name = "context",
    srcs = [
        "context.cpp",
        "notification_queue.cpp",
        "subscribers.cpp",
    ],
    hdrs = [
        "context.h",
        "notification_queue.h",
        "subscribers.h",
    ],
    copts = [
        "-DNDEBUG",
        "-g0",
        "-O3",
    ],
    deps = [
        "//slog_cc/primitives:primitives_cc",
        "//slog_cc/printer",
        "//slog_cc/util",
    ],
)
