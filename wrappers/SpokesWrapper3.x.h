#pragma once
#include "atlbase.h"
#include "atlcom.h"

#import "mscorlib.tlb" raw_interfaces_only, rename("ReportEvent", "MSReportEvent")

// Spokes 3.0 COM SDK is distributed as tlb file. 
// C++ user can use #import directive that will create all proper C++ types, wrappers and interfaces for communicating with running Spokes3G.exe COM server
#import "Plantronics.tlb" no_namespace, named_guids, raw_interfaces_only

#if defined _MSC_VER
#define __func__ __FUNCTION__
// until such a time that MSC++ gets standards-compliant noexcept
#define noexcept
#endif

#include <string>
#include <iostream>

using namespace std;

/*******
 * 
 * SpokesWrapper.cpp/.h
 * 
 * SpokesWrapper.cpp/.h is a wrapper around the Plantronics Spokes COM Service API for C++.
 * 
 * It's purpose is to make it easier and simpler to integrate support for Plantronics devices into any applications.
 * 
 * It achieves this by hiding a lot of the more tricky aspects of integration behind the wrapper and presenting
 * a simple and consistent set of Event Handlers and functions for the core features of the SDK that the user
 * will typically be needing.
 * 
 * *** WARNING !!! This source code is provided *As Is*! Is is intented as a sample code to show ways of integrating
 * with the Spokes "COM Service API". However in case of problems please feel free to contact Lewis Collins
 * directly via the PDC site at this address: http://developer.plantronics.com/people/lcollins/ ***
 * 
 * The latest version of this file will also be maintained (and feel free to create your own Fork!) on Github, here:
 * 
 * https://github.com/pltdev/Samples/tree/master/wrappers
 * 
 * Read more about Plantronics Spokes at the Plantronics Developer Connection web site:
 * 
 * http://developer.plantronics.com/community/devzone/
 * 
 * Lewis Collins, http://developer.plantronics.com/people/lcollins
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.5.29:
 * Date: 12th Dec 2014
 * Compatible with Spokes SDK version(s): 3.3.50873.10888 (pre-release)
 * Changed by: Lewis Collins
 *   Changes:
 *     - Initial version of C++ wrapper for Plantronics SDK v3.x, matches 2.x C++ Wrapper version 1.0.8
 *       Also has near feature parity with C# Wrapper 1.5.29, so adopting same version number
 *     - NOTE: define Preprocessor Definition: newDASeries if you want to use the new DA Series 
 *       Quickdisconnect QD events (Connected/Disconnected)
 *
 * Version 1.0.8:
 * Date: 29th Oct 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added new IsSpokesInstalled convenience function. You can optionally call this before
 *       calling the Spokes Wrapper Connect function to check if Spokes is installed.
 *
 * Version 1.0.7:
 * Date: 17th Sept 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added knowledge of the Plantronics device capabilities through
 *       deployment of supplementary file: "DeviceCapabilities.csv"
 *       This file should be placed in the working directory of the calling
 *       application.
 *
 * Version 1.0.6:
 * Date: 12th Sept 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes: (thanks Olivier for the feedback)
 *     - Added special case to identify devices C220 and C210 to correct the device capabilities
 *       (see UpdateOtherDeviceCapabilities function).
 *     - Added HoldCall and ResumeCall functions so softphone can tell Spokes when these actions
 *       have occured in the softphone
 *     - Added placeholder code for system suspend/resume events (not currently exposed through Spokes COM)
 *
 * Version 1.0.5:
 * Date: 25th June 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes: (thanks Harel for the feedback)
 *     - Removing MFC dependency, changing CString's for std::strings
 *     - Moved DebugPrint, NotifyEvent and GetInstance members of SpokesWrapper from .h to .cpp
 *     - Adding Hungarian Notation to the object class member names for consistency
 *	   - Removing all 'cout' calls in wrapper, replace with DebugPrint facility for user's app
 *       to optionally process if required
 *
 * Version 1.0.4:
 * Date: 24th May 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding battery level change events and GetBatteryLevel() function
 *       so apps can know the battery level of attached headset.
 *
 * Version 1.0.3:
 * Date: 5th April 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Has been updated with Call Control, Mute Sync, Line Active Sync
 *     - Don/Doff, Near/Far may work but not tested
 *     - Other advanced features are INCOMPLETE/MISSING
 *
 * Version 1.0.2:
 * Date: 25th January 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Initial version of C++ wrapper, matches C# version 1.0.2
 *
 * ********************************************************************************
 * 
 **/

