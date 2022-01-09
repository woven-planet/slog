# Packages python target into a .whl file and uploads to Artifactory.
# TODO(vsbus): Use OSS macro or refactor to a more generic one.
def pkg_py(name, libs, version):
    pkg_py_tar(
        name = name + "_tar",
        data = libs,
        pkg_name = name,
        version = version,
    )

    publish_pkg_py(
        name = name,
        dist_tar = name + "_tar",
    )

_SETUP_PY_TEMPLATE = """
from setuptools import setup, find_packages
from wheel.bdist_wheel import bdist_wheel as _bdist_wheel

class bdist_wheel(_bdist_wheel):
    def finalize_options(self):
        _bdist_wheel.finalize_options(self)
        self.root_is_pure = False
        self.plat_name = "linux_x86_64"

setup(
    name = "{name}",
    version = "{version}",
    packages=find_packages(),
    zip_safe=False,
    include_package_data=True,
    setup_requires=["wheel"],
)
"""

_DISTRIBUTION_BUILDER_SCRIPT = """
set -e
rm -rf tmp && mkdir tmp/
{init_directories}
{copy_runfile_commands}
{copy_symlink_commands}
touch tmp/README
echo '{setup_py}' > tmp/setup.py
echo '{manifest_content}' > tmp/MANIFEST.in
cd tmp
{python_cmd} setup.py bdist_wheel > /dev/null
{rename_wheel}
tar -chf dist.tar dist/*
cd ../
mv ./tmp/dist.tar {output}
"""

# TODO(vsbus): automate manifest in by including everything from files returned by distribution() func
_MANIFEST_IN = """
include slog_py/slog_pybind.so
graft _solib_k8/
"""

# TODO(vsbus): add metadata to pkg to make it available ONLY for Linux and ONLY for Python 3.8+

_PkgPypiInfo = provider(fields = {
    "package": "package name == <exact version> of the package produced",
    "files": "list of File objects that were copied and included in the wheel package",
    "dist_tar": "the File corresponding to the package distribution tar",
})

def _pkg_py_tar_impl(ctx):
    platform = "x86_64-linux-gnu"

    files_to_copy, wheel_deps = _distribution_files(
        ctx.attr.data,
    )

    setup_py_content = _SETUP_PY_TEMPLATE.format(
        name = ctx.attr.pkg_name,
        version = ctx.attr.version,
    )

    symlinks = [s for s in ctx.runfiles(collect_data = True).symlinks.to_list() if s.target_file.path]
    symlink_files = [s.target_file for s in symlinks]

    copy_runfile_commands = _copy_files_into_tmp(files_to_copy)
    copy_symlink_commands = _copy_symlinks_into_tmp(symlinks)
    file_packages = _get_directories_with_parents([f.short_path for f in files_to_copy])
    symlink_packages = _get_directories_with_parents([s.path for s in symlinks])
    packages = sorted(dedupe_list(file_packages + symlink_packages))

    python_cmd = "PYTHONPATH=../{} ../external/python_build_standalone/python/install/bin/python3.8 -s".format(ctx.attr._wheel.label.workspace_root)
    interpreter = ctx.attr._python3_8
    tools = interpreter.files.to_list() + ctx.attr._wheel.files.to_list()

    builder_executable = ctx.actions.declare_file("_builder_executable_%s" % ctx.attr.pkg_name)
    ctx.actions.write(
        output = builder_executable,
        content = _DISTRIBUTION_BUILDER_SCRIPT.format(
            python_cmd = python_cmd,
            init_directories = _init_pypi_paths(packages),
            copy_runfile_commands = copy_runfile_commands,
            copy_symlink_commands = copy_symlink_commands,
            setup_py = setup_py_content,
            manifest_content = _MANIFEST_IN,
            # TODO(vsbus): do we really need rename_wheel?
            rename_wheel = "",
            output = ctx.outputs.out.path,
        ),
        is_executable = True,
    )
    ctx.actions.run_shell(
        inputs = files_to_copy + symlink_files + [builder_executable, ctx.info_file, ctx.version_file],
        tools = tools,
        outputs = [ctx.outputs.out],
        command = builder_executable.path,
        use_default_shell_env = True,
    )
    return [_PkgPypiInfo(
        package = "{}=={}".format(ctx.attr.pkg_name, ctx.attr.version),
        files = files_to_copy + [s.target_file for s in symlinks],
        dist_tar = ctx.outputs.out,
    )]

def _get_directories_with_parents(paths):
    packages = []
    for path in paths:
        tokens = path.split("/")
        for prefix_len in range(1, len(tokens)):
            packages.append("/".join(tokens[:prefix_len]))
    return sorted(dedupe_list(packages))

