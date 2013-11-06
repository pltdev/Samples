#include "stdafx.h"
#include "SpokesWrapper.h"

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iomanip>
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

// Original Events.h contents:

/*!**************************************************************
 * class getDeviceState, getDeviceEvent, getCallState, getHeadstateButton, getAudioType
 *
 * Description: Map enums to strings
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/

string getDeviceState(DeviceState state)
{
	switch(state)
	{
	case DeviceState_Unknown:		return "Unknown";
	case DeviceState_Added:			return "Added";
	case DeviceState_Removed:		return "Removed";
	case DeviceState_RemoveRequest: return "RemoveRequest";
	case DeviceState_RemovePending: return "RemovePending";
	default:						return "Unknown";
	}
}
string getDeviceEvent(DeviceEventKind eventKind)
{
	switch(eventKind)
	{
	case DeviceEventKind_Docked:	return "Docked";
	case DeviceEventKind_UnDocked:	return "UnDocked";
	case DeviceEventKind_TalkPress: return "TalkPress";
	case DeviceEventKind_Unknown:	return "Unknown";
	default:						return "Unknown";
	}
}

string getCallState(CallState callState)
{
	switch(callState)
	{
	case CallState_Unknown:			return "Unknown";
	case CallState_AcceptCall:		return "AcceptCall";
	case CallState_TerminateCall:	return "TerminateCall";
	case CallState_HoldCall:		return "HoldCall";
	case CallState_Resumecall:		return "Resumecall";
	case CallState_Flash:			return "Flash";
	case CallState_CallInProgress:	return "CallInProgress";
	case CallState_CallRinging:		return "CallRinging";
	case CallState_CallEnded:		return "CallEnded";
	case CallState_TransferToHeadSet: return "TransferToHeadSet";
	case CallState_TransferToSpeaker: return "TransferToSpeaker";
	case CallState_MuteON:			return "MuneOn";
	case CallState_MuteOFF:			return "MuteOff";
	case CallState_MobileCallRinging: return "MobileCallRing";
	case CallState_MobileCallInProgress: return "MobileCallInProgress";
	case CallState_MobileCallEnded:	return "MobileCallEnded";
	case CallState_Don:				return "Don";
	case CallState_Doff:			return "Doff";
	case CallState_CallIdle:		return "CallIdle";			
	case CallState_Play:			return "Play";
	case CallState_Pause:			return "Pause";			
	case CallState_Stop:			return "Stop";
	case CallState_DTMFKey:			return "DTMFKey";
	case CallState_RejectCall:		return "RejectCall";
	default:						return "Unknown";

	}
}
string getHeadstateButton(HeadsetButton button)
{
	switch(button)
	{
		case HeadsetButton_Unknown:			return "Unknown";
		case HeadsetButton_VolumeUp:		return "VolumeUp";
		case HeadsetButton_VolumeDown:		return "VolumeDown";
		case HeadsetButton_VolumeUpHeld:	return "UpHeld";
		case HeadsetButton_VolumeDownHeld:	return "DownHeld";
		case HeadsetButton_Mute:			return "Mute";
		case HeadsetButton_MuteHeld:		return "MuteHeld";
		case HeadsetButton_Talk:			return "Talk";
		case HeadsetButton_Audio:			return "Audio";
		case HeadsetButton_Play:			return "Play";
		case HeadsetButton_Pause:			return "Pause";
		case HeadsetButton_Next:			return "Next";
		case HeadsetButton_Previous:		return "Previous";
		case HeadsetButton_FastForward:		return "FastForward";
		case HeadsetButton_Rewind:			return "Rewind";
		case HeadsetButton_Stop:			return "Stop";
		case HeadsetButton_Flash:			return "Flash";
		case HeadsetButton_Smart:			return "Smart";
		case HeadsetButton_OffHook:			return "OffHook";
		case HeadsetButton_OnHook:			return "OnHook";
		case HeadsetButton_Key0:			return "Key0";
		case HeadsetButton_Key1:			return "Key1";
		case HeadsetButton_Key2:			return "Key2";
		case HeadsetButton_Key3:			return "Key3";
		case HeadsetButton_Key4:			return "Key4";
		case HeadsetButton_Key5:			return "Key5";
		case HeadsetButton_Key6:			return "Key6";
		case HeadsetButton_Key7:			return "Key7";
		case HeadsetButton_Key8:			return "Key8";
		case HeadsetButton_Key9:			return "Key9";
		case HeadsetButton_KeyStar:			return "KeyStar";
		case HeadsetButton_KeyPound:		return "KeyPound";
		case HeadsetButton_Speaker:			return "Speaker";
		case HeadsetButton_Reject:			return "Reject";
		default:							return "Unknown";
	}

}
string getAudioType(AudioType type)
{
	switch(type)
	{
	case AudioType_Unknown:			return "Unknown";
	case AudioType_MonoOn:			return "MonoOn";
	case AudioType_MonoOff:			return "MonoOff";
	case AudioType_StereoOn:		return "StereoOn";
	case AudioType_StereoOff:		return "StereoOff";
	case AudioType_MonoOnWait:		return "MonoOnWait";
	case AudioType_StereoOnWait:	return "StereoOnWait";
	default:						return "Unknown";
	}
}

string getHeadsetStateChange(HeadsetStateChange headsetState)
{
	switch(headsetState)
	{
		case HeadsetStateChange_Unknown:	return "Unknown";
		case HeadsetStateChange_MonoON:		return "MonoOn";
		case HeadsetStateChange_MonoOFF:	return "MonoOff";
		case HeadsetStateChange_StereoON:	return "StereoON";
		case HeadsetStateChange_StereoOFF:	return "StereoOFF";
		case HeadsetStateChange_MuteON:		return "MuteON";
		case HeadsetStateChange_MuteOFF:	return "MuteOff";
		case HeadsetStateChange_BatteryLevel: return "BatteryLevel";
		case HeadsetStateChange_InRange:	return "InRange";
		case HeadsetStateChange_OutofRange: return "OutofRange";
		case HeadsetStateChange_Docked:		return "Docked";
		case HeadsetStateChange_UnDocked:	return "UnDocked";
		case HeadsetStateChange_InConference: return "InConference";
		case HeadsetStateChange_Don:		return "Don";
		case HeadsetStateChange_Doff:		return "Doff";
		case HeadsetStateChange_SerialNumber: return "SerialNumber";
		case HeadsetStateChange_Near:		return "Near";
		case HeadsetStateChange_Far:		return "Far";
		case HeadsetStateChange_DockedCharging:		return "DockedCharging";
		case HeadsetStateChange_ProximityUnknown:	return "ProximityUnknown";
		case HeadsetStateChange_ProximityEnabled:	return "ProximityEnabled";
		case HeadsetStateChange_ProximityDisabled:	return "ProximityDisabled";
		default:						return "Unknown";	
	}
}
string getBaseButton(BaseButton button)
{
	switch(button)
	{
		case BaseButton_Unknown:		return "Unknown";
		case BaseButton_PstnTalk:		return "PstnTalk";
		case BaseButton_VoipTalk:		return "VoipTalk";
		case BaseButton_Subscribe:		return "Subscribe";
		case BaseButton_PstnTalkHeld:	return "PstnTalkHeld";
		case BaseButton_VoipTalkHeld:	return "VoipTalkHeld";
		case BaseButton_SubscribeHeld:	return "SubscribeHeld";
		case BaseButton_PstnTalkAndSubscribeHeld: return "PstnTalkAndSubscribeHeld";
		case BaseButton_PstnTalkAndVoipTalkHeld:	return "PstnTalkAndVoipTalkHeld";
		case BaseButton_MakeCall:		return "MakeCall";
		case BaseButton_MobileTalk:		return "MobileTalk";
		case BaseButton_MobileTalkHeld:	return "MobileTalkHeld";
		case BaseButton_PstnTalkAndMobileTalkHeld:	return "PstnTalkAndMobileTalkHeld";
		case BaseButton_VoipTalkAndMobileTalkHeld:	return "VoipTalkAndMobileTalkHeld";
		case BaseButton_DialPad:		return "DialPad";
		case BaseButton_MakeCallFromCallLog: return "MakeCallFromCallLog";
		default:						return "Unknown";	
	}
}
string getBaseStateButton(BaseStateChange buttonState)
{
	switch(buttonState)
	{
		case BaseStateChange_Unknown:				return "Unknown";
		case BaseStateChange_PstnLinkEstablished:	return "PstnLinkEstablished";
		case BaseStateChange_PstnLinkDown:			return "PstnLinkDown";
		case BaseStateChange_VoipLinkEstablished:	return "VoipLinkEstablished";
		case BaseStateChange_VoipLinkDown:			return "VoipLinkDown";
		case BaseStateChange_AudioMixer:			return "AudioMixer";
		case BaseStateChange_RFLinkWideBand:		return "RFLinkWideBand";
		case BaseStateChange_RFLinkNarrowBand:		return "RFLinkNarrowBand";
		case BaseStateChange_MobileLinkEstablished:	return "MobileLinkEstablished";
		case BaseStateChange_MobileLinkDown:		return "MobileLinkDown";
		case BaseStateChange_InterfaceStateChanged:	return "InterfaceStateChanged";
		case BaseStateChange_AudioLocationChanged:	return "AudioLocationChanged";
		case BaseStateChange_SerialNumber:			return "SerialNumber";
		default:						return "Unknown";	
	}
}

// convert BSTR to std string, thanks http://stackoverflow.com/questions/6284524/bstr-to-stdstring-stdwstring-and-vice-versa#
string ConvertWCSToMBS(const wchar_t* pstr, long wslen)
{
	int len = ::WideCharToMultiByte(CP_ACP, 0, pstr, wslen, NULL, 0, NULL, NULL);

	string dblstr(len, '\0');
	len = ::WideCharToMultiByte(CP_ACP, 0 /* no flags */,
								pstr, wslen /* not necessary NULL-terminated */,
								&dblstr[0], len,
								NULL, NULL /* no default char */);

	return dblstr;
}

BSTR ConvertMBSToBSTR(const string& str)
{
	int wslen = ::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
										str.data(), str.length(),
										NULL, 0);

	BSTR wsdata = ::SysAllocStringLen(NULL, wslen);
	::MultiByteToWideChar(CP_ACP, 0 /* no flags */,
							str.data(), str.length(),
							wsdata, wslen);
	return wsdata;
}

string ConvertBSTRToMBS(BSTR bstr)
{
	int wslen = ::SysStringLen(bstr);
	return ConvertWCSToMBS((wchar_t*)bstr, wslen);
}

// SpokesEvents.h

/// <summary>
/// Returns a nice string representation of device capabilites, e.g. for use in logs
/// </summary>
string SpokesDeviceCaps::ToString()
{
	string tempstr;
	ostringstream tmpstrm;

	tmpstrm << "Proximity = " << m_bHasProximity << endl
		<< "Mobile Caller Id = " << m_bHasMobCallerId << endl
		<< "Mobile Call State = " << m_bHasMobCallState << endl
		<< "Dockable = " << m_bHasDocking << endl
		<< "Wearing Sensor = " << m_bHasWearingSensor << endl
		<< "Multiline = " << m_bHasMultiline << endl
		<< "Is Wireless = " << m_bIsWireless << endl;

	string retval = tmpstrm.str();
	return retval;
}


