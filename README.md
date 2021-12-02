# slog
Structured Logging

# Requirements
## Importing C++
* C++14
## Importing Python
* All C++ requirements from above
* pybind11 and pybind11_bazel

# Importing
## Into a project built by Bazel
See examples and instructions in `test_import` directory.

# Contributing
## Running tests
* `bazelisk test --test_output=errors slog/...` -- run all unit tests.

## Code formatting
* `./lint.sh ./ -i` -- automatically format all code.

## Releases.
CI automatically uploads a zip-archive to https://artifactory.pdx.l5.woven-planet.tech/repository/webapp/#/artifacts/browse/tree/General/mothership/slog/. This .zip archive could be imported into another bazel project, e.g. using the following URL: `https://artifactory.pdx.l5.woven-planet.tech/repository/mothership/slog/slog-v0.2.27.zip`.  The .zip file naming and content are matching a .zip file that could be created via GitHub releases at https://github.tri-ad.tech/level5/slog/releases.

