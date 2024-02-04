#pragma once
#include "MidiSessionInformation.g.h"


namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiSessionInformation : MidiSessionInformationT<MidiSessionInformation>
    {
        MidiSessionInformation() = default;

        winrt::guid SessionId() { return m_sessionId; }
        uint64_t ProcessId() { return m_processId; }
        winrt::hstring ProcessName() { return m_processName; }
        winrt::hstring SessionName() { return m_sessionName; }
        foundation::DateTime StartTime() { return m_startTime; }

        collections::IVectorView<midi2::MidiSessionConnectionInformation> Connections() { return m_connections.GetView(); }

    private:
        winrt::guid m_sessionId{};
        uint64_t m_processId{};
        winrt::hstring m_processName{};
        winrt::hstring m_sessionName{};
        foundation::DateTime m_startTime{};

        foundation::Collections::IVector<midi2::MidiSessionConnectionInformation>
            m_connections{ winrt::single_threaded_vector<midi2::MidiSessionConnectionInformation>() };


    };
}
