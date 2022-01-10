#!/usr/bin/env bash

# TODO(vsbus): use argument and login to `pypi-internal/` for `main` branch and `pypi-internal-ephemeral/` for others.
echo "[INFO] Logging into https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal..."

cat > ~/.pypirc <<EOF
[distutils]
index-servers =
    nexus
[nexus]
repository: https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal/
username: $BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_USERNAME
password: $BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_PASSWORD
EOF