/*!**************************************************************
 * class SessionManagerEventSync
 *
 * Description: Handles Session Manager events
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/

extern void KeepDialToneSuppressed(bool bSuppress);

// external def for g_pAtdCommand to get caller id
extern CComPtr<IATDCommand> g_pAtdCommand;

class SessionManagerEventSync : public CComObjectRootEx<CComSingleThreadModel>, 
								public IDispatch
{
private:
	typedef void (*DeviceCallback)(void);
	DeviceCallback m_fnAttach, m_fnDetach;
public:	
	BEGIN_COM_MAP(SessionManagerEventSync)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID( __uuidof(ISessionCOMManagerEvents), IDispatch)
	END_COM_MAP()

	// set functions that need to be called when DeviceStateChanged
	void DeviceStateHandlers(DeviceCallback attach,DeviceCallback detach)
	{
		m_fnAttach = attach;
		m_fnDetach = detach;
	}

	/*!**************************************************************
	 * CallStateEventHandler
	 *
	 * Description: Called from SPOKES when Session Manager CallStateChanged event is fired
	 * Revision:	1.0
	 * Author:		Andrija M. 02/13/2012
	 ****************************************************************/
	STDMETHOD(CallStateEventHandler)(VARIANT sender, struct _CallStateEventArgs *args)	
	{
		DeviceEventKind eventKind;
		CallState callState;
		args->get_DeviceEvent(&eventKind);
		args->get_Action(&callState);

		CComPtr<ICall> call;
		long callId;
		if( SUCCEEDED (args->get_CallId(&call) ) )
		{
			call->get_Id(&callId);
			// trace call state details

			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "SM Event: EventKind (" << getDeviceEvent(eventKind) << ") Call State (" << getCallState(callState) << ") Call Id (" << callId << ")";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}
		else
		{
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Unable to process Call State Event. get_CallId failed ";
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}

		OnCallArgs * oca;
		OnMobileCallArgs * omca;
		CallAnsweredArgs * caa;
		CallEndedArgs * cea;
		BSTR callsourcebstr;
		string callsource = "";

		switch (callState)
        {
			case Plantronics_UC_Common::CallState_CallRinging:
                Spokes::GetInstance()->m_bVoipIncoming = true;
                // Getting here indicates user is ON A CALL!
				Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity detected!");
				if(SUCCEEDED( args->get_CallSource(&callsourcebstr) ) )
				{
					if (SysStringLen(callsourcebstr)>0)
 						callsource = ConvertBSTRToMBS(callsourcebstr);
				}
				oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_Ringing);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
                break;
            case Plantronics_UC_Common::CallState_MobileCallRinging:
                Spokes::GetInstance()->m_bMobileIncoming = true;
                // user incoming mobile call
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity detected!");
				omca = new OnMobileCallArgs(Spokes::GetInstance()->m_bMobileIncoming, SpokesMobileCallState_Ringing);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnMobileCall, omca);
                break;
            case Plantronics_UC_Common::CallState_MobileCallInProgress:
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity detected!");
				omca = new OnMobileCallArgs(Spokes::GetInstance()->m_bMobileIncoming, SpokesMobileCallState_OnCall);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnMobileCall, omca);
                break;
            case Plantronics_UC_Common::CallState_AcceptCall:
            case Plantronics_UC_Common::CallState_CallInProgress: 
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Call was answered/in progress!");
				// trigger user's event handler
				if(SUCCEEDED( args->get_CallSource(&callsourcebstr) ) )
				{
					if (SysStringLen(callsourcebstr)>0)
						callsource = ConvertBSTRToMBS(callsourcebstr);
				}
				oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_OnCall);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
				caa = new CallAnsweredArgs(callId, callsource);
				Spokes::GetInstance()->NotifyEvent(Spokes_CallAnswered, caa);
                break;
            case Plantronics_UC_Common::CallState_HoldCall:
            case Plantronics_UC_Common::CallState_Resumecall:
            case Plantronics_UC_Common::CallState_TransferToHeadSet:
            case Plantronics_UC_Common::CallState_TransferToSpeaker:
                // Getting here indicates user is ON A CALL!
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity detected!");
				if(SUCCEEDED( args->get_CallSource(&callsourcebstr) ) )
				{
					if (SysStringLen(callsourcebstr)>0)
						callsource = ConvertBSTRToMBS(callsourcebstr);
				}
				oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_OnCall);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
                break;
            case Plantronics_UC_Common::CallState_MobileCallEnded:
                Spokes::GetInstance()->m_bMobileIncoming = false;
                // Getting here indicates user HAS FINISHED A CALL!
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity ended.");
				Spokes::GetInstance()->NotifyEvent(Spokes_NotOnMobileCall, EventArgs::Empty());
                break;
            case Plantronics_UC_Common::CallState_CallEnded:
            case Plantronics_UC_Common::CallState_CallIdle:
            case Plantronics_UC_Common::CallState_RejectCall:
            case Plantronics_UC_Common::CallState_TerminateCall:
                Spokes::GetInstance()->m_bVoipIncoming = false;
                // Getting here indicates user HAS FINISHED A CALL!
                Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity ended.");
				if(SUCCEEDED( args->get_CallSource(&callsourcebstr) ) )
				{
					if (SysStringLen(callsourcebstr)>0)
						callsource = ConvertBSTRToMBS(callsourcebstr);
				}
				Spokes::GetInstance()->NotifyEvent(Spokes_NotOnCall, EventArgs::Empty());
				cea = new CallEndedArgs(callId, callsource);
				Spokes::GetInstance()->NotifyEvent(Spokes_CallEnded, cea);
                break;
            default:
                // ignore other call state events
                break;
        }

		return S_OK;
	}
	/*!**************************************************************
	 * DeviceStateChanged
	 *
	 * Description: Called from SPOKES when Session Manager DeviceStateChanged event is fired
	 * Revision:	1.0
	 * Author:		Andrija M. 02/13/2012
	 ****************************************************************/
	STDMETHOD(DeviceStateChanged)(VARIANT sender, struct _DeviceStateEventArgs *args)	
	{
		DeviceState deviceState;
		args->get_State(&deviceState);
		// trace device state details
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "SM Event: Device state changed " << getDeviceState( deviceState ); 
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		
		// detach from previous device
		m_fnDetach();
		// attach to new device
		m_fnAttach();

		return S_OK;

	}

	//TODO: this events are not exposed from Spokes COM level
	STDMETHOD(Suspending)(VARIANT sender, struct _EventArgs *args)						
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "SM Event: Suspending";		
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		Spokes::GetInstance()->NotifyEvent(Spokes_SystemSuspending, EventArgs::Empty());

		return S_OK;
	}

	STDMETHOD(Resuming)(VARIANT sender, struct _EventArgs *args)						
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "SM Event: Resuming";			
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		Spokes::GetInstance()->NotifyEvent(Spokes_SystemResuming, EventArgs::Empty());

		return S_OK;
	}

	STDMETHOD(QueryConfigChange)(VARIANT sender, struct _EventArgs *args)				
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "SM Event: QueryConfigChange";
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};

