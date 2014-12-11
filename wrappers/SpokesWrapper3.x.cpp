#include "stdafx.h"
#include "SpokesWrapper3.x.h"

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



// *
// ********************************************************************************
// **************** SPOKES Interface definitions *****************************************
// ********************************************************************************
// *

CComPtr<ICOMSessionManager> g_pSessMgr = nullptr;
CComPtr<ICOMSession> g_pSession = nullptr;
CComPtr<ICOMHostCommand> g_pHostCommand = nullptr;;
CComPtr<ICOMHostCommandExt> g_pHostCommandExt = nullptr;;
CComPtr<ICOMATDCommand> g_pAtdCommand = nullptr;;
CComPtr<ICOMDeviceListener> g_pDeviceListener = nullptr;;
CComPtr<ICOMDeviceEvents> g_pDeviceEvents = nullptr;;
CComPtr<ICOMDevice> g_pActiveDevice = nullptr;;
CComPtr<ICOMCallCommand> g_pCallCommand = nullptr;
CComPtr<ICOMCallEvents> g_pCallEvents = nullptr;;


// *
// ********************************************************************************
// **************** Misc Convenience Functions *****************************************
// ********************************************************************************
// *


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


// *
// ********************************************************************************
// **************** ORIGINAL CallObject.h *****************************************
// ********************************************************************************
// *

class ATL_NO_VTABLE CallObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMCall, &__uuidof(ICOMCall), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:
	CallObject()
	{
	}

	CallObject(long id)
	{
		put_Id( id );
	}

	static CComObject<CallObject>* GetCallObject()
	{
		CComObject<CallObject>* pCall;
		CComObject<CallObject>::CreateInstance(&pCall);	
		pCall->AddRef(); // this object is created with ref count 0;
		std::string strCallId;
		std::cout << "Enter Call Id: ";
		std::getline(std::cin, strCallId);

		long id;
		std::stringstream myStream(strCallId);
		myStream >> id;

		pCall->put_Id( id );
		return pCall;
	}

	// LC new function override takes the call id as a parameter
	// rather than reading it on standard input
	static CComObject<CallObject>* GetCallObject(long id)
	{
		CComObject<CallObject>* pCall;
		CComObject<CallObject>::CreateInstance(&pCall);	
		pCall->AddRef(); // this object is created with ref count 0;
		pCall->put_Id( id );
		return pCall;
	}

	BEGIN_COM_MAP(CallObject)
		COM_INTERFACE_ENTRY2(IDispatch, ICOMCall)
		COM_INTERFACE_ENTRY(ICOMCall)
	END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



private:
	long m_id;

	// ICOMCall Methods
public:
	STDMETHOD(get_Id)(long * pVal)
	{
		*pVal = m_id;
		return S_OK;
	}
	STDMETHOD(put_Id)(long pVal)
	{
		m_id = pVal;
		return S_OK;
	}
	STDMETHOD(get_ConferenceId)(long * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(put_ConferenceId)(long pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(get_InConference)(VARIANT_BOOL * pVal)
	{
		return E_NOTIMPL;
	}
	STDMETHOD(put_InConference)(VARIANT_BOOL pVal)
	{
		return E_NOTIMPL;
	}
};


class ATL_NO_VTABLE CallContact :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<ICOMContact, &__uuidof(ICOMContact), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
private: 
	_bstr_t m_name;
	_bstr_t m_friendName;
	 LONG m_id;
	_bstr_t m_sipUri;
	_bstr_t m_phone;
	_bstr_t m_email;
	_bstr_t m_workPhone;
	_bstr_t m_mobilePhone;
	_bstr_t m_homePhone;
public:
	CallContact()
	{
		static LONG id = 0;
		m_id = ++id;
	}
	~CallContact()
	{
	}
	static CComObject<CallContact>* GetContactObject()
	{
		CComObject<CallContact>* pContact;
		CComObject<CallContact>::CreateInstance(&pContact);	
		pContact->AddRef(); // this object is created with ref count 0;
		std::string name;
		std::cout << "Enter Contact Name: ";
		std::getline(std::cin, name);
		pContact->put_Name( _bstr_t( name.c_str() ) );
		return pContact;
	}
	// LC new function override takes the contact name as a parameter
	// rather than reading it on standard input
	static CComObject<CallContact>* GetContactObject(std::string name)
	{
		CComObject<CallContact>* pContact;
		CComObject<CallContact>::CreateInstance(&pContact);	
		pContact->AddRef(); // this object is created with ref count 0;
		pContact->put_Name( _bstr_t( name.c_str() ) );
		return pContact;
	}

	BEGIN_COM_MAP(CallContact)
		COM_INTERFACE_ENTRY2(IDispatch, ICOMContact)
		COM_INTERFACE_ENTRY(ICOMContact)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:



	// ICOMContact Methods
public:
	STDMETHOD(get_Name)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_name );
		return S_OK;
	}
	STDMETHOD(put_Name)(BSTR pVal)
	{
		m_name = pVal;
		return S_OK;
	}
	STDMETHOD(get_FriendlyName)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_friendName );
		return S_OK;
	}
	STDMETHOD(put_FriendlyName)(BSTR pVal)
	{
		m_friendName = pVal;
		return S_OK;
	}
	STDMETHOD(get_Id)(LONG * pVal)
	{
		*pVal = m_id;
		return S_OK;
	}
	STDMETHOD(put_Id)(LONG pVal)
	{
		m_id = pVal;
		return S_OK;
	}
	STDMETHOD(get_SipUri)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_sipUri );
		return S_OK;
	}
	STDMETHOD(put_SipUri)(BSTR pVal)
	{
		m_sipUri = pVal;
		return S_OK;
	}
	STDMETHOD(get_Phone)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_phone );
		return S_OK;
	}
	STDMETHOD(put_Phone)(BSTR pVal)
	{
		m_phone = pVal;
		return S_OK;
	}
	STDMETHOD(get_Email)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_email );
		return S_OK;
	}
	STDMETHOD(put_Email)(BSTR pVal)
	{
		m_email = pVal;
		return S_OK;
	}
	STDMETHOD(get_WorkPhone)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_workPhone );
		return S_OK;
	}
	STDMETHOD(put_WorkPhone)(BSTR pVal)
	{
		m_workPhone = pVal;
		return S_OK;
	}
	STDMETHOD(get_MobilePhone)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_mobilePhone );
		return S_OK;
	}
	STDMETHOD(put_MobilePhone)(BSTR pVal)
	{
		m_mobilePhone = pVal;
		return S_OK;
	}
	STDMETHOD(get_HomePhone)(BSTR * pVal)
	{
		*pVal = SysAllocString( m_homePhone );
		return S_OK;
	}
	STDMETHOD(put_HomePhone)(BSTR pVal)
	{
		m_homePhone = pVal;
		return S_OK;
	}
};


// *
// ********************************************************************************
// **************** ORIGINAL EnumToString.cpp *****************************************
// ********************************************************************************
// *


