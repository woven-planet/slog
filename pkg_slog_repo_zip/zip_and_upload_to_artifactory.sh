zip -r "/tmp/slog-v$1.zip" "./" -x ".*"

curl -u "${BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_USERNAME}:${BUILDKITE_VAULTBUILDKITEPLUGIN_INFRA_NEXUS_BUILDKITE_PASSWORD}" \
  --upload-file "/tmp/slog-v$1.zip" \
  "https://artifactory.pdx.aws.av.lyft.net/repository/mothership/slog/slog-v$1.zip"

