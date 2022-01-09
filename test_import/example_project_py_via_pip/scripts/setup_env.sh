sudo update-alternatives --install /usr/bin/python python /home/buildkite/.pyenv/versions/3.8.12/bin/python3.8 10
python -m pip install slog_py==$1 --extra-index-url https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal-ephemeral/simple
