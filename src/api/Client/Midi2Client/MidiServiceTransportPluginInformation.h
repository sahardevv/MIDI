// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once
#include "MidiServiceTransportPluginInformation.g.h"

namespace winrt::Windows::Devices::Midi2::implementation
{
    struct MidiServiceTransportPluginInformation : MidiServiceTransportPluginInformationT<MidiServiceTransportPluginInformation>
    {
        MidiServiceTransportPluginInformation() = default;

        winrt::guid ClassId() const { return m_classId; }
        winrt::hstring RegistryKey() const { return m_registryKey; }
        bool IsEnabled() const { return m_isEnabled; }
        winrt::hstring Name() const { return m_name; }
        winrt::hstring ShortName() const { return m_shortName; }
        winrt::hstring Mnemonic() const { return m_mnemonic; }
        winrt::hstring Description() const { return m_description; }
        winrt::hstring IconPath() const { return m_iconPath; }
        winrt::hstring Author() const { return m_author; }
        winrt::hstring ServicePluginFileName() const { return m_servicePluginFileName; }
        bool IsSystemManaged() const { return m_isSystemManaged; }
        bool IsRuntimeCreatable() const { return m_isRuntimeCreatable; }
        bool IsClientConfigurable() const { return m_isClientConfigurable; }
        winrt::hstring ClientConfigurationAssemblyName() const { return m_clientConfigurationAssemblyName; }

    private:
        winrt::guid m_classId{};
        winrt::hstring m_registryKey{};
        bool m_isEnabled{ false };
        winrt::hstring m_name{};
        winrt::hstring m_shortName{};
        winrt::hstring m_mnemonic{};
        winrt::hstring m_description{};
        winrt::hstring m_iconPath{};
        winrt::hstring m_author{};
        winrt::hstring m_servicePluginFileName{};
        bool m_isSystemManaged{ false };
        bool m_isRuntimeCreatable{ false };
        bool m_isClientConfigurable{ false };
        winrt::hstring m_clientConfigurationAssemblyName{};
    };
}
