def _fetch_python_build_standalone_impl(repository_ctx):
    repository_ctx.download(
        url = ["https://github.com/indygreg/python-build-standalone/releases/download/20210228/cpython-3.8.8-x86_64-unknown-linux-gnu-pgo+lto-20210228T1503.tar.zst"],
        sha256 = "74c9067b363758e501434a02af87047de46085148e673547214526da6e2b2155",
        output = "python.tar.zst",
    )

    unzstd_bin_path = repository_ctx.which("unzstd")
    if unzstd_bin_path == None:
        fail("Python toolchain requires that the zstd and unzstd are available on the $PATH, but it was not found.")

    res = repository_ctx.execute([unzstd_bin_path, "python.tar.zst"])
    if res.return_code:
        fail("Error decompressing with zstd" + res.stdout + res.stderr)

    repository_ctx.extract(archive = "python.tar")
    repository_ctx.delete("python.tar")
    repository_ctx.delete("python.tar.zst")

    repository_ctx.file("BUILD.bazel", """
package(default_visibility = ["//visibility:public"])
filegroup(
    name = "files",
    srcs = glob(["install/**"], exclude = ["**/* *"]),
)
filegroup(
    name = "interpreter",
    srcs = ["python/install/bin/python3.8"],
)
sh_binary(
    name = "foo",
    srcs = ["python/install/bin/python3.8"],
)
""")

fetch_python_build_standalone = repository_rule(
    implementation = _fetch_python_build_standalone_impl,
    attrs = {},
)
