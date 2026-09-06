---
title: Simulated Source
parent: Source Types
nav_order: 1
---

# Simulated Source

The Simulated source generates synthetic waveforms without any external hardware. It's useful for testing channel assignment, scaling, and display behavior without a live data feed.

## Adding a simulated source

1. Right-click in the **Sources** pane and select **Add Simulated source**.
2. In the configuration dialog, click **+ Add new form** to define a waveform.
3. Choose a form type and set its parameters (see below).
4. Repeat to add multiple waveforms — each becomes a separate variable you can assign to its own channel.
5. Click **OK** to create the source.

Each form can be edited or deleted from the list before confirming.

## Form types

### Constant Value

Outputs a fixed value on every sample.

| Parameter | Description |
|---|---|
| Value | The constant output value (range: -1000 to 1000). |

### Sinusoidal Wave

Outputs a sine wave.

| Parameter | Description |
|---|---|
| Amplitude | Peak amplitude of the wave (range: 1 to 1000). |
| Frequency (Hz) | Wave frequency in hertz (range: 1 to 1,000,000). |

## Sample rate

The simulated source samples at a fixed rate of 100 Hz, independent of the waveform frequency.

## Notes

- Frequencies above roughly half the sample rate will alias rather than display as a clean sine wave — this is a sampling limitation, not a bug in the reader.
- Each form is tracked as its own variable and must be individually assigned to a channel; adding a form does not automatically enable a channel.