/// <summary>
/// Interface to allow your application's class to handle log debug tracing from the SpokesWrapper...
/// </summary>
class IDebugLogger
{
public:
    virtual void DebugPrint(string methodname, string str) = 0;
};

/// <summary>
/// Class structure to hold info on Plantronics device capabilities
/// </summary>
class SpokesDeviceCaps
{
public:
    bool m_bHasProximity;
    bool m_bHasMobCallerId;
	bool m_bHasMobCallState;
    bool m_bHasDocking;
    bool m_bHasWearingSensor;
    bool m_bHasMultiline;
    bool m_bIsWireless;
	string m_strProductId;

	// default constructor
	SpokesDeviceCaps() { };

    /// <summary>
    /// Constructor: pass in boolean values for whether it has the given device capabilities or not
    /// </summary>
    SpokesDeviceCaps(bool hasProximity, bool hasCallerId, bool hasDocking, bool hasWearingSensor, bool hasMultiline, bool isWireless)
    {
        Init(hasProximity, hasCallerId, hasDocking, hasWearingSensor, hasMultiline, isWireless);
    }

    void Init(bool hasProximity, bool hasCallerId, bool hasDocking, bool hasWearingSensor, bool hasMultiline, bool isWireless)
    {
        m_bHasProximity = hasProximity;
        m_bHasMobCallerId = hasCallerId;
        m_bHasDocking = hasDocking;
        m_bHasWearingSensor = hasWearingSensor;
        m_bHasMultiline = hasMultiline;
        m_bIsWireless = isWireless;
    }

    /// <summary>
    /// Returns a nice string representation of device capabilites, e.g. for use in logs
    /// </summary>
    string ToString();
};

/// <summary>
/// Base class used to pass arguments to Spokes event interface ISpokesEvents
/// </summary>
class EventArgs
{
	public:
		// Summary:
		//     Represents an event with no event data.
		static EventArgs * Empty() { EventArgs * e = new EventArgs(); return e; };

		// Summary:
		//     Initializes a new instance of the System.EventArgs class.
		EventArgs() { };
};

/// <summary>
/// Event args for Mute Changed event handler
/// </summary>
class MuteChangedArgs : public EventArgs
{
public:
    bool m_bMuteOn;

	MuteChangedArgs()
	{
		m_bMuteOn = false;
	}

    MuteChangedArgs(bool isMuteOn)
    {
		Init(isMuteOn);
    }

	void Init(bool isMuteOn)
	{
        m_bMuteOn = isMuteOn;
	}
};

/// <summary>
/// Event args for Mute Changed event handler
/// </summary>
class ButtonPressArgs : public EventArgs
{
public:
    DeviceHeadsetButton m_headsetButton;
    DeviceAudioState m_audioType;
    bool m_mute;

	ButtonPressArgs()
	{
		m_mute = false;
	}

	ButtonPressArgs(DeviceHeadsetButton headsetButton, DeviceAudioState audioType, bool aMute)
    {
		Init(headsetButton, audioType, aMute);
    }

	void Init(DeviceHeadsetButton headsetButton, DeviceAudioState audioType, bool aMute)
	{
		m_headsetButton = headsetButton;
		m_audioType = m_audioType;
        m_mute = aMute;
	}
};

