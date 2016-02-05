﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Plantronics.EZ.API;

/*******
 * 
 * PLTLayerTestApp
 * 
 * This is a test program to show the features of the PLTLayer API
 *
 * PLTLayer is a new minimalist API for controlling and interacting with
 * Plantronics devices from your applications.
 * For more details refer to the PLTLayer project.
 * 
 * Author: Lewis Collins
 * 
 * Contact: Lewis.Collins@Plantronics.com
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.0.0.5:
 * Date: 5th Feb 2016
 * Compatible with Plantronics Hub version(s): 3.7.x
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding demonstration of audioon/audiooff to the test application
 *       to show turning on/off the wireless link of pre-Focus UC products.
 *     - Adding showfeatures command to avoid too much debug for user.
 *
 * Version 1.0.0.4:
 * Date: 22nd Dec 2016
 * Compatible with Plantronics Hub version(s): 3.7.x
 * Changed by: Lewis Collins
 *   Changes:
 *     - Recompiling with updated wrapper/PLTLayer
 *     
 * Version 1.0.0.3:
 * Date: 10th Dec 2015
 * Compatible with Plantronics Hub version(s): 3.7.x
 * Changed by: Lewis Collins
 *   Changes:
 *     - Recompiling with bug fixed Spokes Wrapper
 *
 * Version 1.0.0.2:
 * Date: 2nd December 2014
 * Compatible with Plantronics Hub version(s): 3.3.50862.10305 (24/11/2014 pre-release for DA Series)
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding new DA Series QD events (Connected/Disconnected)
 *     - Remove un-needed linefeeds
 *     
 * Version 1.0.0.1:
 * Date: 26th August 2014
 * Compatible with Plantronics Hub version(s): 3.0.50718.1966
 * Changed by: Lewis Collins
 *   Changes:
 *     - Minor fixes to mobile call control convenience function
 *
 * Version 1.0.0.0:
 * Date: 22nd August 2014
 * Compatible with Plantronics Hub version(s): 3.0.50718.1966
 * Changed by: Lewis Collins
 *   Changes:
 *     - Initial version
 * ********************************************************************************
 * 
 **/

namespace Plantronics.EZ.PLTLayerTestApp
{
    /// <summary>
    /// The main class defining Main entry point for
    /// this sample console application.
    /// The way this application integrates with Plantronics
    /// is representative of how your application should do it.
    /// Follow the number steps in comments below 1-5.
    /// </summary>
    class PLTLayerTestApp
    {
        static PLTLayer plt;
        static bool m_quit = false;
        static string MY_APP_NAME = "PLTLayerTestApp";
        private static List<string> m_deviceCapabilities;
        private static bool m_deviceAttached = false;

        static void Main(string[] args)
        {
            // 1. Setup Plantronics
            plt = PLTLayer.Instance;
            //plt.SetConsoleLogging(true); // enable optional detailed logging info from Plantronics SDK
            plt.PltEvent += new PLTLayer.PltEventHandler(plt_PltEvent);
            plt.setup(MY_APP_NAME);

            // 2. Main application loop
            while (!m_quit)
            {
                NextUserCommand();
            }

            // 3. Shutdown Plantronics
            plt.PltEvent -= plt_PltEvent;
        }

