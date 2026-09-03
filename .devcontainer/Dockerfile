FROM dalogik/qt-docker:qt6.11.0-linux64-gcc AS base-img

LABEL version="0.7"

# Install additional required packages
RUN apt-get update \
    && apt-get install -y --no-install-suggests --no-install-recommends \
    clang-format clang-tidy cmake-format doxygen \
    && rm -rf /var/lib/apt/lists/*
