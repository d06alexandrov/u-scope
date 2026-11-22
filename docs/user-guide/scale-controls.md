---
title: Scale Controls
parent: User Guide
nav_order: 5
description: "How to use U-Scope's horizontal and vertical scale and position dials to zoom into and shift captured waveform data per channel."
---

# Scale Controls

U-Scope replicates traditional hardware oscilloscope behavior for graphical scaling, using dials in the Control Panel.

## Horizontal Scale

Adjusts the time resolution per horizontal division. The current value is displayed in the top-left corner of the screen.

In [Stopped Mode](./working-modes.html#stopped-mode), a second **Horizontal Position** dial becomes available, letting you move the sliding window along the captured history in increments of one division.

## Vertical Scale

![Vertical division]({{ '/assets/images/vertical_scale.png' | relative_url }})

Adjusts the amplitude scale of the **currently selected channel** independently of other channels.

To use it:

1. Select a channel by clicking its badge in the [channel bar](./channel-assignment.html#enabling-and-selecting-channels).
2. Turn the **Vertical Scale** dial in the Control Panel.

The current vertical scale is shown directly on the selected channel's badge in the channel bar. If no channel is selected, the vertical scale controls are disabled.

## Vertical Position

![Vertical position markers]({{ '/assets/images/vertical_position.png' | relative_url }})

Moves the waveform of the **currently selected channel** up or down on the screen, independently of other channels. This makes it possible to separate overlapping signals.

To use it:

1. Select a channel by clicking its badge in the [channel bar](./channel-assignment.html#enabling-and-selecting-channels).
2. Turn the **Vertical Position** dial in the Control Panel. The dial turns continuously, and one full turn moves the waveform by one vertical division.

The zero level of each enabled channel is marked by a triangle in its channel color on the left edge of the screen. The marker of the selected channel has a white outline. If a channel is shifted beyond the visible area, its marker is hidden.

The vertical position and scale of a channel are reset when a new variable is assigned to it or when it is disconnected.