        /// <summary>
        /// 4. Send app commands to Plantronics.
        /// 
        /// The command and parameters will be read from the user on the command line,
        /// then the appropriate method of the PLTLayer object "plt" will be called.
        /// </summary>
        private static void NextUserCommand()
        {
            string command = GetUserInput();
            bool isincoming = true;
            int callid = 0;
            string contactname = "";
            int line = 0;
            string numbertodial = "";
            switch (command.ToLower())
            {
                // BASIC SOFTPHONE CALL CONTROL FEATURE EXAMPLES:
                //
                case "on":
                    ReadCallInfo(ref isincoming, ref callid, ref contactname);
                    // Notify Plantronics our app has a new incoming or outgoing call
                    if (callid == Int32.MinValue)
                    {
                        // generate a call id automatically
                        if (contactname.Length > 0) callid = plt.on(isincoming, contactname);
                        else callid = plt.on(isincoming);
                        Console.WriteLine("\r\nAutogenerated call id was: " + callid);
                    }
                    else
                    {
                        // use the call id specified by our app
                        plt.on(isincoming, callid, contactname);
                    }
                    break;
                case "ring":
                    ReadCallInfo(ref callid, ref contactname);
                    // Notify Plantronics our app has a new incoming call
                    if (callid == Int32.MinValue)
                    {
                        // generate a call id automatically
                        if (contactname.Length > 0) callid = plt.ring(contactname);
                        else callid = plt.ring();
                        Console.WriteLine("\r\nAutogenerated call id was: " + callid);
                    }
                    else
                    {
                        // use the call id specified by our app
                        plt.ring(callid, contactname);
                    }
                    break;
                case "ans":
                    ReadCallInfo(ref callid);
                    // Notify Plantronics an incoming call was answered in our app
                    if (callid == Int32.MinValue)
                    {
                        // just answer the last call
                        plt.ans();
                    }
                    else
                    {
                        // use the call id specified by our app
                        plt.ans(callid);
                    }
                    break;
                case "off":
                    ReadCallInfo2(ref callid);
                    // Notify Plantronics a call was ended in our app
                    if (callid == Int32.MinValue)
                    {
                        // end all calls
                        plt.off();
                    }
                    else
                    {
                        // end specific call
                        plt.off(callid);
                    }
                    break;
                case "muteon":
                    plt.mute(true); // go on mute
                    break;
                case "muteoff":
                    plt.mute(false); // go off mute
                    break;
                case "hold":
                    ReadCallInfo(ref callid);
                    // Notify Plantronics a call was held in our app
                    if (callid == Int32.MinValue)
                    {
                        // just hold the last call
                        plt.hold(true);
                    }
                    else
                    {
                        // use the call id specified by our app
                        plt.hold(callid, true);
                    }
                    break;
                case "resume":
                    ReadCallInfo(ref callid);
                    // Notify Plantronics a call was resumed in our app
                    if (callid == Int32.MinValue)
                    {
                        // just resume the last call
                        plt.hold(false);
                    }
                    else
                    {
                        // use the call id specified by our app
                        plt.hold(callid, false);
                    }
                    break;

                // PLANTRONICS MULTI-LINE DEVICE EXAMPLES (e.g. Savi 700 Series):
                case "lineon":
                    ReadLineInfo(ref line);
                    // activate the chosen line
                    plt.on((PLTLine)line);
                    break;
                case "lineoff":
                    ReadLineInfo(ref line);
                    // de-activate the chosen line
                    plt.off((PLTLine)line);
                    break;
                case "audioon":
                    // activate the wireless link
                    plt.audioon();
                    break;
                case "audiooff":
                    // de-activate the wireless link
                    plt.audiooff();
                    break;
                case "linehold":
                    ReadLineInfo(ref line);
                    // hold the chosen line
                    plt.hold((PLTLine)line, true);
                    break;
                case "lineresume":
                    ReadLineInfo(ref line);
                    // resume/un-hold the chosen line
                    plt.hold((PLTLine)line, false);
                    break;

                // MOBILE CALL CONTROL EXAMPLES (e.g. Voyager Legend, Voyager Edge, Calisto 620):
                case "dialmob":
                    ReadNumberToDial(ref numbertodial);
                    // use mobile to dial the specified number
                    plt.dialmob(numbertodial);
                    break;
                case "ansmob":
                    // answer ringing incoming call on mobile
                    plt.ansmob();
                    break;
                case "rejmob":
                    // reject ringing incoming call on mobile
                    plt.rejmob();
                    break;
                case "endmob":
                    // end call on mobile
                    plt.endmob();
                    break;

                // PLANTRONICS "GENES" FEATURE (GET DEVICE SERIAL NUMBERS):
                case "getgenes":
                    // request the genes id's of base and headset devices
                    plt.getgenes();
                    break;

                case "showfeatures":
                    // show the Plantronics device capabilities (features)
                    if (m_deviceAttached)
                    {
                        Console.WriteLine("  [device features: "
                            + "docking: " + m_deviceCapabilities[0] + ", "
                            + "mob callid: " + m_deviceCapabilities[1] + ", "
                            + "mob state: " + m_deviceCapabilities[2] + "\r\n"
                            + "  multiline: " + m_deviceCapabilities[3] + ", "
                            + "proximit: " + m_deviceCapabilities[4] + ", "
                            + "wearstate: " + m_deviceCapabilities[5] + ", "
                            + "wireless: " + m_deviceCapabilities[6] + "]");                        
                    }
                    else
                    {
                        Console.WriteLine("Please attach a Plantronics device to PC first.");
                    }
                    break;

                case "help":
                    ShowValidCommands();
                    break;
                case "quit": m_quit = true;
                    break;
                default:
                    Console.WriteLine("unknown command");
                    ShowValidCommands();
                    break;
            }
        }