def _distribution_files(libs):
    files = []
    wheels_used = {}
    for lib in libs:
        candidates = lib[DefaultInfo].default_runfiles.files.to_list()
        files += candidates
    return sorted(dedupe_list(files)), wheels_used.keys()

def _init_pypi_paths(paths):
    out_lines = []
    for path in paths:
        out_lines += [
            "mkdir -p tmp/{path}".format(path = path),
            "touch tmp/{path}/__init__.py".format(path = path),
        ]
    return "\n".join(out_lines)

def _copy_files_into_tmp(files):
    out_lines = []
    for file in files:
        if file.short_path.startswith(".."):
            continue
        out_lines += [
            "cp -f {src} tmp/{dest}".format(
                src = file.path,
                dest = "/".join(file.short_path.split("/")[:-1]),
            ),
        ]
    return "\n".join(out_lines)

def _copy_symlinks_into_tmp(symlinks):
    out_lines = []
    for symlink in symlinks:
        out_lines += [
            "if [ ! -e tmp/{dest} ]; then cp {src} tmp/{dest} ; fi".format(
                src = symlink.target_file.path,
                dest = symlink.path,
            ),
        ]
    return "\n".join(out_lines)

def dedupe_list(values):
    """Remove duplicates from a list.
    """
    return [k for k in dict(zip(values, values))]

pkg_py_tar = rule(
    attrs = {
        "data": attr.label_list(mandatory = True),
        "pkg_name": attr.string(mandatory = True),
        "version": attr.string(mandatory = True),
        "_python3_8": attr.label(default = "@python_build_standalone//:interpreter", allow_files = True),
        "_wheel": attr.label(default = "@python_wheel//:pkg"),
    },
    outputs = {"out": "%{name}.dist.tar"},
    implementation = _pkg_py_tar_impl,
)

def publish_pkg_py(name, dist_tar):
    publish_pkg_py_executable_name = "publish_pypi_executable_%s" % name
    artifact_metadata_name = "%s_artifact_metadata" % name

    publish_pkg_py_executable(
        name = publish_pkg_py_executable_name,
        dist_tar = dist_tar,
    )

    run_publish_executable(
        name = name,
        publish_script = (":%s" % publish_pkg_py_executable_name),
        src = dist_tar,
    )

_PUBLISH_PYPI_SCRIPT = """
set -o errexit
TMP=$(mktemp -d -t pkg_py-XXXXXXXXXX)
if [[ "{dist_tar_path}" == *.gz ]] ; then
  tar -xzf {dist_tar_path} -C $TMP
else
  tar -xf {dist_tar_path} -C $TMP
fi
twine upload {pkg_py_repo_args} $TMP/dist/*
rm -rf $TMP
"""

def _publish_pkg_py_executable_impl(ctx):
    publish_pkg_py_script = _PUBLISH_PYPI_SCRIPT.format(
        dist_tar_path = ctx.files.dist_tar[0].path,
        # TODO(vsbus): add code to write to pypi-internal with right user/password
        pkg_py_repo_args = "--repository-url=https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal-ephemeral/ -u \"\" -p \"\"",
    )
    ctx.actions.write(
        content = publish_pkg_py_script,
        output = ctx.outputs.out,
        is_executable = True,
    )
    return [DefaultInfo(executable = ctx.outputs.out)]

"""Create a script to unpack a tarball and upload the contents.
This makes an assumption that the the unpacked tarball stores the
*.whl file in a dist directory.
"""
publish_pkg_py_executable = rule(
    attrs = {
        "dist_tar": attr.label(mandatory = True, allow_single_file = [".tar", ".tar.gz"]),
    },
    executable = True,
    outputs = {"out": "%{name}.sh"},
    implementation = _publish_pkg_py_executable_impl,
)

def _run_publish_executable_impl(ctx):
    inputs = ctx.files.src

    to_execute = "{script_path} > {output_path}".format(
        script_path = ctx.executable.publish_script.path,
        output_path = ctx.outputs.out.path,
    )
    progress_message = "Pushing artifact: %s" % ctx.label.name
    metadata_files = []

    ctx.actions.run_shell(
        inputs = inputs,
        tools = [ctx.executable.publish_script],
        outputs = [ctx.outputs.out],
        command = to_execute,
        progress_message = progress_message,
    )

    return [
        DefaultInfo(files = depset(ctx.files.src + [ctx.outputs.out])),
        OutputGroupInfo(metadata = depset(metadata_files)),
    ]

run_publish_executable = rule(
    attrs = {
        "publish_script": attr.label(
            mandatory = True,
            allow_files = True,
            executable = True,
            cfg = "target",
            doc = "a script that will publish the artifact",
        ),
        "src": attr.label(
            mandatory = True,
            allow_files = True,
            doc = "the source files that were used to build the package",
        ),
    },
    outputs = {
        "out": "%{name}_publish_output",
    },
    implementation = _run_publish_executable_impl,
)
