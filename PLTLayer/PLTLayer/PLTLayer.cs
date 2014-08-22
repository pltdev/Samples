using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Interop.Plantronics;
using Plantronics.UC.SpokesWrapper;

/*******
 * 
 * PLTLayer
 * 
 * PLTLayer is a new Plantronics API library designed to provide a 
 * "minimalist" API to applications wishing to implement common telephony 
 * and contextual intelligence scenarios with Plantronics products.
 * 
 * *** WARNING !!! This source code (and any pre-compiled binaries of it) is 
 * provided *As Is*! It is intented as a sample code to show ways of integrating
 * with the Spokes "COM Service .NET API". However in case of problems 
 * please feel free to contact Lewis Collins directly via the PDC site at this 
 * address: http://developer.plantronics.com/people/lcollins/ ***
 * 
 * The latest version of this file will also be maintained on Github (feel free to create your own Fork!), here:
 * 
 * https://github.com/pltdev/Samples/PLTLayer/
 * 
 * Read more about Plantronics Spokes at the Plantronics Developer Connection web site:
 * 
 * http://developer.plantronics.com/community/devzone/
 * 
 * Author: Lewis Collins
 * 
 * Contact: Lewis.Collins@Plantronics.com
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.0.0.0:
 * Date: 22nd August 2014
 * Compatible with Plantronics Hub version(s): 3.0.50718.1966
 * Changed by: Lewis Collins
 *   Changes:
 *     - Initial version
 * ********************************************************************************
 * 
 **/

namespace Plantronics.EZ.API
{
    /// <summary>
    /// PLTLayer is the main class of PLTLayer API with methods
    /// for interacting with Plantronics devices.
    /// PLTLayer is a singleton object class.
    /// You cannot construct directly, instead call the Instance
    /// method to obtain the singleton.
    /// </summary>
    public sealed class PLTLayer : DebugLogger
    {
        #region singleton
        private static volatile PLTLayer instance;
        private static object syncRoot = new Object();

        /// <summary>
        /// Call this method to obtain the singleton instance of PLTLayer.
        /// </summary>
        public static PLTLayer Instance
        {
            get
            {
                if (instance == null)
                {
                    lock (syncRoot)
                    {
                        // Instantiate a singleton PLTLayer object
                        if (instance == null)
                            instance = new PLTLayer();
                    }
                }

                return instance;
            }
        }
        #endregion
        
        #region Plantronics Spokes Wrapper API
        Spokes m_spokes;
        private bool m_IsConnected = false;
        DebugLogger m_debuglog = null;
        bool m_consolelogging = false;
        /// <summary>
        /// If your application class implements the Spokes.DebugLogger interface you can pass a reference to your application class
        /// to the SetLogger method. This allows your class to be responsible for debug logging of Spokes related debug trace information.
        /// </summary>
        /// <param name="aLogger">For this parameter pass the "this" reference of your class that implements Spokes.DebugLogger interface.</param>
        public void SetLogger(DebugLogger aLogger)
        {
            m_debuglog = aLogger;
        }
        public void SetConsoleLogging(bool enable)
        {
            m_consolelogging = enable;
        }
        bool m_muted = false;
        bool m_docked = true;
        int m_internalcallid = 0;
        List<PLTCallId> m_callids = new List<PLTCallId>();
        #endregion
     
        private PLTLayer() // can't call constructor due to singleton
        {
        }

        ~PLTLayer()
        {
            shutdown();
        }

        #region Plantronics SDK event handler
        public delegate void PltEventHandler(object sender, PltEventArgs e);

        /// <summary>
        /// This event handler method is called by PLTLayer whenever a 
        /// Plantronics event occurs, e.g. device events, call state events etc.
        /// 
        /// By examining the "e.EventType" and "e.MyParams" parameters, your
        /// app can see what the event was and additional information about the event.
        /// For example, in the case of EventType SerialNumber, you get 2 MyParams
        /// strings, the first is the Serial Number (also known as Genes id), the second 
        /// contains whether it is a Base or Headset serial.
        /// </summary>
        public event PltEventHandler PltEvent;

