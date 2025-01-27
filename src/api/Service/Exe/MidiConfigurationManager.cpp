// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================


//
// TODO: Refactor this code to use the json_helpers.h functions and defines
//


#include "stdafx.h"

#include "MidiConfigurationManager.h"

#define MAX_KEY_LENGTH 255
#define MAX_VALUE_NAME 16383


// TODO: Refactor these two methods and abstract out the registry code. Do this once wil adds the enumeration helpers to the NuGet
std::vector<GUID> CMidiConfigurationManager::GetEnabledTransportAbstractionLayers() const noexcept
{
    //OutputDebugString(L"GetEnabledAbstractionLayers");

    std::vector<GUID> availableAbstractionLayers;

    // the diagnostics abstraction is always instantiated. We don't want to take the
    // chance someone removes this from the registry, so it's hard-coded here. It's
    // needed for support, tools, diagnostics, etc.
    availableAbstractionLayers.push_back(__uuidof(Midi2DiagnosticsAbstraction));

    try
    {
        auto rootKey = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, MIDI_ROOT_TRANSPORT_PLUGINS_REG_KEY, wil::reg::key_access::read);

        //OutputDebugString(L"Root Key Opened");

        // current wil NuGet doesn't include the new registry key/value enumeration, so have to do this the hard way
        // https://github.com/microsoft/wil/commit/a6c9f2d8520e830b04c47c3c395bac428696cc0d

        TCHAR    achKey[MAX_KEY_LENGTH];        // buffer for subkey name
        DWORD    cbName;                        // size of name string 
        TCHAR    achClass[MAX_PATH] = TEXT(""); // buffer for class name 
        DWORD    cchClassName = MAX_PATH;       // size of class string 
        DWORD    cSubKeys = 0;                  // number of subkeys 
        DWORD    cbMaxSubKey;                   // longest subkey size 
        DWORD    cchMaxClass;                   // longest class string 
        DWORD    cValues;                       // number of values for key 
        DWORD    cchMaxValue;                   // longest value name 
        DWORD    cbMaxValueData;                // longest value data 
        DWORD    cbSecurityDescriptor;          // size of security descriptor 
        FILETIME ftLastWriteTime;               // last write time 

        DWORD i, retCode;

        //TCHAR  achValue[MAX_VALUE_NAME];
        //DWORD cchValue = MAX_VALUE_NAME;

        // Get the class name and the value count. 
        retCode = RegQueryInfoKey(
            rootKey.get(),           // key handle 
            achClass,                // buffer for class name 
            &cchClassName,           // size of class string 
            NULL,                    // reserved 
            &cSubKeys,               // number of subkeys 
            &cbMaxSubKey,            // longest subkey size 
            &cchMaxClass,            // longest class string 
            &cValues,                // number of values for this key 
            &cchMaxValue,            // longest value name 
            &cbMaxValueData,         // longest value data 
            &cbSecurityDescriptor,   // security descriptor 
            &ftLastWriteTime);       // last write time 

        // Enumerate the subkeys, until RegEnumKeyEx fails.

        //OutputDebugString(L"Queried Info Key");


        if (cSubKeys)
        {
            for (i = 0; i < cSubKeys; i++)
            {
                //OutputDebugString(L"Reading Sub Key");

                cbName = MAX_KEY_LENGTH;

                retCode = RegEnumKeyEx(
                    rootKey.get(), 
                    i,
                    achKey,
                    &cbName,
                    NULL,
                    NULL,
                    NULL,
                    &ftLastWriteTime);

                if (retCode == ERROR_SUCCESS)
                {
                    //OutputDebugString(L"Sub Key Read");

                    DWORD abstractionEnabled = 1;

                    // Check to see if that sub key has an Enabled value.

                    std::wstring keyPath = MIDI_ROOT_TRANSPORT_PLUGINS_REG_KEY;
                    keyPath += L"\\";
                    keyPath += achKey;

                    //OutputDebugString(L"Opening Key");

                    auto pluginKey = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, keyPath.c_str(), wil::reg::key_access::read);

                    try
                    {
                        //OutputDebugString(L"Getting Enabled Value");

                        // is the abstraction layer enabled? > 0 or missing Enabled value mean it is
                        abstractionEnabled = wil::reg::get_value<DWORD>(pluginKey.get(), MIDI_PLUGIN_ENABLED_REG_VALUE);
                    }
                    catch (...)
                    {
                        // value doesn't exist, so we default to enabled
                        abstractionEnabled = (DWORD)1;
                    }

                    // we only proceed with this abstraction entry if it's enabled
                    if (abstractionEnabled > 0)
                    {
                        try
                        {
                            //OutputDebugString(L"Getting CLSID Value");

                            GUID abstractionLayerGuid;

                            auto clsidString = wil::reg::get_value<std::wstring>(pluginKey.get(), MIDI_PLUGIN_CLSID_REG_VALUE);

                            auto result = CLSIDFromString((LPCTSTR)clsidString.c_str(), (LPGUID)&abstractionLayerGuid);

                            LOG_IF_FAILED(result);

                            if (result == NOERROR)
                            {
                                //OutputDebugString(L"Adding CLSID to Vector");

                                // Add to the vector
                                availableAbstractionLayers.push_back(abstractionLayerGuid);
                            }

                        }
                        CATCH_LOG()
                    }
                }
            }
        }

    }
    CATCH_LOG()

    // return a copy
    return availableAbstractionLayers;
}

