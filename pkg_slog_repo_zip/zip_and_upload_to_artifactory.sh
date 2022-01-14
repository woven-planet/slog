zip -r "/tmp/slog-v$1.zip" "./" -x ".*"

PACKAGE_PATH=tmp/slog
if [[ "${BUILDKITE_BRANCH}" = "main" ]]; then
    PACKAGE_PATH=slog
fi

echo "https://artifactory.pdx.aws.av.lyft.net/repository/mothership/${PACKAGE_PATH}/slog-v$1.zip"

curl -u "${BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_USERNAME}:${BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_PASSWORD}" \
  --upload-file "/tmp/slog-v$1.zip" \
  "https://artifactory.pdx.aws.av.lyft.net/repository/mothership/${PACKAGE_PATH}/slog-v$1.zip"

