SpokesWrapper
=============

Spokes Wrapper is a source code sample that presents simplified API to
use with Plantronics Spokes Software.

The C# version is fairly feature complete.

The C++ version is not complete (see the comment block).

Comment blocks for both versions are cut and paste below.


/*******
 * 
 * SpokesWrapper.cs
 * 
 * SpokesWrapper.cs is a wrapper around the Plantronics Spokes COM Service API for C# .NET (.NET Framework 4 and higher).
 * 
 * It's purpose is to make it easier and simpler to integrate support for Plantronics devices into any applications.
 * 
 * It achieves this by hiding a lot of the more tricky aspects of integration behind the wrapper and presenting
 * a simple and consistent set of Event Handlers and functions for the core features of the SDK that the user
 * will typically be needing.
 * 
 * *** WARNING !!! This source code is provided *As Is*! It is intented as a sample code to show ways of integrating
 * with the Spokes "COM Service .NET API". However in case of problems please feel free to contact Lewis Collins
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
 * Lewis Collins
 * 
 * http://developer.plantronics.com/people/lcollins
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.0.22:
 * Date: 28th June 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added new IsSpokesInstalled convenience function. You can optionally call this before
 *       calling the Spokes.Instance.Connect method to check if Spokes is installed.
 *       It is called by default anyway when Connect is called so that if Spokes is not
 *       installed, Connect will immediately return false (failed to connect), rather
 *       than cause a timeout waiting for COM Object to instantiate (and fail).
 *
 * Version 1.0.21:
 * Date: 17th June 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Fix to initial undocking detection for Savi 7xx
 *
 * Version 1.0.20:
 * Date: 30th May 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Final fix to docking detection of Legend UC!!!
 *
 * Version 1.0.19:
 * Date: 30th May 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Yet another minor fix to docking detection of Legend UC!
 *
 * Version 1.0.18:
 * Date: 28th May 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Another minor fix to docking detection of Legend UC
 *
 * Version 1.0.17:
 * Date: 24th May 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding BatteryLevelChange event notification when the battery charge state
 *       changes.
 *     - Adding GetBatteryLevel methods to be called by app in response
 *       to the above event notification event to obtain the current battery
 *       level.
 *     - Minor fix to docking detection of Legend UC
 *
 * Version 1.0.16:
 * Date: 14th May 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding base button press event handler so apps can know when base button
 *       is pressed.
 *     - Changing headset button press event registration to avoid duplicate headset
 *       talk button pressed events
 *
 * Version 1.0.15:
 * Date: 25th Apr 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Add function to send a custom message to headset
 *
 * Version 1.0.14:
 * Date: 12th Apr 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Minor fix to establish when _not_ on softphone call on device attach
 *     - Added initialization of display devices (dialpad devices) in case user
 *       wants to use CallRequested event (call dialled) from those devices
 *
 * Version 1.0.13:
 * Date: 11th Apr 2013
 * Compatible/tested with Spokes SDK version(s): 2.8.24304.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added docking/undocking (charging base) detection for Voyager Legend UC headset
 *
 * Version 1.0.12:
 * Date: 5th Apr 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0, 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added get ongoing softphone call status when you first start your app!
 *
 * Version 1.0.11:
 * Date: 14th Mar 2013
 * Compatible with Spokes SDK version(s): 2.8.24304.0, 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added try/catch exception handling and debug around registering for extended headsetstatechange
 *       and extended basestatechange events that were used for "asyncronous" method of receiving serial
 *       numbers from device (works for base serial, NOT for headset serial)
 *     - Also added "syncronous" method of obtaining serial numbers following discussion with Ramesh Mar 2013
 *       however this is NOT working for base OR headset serial! (So left in "asyncronous" method for now,
 *       at least we have base!)
 *
 * 
 * Version 1.0.10b:
 * Date: 08th Mar 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - New, get all the device state info on inrange trigger (not just proximity and headset serial number)
 *     
 * Version 1.0.10:
 * Date: 05th Mar 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Updated some internal methods to public (they didn't need to be internal)
 *     
 * Version 1.0.9:
 * Date: 22nd Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Add proximity enabled / proximity disabled event handlers
 *
 * Version 1.0.8:
 * Date: 21st Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Add headset button events via new ButtonPressed event handler in the wrapper.
 *       NOTE: you are advised NOT to use headset buttons e.g. talk button for
 *       call control, but rather use the IncomingCall/OutgoingCall functions
 *       and CallAnswered/CallEnded event handlers. Using talk button will
 *       cause problems with multiline devices as talk button events for the
 *       deskphone (+EHS) will also be received by your app through the SDK!!!!
 *       Also bad interactions can occur with talk button and other softphones
 *       on your system e.g. Lync if you try to use raw button events.
 *       You have been warned.
 *     - Add CallRequested event handler to obtain user call requested events from
 *       dialpad devices (Calisto P240/800 series)
 *
 * Version 1.0.7:
 * Date: 19th Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Change namespace to Plantronics.UC.SpokesWrapper (from Plantronics.UC.Spokes)
 *
 * Version 1.0.6:
 * Date: 14th Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Minor fix to incorrect worn state in TakenOff event handler
 *
 * Version 1.0.5:
 * Date: 8th Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added flag for don/doff/dock/undock event to say if it is the initial status
 *       so an app can ignore the initial status if it wants to (i.e. not lock screen
 *       when it first runs and receives initial status event!)
 *
 * Version 1.0.4:
 * Date: 6th Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added new method to ask if link is active
 *
 * Version 1.0.3:
 * Date: 1st Feb 2013
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Added new "line active changed" event handler so apps can know when 
 *       line to device is active or not
 *
 * Version 1.0.2:
 * Date: 4th December 2012
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding multiline device features e.g. for Savi 7xx
 *
 * Version 1.0.1:
 * Date: 30th November 2012
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Fixed order of events for DetachDevice flow
 *     - Fixed need to check for null serial number member
 *
 * Version 1.0:
 * Date: 30th November 2012
 * Compatible with Spokes SDK version(s): 2.7.14092.0
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adds code to extract serial number (thanks Nemanja)
 *     - Adds comments to all publicly expose methods and event handlers  
 * ********************************************************************************
 * 
 **/

 --

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
