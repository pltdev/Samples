using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Plantronics.UC.SpokesWrapper;

namespace Plantronics.UC.SpokesSocketMiddleware
{
    class Program
    {
        static Spokes spokes;

        static SpokesSocketInterface socket;

        static void Main(string[] args)
        {
            spokes = Spokes.Instance;
            spokes.PutOn += new Spokes.PutOnEventHandler(spokes_PutOn);
            spokes.TakenOff += new Spokes.TakenOffEventHandler(spokes_TakenOff);
            spokes.Near += new Spokes.NearEventHandler(spokes_Near);
            spokes.Far += new Spokes.FarEventHandler(spokes_Far);
            spokes.InRange += new Spokes.InRangeEventHandler(spokes_InRange);
            spokes.OutOfRange += new Spokes.OutOfRangeEventHandler(spokes_OutOfRange);
            spokes.Docked += new Spokes.DockedEventHandler(spokes_Docked);
            spokes.UnDocked += new Spokes.DockedEventHandler(spokes_UnDocked);
            spokes.MobileCallerId += new Spokes.MobileCallerIdEventHandler(spokes_MobileCallerId);
            spokes.OnMobileCall += new Spokes.OnMobileCallEventHandler(spokes_OnMobileCall);
            spokes.NotOnMobileCall += new Spokes.NotOnMobileCallEventHandler(spokes_NotOnMobileCall);
            spokes.SerialNumber += new Spokes.SerialNumberEventHandler(spokes_SerialNumber);
            spokes.CallAnswered += new Spokes.CallAnsweredEventHandler(spokes_CallAnswered);
            spokes.CallEnded += new Spokes.CallEndedEventHandler(spokes_CallEnded);
            spokes.CallSwitched += new Spokes.CallSwitchedEventHandler(spokes_CallSwitched);
            spokes.OnCall += new Spokes.OnCallEventHandler(spokes_OnCall);
            spokes.NotOnCall += new Spokes.NotOnCallEventHandler(spokes_NotOnCall);
            spokes.MuteChanged += new Spokes.MuteChangedEventHandler(spokes_MuteChanged);
            spokes.LineActiveChanged += new Spokes.LineActiveChangedEventHandler(spokes_LineActiveChanged);
            spokes.Attached += new Spokes.AttachedEventHandler(spokes_Attached);
            spokes.Detached += new Spokes.DetachedEventHandler(spokes_Detached);
            spokes.CapabilitiesChanged += new Spokes.CapabilitiesChangedEventHandler(spokes_CapabilitiesChanged);
            spokes.MultiLineStateChanged += new Spokes.MultiLineStateChangedEventHandler(spokes_MultiLineStateChanged);
            
            // NOTE: we will not call Connect on spokes until client app has connected
            // otherwise client will miss initial state events!

            socket = new SpokesSocketInterface(spokes);
            socket.StartSocket();

            // TODO make socket threaded so you are not stuck in a loop in above call...
            Console.WriteLine("Enter to quit");
            Console.ReadLine();

            socket.StopSocket();
        }