// TODO: Refactor these two methods and abstract out the registry code. Do this once wil adds the enumeration helpers to the NuGet
std::vector<GUID> CMidiConfigurationManager::GetEnabledEndpointProcessingTransforms() const noexcept
{
    //OutputDebugString(L"GetEnabledAbstractionLayers");

    std::vector<GUID> availableAbstractionLayers;

    // the diagnostics abstraction is always instantiated. We don't want to take the
    // chance someone removes this from the registry, so it's hard-coded here. It's
    // needed for support, tools, diagnostics, etc.
    availableAbstractionLayers.push_back(__uuidof(Midi2DiagnosticsAbstraction));

    try
    {
        auto rootKey = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, MIDI_ROOT_ENDPOINT_PROCESSING_PLUGINS_REG_KEY, wil::reg::key_access::read);

        //OutputDebugString(L"Root Key Opened");

        // current wil NuGet doesn't include the new registry key/value enumeration, so have to do this the hard way
        // https://github.com/microsoft/wil/commit/a6c9f2d8520e830b04c47c3c395bac428696cc0d

        TCHAR    achKey[MAX_KEY_LENGTH];        // buffer for subkey name
        DWORD    cbName;                        // size of name string 
        TCHAR    achClass[MAX_PATH] = TEXT(""); // buffer for class name 
        DWORD    cchClassName = MAX_PATH;       // size of class string 
        DWORD    cSubKeys = 0;                  // number of subkeys 
        DWORD    cbMaxSubKey;                   // longest subkey size 
        DWORD    cchMaxClass;                   // longest class string 
        DWORD    cValues;                       // number of values for key 
        DWORD    cchMaxValue;                   // longest value name 
        DWORD    cbMaxValueData;                // longest value data 
        DWORD    cbSecurityDescriptor;          // size of security descriptor 
        FILETIME ftLastWriteTime;               // last write time 

        DWORD i, retCode;

        //TCHAR  achValue[MAX_VALUE_NAME];
        //DWORD cchValue = MAX_VALUE_NAME;

        // Get the class name and the value count. 
        retCode = RegQueryInfoKey(
            rootKey.get(),           // key handle 
            achClass,                // buffer for class name 
            &cchClassName,           // size of class string 
            NULL,                    // reserved 
            &cSubKeys,               // number of subkeys 
            &cbMaxSubKey,            // longest subkey size 
            &cchMaxClass,            // longest class string 
            &cValues,                // number of values for this key 
            &cchMaxValue,            // longest value name 
            &cbMaxValueData,         // longest value data 
            &cbSecurityDescriptor,   // security descriptor 
            &ftLastWriteTime);       // last write time 

        // Enumerate the subkeys, until RegEnumKeyEx fails.

        //OutputDebugString(L"Queried Info Key");


        if (cSubKeys)
        {
            for (i = 0; i < cSubKeys; i++)
            {
                //OutputDebugString(L"Reading Sub Key");

                cbName = MAX_KEY_LENGTH;

                retCode = RegEnumKeyEx(
                    rootKey.get(),
                    i,
                    achKey,
                    &cbName,
                    NULL,
                    NULL,
                    NULL,
                    &ftLastWriteTime);

                if (retCode == ERROR_SUCCESS)
                {
                    //OutputDebugString(L"Sub Key Read");

                    DWORD abstractionEnabled = 1;

                    // Check to see if that sub key has an Enabled value.

                    std::wstring keyPath = MIDI_ROOT_ENDPOINT_PROCESSING_PLUGINS_REG_KEY;
                    keyPath += L"\\";
                    keyPath += achKey;

                    //OutputDebugString(L"Opening Key");

                    auto pluginKey = wil::reg::open_unique_key(HKEY_LOCAL_MACHINE, keyPath.c_str(), wil::reg::key_access::read);

                    try
                    {
                        //OutputDebugString(L"Getting Enabled Value");

                        // is the abstraction layer enabled? > 0 or missing Enabled value mean it is
                        abstractionEnabled = wil::reg::get_value<DWORD>(pluginKey.get(), MIDI_PLUGIN_ENABLED_REG_VALUE);
                    }
                    catch (...)
                    {
                        // value doesn't exist, so we default to enabled
                        abstractionEnabled = (DWORD)1;
                    }

                    // we only proceed with this abstraction entry if it's enabled
                    if (abstractionEnabled > 0)
                    {
                        try
                        {
                            //OutputDebugString(L"Getting CLSID Value");

                            GUID abstractionLayerGuid;

                            auto clsidString = wil::reg::get_value<std::wstring>(pluginKey.get(), MIDI_PLUGIN_CLSID_REG_VALUE);

                            auto result = CLSIDFromString((LPCTSTR)clsidString.c_str(), (LPGUID)&abstractionLayerGuid);

                            LOG_IF_FAILED(result);

                            if (result == NOERROR)
                            {
                                //OutputDebugString(L"Adding CLSID to Vector");

                                // Add to the vector
                                availableAbstractionLayers.push_back(abstractionLayerGuid);
                            }

                        }
                        CATCH_LOG()
                    }
                }
            }
        }

    }
    CATCH_LOG()

    // return a copy
    return availableAbstractionLayers;
}

