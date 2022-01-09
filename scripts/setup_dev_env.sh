# Install bazelisk that will install bazel
wget https://github.com/bazelbuild/bazelisk/releases/download/v1.11.0/bazelisk-linux-amd64
chmod +x bazelisk-linux-amd64
sudo mv bazelisk-linux-amd64 /usr/local/bin/bazelisk

# Make `python` pointing to python3.8
sudo update-alternatives --install /usr/bin/python python /home/buildkite/.pyenv/versions/3.8.12/bin/python3.8 10

# Install zstd, it is needed to import python interpreter.
sudo apt-get install zstd
