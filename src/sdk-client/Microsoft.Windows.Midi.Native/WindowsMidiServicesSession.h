// ------------------------------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the GitHub project root for license information.
// ------------------------------------------------------------------------------------------------

// ====================================================================
// PRE-RELEASE VERSION. BREAKING CHANGES LIKELY. NOT FOR PRODUCTION USE.
// For more information, please see https://github.com/microsoft/midi
// ====================================================================

#pragma once

#include <Windows.h>
#include <winnt.h>		// for flags operator macro

#ifdef WINDOWSMIDISERVICES_EXPORTS
#define WINDOWSMIDISERVICES_API __declspec(dllexport)
#else
#define WINDOWSMIDISERVICES_API __declspec(dllimport)
#endif

#include "WindowsMidiServicesUmp.h"
#include "WindowsMidiServicesEnumeration.h"


// TODO: May need to differentiate this namespace from the WinRT namespace
namespace Microsoft::Windows::Midi //::inline v0_1_0_pre
{
	// ----------------------------------------------------------------------------
	// MIDI Message received callback / delegate
	// ----------------------------------------------------------------------------

	class WINDOWSMIDISERVICES_API MidiMessageReader
	{
	private:
		struct implMidiMessageReader;
		implMidiMessageReader* _pimpl;

		MidiMessageReader(const MidiMessageReader& info) { _pimpl = nullptr; }	// don't copy
		MidiMessageReader();
	public:
		~MidiMessageReader();

		bool eof();
		int GetNextUmpWordCount();			// get the word count for the next message in the queue

		// reads a message from the queue. If valdate == true, will peek first to validate
		// that the next message really is a Ump32, and fail without a read if the message
		// type doesn't match the method used.
		bool ReadUmp32(Messages::Ump32& message, bool validate = true);
		bool ReadUmp64(Messages::Ump64& message, bool validate = true);
		bool ReadUmp96(Messages::Ump96& message, bool validate = true);
		bool ReadUmp128(Messages::Ump128& message, bool validate = true);

		// requires that a buffer large enough for 4 32-bit words is supplied
		// returns the number of words actually written to the buffer. The results
		// can be used to initialize a UMP. Returns 0 on failure, including an
		// incomplete next message in the queue.
		uint8_t ReadNextSingleUmp(const MidiWord32* buffer);

		friend class MidiStream;
	};

	typedef WINDOWSMIDISERVICES_API void(*MidiMessagesReceivedCallback)(
		const MidiObjectId& sessionId,
		const MidiObjectId& deviceId,
		const MidiObjectId& streamId,
		const MidiMessageReader& reader);




	// ----------------------------------------------------------------------------
	// Devices and Streams (TODO: GO back to Devices and Endpoints)
	// ----------------------
	// Not to be confused with StreamInformation and DeviceInformation, these
	// are the actual interfaces to the devices and streams themselves.
	// ----------------------------------------------------------------------------


	// TODO: May need to prepend a send/receive timestamp to incoming and outgoing
	// messages, adding to the overall size. This is not MIDI protocol, but is
	// internal implementation. second or third most important request from
	// app devs is service-side scheduling of message sending. Maybe include this
	// in the options when opening so we can configure the message queue to include
	// the timestamps

	enum WINDOWSMIDISERVICES_API MidiStreamOpenOptions
	{
		MidiStreamOpenOutput = 1,				// MIDI Out
		MidiStreamOpenInput = 2,				// MIDI In, will need to wire up message received handler
		MidiStreamOpenBidirectional = 3,		// Both. Will need to wire up message received handler

		MidiStreamOpenExclusive = 4,			// TBD if we also include Exclusive here.

		MidiStreamOpenUseSendTimestamps = 8
	};
	DEFINE_ENUM_FLAG_OPERATORS(MidiStreamOpenOptions)

	enum WINDOWSMIDISERVICES_API MidiStreamOpenResultErrorDetail
	{
		MidiStreamOpenResultErrorNone = 0,

		MidiStreamOpenErrorAlreadyOpenInSession,
		MidiStreamOpenErrorExclusiveElsewhere,
		MidiStreamOpenErrorServiceTimeout,

		MidiStreamOpenErrorOther
	};

	// timestamp for sending scheduled messages
	struct WINDOWSMIDISERVICES_API MidiMessageTimestamp
	{
		uint64_t Timestamp;							// when to send this message

		const uint64_t Immediate = 0;				// constant for any message that should be sent now
		static const uint64_t getCurrent();				// system call to get the current counter value
		static const uint64_t getFrequency();				// frequency of counts in khz. Won't change until reboot.
	};

	// Info on hardware timers and QPC in Windows: 
	// https://docs.microsoft.com/en-us/windows/win32/sysinfo/acquiring-high-resolution-time-stamps#hardware-timer-info


	// TODO: I strongly dislike the word "Stream", but there was ambiguout around "Endpoint". However
	// this needs to be looked at a second time due to how group/function blocks should be represented
	// as Endpoints themselves, based on the latest info (Also see Andrew Mee's whitepaper on MIDI 2.0
	// API design for some more information)
	class WINDOWSMIDISERVICES_API MidiStream
	{
	protected:
		struct implMidiStream;
		implMidiStream* _pimpl;

