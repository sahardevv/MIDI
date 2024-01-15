// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiChannelPressureMessage.g.h"


namespace winrt::MIDI_ROOT_NAMESPACE_CPP::implementation
{
    struct MidiChannelPressureMessage : MidiChannelPressureMessageT<MidiChannelPressureMessage>
    {
        MidiChannelPressureMessage() = default;
        MidiChannelPressureMessage(_In_ uint8_t channel, _In_ uint8_t pressure);

        uint8_t Channel() { return m_channel; }
        uint8_t Pressure() { return m_pressure; }

        foundation::TimeSpan Timestamp() { return m_timestamp; }
        midi1::MidiMessageType Type() { return m_type; }
        streams::IBuffer RawData();

    private:
        foundation::TimeSpan m_timestamp{};
        midi1::MidiMessageType m_type{ midi1::MidiMessageType::None };

        uint8_t m_channel{};
        uint8_t m_pressure{};
    };
}
namespace winrt::MIDI_ROOT_NAMESPACE_CPP::factory_implementation
{
    struct MidiChannelPressureMessage : MidiChannelPressureMessageT<MidiChannelPressureMessage, implementation::MidiChannelPressureMessage>
    {
    };
}
