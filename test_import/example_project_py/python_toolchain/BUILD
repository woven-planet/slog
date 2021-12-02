load("@bazel_tools//tools/python:toolchain.bzl", "py_runtime_pair")

py_runtime(
    name = "python_runtime",
    files = ["@python_build_standalone//:files"],
    interpreter = "@python_build_standalone//:interpreter",
    python_version = "PY3",
)

py_runtime_pair(
    name = "python_runtime_pair",
    py2_runtime = None,
    py3_runtime = ":python_runtime",
)

toolchain(
    name = "python_toolchain",
    toolchain = ":python_runtime_pair",
    toolchain_type = "@bazel_tools//tools/python:toolchain_type",
)