        /// <summary>
        /// 5. Handle events received from Plantronics
        /// 
        /// This event handler method is called by PLTLayer whenever a 
        /// Plantronics event occurs, e.g. device events, call state events etc.
        /// 
        /// By examining the "e.EventType" and "e.MyParams" parameters, your
        /// app can see what the event was and additional information about the event.
        /// For example, in the case of EventType SerialNumber, you get 2 MyParams
        /// strings, the first is the Serial Number (also known as Genes id), the second 
        /// contains whether it is a Base or Headset serial.
        /// </summary>
        /// <param name="sender">The object that has sent us the event, in this case 
        /// the PLTLayer object.</param>
        /// <param name="e">The arguments for this event, containing the EventType 
        /// and for some EventTypes a list of useful parameters (as strings) that 
        /// are specific to the EventType.</param>
        static void plt_PltEvent(object sender, PltEventArgs e)
        {
            // Example processing of incoming events/parameters to inform my app
            // what is happening with Plantronics:
            switch (e.EventType)
            {
                // BASIC SOFTPHONE CALL CONTROL EVENTS:
                //
                case PltEventType.CallAnswered:
                    Console.WriteLine("> Plantronics answered a Softphone Call:\r\n"
                        + "Call Id: " + e.MyParams[0] + "\r\n"
                        + "Call Source: " + e.MyParams[1]);
                    // is the call in my app?
                    if (e.MyParams[1] == MY_APP_NAME)
                    {
                        Console.WriteLine("\r\nTHIS CALL ID: " + e.MyParams[0] + " IS IN MY APP!: " + MY_APP_NAME);
                        // TODO: here you would answer the call in your app
                    }
                    break;
                case PltEventType.CallEnded:
                    Console.WriteLine("> Plantronics ended a Softphone Call:\r\n"
                        + "Call Id: " + e.MyParams[0] + "\r\n"
                        + "Call Source: " + e.MyParams[1]);
                    // is the call in my app?
                    if (e.MyParams[1] == MY_APP_NAME)
                    {
                        Console.WriteLine("\r\nTHIS CALL ID: " + e.MyParams[0] + " IS IN MY APP!: " + MY_APP_NAME);
                        // TODO: here you would end the call in your app
                    }
                    break;
                case PltEventType.Muted:
                    Console.WriteLine("> Plantronics was muted");
                    // TODO: syncronise with your app's mute feature
                    break;
                case PltEventType.UnMuted:
                    Console.WriteLine("> Plantronics was un-muted");
                    // TODO: syncronise with your app's mute feature
                    break;

                // ADVANCED SOFTPHONE CALL CONTROL EVENTS:
                //
                case PltEventType.OnCall:
                    Console.WriteLine("> Plantronics went on Softphone Call:\r\n"
                        + "Call Id: " + e.MyParams[0] + "\r\n"
                        + "Call Source: " + e.MyParams[1] + "\r\n"
                        + "Is Incoming?: " + e.MyParams[2] + "\r\n"
                        + "Call State: " + e.MyParams[3]);
                    // is the call in my app?
                    if (e.MyParams[1] == MY_APP_NAME)
                    {
                        Console.WriteLine("\r\nTHIS CALL ID: " + e.MyParams[0] + " IS IN MY APP!: " + MY_APP_NAME);
                        // OnCall event is for information only.
                        // you should use CallAnswered event to answer the call in your app
                    }
                    else
                    {
                        // TODO: optional syncronise with your app's agent availability feature
                    }
                    break;
                case PltEventType.NotOnCall:
                    Console.WriteLine("> Plantronics ended a Softphone Call:\r\n"
                        + "Call Id: " + e.MyParams[0] + "\r\n"
                        + "Call Source: " + e.MyParams[1]);
                    // is the call in my app?
                    if (e.MyParams[1] == MY_APP_NAME)
                    {
                        Console.WriteLine("\r\nTHIS CALL ID: " + e.MyParams[0] + "WAS IN MY APP!: " + MY_APP_NAME);
                        // NotOnCall event is for information only.
                        // you should use CallEnded event to end the call in your app
                    }
                    else
                    {
                        // TODO: optional syncronise with your app's agent availability feature
                    }
                    break;
                case PltEventType.CallSwitched:
                    Console.WriteLine("> Plantronics switched a Softphone Call");
                    // this event is for information only, the callid of activated call will
                    // be available via a CallAnswered event
                    break;
                case PltEventType.CallRequested:
                    Console.WriteLine("> Plantronics dialpad device requested (dialed) a Softphone Call to this contact:\r\n"
                        + "Email: " + e.MyParams[0] + "\r\n"
                        + "FriendlyName: " + e.MyParams[1] + "\r\n"
                        + "HomePhone: " + e.MyParams[2] + "\r\n"
                        + "Id: " + e.MyParams[3] + "\r\n"
                        + "MobilePhone: " + e.MyParams[4] + "\r\n"
                        + "Name: " + e.MyParams[5] + "\r\n"
                        + "Phone: " + e.MyParams[6] + "\r\n"
                        + "SipUri: " + e.MyParams[7] + "\r\n"
                        + "WorkPhone: " + e.MyParams[8]);
                    // TODO: optional: here you would dial an outgoing call in your app
                    break;

                // PLANTRONICS MULTI-LINE DEVICE EVENTS (e.g. Savi 700 Series):
                //
                case PltEventType.MultiLineStateChanged:
                    Console.WriteLine("> Plantronics multiline state changed: \r\n"
                        + "       PC Line: Active?: " + e.MyParams[0] + ", Held?: " + e.MyParams[1] + "\r\n"
                        + "   Mobile Line: Active?: " + e.MyParams[2] + ", Held?: " + e.MyParams[3] + "\r\n"
                        + "Deskphone Line: Active?: " + e.MyParams[4] + ", Held?: " + e.MyParams[5]);
                    // TODO: optional syncronise with your app's agent availability feature
                    break;

                // PLANTRONICS "GENES ID" FEATURE EVENTS (DEVICE SERIAL NUMBERS):
                //
                case PltEventType.SerialNumber:
                    if (e.MyParams[0] != "")
                    {
                        Console.WriteLine("> Plantronics Genes ID (Serial Number) was received:\r\n"
                                          + "Serial Number: " + e.MyParams[0] + "\r\n"
                                          + "Serial Type: " + e.MyParams[1]);
                    }
                    // TODO: optional syncronise with your app's agent tracking system
                    // (e.g. asset tracking or used to apply user personalised settings, i.e. on shared workstation)
                    break;

                // PLANTRONICS "CONTEXTUAL INTELLIGENCE FEATURE EVENTS:
                //
                case PltEventType.PutOn:
                    Console.WriteLine("> Plantronics was put on");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.TakenOff:
                    Console.WriteLine("> Plantronics was taken off");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.Near:
                    Console.WriteLine("> Plantronics in wireless range: NEAR");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.Far:
                    Console.WriteLine("> Plantronics in wireless range: FAR");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.InRange:
                    Console.WriteLine("> Plantronics came into wireless range");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.OutOfRange:
                    Console.WriteLine("> Plantronics went out of wireless range");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.Docked:
                    Console.WriteLine("> Plantronics was docked");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.UnDocked:
                    Console.WriteLine("> Plantronics was un-docked");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.Connected:
                    Console.WriteLine("> Plantronics was connected to QD connector");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.Disconnected:
                    Console.WriteLine("> Plantronics was disconnected from QD connector");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;

                // PLANTRONICS DEVICE INFORMATION EVENTS:
                //
                case PltEventType.Attached:
                    m_deviceAttached = true;
                    Console.WriteLine("\r\n> Plantronics was attached: Product Name: "
                        + e.MyParams[0] + ", Product Id: " + Int32.Parse(e.MyParams[1]).ToString("X")+"\r\n");
                    // TODO: optional: switch your app to headset audio mode when Plantronics attached
                    break;
                case PltEventType.Detached:
                    m_deviceAttached = false;
                    Console.WriteLine("> Plantronics was detached");
                    // TODO: optional: switch your app to non-headset audio mode when Plantronics detached
                    break;
                case PltEventType.CapabilitiesChanged:
                    m_deviceCapabilities = e.MyParams;
                // Optional: uncomment for debugging purposes
                //    Console.WriteLine("  [device features: "
                //        + "docking: " + e.MyParams[0] + ", "
                //        + "mob callid: " + e.MyParams[1] + ", "
                //        + "mob state: " + e.MyParams[2] + "\r\n"
                //        + "  multiline: " + e.MyParams[3] + ", "
                //        + "proximit: " + e.MyParams[4] + ", "
                //        + "wearstate: " + e.MyParams[5] + ", "
                //        + "wireless: " + e.MyParams[6] + "]");
                    // TODO: optional: use this information to know what features/events to expect from
                    // Plantronics device in your app
                    break;

                // MOBILE CALL CONTROL EVENTS (e.g. Voyager Legend, Voyager Edge, Calisto 620):
                //
                case PltEventType.OnMobileCall:
                    Console.WriteLine("> Plantronics went on Mobile Call:\r\n"
                        + "Is Incoming?: " + e.MyParams[0] + "\r\n"
                        + "Call State: " + e.MyParams[1]);
                    // TODO: optional syncronise with your app's agent availability feature
                    break;
                case PltEventType.MobileCallerId:
                    Console.WriteLine("> Plantronics reported Mobile Caller Id (remote party phone number):\r\n"
                        + "Mobile Caller Id?: " + e.MyParams[0]);
                    // TODO: optional syncronise with your app's contacts database / CRM system
                    break;
                case PltEventType.NotOnMobileCall:
                    Console.WriteLine("> Plantronics ended a Mobile Call");
                    // TODO: optional syncronise with your app's agent availability feature
                    break;

                // OTHER DEVICE EVENTS:
                //
                case PltEventType.BaseButtonPressed:
                    Console.WriteLine("> Plantronics base button pressed: button id: "
                        + e.MyParams[0]);
                    // BaseButtonPressed event is for information only
                    // Note: some devices will generate button events internally even when no
                    // physical button is pressed.
                    break;
                case PltEventType.ButtonPressed:
                    Console.WriteLine("> Plantronics button pressed: button id: "
                        + e.MyParams[0]);
                    // ButtonPressed event is for information only
                    // Note: some devices will generate button events internally even when no
                    // physical button is pressed.
                    break;
            }

            // Example debug output to show ALL events/parameters (commented out)
            //Console.WriteLine("\r\nPltEvent received: " + e.EventType.ToString());
            //if (e.MyParams != null && e.MyParams.Count() > 0)
            //{
            //    Console.WriteLine("Params: ");
            //    int i = 0;
            //    foreach (string param in e.MyParams)
            //    {
            //        Console.Write((++i).ToString() + " ");
            //        Console.WriteLine(param);
            //    }
            //}
        }