/*!**************************************************************
 * class SessionEventSync
 *
 * Description: Handles Call State events
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
class SessionEventSync : public CComObjectRootEx<CComSingleThreadModel>, 
						 public IDispatch
{
public:	
	BEGIN_COM_MAP(SessionManagerEventSync)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID( IID_ICOMCallEvents, IDispatch)
	END_COM_MAP()

	/*!**************************************************************
	 * class CallStateEventHandler
	 *
	 * Description: Callback from SPOKES fired when Call State is changed
	 * Revision:	1.0
	 *				2.0 02/22/2012 Added error handling
	 * Author:		Andrija M. 02/13/2012
	 ****************************************************************/
	STDMETHOD(CallStateEventHandler)(VARIANT sender, struct _CallStateEventArgs *args)	
	{
		DeviceEventKind eventKind;
		CallState callState;
		args->get_DeviceEvent(&eventKind);
		args->get_Action(&callState);

		CComPtr<ICall> call;
		long callId;
		if( SUCCEEDED( args->get_CallId(&call) ) )
		{
			call->get_Id(&callId);
			// trace call state events
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Session Event: Event Kind (" << getDeviceEvent(eventKind) << ") Call State (" << getCallState(callState) << ") Call Id (" << callId << ")";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}
		else
		{
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Unable to process Call State Event. get_CallId failed ";
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}

		return S_OK;
	}
	/*!**************************************************************
	 * class CallRequestEventHandler
	 *
	 * Description: Callback from SPOKES fired when we have call request
	 * Revision:	1.0 
	 *				2.0 02/22/2012 Added resource cleanup and error checking
	 * Author:		Andrija M. 02/13/2012
	 ****************************************************************/
	STDMETHOD(CallRequestEventHandler)(VARIANT sender, struct _CallRequestEventArgs *args)	
	{
		CComPtr<IContact> contact;
		args->get_Contact(&contact);
		BSTR name, phone;
		if( contact != NULL )
		{
			if(SUCCEEDED( contact->get_Name(&name) ) )
			{
				_bstr_t bstrName(name, false); 
				if( bstrName.length() == 0 ) bstrName = "<none>";

				if( SUCCEEDED( contact->get_Phone(&phone) ) )
				{
					_bstr_t bstrPhone(phone, false);
					if( bstrPhone.length() == 0 ) bstrPhone = "<none>";
					string outstr;
					ostringstream tmpstrm;
					tmpstrm << "Session Event: Call request from " << bstrName << "(" << bstrPhone  << ")";
					outstr = tmpstrm.str();
					Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
				}
				else
				{
					string outstr;
					ostringstream tmpstrm;
					tmpstrm << "Unable to process Call Request Event. get_Phone failed ";
					outstr = tmpstrm.str();
					Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
				}
			}
			else
			{
				string outstr;
				ostringstream tmpstrm;
				tmpstrm << "Unable to process Call Request Event. get_Name failed ";
				outstr = tmpstrm.str();
				Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
			}
		}
		else
		{
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Unable to process Call Request Event. get_Contact failed ";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}

		return S_OK;
	}


	//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};

/*!**************************************************************
 * class DeviceEventSink
 *
 * Description: Handles Device Events. This events are fired when different buttons are pressed
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
class DeviceEventSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatch
{
public:	

	BEGIN_COM_MAP(DeviceEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(IDeviceCOMEvents), IDispatch)
	END_COM_MAP()

	
	//IID_IDeviceCOMEvents Methods
	STDMETHOD(TalkPressed)(VARIANT sender, struct _DeviceEventArgs *args)			
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: TalkPressed";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	STDMETHOD(ButtonPressed)(VARIANT sender, struct _DeviceEventArgs *args)			
	{
		HeadsetButton button;
		args->get_ButtonPressed(&button);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: ButtonPressed " << getHeadstateButton(button);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	STDMETHOD(MuteStateChanged)(VARIANT sender, struct _DeviceEventArgs *args)		
	{
		VARIANT_BOOL mute = VARIANT_TRUE;
		args->get_Mute(&mute);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: MuteStateChanged Mute=" << ((mute==VARIANT_TRUE)?"true":"false");	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		MuteChangedArgs * muteargs = new MuteChangedArgs(mute==VARIANT_TRUE);
		Spokes::GetInstance()->NotifyEvent(Spokes_MuteChanged, muteargs);
		return S_OK;
	}

	STDMETHOD(AudioStateChanged)(VARIANT sender, struct _DeviceEventArgs *args)		
	{
		AudioType audioType;
		args->get_AudioState(&audioType);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: AudioStateChanged AudioState= " << getAudioType(audioType);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		LineActiveChangedArgs * lineargs = new LineActiveChangedArgs(audioType==AudioType_MonoOn);
		Spokes::GetInstance()->NotifyEvent(Spokes_LineActiveChanged, lineargs);
		return S_OK;
	}

	STDMETHOD(FlashPressed)(VARIANT,_DeviceEventArgs *)								
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: FlashPressed";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	STDMETHOD(SmartPressed)(VARIANT,_DeviceEventArgs *)								
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Event: SmartPressed";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}


		//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};

/*!**************************************************************
 * class DeviceListenerEventSink
 *
 * Description: Handles Device Listner Events.
 * Revision:	1.0
 * Author:		Andrija M. 02/15/2012
 ****************************************************************/
class DeviceListenerEventSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatch
{
public:	

	BEGIN_COM_MAP(DeviceListenerEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(IDeviceListenerCOMEvents), IDispatch)
	END_COM_MAP()

	
	//IID_IDeviceListenerCOMEvents Methods
	STDMETHOD(HeadsetButtonPressed)(VARIANT sender, struct _DeviceListenerEventArgs *args)			
	{
        KeepDialToneSuppressed(true);

		HeadsetButton button;
		args->get_HeadsetButtonPressed(&button);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: HeadsetButton " << getHeadstateButton(button);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	STDMETHOD(HeadsetStateChanged)(VARIANT sender, struct _DeviceListenerEventArgs *args)			
	{
		HeadsetStateChange headstateChange;
		args->get_HeadsetStateChange(&headstateChange);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: HeadsetState " << getHeadsetStateChange(headstateChange);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		// For proximity, re-register for proximity on "inrange" event
		switch (headstateChange)
		{
			case HeadsetStateChange_Don:
				Spokes::GetInstance()->NotifyEvent(Spokes_PutOn, EventArgs::Empty());
				break;
			case HeadsetStateChange_Doff:
				Spokes::GetInstance()->NotifyEvent(Spokes_TakenOff, EventArgs::Empty());
				break;
			case HeadsetStateChange_InRange:
				{
					Spokes::GetInstance()->NotifyEvent(Spokes_InRange, EventArgs::Empty());
					//Spokes::GetInstance()->RegisterForProximity(true);
					Spokes::GetInstance()->GetInitialDeviceState();
					string outstr;
					ostringstream tmpstrm;
					tmpstrm << "InRange received - re-register for proximity...";	
					outstr = tmpstrm.str();
					Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

					Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty()); // tell app to look at battery state
				}
			break;
			case HeadsetStateChange_OutofRange:
				Spokes::GetInstance()->NotifyEvent(Spokes_OutOfRange, EventArgs::Empty());

                Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty()); // tell app to look at battery state
			break;
			case HeadsetStateChange_BatteryLevel:
				Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty()); // tell app to look at battery state
			break;
			case HeadsetStateChange_Near:
				{
					Spokes::GetInstance()->NotifyEvent(Spokes_Near, EventArgs::Empty());
					string outstr;
					ostringstream tmpstrm;
					tmpstrm << "Do something for Near!";
					outstr = tmpstrm.str();
					Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
				}
			break;
			case HeadsetStateChange_Far:
				{
					Spokes::GetInstance()->NotifyEvent(Spokes_Far, EventArgs::Empty());
					string outstr;
					ostringstream tmpstrm;
					tmpstrm << "Do something for Far!";
					outstr = tmpstrm.str();
					Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
				}
				break;
		}

		return S_OK;
	}

	STDMETHOD(BaseButtonPressed)(VARIANT sender, struct _DeviceListenerEventArgs *args)		
	{
		BaseButton button;
		args->get_BaseButtonPressed(&button);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: BaseButton " << getBaseButton(button);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

	STDMETHOD(BaseStateChanged)(VARIANT sender, struct _DeviceListenerEventArgs *args)		
	{
		BaseStateChange basestateChanged;
		args->get_BaseStateChange(&basestateChanged);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: BaseState " << getBaseStateButton(basestateChanged);	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}


	// NOTE, LC 13-09-2012: using Device Listener ATDStateChanged event and ATDCommand interface
	// to obtain caller id! Via COM Service interface this is the only way to do it
	// because the IMobilePresenceEvents available in iPlugin SDK is not exposed to COM
    STDMETHOD(ATDStateChanged)(VARIANT,_DeviceListenerEventArgs * args)								
	{
		ATDStateChange atdState;
		args->get_ATDStateChange(&atdState); // supporting only Unknown for now
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: ATDState Unknown";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		BSTR callerId;

		switch (atdState)
		{
		case ATDStateChange_MobileCallerID:
			g_pAtdCommand->get_CallerID(&callerId);
			wcout << L"MOBILE CALLER ID RECEIVED = " << callerId << endl;
			break;
		}
		return S_OK;
	}

		//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};



// Additional event sinks for serial number events:

/*!**************************************************************
 * class DeviceCOMEventSink
 *
 * Description: Handles Extended Device Events, i.e. Serial Number
 * Revision:	1.0
 * Author:		Lewis Collins. 12/12/2012
 ****************************************************************/
class DeviceEventExtSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatch
{
public:	

	BEGIN_COM_MAP(DeviceEventExtSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(IDeviceEventsExt), IDispatch)
	END_COM_MAP()

	
	//IID_IDeviceCOMEventsExt Methods
	STDMETHOD(HeadsetStateChanged)(VARIANT sender, struct HeadsetStateEventArgs *args)			
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Extended Event: HeadsetStateChanged";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		BSTR pRetVal = NULL;

		//args.
		// get serial number of the device
		HRESULT hRes = g_pActiveDevice->get_SerialNumber(&pRetVal);
		//if (!SUCCEEDED(hRes))
		//	ComplainAndExit(hRes, L"get_SerialNumber FAILED");
		//	PopulateInfoInternal(pRetVal);

		//args->get_SerialNumber();

  //    virtual HRESULT __stdcall get_SerialNumber (
  //      /*[out,retval]*/ SAFEARRAY * * pRetVal ) = 0;
		//
		return S_OK;
	}

		//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};

/*!**************************************************************
 * class BaseEventSink
 *
 * Description: Handles Base Events, i.e. Serial Number
 * Revision:	1.0
 * Author:		Lewis Collins. 12/12/2012
 ****************************************************************/
class BaseEventSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatch
{
public:	

	BEGIN_COM_MAP(BaseEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(IBaseEvents), IDispatch)
	END_COM_MAP()

	
	//IID_IDeviceCOMEventsExt Methods
	STDMETHOD(BaseEventReceived)(VARIANT sender, struct BaseEventArgs *args)			
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Base Event: BaseEventReceived";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		return S_OK;
	}

		//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};



// CallObjects.h:




/*!**************************************************************
 * class CallContact
 *
 * Description: Implementation of IContact interface
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
class CallContact : public CComObjectRootEx<CComSingleThreadModel>, 
					public IDispatch
{
private:
	bstr_t m_bstrcontactName;
public:	
	BEGIN_COM_MAP(SessionManagerEventSync)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID( __uuidof(IContact), IDispatch)
	END_COM_MAP()
	
	// called by Spokes. Returns name that will be called.
	STDMETHOD(get_Name)(BSTR * pRetVal )		
	{ 
		*pRetVal = ::SysAllocString( m_bstrcontactName );
		return S_OK;
	}
	// save contact name
	STDMETHOD(put_Name)( BSTR pRetVal )			
	{ 
		m_bstrcontactName = pRetVal; 
		return S_OK;
	}
	STDMETHOD(get_FriendlyName)(BSTR * pRetVal ){*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_FriendlyName)( BSTR pRetVal ) {return S_OK;}
	STDMETHOD(get_Id)(long * pRetVal)			{*pRetVal = 0; return S_OK;}
	STDMETHOD(put_Id)(long pRetVal )			{return S_OK;}
	STDMETHOD(get_SipUri)(BSTR * pRetVal )		{*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_SipUri)( BSTR pRetVal )		{return S_OK;}
	STDMETHOD(get_Phone)( BSTR* pRetVal )		{*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_Phone)(BSTR pRetVal )		{return S_OK;}
	STDMETHOD(get_Email)(BSTR * pRetVal )		{*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_Email)( BSTR pRetVal )		{return S_OK;}
	STDMETHOD(get_WorkPhone)(BSTR * pRetVal )	{*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_WorkPhone)( BSTR pRetVal )	{return S_OK;}
	STDMETHOD(get_MobilePhone)(BSTR * pRetVal ) {*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_MobilePhone)( BSTR pRetVal )	{return S_OK;}	
	STDMETHOD(get_HomePhone)(BSTR * pRetVal )	{*pRetVal = ::SysAllocString( L"" );return S_OK;}
	STDMETHOD(put_HomePhone)( BSTR pRetVal )	{return S_OK;}
 
	static CComObject<CallContact>* GetContact(string name)
	{
		CComObject<CallContact> *contact = NULL;
		CComObject<CallContact>::CreateInstance(&contact);
		contact->put_Name( _bstr_t( name.c_str() ) );
		return contact;
	}
	
	//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};
/*!**************************************************************
 * class Call
 *
 * Description: Implementation of ICall interface
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
class Call : public CComObjectRootEx<CComSingleThreadModel>, 
			 public IDispatch
{
private:
	long m_ldid;
public:	
	BEGIN_COM_MAP(SessionManagerEventSync)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID( __uuidof(ICall), IDispatch)
	END_COM_MAP()
	
	STDMETHOD( get_Id )( long * pRetVal ) { *pRetVal = m_ldid; return S_OK;}
	STDMETHOD( put_Id )( long * pRetVal ) { m_ldid = *pRetVal; return S_OK;}	
	STDMETHOD( get_InConference )( long * pRetVal ) {*pRetVal = 0; return S_OK;}
	STDMETHOD( put_InConference )( long * pRetVal ) {return S_OK;}
	STDMETHOD( get_ConferenceId )( long * pRetVal ) {*pRetVal = 0; return S_OK;}
	STDMETHOD( put_ConferenceId )( long * pRetVal ) {return S_OK;}

	static CComObject<Call>* GetCall(long id)
	{
		CComObject<Call> *call = NULL;
		CComObject<Call>::CreateInstance(&call);
		call->put_Id(&id);
		return call;
	}


	//empty dispatch
	STDMETHOD(Invoke)(DISPID dispID,REFIID riid,LCID lcid,WORD wFlags,DISPPARAMS* pDispParams, VARIANT* pVarResult,EXCEPINFO* pExcepInfo,UINT* puArgErr) {return S_OK;}  
	STDMETHOD(GetTypeInfoCount)(UINT *)	{return S_OK;}  
	STDMETHOD(GetTypeInfo)(UINT,LCID,ITypeInfo **)	{return S_OK;}  
	STDMETHOD(GetIDsOfNames)(const IID &,LPOLESTR *,UINT,LCID,DISPID *){return S_OK;}  
};




CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule=&_Module;

// SPOKES Interface definitions
CComPtr<ISessionCOMManager> g_pSessMgr;
CComPtr<IComSession> g_pSession;
CComPtr<IHostCommand> g_pHostCommand;
CComPtr<IHostCommandExt> g_pHostCommandExt;
CComPtr<IATDCommand> g_pAtdCommand;
CComPtr<IDeviceListener> g_pDeviceListener;
CComPtr<IDeviceEvents> g_pDeviceEvents;
CComPtr<IDevice> g_pActiveDevice = NULL;
CComPtr<ICallCommand> g_pCallCommand;
CComPtr<ICallEvents> g_pCallEvents;
//Encapsulates event sink defined in SpokesEvents.h
CComObject<SessionManagerEventSync> * g_pSessionMgrEventsSink = NULL;
CComObject<SessionEventSync> * g_pSessionEventSink = NULL;
CComObject<DeviceEventSink> * g_pDeviceEventSink = NULL;
CComObject<DeviceListenerEventSink> * g_pDeviceListenerSink = NULL;

// for getting serial numbers...
CComPtr<IBaseEvents> g_pBaseEvents;
CComPtr<IDeviceEventsExt> g_pDeviceEventsExt;

CComObject<DeviceEventExtSink> * g_pDeviceEventExtSink = NULL;
CComObject<BaseEventSink> * g_pBaseEventSink = NULL;


DWORD g_pSinkCookie, g_pSinkDeviceCookie, g_pSinkSessionCookie, g_pSinkListenerCookie, g_pSinkMobileCookie,
	g_pSinkDeviceExtCookie, g_pSinkBaseCookie;


/*!**************************************************************
 * PrintDevice
 *
 * Description: Print device information to console
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
void PrintDevice()
{
	if( g_pActiveDevice == NULL) return;

	// get basic information about device
	BSTR internalName, manufacturerName, productName;
	BSTR usbVersion, baseVersion, remoteFirmware;
	long vendorId, productId, versionNum;
	g_pActiveDevice->get_InternalName(&internalName);
	g_pActiveDevice->get_ManufacturerName(&manufacturerName);
	g_pActiveDevice->get_ProductName(&productName);
	g_pActiveDevice->get_VendorID(&vendorId);
	g_pActiveDevice->get_ProductID(&productId);
	g_pActiveDevice->get_VersionNumber(&versionNum);

	if( g_pHostCommand != NULL )
	{
		g_pHostCommand->GetVersion(VersionType_USBFirmware, &usbVersion);
		g_pHostCommand->GetVersion(VersionType_BaseFirmware, &baseVersion);
		g_pHostCommand->GetVersion(VersionType_RemoteFirmware, &remoteFirmware);

		// print this data on console
		wcout << L"********************* Device Information ******************** " << endl;
		wcout << L"Internal Name:       " << internalName << endl;
		wcout << L"Manufacturer Name:   " << manufacturerName << endl;
		wcout << L"Product Name:        " << productName << endl;
		wcout << L"Vendor ID:           " << vendorId << endl;
		wcout << L"Product ID:          " << productId << endl;
		wcout << L"Version Num:         " << versionNum << endl;
		wcout << L"USB firmware:        " << usbVersion << endl;
		wcout << L"Base firmware:       " << baseVersion << endl;
		wcout << L"Remote firmware:     " << remoteFirmware << endl;
		wcout << L"************************************************************* " << endl;

		::SysFreeString(usbVersion);
		::SysFreeString(baseVersion);
		::SysFreeString(remoteFirmware);
	}
	::SysFreeString(internalName);
	::SysFreeString(manufacturerName);
	::SysFreeString(productName);
}
/*!**************************************************************
 * PrintSession
 *
 * Description: Print g_pSession information to console
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
void PrintSession()
{
	if( g_pSession == NULL ) return;

	// get basic information about g_pSession
	BSTR pluginName;
	GUID sessionID;
	wchar_t  szGuid[40]={0};
	g_pSession->get_PluginName(&pluginName); 
	g_pSession->get_Id(&sessionID);
	StringFromGUID2( sessionID, (LPOLESTR )szGuid, 40 );

	// print this data on console
	wcout << L"********************* Session Information ******************** " << endl;
	wcout << L"Plugin Name:     " << pluginName << endl;
	wcout << L"Session ID:      " << wstring(szGuid) << endl;
	wcout << L"************************************************************* " << endl;

	::SysFreeString( pluginName );
}

/*!**************************************************************
 * RegisterForProximity
 *
 * Description: Query for IDevice Interface and get HostCommand, DeviceListener and DeviceEvents
 * Revision:	1.0 
 * Author:		Collins L. 09/10/2012
 ****************************************************************/
void RegisterForProximity()
{
	// Plan: Use the IHostCommandExt interface (if there is one for connected device)
	// Then call the enableproximity and getproximity functions to start proximity events being send to us from Spokes SDK
	
    if (g_pHostCommandExt != NULL)
    {
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "About to enable proximity...";
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		VARIANT_BOOL bSuccess = FALSE;
		g_pHostCommandExt->EnableProximity(TRUE, &bSuccess);
		if (bSuccess != FALSE)
		{
			g_pHostCommandExt->GetProximity(&bSuccess);
			if (bSuccess != FALSE)
			{
				string outstr;
				ostringstream tmpstrm;
				tmpstrm << "Proximity Enabled.";
				outstr = tmpstrm.str();
				Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
			}
			else
			{
				string outstr;
				ostringstream tmpstrm;
				tmpstrm << "Unable to register proximity. Are you sure you have a device attached that supports it and have updated firmware to latest using Plantronics Update Manager (part of Spokes from plantronics.com)?";
				outstr = tmpstrm.str();
				Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
			}
		}
		else
		{
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Unable to register proximity. Are you sure you have a device attached that supports it and have updated firmware to latest using Plantronics Update Manager (part of Spokes from plantronics.com)?";
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		}
    }
}

/*!**************************************************************
 * AttachDevice
 *
 * Description: Query for IDevice Interface and get HostCommand, DeviceListener and DeviceEvents
 * Revision:	1.0 
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
void AttachDevice()
{
	HRESULT hRes;

	// Get device from active g_pSession
	if( g_pActiveDevice == NULL && g_pSession != NULL)
	{
		g_pSession->get_ActiveDevice(&g_pActiveDevice);
	}

	// If we have active device, then get HostCommad, DeviceListener and DeviceEvent
	if( g_pActiveDevice != NULL)
	{
        // LC assume minimum first set of device capabilities...
		Spokes::GetInstance()->m_SpokesDeviceCapabilities.Init(false, false, false, false, false, false, false);
		Spokes::GetInstance()->NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());

		SerialNumberArgs * sn = new SerialNumberArgs("", Spokes_Base);
		Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
		sn = new SerialNumberArgs("", Spokes_Headset);
		Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);

        // LC have seen case where ProductName was empty but InternalName was not...
		BSTR productName;
		g_pActiveDevice->get_ProductName(&productName);
		string csStr = ConvertBSTRToMBS(productName);
		if (csStr.length()<1)
		{
			::SysFreeString(productName);
			g_pActiveDevice->get_InternalName(&productName);
			csStr = ConvertBSTRToMBS(productName);
		}
		if (csStr.length()<1)
		{
			csStr = "Could not determine device name";
		}

		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "ATTACHED = " << csStr;
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		outstr = tmpstrm.str();
		Spokes::GetInstance()->m_strDeviceName = csStr;
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		::SysFreeString(productName);

		g_pActiveDevice->get_HostCommand(&g_pHostCommand);

		g_pHostCommandExt = NULL;
		hRes = g_pHostCommand->QueryInterface(IID_IHostCommandExt,
				(void**)&g_pHostCommandExt);
		if (!SUCCEEDED(hRes))
			wcout << L"INFORMATION unable to get IID_IHostCommandExt interface (only supported by some products)" << endl;

		// need to get caller ids
		g_pAtdCommand = NULL;
		hRes = g_pHostCommand->QueryInterface(IID_IATDCommand,
				(void**)&g_pAtdCommand);
		if (!SUCCEEDED(hRes))
			wcout << L"INFORMATION unable to get IID_IATDCommand interface (only supported by some products)" << endl;

		PrintDevice();
		
		// hook to device event
		if( SUCCEEDED(g_pActiveDevice->get_DeviceEvents(&g_pDeviceEvents)))
		{
			CComObject<DeviceEventSink>::CreateInstance(&g_pDeviceEventSink);
			AtlAdvise(g_pDeviceEvents, g_pDeviceEventSink, __uuidof(IDeviceCOMEvents), &g_pSinkDeviceCookie);
		}

		// hook to device listner event
		if( SUCCEEDED(g_pActiveDevice->get_DeviceListener(&g_pDeviceListener)))
		{
			CComObject<DeviceListenerEventSink>::CreateInstance(&g_pDeviceListenerSink);
			AtlAdvise(g_pDeviceListener, g_pDeviceListenerSink, __uuidof(IDeviceListenerCOMEvents), &g_pSinkListenerCookie);
		}

        //// LC, Nemanja change, wire up serial number friendly events
		// namely: IDeviceCOMEventsExt::HeadsetStateChanged
		// and: IBaseCOMEvents::BaseEventReceived
		hRes = g_pDeviceEvents->QueryInterface(IID_IDeviceEventsExt,
			(void**)&g_pDeviceEventsExt);
		if (SUCCEEDED(hRes))
		{
			CComObject<DeviceEventExtSink>::CreateInstance(&g_pDeviceEventExtSink);
			AtlAdvise(g_pDeviceEventsExt, g_pDeviceEventExtSink, __uuidof(IDeviceCOMEventsExt), &g_pSinkDeviceExtCookie);
		}
		else
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, "INFORMATION unable to get IID_IDeviceEventsExt interface (only supported by some products)");

		hRes = g_pDeviceEvents->QueryInterface(IID_IBaseEvents,
			(void**)&g_pBaseEvents);
		if (SUCCEEDED(hRes))
		{
			CComObject<BaseEventSink>::CreateInstance(&g_pBaseEventSink);
			AtlAdvise(g_pBaseEvents, g_pBaseEventSink, __uuidof(IBaseCOMEvents), &g_pSinkBaseCookie);
		}
		else
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, "INFORMATION unable to get IID_IBaseEvents interface (only supported by some products)");

        Spokes::GetInstance()->UpdateOtherDeviceCapabilities();

        // trigger user's event handler
		EventArgs * att = new AttachedArgs(Spokes::GetInstance()->m_strDeviceName);
		Spokes::GetInstance()->NotifyEvent(Spokes_Attached, att);

		Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty());

        // now poll for current state (proximity, mobile call status, donned status, mute status)
        Spokes::GetInstance()->GetInitialDeviceState();

        Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: AttachedEventHandler to device");
	}
}

/*!**************************************************************
 * DetachDevice
 *
 * Description: Release all COM interfaces
 * Revision:	1.0
 * Author:		Andrija M. 02/13/2012
 ****************************************************************/
void DetachDevice()
{
	if( g_pActiveDevice != NULL )
	{
		// release all interfaces
		g_pActiveDevice.Release();

		// Unhook Device Events and Device Listener Events
		AtlUnadvise(g_pDeviceEvents, __uuidof(IDeviceCOMEvents), g_pSinkDeviceCookie);
		AtlUnadvise(g_pDeviceListener, __uuidof(IDeviceListenerCOMEvents), g_pSinkListenerCookie);
		AtlUnadvise(g_pDeviceEventsExt, __uuidof(IDeviceCOMEventsExt), g_pSinkDeviceExtCookie);
		AtlUnadvise(g_pBaseEvents, __uuidof(IBaseCOMEvents), g_pSinkBaseCookie);

		g_pHostCommand.Release();
		g_pHostCommandExt.Release();
		g_pAtdCommand.Release();
		g_pDeviceListener.Release();
		g_pDeviceEvents.Release();
		g_pDeviceEventsExt.Release();
		g_pBaseEvents.Release();

		// LC Device was disconnected, clear down the GUI state...
        Spokes::GetInstance()->m_bMobileIncoming = false; // clear mobile call direction flag
        Spokes::GetInstance()->m_bVoipIncoming = false; // clear call direction flag
        //OnNotOnCall(EventArgs.Empty);
        //OnNotOnMobileCall(EventArgs.Empty);

        //OnSerialNumber(new SerialNumberArgs("", SerialNumberTypes.Base));
        //OnSerialNumber(new SerialNumberArgs("", SerialNumberTypes.Headset));

        //// LC Device was disconnected, remove capability data
        //DeviceCapabilities = new SpokesDeviceCaps(false, false, false, false, false, false); // no device = no capabilities!
        //m_strdevicename = "";
        //OnCapabilitiesChanged(EventArgs.Empty);

		// trigger user's event handler
		Spokes::GetInstance()->NotifyEvent(Spokes_Detached, EventArgs::Empty());

		Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty());

		Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: DetachedEventHandler from device");
	}
	g_pActiveDevice = NULL;
}

// Variables used to track "on hold" and "active" states for multi-line devices, e.g. Savi 7xx
bool m_bPSTNOnHold, m_bMobileOnHold, m_bVOIPOnHold;
bool m_bPSTNActive, m_bMobileActive, m_bVOIPActive;


void KeepDialToneSuppressed(bool bSuppress)
{
	if( g_pDeviceListener != NULL )
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "***Suppressing dial-tone in IDeviceListener." << endl;
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		g_pDeviceListener->SuppressDialTone(true);
	}
}





//Declaration of CComCriticalSection static member
CComAutoCriticalSection Spokes::s_critSect;
Spokes * Spokes::m_pOnlyOneInstance = NULL;

Spokes::Spokes(void)
{
	// instantiate member variables as required
	m_pDebugLog = NULL;
	m_pSpokesEventsHandler = NULL;
	m_bIsConnected = false;
    m_bMobileIncoming = false; // mobile call direction flag
    m_bVoipIncoming = false; // mobile call direction flag
	m_strDeviceName = "";

	PreLoadAllDeviceCapabilities();
}

SpokesDeviceCaps Spokes::GetMyDeviceCapabilities()
{
	SpokesDeviceCaps retval;
	retval.m_strProductId="";
	char prodidstr[10];

	if (g_pActiveDevice!=NULL && m_AllDeviceCapabilities.size()>0)
	{
		long productId;
		g_pActiveDevice->get_ProductID(&productId);
		sprintf_s(prodidstr, "%x", productId);
		string prodidstring = prodidstr;
		std::transform(prodidstring.begin(), prodidstring.end(),prodidstring.begin(), ::toupper);

		for (int i = 0;i<m_AllDeviceCapabilities.size();i++)
		{
			if (m_AllDeviceCapabilities[i].m_strProductId.compare(prodidstring)==0)
			{
				// we got a match of our product!
				DebugPrint(__FUNCTION__, "INFO: Got a match of our Plantronics device in DeviceCapabilities.csv:");
				DebugPrint(__FUNCTION__, prodidstr);
				retval.m_strProductId = prodidstr;
				retval.m_bHasProximity = m_AllDeviceCapabilities[i].m_bHasProximity;
				retval.m_bHasMobCallerId = m_AllDeviceCapabilities[i].m_bHasMobCallerId;
				retval.m_bHasMobCallState = m_AllDeviceCapabilities[i].m_bHasMobCallState;
				retval.m_bHasDocking = m_AllDeviceCapabilities[i].m_bHasDocking;
				retval.m_bHasWearingSensor = m_AllDeviceCapabilities[i].m_bHasWearingSensor;
				retval.m_bHasMultiline = m_AllDeviceCapabilities[i].m_bHasMultiline;
				retval.m_bIsWireless = m_AllDeviceCapabilities[i].m_bIsWireless;
			}
		}
	}
	return retval;
}

void Spokes::PreLoadAllDeviceCapabilities()
{
	ifstream in_stream;

	string line;

	SpokesDeviceCaps devicecaps;

	try 
	{
		in_stream.exceptions ( std::ifstream::failbit | std::ifstream::badbit );

		in_stream.open("DeviceCapabilities.csv");

		while(!in_stream.eof())
		{
			try
			{
				std::getline(in_stream, line);

				if (line.length()>0)
				{
					if (line.substr(0,1).compare("#")!=0 && line.substr(0,1).compare(",")!=0)
					{

						// not a comment line or empty line (with only commas)
						
						char *next_token;
						char *p = strtok_s((char*)line.c_str(), ",", &next_token);
						int i = 0;
						string token;
						while (p) {
							//printf ("Token: %s\n", p);
							token = p;
							switch(i)
							{
							case 0:
								devicecaps.m_strProductId = p;
								std::transform(devicecaps.m_strProductId.begin(), devicecaps.m_strProductId.end(),devicecaps.m_strProductId.begin(), ::toupper);
								break;
							case 1:
								// no action - this is the device name we don't need
								break;
							case 2:
								devicecaps.m_bHasProximity = token.compare("Yes")==0 ? true : false;
								break;
							case 3:
								devicecaps.m_bHasMobCallerId = token.compare("Yes")==0 ? true : false;
								break;
							case 4:
								devicecaps.m_bHasMobCallState = token.compare("Yes")==0 ? true : false;
								break;
							case 5:
								devicecaps.m_bHasDocking = token.compare("Yes")==0 ? true : false;
								break;
							case 6:
								devicecaps.m_bHasWearingSensor = token.compare("Yes")==0 ? true : false;
								break;
							case 7:
								devicecaps.m_bHasMultiline = token.compare("Yes")==0 ? true : false;
								break;
							case 8:
								devicecaps.m_bIsWireless = token.compare("Yes")==0 ? true : false;

								// now, add the devicecaps to our list:
								m_AllDeviceCapabilities.push_back(devicecaps);

								break;
							}

							p = strtok_s(NULL, ",", &next_token);
							i++;
						}
						
						//DebugPrint(__FUNCTION__, "got some tokens");


					}
				}
				//devicecaps
			}
			catch(std::ifstream::failure e) {
				// it's ok, probably just reached end of file, see: http://stackoverflow.com/questions/11807804/stdgetline-throwing-when-it-hits-eof
			}
		}
		in_stream.close();
	}
	catch(std::ifstream::failure e) {
		//std::cerr << "Exception opening/reading/closing file\n";
		DebugPrint(__FUNCTION__, "Exception reading DeviceCapabilities.csv. Does this file exist in current working directory?");
	}
}

Spokes * Spokes::GetInstance()
{
	if (m_pOnlyOneInstance == NULL)
	{
		s_critSect.Lock();
		if (m_pOnlyOneInstance == NULL)
			// Solution 1 and 2 gaps addressed by moving
			// critical section block and by re-doing this check!
		{
			m_pOnlyOneInstance = new Spokes();
		}
		s_critSect.Unlock();
	}
	return m_pOnlyOneInstance;
}

/// <summary>
/// If your application class implements the Spokes.DebugLogger interface you can pass a reference to your application class
/// to the SetLogger method. This allows your class to be responsible for debug logging of Spokes related debug trace information.
/// </summary>
/// <param name="aLogger">For this parameter pass the "this" reference of your class that implements Spokes.DebugLogger interface.</param>
void Spokes::SetLogger(IDebugLogger * aLogger)
{
    m_pDebugLog = aLogger;
}

/// <summary>
/// If your application class implements the Spokes.ISpokesEvents interface you can pass a reference to your application class
/// to the SetEventHandler method. This allows your class to receive Spokes Events that you may be interested in and that you
/// also provide an implementation using the ISpokesEvents virtual functions.
/// </summary>
/// <param name="eventsHandler">For this parameter pass the "this" reference of your class that implements Spokes.ISpokesEvents interface.</param>
void Spokes::SetEventHandler(ISpokesEvents * eventsHandler)
{
    m_pSpokesEventsHandler = eventsHandler;
}

bool Spokes::Connect(const char * appName, bool forceconnect)
{
    if (!IsSpokesInstalled() && forceconnect==false)
    {
		DebugPrint(__FUNCTION__, "FATAL ERROR: cannot connect if Spokes COMSessionManager/SessionCOMManager class is not registered! Spokes not installed (or wrong major version installed for this Spokes Wrapper)!");
        return false; // cannot connect if Spokes COM SessionManager class is not registered! Spokes not installed!
    }

	if (m_bIsConnected) return true;

    m_SpokesDeviceCapabilities.Init(false, false, false, false, false, false, false); // we don't yet know what the capabilities are
	NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
    bool success = false;

	// initialize COM
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	DebugPrint(__FUNCTION__, "Connecting to SPOKES session manager");

	// Get session manager from 
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr = ::CoCreateInstance(CLSID_SessionComManager, NULL,CLSCTX_LOCAL_SERVER, IID_ISessionCOMManager, (LPVOID*)&g_pSessMgr )))
	{
		// hook to g_pSession manager events
		CComObject<SessionManagerEventSync>::CreateInstance(&g_pSessionMgrEventsSink);
		AtlAdvise(g_pSessMgr, g_pSessionMgrEventsSink, __uuidof(ISessionCOMManagerEvents), &g_pSinkCookie);
		// set function pointers to attach/detach to device when we recieve Device events
		g_pSessionMgrEventsSink->DeviceStateHandlers( AttachDevice, DetachDevice );
 
		// register new plugin with Spokes
		if (SUCCEEDED(hr = g_pSessMgr->Register(_bstr_t(appName), &g_pSession )))
		{	
			// hook to Session call events
			g_pSession->get_CallEvents(&g_pCallEvents);
			if( g_pCallEvents != NULL )
			{
				CComObject<SessionEventSync>::CreateInstance(&g_pSessionEventSink);
				AtlAdvise(g_pCallEvents, g_pSessionEventSink, IID_ICOMCallEvents, &g_pSinkSessionCookie);
			}

			// print basic g_pSession information
			PrintSession();
			
			g_pSession->get_CallCommand(&g_pCallCommand);

			// Attach to active device
			AttachDevice();

			m_bIsConnected = true;
			success = true;
		}
		else
		{
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Unable to register COM session. Error code = 0x" << hr;
			outstr = tmpstrm.str();
			DebugPrint(__FUNCTION__, outstr);

			// relese hook to g_pSession events
			AtlUnadvise (g_pSessMgr, __uuidof(ISessionCOMManagerEvents), g_pSinkCookie);
			g_pSessMgr.Release();

			success = false;
		}
	}
	else
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Unable to instantiate Session Manager. Error code = 0x" << hr;
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);

		success = false;
	}
	return success;
}