		//MidiStream(const MidiStream& info);				// don't copy
		MidiStream(MidiObjectId streamId, MidiObjectId parentDeviceId, MidiStreamOpenOptions options);
	public:
		~MidiStream();
		void Close();

		const MidiObjectId getStreamInformationId();
		const MidiObjectId getParentDeviceInformationId();

		// send a UMP with no scheduling. 
		bool SendUmp(const Messages::Ump& message);

		// send a UMP with scheduling. Only works if the stream was created with that option
		bool SendUmp(MidiMessageTimestamp sendTimestamp, const Messages::Ump& message);


		friend class MidiDevice;	// Want to construct streams only through device class

		// TODO: MIDI CI Profile Support
		// TODO: MIDI CI Property Exchange Support
	};


	struct WINDOWSMIDISERVICES_API MidiStreamOpenResult
	{
		bool Success = false;
		MidiStreamOpenResultErrorDetail ErrorDetail = MidiStreamOpenResultErrorDetail::MidiStreamOpenResultErrorNone;
		MidiStream* StreamReference;						// pointer to stream in the internal collection
	};


	struct WINDOWSMIDISERVICES_API MidiDeviceOpenOptions
	{
		// TODO
	};

	class WINDOWSMIDISERVICES_API MidiDevice final
	{
	private:
		struct implMidiDevice;
		implMidiDevice* _pimpl;

		MidiDevice(const MidiDevice& info);	// don't copy
		MidiDevice(MidiObjectId deviceId, MidiObjectId& parentSessionId);
	public:
		~MidiDevice();

		const MidiObjectId getDeviceInformationId();

		const bool getOpenedStream(const MidiObjectId& streamId, MidiStream& stream);
		// todo: method to get all open streams. Or maybe just a custom collection type

		// session sets these when you open the device
		const MidiObjectId getParentSessionID();

		MidiStreamOpenResult OpenStream(const MidiObjectId& streamId, const MidiStreamOpenOptions options, const MidiMessagesReceivedCallback& messagesReceivedCallback);
		MidiStreamOpenResult OpenStream(const MidiObjectId& streamId, const MidiStreamOpenOptions options);

		// close this device connection and any open stream connections
		void Close();

		friend class MidiSession;	// TBD if this is necessary. Want to construct devices only through session class
	};

	enum WINDOWSMIDISERVICES_API MidiDeviceOpenResultErrorDetail
	{
		MidiDeviceOpenErrorNone = 0,

		MidiDeviceOpenErrorAlreadyOpenInSession,
		MidiDeviceOpenErrorServiceTimeout,

		MidiDeviceOpenErrorOther
	};

	struct WINDOWSMIDISERVICES_API MidiDeviceOpenResult
	{
		bool Success;
		MidiDeviceOpenResultErrorDetail ErrorDetail = MidiDeviceOpenResultErrorDetail::MidiDeviceOpenErrorNone;	// Additional error information
		MidiDevice* DeviceReference;					// pointer to device in the internal collection
	};


	// ----------------------------------------------------------------------------
	// Session
	// ----------------------------------------------------------------------------

	struct WINDOWSMIDISERVICES_API MidiSessionCreateOptions
	{

	};

	enum WINDOWSMIDISERVICES_API MidiSessionCreateResultErrorDetail
	{
		MidiSessionCreateErrorNone = 0,
		MidiSessionCreateErrorOther
	};

	struct WINDOWSMIDISERVICES_API MidiSessionCreateResult
	{
		bool Success = false;
		MidiSessionCreateResultErrorDetail ErrorDetail = MidiSessionCreateResultErrorDetail::MidiSessionCreateErrorNone;
		MidiSession* Session = nullptr;
	};

	class WINDOWSMIDISERVICES_API MidiSession final
	{
	private:
		struct implMidiSession;
		implMidiSession* _pimpl;

		MidiSession();								// use the Create method to construct objects
		MidiSession(const MidiSession& session);	// not copyable
	public:
		~MidiSession();

		const MidiObjectId getId();
		const SYSTEMTIME getCreatedDateTime();

		const wchar_t* getName();
		void UpdateName(const wchar_t* newName);		// this makes a server call


		// TODO: may want to consider changing this to similar pattern as others, with a 
		// result class and an instance of the new session.
		static MidiSessionCreateResult Create(const wchar_t* name, const wchar_t* appName, const MidiSessionCreateOptions& options);
		static MidiSessionCreateResult Create(const wchar_t* name, const wchar_t* appName);
		void Close();

		const MidiDevice* GetOpenedDevice(const MidiObjectId id);

		MidiDeviceOpenResult OpenDevice(const MidiObjectId& deviceId, const MidiDeviceOpenOptions& options);
		MidiDeviceOpenResult OpenDevice(const MidiObjectId& deviceId);

		// TODO: enumerator for open devices/streams
		//const std::vector<MidiDevice> getAllOpenedDevices();


	};
}