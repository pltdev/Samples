// SpokesSamples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "MySpokesIntegration.h"

// We need this as project is created as Win console app, and we are instantiating ATL COM objects that are used as Event sink's
CComModule _Module;
extern __declspec(selectany) CAtlModule* _pAtlModule=&_Module;


bool quit = false;
int mycallid = 0;


/*!**************************************************************
 * CommandLoop
 *
 * Description: Wait for user Input and send proper commands (ring, audio, mute, makecall,....)
 * Revision:	1.0
 * Author:		Andrija M. 03/01/2012
 ****************************************************************/
void CommandLoop()
{
	wcout << "Enter command (type help for list of commands) " << endl;
	// loop in command, until quit is called
	while(!quit)
	{
		string input;
		getline(cin, input);
		
		if( input == "ringon") // set ring state to on
		{
			Spokes::GetInstance()->IncomingCall(++mycallid);
		}
		else if( input == "ringoff") // set ring state to off
		{
			wcout << "ringoff not implemented" << endl;
			wcout << "to stop headset ringing use: Spokes::GetInstance()->EndCall(mycallid);" << endl;
			wcout << "(in this sample program type: endcall)" << endl;
		}
		else if( input == "audioon" )
		{	
			Spokes::GetInstance()->ConnectAudioLinkToDevice(true);
		}
		else if( input == "audiooff" )
		{		
			Spokes::GetInstance()->ConnectAudioLinkToDevice(false);
		}
		else if ( input == "muteon")
		{
			Spokes::GetInstance()->SetMute(true);
		}
		else if ( input == "muteoff")
		{
			Spokes::GetInstance()->SetMute(false);
		}
		else if ( input == "makecall")
		{
			wcout << "makecall not implemented" << endl;
			wcout << "To tell Plantronics that your softphone has connected an outgoing call, use:" << endl;
			wcout << "	Spokes::GetInstance()->OutgoingCall(++mycallid, "");" << endl;
			wcout << "(in this sample program type: outgoing)" << endl;

			// Make call using Hub's "default softphone"
			//callcommand->MakeCall( CallContact::GetContactObject(contactname) );			
		}
		else if( input == "holdcall" || input == "endcall" || input == "answeredcall" || input == "resumecall" || input == "setconferenceid" )
		{
		//	// NOTE: making assumption for this demo to just work with the current call id in mycallid
		//	// if you are implementing a call queue with multiple call id's just pass the
		//	// required call id from your queue to the EndCall / AnswerCall etc function below...

			if( input == "holdcall")			Spokes::GetInstance()->HoldCall(mycallid);
			else if( input == "endcall")		Spokes::GetInstance()->EndCall(mycallid);
			else if (input == "answeredcall" )	Spokes::GetInstance()->AnswerCall(mycallid);
			else if( input == "resumecall")		Spokes::GetInstance()->ResumeCall(mycallid);
			else if( input == "setconferenceid") Spokes::GetInstance()->SetConferenceId(mycallid);
		}
		else if (input == "incoming" || input == "outgoing" )
		{
			// NOTE: making assumption for this demo to just increment the call id in mycallid
			// if you are implementing a call queue with multiple call id's just pass the
			// required call id from your queue to the OutgoingCall / IncomingCall function below...

			if( input == "outgoing")	Spokes::GetInstance()->OutgoingCall(++mycallid, "");
			else						Spokes::GetInstance()->IncomingCall(++mycallid, "");
		}
		else if( input == "quit" || input == "exit" )
		{
			// exit from loop
			break;
		}
		else if ( input == "help" )
		{
			wcout << "Supported commands" << endl;
			wcout << "ringon:          calls Spokes::GetInstance()->IncomingCall(++mycallid)" << endl;
			wcout << "ringoff:         NOT IMPLEMENTED/REQUIRED, USE endcall" << endl;
			wcout << "audioon:         calls Spokes::GetInstance()->ConnectAudioLinkToDevice(true)" << endl;
			wcout << "audiooff:        calls Spokes::GetInstance()->ConnectAudioLinkToDevice(false)" << endl;
			wcout << "muteon:          calls Spokes::GetInstance()->SetMute(true)" << endl;
			wcout << "muteoff:         calls Spokes::GetInstance()->SetMute(false)" << endl;
			wcout << "makecall:        NOT IMPLEMENTED/REQUIRED, USE outgoing" << endl;
			wcout << "holdcall:        calls Spokes::GetInstance()->HoldCall(mycallid)" << endl;
			wcout << "endcall:         calls Spokes::GetInstance()->EndCall(mycallid)" << endl;
			wcout << "answeredcall:    calls Spokes::GetInstance()->AnswerCall(mycallid)" << endl;
			wcout << "resumecall:      calls Spokes::GetInstance()->ResumeCall(mycallid)" << endl;
			wcout << "setconferenceid: calls Spokes::GetInstance()->SetConferenceId(mycallid)" << endl;
			wcout << "incoming:        calls Spokes::GetInstance()->OutgoingCall(++mycallid, \"\")" << endl;
			wcout << "outgoing:        calls Spokes::GetInstance()->IncomingCall(++mycallid, \"\")" << endl;
			wcout << "quit / exit:	   close application" << endl;
		}
		else if( !quit )
		{
			wcout << "Enter command (type help for list of commands) " << endl;
		}
		cin.clear();
	}
}

// sample entry point
int _tmain(int argc, _TCHAR* argv[])
{
	wcout << "Let's integrate Plantronics support...";

	// attach to Spokes
	MySpokesIntegration * mySpokesIntegration = new MySpokesIntegration();
	if (Spokes::GetInstance()->Connect("My C++ App")) // do this after instantiation of MySpokesIntegration (i.e. not in constructor)
												  // so that C++ can sort out its virtual method tables else we won't receive Spokes events
	{
		// start waiting for user input
		CommandLoop();

		// detach from Spokes
		Spokes::GetInstance()->Disconnect();
		delete mySpokesIntegration;
	}
	else
	{
		wcout << "Failed to connect to Plantronics. Do you have Hub installed from www.plantronics.com/software ?";
	}

	return 0;
}