        #region User input functions
        /// <summary>
        /// Read a textual command from the user on the console.
        /// </summary>
        /// <param name="prompt">An optional custom prompt to ask user to type something else.</param>
        /// <returns></returns>
        private static string GetUserInput(string prompt = "Enter Command (or type help) > ")
        {
            Console.WriteLine();
            Console.Write(prompt);
            string command = Console.ReadLine();
            return command;
        }

        /// <summary>
        /// A useful method for debugging the shutdown of a command line application.
        /// Waits for user to press a key before continuing.
        /// </summary>
        private static void Pause()
        {
            Console.WriteLine();
            Console.WriteLine("Press a key to continue...");
            Console.ReadKey();
        }

        /// <summary>
        /// Converts a string to an int32 but with exception handling
        /// in case the input is wrong.
        /// </summary>
        /// <param name="input">The string we want to convert to an int.</param>
        /// <returns>The integer value or Int32.MinValue in case nothing was entered or an error occured.</returns>
        private static int SafeConvertToInt(string input, int min = Int32.MinValue, int max = Int32.MaxValue)
        {
            int retval = Int32.MinValue;
            try
            {
                retval = Convert.ToInt32(input);
                if (retval < min) retval = min;
                if (retval > max) retval = max;
            }
            catch (Exception)
            {
                retval = Int32.MinValue;
            }
            return retval;
        }

