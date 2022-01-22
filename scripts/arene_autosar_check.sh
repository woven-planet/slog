#!/bin/bash

set -e

/bin/bash -c "$(curl -fsSL https://arene-lab-artifact-repo-staging.s3-ap-northeast-1.amazonaws.com/installers/install.sh)"

echo DEBUG HOME: $HOME
echo DEBUG PWD: $PWD

echo DEBUG ls -l $PWD/
ls -l $PWD/

echo DEBUG ls -l $HOME/.arene/bin/arene
ls -l $HOME/.arene/bin/arene

$HOME/.arene/bin/arene install arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu

echo DEBUG ls -l $PWD/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/
ls -l $PWD/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/

rm -rf build/codeql-database && \
mkdir -p build && \
/workdir/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/bin/codeql database create build/codeql-database --language=cpp --command="bazelisk build //slog_cc" && \
/workdir/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/bin/codeql database analyze build/codeql-database autosar-default.qls \
    --format=csv \
    --output=build/codeql-analysis.csv \
    --threads=12

# #  --mount type=bind,source=$PWD,destination=/code/slog \
# #  --mount type=bind,source=$PWD/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources,destination=/opt/codeql \
# # A hacky attempt to run arene codeql agains code that isn't setup as arene project.
# docker run \
#  --rm \
#  -u root \
#  -w /code/slog \
#  -v /workdir:/code/slog:rw \
#  -v /workdir/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources:/opt/codeql:rw \
#     library.pdx.l5.woven-planet.tech/ci/ci-python38-bionic:1.1.3778 /usr/bin/env bash -c ' \
#         echo DEBUG ls -l /code/slog/ && ls -l /code/slog/ && \
#         echo DEBUG ls -l /opt/codeql/ && ls -l /opt/codeql/ && \
#         rm -rf build/codeql-database && \
#         mkdir -p build && \
#         /opt/codeql/bin/codeql database create build/codeql-database --language=cpp --command="bazelisk build //slog_cc" && \
#         /opt/codeql/bin/codeql database analyze build/codeql-database autosar-default.qls \
#             --format=csv \
#             --output=build/codeql-analysis.csv \
#             --threads=12 \
#     '

cat build/codeql-analysis.csv
wc build/codeql-analysis.csv

if [ -s build/codeql-analysis.csv ]
then
    echo "Code is not AUTOSAR compliant."
    exit 1
else
    echo "Code is AUTOSAR compliant."
    exit 0
fi
