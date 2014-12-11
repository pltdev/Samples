#pragma once

#include "..\..\wrappers\SpokesWrapper3.x.h"

class MySpokesIntegration : public IDebugLogger, public ISpokesEvents
{
private:
	Spokes * m_spokes;
public:
	MySpokesIntegration(void);
	~MySpokesIntegration(void);

	virtual void Spokes_TakenOff(EventArgs * e);
	virtual void Spokes_PutOn(EventArgs * e);
	virtual void Spokes_Near(EventArgs * e);
	virtual void Spokes_Far(EventArgs * e);
	virtual void Spokes_InRange(EventArgs * e);
	virtual void Spokes_OutOfRange(EventArgs * e);
	virtual void Spokes_Docked(EventArgs * e);
	virtual void Spokes_UnDocked(EventArgs * e);
	virtual void Spokes_MobileCallerId(EventArgs * e);
	virtual void Spokes_OnMobileCall(EventArgs * e);
	virtual void Spokes_NotOnMobileCall(EventArgs * e);
	virtual void Spokes_SerialNumber(EventArgs * e);
	virtual void Spokes_CallAnswered(EventArgs * e);
	virtual void Spokes_CallEnded(EventArgs * e);
	virtual void Spokes_CallSwitched(EventArgs * e);
	virtual void Spokes_OnCall(EventArgs * e);
	virtual void Spokes_NotOnCall(EventArgs * e);
	virtual void Spokes_MuteChanged(EventArgs * e);
	virtual void Spokes_Attached(AttachedArgs * e);
	virtual void Spokes_Detached(EventArgs * e);
	virtual void Spokes_CapabilitiesChanged(EventArgs * e);
	virtual void Spokes_MultiLineStateChanged(EventArgs * e);
	virtual void Spokes_SystemSuspending(EventArgs * e);
	virtual void Spokes_SystemResuming(EventArgs * e);

	virtual void Spokes_ProximityUnknown(EventArgs * e);
	virtual void Spokes_ProximityEnabled(EventArgs * e);
	virtual void Spokes_ProximityDisabled(EventArgs * e);
	virtual void Spokes_Connected(EventArgs * e);
	virtual void Spokes_Disconnected(EventArgs * e);
	virtual void Spokes_ButtonPressed(ButtonPressArgs * e);
	virtual void Spokes_BaseButtonPressed(BaseButtonPressArgs * e);
	virtual void Spokes_CallRequested(CallRequestedArgs * e);

	virtual void DebugPrint(string methodname, string str);
};