/// <summary>
/// EventArgs used with BaseButtonPress event handler to receive details of which button
/// was pressed
/// </summary>
class BaseButtonPressArgs : public EventArgs
{
public:
	DeviceBaseButton m_baseButton;

    BaseButtonPressArgs()
    {
    }

	BaseButtonPressArgs(DeviceBaseButton baseButton)
    {
        Init(baseButton);
    }

	void Init(DeviceBaseButton baseButton)
	{
		m_baseButton = baseButton;
	}
};

/// <summary>
/// EventArgs used with CallRequested event handler to receive details of the
/// number requested to dial from dialpad device (Calisto P240/800 series)
/// </summary>
class CallRequestedArgs : public EventArgs
{
public:
	ICOMContact * m_contact;
	string m_contactName;

    CallRequestedArgs()
    {
        m_contact = nullptr;
		m_contactName = "";
    }

	CallRequestedArgs(ICOMContact * contact, string contactName)
    {
        Init(contact, contactName);
    }

	void Init(ICOMContact * contact, string contactName)
    {
        m_contact = contact;
		m_contactName = contactName;
    }
};

/// <summary>
/// Event args for Mute Changed event handler
/// </summary>
class LineActiveChangedArgs : public EventArgs
{
public:
    bool m_bLineActive;

	LineActiveChangedArgs()
	{
		m_bLineActive = false;
	}

    LineActiveChangedArgs(bool isLineActive)
    {
		m_bLineActive = isLineActive;
    }

	void Init(bool isLineActive)
	{
        m_bLineActive = isLineActive;
	}
};

/// <summary>
/// Event args for Attached (device attached) event handler
/// </summary>
class AttachedArgs : public EventArgs
{
public:
    string m_strDeviceName;

	AttachedArgs()
	{
		m_strDeviceName = "";
	}

    AttachedArgs(string aDevice)
    {
		Init(aDevice);
    }

	void Init(string aDevice)
	{
        m_strDeviceName = aDevice;
	}
};

/// <summary>
/// Enumeration of call states
/// </summary>
enum SpokesCallState
{
    SpokesCallState_Ringing,
    SpokesCallState_OnCall,
    SpokesCallState_Idle,
	SpokesCallState_Held
};

/// <summary>
/// Event args for OnCall event handler
/// </summary>
class OnCallArgs : public EventArgs
{
public:
    string m_strCallSource;
    bool m_bIncoming;
    SpokesCallState m_State;

	OnCallArgs()
	{
		m_strCallSource = "";
		m_bIncoming = false;
		m_State = SpokesCallState_Idle;
	}

    OnCallArgs(string source, bool isIncoming, SpokesCallState state)
    {
		Init(source, isIncoming, state);
    }

	void Init(string source, bool isIncoming, SpokesCallState state)
    {
        m_strCallSource = source;
        m_bIncoming = isIncoming;
        m_State = state;
    }
};

/// <summary>
/// Enumeration of mobile call states
/// </summary>
enum SpokesMobileCallState
{
    SpokesMobileCallState_Ringing,
    SpokesMobileCallState_OnCall,
    SpokesMobileCallState_Idle
};

/// <summary>
/// Event args for OnMobileCall event handler
/// </summary>
class OnMobileCallArgs : public EventArgs
{
public:
    bool m_bIncoming;
    SpokesMobileCallState m_State;

	OnMobileCallArgs()
	{
		m_bIncoming = false;
		m_State = SpokesMobileCallState_Idle;
	}

    OnMobileCallArgs(bool isIncoming, SpokesMobileCallState state)
    {
		Init(isIncoming, state);
    }

	void Init(bool isIncoming, SpokesMobileCallState state)
	{        
		m_bIncoming = isIncoming;
        	m_State = state;
	}
};

/// <summary>
/// Event args for MobileCallerId event handler
/// </summary>
class MobileCallerIdArgs : public EventArgs
{
public:
    string m_strMobileCallerId;

