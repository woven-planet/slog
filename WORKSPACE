load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "com_github_gflags_gflags",
    sha256 = "34af2f15cf7367513b352bdcd2493ab14ce43692d2dcd9dfc499492966c64dcf",
    strip_prefix = "gflags-2.2.2",
    urls = ["https://github.com/gflags/gflags/archive/v2.2.2.tar.gz"],
)

http_archive(
    name = "com_github_google_googletest",
    sha256 = "9dc9157a9a1551ec7a7e43daea9a694a0bb5fb8bec81235d8a1e6ef64c716dcb",
    strip_prefix = "googletest-release-1.10.0",
    url = "https://github.com/google/googletest/archive/release-1.10.0.tar.gz",
)

http_archive(
    name = "com_github_google_glog",
    sha256 = "21bc744fb7f2fa701ee8db339ded7dce4f975d0d55837a97be7d46e8382dea5a",
    strip_prefix = "glog-0.5.0",
    urls = ["https://github.com/google/glog/archive/v0.5.0.zip"],
)

http_archive(
    name = "com_github_google_benchmark",
    # build_file = "//third_party:benchmark.BUILD",
    sha256 = "21e6e096c9a9a88076b46bd38c33660f565fa050ca427125f64c4a8bf60f336b",
    strip_prefix = "benchmark-1.5.2",
    urls = ["https://github.com/google/benchmark/archive/v1.5.2.zip"],
)

# Importing python_build_standalone archive that will be used as a python
# interpreter for bazel builds and for pybind11_bazel.
load("//python_toolchain:fetch_python_build_standalone.bzl", "fetch_python_build_standalone")

fetch_python_build_standalone(name = "python_build_standalone")

# Register python3 interpreter from python_build_standalone to be used by bazel builds.
register_toolchains("//python_toolchain")

######## Import pybind11 begin ########

http_archive(
    name = "pybind11_bazel",
    sha256 = "3dc6435bd41c058453efe102995ef084d0a86b0176fd6a67a6b7100a2e9a940e",
    strip_prefix = "pybind11_bazel-992381ced716ae12122360b0fbadbc3dda436dbf",
    urls = ["https://github.com/pybind/pybind11_bazel/archive/992381ced716ae12122360b0fbadbc3dda436dbf.zip"],
)

http_archive(
    name = "pybind11",
    build_file = "@pybind11_bazel//:pybind11.BUILD",
    strip_prefix = "pybind11-2.5.0",
    urls = ["https://github.com/pybind/pybind11/archive/v2.5.0.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(
    name = "local_config_python",
    python_interpreter_target = "@python_build_standalone//:python/install/bin/python3.8",
)

######## Import pybind11 end ########