void PrintReturn( VARIANT_BOOL val )		{ std::cout << "Returned: " << ((val==VARIANT_TRUE) ? "TRUE " : "FALSE "); } ; 
void PrintReturn( LONG val )				{ std::cout << "Returned: " << (int) val << " "; }
std::string EnumToString(CallRingTone val)
{
	std::string ret;
	switch( val )
	{
		case RingTone_Unknown: ret = "RingTone_Unknown";break;
	}
	return ret;
}
void PrintReturn(CallRingTone val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(CallAudioRoute val)
{
	std::string ret;
	switch( val )
	{
		case AudioRoute_ToHeadset: ret = "AudioRoute_ToHeadset";break;
		case AudioRoute_ToSpeaker: ret = "AudioRoute_ToSpeaker";break;
	}
	return ret;
}
void PrintReturn(CallAudioRoute val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceAudioState val)
{
	std::string ret;
	switch( val )
	{
		case AudioState_Unknown: ret = "AudioState_Unknown";break;
		case AudioState_MonoOn: ret = "AudioState_MonoOn";break;
		case AudioState_MonoOff: ret = "AudioState_MonoOff";break;
		case AudioState_StereoOn: ret = "AudioState_StereoOn";break;
		case AudioState_StereoOff: ret = "AudioState_StereoOff";break;
		case AudioState_MonoOnWait: ret = "AudioState_MonoOnWait";break;
		case AudioState_StereoOnWait: ret = "AudioState_StereoOnWait";break;
	}
	return ret;
}
void PrintReturn(DeviceAudioState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(FirmwareVersionType val)
{
	std::string ret;
	switch( val )
	{
		case VersionType_USBFirmware: ret = "VersionType_USBFirmware";break;
		case VersionType_BaseFirmware: ret = "VersionType_BaseFirmware";break;
		case VersionType_RemoteFirmware: ret = "VersionType_RemoteFirmware";break;
		case VersionType_BluetoothFirmware: ret = "VersionType_BluetoothFirmware";break;
	}
	return ret;
}
void PrintReturn(FirmwareVersionType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceRegistrationType val)
{
	std::string ret;
	switch( val )
	{
		case RegistrationType_SignIn: ret = "RegistrationType_SignIn";break;
		case RegistrationType_SignOut: ret = "RegistrationType_SignOut";break;
		case RegistrationType_Exclusive: ret = "RegistrationType_Exclusive";break;
		case RegistrationType_CallManager: ret = "RegistrationType_CallManager";break;
		case RegistrationType_Attach: ret = "RegistrationType_Attach";break;
		case RegistrationType_Detach: ret = "RegistrationType_Detach";break;
		case RegistrationType_DetachSignOut: ret = "RegistrationType_DetachSignOut";break;
	}
	return ret;
}
void PrintReturn(DeviceRegistrationType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceMuteState val)
{
	std::string ret;
	switch( val )
	{
		case MuteState_None: ret = "MuteState_None";break;
		case MuteState_HeadsetToPC: ret = "MuteState_HeadsetToPC";break;
		case MuteState_PCToHeadset: ret = "MuteState_PCToHeadset";break;
		case MuteState_HeadsetToPSTN: ret = "MuteState_HeadsetToPSTN";break;
		case MuteState_PSTNToHeadset: ret = "MuteState_PSTNToHeadset";break;
		case MuteState_PCToPSTN: ret = "MuteState_PCToPSTN";break;
		case MuteState_PSTNToPC: ret = "MuteState_PSTNToPC";break;
		case MuteState_All: ret = "MuteState_All";break;
	}
	return ret;
}
void PrintReturn(DeviceMuteState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMLineType val)
{
	std::string ret;
	switch( val )
	{
		case LineType_PSTN: ret = "LineType_PSTN";break;
		case LineType_VOIP: ret = "LineType_VOIP";break;
		case LineType_Mobile: ret = "LineType_Mobile";break;
	}
	return ret;
}
void PrintReturn(COMLineType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceAudioLinkState val)
{
	std::string ret;
	switch( val )
	{
		case AudioLinkState_Idle: ret = "AudioLinkState_Idle";break;
		case AudioLinkState_Pending: ret = "AudioLinkState_Pending";break;
		case AudioLinkState_Established: ret = "AudioLinkState_Established";break;
		case AudioLinkState_Failed: ret = "AudioLinkState_Failed";break;
		case AudioLinkState_ReleasePending: ret = "AudioLinkState_ReleasePending";break;
		case AudioLinkState_Lost: ret = "AudioLinkState_Lost";break;
	}
	return ret;
}
void PrintReturn(DeviceAudioLinkState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceAudioLocation val)
{
	std::string ret;
	switch( val )
	{
		case AudioLocation_Headset: ret = "AudioLocation_Headset";break;
		case AudioLocation_Handset: ret = "AudioLocation_Handset";break;
	}
	return ret;
}
void PrintReturn(DeviceAudioLocation val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceChangeState val)
{
	std::string ret;
	switch( val )
	{
		case DeviceState_Unknown: ret = "DeviceState_Unknown";break;
		case DeviceState_Added: ret = "DeviceState_Added";break;
		case DeviceState_Removed: ret = "DeviceState_Removed";break;
	}
	return ret;
}
void PrintReturn(DeviceChangeState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(CallState val)
{
	std::string ret;
	switch( val )
	{
		case CallState_Unknown: ret = "CallState_Unknown";break;
		case CallState_AcceptCall: ret = "CallState_AcceptCall";break;
		case CallState_TerminateCall: ret = "CallState_TerminateCall";break;
		case CallState_HoldCall: ret = "CallState_HoldCall";break;
		case CallState_Resumecall: ret = "CallState_Resumecall";break;
		case CallState_Flash: ret = "CallState_Flash";break;
		case CallState_CallInProgress: ret = "CallState_CallInProgress";break;
		case CallState_CallRinging: ret = "CallState_CallRinging";break;
		case CallState_CallEnded: ret = "CallState_CallEnded";break;
		case CallState_TransferToHeadSet: ret = "CallState_TransferToHeadSet";break;
		case CallState_TransferToSpeaker: ret = "CallState_TransferToSpeaker";break;
		case CallState_MuteON: ret = "CallState_MuteON";break;
		case CallState_MuteOFF: ret = "CallState_MuteOFF";break;
		case CallState_MobileCallRinging: ret = "CallState_MobileCallRinging";break;
		case CallState_MobileCallInProgress: ret = "CallState_MobileCallInProgress";break;
		case CallState_MobileCallEnded: ret = "CallState_MobileCallEnded";break;
		case CallState_Don: ret = "CallState_Don";break;
		case CallState_Doff: ret = "CallState_Doff";break;
		case CallState_CallIdle: ret = "CallState_CallIdle";break;
		case CallState_Play: ret = "CallState_Play";break;
		case CallState_Pause: ret = "CallState_Pause";break;
		case CallState_Stop: ret = "CallState_Stop";break;
		case CallState_DTMFKey: ret = "CallState_DTMFKey";break;
		case CallState_RejectCall: ret = "CallState_RejectCall";break;
	}
	return ret;
}
void PrintReturn(CallState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceEventKind val)
{
	std::string ret;
	switch( val )
	{
		case DeviceEventKind_Docked: ret = "DeviceEventKind_Docked";break;
		case DeviceEventKind_UnDocked: ret = "DeviceEventKind_UnDocked";break;
		case DeviceEventKind_TalkPress: ret = "DeviceEventKind_TalkPress";break;
		case DeviceEventKind_Unknown: ret = "DeviceEventKind_Unknown";break;
	}
	return ret;
}
void PrintReturn(DeviceEventKind val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceHeadsetButton val)
{
	std::string ret;
	switch( val )
	{
		case HeadsetButton_Unknown: ret = "HeadsetButton_Unknown";break;
		case HeadsetButton_VolumeUp: ret = "HeadsetButton_VolumeUp";break;
		case HeadsetButton_VolumeDown: ret = "HeadsetButton_VolumeDown";break;
		case HeadsetButton_VolumeUpHeld: ret = "HeadsetButton_VolumeUpHeld";break;
		case HeadsetButton_VolumeDownHeld: ret = "HeadsetButton_VolumeDownHeld";break;
		case HeadsetButton_Mute: ret = "HeadsetButton_Mute";break;
		case HeadsetButton_MuteHeld: ret = "HeadsetButton_MuteHeld";break;
		case HeadsetButton_Talk: ret = "HeadsetButton_Talk";break;
		case HeadsetButton_Audio: ret = "HeadsetButton_Audio";break;
		case HeadsetButton_Play: ret = "HeadsetButton_Play";break;
		case HeadsetButton_Pause: ret = "HeadsetButton_Pause";break;
		case HeadsetButton_Next: ret = "HeadsetButton_Next";break;
		case HeadsetButton_Previous: ret = "HeadsetButton_Previous";break;
		case HeadsetButton_FastForward: ret = "HeadsetButton_FastForward";break;
		case HeadsetButton_Rewind: ret = "HeadsetButton_Rewind";break;
		case HeadsetButton_Stop: ret = "HeadsetButton_Stop";break;
		case HeadsetButton_Flash: ret = "HeadsetButton_Flash";break;
		case HeadsetButton_Smart: ret = "HeadsetButton_Smart";break;
		case HeadsetButton_OffHook: ret = "HeadsetButton_OffHook";break;
		case HeadsetButton_OnHook: ret = "HeadsetButton_OnHook";break;
		case HeadsetButton_Key0: ret = "HeadsetButton_Key0";break;
		case HeadsetButton_Key1: ret = "HeadsetButton_Key1";break;
		case HeadsetButton_Key2: ret = "HeadsetButton_Key2";break;
		case HeadsetButton_Key3: ret = "HeadsetButton_Key3";break;
		case HeadsetButton_Key4: ret = "HeadsetButton_Key4";break;
		case HeadsetButton_Key5: ret = "HeadsetButton_Key5";break;
		case HeadsetButton_Key6: ret = "HeadsetButton_Key6";break;
		case HeadsetButton_Key7: ret = "HeadsetButton_Key7";break;
		case HeadsetButton_Key8: ret = "HeadsetButton_Key8";break;
		case HeadsetButton_Key9: ret = "HeadsetButton_Key9";break;
		case HeadsetButton_KeyStar: ret = "HeadsetButton_KeyStar";break;
		case HeadsetButton_KeyPound: ret = "HeadsetButton_KeyPound";break;
		case HeadsetButton_Speaker: ret = "HeadsetButton_Speaker";break;
		case HeadsetButton_Reject: ret = "HeadsetButton_Reject";break;
	}
	return ret;
}
void PrintReturn(DeviceHeadsetButton val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceBatteryLevel val)
{
	std::string ret;
	switch( val )
	{
		case BatteryLevel_Empty: ret = "BatteryLevel_Empty";break;
		case BatteryLevel_Low: ret = "BatteryLevel_Low";break;
		case BatteryLevel_Medium: ret = "BatteryLevel_Medium";break;
		case BatteryLevel_High: ret = "BatteryLevel_High";break;
		case BatteryLevel_Full: ret = "BatteryLevel_Full";break;
	}
	return ret;
}
void PrintReturn(DeviceBatteryLevel val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceHeadsetState val)
{
	std::string ret;
	switch( val )
	{
		case HeadsetState_Unknown: ret = "HeadsetState_Unknown";break;
		case HeadsetState_InRange: ret = "HeadsetState_InRange";break;
		case HeadsetState_OutofRange: ret = "HeadsetState_OutofRange";break;
		case HeadsetState_Docked: ret = "HeadsetState_Docked";break;
		case HeadsetState_UnDocked: ret = "HeadsetState_UnDocked";break;
		case HeadsetState_InConference: ret = "HeadsetState_InConference";break;
		case HeadsetState_Don: ret = "HeadsetState_Don";break;
		case HeadsetState_Doff: ret = "HeadsetState_Doff";break;
		case HeadsetState_BTRFPowerMode: ret = "HeadsetState_BTRFPowerMode";break;
		case HeadsetState_DockedCharging: ret = "HeadsetState_DockedCharging";break;
		case HeadsetState_ProductName: ret = "HeadsetState_ProductName";break;
		case HeadsetState_BatteryInfo: ret = "HeadsetState_BatteryInfo";break;
		case HeadsetState_SerialNumber: ret = "HeadsetState_SerialNumber";break;
		case HeadsetState_Proximity: ret = "HeadsetState_Proximity";break;
	}
	return ret;
}
void PrintReturn(DeviceHeadsetState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceProximity val)
{
	std::string ret;
	switch( val )
	{
		case Proximity_ProximityUnknown: ret = "Proximity_ProximityUnknown";break;
		case Proximity_Near: ret = "Proximity_Near";break;
		case Proximity_Far: ret = "Proximity_Far";break;
		case Proximity_ProximityDisabled: ret = "Proximity_ProximityDisabled";break;
		case Proximity_ProximityEnabled: ret = "Proximity_ProximityEnabled";break;
	}
	return ret;
}
void PrintReturn(DeviceProximity val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceBatteryStatus val)
{
	std::string ret;
	switch( val )
	{
		case BTBatteryStatus_Unknown: ret = "BTBatteryStatus_Unknown";break;
		case BTBatteryStatus_Critical: ret = "BTBatteryStatus_Critical";break;
		case BTBatteryStatus_Low: ret = "BTBatteryStatus_Low";break;
		case BTBatteryStatus_Medium: ret = "BTBatteryStatus_Medium";break;
		case BTBatteryStatus_High: ret = "BTBatteryStatus_High";break;
		case BTBatteryStatus_Full: ret = "BTBatteryStatus_Full";break;
		case BTBatteryStatus_NotBatteryPowered: ret = "BTBatteryStatus_NotBatteryPowered";break;
	}
	return ret;
}
void PrintReturn(DeviceBatteryStatus val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceChargingStatus val)
{
	std::string ret;
	switch( val )
	{
		case BTChargingStatus_Unknown: ret = "BTChargingStatus_Unknown";break;
		case BTChargingStatus_NotConnected: ret = "BTChargingStatus_NotConnected";break;
		case BTChargingStatus_ConnectedNotCharging: ret = "BTChargingStatus_ConnectedNotCharging";break;
		case BTChargingStatus_ConnectedAndTrickleCharging: ret = "BTChargingStatus_ConnectedAndTrickleCharging";break;
		case BTChargingStatus_ConnectedAndFastCharging: ret = "BTChargingStatus_ConnectedAndFastCharging";break;
		case BTChargingStatus_ConnectedAndChargeError: ret = "BTChargingStatus_ConnectedAndChargeError";break;
		case BTChargingStatus_NotBatteryPowered: ret = "BTChargingStatus_NotBatteryPowered";break;
	}
	return ret;
}
void PrintReturn(DeviceChargingStatus val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceMobileCallState val)
{
	std::string ret;
	switch( val )
	{
		case MobileCallState_Idle: ret = "MobileCallState_Idle";break;
		case MobileCallState_InComing: ret = "MobileCallState_InComing";break;
		case MobileCallState_OutGoing: ret = "MobileCallState_OutGoing";break;
		case MobileCallState_OnCall: ret = "MobileCallState_OnCall";break;
		case MobileCallState_CallerID: ret = "MobileCallState_CallerID";break;
	}
	return ret;
}
void PrintReturn(DeviceMobileCallState val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(BaseEventTypeExt val)
{
	std::string ret;
	switch( val )
	{
		case BaseEventTypeExt_Unknown: ret = "BaseEventTypeExt_Unknown";break;
		case BaseEventTypeExt_FeatureMask: ret = "BaseEventTypeExt_FeatureMask";break;
		case BaseEventTypeExt_Password: ret = "BaseEventTypeExt_Password";break;
		case BaseEventTypeExt_RFPowerMode: ret = "BaseEventTypeExt_RFPowerMode";break;
		case BaseEventTypeExt_RFLinkType: ret = "BaseEventTypeExt_RFLinkType";break;
		case BaseEventTypeExt_VoipTalk: ret = "BaseEventTypeExt_VoipTalk";break;
		case BaseEventTypeExt_PstnTalk: ret = "BaseEventTypeExt_PstnTalk";break;
		case BaseEventTypeExt_MobileTalk: ret = "BaseEventTypeExt_MobileTalk";break;
		case BaseEventTypeExt_VoipTalkHeld: ret = "BaseEventTypeExt_VoipTalkHeld";break;
		case BaseEventTypeExt_PstnTalkHeld: ret = "BaseEventTypeExt_PstnTalkHeld";break;
		case BaseEventTypeExt_MobileTalkHeld: ret = "BaseEventTypeExt_MobileTalkHeld";break;
		case BaseEventTypeExt_PstnLinkEstablished: ret = "BaseEventTypeExt_PstnLinkEstablished";break;
		case BaseEventTypeExt_PstnLinkDown: ret = "BaseEventTypeExt_PstnLinkDown";break;
		case BaseEventTypeExt_VoipLinkEstablished: ret = "BaseEventTypeExt_VoipLinkEstablished";break;
		case BaseEventTypeExt_VoipLinkDown: ret = "BaseEventTypeExt_VoipLinkDown";break;
		case BaseEventTypeExt_MobileLinkEstablished: ret = "BaseEventTypeExt_MobileLinkEstablished";break;
		case BaseEventTypeExt_MobileLinkDown: ret = "BaseEventTypeExt_MobileLinkDown";break;
		case BaseEventTypeExt_PstnInComingCallRingOn: ret = "BaseEventTypeExt_PstnInComingCallRingOn";break;
		case BaseEventTypeExt_PstnInComingCallRingOff: ret = "BaseEventTypeExt_PstnInComingCallRingOff";break;
		case BaseEventTypeExt_InterfaceStateChange: ret = "BaseEventTypeExt_InterfaceStateChange";break;
		case BaseEventTypeExt_PstnTalkAndVoipTalkHeld: ret = "BaseEventTypeExt_PstnTalkAndVoipTalkHeld";break;
		case BaseEventTypeExt_PstnTalkAndMobileTalkHeld: ret = "BaseEventTypeExt_PstnTalkAndMobileTalkHeld";break;
		case BaseEventTypeExt_VoipTalkAndMobileTalkHeld: ret = "BaseEventTypeExt_VoipTalkAndMobileTalkHeld";break;
		case BaseEventTypeExt_PstnTalkAndSubscribeHeld: ret = "BaseEventTypeExt_PstnTalkAndSubscribeHeld";break;
		case BaseEventTypeExt_Subscribe: ret = "BaseEventTypeExt_Subscribe";break;
		case BaseEventTypeExt_SubscribeHeld: ret = "BaseEventTypeExt_SubscribeHeld";break;
		case BaseEventTypeExt_DialPad: ret = "BaseEventTypeExt_DialPad";break;
		case BaseEventTypeExt_BTAudioLocation: ret = "BaseEventTypeExt_BTAudioLocation";break;
		case BaseEventTypeExt_VoipTalkAndSubscribeHeld: ret = "BaseEventTypeExt_VoipTalkAndSubscribeHeld";break;
		case BaseEventTypeExt_MobileTalkAndSubscribeHeld: ret = "BaseEventTypeExt_MobileTalkAndSubscribeHeld";break;
		case BaseEventTypeExt_SerialNumber: ret = "BaseEventTypeExt_SerialNumber";break;
		case BaseEventTypeExt_DeskphoneHeadset: ret = "BaseEventTypeExt_DeskphoneHeadset";break;
	}
	return ret;
}
void PrintReturn(BaseEventTypeExt val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceFeatureLock val)
{
	std::string ret;
	switch( val )
	{
		case FeatureLock_None: ret = "FeatureLock_None";break;
		case FeatureLock_Intellistand: ret = "FeatureLock_Intellistand";break;
		case FeatureLock_DefaultLine: ret = "FeatureLock_DefaultLine";break;
		case FeatureLock_OTA: ret = "FeatureLock_OTA";break;
		case FeatureLock_PowerLevel: ret = "FeatureLock_PowerLevel";break;
		case FeatureLock_PSTNBandwidth: ret = "FeatureLock_PSTNBandwidth";break;
		case FeatureLock_VOIPBandwidth: ret = "FeatureLock_VOIPBandwidth";break;
		case FeatureLock_PSTNRingType: ret = "FeatureLock_PSTNRingType";break;
		case FeatureLock_VOIPRingType: ret = "FeatureLock_VOIPRingType";break;
		case FeatureLock_PSTNRingTone: ret = "FeatureLock_PSTNRingTone";break;
		case FeatureLock_VOIPRingTone: ret = "FeatureLock_VOIPRingTone";break;
		case FeatureLock_SystemTone: ret = "FeatureLock_SystemTone";break;
		case FeatureLock_MuteTone: ret = "FeatureLock_MuteTone";break;
		case FeatureLock_PSTNToneControl: ret = "FeatureLock_PSTNToneControl";break;
		case FeatureLock_VOIPToneControl: ret = "FeatureLock_VOIPToneControl";break;
		case FeatureLock_RingInCall: ret = "FeatureLock_RingInCall";break;
		case FeatureLock_AlwaysOn: ret = "FeatureLock_AlwaysOn";break;
		case FeatureLock_AntiStartle: ret = "FeatureLock_AntiStartle";break;
		case FeatureLock_AudioLimit: ret = "FeatureLock_AudioLimit";break;
		case FeatureLock_G616: ret = "FeatureLock_G616";break;
		case FeatureLock_TWAPeriod: ret = "FeatureLock_TWAPeriod";break;
		case FeatureLock_MobileBandwidth: ret = "FeatureLock_MobileBandwidth";break;
		case FeatureLock_BTEnable: ret = "FeatureLock_BTEnable";break;
		case FeatureLock_BTAutoConnect: ret = "FeatureLock_BTAutoConnect";break;
		case FeatureLock_BTACLPolling: ret = "FeatureLock_BTACLPolling";break;
		case FeatureLock_BTVoiceCommand: ret = "FeatureLock_BTVoiceCommand";break;
		case FeatureLock_BTA2DP: ret = "FeatureLock_BTA2DP";break;
		case FeatureLock_All: ret = "FeatureLock_All";break;
	}
	return ret;
}
void PrintReturn(DeviceFeatureLock val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceHeadsetType val)
{
	std::string ret;
	switch( val )
	{
		case HeadsetType_Unknown: ret = "HeadsetType_Unknown";break;
		case HeadsetType_Theo428: ret = "HeadsetType_Theo428";break;
		case HeadsetType_Eros428: ret = "HeadsetType_Eros428";break;
		case HeadsetType_Helios480Monoaural: ret = "HeadsetType_Helios480Monoaural";break;
		case HeadsetType_Helios480Binaural: ret = "HeadsetType_Helios480Binaural";break;
		case HeadsetType_Theo480: ret = "HeadsetType_Theo480";break;
		case HeadsetType_Eros480: ret = "HeadsetType_Eros480";break;
		case HeadsetType_Unknown480: ret = "HeadsetType_Unknown480";break;
		case HeadsetType_Hermit480: ret = "HeadsetType_Hermit480";break;
		case HeadsetType_HermitLite: ret = "HeadsetType_HermitLite";break;
		case HeadsetType_Hermit4804MM: ret = "HeadsetType_Hermit4804MM";break;
		case HeadsetType_HermitLite4MM: ret = "HeadsetType_HermitLite4MM";break;
		case HeadsetType_Unknown428ROM: ret = "HeadsetType_Unknown428ROM";break;
		case HeadsetType_Theo428ROM: ret = "HeadsetType_Theo428ROM";break;
		case HeadsetType_Eros428ROM: ret = "HeadsetType_Eros428ROM";break;
		case HeadsetType_HeliosMonaural480ROM: ret = "HeadsetType_HeliosMonaural480ROM";break;
		case HeadsetType_HeliosBinaural480ROM: ret = "HeadsetType_HeliosBinaural480ROM";break;
		case HeadsetType_Theo480ROM: ret = "HeadsetType_Theo480ROM";break;
		case HeadsetType_Eros480ROM: ret = "HeadsetType_Eros480ROM";break;
		case HeadsetType_Unknown480ROM: ret = "HeadsetType_Unknown480ROM";break;
		case HeadsetType_Hermit480ROM: ret = "HeadsetType_Hermit480ROM";break;
		case HeadsetType_HermitLite480ROM: ret = "HeadsetType_HermitLite480ROM";break;
		case HeadsetType_Hermit4MM480ROM: ret = "HeadsetType_Hermit4MM480ROM";break;
		case HeadsetType_HermitLite4MM480ROM: ret = "HeadsetType_HermitLite4MM480ROM";break;
	}
	return ret;
}
void PrintReturn(DeviceHeadsetType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMDeviceType val)
{
	std::string ret;
	switch( val )
	{
		case DeviceType_Base: ret = "DeviceType_Base";break;
		case DeviceType_Headset: ret = "DeviceType_Headset";break;
	}
	return ret;
}
void PrintReturn(COMDeviceType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMRingTone val)
{
	std::string ret;
	switch( val )
	{
		case RingTone_Type1: ret = "RingTone_Type1";break;
		case RingTone_Type2: ret = "RingTone_Type2";break;
		case RingTone_Type3: ret = "RingTone_Type3";break;
	}
	return ret;
}
void PrintReturn(COMRingTone val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMAudioBandwidth val)
{
	std::string ret;
	switch( val )
	{
		case AudioBandwidth_Unknown: ret = "AudioBandwidth_Unknown";break;
		case AudioBandwidth_NarrowBand: ret = "AudioBandwidth_NarrowBand";break;
		case AudioBandwidth_WideBand: ret = "AudioBandwidth_WideBand";break;
	}
	return ret;
}
void PrintReturn(COMAudioBandwidth val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMVolumeLevel val)
{
	std::string ret;
	switch( val )
	{
		case VolumeLevel_Off: ret = "VolumeLevel_Off";break;
		case VolumeLevel_Low: ret = "VolumeLevel_Low";break;
		case VolumeLevel_Standard: ret = "VolumeLevel_Standard";break;
	}
	return ret;
}
void PrintReturn(COMVolumeLevel val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMToneLevel val)
{
	std::string ret;
	switch( val )
	{
		case ToneLevel_MaxBass: ret = "ToneLevel_MaxBass";break;
		case ToneLevel_MidBass: ret = "ToneLevel_MidBass";break;
		case ToneLevel_MinBass: ret = "ToneLevel_MinBass";break;
		case ToneLevel_NoBoost: ret = "ToneLevel_NoBoost";break;
		case ToneLevel_MinTreble: ret = "ToneLevel_MinTreble";break;
		case ToneLevel_MidTreble: ret = "ToneLevel_MidTreble";break;
		case ToneLevel_MaxTreble: ret = "ToneLevel_MaxTreble";break;
	}
	return ret;
}
void PrintReturn(COMToneLevel val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMActiveCallRing val)
{
	std::string ret;
	switch( val )
	{
		case ActiveCallRing_Ignore: ret = "ActiveCallRing_Ignore";break;
		case ActiveCallRing_RingOnce: ret = "ActiveCallRing_RingOnce";break;
		case ActiveCallRing_RingContinuous: ret = "ActiveCallRing_RingContinuous";break;
	}
	return ret;
}
void PrintReturn(COMActiveCallRing val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMAudioLimit val)
{
	std::string ret;
	switch( val )
	{
		case AudioLimit_Off: ret = "AudioLimit_Off";break;
		case AudioLimit_EightyFive: ret = "AudioLimit_EightyFive";break;
		case AudioLimit_Eighty: ret = "AudioLimit_Eighty";break;
	}
	return ret;
}
void PrintReturn(COMAudioLimit val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMPowerLevel val)
{
	std::string ret;
	switch( val )
	{
		case PowerLevel_FixedLow: ret = "PowerLevel_FixedLow";break;
		case PowerLevel_AdaptiveMedium: ret = "PowerLevel_AdaptiveMedium";break;
		case PowerLevel_AdaptiveHigh: ret = "PowerLevel_AdaptiveHigh";break;
	}
	return ret;
}
void PrintReturn(COMPowerLevel val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DSPTWAPeriod val)
{
	std::string ret;
	switch( val )
	{
		case TWAPeriod_Unknown: ret = "TWAPeriod_Unknown";break;
		case TWAPeriod_TwoHours: ret = "TWAPeriod_TwoHours";break;
		case TWAPeriod_FourHours: ret = "TWAPeriod_FourHours";break;
		case TWAPeriod_SixHours: ret = "TWAPeriod_SixHours";break;
		case TWAPeriod_EightHours: ret = "TWAPeriod_EightHours";break;
	}
	return ret;
}
void PrintReturn(DSPTWAPeriod val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceSensorControl val)
{
	std::string ret;
	switch( val )
	{
		case SensorControl_HeadsetNotConnected: ret = "SensorControl_HeadsetNotConnected";break;
		case SensorControl_Undefined: ret = "SensorControl_Undefined";break;
		case SensorControl_Disabled: ret = "SensorControl_Disabled";break;
		case SensorControl_Enabled: ret = "SensorControl_Enabled";break;
	}
	return ret;
}
void PrintReturn(DeviceSensorControl val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(COMDeviceEventType val)
{
	std::string ret;
	switch( val )
	{
		case DeviceEventType_Unknown: ret = "DeviceEventType_Unknown";break;
		case DeviceEventType_HeadsetButtonPressed: ret = "DeviceEventType_HeadsetButtonPressed";break;
		case DeviceEventType_HeadsetStateChanged: ret = "DeviceEventType_HeadsetStateChanged";break;
		case DeviceEventType_BaseButtonPressed: ret = "DeviceEventType_BaseButtonPressed";break;
		case DeviceEventType_BaseStateChanged: ret = "DeviceEventType_BaseStateChanged";break;
		case DeviceEventType_ATDButtonPressed: ret = "DeviceEventType_ATDButtonPressed";break;
		case DeviceEventType_ATDStateChanged: ret = "DeviceEventType_ATDStateChanged";break;
	}
	return ret;
}
void PrintReturn(COMDeviceEventType val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceHeadsetStateChange val)
{
	std::string ret;
	switch( val )
	{
		case HeadsetStateChange_Unknown: ret = "HeadsetStateChange_Unknown";break;
		case HeadsetStateChange_MonoON: ret = "HeadsetStateChange_MonoON";break;
		case HeadsetStateChange_MonoOFF: ret = "HeadsetStateChange_MonoOFF";break;
		case HeadsetStateChange_StereoON: ret = "HeadsetStateChange_StereoON";break;
		case HeadsetStateChange_StereoOFF: ret = "HeadsetStateChange_StereoOFF";break;
		case HeadsetStateChange_MuteON: ret = "HeadsetStateChange_MuteON";break;
		case HeadsetStateChange_MuteOFF: ret = "HeadsetStateChange_MuteOFF";break;
		case HeadsetStateChange_BatteryLevel: ret = "HeadsetStateChange_BatteryLevel";break;
		case HeadsetStateChange_InRange: ret = "HeadsetStateChange_InRange";break;
		case HeadsetStateChange_OutofRange: ret = "HeadsetStateChange_OutofRange";break;
		case HeadsetStateChange_Docked: ret = "HeadsetStateChange_Docked";break;
		case HeadsetStateChange_UnDocked: ret = "HeadsetStateChange_UnDocked";break;
		case HeadsetStateChange_InConference: ret = "HeadsetStateChange_InConference";break;
		case HeadsetStateChange_Don: ret = "HeadsetStateChange_Don";break;
		case HeadsetStateChange_Doff: ret = "HeadsetStateChange_Doff";break;
		case HeadsetStateChange_SerialNumber: ret = "HeadsetStateChange_SerialNumber";break;
		case HeadsetStateChange_Near: ret = "HeadsetStateChange_Near";break;
		case HeadsetStateChange_Far: ret = "HeadsetStateChange_Far";break;
		case HeadsetStateChange_DockedCharging: ret = "HeadsetStateChange_DockedCharging";break;
		case HeadsetStateChange_ProximityUnknown: ret = "HeadsetStateChange_ProximityUnknown";break;
		case HeadsetStateChange_ProximityEnabled: ret = "HeadsetStateChange_ProximityEnabled";break;
		case HeadsetStateChange_ProximityDisabled: ret = "HeadsetStateChange_ProximityDisabled";break;
	}
	return ret;
}
void PrintReturn(DeviceHeadsetStateChange val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceBaseButton val)
{
	std::string ret;
	switch( val )
	{
		case BaseButton_Unknown: ret = "BaseButton_Unknown";break;
		case BaseButton_PstnTalk: ret = "BaseButton_PstnTalk";break;
		case BaseButton_VoipTalk: ret = "BaseButton_VoipTalk";break;
		case BaseButton_Subscribe: ret = "BaseButton_Subscribe";break;
		case BaseButton_PstnTalkHeld: ret = "BaseButton_PstnTalkHeld";break;
		case BaseButton_VoipTalkHeld: ret = "BaseButton_VoipTalkHeld";break;
		case BaseButton_SubscribeHeld: ret = "BaseButton_SubscribeHeld";break;
		case BaseButton_PstnTalkAndSubscribeHeld: ret = "BaseButton_PstnTalkAndSubscribeHeld";break;
		case BaseButton_PstnTalkAndVoipTalkHeld: ret = "BaseButton_PstnTalkAndVoipTalkHeld";break;
		case BaseButton_MakeCall: ret = "BaseButton_MakeCall";break;
		case BaseButton_MobileTalk: ret = "BaseButton_MobileTalk";break;
		case BaseButton_MobileTalkHeld: ret = "BaseButton_MobileTalkHeld";break;
		case BaseButton_PstnTalkAndMobileTalkHeld: ret = "BaseButton_PstnTalkAndMobileTalkHeld";break;
		case BaseButton_VoipTalkAndMobileTalkHeld: ret = "BaseButton_VoipTalkAndMobileTalkHeld";break;
		case BaseButton_DialPad: ret = "BaseButton_DialPad";break;
		case BaseButton_MakeCallFromCallLog: ret = "BaseButton_MakeCallFromCallLog";break;
	}
	return ret;
}
void PrintReturn(DeviceBaseButton val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceBaseStateChange val)
{
	std::string ret;
	switch( val )
	{
		case BaseStateChange_Unknown: ret = "BaseStateChange_Unknown";break;
		case BaseStateChange_PstnLinkEstablished: ret = "BaseStateChange_PstnLinkEstablished";break;
		case BaseStateChange_PstnLinkDown: ret = "BaseStateChange_PstnLinkDown";break;
		case BaseStateChange_VoipLinkEstablished: ret = "BaseStateChange_VoipLinkEstablished";break;
		case BaseStateChange_VoipLinkDown: ret = "BaseStateChange_VoipLinkDown";break;
		case BaseStateChange_AudioMixer: ret = "BaseStateChange_AudioMixer";break;
		case BaseStateChange_RFLinkWideBand: ret = "BaseStateChange_RFLinkWideBand";break;
		case BaseStateChange_RFLinkNarrowBand: ret = "BaseStateChange_RFLinkNarrowBand";break;
		case BaseStateChange_MobileLinkEstablished: ret = "BaseStateChange_MobileLinkEstablished";break;
		case BaseStateChange_MobileLinkDown: ret = "BaseStateChange_MobileLinkDown";break;
		case BaseStateChange_InterfaceStateChanged: ret = "BaseStateChange_InterfaceStateChanged";break;
		case BaseStateChange_AudioLocationChanged: ret = "BaseStateChange_AudioLocationChanged";break;
		case BaseStateChange_SerialNumber: ret = "BaseStateChange_SerialNumber";break;
	}
	return ret;
}
void PrintReturn(DeviceBaseStateChange val){ std::cout << "Returned: " << EnumToString(val); }
std::string EnumToString(DeviceATDStateChange val)
{
	std::string ret;
	switch( val )
	{
		case ATDStateChange_Unknown: ret = "ATDStateChange_Unknown";break;
		case ATDStateChange_MobileCallEnded: ret = "ATDStateChange_MobileCallEnded";break;
		case ATDStateChange_MobileInComing: ret = "ATDStateChange_MobileInComing";break;
		case ATDStateChange_MobileOutGoing: ret = "ATDStateChange_MobileOutGoing";break;
		case ATDStateChange_MobileOnCall: ret = "ATDStateChange_MobileOnCall";break;
		case ATDStateChange_PstnInComingCallRingOn: ret = "ATDStateChange_PstnInComingCallRingOn";break;
		case ATDStateChange_PstnInComingCallRingOff: ret = "ATDStateChange_PstnInComingCallRingOff";break;
		case ATDStateChange_DeskphoneHeadset: ret = "ATDStateChange_DeskphoneHeadset";break;
		case ATDStateChange_MobileCallerID: ret = "ATDStateChange_MobileCallerID";break;
	}
	return ret;
}
void PrintReturn(DeviceATDStateChange val){ std::cout << "Returned: " << EnumToString(val); }
void PrintReturn( BSTR val )				{ std::cout << "Returned: " <<  (val != nullptr ? std::string( _bstr_t(val )) : "none") << " "; }
void PrintReturn( ICOMBatteryInfoPtr batt ) 
{
	if( batt == nullptr ) return; 
	SHORT life;
	DeviceChargingStatus chargStat;
	DeviceBatteryStatus battStat;
	batt->get_BatteryStatus(&battStat);
	batt->get_ChargingStatus(&chargStat);
	batt->get_Lifetime(&life);
	std::cout << "Returned: " << "Life:" << life << " Battstatus: " << battStat << " Chargstatus: " << chargStat << " " ; 
}

std::string VariantToString(VARIANT var)
{
	if(var.vt == (VT_ARRAY | VT_UI1))
	{
		std::ostringstream ss;
		long arrStartIndex, arrEndIndex;
		if(SUCCEEDED ( SafeArrayGetLBound(var.parray, 1, &arrStartIndex) ) &&
		   SUCCEEDED ( SafeArrayGetUBound(var.parray, 1, &arrEndIndex) ) )
		{
			byte b;
			for (long currIndex = arrStartIndex; currIndex <= arrEndIndex; currIndex++)
			{
				SafeArrayGetElement(var.parray, &currIndex, &b);
				ss << std::setfill ('0') << std::setw(2) << std::hex << (int) b;
			}
			return ss.str();
		}
	}

	return "VARIANT";
}
void PrintReturn( VARIANT val )		{ std::cout << "Returned: " << VariantToString(val); } ; 

void PrintAudioMixerState(LONG val)
{
	std::cout << "DeviceMuteState(s):" << std::endl;
	if (0 == val )
		std::cout << EnumToString(MuteState_None) << std::endl;
	if (0 != (val & MuteState_HeadsetToPC))
		std::cout << EnumToString(MuteState_HeadsetToPC) << std::endl;
	if (0 != (val & MuteState_PCToHeadset))
		std::cout << EnumToString(MuteState_PCToHeadset) << std::endl;
	if (0 != (val & MuteState_HeadsetToPSTN))
		std::cout << EnumToString(MuteState_HeadsetToPSTN) << std::endl;
	if (0 != (val & MuteState_PSTNToHeadset))
		std::cout << EnumToString(MuteState_PSTNToHeadset) << std::endl;
	if (0 != (val & MuteState_PCToPSTN))
		std::cout << EnumToString(MuteState_PCToPSTN) << std::endl;
	if (0 != (val & MuteState_PSTNToPC))
		std::cout << EnumToString(MuteState_PSTNToPC) << std::endl;
}


// *
// ********************************************************************************
// **************** ORIGINAL EventSink.h *****************************************
// ********************************************************************************
// *

#define PRINT_ERROR(str, res) std::cout << str << "Error Code: " << std::hex << res << std::endl

// Sink for Session Call Events
class SessionEventSink :
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatchImpl<ICOMCallEvents, &__uuidof(ICOMCallEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:	

	BEGIN_COM_MAP(SessionEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMCallEvents), IDispatch)
	END_COM_MAP()

	STDMETHOD(onCallRequested)(struct ICOMCallRequestEventArgs *args);
	STDMETHOD(onCallStateChanged)(struct ICOMCallEventArgs *args);	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
// Sink for Session Manager events
class ManagerEventSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatchImpl<ICOMSessionManagerEvents, &__uuidof(ICOMSessionManagerEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
private:
	typedef void (*DeviceCallback)(void);
	DeviceCallback m_fnAttach, m_fnDetach;
public:	

	BEGIN_COM_MAP(ManagerEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMSessionManagerEvents), IDispatch)
	END_COM_MAP()

	// set functions that need to be called when DeviceStateChanged
	void DeviceStateHandlers(DeviceCallback attach,DeviceCallback detach)
	{
		m_fnAttach = attach;
		m_fnDetach = detach;
	}

	STDMETHOD(onDeviceStateChanged)( struct ICOMStateDeviceEventArgs *args);
	STDMETHOD(onCallStateChanged)(struct ICOMCallEventArgs *args);	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
class DeviceEventSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<ICOMDeviceEvents,			&__uuidof(ICOMDeviceEvents),		&LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0>
{
public:	

	BEGIN_COM_MAP(DeviceEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMDeviceEvents), IDispatch)
	END_COM_MAP()

	// ICOMDeviceEvents callbacks
	STDMETHOD(onDataReceived)(VARIANT *report);
	STDMETHOD(onTalkButtonPressed)(struct ICOMDeviceEventArgs *args);
	STDMETHOD(onButtonPressed)(struct ICOMDeviceEventArgs *args);
	STDMETHOD(onMuteStateChanged)(struct ICOMDeviceEventArgs *args);
	STDMETHOD(onAudioStateChanged)(struct ICOMDeviceEventArgs *args);
	STDMETHOD(onFlashButtonPressed)(struct ICOMDeviceEventArgs *args);
	STDMETHOD(onSmartButtonPressed)(struct ICOMDeviceEventArgs *args);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
class DeviceEventExtSink : 
        public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<ICOMDeviceEventsExt, &__uuidof(ICOMDeviceEventsExt), &LIBID_Spokes3GCOMServerLib , /*wMajor =*/ 3, /*wMinor =*/ 0>
{
public:	

	BEGIN_COM_MAP(DeviceEventExtSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMDeviceEventsExt), IDispatch)
	END_COM_MAP()

	//ICOMDeviceEventsExt callback
	STDMETHOD(onBatteryLevelChanged)(struct ICOMBatteryLevelEventArgs *args);
	STDMETHOD(onHeadsetStateChanged)(struct ICOMHeadsetStateEventArgs *args);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
class DeviceEventMobileSink: 
        public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<ICOMMobilePresenceEvents, &__uuidof(ICOMMobilePresenceEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:	

	BEGIN_COM_MAP(DeviceEventMobileSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMMobilePresenceEvents), IDispatch)
	END_COM_MAP()

	//ICOMMobilePresenceEvents callback
	STDMETHOD(onPresenceChanged)(struct ICOMMobilePresenceEventArgs *args);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
class DeviceEventBaseSink: 
        public CComObjectRootEx<CComSingleThreadModel>,
		public IDispatchImpl<ICOMBaseEvents, &__uuidof(ICOMBaseEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0>
{
public:	

	BEGIN_COM_MAP(DeviceEventBaseSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMBaseEvents), IDispatch)
	END_COM_MAP()

	//ICOMBaseEvents callback
	STDMETHOD(onBaseEventReceived)(struct ICOMBaseEventArgs *args);
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};
// Sink for DeviceListener Events
class DeviceListenerEventSink :
        public CComObjectRootEx<CComSingleThreadModel>,
        public IDispatchImpl<ICOMDeviceListenerEvents, &__uuidof(ICOMDeviceListenerEvents), &LIBID_Spokes3GCOMServerLib, /*wMajor =*/ 3, /*wMinor =*/ 0 >
{
public:	

	BEGIN_COM_MAP(DeviceListenerEventSink)
		COM_INTERFACE_ENTRY(IDispatch)
		COM_INTERFACE_ENTRY_IID(__uuidof(ICOMDeviceListenerEvents), IDispatch)
	END_COM_MAP()

	STDMETHOD(onHeadsetButtonPressed)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onHeadsetStateChanged)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onBaseButtonPressed)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onBaseStateChanged)(struct ICOMDeviceListenerEventArgs *args);
	STDMETHOD(onATDStateChanged)(struct ICOMDeviceListenerEventArgs *args);	
	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}
};

// wrapper to Advise/Unadvise on Session Manager events
class SessionManagerSink
{
private:
	typedef void (*DeviceCallback)(void);
	DeviceCallback m_fnAttach, m_fnDetach;
public:
	SessionManagerSink(CComPtr<ICOMSessionManager> sessionManager)
	{
		m_sink = nullptr;
		m_sessionManager = sessionManager;
		CComObject<ManagerEventSink>::CreateInstance(&m_sink);
		if(m_sessionManager != nullptr && m_sink != nullptr )
		{
			m_sink->AddRef();
			HRESULT hr = AtlAdvise(m_sessionManager, m_sink, __uuidof(ICOMSessionManagerEvents), &m_cookie);
			if( FAILED( hr ) ) PRINT_ERROR( "Advise Session Manager events Error" , hr);
		}
	}
	~SessionManagerSink()
	{
		HRESULT hr = AtlUnadvise(m_sessionManager, __uuidof(ICOMSessionManagerEvents), m_cookie);
		if( FAILED( hr) ) PRINT_ERROR( "Unadvise Session Manager events Error" , hr );
		m_sink->Release();
	}
	// set functions that need to be called when DeviceStateChanged
	void DeviceStateHandlers(DeviceCallback attach,DeviceCallback detach)
	{
		if (m_sink!=nullptr)
		{
			m_sink->DeviceStateHandlers(attach, detach);
		}
	}
private:
	CComObject<ManagerEventSink> *m_sink;
	CComPtr<ICOMSessionManager> m_sessionManager;
	DWORD m_cookie;
};
// wrapper to Advise/Unadvise on Session Call events
class SessionSink
{
public:
	SessionSink(CComPtr<ICOMSession> session)
	{
		if(session != nullptr )
		{
			m_session = session;
			if( SUCCEEDED( CComObject<SessionEventSink>::CreateInstance(&m_sink) ) && m_sink != nullptr )
			{
				m_sink->AddRef();
				HRESULT hr = AtlAdvise(m_session, m_sink, __uuidof(ICOMCallEvents), &m_cookie);
				if( FAILED( hr ) ) PRINT_ERROR( "Advise Session Call event Error" , hr);
			}
		}
	}
	~SessionSink()
	{
		HRESULT hr = AtlUnadvise(m_session, __uuidof(ICOMCallEvents), m_cookie);
		if( FAILED( hr ) ) PRINT_ERROR( "Unadvise Session Call event Error" , hr);
		m_sink->Release();
	}
private:
	CComObject<SessionEventSink> *m_sink;
	CComPtr<ICOMSession> m_session;
	DWORD m_cookie;
};
// wrapper to Advise/Unadvise on Device Listener events
class DeviceListenerSink
{
public:
	DeviceListenerSink( CComPtr<ICOMSession> session)
	{
		if( session != nullptr )
		{
			ICOMDevicePtr device;
			if( SUCCEEDED( session->GetActiveDevice(&device) ) && device != nullptr )
			{
				if( SUCCEEDED( device->get_DeviceListener(&m_deviceListener) ) && m_deviceListener != nullptr )
				{
					if( SUCCEEDED( CComObject<DeviceListenerEventSink>::CreateInstance(&m_sink) ) && m_sink != nullptr )
					{
						m_sink->AddRef();
						HRESULT hr = AtlAdvise(m_deviceListener, m_sink, __uuidof(ICOMDeviceListenerEvents), &m_cookie);
						if( FAILED( hr ) ) PRINT_ERROR( "Advise Device Listener event Error: " , hr);
					}
				}
			}
		}
	}
	~DeviceListenerSink()
	{
		HRESULT hr = AtlUnadvise(m_deviceListener, __uuidof(ICOMDeviceListenerEvents), m_cookie);
		if( FAILED( hr ) ) PRINT_ERROR( "Unadvise Device Listener event Error: " , hr);
		m_sink->Release();
	}
private:
	CComObject<DeviceListenerEventSink> *m_sink;
	ICOMDeviceListenerPtr m_deviceListener;
	DWORD m_cookie;
};
// wrapper to Advise/Unadvise on Device events
class DeviceSink
{
public:
	DeviceSink( CComPtr<ICOMSession> session)
	{
		if( session != nullptr )
		{
			if( SUCCEEDED( session->GetActiveDevice(&m_device) ) && m_device != nullptr )
			{
				// hook to device listener event
				m_devListSink = new DeviceListenerSink( session );

				if( SUCCEEDED( CComObject<DeviceEventSink>::CreateInstance(&m_sink1) ) && m_sink1 != nullptr &&
					SUCCEEDED( CComObject<DeviceEventExtSink>::CreateInstance(&m_sink2) ) && m_sink2 != nullptr &&
					SUCCEEDED( CComObject<DeviceEventMobileSink>::CreateInstance(&m_sink3) ) && m_sink3 != nullptr &&
					SUCCEEDED( CComObject<DeviceEventBaseSink>::CreateInstance(&m_sink4) ) && m_sink4 != nullptr )
				{
					AtlAdvise(m_device, m_sink1, __uuidof(ICOMDeviceEvents), &m_cookie1);
					AtlAdvise(m_device, m_sink2, __uuidof(ICOMDeviceEventsExt), &m_cookie2);
					AtlAdvise(m_device, m_sink3, __uuidof(ICOMMobilePresenceEvents), &m_cookie3);
					AtlAdvise(m_device, m_sink4, __uuidof(ICOMBaseEvents), &m_cookie4);
				}
			}
		}
	}
	~DeviceSink()
	{
		delete m_devListSink;

		AtlUnadvise(m_device, __uuidof(ICOMDeviceEvents), m_cookie1);
		AtlUnadvise(m_device, __uuidof(ICOMDeviceEventsExt), m_cookie2);
		AtlUnadvise(m_device, __uuidof(ICOMMobilePresenceEvents), m_cookie3);
		AtlUnadvise(m_device, __uuidof(ICOMBaseEvents), m_cookie4);
	}
private:
	DeviceListenerSink* m_devListSink;
	CComObject<DeviceEventSink> *m_sink1;
	CComObject<DeviceEventExtSink> *m_sink2;
	CComObject<DeviceEventMobileSink> *m_sink3;
	CComObject<DeviceEventBaseSink> *m_sink4;
	ICOMDevicePtr m_device;
	DWORD m_cookie1;
	DWORD m_cookie2;
	DWORD m_cookie3;
	DWORD m_cookie4;
};


// *
// ********************************************************************************
// **************** ORIGINAL EventSink.cpp *****************************************
// ********************************************************************************
// *


_ATL_FUNC_INFO OnDeviceEvents = { CC_STDCALL, VT_EMPTY, 1, { VT_DISPATCH }};

void PrintCOMCallEventArgs(std::string str, struct ICOMCallEventArgs *callEventArgs )
{
	ICOMCall* call = nullptr;
	CallState callState;
	BSTR source;
	BSTR callerID;
	long callId(0);

	callEventArgs->get_CallSource(&source);
	callEventArgs->get_CallState(&callState);
	callEventArgs->get_CallerIdentity(&callerID);
	if( SUCCEEDED( callEventArgs->get_call(&call) ) && call != nullptr)
		call->get_Id(&callId);

	std::cout << std::endl << str << "Call ID (internal): " << callId << " CallState: " << EnumToString(callState ) << " Source: " << std::string(_bstr_t(source)) << " CallerID: " << std::string(_bstr_t(callerID)); 

	_bstr_t dtmfKey("DTMFKey"), contactPhone("ContactPhone"), contactFN( "ContactFriendlyName"), lineType( "LineType"), direction("Direction");
	BSTR dtmfKeyVal, contactPhoneVal, contactFNVal, lineTypeVal, directionVal;

	if( SUCCEEDED( callEventArgs->GetOptions(dtmfKey, &dtmfKeyVal ) ) && dtmfKeyVal != nullptr)					std::wcout << L" " << dtmfKey << ":" << dtmfKeyVal;
	if( SUCCEEDED( callEventArgs->GetOptions(contactPhone, &contactPhoneVal ) ) && contactPhoneVal != nullptr)	std::wcout << L" " << contactPhone << ":" << contactPhoneVal;
	if( SUCCEEDED( callEventArgs->GetOptions(contactFN, &contactFNVal ) )  && contactFNVal != nullptr)			std::wcout << L" " << contactFN << ":" << contactFNVal;
	if( SUCCEEDED( callEventArgs->GetOptions(lineType, &lineTypeVal ) )  && lineTypeVal != nullptr)				std::wcout << L" " << lineType << ":" << lineTypeVal;
	if( SUCCEEDED( callEventArgs->GetOptions(direction, &directionVal ) )  && directionVal != nullptr)			std::wcout << L" " << direction << ":" << directionVal;
	
}
STDMETHODIMP SessionEventSink::onCallRequested( struct ICOMCallRequestEventArgs *requestArgs)			
{
	std::string contactName;
	ICOMContact* callContact = nullptr;
	requestArgs->get_contact(&callContact);
	if( callContact != nullptr )
	{
		BSTR name;
		callContact->get_Name(&name);
		contactName = (name != nullptr ? _bstr_t( name ) : "none");
	}
	std::cout << std::endl << "Session onCallRequested contact: " << contactName ;

	CallRequestedArgs * cra = new CallRequestedArgs(callContact, contactName);
	Spokes::GetInstance()->NotifyEvent(Spokes_CallRequested, cra);

	return S_OK;
}
STDMETHODIMP SessionEventSink::onCallStateChanged( struct ICOMCallEventArgs *callEventArgs)			
{
	PrintCOMCallEventArgs( "Session onCallStateChanged ", callEventArgs);

	DeviceEventKind eventKind;
	CallState callState;
	callEventArgs->get_DeviceEventKind(&eventKind);
	callEventArgs->get_CallState(&callState);

	CComPtr<ICOMCall> call;
	long callId;
	if( SUCCEEDED (callEventArgs->get_call(&call) ) )
	{
		call->get_Id(&callId);
		// trace call state details

		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "SM Event: EventKind (" << EnumToString(eventKind) << ") Call State (" << EnumToString(callState) << ") Call Id (" << callId << ")";	
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
		case CallState_CallRinging:
            Spokes::GetInstance()->m_bVoipIncoming = true;
            // Getting here indicates user is ON A CALL!
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity detected!");
			if(SUCCEEDED( callEventArgs->get_CallSource(&callsourcebstr) ) )
			{
				if (SysStringLen(callsourcebstr)>0)
 					callsource = ConvertBSTRToMBS(callsourcebstr);
			}
			oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_Ringing);
			Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
            break;
        case CallState_MobileCallRinging:
            Spokes::GetInstance()->m_bMobileIncoming = true;
            // user incoming mobile call
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity detected!");
			omca = new OnMobileCallArgs(Spokes::GetInstance()->m_bMobileIncoming, SpokesMobileCallState_Ringing);
			Spokes::GetInstance()->NotifyEvent(Spokes_OnMobileCall, omca);
            break;
        case CallState_MobileCallInProgress:
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity detected!");
			omca = new OnMobileCallArgs(Spokes::GetInstance()->m_bMobileIncoming, SpokesMobileCallState_OnCall);
			Spokes::GetInstance()->NotifyEvent(Spokes_OnMobileCall, omca);
            break;
        case CallState_AcceptCall:
        case CallState_CallInProgress: 
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Call was answered/in progress!");
			// trigger user's event handler
			if(SUCCEEDED( callEventArgs->get_CallSource(&callsourcebstr) ) )
			{
				if (SysStringLen(callsourcebstr)>0)
					callsource = ConvertBSTRToMBS(callsourcebstr);
			}
			oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_OnCall);
			Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
			caa = new CallAnsweredArgs(callId, callsource);
			Spokes::GetInstance()->NotifyEvent(Spokes_CallAnswered, caa);
            break;
        case CallState_HoldCall:
        case CallState_Resumecall:
        case CallState_TransferToHeadSet:
        case CallState_TransferToSpeaker:
            // Getting here indicates user is ON A CALL!
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity detected!");
			if(SUCCEEDED( callEventArgs->get_CallSource(&callsourcebstr) ) )
			{
				if (SysStringLen(callsourcebstr)>0)
					callsource = ConvertBSTRToMBS(callsourcebstr);
			}
			oca = new OnCallArgs(callsource, Spokes::GetInstance()->m_bVoipIncoming, SpokesCallState_OnCall);
			Spokes::GetInstance()->NotifyEvent(Spokes_OnCall, oca);
            break;
        case CallState_MobileCallEnded:
            Spokes::GetInstance()->m_bMobileIncoming = false;
            // Getting here indicates user HAS FINISHED A CALL!
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Mobile Calling activity ended.");
			Spokes::GetInstance()->NotifyEvent(Spokes_NotOnMobileCall, EventArgs::Empty());
            break;
        case CallState_CallEnded:
        case CallState_CallIdle:
        case CallState_RejectCall:
        case CallState_TerminateCall:
            Spokes::GetInstance()->m_bVoipIncoming = false;
            // Getting here indicates user HAS FINISHED A CALL!
            Spokes::GetInstance()->DebugPrint(__FUNCTION__, "Spokes: Calling activity ended.");
			if(SUCCEEDED( callEventArgs->get_CallSource(&callsourcebstr) ) )
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
STDMETHODIMP ManagerEventSink::onDeviceStateChanged( struct ICOMStateDeviceEventArgs *args)			
{
	DeviceChangeState devState;
	args->get_DeviceState(&devState);
	std::cout << std::endl << "Manager onDeviceStateChanged DeviceChangeState: " << EnumToString( devState );

	// detach from previous device
	m_fnDetach();
	// attach to new device
	m_fnAttach();

	return S_OK;
}
STDMETHODIMP ManagerEventSink::onCallStateChanged(struct ICOMCallEventArgs *callEventArgs)		
{
	PrintCOMCallEventArgs( "Manager onCallStateChanged ", callEventArgs);
	return S_OK;
}

STDMETHODIMP DeviceEventSink::onDataReceived(VARIANT *report)
{
	// uncomment the following line to trace reports received from device
	//std::cout << std::endl << "ICOMDeviceEvents onDataReceived " << VariantToString(*report);

	// TODO - when needed add the raw data event
            //byte[] reportbuf = (byte[])report;
            //RawDataReceivedArgs args = new RawDataReceivedArgs(reportbuf, byteArrayToString(reportbuf));
            //OnRawDataReceived(args);

	return S_OK;
}
STDMETHODIMP DeviceEventSink::onTalkButtonPressed(struct ICOMDeviceEventArgs *args)
{
	std::cout << std::endl << "ICOMDeviceEvents onTalkButtonPressed ";
	return S_OK;
}
STDMETHODIMP DeviceEventSink::onButtonPressed(struct ICOMDeviceEventArgs *args)
{
	// DEBUG
	//DeviceHeadsetButton button;
	//args->get_ButtonPressed(&button);
	//std::cout << std::endl << "ICOMDeviceEvents onButtonPressed button: " << EnumToString( button );

	DeviceHeadsetButton button;
	args->get_ButtonPressed(&button);
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "Device Event: ButtonPressed " << EnumToString(button);	
	outstr = tmpstrm.str();
	Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

	DeviceAudioState deviceState;
	args->get_AudioState(&deviceState);

	VARIANT_BOOL muted = VARIANT_FALSE;
	args->get_mute(&muted);

	ButtonPressArgs * bpa = new ButtonPressArgs(button, deviceState, muted == VARIANT_TRUE);
	Spokes::GetInstance()->NotifyEvent(Spokes_ButtonPressed, bpa);

	return S_OK;
}
STDMETHODIMP DeviceEventSink::onMuteStateChanged(struct ICOMDeviceEventArgs *args)
{
	VARIANT_BOOL muted = VARIANT_FALSE;
	args->get_mute(&muted);
	std::cout << std::endl << "ICOMDeviceEvents onMuteStateChanged mute: " << ( muted ==  VARIANT_TRUE ? "Muted" : "UnMuted" );

	MuteChangedArgs * mca = new MuteChangedArgs(muted == VARIANT_TRUE);
	Spokes::GetInstance()->NotifyEvent(Spokes_MuteChanged, mca);

	return S_OK;
}
STDMETHODIMP DeviceEventSink::onAudioStateChanged(struct ICOMDeviceEventArgs *args)
{
	DeviceAudioState deviceState;
	args->get_AudioState(&deviceState);
	std::cout << std::endl << "ICOMDeviceEvents onAudioStateChanged State: " << EnumToString(deviceState);
	return S_OK;
}
STDMETHODIMP DeviceEventSink::onFlashButtonPressed(struct ICOMDeviceEventArgs *args)
{
	std::cout << std::endl << "ICOMDeviceEvents onFlashButtonPressed ";

	Spokes::GetInstance()->NotifyEvent(Spokes_CallSwitched, EventArgs::Empty());

	return S_OK;
}
STDMETHODIMP DeviceEventSink::onSmartButtonPressed(struct ICOMDeviceEventArgs *args)
{
	std::cout << std::endl << "ICOMDeviceEvents onSmartButtonPressed ";
	return S_OK;
}
STDMETHODIMP DeviceEventExtSink::onBatteryLevelChanged(struct ICOMBatteryLevelEventArgs *args)
{
	DeviceBatteryLevel batteryLevel;
	args->get_BatteryLevel(&batteryLevel);
	std::cout << std::endl << "ICOMDeviceEventsExt onBatteryLevelChanged level: " << EnumToString( batteryLevel );

	Spokes::GetInstance()->NotifyEvent(Spokes_BatteryLevelChanged, EventArgs::Empty()); // tell app to look at battery state

	return S_OK;
}
STDMETHODIMP DeviceEventExtSink::onHeadsetStateChanged(struct ICOMHeadsetStateEventArgs *args)
{
	short numHs;
	BSTR hsName;
	DeviceProximity devProx;
	DeviceHeadsetState hsState;
	args->get_HeadsetName(&hsName);
	args->get_HeadsetState(&hsState);
	args->get_NumHeadsetsInConference( &numHs );
	args->get_Proximity( &devProx );

	SerialNumberArgs * sn;

	std::string wHsName = "<none>";
	if( hsName != nullptr )
	{
		wHsName = _bstr_t(hsName);
		::SysFreeString( hsName );
	}
	std::cout << std::endl << "ICOMDeviceEventsExt onHeadsetStateChanged name: " << wHsName << " state: " << EnumToString(hsState) << " numInConf: " << numHs << " Proximity: " << EnumToString(devProx);
	if(hsState == HeadsetState_SerialNumber)
	{
		VARIANT serNum;
		args->get_SerialNumber(&serNum);
		std::cout << std::endl << "Headset Serial Number received: " + VariantToString(serNum);

		sn = new SerialNumberArgs(VariantToString(serNum), Spokes_Headset);
		Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
	}

	return S_OK;
}
STDMETHODIMP DeviceEventMobileSink::onPresenceChanged(struct ICOMMobilePresenceEventArgs *args)
{
	BSTR callerId = nullptr;
	DeviceMobileCallState callState;
	args->get_MobileCallState( &callState);

	std::string wCallerId;
	if( callerId != nullptr )
	{
		wCallerId = _bstr_t(callerId);
		::SysFreeString( callerId );
	}
	std::cout << std::endl << "ICOMMobilePresenceEvents onPresenceChanged" << EnumToString(callState) << wCallerId;
	return S_OK;
}
STDMETHODIMP DeviceEventBaseSink::onBaseEventReceived(struct ICOMBaseEventArgs *args)
{
	BaseEventTypeExt eventType;
	args->get_EventType(&eventType);

    std::cout << std::endl << "ICOMBaseEvents onBaseEventReceived eventType: " << EnumToString(eventType);

	MultiLineStateArgs * mlsa;
	SerialNumberArgs * sn;

	string outstr;
	ostringstream tmpstrm;

    DeviceFeatureLock devfetlock;
    while (SUCCEEDED(args->get_FeatureLock(&devfetlock)))
    {
	    std::cout << "  featureLock: " << EnumToString(static_cast<DeviceFeatureLock>(devfetlock)) << ",";
    }
	if(eventType == BaseEventTypeExt_SerialNumber)
	{
	}

	switch (eventType)
    {
        case BaseEventTypeExt_Unknown:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: Unknown";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
            break;
        case BaseEventTypeExt_PstnLinkEstablished:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: PstnLinkEstablished";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        case BaseEventTypeExt_PstnLinkDown:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: PstnLinkDown";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
			
			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        case BaseEventTypeExt_VoipLinkEstablished:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: VoipLinkEstablished";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        case BaseEventTypeExt_VoipLinkDown:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: VoipLinkDown";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
            
			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        //case BaseEventTypeExt_AudioMixer:
        //    DebugPrint(MethodInfo.GetCurrentMethod().Name, String.Format("BaseStateChanged: AudioMixer"));
        //    break;
        //case BaseEventTypeExt_RFLinkType.BaseEventTypeExt_RFLinkWideBand:
        //    DebugPrint(MethodInfo.GetCurrentMethod().Name, String.Format("BaseStateChanged: RFLinkWideBand"));
        //    break;
        //case BaseEventTypeExt_RFLinkNarrowBand:
        //    DebugPrint(MethodInfo.GetCurrentMethod().Name, String.Format("BaseStateChanged: RFLinkNarrowBand"));
        //    break;
        case BaseEventTypeExt_MobileLinkEstablished:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: MobileLinkEstablished";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
			
			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        case BaseEventTypeExt_MobileLinkDown:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: MobileLinkDown";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

			Spokes::GetInstance()->GetActiveStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);
            break;
        case BaseEventTypeExt_InterfaceStateChange:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: InterfaceStateChange";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

			Spokes::GetInstance()->GetHoldStates();
			mlsa = new MultiLineStateArgs(Spokes::GetInstance()->m_ActiveHeldFlags);
			Spokes::GetInstance()->NotifyEvent(Spokes_MultiLineStateChanged, mlsa);

        //case BaseEventTypeExt_AudioLocationChanged:
        //    DebugPrint(MethodInfo.GetCurrentMethod().Name, String.Format("BaseStateChanged: AudioLocationChanged"));
        //    break;
        // LC add handler for basestate change serial number
        // We should be able to extract serial number at this point
        case BaseEventTypeExt_SerialNumber:
			outstr = "";
			tmpstrm.clear();
			tmpstrm << "be_BaseEventReceived: SerialNumber";	
			outstr = tmpstrm.str();
			Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

			VARIANT serNum;
			args->get_SerialNumber(&serNum);
			std::cout << std::endl << "Base Serial Number received: " + VariantToString(serNum);

			sn = new SerialNumberArgs(VariantToString(serNum), Spokes_Base);
			Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
            break;
    }

	return S_OK;
}

