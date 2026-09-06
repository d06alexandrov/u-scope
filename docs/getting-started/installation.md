---
title: Installation
parent: Getting Started
nav_order: 2
---

# Installation

## Windows

Download and extract the latest Windows release from the [Releases page](https://github.com/d06alexandrov/u-scope/releases).

Run `u-scope.exe` from the extracted folder.

## Linux

There are currently no prebuilt Linux binaries. Build from source using either the provided dev container (see [Development Container](./dev-container.html)) or a system with Qt 6.10+ installed:

```bash
cmake -B build
cmake --build build
```

The resulting `u-scope` executable will be in `build/source/`.

## Verifying the build

Once built, launch the executable directly:

```bash
./build/source/u-scope
```

You should see the main window with three panes: Sources, Screen, and Control Panel. See the [User Guide](../user-guide/) for how to configure a data source and start capturing data.
