# Slog
Slog (reads "S-log") is a simple, fast, and cross-language structured logging. It's like Glog but allows structure via tags and async handling.

# Importing
## Into a C++ project built by Bazel
Requirements:
* C++14

Instructions: 
* Follow the example from `test_import/example_project_cc_via_bazel` directory.

## Into a Python project built by Bazel
Requirements:
* All C++ requirements from above
* Python 3.8
* pybind11 and pybind11_bazel

Instructions: 
* Follow the example from `test_import/example_project_py_via_bazel` directory.

# Contributing
## Installing tools for build
* `scripts/setup_dev_env.sh`

## Running tests
* `bazelisk test --test_output=errors //...` -- run all unit tests.

## Code formatting
* For the first time, install linters with `./scripts/install_linters.sh `
* `scripts/lint.sh ./ -i` -- automatically format all code.

## Releases.
CI automatically uploads a zip-archive to Artifactory. This .zip archive could be imported into another bazel project. The .zip file naming and content are matching a .zip file that could be created via GitHub releases.

