package(default_visibility = [
    "//:__pkg__",
    "//slog_cc:__subpackages__",
])

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
    copts = [
        "-DNDEBUG",
        "-g0",
        "-O3",
    ],
    deps = [
        "//slog_cc/util",
    ],
)