/// <summary>
/// Instruct Spokes object to disconnect from Spokes runtime engine and unregister its
/// g_pSession in Spokes.
/// </summary>
void Spokes::Disconnect()
{
	// detaching all COM events and releasing interfaces
	DebugPrint(__FUNCTION__, "Detaching client from Spokes g_pSession manager");
	// detach from active device
	DetachDevice();

	if( g_pCallEvents != NULL )
	{
		// relese hook to g_pSession events
		AtlUnadvise (g_pCallEvents, __uuidof(ICOMCallEvents), g_pSinkSessionCookie);
		g_pCallEvents.Release();
	}
	// release COM interfaces
	g_pSessMgr->UnRegister(g_pSession);
	g_pCallCommand.Release();
	g_pSession.Release();

	// relese hook to g_pSession events
	AtlUnadvise (g_pSessMgr, __uuidof(ISessionCOMManagerEvents), g_pSinkCookie);
	g_pSessMgr.Release();

	m_bIsConnected = false;

	::CoUninitialize(); 
}

// hard coded other device caps, beside caller id
void Spokes::UpdateOtherDeviceCapabilities()
{
	// NEW if DeviceCapabilities.csv file exists in your app's current working directory with a list of device
	// features in the following format (one device per line):
	// ProductId,DeviceName,HasProximity,HasMobCallerId,HasMobCallState,HasDocking,HasWearingSensor,HasMultiline,IsWireless
	// Then use those capabilities for current active device
	//

	// Is the m_AllDeviceCapabilities vector populated? And is my device id in there?
	SpokesDeviceCaps myDeviceCapabilities = GetMyDeviceCapabilities();

	if (myDeviceCapabilities.m_strProductId.length()>0)
	{
		// we have found device in the DeviceCapabilities.csv file
		m_SpokesDeviceCapabilities.m_bHasProximity = myDeviceCapabilities.m_bHasProximity;
		m_SpokesDeviceCapabilities.m_bHasMobCallerId = myDeviceCapabilities.m_bHasMobCallerId;
		m_SpokesDeviceCapabilities.m_bHasMobCallState = myDeviceCapabilities.m_bHasMobCallState;
		m_SpokesDeviceCapabilities.m_bHasDocking = myDeviceCapabilities.m_bHasDocking;
		m_SpokesDeviceCapabilities.m_bHasWearingSensor = myDeviceCapabilities.m_bHasWearingSensor;
		m_SpokesDeviceCapabilities.m_bHasMultiline = myDeviceCapabilities.m_bHasMultiline;
		m_SpokesDeviceCapabilities.m_bIsWireless = myDeviceCapabilities.m_bIsWireless;
	}
	else
	{
		// OK, the Spokes Wrapper user maybe doesn't have the DeviceCapabilities.csv file
		// deployed in app's working directory. Falling back to old hard-coded capabilities
		// (which don't cover the whole product range
		DebugPrint(__FUNCTION__, "INFO: Did not find product in DeviceCapabilities.csv or DeviceCapabilities.csv not present for device:");
		DebugPrint(__FUNCTION__, m_strDeviceName);
		DebugPrint(__FUNCTION__, "INFO: Will assume minimum capabilities, unless overridden by hard-coded capabilities in UpdateOtherDeviceCapabilities function.");

		// LC temporarily hard-code some device capabilities
		// e.g. fact that Blackwire C710/C720 do not support proximity, docking and is not wireless
		string devname = m_strDeviceName;
		std::transform(devname.begin(), devname.end(),devname.begin(), ::toupper);
		if (devname.find("BLACKWIRE")>-1)
		{
			m_SpokesDeviceCapabilities.m_bIsWireless = false;
			m_SpokesDeviceCapabilities.m_bHasDocking = false;
			m_SpokesDeviceCapabilities.m_bHasWearingSensor = false;
		}
		if (devname.find("C210")>-1 || devname.find("C220")>-1)
		{
			m_SpokesDeviceCapabilities.m_bIsWireless = false;
			m_SpokesDeviceCapabilities.m_bHasDocking = false;
			m_SpokesDeviceCapabilities.m_bHasWearingSensor = false;
		}
		if (devname.find("C710")>-1 || devname.find("C720")>-1)
		{
			m_SpokesDeviceCapabilities.m_bHasProximity = false;
			m_SpokesDeviceCapabilities.m_bHasMobCallerId = true;
			m_SpokesDeviceCapabilities.m_bHasMobCallState = true;
			m_SpokesDeviceCapabilities.m_bHasWearingSensor = true;
			m_SpokesDeviceCapabilities.m_bHasDocking = false;
			m_SpokesDeviceCapabilities.m_bIsWireless = false;
		}
		// LC new - if using vpro or vlegend then disable docking feature...
		if (devname.find("BT300")>-1)
		{
			m_SpokesDeviceCapabilities.m_bHasDocking = false;
		}
		if (devname.find("SAVI 7")>-1)
		{
			m_SpokesDeviceCapabilities.m_bHasWearingSensor = false;
			m_SpokesDeviceCapabilities.m_bHasMultiline = true;
		}
	}

	NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
}

