FROM ubuntu:24.04

RUN \
    apt-get update \
    && apt-get install -y git sudo \
    cmake ninja-build g++ gdb \
    qt6-base-dev designer-qt6 \
    qt6-charts-dev qt6-serialport-dev \
    && rm -rf /var/lib/apt/lists/*