// this gets just the file name, not the full path
std::wstring CMidiConfigurationManager::GetCurrentConfigurationFileName() noexcept
{
    std::wstring keyPath = MIDI_ROOT_REG_KEY;

    try
    {
        auto val = wil::reg::get_value<std::wstring>(
            HKEY_LOCAL_MACHINE, 
            keyPath.c_str(), 
            MIDI_CONFIG_FILE_REG_VALUE);


        if (!val.empty())
        {
            // strip out any slashes just to prevent hacking this value
            // I couldn't find a built-in function to do this, although I'm sure
            // something reasonable exists
            const std::wstring illegalCharacters = L"><:?*/\\|";
            for (auto iter = val.begin(); iter < val.end(); iter++)
            {
                // in-place sub any illegal characters with a dash
                if (illegalCharacters.find(*iter) != std::wstring::npos)
                    *iter = '-';
            }
        }
        return val;
    }
    catch (...)
    {
        // value is not present in the registry, so just return empty string

        return L"";
    }

}



#pragma push_macro("GetObject")
#undef GetObject

std::map<GUID, std::wstring, GUIDCompare> CMidiConfigurationManager::GetTransportAbstractionSettingsFromJsonString(
    _In_ std::wstring jsonStringSource) const noexcept
{
    TraceLoggingWrite(
        MidiSrvTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );


    //std::map<winrt::guid, std::wstring> abstractionSettings{};
    std::map<GUID, std::wstring, GUIDCompare> abstractionSettings{};

    try
    {       
        json::JsonObject jsonObject{ };

        try
        {
            if (!json::JsonObject::TryParse(jsonStringSource, jsonObject))
            {
                TraceLoggingWrite(
                    MidiSrvTelemetryProvider::Provider(),
                    __FUNCTION__,
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingPointer(this, "this"),
                    TraceLoggingWideString(L"JSON Object parsing failed", "message"),
                    TraceLoggingWideString(jsonStringSource.c_str(), "Source JSON String")
                );

                // return the empty map
                return abstractionSettings;
            }
        }
        catch (...)
        {
            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(L"JSON Object parsing failed. Exception.", "message")
            );

            return abstractionSettings;
        }


        if (jsonObject == nullptr)
        {
            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(L"JSON Object null after parsing", "message")
            );

            // return the empty map
            return abstractionSettings;
        }


        // we treat this as an object where each abstraction id is a property
        auto plugins = internal::JsonGetObjectProperty(jsonObject, MIDI_CONFIG_JSON_TRANSPORT_PLUGIN_SETTINGS_OBJECT, nullptr);

        if (plugins == nullptr)
        {
            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(L"No transport plugins node in JSON", "message")
            );

            // return the empty map
            return abstractionSettings;
        }

        // Iterate through nodes and find each transport abstraction entry. Parse the GUID. Add to results.

        auto it = plugins.First();

        while (it.HasCurrent())
        {
            std::wstring key = it.Current().Key().c_str();

            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(key.c_str(), "AbstractionIdGuidString", "message")
            );

            std::wstring transportJson = it.Current().Value().GetObject().Stringify().c_str();

            GUID abstractionId;

            try
            {
                abstractionId = internal::StringToGuid(key);
            }
            catch (...)
            {
                TraceLoggingWrite(
                    MidiSrvTelemetryProvider::Provider(),
                    __FUNCTION__,
                    TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                    TraceLoggingPointer(this, "this"),
                    TraceLoggingWideString(key.c_str(), "Invalid GUID Property")
                );
            }

            // TODO: Should verify the abstractionId is for an enabled abstraction
            // before adding it to the returned map

            abstractionSettings.insert_or_assign(abstractionId, transportJson);

            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_INFO),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(transportJson.c_str()),
                TraceLoggingGuid(abstractionId)
            );

            it.MoveNext();
        }

    }
    CATCH_LOG();

    return abstractionSettings;
}
#pragma pop_macro("GetObject")