// now poll for current state (proximity, mobile call status, donned status, mute status)
void Spokes::GetInitialDeviceState()
{
	if (g_pActiveDevice != NULL)
    {
        RegisterForProximity(true);

		GetInitialSoftphoneCallStatus(); // are we on a call?

        GetInitialMobileCallStatus(); // are we on a call?

        GetInitialDonnedStatus(); // are we donned?

        GetInitialMuteStatus();

        RequestAllSerialNumbers();

        GetLastDockedStatus();

        GetActiveAndHeldStates();
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: No device is attached, cannot get initial device state.");
    }
}

void Spokes::GetInitialSoftphoneCallStatus()
{
	Plantronics_UC_Common::ICallManagerState * cms;
	if (SUCCEEDED(g_pSessMgr->get_CallManagerState(&cms)))
	{
		VARIANT_BOOL hasActiveCall = VARIANT_FALSE;

		if (SUCCEEDED(cms->get_HasActiveCall(&hasActiveCall)))
		{
			if (hasActiveCall==VARIANT_TRUE)
			{
				OnCallArgs * oca = new OnCallArgs("", false, SpokesCallState_OnCall);
				Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
			}
		}
  //      // TODO Raise a TT - the ICallInfo interface is NOT exposed via Spokes SDK .NET API!
  //      //Collection<ICallInfo> calls = (Collection<ICall>)m_sessionComManager.CallManagerState.GetCalls;
  //      //DebugPrint(MethodInfo.GetCurrentMethod().Name, "Got Calls");
	}
}