	MobileCallerIdArgs()
	{
		m_strMobileCallerId = "";
	}

    MobileCallerIdArgs(string mobilecallerid)
    {
		Init(mobilecallerid);
    }

	void Init(string mobilecallerid)
	{
        m_strMobileCallerId = mobilecallerid;
	}
};

/// <summary>
/// Enumeration of serial numbers in a Plantronics device (i.e. Headset and base/usb adaptor)
/// </summary>
enum SpokesSerialNumberTypes
{
    Spokes_Headset,
    Spokes_Base
};

/// <summary>
/// Event args for SerialNumber event handler
/// </summary>
class SerialNumberArgs : public EventArgs
{
public:
    string m_strSerialNumber;
    SpokesSerialNumberTypes m_SerialNumberType;

	SerialNumberArgs()
	{
		m_strSerialNumber = "";
		m_SerialNumberType = Spokes_Base;
	}

    SerialNumberArgs(string serialnumber, SpokesSerialNumberTypes serialnumtype)
    {
		Init(serialnumber, serialnumtype);
    }

	void Init(string serialnumber, SpokesSerialNumberTypes serialnumtype)
	{
        m_strSerialNumber = serialnumber;
        m_SerialNumberType = serialnumtype;
	}
};


/// <summary>
/// Event args for CallAnswered event handler
/// </summary>
class CallAnsweredArgs : public EventArgs
{
public:
    int m_iCallId;
    string m_strCallSource;

	CallAnsweredArgs()
	{
		m_iCallId = -1;
		m_strCallSource = "";
	}

    CallAnsweredArgs(int callid, string callsource)
    {
		Init(callid, callsource);
    }

	void Init(int callid, string callsource)
	{
		m_iCallId = callid;
        m_strCallSource = callsource;
	}
};

/// <summary>
/// Event args for CallEnded event handler
/// </summary>
class CallEndedArgs : public EventArgs
{
	public:
    int m_iCallId;
    string m_strCallSource;

	CallEndedArgs()
	{
		m_iCallId = -1;
		m_strCallSource = "";
	}

    CallEndedArgs(int callid, string callsource)
    {
		Init(callid, callsource);
    }

	void Init(int callid, string callsource)
	{
		m_iCallId = callid;
        m_strCallSource = callsource;
	}
};

/// <summary>
/// Used with MultiLineStateArgs to hold active/held status of multiple lines (PC, Mobile, Deskphone)
/// </summary>
struct SpokesMultiLineStateFlags
{
public:
    bool m_bPCActive;
    bool m_bMobileActive;
    bool m_bDeskphoneActive;
    bool m_bPCHeld;
    bool m_bMobileHeld;
    bool m_bDeskphoneHeld;
};

/// <summary>
/// EventArgs used with MultiLineStateChanged event handler to receive status of multiple lines (PC, Mobile, Deskphone) 
/// when the state of any of these lines changes.
/// </summary>
class MultiLineStateArgs : public EventArgs
{
public:
	SpokesMultiLineStateFlags m_MultiLineState;

	MultiLineStateArgs()
	{
		m_MultiLineState.m_bPCActive = false;
		m_MultiLineState.m_bPCHeld = false;
		m_MultiLineState.m_bMobileActive = false;
		m_MultiLineState.m_bMobileHeld = false;
		m_MultiLineState.m_bDeskphoneActive = false;
		m_MultiLineState.m_bDeskphoneHeld = false;
	}

    MultiLineStateArgs(SpokesMultiLineStateFlags multilinestate)
    {
        Init(multilinestate);
    }

	void Init(SpokesMultiLineStateFlags multilinestate)
	{
		m_MultiLineState = multilinestate;
	}
};

/// <summary>
/// Enumeration of multiline device line types
/// </summary>
enum Spokes_Multiline_LineType
{
    Spokes_LineType_PC,
    Spokes_LineType_Mobile,
    Spokes_LineType_Deskphone
};

