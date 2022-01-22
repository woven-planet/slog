#!/bin/bash

# Install Bazel linter.
wget -c https://dl.google.com/go/go1.14.2.linux-amd64.tar.gz -O - | sudo tar -xz -C /usr/local
/usr/local/go/bin/go get github.com/bazelbuild/buildtools/buildifier
sudo ln -s $HOME/go/bin/buildifier /usr/bin/buildifier

# Install C++ linter.
sudo apt update && sudo apt install clang-format -y

# Install Python linter.
sudo apt-get install python-pip -y
pip install --upgrade autopep8
sudo ln -s $HOME/.local/bin/autopep8 /usr/bin/autopep8

