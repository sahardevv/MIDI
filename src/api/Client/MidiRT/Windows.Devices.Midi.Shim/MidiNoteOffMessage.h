// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiNoteOffMessage.g.h"


namespace winrt::MIDI_ROOT_NAMESPACE_CPP::implementation
{
    struct MidiNoteOffMessage : MidiNoteOffMessageT<MidiNoteOffMessage>
    {
        MidiNoteOffMessage() = default;

        MidiNoteOffMessage(_In_ uint8_t channel, _In_ uint8_t note, _In_ uint8_t velocity);
        uint8_t Channel();
        uint8_t Note();
        uint8_t Velocity();

        foundation::TimeSpan Timestamp();
        midi1::MidiMessageType Type();
        streams::IBuffer RawData();
    };
}
namespace winrt::MIDI_ROOT_NAMESPACE_CPP::factory_implementation
{
    struct MidiNoteOffMessage : MidiNoteOffMessageT<MidiNoteOffMessage, implementation::MidiNoteOffMessage>
    {
    };
}
