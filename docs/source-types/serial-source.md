---
title: Serial Port Source
parent: Source Types
nav_order: 2
---

# Serial Port Source

The Serial Port source reads raw bytes from a connected serial device and decodes them into one or more numeric variables. Two data formats are supported: **Single Byte** (one sample per received byte) and **Packet** (fixed-layout binary frames with multiple named fields).

## Adding a serial source

1. Right-click in the **Sources** pane and select **Add Serial source**.
2. Choose a **Device** from the list of available ports detected on the system.
3. Set the **Baudrate**, either by selecting a common value or typing a custom one.
4. Choose a **Data format** - Single Byte or Packet - and configure its options (see below).
5. Click **OK** to create the source.

## Data formats

### Single Byte

Each incoming byte is treated as one sample. This is the simplest mode and requires no packet framing.

**Signedness** controls how each byte is interpreted:

| Signedness | Type | Range |
|---|---|---|
| Signed (default) | `Int8` | −128 to 127 |
| Unsigned | `UInt8` | 0 to 255 |

One variable ("Signed byte" or "Unsigned byte") is exposed per source. Assign it to a channel to display it.

Timestamps for individual bytes within a received chunk are estimated based on the configured baud rate, spacing samples backward from the time the chunk was received.

### Packet

Fixed-layout binary frames are extracted from the stream and decoded field by field. Each field becomes its own variable that can be independently assigned to a channel.

**Packet parameters:**

| Parameter | Description |
|---|---|
| Start magic (hex) | Byte sequence marking the start of every packet. Required. Enter as space-separated hex pairs, e.g. `AD 57`. |
| End magic (hex) | Optional byte sequence marking the end of every packet. Leave blank if not used. |
| Packet length (bytes) | Total length of the packet in bytes, including start and end magic. Must be at least 1. |

The parser scans the incoming byte stream for the start magic, then extracts a window of *packet length* bytes. If an end magic is configured, the candidate packet is also checked against it before decoding.

**Adding fields:**

Click **+ Add field** to define each variable **within** the packet. Each field requires:

| Parameter | Description |
|---|---|
| Name | Display name for this variable in the Sources list. |
| Offset (bytes) | Zero-based byte offset of the field within the packet. |
| Type | Numeric type of the field (see table below). |
| Endianness | Byte order for multi-byte types: **Little** (default) or **Big**. Not shown for 1-byte types. |

**Supported field types:**

| Type | Size | Range |
|---|---|---|
| `Int8` | 1 byte | −128 to 127 |
| `UInt8` | 1 byte | 0 to 255 |
| `Int16` | 2 bytes | −32,768 to 32,767 |
| `UInt16` | 2 bytes | 0 to 65,535 |
| `Int32` | 4 bytes | −2,147,483,648 to 2,147,483,647 |
| `UInt32` | 4 bytes | 0 to 4,294,967,295 |

Each field is shown in the Sources list with its offset and type, for example: `Voltage @2 (Int16, LE)`. Single-byte fields omit the endianness suffix.

## Port configuration

The following parameters are currently fixed and not user-configurable:

| Parameter | Value |
|---|---|
| Data bits | 8 |
| Parity | Odd |
| Stop bits | 1 |
| Flow control | None |

## Notes

- In packet mode, bytes before the first recognised start magic are discarded. If the end magic does not match, the parser advances by one byte and retries, allowing recovery from corrupted frames.
- Each field in packet mode is its own variable and must be individually assigned to a channel.
