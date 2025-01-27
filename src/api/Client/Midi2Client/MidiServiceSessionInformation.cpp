// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#include "pch.h"
#include "MidiServiceSessionInformation.h"
#include "MidiServiceSessionInformation.g.cpp"

namespace winrt::Windows::Devices::Midi2::implementation
{
    _Use_decl_annotations_
    void MidiServiceSessionInformation::InternalInitialize(
        winrt::guid sessionId,
        winrt::hstring sessionName,
        uint64_t processId,
        winrt::hstring processName,
        foundation::DateTime startTime)
    {
        m_sessionId = sessionId;
        m_processId = processId;
        m_processName = processName;
        m_sessionName = sessionName;
        m_startTime = startTime;
    }

    _Use_decl_annotations_
    void MidiServiceSessionInformation::InternalAddConnection(
        midi2::MidiServiceSessionConnectionInformation const& info
    )
    {
        m_connections.Append(info);
    }

}
