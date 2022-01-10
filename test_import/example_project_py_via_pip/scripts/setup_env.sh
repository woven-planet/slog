# TODO(vsbus): refactor py38 setup
sudo update-alternatives --install /usr/bin/python python /home/buildkite/.pyenv/versions/3.8.12/bin/python3.8 10
python -m pip install slog_py==$1 --extra-index-url https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal/simple
# TODO(vsbus): use arg and install slog from `pypi-internal/` in main branch and from `pypi-internal-ephemeral/` for others. Do same for repo-zip, so all CI test import and not only main branch.
# python -m pip install slog_py==$1 --extra-index-url https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal-ephemeral/simple