/// <summary>
/// Instruct Spokes to tell us the serial numbers of attached Plantronics device, i.e. headset and base/usb adaptor.
/// </summary>
void Spokes::RequestAllSerialNumbers()
{
	DebugPrint(__FUNCTION__, "Spokes: About to request serial numbers.");
	RequestSingleSerialNumber(Spokes_Base);
	RequestSingleSerialNumber(Spokes_Headset);
}

/// <summary>
/// Instruct Spokes to tell us a serial number of the attached Plantronics device, i.e. headset or base/usb adaptor.
/// </summary>
/// <param name="serialNumberType">Allows you to say if you would like the headset or base/usb adaptor serial number.</param>
void Spokes::RequestSingleSerialNumber(SpokesSerialNumberTypes serialNumberType)
{
	bool success = false;
	VARIANT_BOOL successVar = FALSE;
	HRESULT hr = S_OK;
	DebugPrint(__FUNCTION__, "Spokes: About to request serial number for: " + serialNumberType);
    if (g_pHostCommandExt != NULL)
    {
        switch (serialNumberType)
        {
            case Spokes_Headset:
                g_pHostCommandExt->GetSerialNumber(DeviceType_Headset, &successVar);
				success = SUCCEEDED(hr) && successVar!=FALSE;
                break;
            case Spokes_Base:
                g_pHostCommandExt->GetSerialNumber(DeviceType_Base, &successVar);
				success = SUCCEEDED(hr) && successVar!=FALSE;
                break;
        }
    }
	if (!success)
	{
		DebugPrint(__FUNCTION__, "Spokes: INFO: serial number may not be supported on your device.");
	}
}

/// <summary>
/// Instructs a mobile that is paired with Plantronics device to dial an outbound mobile call.
/// </summary>
/// <param name="numbertodial">The phone number you wish the mobile to call.</param>
void Spokes::DialMobileCall(string numbertodial)
{
	VARIANT_BOOL successVar = FALSE;
	if (g_pAtdCommand != NULL)
    {
		_bstr_t bstr = numbertodial.c_str();
		BSTR num = bstr.Detach();
        g_pAtdCommand->MakeMobileCall(num, &successVar);
		SysFreeString(num); 
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: Error, unable to dial mobile call. atd command is null.");
    }
}

/// <summary>
/// Instructs a mobile that is paired with Plantronics device to answer an inbound (ringing) mobile call
/// </summary>
void Spokes::AnswerMobileCall()
{           
	VARIANT_BOOL successVar = FALSE;
	if (g_pAtdCommand != NULL)
    {
		g_pAtdCommand->AnswerMobileCall(&successVar);
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: Error, unable to answer mobile call. atd command is null.");
    }
}

/// <summary>
/// Instructs a mobile that is paired with Plantronics device to end on ongoing mobile call
/// </summary>
void Spokes::EndMobileCall()
{
	VARIANT_BOOL successVar = FALSE;
	if (g_pAtdCommand != NULL)
    {
        g_pAtdCommand->EndMobileCall(&successVar);
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: Error, unable to end mobile call. atd command is null.");
    }
}

/// <summary>
/// Allows your softphone application to inform Plantronics device about an incoming call. The Plantronics device will then automatically ring. 
/// Note: will automatically open audio/rf link to wireless device.
/// </summary>
/// <param name="callid">A unique numeric identifier for the call that your application and Spokes will use to identify it as.</param>
/// <param name="contactname">Optional caller's contact name that will display on Plantronics display devices, e.g. Calisto P800 and P240 devices.</param>
/// <returns>Boolean indicating if command was issued successfully or not.</returns>
bool Spokes::IncomingCall(int callid, string contactname)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: Incoming call id=" << callid << ", contact=" << contactname;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    bool success = false;
	if (g_pCallCommand != NULL)
    {
		// Create Contact
		CComPtr<IContact> cnt;
		CComObject<CallContact> *contact = CallContact::GetContact(contactname);
		contact->QueryInterface(__uuidof(IContact),(void **) &cnt); 
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 

        success = SUCCEEDED(g_pCallCommand->IncomingCall(call, cnt, Plantronics_UC_Common::RingTone_Unknown, AudioRoute_ToHeadset));
        //ConnectAudioLinkToDevice(true);   // seems this needs to be commented out for internal headset ringtone to sound!
    }
	return success;
}

/// <summary>
/// Allows your softphone application to inform Plantronics device about an incoming call. The Plantronics device will then automatically ring. 
/// Note: will automatically open audio/rf link to wireless device.
/// </summary>
/// <param name="callid">A unique numeric identifier for the call that your application and Spokes will use to identify it as.</param>
/// <param name="contactname">Optional caller's contact name that will display on Plantronics display devices, e.g. Calisto P800 and P240 devices.</param>
/// <returns>Boolean indicating if command was issued successfully or not.</returns>
bool Spokes::AnswerCall(int callid)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: AnswerCall call id=" << callid;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    bool success = false;
	if (g_pCallCommand != NULL)
    {
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 

        success = SUCCEEDED(g_pCallCommand->AnsweredCall(call));
    }
	return success;
}

/// <summary>
/// Informs Spokes that your softphone user has resumed the given softphone call.
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you resumed.</param>
bool Spokes::ResumeCall(int callid)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: ResumeCall call id=" << callid;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    bool success = false;
	if (g_pCallCommand != NULL)
    {
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 

		success = SUCCEEDED(g_pCallCommand->ResumeCall(call));
    }
	return success;
}

/// <summary>
/// Informs Spokes that your softphone user has held the given softphone call.
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you held.</param>
bool Spokes::HoldCall(int callid)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: HoldCall call id=" << callid;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    bool success = false;
	if (g_pCallCommand != NULL)
    {
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 

		success = SUCCEEDED(g_pCallCommand->HoldCall(call));
    }
	return success;
}

