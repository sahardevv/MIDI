// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License
// ============================================================================
// This is part of the Windows MIDI Services App API and should be used
// in your Windows application via an official binary distribution.
// Further information: https://github.com/microsoft/MIDI/
// ============================================================================

#pragma once

#define MIDIWORDNIBBLE1(x) ((uint8_t)((x & 0xF0000000) >> 28))
#define MIDIWORDNIBBLE2(x) ((uint8_t)((x & 0x0F000000) >> 24))
#define MIDIWORDNIBBLE3(x) ((uint8_t)((x & 0x00F00000) >> 20))
#define MIDIWORDNIBBLE4(x) ((uint8_t)((x & 0x000F0000) >> 16))


#define MIDIWORDBYTE1(x) ((uint8_t)((x & 0xFF000000) >> 24))
#define MIDIWORDBYTE2(x) ((uint8_t)((x & 0x00FF0000) >> 16))
#define MIDIWORDBYTE3(x) ((uint8_t)((x & 0x0000FF00) >> 8))
#define MIDIWORDBYTE4(x) ((uint8_t)((x & 0x000000FF)))



#define UMP32_WORD_COUNT 1
#define UMP64_WORD_COUNT 2
#define UMP96_WORD_COUNT 3
#define UMP128_WORD_COUNT 4


#define MIDI_MESSAGE_GROUP_WORD_CLEARING_MASK 0xF0FFFFFF
#define MIDI_MESSAGE_GROUP_WORD_DATA_MASK 0x0F000000
#define MIDI_MESSAGE_GROUP_BITSHIFT 24

#define MIDI_MESSAGE_CHANNEL_WORD_CLEARING_MASK 0xFFF0FFFF
#define MIDI_MESSAGE_CHANNEL_WORD_DATA_MASK 0x000F0000
#define MIDI_MESSAGE_CHANNEL_BITSHIFT 16



namespace Windows::Devices::Midi2::Internal
{
    inline std::uint8_t GetUmpLengthInMidiWordsFromMessageType(_In_ const std::uint8_t messageType) noexcept
    {
        switch (messageType & 0x0F)
        {
        case 0x0:
        case 0x1:
        case 0x2:
        case 0x6:
        case 0x7:
            return 1;

        case 0x3:
        case 0x4:
        case 0x8:
        case 0x9:
        case 0xA:
            return 2;

        case 0xB:
        case 0xC:
            return 3;

        case 0x5:
        case 0xD:
        case 0xE:
        case 0xF:
            return 4;

        default:
            return 0;   // with the mask, shouldn't happen. Here only due to the compiler warning
        };

    }
    


    inline void SetUmpMessageType(_In_ std::uint32_t& firstWord, _In_ const uint8_t messageType) noexcept
    {
        // first four bits of the word is the message type

        uint32_t t = ((uint32_t)(messageType & 0x0F) << 28);
        
        firstWord = (firstWord & 0x0FFFFFFF) | t;
    }

    inline std::uint8_t GetUmpMessageTypeFromFirstWord(_In_ const std::uint32_t firstWord) noexcept
    {
        return (uint8_t)(MIDIWORDNIBBLE1(firstWord));
    }

    inline std::uint8_t GetUmpLengthInMidiWordsFromFirstWord(_In_ const std::uint32_t firstWord) noexcept
    {
        return GetUmpLengthInMidiWordsFromMessageType(GetUmpMessageTypeFromFirstWord(firstWord));
    }


    inline std::uint8_t GetUmpLengthInBytesFromFirstWord(_In_ const std::uint32_t firstWord) noexcept
    {
        return (uint8_t)(GetUmpLengthInMidiWordsFromFirstWord(firstWord) * sizeof(uint32_t));
    }


    inline std::uint8_t GetGroupIndexFromFirstWord(_In_ const std::uint32_t firstWord) noexcept
    {
        return (uint8_t)((firstWord & MIDI_MESSAGE_GROUP_WORD_DATA_MASK) >> MIDI_MESSAGE_GROUP_BITSHIFT);
    }


