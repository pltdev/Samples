#include "stdafx.h"
#include "MySpokesIntegration.h"

MySpokesIntegration::MySpokesIntegration(void)
{
	m_spokes = Spokes::GetInstance();

	m_spokes->SetLogger(this);

	m_spokes->SetEventHandler(this);
}

MySpokesIntegration::~MySpokesIntegration(void)
{
	delete m_spokes;
	m_spokes = NULL;
}

void MySpokesIntegration::Spokes_TakenOff(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Taken Off.");
	delete e;
}

void MySpokesIntegration::Spokes_PutOn(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Put On.");
	delete e;
}

void MySpokesIntegration::Spokes_Near(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Near.");
	delete e;
}

void MySpokesIntegration::Spokes_Far(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Far.");
	delete e;
}

void MySpokesIntegration::Spokes_InRange(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> In Range.");
	delete e;
}

void MySpokesIntegration::Spokes_OutOfRange(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Out of Range.");
	delete e;
}

void MySpokesIntegration::Spokes_Docked(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Docked.");
	delete e;
}

void MySpokesIntegration::Spokes_UnDocked(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> UnDocked.");
	delete e;
}

void MySpokesIntegration::Spokes_MobileCallerId(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Mobile Caller Id.");
	delete e;
}

void MySpokesIntegration::Spokes_OnMobileCall(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> On Mobile Call.");
	delete e;
}

void MySpokesIntegration::Spokes_NotOnMobileCall(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Not On Mobile Call.");
	delete e;
}

void MySpokesIntegration::Spokes_SerialNumber(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Serial Number.");
	delete e;
}

void MySpokesIntegration::Spokes_CallAnswered(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Call Answered.");
	delete e;
}

void MySpokesIntegration::Spokes_CallEnded(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Call Ended.");
	delete e;
}

void MySpokesIntegration::Spokes_CallSwitched(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Call Switched.");
	delete e;
}

void MySpokesIntegration::Spokes_OnCall(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> On Call.");
	delete e;
}

void MySpokesIntegration::Spokes_NotOnCall(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Not On Call.");
	delete e;
}

void MySpokesIntegration::Spokes_MuteChanged(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Mute Changed.");
	delete e;
}

void MySpokesIntegration::Spokes_Attached(AttachedArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Attached.");
	DebugPrint(__FUNCTION__, "Device was: ");
	DebugPrint(__FUNCTION__, e->m_strDeviceName);
	delete e;
}

void MySpokesIntegration::Spokes_Detached(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Detached.");
	delete e;
}

void MySpokesIntegration::Spokes_CapabilitiesChanged(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> Capabilities Changed.");
	delete e;
}

void MySpokesIntegration::Spokes_MultiLineStateChanged(EventArgs * e)
{
	DebugPrint(__FUNCTION__, ">>> MultiLine State Changed.");
	delete e;
}

void MySpokesIntegration::DebugPrint(string methodname, string str)
{
	string newEntry;

		SYSTEMTIME systime;
	GetSystemTime(&systime);

	ostringstream tmpstrm;
	tmpstrm
		<< std::setfill('0') << std::setw(2) << systime.wHour << ":"
		<< std::setfill('0') << std::setw(2) << systime.wMinute << ":"
		<< std::setfill('0') << std::setw(2) << systime.wSecond << "."
		<< std::setfill('0') << std::setw(3) << systime.wMilliseconds << ": "
		<< methodname << ": " << str << endl;
	newEntry = tmpstrm.str();

	// convert to wide string as demonstration to support unicode output (wide character support) for Spokes debug output
	// thanks: http://stackoverflow.com/questions/6691555/converting-narrow-string-to-wide-string
    wchar_t* buf = new wchar_t[newEntry.size() * 2 + 2];
    swprintf( buf, L"%S", newEntry.c_str() );
    std::wstring wideval = buf;
    delete[] buf;

	// using wcout to demonstrate adding Spokes debug output to a wide character standard output (unicode support)
	wcout << wideval;
}