        private void OnPltEvent(PltEventArgs e)
        {
            if (PltEvent != null)
                PltEvent(this, e);
        }
        #endregion

        /// <summary>
        /// Call setup to instruct PLTLayer object to connect to Plantronics Hub runtime engine 
        /// and register itself so that it can begin to communicate with the attached Plantronics device.
        /// </summary>
        /// <param name="AppName">Optional name of your appplication's session within Spokes runtime engine. If omitted it will default to "PLTLayer App".</param>
        public void setup(string AppName = "PLTLayer App")
        {
            try
            {
                // Obtain Spokes Wrapper singtleton object instance
                m_spokes = Spokes.Instance;
                // request log info
                m_spokes.SetLogger(this);

                // Sign up for Plantronics events of interest...

                // Wearing sensor:
                m_spokes.PutOn += new Spokes.PutOnEventHandler(m_spokes_PutOn);
                m_spokes.TakenOff += new Spokes.TakenOffEventHandler(m_spokes_TakenOff);

                // Proximity:
                m_spokes.Near += new Spokes.NearEventHandler(m_spokes_Near);
                m_spokes.Far += new Spokes.FarEventHandler(m_spokes_Far);
                m_spokes.InRange += new Spokes.InRangeEventHandler(m_spokes_InRange);
                m_spokes.OutOfRange += new Spokes.OutOfRangeEventHandler(m_spokes_OutOfRange);
                m_spokes.Docked += new Spokes.DockedEventHandler(m_spokes_Docked);
                m_spokes.UnDocked += new Spokes.DockedEventHandler(m_spokes_UnDocked);

                // Mobile caller id:
                m_spokes.MobileCallerId += new Spokes.MobileCallerIdEventHandler(m_spokes_MobileCallerId);
                m_spokes.OnMobileCall += new Spokes.OnMobileCallEventHandler(m_spokes_OnMobileCall);
                m_spokes.NotOnMobileCall += new Spokes.NotOnMobileCallEventHandler(m_spokes_NotOnMobileCall);

                // Serial Number:
                m_spokes.SerialNumber += new Spokes.SerialNumberEventHandler(m_spokes_SerialNumber);

                // Call control:
                m_spokes.CallAnswered += new Spokes.CallAnsweredEventHandler(m_spokes_CallAnswered);
                m_spokes.CallEnded += new Spokes.CallEndedEventHandler(m_spokes_CallEnded);
                m_spokes.CallSwitched += new Spokes.CallSwitchedEventHandler(m_spokes_CallSwitched);
                m_spokes.OnCall += new Spokes.OnCallEventHandler(m_spokes_OnCall);
                m_spokes.NotOnCall += new Spokes.NotOnCallEventHandler(m_spokes_NotOnCall);
                m_spokes.MuteChanged += new Spokes.MuteChangedEventHandler(m_spokes_MuteChanged);
                m_spokes.CallRequested += new Spokes.CallRequestedEventHandler(m_spokes_CallRequested);

                // Device attach/detach:
                m_spokes.Attached += new Spokes.AttachedEventHandler(m_spokes_Attached);
                m_spokes.Detached += new Spokes.DetachedEventHandler(m_spokes_Detached);
                m_spokes.CapabilitiesChanged += new Spokes.CapabilitiesChangedEventHandler(m_spokes_CapabilitiesChanged);

                // Multiline:
                m_spokes.MultiLineStateChanged += new Spokes.MultiLineStateChangedEventHandler(m_spokes_MultiLineStateChanged);

                // Button presses
                m_spokes.ButtonPress += new Spokes.ButtonPressEventHandler(m_spokes_ButtonPress);
                m_spokes.BaseButtonPress += new Spokes.BaseButtonPressEventHandler(m_spokes_BaseButtonPress);

                // Connect to Plantronics SDK
                m_IsConnected = m_spokes.Connect(AppName);
            }
            catch (Exception e)
            {
                throw new Exception("Failed to connect to Plantronics SDK. See inner exception.", e);
            }
        }

