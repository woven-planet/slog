package(default_visibility = ["//slog_cc:__subpackages__"])

cc_library(
    name = "thread_id",
    hdrs = [
        "thread_id.h",
    ],
    srcs = select({
        "@platforms//os:qnx": ["impl_qnx/thread_id.cc"],
        "//conditions:default": ["impl_ubuntu/thread_id.cc"],
    }),
)
