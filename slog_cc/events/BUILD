package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
    "//slog_py:__pkg__",
])

cc_library(
    name = "events_cc",
    srcs = ["scope.cpp"],
    hdrs = [
        "event.h",
        "scope.h",
    ],
    copts = [
        "-DNDEBUG",
        "-g0",
        "-O3",
    ],
    deps = [
        "//slog_cc/context",
        "//slog_cc/primitives:primitives_cc",
        "//slog_cc/util",
        "//slog_cc/util:string_util",
        "//slog_cc/util/os:thread_id",
    ],
)
