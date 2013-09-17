// C++CallControl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "MySpokesIntegration.h"

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
		//else if( input == "ringoff") // set ring state to off
		//{
		//	// not required, to stop headset ring tone use: Spokes::GetInstance()->EndCall(mycallid); 
		//}
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

			//// Make call to default softphone
			//wcout << "Enter contact: " << std::endl;
			//getline(cin, input);

			//CComPtr<IContact> cnt;
			//CComObject<CallContact> *contact = CallContact::GetContact(input);
			//contact->QueryInterface(__uuidof(IContact),(void **) &cnt); 

			//callcommand->MakeCall( cnt );			
		}
		else if( input == "holdcall" || input == "endcall" || input == "answeredcall" || input == "resumecall" || input == "setconferenceid" )
		{
			// NOTE: making assumption for this demo to just work with the current call id in mycallid
			// if you are implementing a call queue with multiple call id's just pass the
			// required call id from your queue to the EndCall / AnswerCall etc function below...

			if( input == "holdcall") wcout << "holdcall not implemented" << endl; // Spokes::GetInstance()->HoldCall(mycallid);  // NOT IMPLEMENTED
			else if( input == "endcall")		Spokes::GetInstance()->EndCall(mycallid);
			else if (input == "answeredcall" )	Spokes::GetInstance()->AnswerCall(mycallid);
			else if( input == "resumecall") Spokes::GetInstance()->ResumeCall(mycallid);  // NOT IMPLEMENTED
			else if( input == "setconferenceid") wcout << "setconferenceid not implemented" << endl; // Spokes::GetInstance()->SetConferenceId(mycallid);  // NOT IMPLEMENTED
		}
		else if (input == "incoming" || input == "outgoing" )
		{
			// NOTE: making assumption for this demo to just increment the call id in mycallid
			// if you are implementing a call queue with multiple call id's just pass the
			// required call id from your queue to the OutgoingCall / IncomingCall function below...

			if( input == "outgoing")	Spokes::GetInstance()->OutgoingCall(++mycallid, "");
			else						Spokes::GetInstance()->IncomingCall(++mycallid, "");
		}
		else if( input == "quit" )
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
			wcout << "makecall:        NOT IMPLEMENTED" << endl;
			wcout << "holdcall:        NOT IMPLEMENTED" << endl;
			wcout << "endcall:         calls Spokes::GetInstance()->EndCall(mycallid)" << endl;
			wcout << "answeredcall:    calls Spokes::GetInstance()->AnswerCall(mycallid)" << endl;
			wcout << "resumecall:      NOT IMPLEMENTED" << endl;
			wcout << "setconferenceid: NOT IMPLEMENTED" << endl;
			wcout << "incoming:        Spokes::GetInstance()->OutgoingCall(++mycallid, \"\")" << endl;
			wcout << "outgoing:        Spokes::GetInstance()->IncomingCall(++mycallid, \"\")" << endl;
			wcout << "quit: close application" << endl;
		}
		else if( !quit )
		{
			wcout << "Enter command (type help for list of commands) " << endl;
		}
		cin.clear();
	}
}



int _tmain(int argc, _TCHAR* argv[])
{
	wcout << "Let's integrate Plantronics support...";

	// attach to Spokes
	MySpokesIntegration * mySpokesIntegration = new MySpokesIntegration();
	Spokes::GetInstance()->Connect("My C++ App"); // do this after instantiation of MySpokesIntegration (i.e. not in constructor)
												  // so that C++ can sort out its virtual method tables else we won't receive Spokes events

	// start waiting for user input
	CommandLoop();

	// detach from Spokes
	Spokes::GetInstance()->Disconnect();
	delete mySpokesIntegration;

	return 0;
}

