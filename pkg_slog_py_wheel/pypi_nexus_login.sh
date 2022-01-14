#!/usr/bin/env bash

REPO="https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal-ephemeral/"
if [[ "${BUILDKITE_BRANCH}" = "main" ]]; then
    REPO="https://artifactory.pdx.l5.woven-planet.tech/repository/pypi-internal/"
fi

echo "[INFO] Logging into ${REPO} ..."

cat > ~/.pypirc <<EOF
[distutils]
index-servers =
    nexus
[nexus]
repository: $REPO
username: $BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_USERNAME
password: $BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_PASSWORD
EOF

