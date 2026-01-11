FROM ubuntu:24.04

# Install required packages
RUN \
    apt-get update \
    && apt-get install -y git sudo \
    cmake ninja-build g++ gdb \
    clang-format doxygen \
    qt6-base-dev designer-qt6 \
    qt6-tools-dev \
    qt6-charts-dev qt6-serialport-dev \
    && rm -rf /var/lib/apt/lists/*

# Grant sudo rights to the default ubuntu user
RUN usermod -aG sudo ubuntu \
    && echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
    && chmod 0440 /etc/sudoers.d/ubuntu
