# U-Scope: Oscilloscope-Style Streaming Data Visualizer

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

## Usage

Launch the application by running the `u-scope` executable (`u-scope.exe` on Windows).

![Application overview](./docs/images/app_overview.png)

The user interface is divided into three primary components:

*   **Sources:** A configuration list for all active data inputs.
*   **Screen:** The main graphical display area.
*   **Control Panel:** Core interface elements for managing data capture and scaling.

### Source Configuration

To add a new data source, right-click within the **Sources** pane and select your desired source type.

Currently, two source types are supported:

*   **Simulated Source:** A waveform generator that outputs constant and sinusoidal signals with configurable parameters.
*   **Serial Port Source:** Reads incoming serial data, interpreting each byte as a signed 8-bit integer (ranging from -128 to 127).

![Source configuration context menu](./docs/images/source_configuration.png)

### Channel Assignment

Once your sources are configured, you must map their respective data streams to specific display channels. 

To do this, right-click on a target data stream, select **Assign to channel**, and click on your desired channel number.

![Channel assignment context menu](./docs/images/channel_assignment.png)

### Start (Roll Mode)

Click the **Start** button to begin data acquisition. This activates Roll Mode, providing a continuous, real-time display of the incoming data.

![Working application](./docs/images/channel_selection.png)

### Division Manipulation

U-Scope replicates traditional hardware oscilloscope behavior for graphical scaling. 

You can adjust the **Horizontal Scale** knob to modify the time resolution per horizontal division; the current value is dynamically displayed in the top-left corner of the screen. 

To adjust vertical scaling, first select a specific channel by clicking its corresponding indicator in the bar below the graph. Once selected, you can use the **Vertical Scale** knob to independently scale that channel's data (the current vertical scale is displayed directly on the channel's indicator tab).

![Vertical division](./docs/images/vertical_scale.png)

### Stop (Stopped Mode)

Click the **Stop** button to halt data acquisition. 

This transitions the application into Stopped Mode, which is designed for detailed analysis of the captured dataset. In this mode, you can utilize the sliding window at the top of the screen, alongside the horizontal and vertical scale knobs, to zoom in on specific segments of the gathered data.

![Working application](./docs/images/stopped_mode.png)

## Author

Dmitriy Alexandrov [d06alexandrov](https://github.com/d06alexandrov)

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](./LICENSE) file for details.