/// <summary>
/// Allows your softphone application to inform Plantronics device about an outgoing call. Note: will automatically open audio/rf link to wireless device.
/// </summary>
/// <param name="callid">A unique numeric identifier for the call that your application and Spokes will use to identify it as.</param>
/// <param name="contactname">Optional caller's contact name that will display on Plantronics display devices, e.g. Calisto P800 and P240 devices.</param>
/// <returns>Boolean indicating if command was issued successfully or not.</returns>
bool Spokes::OutgoingCall(int callid, string contactname)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: Outgoing call id=" << callid << ", contact=" << contactname;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    bool success = false;
    if (g_pCallCommand != NULL)
    {
		// Create Contact
		CComPtr<IContact> cnt;
		CComObject<CallContact> *contact = CallContact::GetContact(contactname);
		contact->QueryInterface(__uuidof(IContact),(void **) &cnt); 
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 

		g_pCallCommand->OutgoingCall(call, cnt, AudioRoute_ToHeadset);

		ConnectAudioLinkToDevice(true);
        success = true;
    }
    return success;
}

/// <summary>
/// Instructs Spokes to end an ongoing softphone call.
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you want to end.</param>
/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
bool Spokes::EndCall(int callid)
{
    bool success = false;
    if (g_pCallCommand != NULL)
    {
		// Create Call
		CComPtr<ICall> call;
		CComObject<Call> *callObj = Call::GetCall(callid);
		callObj->QueryInterface(__uuidof(ICall),(void **) &call); 
        g_pCallCommand->TerminateCall(call);
        success = true;
    }
    return success;
}

/// <summary>
/// This function will establish or close the audio link between PC and the Plantronics audio device.
/// It is required to be called where your app needs audio (i.e. when on a call) in order to support Plantronics wireless devices, because
/// opening the audio link will also bring up the RF link.
/// </summary>
/// <param name="connect">Tells Spokes whether to open or close the audio/rf link to device</param>
bool Spokes::ConnectAudioLinkToDevice(bool connect)
{
	bool success = false;
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: Setting audio link active = " << connect;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
	if (g_pHostCommand != NULL)
	{
		success = SUCCEEDED(g_pHostCommand->put_AudioState(connect ? AudioType_MonoOn : AudioType_MonoOff));
	}
	else
	{
		DebugPrint(__FUNCTION__, "Spokes: INFO: cannot set audio link state, no device");
	}
	return success;
}

/// <summary>
/// Set the microphone mute state of the attached Plantronics device.
/// Note: For wireless devices mute only works when the audio/rf link is active (see also ConnectAudioLinkToDevice method).
/// </summary>
/// <param name="mute">A boolean indicating if you want mute on or off</param>
void Spokes::SetMute(bool mute)
{
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "INFO: Setting mute = " << mute;
	outstr = tmpstrm.str();
	DebugPrint(__FUNCTION__, outstr);
    if (g_pDeviceListener != NULL)
    {
        g_pDeviceListener->put_Mute(mute ? VARIANT_TRUE : VARIANT_FALSE);
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: INFO: cannot set mute, no device");
    }
}