    // assumes the caller has already checked to see if the message type has a group field. Otherwise, this will stomp on their data
    inline std::uint32_t GetFirstWordWithNewGroup(_In_ std::uint32_t const firstWord, _In_ std::uint8_t const groupIndex) noexcept
    {
        if (groupIndex > 15)
            return firstWord;

        // mask out the group
        uint32_t newWord = firstWord & MIDI_MESSAGE_GROUP_WORD_CLEARING_MASK;

        // move our group over to the right position
        uint32_t groupValue = ((uint32_t)groupIndex) << MIDI_MESSAGE_GROUP_BITSHIFT;

        // return the masked word with the new group value
        return newWord | groupValue;
    }

    // NOTE: This check needs to change when new message types are added. The WinRT enum will need to change as well.

    inline bool MessageTypeHasGroupField(_In_ std::uint8_t messageType) noexcept
    {
        switch (messageType)
        {
        case 0x0: // MidiUmpMessageType::UtilityMessage32  // type 0
            return false;

        case 0x1: // MidiUmpMessageType::SystemCommon32    // type 1
            return true;
        case 0x2: // MidiUmpMessageType::Midi1ChannelVoice32   // type 2
            return true;
        case 0x3: // MidiUmpMessageType::DataMessage64   // type 3
            return true;
        case 0x4: // MidiUmpMessageType::Midi2ChannelVoice64   // type 4
            return true;
        case 0x5: // MidiUmpMessageType::DataMessage128   // type 5
            return true;
        case 0xD: // MidiUmpMessageType::FlexData128       // type D
            return true;

        case 0xF: // MidiUmpMessageType::UmpStream128      // type F
            return false;


            // all other message types are undefined, so return false until those are defined
        default:
            return false;

        }
    }

    inline std::uint8_t GetChannelIndexFromFirstWord(_In_ const std::uint32_t firstWord) noexcept
    {
        return (uint8_t)((firstWord & MIDI_MESSAGE_CHANNEL_WORD_DATA_MASK) >> MIDI_MESSAGE_CHANNEL_BITSHIFT);
    }


    // assumes the caller has already checked to see if the message type has a group field. Otherwise, this will stomp on their data
    inline std::uint32_t GetFirstWordWithNewChannel(_In_ std::uint32_t const firstWord, _In_ std::uint8_t const channelIndex) noexcept
    {
        if (channelIndex > 15)
            return firstWord;

        // mask out the channel
        uint32_t newWord = firstWord & MIDI_MESSAGE_CHANNEL_WORD_CLEARING_MASK;

        // move our group over to the right position. Channel is the lower nibble of the second byte
        uint32_t channelValue = ((uint32_t)channelIndex) << MIDI_MESSAGE_CHANNEL_BITSHIFT;

        // return the masked word with the new group value
        return newWord | channelValue;
    }

    inline bool MessageTypeHasChannelField(_In_ std::uint8_t messageType) noexcept
    {
        switch (messageType)
        {
        case 0x0: // MidiUmpMessageType::UtilityMessage32  // type 0
            return false;

        case 0x1: // MidiUmpMessageType::SystemCommon32    // type 1
            return true;
        case 0x2: // MidiUmpMessageType::Midi1ChannelVoice32   // type 2
            return true;
        case 0x3: // MidiUmpMessageType::DataMessage64   // type 3 - sysex. Sysex doesn't include channel but does have a group
            return false;
        case 0x4: // MidiUmpMessageType::Midi2ChannelVoice64   // type 4
            return true;
        case 0x5: // MidiUmpMessageType::DataMessage128   // type 5 - sysex. Sysex doesn't include channel but does have a group
            return false;
        case 0xD: // MidiUmpMessageType::FlexData128       // type D
            return true;

        case 0xF: // MidiUmpMessageType::UmpStream128      // type F
            return false;

            // all other message types are undefined, so return false until those are defined
        default:
            return false;

        }
    }


    inline uint32_t CleanupInt20(_In_ uint32_t const value) noexcept
    {
        return value & (uint32_t)0x000FFFFF;
    }


    inline uint16_t CleanupInt14(_In_ uint16_t const value) noexcept
    {
        return value & (uint16_t)0x3FFF;
    }

    inline uint16_t CleanupInt10(_In_ uint16_t const value) noexcept
    {
        return value & (uint16_t)0x03FF;
    }

