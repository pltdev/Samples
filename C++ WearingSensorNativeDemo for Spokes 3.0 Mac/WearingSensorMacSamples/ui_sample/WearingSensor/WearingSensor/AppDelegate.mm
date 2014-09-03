//
//  AppDelegate.m
//  WearingSensor
//
//  Copyright (c) 2014 Plantronics Inc. All rights reserved.
//

#import "AppDelegate.h"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

#include "Spokes3G.h"
#include "cpp/query_cast.h"




// create sink for receiving device listener events
class DeviceListenerEventSink : public IDeviceListenerCallback
{
public:
	DeviceListenerEventSink() {}
	// IDeviceListenerCallback implementations
	virtual void onHeadsetButtonPressed(DeviceListenerEventArgs const &args) { }
    virtual void onHeadsetStateChanged(DeviceListenerEventArgs const &args) 
	{ 
		switch (args.headsetStateChange)
		{
			case HS_STATE_CHANGE_DON:
				wcout << "Headset is worn" << endl;
				break;
			case HS_STATE_CHANGE_DOFF:
				wcout << "Headset is not worn" << endl;
				break;
            default:
                break;
		}
	}
    virtual void onBaseButtonPressed(DeviceListenerEventArgs const &args) { }
    virtual void onBaseStateChanged(DeviceListenerEventArgs const &args) { }
    virtual void onATDStateChanged(DeviceListenerEventArgs const &args) { }
};

@interface AppDelegate()
{
    ISessionManager* sessionManager;
    ISession* session;
    IDevice* activeDevice;
    DeviceListenerEventSink* deviceCallback;
}

@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Insert code here to initialize your application

    InitSpokesRuntime();

	// create session manager
	if( SM_RESULT_SUCCESS == getSessionManager(&sessionManager) )
	{
		// create session
		if( SM_RESULT_SUCCESS == sessionManager->registerSession( L"Spokes Wearing Sensor Native Sample", &session ) )
		{
			if( SM_RESULT_SUCCESS != session->getActiveDevice( &activeDevice ) )
			{
				wcout << "there is no active devices, please attach one then run the app again" << endl;
				activeDevice = nullptr;
			}
			else
			{
				// Get device listener
				pDeviceListener dev_listener = nullptr;
				auto dm_result = activeDevice->getDeviceListener(&dev_listener);

				if(dm_result != DM_RESULT_SUCCESS) 
				{
					wcout << "failed to get device listener interface" << endl;
				}
				else
				{
					// Register device listener callbacks
					deviceCallback = new DeviceListenerEventSink();
					dm_result = dev_listener->registerCallback(deviceCallback);

					if(dm_result != DM_RESULT_SUCCESS)
					{
						wcout << "failed to register device listener callback" << endl;
                        delete deviceCallback;
                        deviceCallback = nullptr;
					}
				}
			}
        }
    }
}

- (void)applicationWillTerminate:(NSNotification *)notification;
{
    if (activeDevice != nullptr)
    {
        pDeviceListener dev_listener = nullptr;
        auto dm_result = activeDevice->getDeviceListener(&dev_listener);
        if (dm_result == DM_RESULT_SUCCESS && deviceCallback != nullptr)
        {
            dev_listener->unregisterCallback(deviceCallback);
            delete deviceCallback;
        }
        activeDevice->Release();
    }

    if (sessionManager != nullptr && session != nullptr)
    {
        sessionManager->unregisterSession(session);
        session->Release();
    }

    ShutDownSpokesRuntime();
}

@end
