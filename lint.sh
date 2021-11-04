#!/bin/bash

# Usage:
#   ./lint.sh <path> [-i]
#   e.g.
#     ./lint.sh ./
#     ./lint.sh src/primitives/ -i 

find $1 -name *.h > /tmp/files_to_lint
find $1 -name *.cc >> /tmp/files_to_lint

if [[ ! -z "$2" ]] && [ "$2" == "-i" ]
then
    cat /tmp/files_to_lint | xargs -d $'\n' sh -c 'for arg do clang-format -style=Google -i $arg; done' _
fi

touch /tmp/diff
rm /tmp/diff
cat /tmp/files_to_lint | xargs -d $'\n' sh -c 'for arg do clang-format -style=Google $arg >/tmp/formated; diff --unified=0 $arg /tmp/formated >>/tmp/diff; done' _

cat /tmp/diff
if [ -s /tmp/diff ]
then
    echo "Code needs formatting."
    exit 1
else
    echo "Code is well formatted."
    exit 0
fi
