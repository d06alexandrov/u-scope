# U-Scope: Oscilloscope-Style Streaming Data Visualizer

U-Scope is a streaming data visualizer with an oscilloscope-style interface, supporting configurable data sources and multi-channel display.

![Application overview](./docs/assets/images/app_overview.png)

## Installation

U-Scope currently supports both Windows and Linux operating systems.

### Windows
Download and extract the latest Windows [release](https://github.com/d06alexandrov/u-scope/releases).

### Linux
There are currently no prebuilt Linux binaries. To build from source, use the VS Code dev container, or run the following on a system with Qt 6.10+ installed:

```bash
cmake -B build
cmake --build build
```

The resulting `u-scope` executable will be in `build/source/`.

See the [System Requirements](https://d06alexandrov.github.io/u-scope/getting-started/system-requirements.html) page for full build dependencies.

## Documentation

Full documentation, including the User Guide, source type reference, and API docs, is available at:

**[https://d06alexandrov.github.io/u-scope/](https://d06alexandrov.github.io/u-scope/)**

## Author

Dmitriy Alexandrov [d06alexandrov](https://github.com/d06alexandrov)

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](./LICENSE) file for details.