        void m_spokes_BaseButtonPress(object sender, BaseButtonPressArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.BaseButtonPressed,
                ((int)e.baseButton).ToString()));
        }

        void m_spokes_ButtonPress(object sender, ButtonPressArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.ButtonPressed, 
                ((int)e.headsetButton).ToString()));
        }

        void m_spokes_MultiLineStateChanged(object sender, MultiLineStateArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.MultiLineStateChanged,
                e.MultiLineState.PCActive.ToString(),
                e.MultiLineState.PCHeld.ToString(),
                e.MultiLineState.MobileActive.ToString(),
                e.MultiLineState.MobileHeld.ToString(),
                e.MultiLineState.DeskphoneActive.ToString(),
                e.MultiLineState.DeskphoneHeld.ToString()));
        }

        void m_spokes_CapabilitiesChanged(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.CapabilitiesChanged,
                m_spokes.DeviceCapabilities.HasDocking.ToString(),
                m_spokes.DeviceCapabilities.HasMobCallerId.ToString(),
                m_spokes.DeviceCapabilities.HasMobCallState.ToString(),
                m_spokes.DeviceCapabilities.HasMultiline.ToString(),
                m_spokes.DeviceCapabilities.HasProximity.ToString(),
                m_spokes.DeviceCapabilities.HasWearingSensor.ToString(),
                m_spokes.DeviceCapabilities.IsWireless.ToString(),
                m_spokes.DeviceCapabilities.ProductId));
        }

        void m_spokes_Detached(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.Detached));
        }

        void m_spokes_CallRequested(object sender, CallRequestedArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.CallRequested, 
                e.m_contact.Email,
                e.m_contact.FriendlyName,
                e.m_contact.HomePhone,
                e.m_contact.Id.ToString(),
                e.m_contact.MobilePhone,
                e.m_contact.Name,
                e.m_contact.Phone,
                e.m_contact.SipUri,
                e.m_contact.WorkPhone));
        }

        void m_spokes_NotOnCall(object sender, NotOnCallArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.NotOnCall, e.CallId.ToString(), e.CallSource));
        }

        void m_spokes_OnCall(object sender, OnCallArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.OnCall, e.CallId.ToString(), e.CallSource,
                e.Incoming.ToString(), e.State.ToString()));
        }

        void m_spokes_CallSwitched(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.CallSwitched));
        }

        void m_spokes_CallEnded(object sender, CallEndedArgs e)
        {
            // find the last incoming call in list
            PLTCallId theCall = new PLTCallId();
            bool found = false;
            foreach (PLTCallId cid in m_callids)
            {
                if (cid.isIncoming)
                {
                    theCall = cid;
                    found = true;
                }
            }
            if (found) m_callids.Remove(theCall);

            OnPltEvent(new PltEventArgs(PltEventType.CallEnded, e.CallId.ToString(), e.CallSource));
        }

        void m_spokes_CallAnswered(object sender, CallAnsweredArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.CallAnswered, e.CallId.ToString(), e.CallSource));
        }

        void m_spokes_SerialNumber(object sender, SerialNumberArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.SerialNumber, e.SerialNumber, e.SerialNumberType.ToString()));
        }

        void m_spokes_NotOnMobileCall(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.NotOnMobileCall));
        }

        void m_spokes_OnMobileCall(object sender, OnMobileCallArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.OnMobileCall, e.Incoming.ToString(), e.State.ToString()));
        }

        void m_spokes_MobileCallerId(object sender, MobileCallerIdArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.MobileCallerId, e.MobileCallerId));
        }

        void m_spokes_OutOfRange(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.OutOfRange));
        }

        void m_spokes_InRange(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.InRange));
        }

        void m_spokes_Far(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.Far));
        }

        void m_spokes_Near(object sender, EventArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.Near));
        }

        void m_spokes_TakenOff(object sender, WearingStateArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.TakenOff));
        }

        void m_spokes_PutOn(object sender, WearingStateArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.PutOn));
        }

        public void shutdown()
        {
            if (m_IsConnected) m_spokes.Disconnect();
            m_IsConnected = false;
        }

        void m_spokes_MuteChanged(object sender, MuteChangedArgs e)
        {
            if (m_muted != e.m_muteon)
                OnPltEvent(new PltEventArgs(e.m_muteon ? PltEventType.Muted : PltEventType.UnMuted));
            m_muted = e.m_muteon;
        }

        void m_spokes_Docked(object sender, DockedStateArgs e)
        {
            if (e.m_isInitialStateEvent || m_docked != e.m_docked)
                OnPltEvent(new PltEventArgs(PltEventType.Docked));
            m_docked = e.m_docked;
        }

        void m_spokes_UnDocked(object sender, DockedStateArgs e)
        {
            if (e.m_isInitialStateEvent || m_docked != e.m_docked)
                OnPltEvent(new PltEventArgs(PltEventType.UnDocked));
            m_docked = e.m_docked;
        }

        void m_spokes_Attached(object sender, AttachedArgs e)
        {
            OnPltEvent(new PltEventArgs(PltEventType.Attached, 
                e.m_device.ProductName.ToString(), 
                e.m_device.ProductId.ToString()));
        }

        /// <summary>
        /// This method will pass debugging information to an attached
        /// application object that is implementing the Spokes DebugLogger
        /// interface, allowing it to receive detailed Spokes SDK debug.
        /// Your DebugLogger implementation will also need to call 
        /// SetLogger(this) to start receiving the messages.
        /// 
        /// Alternatively for simple command line applications you can
        /// enabled console debugging by calling the SetConsoleLogging(true)
        /// method.
        /// </summary>
        /// <param name="methodname">The method the debug occured in.</param>
        /// <param name="str">The contents of the debug message.</param>
        /// <seealso cref="SetLogger"/>
        /// <seealso cref="SetConsoleLogging"/>
        public void DebugPrint(string methodname, string str)
        {
            if (m_debuglog != null)
                m_debuglog.DebugPrint(methodname, str);
            if (m_consolelogging)
            {
                StringBuilder sb = new StringBuilder();
                sb.Append(DateTime.Now.ToShortTimeString());
                sb.Append(" ");
                sb.Append(methodname);
                sb.Append(" ");
                sb.Append(str);
                Console.WriteLine(sb.ToString());
            }
        }

        /// <summary>
        /// This method is to notify Plantronics of a phone call in your softphone application, 
        /// adding it to the Plantronics CallManager (call control system) and enabling Plantronics 
        /// to set up the device audio path (in the case of wireless devices), and enable the device 
        /// ringing signal (for incoming calls, for devices with an internal ring alert).
        /// </summary>
        /// <param name="incoming">Specifies if the call incoming (true) or outgoing (false)</param>
        /// <param name="callid">A arbitrary, unique numeric 32-bit integer id that your 
        /// softphone application needs to specify for this call. Future call control events 
        /// from Plantronics during the call life-cycle of this call will include the specified id.
        /// </param>
        /// <param name="contactname">A string consisting of the contact name that is calling
        /// or being called, that will appear on Plantronics display capable devices, 
        /// e.g. Calisto P240.</param>
        public void on(bool incoming, int callid, string contactname = "")
        {
            PLTCallId id = GenerateCallId(callid, incoming);
            if (incoming)
            {
                m_spokes.IncomingCall(callid, contactname);
            }
            else
            {
                m_spokes.OutgoingCall(callid, contactname);
            }
            StoreCallId(id);
        }

        /// <summary>
        /// This override will auto-generate a callid internally. 
        /// This method is to notify Plantronics of a phone call in your softphone application, 
        /// adding it to the Plantronics CallManager (call control system) and enabling Plantronics 
        /// to set up the device audio path (in the case of wireless devices), and enable the device 
        /// ringing signal (for incoming calls, for devices with an internal ring alert).
        /// </summary>
        /// <param name="incoming">Specifies if the call incoming (true) or outgoing (false)</param>
        /// <param name="contactname">A string consisting of the contact name that is calling
        /// or being called, that will appear on Plantronics display capable devices, 
        /// e.g. Calisto P240.</param>
        /// <returns>An auto-generated, arbitrary, unique numeric 32-bit integer id that your 
        /// softphone application can optionally use to compare with future call control events 
        /// from Plantronics during the call life-cycle of this call that will include this id.</returns>
        public int on(bool incoming, string contactname = "")
        {
            PLTCallId id = GenerateCallId(incoming);
            if (incoming)
            {
                m_spokes.IncomingCall(id.callid, "");
            }
            else
            {
                m_spokes.OutgoingCall(id.callid, "");
            }
            StoreCallId(id);
            return id.callid;
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to activate the specified phone line.
        /// </summary>
        /// <param name="line">Enum value of the line to activate, PC, Mobile or Desk Phone</param>
        public void on(PLTLine line)
        {
            m_spokes.SetLineActive(ConvertLineType(line), true);
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to activate the specified phone line.
        /// </summary>
        /// <param name="line">Integer specifying line to activate, PC=1, Mobile=2 or Desk Phone=3</param>
        public void lineon(int line)
        {
            m_spokes.SetLineActive(ConvertLineType((PLTLine)line), true);
        }

        /// <summary>
        /// This method is to notify Plantronics of an incoming phone call in your softphone application, 
        /// adding it to the Plantronics CallManager (call control system) and enabling Plantronics 
        /// to set up the device audio path (in the case of wireless devices), and enable the device 
        /// ringing signal (for incoming calls, for devices with an internal ring alert).
        /// </summary>
        /// <param name="callid">A arbitrary, unique numeric 32-bit integer id that your 
        /// softphone application needs to specify for this call. Future call control events 
        /// from Plantronics during the call life-cycle of this call will include the specified id.
        /// </param>
        /// <param name="contactname">A string consisting of the contact name that is calling
        /// or being called, that will appear on Plantronics display capable devices, 
        /// e.g. Calisto P240.</param>
        public void ring(int callid, string contactname = "")
        {
            on(true, callid, contactname);
        }

        /// <summary>
        /// This override will auto-generate a callid internally. 
        /// This method is to notify Plantronics of an incoming phone call in your softphone application, 
        /// adding it to the Plantronics CallManager (call control system) and enabling Plantronics 
        /// to set up the device audio path (in the case of wireless devices), and enable the device 
        /// ringing signal (for incoming calls, for devices with an internal ring alert).
        /// </summary>
        /// <param name="contactname">A string consisting of the contact name that is calling
        /// or being called, that will appear on Plantronics display capable devices, 
        /// e.g. Calisto P240.</param>
        /// <returns>An auto-generated, arbitrary, unique numeric 32-bit integer id that your 
        /// softphone application can optionally use to compare with future call control events 
        /// from Plantronics during the call life-cycle of this call that will include this id.</returns>
        public int ring(string contactname = "")
        {
            return on(true, contactname);
        }

        /// <summary>
        /// Informs Spokes that user has answered an incoming (ringing) softphone call,
        /// for example with a softphone GUI.
        /// </summary>
        /// <param name="callid">The unique numeric id that defines which softphone call you want to answer.</param>
        public void ans(int callid)
        {
            bool found = false;
            PLTCallId theCall = FindCall(callid, ref found);
            if (found)
            {
                m_spokes.AnswerCall(callid);
            }
        }

        // 
        /// <summary>
        /// Informs Spokes that user has answered an incoming (ringing) softphone call,
        /// for example with a softphone GUI.
        /// This override will just answer the last incoming call that occured.
        /// </summary>
        public void ans()
        {
            // find the last incoming call in list
            PLTCallId theCall = new PLTCallId();
            bool found = false;
            foreach (PLTCallId cid in m_callids)
            {
                if (cid.isIncoming)
                {
                    theCall = cid;
                    found = true;
                }
            }
            if (found) m_spokes.AnswerCall(theCall.callid);
        }

        /// <summary>
        /// This method is to notify Plantronics that a call in your softphone application has
        /// ended. Plantronics will close the audio path (for wireless devices), and remove
        /// the record of the call from the Plantronics CallManager (call control system).
        /// </summary>
        /// <param name="callid">The call id of the call that was ended</param>
        public void off(int callid)
        {
            RemoveCall(callid);
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to deactivate the specified phone line.
        /// </summary>
        /// <param name="line">Enum value of the line to deactivate, PC, Mobile or Desk Phone</param>
        public void off(PLTLine line)
        {
            m_spokes.SetLineActive(ConvertLineType(line), false);
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to deactivate the specified phone line.
        /// </summary>
        /// <param name="line">Integer specifying line to deactivate, PC=1, Mobile=2 or Desk Phone=3</param>
        public void lineoff(int line)
        {
            m_spokes.SetLineActive(ConvertLineType((PLTLine)line), false);
        }

        /// <summary>
        /// This method is to notify Plantronics that a call or calls in your softphone 
        /// application has ended. Plantronics will close the audio path (for wireless devices), 
        /// and remove the record of the call from the Plantronics CallManager (call control system).
        /// NOTE: this override with no parameters will tell Plantronics to effectively RESET its
        /// knowledge of ALL calls that were in your softphone app.
        /// </summary>
        public void off()
        {
            RemoveAllCalls();
        }

        /// <summary>
        /// This method enables or disables the mute feature of the attached Plantronics
        /// device.
        /// </summary>
        /// <param name="mute">True to activate mute, false to de-activate it.</param>
        public void mute(bool mute)
        {
            m_spokes.SetMute(mute);
        }

        // hold specified call

        /// <summary>
        /// This method will inform Plantronics that user has held or resumed a 
        /// call in your application, the call identified by specified call id.
        /// </summary>
        /// <param name="callid">Integer id identifying call to hold or resume</param>
        /// <param name="hold">true = hold, false = resume</param>
        public void hold(int callid, bool hold)
        {
            if (hold)
            {
                m_spokes.HoldCall(callid);
            }
            else
            {
                m_spokes.ResumeCall(callid);
            }
        }

        /// <summary>
        /// This method will inform Plantronics that user has held or resumed a 
        /// call in your application. This override will hold/resume the most
        /// recent active call. The effect on Plantronics is usually that the
        /// green active light on device changes to red to indicate the call is
        /// on hold.
        /// </summary>
        /// <param name="hold">true = hold, false = resume</param>
        public void hold(bool hold)
        {
            if (m_callids.Count() > 0)
            {
                PLTCallId theCall = m_callids.Last();
                if (hold)
                {
                    m_spokes.HoldCall(theCall.callid);
                }
                else
                {
                    m_spokes.ResumeCall(theCall.callid);
                }
            }
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to hold or resume the specified phone line.
        /// </summary>
        /// <param name="line">Enum value of the line to hold/resume, PC, Mobile or Desk Phone</param>
        public void hold(PLTLine line, bool hold)
        {
            m_spokes.SetLineHold(ConvertLineType(line), hold);
        }

        /// <summary>
        /// Instruct the Plantronics multiline device to hold or resume the specified phone line.
        /// </summary>
        /// <param name="line">Integer value of the line to hold/resume, PC=1, Mobile=2 or Desk Phone=3</param>
        public void linehold(int line, bool hold)
        {
            this.hold((PLTLine)line, hold);
        }

        #region convenience functions
        private static Multiline_LineType ConvertLineType(PLTLine line)
        {
            Multiline_LineType retvalline;
            switch (line)
            {
                case PLTLine.pc:
                    retvalline = Multiline_LineType.PC;
                    break;
                case PLTLine.mobile:
                    retvalline = Multiline_LineType.Mobile;
                    break;
                case PLTLine.desk:
                    retvalline = Multiline_LineType.Deskphone;
                    break;
                default:
                    retvalline = Multiline_LineType.PC;
                    break;
            }
            return retvalline;
        }

        private PLTCallId GenerateCallId(bool isIncoming)
        {
            PLTCallId retval = new PLTCallId();
            retval.callid = ++m_internalcallid;
            retval.isInternal = true;
            retval.isIncoming = isIncoming;
            return retval;
        }

        private PLTCallId GenerateCallId(int callid, bool isIncoming)
        {
            PLTCallId retval = new PLTCallId();
            retval.callid = callid;
            retval.isInternal = false;
            retval.isIncoming = isIncoming;
            return retval;
        }

        private void StoreCallId(PLTCallId id)
        {
            m_callids.Add(id);
        }

        private void RemoveCall(int id)
        {
            bool found = false;
            PLTCallId matched = FindCall(id, ref found);           
            if (found)
            {
                m_callids.Remove(matched);
                m_spokes.EndCall(id);
            }
        }

        private PLTCallId FindCall(int id, ref bool found)
        {
            PLTCallId matched = new PLTCallId();
            foreach (PLTCallId cid in m_callids)
            {
                if (cid.callid == id)
                {
                    matched = cid;
                    found = true;
                }
            }
            return matched;
        }

        private void RemoveAllCalls()
        {
            foreach (PLTCallId cid in m_callids)
            {
                m_spokes.EndCall(cid.callid);
            }
            m_callids.Clear();
        }
        #endregion

        /// <summary>
        /// This method will ask Plantronics to query the Genes IDs (serial numbers) of the attached
        /// Plantronics device (base serial, and where applicable headset serial). The serial numbers
        /// will arrive later as a PltEvent with EventType of SerialNumber
        /// </summary>
        public void getgenes()
        {
            m_spokes.RequestAllSerialNumbers();
        }

        /// <summary>
        /// Instructs a mobile that is paired with Plantronics device to dial an outbound mobile call.
        /// </summary>
        /// <param name="numbertodial">The phone number you wish the mobile to call.</param>
        public void dialmob(string numbertodial)
        {
            if (numbertodial.Length > 0)
            {
                m_spokes.DialMobileCall(numbertodial);
            }
        }

        /// <summary>
        /// Instructs a mobile that is paired with Plantronics device to answer an inbound (ringing) mobile call
        /// </summary>
        public void ansmob()
        {
            m_spokes.AnswerMobileCall();
        }

        /// <summary>
        /// Instructs a mobile that is paired with Plantronics device to reject an incoming (ringing) mobile call
        /// </summary>
        public void rejmob()
        {
            m_spokes.EndMobileCall();
        }

        /// <summary>
        /// Instructs a mobile that is paired with Plantronics device to end on ongoing mobile call
        /// </summary>
        public void endmob()
        {
            m_spokes.EndMobileCall();
        }
    }

    #region Plt Event Types and Args
    /// <summary>
    /// This enum defines the types of events that can be received from Plantronics
    /// via the PltEvent event handler.
    /// </summary>
    public enum PltEventType
    {
        Docked = 1,
        UnDocked,
        Muted,
        UnMuted,
        Attached,
        Detached,
        BaseButtonPressed,
        ButtonPressed,
        MultiLineStateChanged,
        TakenOff,
        SerialNumber,
        PutOn,
        OutOfRange,
        OnMobileCall,
        OnCall,
        NotOnMobileCall,
        NotOnCall,
        Near,
        MobileCallerId,
        InRange,
        Far,
        CapabilitiesChanged,
        CallSwitched,
        CallRequested,
        CallEnded,
        CallAnswered
    }

    /// <summary>
    /// The PltEventArgs defines the event parameters of events from
    /// Plantronics received via the PltEvent event handler.
    /// Including the EventType, and MyParams.
    /// </summary>
    public class PltEventArgs : EventArgs
    {
        public PltEventType EventType { get; set; }
        public string EventTypeStr { get; set; }
        public List<string> MyParams { get; set; }

        public PltEventArgs(PltEventType eventType, params string[] list)
        {
            EventType = eventType;
            EventTypeStr = eventType.ToString();
            MyParams = null;
            // any parameters?
            if (list.Count() > 0)
            {
                MyParams = new List<string>();
                foreach (string param in list)
                {
                    MyParams.Add(param);
                }
            }
        }
    }

    /// <summary>
    /// An enum defining the 3 line types of multiline device (e.g. Savi 700 Series)
    /// </summary>
    public enum PLTLine
    {
        pc = 1,
        mobile,
        desk
    }

    /// <summary>
    /// A struct to model a call id in the system.
    /// These can either be internal (auto generated) or
    /// supplied by calling application (not internal).
    /// </summary>
    public struct PLTCallId
    {
        public bool isInternal;
        public int callid;
        public bool isIncoming;
    }
    #endregion
}
