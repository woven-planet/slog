#!/bin/bash

PACKAGE_PATH=tmp\\/slog
if [[ "${BUILDKITE_BRANCH}" = "main" ]]; then
    PACKAGE_PATH=slog
fi

cat WORKSPACE.template \
    | sed "s/{{version}}/v$1/g" \
    | sed "s/{{package_path}}/$PACKAGE_PATH/g" \
    > WORKSPACE