/// <summary>
/// Instruct the Plantronics multiline device to activate or deactivate the specified phone line.
/// </summary>
/// <param name="multiline_LineType">The line to activate or deactive, PC, Mobile or Desk Phone</param>
/// <param name="activate">Boolean indicating whether to activate or de-activate the line</param>
void Spokes::SetLineActive(Spokes_Multiline_LineType multiline_LineType, bool activate)
{
	VARIANT_BOOL successVar = FALSE;
    if (g_pHostCommandExt != NULL)
    {
        switch (multiline_LineType)
        {
			case Spokes_LineType_PC:
				g_pHostCommandExt->SetActiveLink(LineType_VOIP, (activate ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
            case Spokes_LineType_Mobile:
                g_pHostCommandExt->SetActiveLink(LineType_Mobile, (activate ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
            case Spokes_LineType_Deskphone:
                g_pHostCommandExt->SetActiveLink(LineType_PSTN, (activate ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
        }
    }
}

/// <summary>
/// Instruct the Plantronics multiline device to place on hold or remove from hold the specified phone line.
/// </summary>
/// <param name="multiline_LineType">The line to place on hold or remove from hold, PC, Mobile or Desk Phone</param>
/// <param name="hold">Boolean indicating whether to hold or un-hold the line</param>
void Spokes::SetLineHold(Spokes_Multiline_LineType multiline_LineType, bool hold)
{
	VARIANT_BOOL successVar = FALSE;
	if (g_pHostCommandExt != NULL)
    {
        switch (multiline_LineType)
        {
            case Spokes_LineType_PC:
                g_pHostCommandExt->Hold(LineType_VOIP, (hold ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
            case Spokes_LineType_Mobile:
                g_pHostCommandExt->Hold(LineType_Mobile, (hold ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
            case Spokes_LineType_Deskphone:
                g_pHostCommandExt->Hold(LineType_PSTN, (hold ? VARIANT_TRUE : VARIANT_FALSE), &successVar);
                break;
        }
    }
}

// some more private members...

bool Spokes::GetActiveAndHeldStates()
{
	bool success = false;
	bool success1 = false, success2 = false;

    success1 = GetHoldStates();
    success2 = GetActiveStates();
	MultiLineStateArgs * args = new MultiLineStateArgs(m_ActiveHeldFlags);
	NotifyEvent(Spokes_MultiLineStateChanged, args);

	success = success1 && success2;
	if (!success)
	{
        // probably the attached device doesn't have multiline, lets inform user...
        m_SpokesDeviceCapabilities.m_bHasMultiline = false;
        NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		success = false;
    }
	return success;
}

bool Spokes::GetHoldStates()
{
	bool success = false;
	success = m_ActiveHeldFlags.m_bDeskphoneHeld = GetHoldState(LineType_PSTN);
	if (success)
	{
		m_ActiveHeldFlags.m_bMobileHeld = GetHoldState(LineType_Mobile);
		m_ActiveHeldFlags.m_bPCHeld = GetHoldState(LineType_VOIP);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Current Interface Hold States: PSTN: " << m_ActiveHeldFlags.m_bDeskphoneHeld << " Mobile: " << m_ActiveHeldFlags.m_bMobileHeld << " VOIP: " << m_ActiveHeldFlags.m_bPCHeld;
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
	}
	return success;
}

bool Spokes::GetActiveStates()
{
	bool success = false;
	success = m_ActiveHeldFlags.m_bDeskphoneActive = GetActiveState(LineType_PSTN);
	if (success)
	{
		m_ActiveHeldFlags.m_bMobileActive = GetActiveState(LineType_Mobile);
		m_ActiveHeldFlags.m_bPCActive = GetActiveState(LineType_VOIP);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Current Interface Is Line Active States: PSTN: " << m_ActiveHeldFlags.m_bDeskphoneActive << " Mobile: " << m_ActiveHeldFlags.m_bMobileActive<< " VOIP: " << m_ActiveHeldFlags.m_bPCActive;
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
	}
	return success;
}

void Spokes::RegisterForProximity(bool registerForProx)
{
	DebugPrint(__FUNCTION__, "Spokes: About to register for proximity.");
	VARIANT_BOOL successVar = FALSE;
	if (g_pHostCommandExt != NULL)
    {
		HRESULT hr = g_pHostCommandExt->EnableProximity((registerForProx ? TRUE : FALSE), &successVar); // enable proximity reporting for device
		if (SUCCEEDED(hr) && successVar != FALSE)
		{
			if (registerForProx) g_pHostCommandExt->GetProximity(&successVar);    // request to receive asyncrounous near/far proximity event to HeadsetStateChanged event handler.
			DebugPrint(__FUNCTION__, "Spokes: Completed request to register/unregister for proximity.");

			m_SpokesDeviceCapabilities.m_bHasProximity = true;

			// Tweak availability of proximity per-device...
			string devname = m_strDeviceName.c_str();
			std::transform(devname.begin(), devname.end(),devname.begin(), ::toupper);
			if (devname.find("C710")>-1 || devname.find("C720")>-1)
			{
				m_SpokesDeviceCapabilities.m_bHasProximity = false;
			}
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		}
		else
		{
			DebugPrint(__FUNCTION__, "Spokes: INFO: proximity may not be supported on your device.");
			// uh-oh proximity may not be supported... disable it as option in GUI
			m_SpokesDeviceCapabilities.m_bHasProximity = false;
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());

		}
    }
}

// new get last docked status of device when plugin first runs
bool Spokes::GetLastDockedStatus()
{
	if (g_pHostCommandExt != NULL)
    {
		VARIANT_BOOL docked;
		g_pHostCommandExt->get_IsHeadsetDocked(&docked);
		if (docked) wcout << L"@@MULTI-LINE: HEADSET IS DOCKED!!!" << endl;
		else wcout << L"@@MULTI-LINE: HEADSET IS UN-DOCKED!!!" << endl;
		if (docked==TRUE)
		{
			NotifyEvent(Spokes_Docked, EventArgs::Empty());
		}
		else
		{
			NotifyEvent(Spokes_UnDocked, EventArgs::Empty());
		}
    }

	return true;
}

void Spokes::GetInitialMobileCallStatus()
{
	VARIANT_BOOL successVar;
	if (g_pAtdCommand != NULL)
	{
		HRESULT hr = g_pAtdCommand->GetMobileCallStatus(&successVar); // are we on a call?

		if (SUCCEEDED(hr) && successVar != FALSE)
		{
			bool tmpHasCallerId = true; // device does support caller id feature

			// LC temporarily hard-code some device capabilities
			// e.g. fact that Blackwire C710/C720 do not support proximity, docking and is not wireless
			string devname = m_strDeviceName;
			std::transform(devname.begin(), devname.end(),devname.begin(), ::toupper);
			if (devname.find("SAVI 7")>-1)
			{
				tmpHasCallerId = false; // Savi 7xx does not support caller id feature
			}
			if (devname.find("BLACKWIRE")>-1)
			{
				tmpHasCallerId = false; // Blackwire range does not support caller id feature
			}
			if (devname.find("C710")>-1 || devname.find("C720")>-1)
			{
				tmpHasCallerId = false; // Blackwire 700 range does not support caller id feature
			}

			m_SpokesDeviceCapabilities.m_bHasMobCallerId = tmpHasCallerId; // set whether device supports caller id feature
			m_SpokesDeviceCapabilities.m_bHasMobCallState = tmpHasCallerId; // set whether device supports caller id feature
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		}
		else
		{
			DebugPrint(__FUNCTION__, "Spokes: INFO: Problem occured getting mobile call status");
			m_SpokesDeviceCapabilities.m_bHasMobCallerId = false;
			m_SpokesDeviceCapabilities.m_bHasMobCallState = false;
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		}
	}
	else
	{
		DebugPrint(__FUNCTION__, "Spokes: Error, unable to get mobile status. atd command is null.");
		m_SpokesDeviceCapabilities.m_bHasMobCallerId = false; // device does not support caller id feature
		m_SpokesDeviceCapabilities.m_bHasMobCallState = false; // device does not support caller id feature
		NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
	}
}

// new get last donned status of device when plugin first runs
void Spokes::GetInitialDonnedStatus()
{
    if (g_pHostCommandExt != NULL)
    {
        HeadsetState laststate;
		g_pHostCommandExt->get_HeadsetState(&laststate);
        switch (laststate)
        {
            case HeadsetState_Doff:
				NotifyEvent(Spokes_TakenOff, EventArgs::Empty());
                break;
            case HeadsetState_Don:
				NotifyEvent(Spokes_PutOn, EventArgs::Empty());
                break;
        }
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Spokes: Last donned state was: " << ((laststate == HeadsetState_Don) ? "Worn" : "Not Worn");
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
    }
}

void Spokes::GetInitialMuteStatus()
{
    if (g_pHostCommand != NULL)
    {
		VARIANT_BOOL mute;
		g_pHostCommand->get_Mute(&mute);
		MuteChangedArgs * muteargs = new MuteChangedArgs(mute == VARIANT_TRUE);
		NotifyEvent(Spokes_MuteChanged, muteargs);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Spokes: Last mute state was: ", ((mute == VARIANT_TRUE) ? "On" : "Off");
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
    }
}

bool Spokes::GetHoldState(Plantronics_Device_Common::LineType lineType)
{
    bool state = false; // default - unknown state

    //Get the current hold state
    //make sure this is a base device if we can case the BaseHostCommand
    if (g_pHostCommandExt != NULL)
    {
		VARIANT_BOOL bState = FALSE;
        g_pHostCommandExt->GetHoldState(lineType, &bState);
		if (bState) state = true;
    }

    return state;
}

bool Spokes::GetActiveState(Plantronics_Device_Common::LineType lineType)
{
    bool state = false; // default - unknown state

    //Get the current hold state
    //make sure this is a base device if we can case the BaseHostCommand
    if (g_pHostCommandExt != NULL)
    {
		VARIANT_BOOL bState = FALSE;
        g_pHostCommandExt->IsLineActive(lineType, &bState);
		if (bState) state = true;
    }

    return state;
}

/// <summary>
/// Request from Spokes information about the battery level in the attached wireless device.
/// Typically your app will call this after receiving a BatteryLevel headset event.
/// </summary>
/// <returns>A BatteryLevel structure containing information about the battery level.</returns>
BatteryLevel Spokes::GetBatteryLevel()
{
    BatteryLevel level = BatteryLevel_Empty;
    if (g_pActiveDevice != NULL)
    {
        if (g_pHostCommandExt != NULL)
        {
			g_pHostCommandExt->get_BatteryLevel(&level);
        }
    }
    return level;
}

/// <summary>
/// Notify event to application that is registered as spokes event handler...
/// Intended to be called by the Spokes Wrapper itself so it can invoke
/// any overridden event handler functions in the user's application...
/// NOTE: It is the responsibility of the receiving application to delete
/// the memory of e (the EventArgs), using delete statement (delete e)
/// </summary>
/// <param name="e_type">The type of event that is to be notified to user's application.</param>
/// <param name="e">The event arguments (parameters). Maybe be a specialised sub-class of base EventArgs type as required.</param>
void Spokes::NotifyEvent(SpokesEventType e_type, EventArgs * e)
{
	if (m_pSpokesEventsHandler!=NULL)
	{
		switch (e_type)
		{
		case Spokes_TakenOff:
			m_pSpokesEventsHandler->Spokes_TakenOff(e);
			break;
		case Spokes_PutOn:
			m_pSpokesEventsHandler->Spokes_PutOn(e);
			break;
		case Spokes_Near:
			m_pSpokesEventsHandler->Spokes_Near(e);
			break;
		case Spokes_Far:
			m_pSpokesEventsHandler->Spokes_Far(e);
			break;
		case Spokes_InRange:
			m_pSpokesEventsHandler->Spokes_InRange(e);
			break;
		case Spokes_OutOfRange:
			m_pSpokesEventsHandler->Spokes_OutOfRange(e);
			break;
		case Spokes_Docked:
			m_pSpokesEventsHandler->Spokes_Docked(e);
			break;
		case Spokes_UnDocked:
			m_pSpokesEventsHandler->Spokes_UnDocked(e);
			break;
		case Spokes_MobileCallerId:
			m_pSpokesEventsHandler->Spokes_MobileCallerId(e);
			break;
		case Spokes_OnMobileCall:
			m_pSpokesEventsHandler->Spokes_OnMobileCall((OnMobileCallArgs*)e);
			break;
		case Spokes_NotOnMobileCall:
			m_pSpokesEventsHandler->Spokes_NotOnMobileCall(e);
			break;
		case Spokes_SerialNumber:
			m_pSpokesEventsHandler->Spokes_SerialNumber(e);
			break;
		case Spokes_CallAnswered:
			m_pSpokesEventsHandler->Spokes_CallAnswered((CallAnsweredArgs*)e);
			break;
		case Spokes_CallEnded:
			m_pSpokesEventsHandler->Spokes_CallEnded((CallEndedArgs*)e);
			break;
		case Spokes_CallSwitched:
			m_pSpokesEventsHandler->Spokes_CallSwitched(e);
			break;
		case Spokes_OnCall:
			m_pSpokesEventsHandler->Spokes_OnCall((OnCallArgs*)e);
			break;
		case Spokes_NotOnCall:
			m_pSpokesEventsHandler->Spokes_NotOnCall(e);
			break;
		case Spokes_MuteChanged:
			m_pSpokesEventsHandler->Spokes_MuteChanged((MuteChangedArgs*)e);
			break;
		case Spokes_LineActiveChanged:
			m_pSpokesEventsHandler->Spokes_LineActiveChanged((LineActiveChangedArgs*)e);
			break;
		case Spokes_Attached:
			m_pSpokesEventsHandler->Spokes_Attached((AttachedArgs*)e);
			break;
		case Spokes_Detached:
			m_pSpokesEventsHandler->Spokes_Detached(e);
			break;	
		case Spokes_CapabilitiesChanged:
			m_pSpokesEventsHandler->Spokes_CapabilitiesChanged(e);
			break;
		case Spokes_MultiLineStateChanged:
			m_pSpokesEventsHandler->Spokes_MultiLineStateChanged(e);
			break;
		case Spokes_BatteryLevelChanged:
			m_pSpokesEventsHandler->Spokes_BatteryLevelChanged(e);
			break;
		default:
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Invalid event was asked to notified, type number was: " << (int)e_type;
			outstr = tmpstrm.str();
			DebugPrint(__FUNCTION__, outstr);
			break;
		}
	}
}

/// <summary>
/// DebugPrint is used to pass debugging information about Spokes and the
/// device to the connected application's option IDebugLogger interface DebugPrint
/// method.
/// </summary>
/// <param name="methodname">The name of the function that is calling to DebugPrint, in C++ use the __FUNCTION__ macro.</param>
/// <param name="message">The message text to display in this log message.</param>
void Spokes::DebugPrint(string methodname, string message)
{
	if (m_pDebugLog!=NULL)
		m_pDebugLog->DebugPrint(methodname, message);
}

LONG GetStringRegKey(HKEY hKey, const std::wstring &strValueName, std::wstring &strValue, const std::wstring &strDefaultValue)
{
	strValue = strDefaultValue;
	WCHAR szBuffer[512];
	DWORD dwBufferSize = sizeof(szBuffer);
	ULONG nError;
	nError = RegQueryValueExW(hKey, strValueName.c_str(), 0, NULL, (LPBYTE) szBuffer, &dwBufferSize);
	if (ERROR_SUCCESS == nError)
	{
		strValue = szBuffer;
	}
	return nError;
}

bool Spokes::IsSpokesComSessionManagerClassRegistered(int spokesMajorVersion)
{
	bool foundCOMSessionManagerKey = false;

	//DebugPrint(MethodInfo.GetCurrentMethod().Name, "About to look see if Spokes SessionManager is in registry");
	try
	{
		// reg keys of interest...
		string Spokes2xKeyName = "Plantronics.UC.Common.SessionComManager\\CLSID";
		string Spokes3xKeyName = "Plantronics.COMSessionManager\\CLSID";
		wstring Spokes2xSubKeyName = L"{F9E7AE8D-31E2-4968-BA53-3CC5E5A3100A}";
		wstring Spokes3xSubKeyName = L"{750B4A16-1338-4DB0-85BB-C6C89E4CB9AC}";

		// open them all...
		HKEY Spokes2xKey;
		HKEY Spokes3xKey;
		LONG lRes2x = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Plantronics.UC.Common.SessionComManager\\CLSID", 0, KEY_READ, &Spokes2xKey);
		bool b2xExistsAndSuccess(lRes2x == ERROR_SUCCESS);
		bool b2xDoesNotExistsSpecifically(lRes2x == ERROR_FILE_NOT_FOUND);
		LONG lRes3x = RegOpenKeyExW(HKEY_CLASSES_ROOT, L"Plantronics.COMSessionManager\\CLSID", 0, KEY_READ, &Spokes3xKey);
		bool b3xExistsAndSuccess(lRes3x == ERROR_SUCCESS);
		bool b3xDoesNotExistsSpecifically(lRes3x == ERROR_FILE_NOT_FOUND);

		std::wstring str2xKeyDefaultValue;
		GetStringRegKey(Spokes2xKey, L"", str2xKeyDefaultValue, L"bad2x");
		std::wstring str3xKeyDefaultValue;
		GetStringRegKey(Spokes3xKey, L"", str3xKeyDefaultValue, L"bad3x");

		// check if default value == Spokes2xSubKeyName / Spokes3xSubKeyName

		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "About to check if Spokes is installed, Major Version = " << spokesMajorVersion << ".x";
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);

		switch (spokesMajorVersion)
		{
		case 2:
			// is Spokes 2x installed?
			if (b2xExistsAndSuccess)
			{
				// did we find Spokes 2x SessionCOMManager key?
				if (str2xKeyDefaultValue.compare(Spokes2xSubKeyName)==0)
					foundCOMSessionManagerKey = true;
				RegCloseKey(Spokes2xKey);
			}
			break;
		case 3:
			// is Spokes 3x installed?
			if (b3xExistsAndSuccess)
			{
				// did we find Spokes 3x SessionCOMManager key?
				if (str3xKeyDefaultValue.compare(Spokes3xSubKeyName) == 0)
					foundCOMSessionManagerKey = true;
				RegCloseKey(Spokes3xKey);
			}
			break;
		default:
			string outstr;
			ostringstream tmpstrm;
			tmpstrm << "Attempt to check for unknown Spokes Major Version: " << spokesMajorVersion;
			outstr = tmpstrm.str();
			DebugPrint(__FUNCTION__, outstr);
			break;
		}
	}
	catch (exception& e)
	{
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "An exception was caught while looking to see if Spokes SessionManager is in registry.\r\nException = " << e.what();
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
	}

	return foundCOMSessionManagerKey;
}

/// <summary>
/// This method returns a boolean to indicate if the Spokes software runtime is currently installed on the system.
/// If the return value is false then any subsequent attempt to call Spokes.Instance.Connect("My App") will fail
/// because it means that Spokes is not installed so there is no out of proc COM Service for your app to connect to.
/// Note: Is also called by default at start of Connect method, so it is not necessary to call this directly from
/// your app, but you have the option.
/// Note: current version of this function is designed for Spokes 2.x and 3.x. For future major releases would need updating
/// in IsSpokesComSessionManagerClassRegistered private function below.
/// </summary>
bool Spokes::IsSpokesInstalled(int spokesMajorVersion)     // TODO: always insert the CORRECT major version for this Spokes Wrapper version here!
{
	return IsSpokesComSessionManagerClassRegistered(spokesMajorVersion);
}