        private static void ShowValidCommands()
        {
            Console.WriteLine();
            Console.WriteLine("Valid commands:");
            Console.WriteLine("--");
            Console.WriteLine("on, ring, ans, off, muteon, muteoff, hold, resume, audioon, audiooff,\r\n"+
                "lineon, lineoff, linehold, lineresume, dialmob, ansmob, rejmob, endmob,\r\n"+
                "getgenes, showfeatures, help, quit");
        }

        private static void ReadCallInfo(ref int callid)
        {
            callid = SafeConvertToInt(GetUserInput("Enter call id (integer)\r\nor press Enter for last call : "), 0);
        }

        private static void ReadLineInfo(ref int line)
        {
            line = SafeConvertToInt(GetUserInput("Enter line number (integer, 1 = PC, 2 = Mobile, 3 = Desk) : "), 1, 3);
        }

        private static void ReadCallInfo2(ref int callid)
        {
            callid = SafeConvertToInt(GetUserInput("Enter call id (integer)\r\nor press Enter to end all calls : "), 0);
        }

        private static void ReadCallInfo(ref int callid, ref string contactname)
        {
            callid = SafeConvertToInt(GetUserInput("Enter call id (integer)\r\nor press Enter to generate call id automatically : "), 0);
            contactname = GetUserInput("Enter contact name (string)\r\nor press Enter to skip : ");
        }

        private static void ReadCallInfo(ref bool isincoming, ref int callid, ref string contactname)
        {
            isincoming = SafeConvertToInt(GetUserInput("Enter call direction \r\n(integer, 1 = incoming, 2 = outgoing) : "), 1, 2) != 2;
            callid = SafeConvertToInt(GetUserInput("Enter call id (integer)\r\nor press Enter to generate call id automatically : "), 0);
            contactname = GetUserInput("Enter contact name (string)\r\nor press Enter to skip : ");
        }

        private static void ReadNumberToDial(ref string numbertodial)
        {
            numbertodial = GetUserInput("Enter number to dial (string) : ");
        }
        #endregion
    }
}
