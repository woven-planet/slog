# Copyright (c) 2019 The Pybind Development Team. All rights reserved.
#
# All rights reserved. Use of this source code is governed by a
# BSD-style license that can be found in the LICENSE file.

"""Build rules for pybind11."""

def register_extension_info(**kwargs):
    pass

PYBIND_COPTS = [
    "-fexceptions",
]

PYBIND_FEATURES = [
    "-use_header_modules",  # Required for pybind11.
    "-parse_headers",
]

PYBIND_DEPS = [
    "@pybind11",
    # "@local_config_python//:python_headers",
]

# Builds a Python extension module using pybind11.
# This can be directly used in python with the import statement.
# This adds rules for a .so binary file.
def pybind_extension(
        name,
        copts = [],
        features = [],
        linkopts = [],
        tags = [],
        deps = [],
        **kwargs):
    # Mark common dependencies as required for build_cleaner.
    tags = tags + ["req_dep=%s" % dep for dep in PYBIND_DEPS]

    native.cc_binary(
        name = name + ".so",
        copts = copts + PYBIND_COPTS + ["-fvisibility=hidden"],
        features = features + PYBIND_FEATURES,
        linkopts = linkopts + select({
            "@pybind11//:osx": [],
            "//conditions:default": ["-Wl,-Bsymbolic"],
        }),
        linkshared = 1,
        # linkshared = 0,
        tags = tags + ["local"],
        deps = deps + PYBIND_DEPS,
        **kwargs
    )

# Builds a pybind11 compatible library. This can be linked to a pybind_extension.
def pybind_library(
        name,
        copts = [],
        features = [],
        tags = [],
        deps = [],
        **kwargs):
    # Mark common dependencies as required for build_cleaner.
    tags = tags + ["req_dep=%s" % dep for dep in PYBIND_DEPS]

    native.cc_library(
        name = name,
        copts = copts + PYBIND_COPTS,
        features = features + PYBIND_FEATURES,
        tags = tags,
        deps = deps + PYBIND_DEPS,
        **kwargs
    )

# Builds a C++ test for a pybind_library.
def pybind_library_test(
        name,
        copts = [],
        features = [],
        tags = [],
        deps = [],
        **kwargs):
    # Mark common dependencies as required for build_cleaner.
    tags = tags + ["req_dep=%s" % dep for dep in PYBIND_DEPS]

    native.cc_test(
        name = name,
        copts = copts + PYBIND_COPTS,
        features = features + PYBIND_FEATURES,
        tags = tags,
        deps = deps + PYBIND_DEPS + [
            "//util/python:python_impl",
            "//util/python:test_main",
        ],
        **kwargs
    )

# Register extension with build_cleaner.
register_extension_info(
    extension = pybind_extension,
    label_regex_for_dep = "{extension_name}",
)

register_extension_info(
    extension = pybind_library,
    label_regex_for_dep = "{extension_name}",
)

register_extension_info(
    extension = pybind_library_test,
    label_regex_for_dep = "{extension_name}",
)

def _copy_keeping_deps_impl(ctx):
    ctx.actions.run_shell(
        inputs = [ctx.file.src],
        outputs = [ctx.outputs.out],
        command = "cp -f \"$1\" \"$2\"",
        arguments = [ctx.file.src.path, ctx.outputs.out.path],
        mnemonic = "CopyFile",
        progress_message = "Copying files",
        use_default_shell_env = True,
    )
    runfiles = ctx.runfiles(
        files = [ctx.outputs.out],
        # Keep the transitive runfile dependencies of the original.
        collect_default = True,
    )
    return [DefaultInfo(files = depset(direct = [ctx.outputs.out]), runfiles = runfiles)]

copy_keeping_deps = rule(
    attrs = {
        "src": attr.label(
            mandatory = True,
            allow_single_file = True,
        ),
        "out": attr.output(
            mandatory = True,
        ),
        "deps": attr.label_list(
            mandatory = True,
            allow_empty = False,
        ),
    },
    implementation = _copy_keeping_deps_impl,
)

def pybind_with_tdeps(
        name,
        so):
    copy_keeping_deps(
        name = name,
        src = so,
        out = name + "_with_tdeps",
        deps = [so],
    )
