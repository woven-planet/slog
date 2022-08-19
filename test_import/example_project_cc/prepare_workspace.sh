#!/bin/bash

cat WORKSPACE.template \
    | sed "s/{{git_commit}}/$1/g" \
    > WORKSPACE
