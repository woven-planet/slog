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
    sha256 = "97504db65640570f32d3fdf701c25a340c8643037c3b69aec469c10c93dc8504",
    strip_prefix = "pybind11-2.5.0",
    urls = ["https://github.com/pybind/pybind11/archive/v2.5.0.tar.gz"],
)

load("@pybind11_bazel//:python_configure.bzl", "python_configure")

python_configure(
    name = "local_config_python",
    python_interpreter_target = "@python_build_standalone//:python/install/bin/python3.8",
)

######## Import pybind11 end ########

http_archive(
    name = "python_wheel",
    build_file = "//pkg_slog_py_wheel:python_wheel.BUILD",
    sha256 = "9515fe0a94e823fd90b08d22de45d7bde57c90edce705b22f5e1ecf7e1b653c8",
    strip_prefix = "wheel-0.30.0",
    urls = ["https://files.pythonhosted.org/packages/fa/b4/f9886517624a4dcb81a1d766f68034344b7565db69f13d52697222daeb72/wheel-0.30.0.tar.gz"],
)