    // 7 bit byte
    inline uint8_t CleanupByte7(_In_ uint8_t const value) noexcept
    {
        return value & (uint8_t)0x7F;
    }

    // 4-bit value
    inline uint8_t CleanupNibble(_In_ uint8_t const value) noexcept
    {
        return value & (uint8_t)0x0F;
    }

    // 2-bit value
    inline uint8_t CleanupCrumb(_In_ uint8_t const value) noexcept
    {
        return value & (uint8_t)0x03;
    }

    inline void SetGroupIndexInFirstWord(_Inout_ std::uint32_t firstWord, _In_ std::uint8_t groupIndex) noexcept
    {
        firstWord &= MIDI_MESSAGE_GROUP_WORD_CLEARING_MASK;
        firstWord |= CleanupNibble(groupIndex) << MIDI_MESSAGE_GROUP_BITSHIFT;
    }

    // in order from msb to lsb. Avoids endian issues
    inline uint32_t MidiWordFromBytes(
        _In_ uint8_t const byte0,
        _In_ uint8_t const byte1,
        _In_ uint8_t const byte2,
        _In_ uint8_t const byte3
        ) noexcept
    {
        return (uint32_t)(byte0 << 24 | byte1 << 16 | byte2 << 8 | byte3);
    }


    inline uint8_t GetFormFromStreamMessageFirstWord(
        _In_ uint32_t const word0
        ) noexcept
    {
        return (uint8_t)((word0 & 0x0C000000) >> 26);
    }

    inline uint16_t GetStatusFromStreamMessageFirstWord(
        _In_ uint32_t const word0
        ) noexcept
    {
        return (uint16_t)((word0 & 0x03FF0000) >> 16);
    }




    inline bool GetFunctionBlockActiveFlagFromInfoNotificationFirstWord(
        _In_ uint32_t const word0
        ) noexcept
    {
        // high bit on the second 16 bit half of the first word
        return (bool)((word0 & 0x00008000) > 0);
    }

