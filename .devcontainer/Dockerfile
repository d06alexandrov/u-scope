FROM ubuntu:24.04 AS base-img

LABEL version="0.1"

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