HRESULT CMidiConfigurationManager::Initialize()
{
    TraceLoggingWrite(
        MidiSrvTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );

    // load the current configuration

    auto fileName = GetCurrentConfigurationFileName();

    //OutputDebugString(L"Config file name before expansion");
    //OutputDebugString((MIDI_CONFIG_FILE_FOLDER + fileName).c_str());

    // expanding this requires that the service is impersonating the current user.
    // WinRT doesn't support relative paths or unexpanded 
    fileName = ExpandPath(MIDI_CONFIG_FILE_FOLDER) + fileName;

    //OutputDebugString(L"Config file name after expansion");
    //OutputDebugString(fileName.c_str());

    if (!fileName.empty())
    {
        winrt::hstring fileContents{};

        try
        {
            // try to open the file
            auto file = winrt::Windows::Storage::StorageFile::GetFileFromPathAsync(fileName).get();

            // try to read the text from the file
            fileContents = winrt::Windows::Storage::FileIO::ReadTextAsync(file).get();
        }
        catch (...)
        {
            // file does not exist or we can't open it
            // we don't fail if no configuration file, we just don't config anything

            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(L"Missing or inaccessible MIDI configuration file")
            );

            return S_OK;
        }
        
        if (!fileContents.empty())
        {
            // parse out the JSON.
            // If the JSON is bad, we still just run. We just don't config.
            // Config is a privilege, not a right, and is certainly not essential :)

            try
            {
                if (!json::JsonObject::TryParse(fileContents, m_jsonObject))
                {
                    TraceLoggingWrite(
                        MidiSrvTelemetryProvider::Provider(),
                        __FUNCTION__,
                        TraceLoggingLevel(WINEVENT_LEVEL_ERROR),
                        TraceLoggingPointer(this, "this"),
                        TraceLoggingWideString(L"Configuration file JSON parsing failed")
                    );
                }
            }
            CATCH_LOG()
        }
        else
        {
            TraceLoggingWrite(
                MidiSrvTelemetryProvider::Provider(),
                __FUNCTION__,
                TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
                TraceLoggingPointer(this, "this"),
                TraceLoggingWideString(L"Configuration file JSON is empty")
            );
        }

    }
    else
    {
        TraceLoggingWrite(
            MidiSrvTelemetryProvider::Provider(),
            __FUNCTION__,
            TraceLoggingLevel(WINEVENT_LEVEL_WARNING),
            TraceLoggingPointer(this, "this"),
            TraceLoggingWideString(L"Missing MIDI configuration file")
        );

        return S_OK;
    }

    return S_OK;
}