/// <summary>
/// Interface to allow your application's class to receive Plantronics Spokes events...
/// Each method is optional, you don't have to implement all of them, only the events
/// you are interested in.
/// </summary>
class ISpokesEvents
{
public:
	virtual void Spokes_TakenOff(EventArgs * e) { };
	virtual void Spokes_PutOn(EventArgs * e) { };
	virtual void Spokes_Near(EventArgs * e) { };
	virtual void Spokes_Far(EventArgs * e) { };
	virtual void Spokes_InRange(EventArgs * e) { };
	virtual void Spokes_OutOfRange(EventArgs * e) { };
	virtual void Spokes_Docked(EventArgs * e) { };
	virtual void Spokes_UnDocked(EventArgs * e) { };
	virtual void Spokes_MobileCallerId(EventArgs * e) { };
	virtual void Spokes_OnMobileCall(EventArgs * e) { };
	virtual void Spokes_NotOnMobileCall(EventArgs * e) { };
	virtual void Spokes_SerialNumber(EventArgs * e) { };
	virtual void Spokes_CallAnswered(CallAnsweredArgs * e) { };
	virtual void Spokes_CallEnded(CallEndedArgs * e) { };
	virtual void Spokes_CallSwitched(EventArgs * e) { };
	virtual void Spokes_OnCall(OnCallArgs * e) { };
	virtual void Spokes_NotOnCall(EventArgs * e) { };
	virtual void Spokes_MuteChanged(MuteChangedArgs * e) { };
	virtual void Spokes_LineActiveChanged(LineActiveChangedArgs * e) { };
	virtual void Spokes_Attached(AttachedArgs * e) { };
	virtual void Spokes_Detached(EventArgs * e) { };
	virtual void Spokes_CapabilitiesChanged(EventArgs * e) { };
	virtual void Spokes_MultiLineStateChanged(EventArgs * e) { };
	virtual void Spokes_BatteryLevelChanged(EventArgs * e) { };

	virtual void Spokes_ProximityUnknown(EventArgs * e) { };
	virtual void Spokes_ProximityEnabled(EventArgs * e) { };
	virtual void Spokes_ProximityDisabled(EventArgs * e) { };
	virtual void Spokes_Connected(EventArgs * e) { };
	virtual void Spokes_Disconnected(EventArgs * e) { };
	virtual void Spokes_ButtonPressed(ButtonPressArgs * e) { };
	virtual void Spokes_BaseButtonPressed(BaseButtonPressArgs * e) { };
	virtual void Spokes_CallRequested(CallRequestedArgs * e) { };
};

// internal list of Spokes event types
enum SpokesEventType
{
	Spokes_TakenOff,
	Spokes_PutOn,
	Spokes_Near,
	Spokes_Far,
	Spokes_InRange,
	Spokes_OutOfRange,
	Spokes_Docked,
	Spokes_UnDocked,
	Spokes_MobileCallerId,
	Spokes_OnMobileCall,
	Spokes_NotOnMobileCall,
	Spokes_SerialNumber,
	Spokes_CallAnswered,
	Spokes_CallEnded,
	Spokes_CallSwitched,
	Spokes_OnCall,
	Spokes_NotOnCall,
	Spokes_MuteChanged,
	Spokes_LineActiveChanged,
	Spokes_Attached,
	Spokes_Detached,
	Spokes_CapabilitiesChanged,
	Spokes_MultiLineStateChanged,
	Spokes_BatteryLevelChanged,
	Spokes_ProximityUnknown,
	Spokes_ProximityEnabled,
	Spokes_ProximityDisabled,
	Spokes_Connected,
	Spokes_Disconnected,
	Spokes_ButtonPressed,
	Spokes_BaseButtonPressed,
	Spokes_CallRequested
};

// forward definitions of Spokes globals
extern CComPtr<ICOMDevice> g_pActiveDevice;