        static void spokes_LineActiveChanged(object sender, LineActiveChangedArgs e)
        {
            Console.WriteLine("LineActiveChanged, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("LineActiveChanged,"+e.m_lineactive);
        }

        static void spokes_MultiLineStateChanged(object sender, MultiLineStateArgs e)
        {
            Console.WriteLine("MultiLineStateChanged, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("MultiLineStateChanged,"+
                e.MultiLineState.PCActive+","+
                e.MultiLineState.PCHeld+","+
                e.MultiLineState.MobileActive+","+
                e.MultiLineState.MobileHeld+","+
                e.MultiLineState.DeskphoneActive+","+
                e.MultiLineState.DeskphoneHeld);
        }

        static void spokes_CapabilitiesChanged(object sender, EventArgs e)
        {
            Console.WriteLine("CapabilitiesChanged, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("CapabilitiesChanged,"+spokes.DeviceCapabilities.HasCallerId+","+
                spokes.DeviceCapabilities.HasDocking + ","+
                spokes.DeviceCapabilities.HasMultiline + ","+
                spokes.DeviceCapabilities.HasProximity + ","+
                spokes.DeviceCapabilities.HasWearingSensor + ","+
                spokes.DeviceCapabilities.IsWireless);
        }

        static void spokes_Detached(object sender, EventArgs e)
        {
            Console.WriteLine("Detached, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("Detached");
        }

        static void spokes_Attached(object sender, AttachedArgs e)
        {
            Console.WriteLine("Attached, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("Attached,"+e.m_device.ProductName+","+e.m_device.ProductID);
        }

        static void spokes_MuteChanged(object sender, MuteChangedArgs e)
        {
            Console.WriteLine("MuteChanged, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("MuteChanged,"+e.m_muteon);
        }

        static void spokes_NotOnCall(object sender, EventArgs e)
        {
            Console.WriteLine("NotOnCall, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("NotOnCall");
        }

        static void spokes_OnCall(object sender, OnCallArgs e)
        {
            Console.WriteLine("OnCall, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("OnCall,"+e.Incoming+","+(int)e.State+","+e.CallSource);
        }

        static void spokes_CallSwitched(object sender, EventArgs e)
        {
            Console.WriteLine("CallSwitched, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("CallSwitched");
        }

        static void spokes_CallEnded(object sender, CallEndedArgs e)
        {
            Console.WriteLine("CallEnded, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("CallEnded," + e.CallId + "," + e.CallSource);
        }

        static void spokes_CallAnswered(object sender, CallAnsweredArgs e)
        {
            Console.WriteLine("CallAnswered, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("CallAnswered,"+e.CallId+","+e.CallSource);
        }

        static void spokes_SerialNumber(object sender, SerialNumberArgs e)
        {
            Console.WriteLine("SerialNumber, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("SerialNumber,"+e.SerialNumber+","+(int)e.SerialNumberType);
        }

        static void spokes_NotOnMobileCall(object sender, EventArgs e)
        {
            Console.WriteLine("NotOnMobileCall, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("NotOnMobileCall");
        }

        static void spokes_OnMobileCall(object sender, OnMobileCallArgs e)
        {
            Console.WriteLine("OnMobileCall, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("OnMobileCall,"+e.Incoming+","+(int)e.State);
        }

        static void spokes_MobileCallerId(object sender, MobileCallerIdArgs e)
        {
            Console.WriteLine("MobileCallerId, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("MobileCallerId,"+e.MobileCallerId);
        }

        static void spokes_UnDocked(object sender, DockedStateArgs e)
        {
            Console.WriteLine("UnDocked, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("UnDocked," + e.m_docked + "," + e.m_isInitialStateEvent);
        }

        static void spokes_Docked(object sender, DockedStateArgs e)
        {
            Console.WriteLine("Docked, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("Docked," + e.m_docked + "," + e.m_isInitialStateEvent);
        }

        static void spokes_OutOfRange(object sender, EventArgs e)
        {
            Console.WriteLine("OutOfRange, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("OutOfRange");
        }

        static void spokes_InRange(object sender, EventArgs e)
        {
            Console.WriteLine("InRange, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("InRange");
        }

        static void spokes_Far(object sender, EventArgs e)
        {
            Console.WriteLine("Far, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("Far");
        }

        static void spokes_Near(object sender, EventArgs e)
        {
            Console.WriteLine("Near, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("Near");
        }

        static void spokes_TakenOff(object sender, WearingStateArgs e)
        {
            Console.WriteLine("Headset taken off, sending message to Client App via socket...");
            if (socket==null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("TakenOff," + e.m_worn + "," + e.m_isInitialStateEvent); 
        }

        static void spokes_PutOn(object sender, WearingStateArgs e)
        {
            Console.WriteLine("Headset put on, sending message to Client App via socket...");
            if (socket == null)
            {
                Console.WriteLine("INFO: No client connected.");
                return;
            }
            socket.sendMsg("PutOn," + e.m_worn + "," + e.m_isInitialStateEvent);
        }
    }
}
