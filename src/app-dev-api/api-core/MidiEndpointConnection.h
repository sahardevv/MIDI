// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiEndpointConnection.g.h"

#include "midi_service_interface.h"
//#include <functional>

namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiEndpointConnection : MidiEndpointConnectionT<MidiEndpointConnection>
    {
        MidiEndpointConnection() = default;

        static hstring GetDeviceSelector();

        hstring Id() { return m_id; }
        hstring DeviceId() { return m_deviceId; }
        bool IsConnected() { return m_isConnected; }
        IMidiEndpointConnectionSettings Settings() { return m_settings; }

        IInspectable Tag() { return m_tag; }
        void Tag(IInspectable value) { m_tag = value; }

        void InternalSetSettings(IMidiEndpointConnectionSettings value) { m_settings = value; }
        void InternalSetDeviceId(hstring value) { m_deviceId = value; }
        void InternalSetId(hstring value) { m_id = value; }
        void InternalSetIsConnected(bool value) { m_isConnected = value; }

    protected:
        hstring m_id {};
        hstring m_deviceId {};
        IInspectable m_tag{ nullptr };
        
        bool m_isConnected{ false };

        IMidiEndpointConnectionSettings m_settings{ nullptr };

    };
}
namespace winrt::Windows::Devices::Midi2::factory_implementation
{
    struct MidiEndpointConnection : MidiEndpointConnectionT<MidiEndpointConnection, implementation::MidiEndpointConnection>
    {
    };
}