    inline uint8_t GetFunctionBlockNumberFromInfoNotificationFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)((word0 & 0x00007F00) >> 8);
    }

    inline uint8_t GetFunctionBlockUIHintFromInfoNotificationFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)((word0 & 0x00000030) >> 4);
    }

    inline uint8_t GetFunctionBlockMidi10FromInfoNotificationFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)((word0 & 0x0000000C) >> 2);
    }

    inline uint8_t GetFunctionBlockDirectionFromInfoNotificationFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)(word0 & 0x00000003);
    }



  

    inline uint8_t GetFunctionBlockFirstGroupFromInfoNotificationSecondWord(
        _In_ uint32_t const word1
        ) noexcept
    {
        return (uint8_t)((word1 & 0xFF000000) >> 24);
    }

    inline uint8_t GetFunctionBlockNumberOfGroupsFromInfoNotificationSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (uint8_t)((word1 & 0x00FF0000) >> 16);
    }

    inline uint8_t GetFunctionBlockMidiCIVersionFromInfoNotificationSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (uint8_t)((word1 & 0x0000FF00) >> 8);
    }

    inline uint8_t GetFunctionBlockMaxSysex8StreamsFromInfoNotificationSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (uint8_t)(word1 & 0x000000FF);
    }



    inline uint8_t GetEndpointInfoNotificationUmpVersionMajorFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)((word0 & 0x0000FF00) >> 8);
    }

    inline uint8_t GetEndpointInfoNotificationUmpVersionMinorFirstWord(
        _In_ uint32_t const word0
    ) noexcept
    {
        return (uint8_t)(word0 & 0x000000FF);
    }

    inline bool GetEndpointInfoNotificationStaticFunctionBlocksFlagFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        // highest bit is this flag
        return (bool)((word1 & 0x80000000) > 0);
    }

    inline uint8_t GetEndpointInfoNotificationNumberOfFunctionBlocksFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (uint8_t)((word1 & 0x7F000000) >> 24);
    }

    inline bool GetEndpointInfoNotificationMidi2ProtocolCapabilityFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (bool)((word1 & 0x00000200) > 0);
    }

    inline bool GetEndpointInfoNotificationMidi1ProtocolCapabilityFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (bool)((word1 & 0x00000100) > 0);
    }

    inline bool GetEndpointInfoNotificationReceiveJRTimestampCapabilityFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (bool)((word1 & 0x00000002) > 0);
    }

    inline bool GetEndpointInfoNotificationTransmitJRTimestampCapabilityFromSecondWord(
        _In_ uint32_t const word1
    ) noexcept
    {
        return (bool)((word1 & 0x00000001) > 0);
    }



    inline uint8_t GetStatusFromUmp32FirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE3(word0);
    }

    // MIDI 1 and MIDI 2
    inline uint8_t GetStatusFromChannelVoiceMessage(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE3(word0);
    }

    


    inline uint8_t GetStatusBankFromFlexDataMessageFirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDBYTE3(word0);
    }

    inline uint8_t GetStatusFromFlexDataMessageFirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDBYTE4(word0);
    }


    inline uint8_t GetStatusFromSystemCommonMessage(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDBYTE2(word0);
    }

    inline uint8_t GetStatusFromDataMessage64FirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE3(word0);
    }

    inline uint8_t GetNumberOfBytesFromDataMessage64FirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE4(word0);
    }

    inline uint8_t GetStatusFromDataMessage128FirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE3(word0);
    }

    inline uint8_t GetNumberOfBytesFromDataMessage128FirstWord(_In_ uint32_t const word0) noexcept
    {
        return MIDIWORDNIBBLE4(word0);
    }


    inline bool IsSysEx7StartMessage(_In_ uint32_t const word0) noexcept
    {
        UNREFERENCED_PARAMETER(word0);

        // TODO
        return false;
    }

    inline bool IsSysEx7ContinueMessage(_In_ uint32_t const word0) noexcept
    {
        UNREFERENCED_PARAMETER(word0);

        // TODO
        return false;
    }

    inline bool IsSysEx7CompleteMessage(_In_ uint32_t const word0) noexcept
    {
        UNREFERENCED_PARAMETER(word0);

        // TODO
        return false;
    }

    inline bool IsSysEx7SelfContainedMessage(_In_ uint32_t const word0) noexcept
    {
        UNREFERENCED_PARAMETER(word0);

        // TODO
        return false;
    }


    inline bool IsSysEx8Message(_In_ uint32_t const word0) noexcept
    {
        UNREFERENCED_PARAMETER(word0);

        // TODO
        return false;
    }

    // To preserve robust connection to all MIDI devices and systems, Senders shall obey the following data rules of the
    // MIDI 1.0 Protocol that govern interspersing other messages and termination of System Exclusive within a Group :
    // � The Sender shall not send any other Message or UMP on the same Group between the Start and End of the
    //   System Exclusive Message, except for System Exclusive Continue UMPs, and System Real Time Messages.
    // � System Real Time Messages on the same Group may be inserted between the UMPs of a System Exclusive
    //   message, in order to maintain timing synchronization.
    // � If any Message or UMP on the same Group, other than a System Exclusive Continue UMP or a System Real
    //   Time Message, is sent after a System Exclusive Start UMP and before the associated System Exclusive End
    //   UMP, then that UMP shall terminate the System Exclusive Message.
    // Messages which are Groupless(MT = 0x0 and 0xF) and those which are sent to a different Group may be
    // interspersed with System Exclusive Messages.
    //
    // In a multi-client situation, however, we wouldn't want the UMP from Client A to terminate sysex from Client B.

    inline bool IsMessageOkToSendDuringSysEx7(_In_ uint8_t activeSysExGroupIndex, _In_ uint32_t const word0) noexcept
    {
        uint8_t mt = GetUmpMessageTypeFromFirstWord(word0);

        if (mt == 0x0 || mt == 0xF)
        {
            // stream messages
            return true;
        }

        // messages to other groups in same stream are allowed
        if (GetGroupIndexFromFirstWord(word0) != activeSysExGroupIndex)
        {
            return true;
        }

        // while in sysex7 for this current group, you can end or continue sysex7
        if (IsSysEx7ContinueMessage(word0) || IsSysEx7CompleteMessage(word0))
        {
            return true;
        }


        return false;
    }





}