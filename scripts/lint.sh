#!/bin/bash

# Usage (from repo root dir):
#   scripts/lint.sh <path> [-i]
#   e.g.
#     scripts/lint.sh ./
#     scripts/lint.sh slog/primitives/ -i 

set -e

find $1 -name *.h > /tmp/files_to_lint_cc
find $1 -name *.cc >> /tmp/files_to_lint_cc

find $1 -name *.py > /tmp/files_to_lint_py

if [[ ! -z "$2" ]] && [ "$2" == "-i" ]
then
    # Lint-fix Bazel files.
    buildifier --mode=fix -r $1
    # Lint-fix C++ files.
    cat /tmp/files_to_lint_cc | xargs -d $'\n' sh -c 'for arg do clang-format -style=Google -i $arg; done' _
    # Lint-fix Python files.
    cat /tmp/files_to_lint_py | xargs autopep8 --in-place $arg
fi

touch /tmp/diff && rm /tmp/diff
touch /tmp/formated && rm /tmp/formated

# Find lint errors in Bazel files.
buildifier --mode=diff -r $1
# Find lint errors in C++ files.
cat /tmp/files_to_lint_cc | xargs -d $'\n' sh -c 'for arg do clang-format -style=Google $arg >/tmp/formated && diff --unified=0 $arg /tmp/formated >>/tmp/diff; done' _
# Find lint errors in Python files.
cat /tmp/files_to_lint_py | xargs autopep8 --diff $arg >>/tmp/diff

cat /tmp/diff
if [ -s /tmp/diff ]
then
    echo "Code needs formatting."
    exit 1
else
    echo "Code is well formatted."
    exit 0
fi
