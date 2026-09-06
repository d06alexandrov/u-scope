---
title: Serial Port Source
parent: Source Types
nav_order: 2
---

# Serial Port Source

The Serial Port source reads raw bytes from a connected serial device and interprets each byte as a signed 8-bit integer, ranging from -128 to 127.

## Adding a serial source

1. Right-click in the **Sources** pane and select **Add Serial source**.
2. Choose a **Device** from the list of available ports detected on the system.
3. Set the **Baudrate**, either by selecting a common value or typing a custom one.
4. Click **OK** to create the source.

The dialog requires a device to be selected and a baud rate of at least 9600 before it can be confirmed.

## Port configuration

The following parameters are currently fixed and not user-configurable:

| Parameter | Value |
|---|---|
| Data bits | 8 |
| Parity | Odd |
| Stop bits | 1 |
| Flow control | None |

## Data interpretation

Each incoming byte is treated as one signed 8-bit sample. Timestamps for individual bytes within a received chunk are estimated based on the configured baud rate, spacing samples backward from the time the chunk was received.

## Notes

- Only one variable ("Byte data") is exposed per serial source; assign it to a channel to display it.
- If the port can't be opened (e.g. already in use, insufficient permissions, or disconnected), the source will report an error status rather than silently failing.
