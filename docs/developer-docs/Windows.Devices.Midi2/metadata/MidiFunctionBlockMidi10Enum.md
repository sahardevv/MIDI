---
layout: api_page
title: MidiFunctionBlockMidi10
parent: Metadata
grand_parent: Windows.Devices.Midi2 API
has_children: false
---

# MidiFunctionBlockMidi10 Enumeration

Indicates the MIDI 1.0 capability restrictions for a function block. Note that Windows MIDI Services does not currently throttle the speed of outbound messages, even if the block indicates it has restricted bandwidth.

## Properties

| Property | Value | Description |
| -------- | ------- | ------ |
| `Not10` | `0x0` | This function block is not a MIDI 1.0 function. |
| `YesBandwidthUnrestricted` | `0x1` | This block represents a MIDI 1.0 function, but has the ability to receive messages faster than the original MIDI 1.0 protocol speed. |
| `YesBandwidthRestricted` | `0x2` | This block represents a MIDI 1.0 function, and applications should take care to send messages to it at the normal MIDI 1.0 protocol speed. |
| `Reserved` | `0x3` | Reserved for future use. |

## IDL

[MidiFunctionBlockMidi10Enum IDL](https://github.com/microsoft/MIDI/blob/main/src/api/Client/Midi2Client/MidiFunctionBlockMidi10Enum.idl)
