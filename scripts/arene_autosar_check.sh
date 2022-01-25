#!/bin/bash

set -e

/bin/bash -c "$(curl -fsSL https://arene-lab-artifact-repo-staging.s3-ap-northeast-1.amazonaws.com/installers/install.sh)"

$HOME/.arene/bin/arene install arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu

rm -rf build/codeql-database && \
mkdir -p build && \
/workdir/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/bin/codeql database create build/codeql-database --language=cpp --command="bazelisk build //slog_cc" && \
/workdir/.arene/plugins/arene_resource_provider_codeql_any_x86_64_unknown_linux_gnu/1.26.0/x86_64-unknown-linux-gnu/resources/bin/codeql database analyze build/codeql-database autosar-default.qls \
    --format=csv \
    --output=build/codeql-analysis.csv \
    --threads=36

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

if [[ ! -z "$1" ]]
then
    cat build/codeql-analysis.csv | grep $1 >build/output.csv
else
    cat build/codeql-analysis.csv >build/output.csv
fi

cat build/output.csv
wc build/output.csv

if [ -s build/output.csv ]
then
    echo "Code is not AUTOSAR compliant."
    exit 1
else
    echo "Code is AUTOSAR compliant."
    exit 0
fi