_Use_decl_annotations_
std::wstring CMidiConfigurationManager::GetSavedConfigurationForTransportAbstraction(GUID abstractionGuid) const noexcept
{
    TraceLoggingWrite(
        MidiSrvTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingGuid(abstractionGuid)
    );


    try
    {

        auto key = internal::GuidToString(abstractionGuid);

        //    OutputDebugString(key.c_str());

        if (m_jsonObject != nullptr)
        {
            // probably need to normalize these to ignore case.
            if (m_jsonObject.HasKey(winrt::to_hstring(MIDI_CONFIG_JSON_TRANSPORT_PLUGIN_SETTINGS_OBJECT)))
            {
                auto plugins = m_jsonObject.GetNamedObject(MIDI_CONFIG_JSON_TRANSPORT_PLUGIN_SETTINGS_OBJECT);

                if (plugins.HasKey(key))
                {
                    auto thisPlugin = plugins.GetNamedObject(key);

                    std::wstring jsonString = (std::wstring)thisPlugin.Stringify();

                    //OutputDebugString(jsonString.c_str());

                    return jsonString;
                }
            }
        }
    }
    CATCH_LOG();

    return L"";
}



_Use_decl_annotations_
std::wstring CMidiConfigurationManager::GetSavedConfigurationForEndpointProcessingTransform(GUID abstractionGuid) const noexcept
{
    TraceLoggingWrite(
        MidiSrvTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this"),
        TraceLoggingGuid(abstractionGuid)
    );

    
    try
    {
        auto key = internal::GuidToString(abstractionGuid);

        if (m_jsonObject != nullptr)
        {
            // probably need to normalize these to ignore case. Not sure if WinRT Json dictionary is case-sensitive
            if (m_jsonObject.HasKey(winrt::to_hstring(MIDI_CONFIG_JSON_ENDPOINT_PROCESSING_PLUGIN_SETTINGS_OBJECT)))
            {
                auto plugins = m_jsonObject.GetNamedObject(MIDI_CONFIG_JSON_ENDPOINT_PROCESSING_PLUGIN_SETTINGS_OBJECT);

                if (plugins.HasKey(key))
                {
                    auto thisPlugin = plugins.GetNamedObject(key);

                    std::wstring jsonString = (std::wstring)thisPlugin.Stringify();

                    return jsonString;
                }
            }
        }
    }
    CATCH_LOG();

    return L"";
}



HRESULT CMidiConfigurationManager::Cleanup() noexcept
{
    TraceLoggingWrite(
        MidiSrvTelemetryProvider::Provider(),
        __FUNCTION__,
        TraceLoggingLevel(WINEVENT_LEVEL_INFO),
        TraceLoggingPointer(this, "this")
    );



    return S_OK;
}