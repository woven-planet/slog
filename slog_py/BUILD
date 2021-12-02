load("@pybind11_bazel//:build_defs.bzl", "pybind_extension")

pybind_extension(
    name = "slog_pybind",
    srcs = ["slog_pybind.cc"],

    # When slog is imported to a user repo the relative paths to .so libs
    # become different. We couldn't find an automated solution to this yet and
    # hardcoding a few paths. If user sees an error like
    # `libexternal_Sslog_Sslog_Ucc_Sbuffer_Slibbuffer_Ucc.so: cannot open shared object file: No such file or directory`
    # they have to use `ldd` and `readelf -d` with the generated slog_pybind.so
    # file, e.g.
    # `bazel-out/k8-fastbuild/bin/src/common/logging/slog/slog_py_test.runfiles/slog/slog_py/slog_pybind.so`
    # and patch this BUILD file with adding a path working in their repo.
    linkopts = [
        # TODO(vsbus): how to detect repo dir automatically? Maybe we can use patch in
        # http_archive rule and get values of repo name and solib dir?
        "-Wl,-rpath=$$ORIGIN/../../com_lyft_avsoftware/_solib_local/",
        "-Wl,-rpath=$$ORIGIN/../../__main__/_solib_k8/",
    ],

    # The slog_pybind.so library has to be linked dynamically because it
    # depends on `//slog_cc/context` that has a singleton. Given the library
    # has a singleton it must be linked dynamically and be shared across all
    # units that import it. E.g. a user can import `:slog_py` and create slog
    # events in their code and to handle them they can implement their own
    # subscriber in C++ with python bindings, so both emitting slog events and
    # subscriber must use the *same* context binary code otherwise they will
    # act like different programs despite being run under the same thread id.
    linkstatic = False,
    visibility = ["//:__pkg__"],
    deps = [
        "//slog_cc/buffer:buffer_cc",
        "//slog_cc/events:events_cc",
    ],
)

py_library(
    name = "slog_py",
    srcs = ["slog.py"],
    data = [":slog_pybind.so"],
    visibility = ["//:__pkg__"],
)

py_test(
    name = "slog_py_test",
    srcs = ["slog_test.py"],
    main = "slog_test.py",
    deps = [":slog_py"],
)