STDMETHODIMP DeviceListenerEventSink::onHeadsetButtonPressed(struct ICOMDeviceListenerEventArgs *args)
{
	DeviceHeadsetButton btn;
	COMDeviceEventType evType;
	args->get_HeadsetButton( &btn );
	args->get_DeviceEventType(&evType);
	
	std::cout << std::endl << "ICOMDeviceListenerEvents onHeadsetButtonPressed hsButton: " << EnumToString(btn) << " devEventType: " << EnumToString( evType );
	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onHeadsetStateChanged(struct ICOMDeviceListenerEventArgs *args)
{
	// DEBUG
	//DeviceHeadsetStateChange state;
	//COMDeviceEventType evType;
	//args->get_HeadsetStateChange( &state );
	//args->get_DeviceEventType(&evType);
	//
	//std::cout << std::endl << "ICOMDeviceListenerEvents onHeadsetStateChanged stateChanged: " << EnumToString(state) << " devEventType: " << EnumToString( evType );

	DeviceHeadsetStateChange headstateChange;
	COMDeviceEventType evType;
	args->get_HeadsetStateChange(&headstateChange);
	args->get_DeviceEventType(&evType);
	string outstr;
	ostringstream tmpstrm;
	tmpstrm << "Device Listener Event: HeadsetState " << EnumToString(headstateChange);	
	outstr = tmpstrm.str();
	Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

	SerialNumberArgs * sn;
	MuteChangedArgs * mca;
	LineActiveChangedArgs * laca;

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
			sn = new SerialNumberArgs("", Spokes_Base);
			Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
			sn = new SerialNumberArgs("", Spokes_Headset);
			Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
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
		case HeadsetStateChange_ProximityUnknown:
			Spokes::GetInstance()->NotifyEvent(Spokes_ProximityUnknown, EventArgs::Empty());
			break;
		case HeadsetStateChange_ProximityEnabled:
			Spokes::GetInstance()->NotifyEvent(Spokes_ProximityEnabled, EventArgs::Empty());
			break;
		case HeadsetStateChange_ProximityDisabled:
            // Note: intepret this event as that the mobile phone has gone out of Bluetooth
            // range and is no longer paired to the headset.
            // Lock the PC, but immediately re-enable proximity
			Spokes::GetInstance()->NotifyEvent(Spokes_ProximityDisabled, EventArgs::Empty());
			// Immediately re-enable proximity
			Spokes::GetInstance()->RegisterForProximity(true);
			break;
		case HeadsetStateChange_Docked:
			Spokes::GetInstance()->NotifyEvent(Spokes_Docked, EventArgs::Empty());
			break;
		case HeadsetStateChange_UnDocked:
			Spokes::GetInstance()->NotifyEvent(Spokes_UnDocked, EventArgs::Empty());
			break;
#if newDASeries
        // NEW CC events
        case HeadsetStateChange_Connected:
            Spokes::GetInstance()->NotifyEvent(Spokes_Connected, EventArgs::Empty());
            break;
        case HeadsetStateChange_Disconnected:
            Spokes::GetInstance()->NotifyEvent(Spokes_Disconnected, EventArgs::Empty());
            break;
#endif
		case HeadsetStateChange_MuteON:
			mca = new MuteChangedArgs(true);
			Spokes::GetInstance()->NotifyEvent(Spokes_MuteChanged, mca);
		break;
		case HeadsetStateChange_MuteOFF:
			mca = new MuteChangedArgs(false);
			Spokes::GetInstance()->NotifyEvent(Spokes_MuteChanged, mca);
		break;
		case HeadsetStateChange_MonoON:
			laca = new LineActiveChangedArgs(true);
			Spokes::GetInstance()->NotifyEvent(Spokes_LineActiveChanged, laca);
			break;
		case HeadsetStateChange_MonoOFF:
			laca = new LineActiveChangedArgs(false);
			Spokes::GetInstance()->NotifyEvent(Spokes_LineActiveChanged, laca);
			break;
	}

	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onBaseButtonPressed(struct ICOMDeviceListenerEventArgs *args)
{
	DeviceBaseButton btn;
	COMDeviceEventType evType;
    SHORT dialedKey;
	args->get_BaseButton(&btn);
	args->get_DeviceEventType(&evType);
	args->get_DialedKey(&dialedKey);

	std::cout << std::endl << "ICOMDeviceListenerEvents onBaseButtonPressed baseButton: " << EnumToString(btn) << " devEventType: " << EnumToString( evType );
    if (BaseButton_DialPad == btn)
    {
        std::cout << ", dialed key = " << dialedKey;
    }

	BaseButtonPressArgs * bbpa = new BaseButtonPressArgs(btn);
	Spokes::GetInstance()->NotifyEvent(Spokes_BaseButtonPressed, bbpa);

	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onBaseStateChanged(struct ICOMDeviceListenerEventArgs *args)
{
	DeviceBaseStateChange state;
	COMDeviceEventType evType;
	args->get_BaseStateChange(&state);
	args->get_DeviceEventType(&evType);

	std::cout << std::endl << "ICOMDeviceListenerEvents onBaseStateChanged stateChanged: " << EnumToString(state) << " devEventType: " << EnumToString( evType );
	return S_OK;
}
STDMETHODIMP DeviceListenerEventSink::onATDStateChanged(struct ICOMDeviceListenerEventArgs *args)
{
	//DeviceATDStateChange state;
	//COMDeviceEventType evType;
	//args->get_ATDStateChange(&state);
	//args->get_DeviceEventType(&evType);

	//std::cout << std::endl << "ICOMDeviceListenerEvents onATDStateChanged stateChanged: " << EnumToString(state) << " devEventType: " << EnumToString( evType );

		DeviceATDStateChange atdState;
		args->get_ATDStateChange(&atdState); // supporting only Unknown for now
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Device Listener Event: ATDState Unknown";	
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);

		BSTR callerId;
		string callerId_str;
		MobileCallerIdArgs * mca;

		switch (atdState)
		{
		case ATDStateChange_MobileCallerID:
			g_pAtdCommand->get_CallerId(&callerId);
			wcout << L"MOBILE CALLER ID RECEIVED = " << callerId << endl;

			callerId_str = ConvertBSTRToMBS(callerId);

			mca = new MobileCallerIdArgs(callerId_str);

			Spokes::GetInstance()->NotifyEvent(Spokes_MobileCallerId, mca);

			break;
		}
	return S_OK;
}



//Encapsulates event sink defined in EventSink.h
DeviceSink* g_pDeviceEventSink = nullptr;
SessionSink* g_pSessionEventSink = nullptr;
SessionManagerSink* g_pSessionMgrEventsSink = nullptr;
DeviceListenerSink* g_pDeviceListenerSink = nullptr;

//CComObject<SessionManagerSink> * g_pSessionMgrEventsSink = NULL;
//CComObject<SessionSink> * g_pSessionEventSink = NULL;
//CComObject<DeviceEventSink> * g_pDeviceEventSink = NULL;
//CComObject<DeviceListenerEventSink> * g_pDeviceListenerSink = NULL;

// for getting serial numbers...
CComPtr<ICOMBaseEvents> g_pBaseEvents;
CComPtr<ICOMDeviceEventsExt> g_pDeviceEventsExt;

CComObject<DeviceEventExtSink> * g_pDeviceEventExtSink = NULL;
CComObject<DeviceEventBaseSink> * g_pBaseEventSink = NULL;


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
	unsigned short vendorId, productId, versionNum;
	g_pActiveDevice->get_InternalName(&internalName);
	g_pActiveDevice->get_ManufacturerName(&manufacturerName);
	g_pActiveDevice->get_ProductName(&productName);
	g_pActiveDevice->get_VendorId(&vendorId);
	g_pActiveDevice->get_ProductId(&productId);
	g_pActiveDevice->get_VersionNumber(&versionNum);

	if( g_pHostCommand != NULL )
	{
		g_pHostCommand->GetFirmwareVersion(VersionType_USBFirmware, &usbVersion);
		g_pHostCommand->GetFirmwareVersion(VersionType_BaseFirmware, &baseVersion);
		g_pHostCommand->GetFirmwareVersion(VersionType_RemoteFirmware, &remoteFirmware);

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
	BSTR bstrSessionId;
	g_pSession->get_PluginName(&pluginName); 
	g_pSession->get_SessionId(&bstrSessionId);

	// print this data on console
	wcout << L"********************* Session Information ******************** " << endl;
	wcout << L"Plugin Name:     " << pluginName << endl;
	wcout << L"Session ID:      " << bstrSessionId << endl;
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
		if (SUCCEEDED(
			g_pHostCommandExt->EnableProximity(TRUE)))
		{
			if (SUCCEEDED(
				g_pHostCommandExt->RequestProximity()))
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
		g_pSession->GetActiveDevice(&g_pActiveDevice);
	}

	// If we have active device, then get HostCommad, DeviceListener and DeviceEvent
	if( g_pActiveDevice != NULL)
	{
        // LC assume minimum first set of device capabilities...
		Spokes::GetInstance()->m_SpokesDeviceCapabilities.Init(false, false, true, true, false, true);
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
		tmpstrm << "ATTACHED = %s" << csStr;
		outstr = tmpstrm.str();
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		outstr = tmpstrm.str();
		Spokes::GetInstance()->m_strDeviceName = csStr;
		Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
		::SysFreeString(productName);

		g_pHostCommandExt = NULL;

		if (SUCCEEDED(
			g_pActiveDevice->get_HostCommand(&g_pHostCommand)
			))
		{
			// TODO, need to test this with a variety of products
			// to ensure if below interfaces not supported it doesn't error!
			// Cast ICOMHostCommand to ICOMHostCommandExt, ICOMATDCommand, ICOMDeviceSettings, and ICOMAdvanceSettings.
			g_pHostCommandExt = g_pHostCommand;
			// need to get caller ids
			g_pAtdCommand = g_pHostCommand;
			//if (!SUCCEEDED(hRes))
			//	wcout << L"INFORMATION unable to get IID_IHostCommandExt interface (only supported by some products)" << endl;

			g_pActiveDevice->get_DeviceListener(&g_pDeviceListener);
		}

		PrintDevice();
		
		// Hook into Device events. When device is detached, this object need to be recreated.
		g_pDeviceEventSink = new DeviceSink( g_pSession );

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
		if (g_pDeviceEventSink!=nullptr)
		{
			delete g_pDeviceEventSink;
			g_pDeviceEventSink = nullptr;
		}

		g_pHostCommand.Release();
		g_pHostCommandExt.Release();
		g_pAtdCommand.Release();
		g_pDeviceListener.Release();

		// LC Device was disconnected, clear down the GUI state...
        Spokes::GetInstance()->m_bMobileIncoming = false; // clear mobile call direction flag
        Spokes::GetInstance()->m_bVoipIncoming = false; // clear call direction flag
		Spokes::GetInstance()->NotifyEvent(Spokes_NotOnCall, EventArgs::Empty());
		Spokes::GetInstance()->NotifyEvent(Spokes_NotOnMobileCall, EventArgs::Empty());

		SerialNumberArgs * sn = new SerialNumberArgs("", Spokes_Base);
		Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);
		sn = new SerialNumberArgs("", Spokes_Headset);
		Spokes::GetInstance()->NotifyEvent(Spokes_SerialNumber, sn);

        //// LC Device was disconnected, remove capability data
		Spokes::GetInstance()->m_SpokesDeviceCapabilities.Init(false, false, true, true, false, true);
        Spokes::GetInstance()->m_strDeviceName = "";
		Spokes::GetInstance()->NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());

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
	// not sure this is needed any more in Spokes 3.x
	//if( g_pDeviceListener != NULL )
	//{
	//	string outstr;
	//	ostringstream tmpstrm;
	//	tmpstrm << "***Suppressing dial-tone in IDeviceListener." << endl;
	//	outstr = tmpstrm.str();
	//	Spokes::GetInstance()->DebugPrint(__FUNCTION__, outstr);
	//	g_pDeviceListener->SuppressDialTone(true);
	//}
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

bool Spokes::Connect(const char * appName)
{
	if (m_bIsConnected) return true;

    m_SpokesDeviceCapabilities.Init(false, false, false, false, false, false); // we don't yet know what the capabilities are
	NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
    bool success = false;

	// Initialize COM
	::CoInitializeEx(NULL, COINIT_MULTITHREADED);

	DebugPrint(__FUNCTION__, "Connecting to SPOKES session manager");

	//// Create Spokes session manager object
	//if( SUCCEEDED ( sessionManager.CreateInstance( CLSID_COMSessionManager ) ) )
	HRESULT hr = S_OK;
	if (SUCCEEDED(hr = ::CoCreateInstance(CLSID_COMSessionManager, NULL,CLSCTX_LOCAL_SERVER, IID_ICOMSessionManager, (LPVOID*)&g_pSessMgr )))
	{
		// Hook into Session Manager events
		g_pSessionMgrEventsSink = new SessionManagerSink( g_pSessMgr );

		// Register new session in Spokes.
		if( SUCCEEDED (g_pSessMgr->Register( _bstr_t(appName) , &g_pSession) ) )
		{
			// Hook into Session  events
			g_pSessionEventSink = new SessionSink( g_pSession );
				
            ICOMUserPreference *userPref;
            g_pSessMgr->get_UserPreference(&userPref);
            userPref->put_DefaultSoftphone(_bstr_t("COM Plugin"));

			// set function pointers to attach/detach to device when we recieve Device events
			g_pSessionMgrEventsSink->DeviceStateHandlers( AttachDevice, DetachDevice );

			// print basic g_pSession information
			PrintSession();

			g_pSession->GetCallCommand(&g_pCallCommand);

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
			AtlUnadvise (g_pSessMgr, __uuidof(ICOMSessionManagerEvents), g_pSinkCookie);
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
	//// detach from active device
	DetachDevice();

	if (g_pCallCommand!=nullptr)
	{
		g_pCallCommand.Release();
	}

	//// cleanup of global COM message sinks
	if (g_pDeviceEventSink!=nullptr) 
	{
		delete g_pDeviceEventSink;
		g_pDeviceEventSink = nullptr;
	}
	if (g_pSessionEventSink!=nullptr)
	{
		delete g_pSessionEventSink;
		g_pSessionEventSink = nullptr;
	}

	// Unregister our application session with SessionManager
	g_pSessMgr->Unregister( g_pSession );

	// release session interface
	if (g_pSession!=nullptr)
	{
		g_pSession.Release();
	}

	// reelase SessionManager interface
	if (g_pSessionMgrEventsSink!=nullptr)
	{
		delete g_pSessionMgrEventsSink;
		g_pSessionMgrEventsSink = nullptr;
	}
	if (g_pSessMgr!=nullptr)
	{
		g_pSessMgr.Release();
	}

	m_bIsConnected = false;

	::CoUninitialize(); 
}

// hard coded other device caps, beside caller id
void Spokes::UpdateOtherDeviceCapabilities()
{
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
    if (devname.find("C710")>-1 || devname.find("C720")>-1)
    {
        m_SpokesDeviceCapabilities.m_bHasProximity = false;
        m_SpokesDeviceCapabilities.m_bHasCallerId = false;
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

		DeviceAudioState das;
		g_pHostCommand->get_AudioState(&das);
		Spokes::GetInstance()->NotifyEvent(Spokes_LineActiveChanged, 
			new LineActiveChangedArgs(das == AudioState_MonoOn));
    }
    else
    {
        DebugPrint(__FUNCTION__, "Spokes: No device is attached, cannot get initial device state.");
    }
}

// new get last connected status of headset (QD connector) when app first runs
bool Spokes::GetLastConnectedStatus()
{
    bool connected = false;
	bool success = false;
	HRESULT hr = S_OK;
    if (g_pHostCommandExt != nullptr)
    {
        //m_lastconnected = m_hostCommandExt.HeadsetConnected;  TODO interface not yet available!
		success = SUCCEEDED(hr);
        // assume it is connected for now:
        m_lastconnected = true;
        connected = m_lastconnected;
		Spokes::GetInstance()->NotifyEvent(connected ? Spokes_Connected : Spokes_Disconnected, EventArgs::Empty());
    }
    if (!success)
    {
        // probably we don't have QD connector, lets inform user...
		//Spokes::GetInstance()->m_SpokesDeviceCapabilities.m_bHasQDConnector = false;   TODO add this capability to the wrapper, see C# version
		Spokes::GetInstance()->NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
    }
    return connected;
}

void Spokes::GetInitialSoftphoneCallStatus()
{
	ICOMCallManagerState * cms;
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
	VARIANT valVariant;
	HRESULT hr = S_OK;
	DebugPrint(__FUNCTION__, "Spokes: About to request serial number for: " + serialNumberType);
    if (g_pHostCommandExt != NULL)
    {
        switch (serialNumberType)
        {
            case Spokes_Headset:
                g_pHostCommandExt->GetSerialNumber(DeviceType_Headset, &valVariant); //&successVar);
				success = SUCCEEDED(hr);// && successVar!=FALSE;
                break;
            case Spokes_Base:
                g_pHostCommandExt->GetSerialNumber(DeviceType_Base, &valVariant); //&successVar);
				success = SUCCEEDED(hr);// && successVar!=FALSE;
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
        g_pAtdCommand->MakeMobileCall(num); //, &successVar);
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
		g_pAtdCommand->AnswerMobileCall(); //&successVar);
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
        g_pAtdCommand->EndMobileCall(); //&successVar);
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
        success = SUCCEEDED(g_pCallCommand->IncomingCall(
			CallObject::GetCallObject(callid), 
			CallContact::GetContactObject(contactname), 
			RingTone_Unknown, 
			AudioRoute_ToHeadset));
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
		success = SUCCEEDED(g_pCallCommand->AnsweredCall(CallObject::GetCallObject(callid)));
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
		success = SUCCEEDED(g_pCallCommand->OutgoingCall(
			CallObject::GetCallObject(callid), 
			CallContact::GetContactObject(contactname), 
			AudioRoute_ToHeadset));

		if (success)
			ConnectAudioLinkToDevice(true);
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
        success = SUCCEEDED(g_pCallCommand->TerminateCall(CallObject::GetCallObject(callid)));
    }
    return success;
}

/// <summary>
/// Instructs Spokes to hold an ongoing softphone call.
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you want to hold.</param>
/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
bool Spokes::HoldCall(int callid)
{
    bool success = false;
    if (g_pCallCommand != NULL)
    {
		success = SUCCEEDED(g_pCallCommand->HoldCall(CallObject::GetCallObject(callid)));
    }
    return success;
}

/// <summary>
/// Instructs Spokes to resume an ongoing softphone call.
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you want to resume.</param>
/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
bool Spokes::ResumeCall(int callid)
{
    bool success = false;
    if (g_pCallCommand != NULL)
    {
		success = SUCCEEDED(g_pCallCommand->ResumeCall(CallObject::GetCallObject(callid)));
    }
    return success;
}

/// <summary>
/// Instructs Spokes to that an ongoing softphone call is a "conference".
/// </summary>
/// <param name="callid">The unique numeric id that defines which softphone call you want to set as conference.</param>
/// <returns>Boolean indicating if the command was called succesfully or not.</returns>
bool Spokes::SetConferenceId(int callid)
{
    bool success = false;
    if (g_pCallCommand != NULL)
    {
		success = SUCCEEDED(g_pCallCommand->SetConferenceId(CallObject::GetCallObject(callid)));
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
		success = SUCCEEDED(g_pHostCommand->put_AudioState(connect ? AudioState_MonoOn : AudioState_MonoOff));
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
        g_pDeviceListener->put_mute(mute ? VARIANT_TRUE : VARIANT_FALSE);
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
				g_pHostCommandExt->SetActiveLink(LineType_VOIP, (activate ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
                break;
            case Spokes_LineType_Mobile:
                g_pHostCommandExt->SetActiveLink(LineType_Mobile, (activate ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
                break;
            case Spokes_LineType_Deskphone:
                g_pHostCommandExt->SetActiveLink(LineType_PSTN, (activate ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
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
                g_pHostCommandExt->SetLinkHoldState(LineType_VOIP, (hold ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
                break;
            case Spokes_LineType_Mobile:
                g_pHostCommandExt->SetLinkHoldState(LineType_Mobile, (hold ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
                break;
            case Spokes_LineType_Deskphone:
                g_pHostCommandExt->SetLinkHoldState(LineType_PSTN, (hold ? VARIANT_TRUE : VARIANT_FALSE)); //, &successVar);
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
		HRESULT hr = g_pHostCommandExt->EnableProximity((registerForProx ? TRUE : FALSE)); //, &successVar); // enable proximity reporting for device
		if (SUCCEEDED(hr)) // && successVar != FALSE)
		{
			if (registerForProx) g_pHostCommandExt->RequestProximity(); // &successVar);    // request to receive asyncrounous near/far proximity event to HeadsetStateChanged event handler.
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
		g_pHostCommandExt->get_HeadsetDocked(&docked);
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
		HRESULT hr = g_pAtdCommand->RequestMobileCallStatus(); //(&successVar); // are we on a call?

		if (SUCCEEDED(hr)) // && successVar != FALSE)
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

			m_SpokesDeviceCapabilities.m_bHasCallerId = tmpHasCallerId; // set whether device supports caller id feature
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		}
		else
		{
			DebugPrint(__FUNCTION__, "Spokes: INFO: Problem occured getting mobile call status");
			m_SpokesDeviceCapabilities.m_bHasCallerId = false;
			NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
		}
	}
	else
	{
		DebugPrint(__FUNCTION__, "Spokes: Error, unable to get mobile status. atd command is null.");
		m_SpokesDeviceCapabilities.m_bHasCallerId = false; // device does not support caller id feature
		NotifyEvent(Spokes_CapabilitiesChanged, EventArgs::Empty());
	}
}

// new get last donned status of device when plugin first runs
void Spokes::GetInitialDonnedStatus()
{
    if (g_pHostCommandExt != NULL)
    {
        DeviceHeadsetState laststate;
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
		g_pHostCommand->get_mute(&mute);
		MuteChangedArgs * muteargs = new MuteChangedArgs(mute == VARIANT_TRUE);
		NotifyEvent(Spokes_MuteChanged, muteargs);
		string outstr;
		ostringstream tmpstrm;
		tmpstrm << "Spokes: Last mute state was: ", ((mute == VARIANT_TRUE) ? "On" : "Off");
		outstr = tmpstrm.str();
		DebugPrint(__FUNCTION__, outstr);
    }
}

bool Spokes::GetHoldState(COMLineType lineType)
{
    bool state = false; // default - unknown state

    //Get the current hold state
    //make sure this is a base device if we can case the BaseHostCommand
    if (g_pHostCommandExt != NULL)
    {
		VARIANT_BOOL bState = FALSE;
        g_pHostCommandExt->GetLinkHoldState(lineType, &bState);
		if (bState) state = true;
    }

    return state;
}

bool Spokes::GetActiveState(COMLineType lineType)
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
DeviceBatteryLevel Spokes::GetBatteryLevel()
{
    DeviceBatteryLevel level = BatteryLevel_Empty;
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

		case Spokes_ProximityUnknown:
			m_pSpokesEventsHandler->Spokes_ProximityUnknown(e);
			break;
		case Spokes_ProximityEnabled:
			m_pSpokesEventsHandler->Spokes_ProximityEnabled(e);
			break;
		case Spokes_ProximityDisabled:
			m_pSpokesEventsHandler->Spokes_ProximityDisabled(e);
			break;
		case Spokes_Connected:
			m_pSpokesEventsHandler->Spokes_Connected(e);
			break;
		case Spokes_Disconnected:
			m_pSpokesEventsHandler->Spokes_Disconnected(e);
			break;
		case Spokes_ButtonPressed:
			m_pSpokesEventsHandler->Spokes_ButtonPressed((ButtonPressArgs*)e);
			break;
		case Spokes_BaseButtonPressed:
			m_pSpokesEventsHandler->Spokes_BaseButtonPressed((BaseButtonPressArgs*)e);
			break;
		case Spokes_CallRequested:
			m_pSpokesEventsHandler->Spokes_CallRequested((CallRequestedArgs*)e);
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