class Spokes
{
public:
	static CComAutoCriticalSection s_critSect;

	static Spokes * GetInstance();

	bool m_bIsConnected;

	bool m_bMobileIncoming; // mobile call direction flag
    bool m_bVoipIncoming; // mobile call direction flag
private:
    bool m_lastdocked;
    bool m_lastconnected;
	static Spokes * m_pOnlyOneInstance;
	//holds one and only object of MySingleton

	Spokes(void); // private constructor

    IDebugLogger * m_pDebugLog;

	ISpokesEvents * m_pSpokesEventsHandler; // the calling application's class to send spokes events to

	bool GetActiveAndHeldStates();

	bool GetLastDockedStatus();

	bool GetLastConnectedStatus();

	void GetInitialSoftphoneCallStatus();

	void GetInitialMobileCallStatus();

	void GetInitialDonnedStatus();

	void GetInitialMuteStatus();

	bool GetHoldState(COMLineType lineType);
	bool GetActiveState(COMLineType lineType);

public:
	void NotifyEvent(SpokesEventType e_type, EventArgs * e);

	bool Connect(const char * appName = "COM Session", bool forceconnect = false);

	void Disconnect();

	void SetLogger(IDebugLogger * aLogger);

	void SetEventHandler(ISpokesEvents * eventsHandler);

	void UpdateOtherDeviceCapabilities();

	/// <summary>
	/// Instruct Spokes to tell us the serial numbers of attached Plantronics device, i.e. headset and base/usb adaptor.
	/// </summary>
	void RequestAllSerialNumbers();

	/// <summary>
	/// Instruct Spokes to tell us a serial number of the attached Plantronics device, i.e. headset or base/usb adaptor.
	/// </summary>
	/// <param name="serialNumberType">Allows you to say if you would like the headset or base/usb adaptor serial number.</param>
	void RequestSingleSerialNumber(SpokesSerialNumberTypes serialNumberType);

	/// <summary>
	/// Instructs a mobile that is paired with Plantronics device to dial an outbound mobile call.
	/// </summary>
	/// <param name="numbertodial">The phone number you wish the mobile to call.</param>
	void DialMobileCall(string numbertodial);

	/// <summary>
	/// Instructs a mobile that is paired with Plantronics device to answer an inbound (ringing) mobile call
	/// </summary>
	void AnswerMobileCall();

	/// <summary>
	/// Instructs a mobile that is paired with Plantronics device to end on ongoing mobile call
	/// </summary>
	void EndMobileCall();

	/// <summary>
	/// Allows your softphone application to inform Plantronics device about an incoming call. The Plantronics device will then automatically ring. 
	/// Note: will automatically open audio/rf link to wireless device.
	/// </summary>
	/// <param name="callid">A unique numeric identifier for the call that your application and Spokes will use to identify it as.</param>
	/// <param name="contactname">Optional caller's contact name that will display on Plantronics display devices, e.g. Calisto P800 and P240 devices.</param>
	/// <returns>Boolean indicating if command was issued successfully or not.</returns>
	bool IncomingCall(int callid, string contactname = "");

	/// <summary>
	/// Informs Spokes that your softphone user has answered the ringing softphone call.
	/// </summary>
	/// <param name="callid">The unique numeric id that defines which softphone call you answered.</param>
	bool AnswerCall(int callid);

	/// <summary>
	/// Allows your softphone application to inform Plantronics device about an outgoing call. Note: will automatically open audio/rf link to wireless device.
	/// </summary>
	/// <param name="callid">A unique numeric identifier for the call that your application and Spokes will use to identify it as.</param>
	/// <param name="contactname">Optional caller's contact name that will display on Plantronics display devices, e.g. Calisto P800 and P240 devices.</param>
	/// <returns>Boolean indicating if command was issued successfully or not.</returns>
	bool OutgoingCall(int callid, string contactname = "");

