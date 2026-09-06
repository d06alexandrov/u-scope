---
title: System Requirements
parent: Getting Started
nav_order: 1
---

# System Requirements

## Supported platforms

U-Scope currently supports:

- **Windows** - prebuilt releases available.
- **Linux** - no prebuilt binaries yet; build from source.

## Build dependencies

If you're building from source, you'll need:

- **Qt 6.10 or later**, including these components:
  - Widgets
  - Quick
  - QuickWidgets
  - Qml
  - Graphs
  - SerialPort
  - LinguistTools
- **CMake 3.24 or later**
- A C++20-capable compiler

## Runtime

Serial port sources require access to the host's serial devices. On Linux this typically means the user running U-Scope needs permission to access `/dev/ttyUSB*` or `/dev/ttyACM*` (commonly via membership in the `dialout` group).
