---
title: Development Container
parent: Getting Started
nav_order: 3
---

# Development Container

U-Scope ships with a [dev container](https://containers.dev/) configuration for a consistent Linux build environment, including Qt, CMake, and all formatting/linting tools preinstalled.

## Requirements

- [Docker](https://www.docker.com/) (or a compatible container runtime)
- [VS Code](https://code.visualstudio.com/) with the [Dev Containers extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode-remote.remote-containers)

## Opening the project

1. Clone the repository.
2. Open the folder in VS Code.
3. When prompted, select **Reopen in Container** (or run **Dev Containers: Reopen in Container** from the command palette).

VS Code will build the container image from `.devcontainer/Dockerfile.dev`, which extends the base U-Scope image with `sudo`, `gdb`, and `ssh` for a more complete development setup.

## Building inside the container

Once the container is running, build using the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension bundled with the `TheQtCompany.qt-cpp-pack` pack:

1. Open the Command Palette and run **CMake: Configure** (or click **Configure** in the CMake Tools status bar). This generates the build directory using the project's `CMakeLists.txt`.
2. Select a kit if prompted - the container's Qt installation should be picked up automatically.
3. Run **CMake: Build** (or use the build button in the status bar, or `F7`) to build the `u-scope` target.

CMake Tools also drives debugging, target selection, and test execution from the same status bar, so it's the recommended workflow over invoking `cmake` directly from the terminal inside the container.


## Notes

- The container sets `LIBGL_ALWAYS_SOFTWARE=1` and `QT_QPA_PLATFORM=xcb` so the Qt Quick UI renders without requiring GPU passthrough - useful for remote or headless dev setups, though slower than hardware-accelerated rendering.
- The default user is `ubuntu`, with passwordless `sudo` enabled inside the container.
- Recommended VS Code extensions are installed automatically via `devcontainer.json`.