	/// <summary>
	/// Instructs Spokes to end an ongoing softphone call.
	/// </summary>
	/// <param name="callid">The unique numeric id that defines which softphone call you want to end.</param>
	/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
	bool EndCall(int callid);

	/// <summary>
	/// Instructs Spokes to hold an ongoing softphone call.
	/// </summary>
	/// <param name="callid">The unique numeric id that defines which softphone call you want to hold.</param>
	/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
	bool Spokes::HoldCall(int callid);

	/// <summary>
	/// Instructs Spokes to resume an ongoing softphone call.
	/// </summary>
	/// <param name="callid">The unique numeric id that defines which softphone call you want to resume.</param>
	/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
	bool Spokes::ResumeCall(int callid);

	/// <summary>
	/// Instructs Spokes to that an ongoing softphone call is a "conference".
	/// </summary>
	/// <param name="callid">The unique numeric id that defines which softphone call you want to set as conference.</param>
	/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
	bool Spokes::SetConferenceId(int callid);

	/// <summary>
	/// This function will establish or close the audio link between PC and the Plantronics audio device.
	/// It is required to be called where your app needs audio (i.e. when on a call) in order to support Plantronics wireless devices, because
	/// opening the audio link will also bring up the RF link.
	/// </summary>
	/// <param name="connect">Tells Spokes whether to open or close the audio/rf link to device</param>
	bool ConnectAudioLinkToDevice(bool connect);

	/// <summary>
	/// Set the microphone mute state of the attached Plantronics device.
	/// Note: For wireless devices mute only works when the audio/rf link is active (see also ConnectAudioLinkToDevice method).
	/// </summary>
	/// <param name="mute">A boolean indicating if you want mute on or off</param>
	void SetMute(bool mute);

    /// <summary>
    /// Instruct the Plantronics multiline device to activate or deactivate the specified phone line.
    /// </summary>
    /// <param name="multiline_LineType">The line to activate or deactive, PC, Mobile or Desk Phone</param>
    /// <param name="activate">Boolean indicating whether to activate or de-activate the line</param>
	void SetLineActive(Spokes_Multiline_LineType multiline_LineType, bool activate);

    /// <summary>
    /// Instruct the Plantronics multiline device to place on hold or remove from hold the specified phone line.
    /// </summary>
    /// <param name="multiline_LineType">The line to place on hold or remove from hold, PC, Mobile or Desk Phone</param>
    /// <param name="hold">Boolean indicating whether to hold or un-hold the line</param>
	void SetLineHold(Spokes_Multiline_LineType multiline_LineType, bool hold);
	
    /// <summary>
    /// A property containing flags that indicate the capabilities of the attached Plantronics device (if any).
    /// </summary>
    SpokesDeviceCaps m_SpokesDeviceCapabilities;

    /// <summary>
    /// Returns boolean to indicate whether there is currently a Plantronics device attached to the PC or not.
    /// </summary>
    bool HasDevice()
    {
        return (g_pActiveDevice != NULL);
    }

	void GetInitialDeviceState();

	void RegisterForProximity(bool registerForProx);

	bool GetHoldStates();

	bool GetActiveStates();

    SpokesMultiLineStateFlags m_ActiveHeldFlags;

	string m_strDeviceName;

	bool IsSpokesComSessionManagerClassRegistered(int spokesMajorVersion);

	bool IsSpokesInstalled(int spokesMajorVersion = 3);

	vector<SpokesDeviceCaps> m_AllDeviceCapabilities;

	void PreLoadAllDeviceCapabilities();

	SpokesDeviceCaps GetMyDeviceCapabilities();

    void DebugPrint(string methodname, string message);

	// battery level

	/// <summary>
    /// Request from Spokes information about the battery level in the attached wireless device.
    /// Typically your app will call this after receiving a BatteryLevel headset event.
    /// </summary>
    /// <returns>A BatteryLevel structure containing information about the battery level.</returns>
    DeviceBatteryLevel GetBatteryLevel();
};

