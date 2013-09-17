using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using Plantronics.UC.SpokesWrapper; // Spokes .NET interop
using System.Speech.Synthesis; // used for simulated phone audio

/*******
 * 
 * Spokes Easy Demo
 * 
 * A demo app that shows use of SpokesWrapper.cs to make it easier to integrate any apps with Plantronics devices.
 * 
 * This application allows you to:
 * 
 *   - Simulate incoming or outgoing softphone calls and ask Spokes to handle the call control of those.
 *     For incoming Spokes will automatically ring the headset and notifying the app when the call has been answered.
 *     
 *   - Plays test phone audio that keeps the audio/rf link open for wireless products and allows you try the mute/unmute via headset buttons.
 *     
 *     **IMPORTANT NOTE:** For the test phone audio to work correctly please set the Plantronics
 *     headset as the **DEFAULT** playback device in Windows before simulating the softphone calls. 
 *     
 *   - Control the mobile phone that is paired with the Plantronics device to dial, answer, reject and end
 *     mobile calls.
 *     
 *   - Display the name and serial numbers (headset and base/usb adaptor) of the attached Plantronics device.
 *   
 *   - Display the status of the attached Plantronics device (depending if supported by device), covering the following areas:
 *     Wearing State, Proximity State, Docked State, Mobile Call State, Call State, Mute State
 *     
 *   - Show a log of debug information from the method calls and events of Spokes.
 * 
 * Lewis Collins, 30th Nov 2012
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.1.0.4:
 * Date: 19th July 2013
 * Changed by: Lewis Collins
 *   Changes:
 *     - Remove SpokesWrapper.cs as file and re-add as link to new github wrappers repository folder:
 *       i.e. https://github.com/pltdev/wrappers
 *     - Fixes in wrapper to check if Spokes is installed before attempting connect
 *
 * Version 1.1.0.3:
 * Date: 17th June 2013
 * Changed by: Lewis Collins
 *   Changes:
 *     - Fixes in wrapper to address the detection of initial undock for Savi 7xx
 *
 * Version 1.1.0.2:
 * Date: 30th May 2013
 * Changed by: Lewis Collins
 *   Changes:
 *     - Fixes in wrapper to address the detection of docked/undocked for Legend UC
 *
 * Version 1.1.0.1:
 * Date: 23rd April 2013
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding red animations on device state icons that change
 *     - Made windows resizable
 *
 * Version 1.1.0.0:
 * Date: 12th April 2013
 * Changed by: Lewis Collins
 *   Changes:
 *     - UX graphics added!
 *
 * Version 1.0.1:
 * Date: 4th December 2012
 * Changed by: Lewis Collins
 *   Changes:
 *     - Adding to demo multiline device features e.g. for Savi 7xx
 *
 * Version 1.0:
 * Date: 30th November 2012
 *   Initial version.
 * ********************************************************************************
 *
 **/


namespace Spokes_Easy_Demo
{
    public partial class Form1 : Form, DebugLogger
    {
        // The Spokes object singleton
        Spokes m_spokes;

        // My simulated softphone internal state...
        int m_callId = 0; // used to generate call id's
        bool m_oncall = false;
        SpeechSynthesizer m_dummyspeechaudio; // used for simulated phone audio
        static string APP_NAME = "Spokes Easy Demo Softphone";
        private bool m_worn;

        private EventsLogForm eventslog = null;

        #region GUI Callbacks
        delegate void LogMessageCallback(string callingmethodname, string message);
        delegate void UpdateLabelCallback(Control label, string message);
        delegate void UpdatePictureBoxCallback(PictureBox picturebox, Bitmap picture);
        delegate void SetEnableCallback(Control guiitem, bool enable, Bitmap pictureBoxImage = null);
        delegate void SetCallControlGUIEnableCallback(bool oncall);
        delegate void SetMobileCallControlGUIEnableCallback(MobileCallState state);
        delegate void UpdateMultiLineButtonTextsCallback(MultiLineStateArgs e);
        delegate void SetPictureBoxColorCallback(Control pbox, Color col);
        #endregion

        Timer wearingpicboxtimer;
        private string m_productname = "";
        private MultiLineStateArgs m_lastMultiLineState;
        private bool m_debugmode = false;
        private bool m_onmobcall = false;

        // New animation timer list for highlighting background color of properties that change!
        // idea: at expire time restore the background color of the picture box (then remove this from the list)
        class AnimTimerInfo
        {
            public DateTime expireTime;
            public Color newBackground;
            public Color origBackground;
            public Color currBackground;
            public Control guicontrol; // item to animate color of
        }

        List<AnimTimerInfo> m_animtimers;
        Timer m_animtimer;
        private Object thisLock = new Object();

        public Form1()
        {
            InitializeComponent();

            MultiLineStateFlags initial_activeHeldFlags = new MultiLineStateFlags();
            initial_activeHeldFlags.PCActive = false;
            initial_activeHeldFlags.MobileActive = false;
            initial_activeHeldFlags.DeskphoneActive = false;
            initial_activeHeldFlags.PCHeld = false;
            initial_activeHeldFlags.MobileHeld = false;
            initial_activeHeldFlags.DeskphoneHeld = false;
            m_lastMultiLineState = new MultiLineStateArgs(initial_activeHeldFlags);

            // Extra GUI init...
            ExtraGUIInit();

            m_dummyspeechaudio = new SpeechSynthesizer();
            m_dummyspeechaudio.SetOutputToDefaultAudioDevice();

            wearingpicboxtimer = new Timer();
            wearingpicboxtimer.Interval = 500;
            wearingpicboxtimer.Tick += wearingpicboxtimer_Tick;

            m_animtimers = new List<AnimTimerInfo>();
            m_animtimer = new Timer();
            m_animtimer.Interval = 40;
            m_animtimer.Tick += m_animtimer_Tick;
            m_animtimer.Start();
        }

        void m_animtimer_Tick(object sender, EventArgs e)
        {
            if (m_animtimers.Count > 0)
            {
                List<AnimTimerInfo> killList = new List<AnimTimerInfo>();
                DateTime now = DateTime.Now;
                lock (thisLock)
                {
                    foreach (AnimTimerInfo animtimer in m_animtimers)
                    {
                        if (animtimer.expireTime < now)
                        {
                            // I have expired
                            SetPictureBoxColor(animtimer.guicontrol, animtimer.origBackground);
                            killList.Add(animtimer);
                        }
                        else if (animtimer.expireTime - now < TimeSpan.FromMilliseconds(1000))
                        {
                            // 1 second from the end of new color, start animating back to old color...
                            // not expired, so animate stepwise between 
                            int Rcur = animtimer.currBackground.R;
                            int Gcur = animtimer.currBackground.G;
                            int Bcur = animtimer.currBackground.B;

                            Rcur = Rcur + 10;
                            Gcur = Gcur + 10;
                            Bcur = Bcur + 10;

                            if (Rcur > 255) Rcur = 255;
                            if (Gcur > 255) Gcur = 255;
                            if (Bcur > 255) Bcur = 255;

                            animtimer.currBackground = Color.FromArgb(Rcur, Gcur, Bcur);
                            SetPictureBoxColor(animtimer.guicontrol, animtimer.currBackground);
                        }
                        else
                        {
                            // >1 second from the end of new color, climb color up towards target color quickly
                            // not expired, so animate stepwise between 
                            // current color
                            int Rcur = animtimer.currBackground.R;
                            int Gcur = animtimer.currBackground.G;
                            int Bcur = animtimer.currBackground.B;

                            // target color
                            int TRcur = animtimer.newBackground.R;
                            int TGcur = animtimer.newBackground.G;
                            int TBcur = animtimer.newBackground.B;

                            if (Rcur > TRcur) Rcur = Rcur - 40;
                            if (Gcur > TGcur) Gcur = Gcur - 40;
                            if (Bcur > TBcur) Bcur = Bcur - 40;

                            if (Rcur < TRcur) Rcur = TRcur;
                            if (Gcur < TGcur) Gcur = TGcur;
                            if (Bcur < TBcur) Bcur = TBcur;

                            animtimer.currBackground = Color.FromArgb(Rcur, Gcur, Bcur);
                            SetPictureBoxColor(animtimer.guicontrol, animtimer.currBackground);
                        }
                    }

                    // kill the dead ones
                    foreach (AnimTimerInfo animtimer in killList)
                    {
                        SetPictureBoxColor(animtimer.guicontrol, animtimer.origBackground);
                        m_animtimers.Remove(animtimer);
                    }
                }
            }
        }

        private void SetPictureBoxColor(Control pbox, Color col)
        {
            if (pbox.InvokeRequired)
            {
                SetPictureBoxColorCallback d = new SetPictureBoxColorCallback(SetPictureBoxColor);
                this.Invoke(d, new object[] { pbox, col });
            }
            else
            {
                pbox.BackColor = col;
                //proximityStateLbl.Text = col.ToString();
                pbox.Update();
            }
        }

        void wearingpicboxtimer_Tick(object sender, EventArgs e)
        {
            UpdateDevicePictureBox(wearingStatePictureBox, m_worn ? Properties.Resources.wearing_don :
                Properties.Resources.wearing_doff);
            wearingpicboxtimer.Stop();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, "Spokes Easy Demo Loading main form.");

            // Get the Spokes object singleton
            m_spokes = Spokes.Instance;

            m_spokes.SetLogger(this); // tell spokes to log debug output to me using DebugLogger interface

            // Sign up for Plantronics events of interest...

            // Wearing sensor:
            m_spokes.PutOn += spokes_DevicePutOn;
            m_spokes.TakenOff += spokes_DeviceTakenOff;

            // Proximity:
            m_spokes.Near += spokes_Near;
            m_spokes.Far += spokes_Far;
            m_spokes.InRange += spokes_InRange;
            m_spokes.OutOfRange += spokes_OutOfRange;
            m_spokes.Docked += spokes_Docked;
            m_spokes.UnDocked += spokes_UnDocked;

            // Mobile caller id:
            m_spokes.MobileCallerId += spokes_MobileCallerId;
            m_spokes.OnMobileCall += m_spokes_OnMobileCall;
            m_spokes.NotOnMobileCall += m_spokes_NotOnMobileCall;

            // Serial Number:
            m_spokes.SerialNumber += spokes_SerialNumber;

            // Call control:
            m_spokes.CallAnswered += spokes_CallAnswered;
            m_spokes.CallEnded += spokes_CallEnded;
            m_spokes.CallSwitched += spokes_CallSwitched;
            m_spokes.OnCall += spokes_OnCall;
            m_spokes.NotOnCall += spokes_NotOnCall;
            m_spokes.MuteChanged += spokes_DeviceMuteChanged;
            m_spokes.CallRequested += m_spokes_CallRequested;

            // Device attach/detach:
            m_spokes.Attached += spokes_DeviceAttached;
            m_spokes.Detached += spokes_DeviceDetached;
            m_spokes.CapabilitiesChanged += spokes_DeviceCapabilitiesChanged;

            // Multiline:
            m_spokes.MultiLineStateChanged += m_spokes_MultiLineStateChanged;

            // Button presses
            m_spokes.ButtonPress += m_spokes_ButtonPress;
            m_spokes.BaseButtonPress += m_spokes_BaseButtonPress;

            Show();
            Update();

            // Now connect to attached device, if any
            m_spokes.Connect(APP_NAME);
        }

        void m_spokes_BaseButtonPress(object sender, BaseButtonPressArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> App got a BASE BUTTON PRESS: " + e.baseButton.ToString());
        }

        void m_spokes_ButtonPress(object sender, ButtonPressArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> App got a BUTTON PRESS: " + e.headsetButton.ToString());
        }

        void m_spokes_CallRequested(object sender, CallRequestedArgs e)
        {
            // user has dialled using a Plantronics Dialpad device!
            // In response to this event my softphone should establish out
            // outgoing call using the contact info provided...
            if (e.m_contact.Phone.Length > 0)
            {
                // user dialled a number on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to number: " + e.m_contact.Phone);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.Phone);
            }
            else if (e.m_contact.MobilePhone.Length > 0)
            {
                // user dialled a mobile number on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to mobile number: " + e.m_contact.MobilePhone);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.MobilePhone);
            }
            else if (e.m_contact.HomePhone.Length > 0)
            {
                // user dialled a home number on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to home number: " + e.m_contact.HomePhone);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.HomePhone);
            }
            else if (e.m_contact.FriendlyName.Length > 0)
            {
                // user dialled a contact name on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to contact name: " + e.m_contact.FriendlyName);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.FriendlyName);
            }
            else if (e.m_contact.SipUri.Length > 0)
            {
                // user dialled a SipUri on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to SipUri: " + e.m_contact.SipUri);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.SipUri);
            }
            else if (e.m_contact.Email.Length > 0)
            {
                // user dialled an Email on their calisto...
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Making Outgoing call to Email: " + e.m_contact.Email);
                m_spokes.OutgoingCall(GetNewCallId(), e.m_contact.Email);
            }
        }

        void spokes_DevicePutOn(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device wearing state: is worn");
            UpdateDeviceStatusGUIItem(wearingStateLbl, "Worn");
            UpdateDevicePictureBox(wearingStatePictureBox, Properties.Resources.wearing_don);
            m_worn = true;
            if (((WearingStateArgs)e).m_isInitialStateEvent)
            {
                wearingpicboxtimer.Start(); // start a timer to update the picture box 
                 // after 500ms, otherwise the change doesn't work!
            }

            AnimateBackgroundColor(wearingStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_DeviceTakenOff(object sender, EventArgs e)
        {
            if (m_spokes.DeviceCapabilities.HasWearingSensor)
            {
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device wearing state: taken off");
                UpdateDeviceStatusGUIItem(wearingStateLbl, "Not Worn");
                UpdateDevicePictureBox(wearingStatePictureBox, Properties.Resources.wearing_doff);
                m_worn = false;
            }

            AnimateBackgroundColor(wearingStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_Near(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is near");
            UpdateDeviceStatusGUIItem(proximityStateLbl, "Near");
            UpdateDevicePictureBox(proximityPictureBox, Properties.Resources.proximity_near);

            AnimateBackgroundColor(proximityPictureBox, Color.FromArgb(0, 51, 102));
        }

        private void AnimateBackgroundColor(Control pbox, Color col)
        {
            bool hastimerforcombo = false;
            Color startBgCol = pbox.BackColor;
            AnimTimerInfo killItem = null;
            foreach (AnimTimerInfo inf in m_animtimers)
            {
                if (inf.guicontrol == pbox)
                {
                    hastimerforcombo = true;
                    startBgCol = inf.origBackground;
                    break;
                }
            }

            if (hastimerforcombo && killItem!=null) killItem.expireTime = DateTime.Now;

            AnimTimerInfo info = new AnimTimerInfo();
            info.guicontrol = pbox;
            info.origBackground = startBgCol; // original, fade up to original (white) color in final 1000ms
            info.newBackground = col; // target, fade down to this dark color in first 500ms
            info.currBackground = startBgCol;
            info.expireTime = DateTime.Now + TimeSpan.FromMilliseconds(1500); // 1500ms total anim time
            lock (thisLock)
            {
                m_animtimers.Add(info);
            }
            SetPictureBoxColor(pbox, startBgCol);
        }

        void spokes_Far(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is far");
            UpdateDeviceStatusGUIItem(proximityStateLbl, "Far");
            UpdateDevicePictureBox(proximityPictureBox, Properties.Resources.proximity_far);

            AnimateBackgroundColor(proximityPictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_InRange(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is in range");
            UpdateDeviceStatusGUIItem(proximityStateLbl, "In Range");
            UpdateDevicePictureBox(proximityPictureBox, Properties.Resources.proximity_unknown);

            AnimateBackgroundColor(proximityPictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_OutOfRange(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is out of range");
            UpdateDeviceStatusGUIItem(proximityStateLbl, "Out Of Range");
            UpdateDevicePictureBox(proximityPictureBox, Properties.Resources.proximity_unknown);

            AnimateBackgroundColor(proximityPictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_Docked(object sender, EventArgs e)
        {
            if (m_spokes.DeviceCapabilities.HasDocking)
            {
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is docked");
                UpdateDeviceStatusGUIItem(dockedStateLbl, "Docked");
                UpdateDevicePictureBox(dockedPictureBox, Properties.Resources.dock_docked);
            }

            AnimateBackgroundColor(dockedPictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_UnDocked(object sender, EventArgs e)
        {
            if (m_spokes.DeviceCapabilities.HasDocking)
            {
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device is undocked");
                UpdateDeviceStatusGUIItem(dockedStateLbl, "UnDocked");
                UpdateDevicePictureBox(dockedPictureBox, Properties.Resources.dock_undocked);
            }

            AnimateBackgroundColor(dockedPictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_MobileCallerId(object sender, MobileCallerIdArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Mobile Caller id: " + e.MobileCallerId);
            UpdateDeviceStatusGUIItem(mobileCallerIdLbl, e.MobileCallerId);
            UpdateDeviceStatusGUIItem(mobileNumberTextBox, e.MobileCallerId);

            AnimateBackgroundColor(mobileNumberTextBox, Color.FromArgb(0, 51, 102));
        }

        void m_spokes_OnMobileCall(object sender, OnMobileCallArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> On Mobile Call: " + e.State);
            UpdateDeviceStatusGUIItem(mobileStateLbl, e.State == MobileCallState.OnCall ? "On Call" : "Ringing");
            UpdateDeviceStatusGUIItem(mobileCallDirectionLbl, e.Incoming ? "Incoming " : "Outgoing");
            UpdateDevicePictureBox(mobileStatePictureBox, Properties.Resources.mobile_onCall);
            SetMobileCallControlGUIEnabled(e.State);
            m_onmobcall = true;


            AnimateBackgroundColor(mobileStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void m_spokes_NotOnMobileCall(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Not on Mobile Call");
            UpdateDeviceStatusGUIItem(mobileStateLbl, "Mobile idle");
            UpdateDeviceStatusGUIItem(mobileCallDirectionLbl, "");
            UpdateDeviceStatusGUIItem(mobileCallerIdLbl, "");
            UpdateDevicePictureBox(mobileStatePictureBox, Properties.Resources.mobile_idle);
            SetMobileCallControlGUIEnabled(MobileCallState.Idle);
            m_onmobcall = false;

            AnimateBackgroundColor(mobileStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_SerialNumber(object sender, SerialNumberArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> "+e.SerialNumberType.ToString()+" serial number is: " + e.SerialNumber);
            switch (e.SerialNumberType)
            {
                case SerialNumberTypes.Base:
                    UpdateDeviceStatusGUIItem(baseSerialNumberTxt, e.SerialNumber);
                    AnimateBackgroundColor(baseSerialNumberTxt, Color.FromArgb(0, 51, 102));
                    break;
                case SerialNumberTypes.Headset:
                    UpdateDeviceStatusGUIItem(headsetSerialNumberTxt, e.SerialNumber);
                    AnimateBackgroundColor(headsetSerialNumberTxt, Color.FromArgb(0, 51, 102));
                    break;
            }
        }

        void spokes_CallAnswered(object sender, CallAnsweredArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> User has answered call, call id: " + e.CallId);
            // if this was My Softphone's call then activate the audio link to headset
            if (e.CallId > 0 && e.CallSource.CompareTo(APP_NAME) == 0)
            {
                m_spokes.ConnectAudioLinkToDevice(true);
                SimulatePhoneAudio();
                m_spokes.SetMute(false);
            }
            else
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Ignoring spurious call event, call id: " + e.CallId+", call source = "+e.CallSource);
        }

        // Play some dummy audio especially to keep audio link
        // to wireless device open, so user can play with mute sync function
        private void SimulatePhoneAudio()
        {
            // build and speak a prompt
            PromptBuilder builder = new PromptBuilder();
            #region Dummy Phone Text
            builder.AppendText(
                @"And sails upon the bosom of the air.

JULIET

    O Romeo, Romeo! wherefore art thou Romeo?
    Deny thy father and refuse thy name;
    Or, if thou wilt not, be but sworn my love,
    And I'll no longer be a Capulet.

ROMEO

    [Aside] Shall I hear more, or shall I speak at this?

JULIET

    'Tis but thy name that is my enemy;
    Thou art thyself, though not a Montague.
    What's Montague? it is nor hand, nor foot,
    Nor arm, nor face, nor any other part
    Belonging to a man. O, be some other name!
    What's in a name? that which we call a rose
    By any other name would smell as sweet;
    So Romeo would, were he not Romeo call'd,
    Retain that dear perfection which he owes
    Without that title. Romeo, doff thy name,
    And for that name which is no part of thee
    Take all myself.

ROMEO

    I take thee at thy word:
    Call me but love, and I'll be new baptized;
    Henceforth I never will be Romeo.

JULIET

    What man art thou that thus bescreen'd in night
    So stumblest on my counsel?

ROMEO

    By a name
    I know not how to tell thee who I am:
    My name, dear saint, is hateful to myself,
    Because it is an enemy to thee;
    Had I it written, I would tear the word.

JULIET

    My ears have not yet drunk a hundred words
    Of that tongue's utterance, yet I know the sound:
    Art thou not Romeo and a Montague?

ROMEO

    Neither, fair saint, if either thee dislike.

JULIET

    How camest thou hither, tell me, and wherefore?
    The orchard walls are high and hard to climb,
    And the place death, considering who thou art,
    If any of my kinsmen find thee here.

ROMEO

    With love's light wings did I o'er-perch these walls;
    For stony limits cannot hold love out,
    And what love can do that dares love attempt;
    Therefore thy kinsmen are no let to me."
                );
            #endregion
            m_dummyspeechaudio.SpeakAsync(builder);
        }

        void spokes_CallEnded(object sender, CallEndedArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> User has ended call, call id: " + e.CallId);

            // if this was My Softphone's call then terminate the audio link to headset
            if (e.CallId > 0 && e.CallSource.CompareTo(APP_NAME) == 0)
            {
                m_dummyspeechaudio.SpeakAsyncCancelAll();
                m_spokes.SetMute(false);
                m_spokes.ConnectAudioLinkToDevice(false);
            }
            else
                LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Ignoring spurious call event, call id: " + e.CallId + ", call source = " + e.CallSource);
        }

        void spokes_CallSwitched(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> User has switched calls");
            m_spokes.ConnectAudioLinkToDevice(true);
        }

        void spokes_OnCall(object sender, OnCallArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> On a call, call source: " + e.CallSource + ", incoming? :" + e.Incoming);
            UpdateDeviceStatusGUIItem(callStateLbl, e.State == OnCallCallState.Ringing ? "Ringing" : "On Call");
            UpdateDeviceStatusGUIItem(callDirectionLbl, e.Incoming ? "Incoming" : "Outgoing");
            UpdateDeviceStatusGUIItem(callSourceLbl, e.CallSource);
            UpdateDevicePictureBox(callStatePictureBox, Properties.Resources.call_onCall);

            AnimateBackgroundColor(callStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_NotOnCall(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> NOT on a call: " + e.ToString());
            UpdateDeviceStatusGUIItem(callStateLbl, "Not On Call");
            UpdateDeviceStatusGUIItem(callDirectionLbl, "");
            UpdateDeviceStatusGUIItem(callSourceLbl, "");
            UpdateDevicePictureBox(callStatePictureBox, Properties.Resources.call_idle);
            SetCallControlGUIEnabled(false);

            AnimateBackgroundColor(callStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_DeviceMuteChanged(object sender, MuteChangedArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device mute = " + e.m_muteon);
            UpdateDeviceStatusGUIItem(muteStateLbl, e.m_muteon ? "Muted" : "Not Muted");
            UpdateDevicePictureBox(muteStatePictureBox,
                e.m_muteon ? Properties.Resources.mute_muted : Properties.Resources.mute_notMuted);

            AnimateBackgroundColor(muteStatePictureBox, Color.FromArgb(0, 51, 102));
        }

        void spokes_DeviceAttached(object sender, AttachedArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device was attached: " + e.m_device.ProductName);
            UpdateDeviceStatusGUIItem(deviceStatusLabel, e.m_device.ProductName);
            m_productname = e.m_device.ProductName;

            AnimateBackgroundColor(deviceStatusLabel, Color.FromArgb(0, 51, 102));
        }

        void spokes_DeviceDetached(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, ">>> Device was detached");
            UpdateDeviceStatusGUIItem(deviceStatusLabel, "(no device attached!)");
            m_productname = "";

            AnimateBackgroundColor(deviceStatusLabel, Color.FromArgb(0, 51, 102));
        }

        void spokes_DeviceCapabilitiesChanged(object sender, EventArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, "Device capabilities updated:\r\n" + m_spokes.DeviceCapabilities);

            // Enable or disable the GUI features depending on connected device's capabilities: 
            SetWearingStateGUIEnabled(m_spokes.DeviceCapabilities.HasWearingSensor);
            SetProximityStateGUIEnabled(m_spokes.DeviceCapabilities.HasProximity);
            SetMobileCallerGUIEnabled(m_spokes.DeviceCapabilities.HasMobCallerId, m_spokes.DeviceCapabilities.HasMobCallState);
            SetMobileCallControlGUIEnabled(MobileCallState.Idle);
            SetDockedGUIEnabled(m_spokes.DeviceCapabilities.HasDocking);
            SetMultiLineStateGUIEnabled(m_spokes.DeviceCapabilities.HasMultiline);

            // last 2 GUI items grey out just based on whether we have a device or not
            SetCallStateGUIEnabled(m_spokes.HasDevice);
            SetMuteStateGUIEnabled(m_spokes.HasDevice);
            SetCallControlStateGUIEnabled(m_spokes.HasDevice);

            SetEnable(requestHeadsetSerialBtn, m_spokes.HasDevice);
        }

        private void SetCallControlStateGUIEnabled(bool enable)
        {
            SetEnable(callDirectionCombo, enable);
            SetEnable(contactCombo, enable);
            SetEnable(connectCallBtn, enable);
            SetEnable(endCallBtn, enable);
        }

        private void SetMultiLineStateGUIEnabled(bool enable)
        {
            SetEnable(multiLineStateHdrLbl, enable);
            SetEnable(pcLineStateHdrLbl, enable);
            SetEnable(mobLineStateHdrLbl, enable);
            SetEnable(deskLineStateHdrLbl, enable);

            SetEnable(pcLineActiveBtn, enable);
            SetEnable(mobLineActiveBtn, enable);
            SetEnable(deskLineActiveBtn, enable);
            SetEnable(pcLineHoldBtn, enable);
            SetEnable(mobLineHoldBtn, enable);
            SetEnable(deskLineHoldBtn, enable);

            //SetEnable(multiLineStatePictureBox, enable,
            //    !enable ? Properties.Resources.multiline_greyed : Properties.Resources.multiline);

            SetEnable(pcLineActivePictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);
            SetEnable(mobLineActivePictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);
            SetEnable(deskLineActivePictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);

            SetEnable(pcLineHeldPictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);
            SetEnable(mobLineHeldPictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);
            SetEnable(deskLineHeldPictureBox, enable,
                !enable ? Properties.Resources.status_idle : Properties.Resources.status_idle);

            AnimateBackgroundColor(multiLineStateHdrLbl, Color.FromArgb(0, 51, 102));
        }

        void m_spokes_MultiLineStateChanged(object sender, MultiLineStateArgs e)
        {
            LogMessage(MethodInfo.GetCurrentMethod().Name, "Multiline state updated:\r\n" + e.ToString());

            // show active states
            SetEnable(pcLineActivePictureBox, e.MultiLineState.PCActive,
                !e.MultiLineState.PCActive ? Properties.Resources.status_idle : Properties.Resources.status_on);
            SetEnable(mobLineActivePictureBox, e.MultiLineState.MobileActive,
                !e.MultiLineState.MobileActive ? Properties.Resources.status_idle : Properties.Resources.status_on);
            SetEnable(deskLineActivePictureBox, e.MultiLineState.DeskphoneActive,
                !e.MultiLineState.DeskphoneActive ? Properties.Resources.status_idle : Properties.Resources.status_on);

            // show held states
            SetEnable(pcLineHeldPictureBox, e.MultiLineState.PCHeld,
                !e.MultiLineState.PCHeld ? Properties.Resources.status_idle : Properties.Resources.status_off);
            SetEnable(mobLineHeldPictureBox, e.MultiLineState.MobileHeld,
                !e.MultiLineState.MobileHeld ? Properties.Resources.status_idle : Properties.Resources.status_off);
            SetEnable(deskLineHeldPictureBox, e.MultiLineState.DeskphoneHeld,
                !e.MultiLineState.DeskphoneHeld ? Properties.Resources.status_idle : Properties.Resources.status_off);

            UpdateMultiLineButtonTexts(e);

            m_lastMultiLineState = e;
        }

        private void UpdateMultiLineButtonTexts(MultiLineStateArgs e)
        {
            if (pcLineActiveBtn.InvokeRequired)
            {
                UpdateMultiLineButtonTextsCallback d = new UpdateMultiLineButtonTextsCallback(UpdateMultiLineButtonTexts);
                this.Invoke(d, new object[] { e });
            }
            else
            {
                // Update the button text for Activate buttons...
                pcLineActiveBtn.Text = e.MultiLineState.PCActive ? "De-Activate" : "Activate";
                mobLineActiveBtn.Text = e.MultiLineState.MobileActive ? "De-Activate" : "Activate";
                deskLineActiveBtn.Text = e.MultiLineState.DeskphoneActive ? "De-Activate" : "Activate";

                // Update the button text for Hold buttons...
                pcLineHoldBtn.Text = e.MultiLineState.PCHeld ? "Un-hold" : "Hold";
                mobLineHoldBtn.Text = e.MultiLineState.MobileHeld ? "Un-hold" : "Hold";
                deskLineHoldBtn.Text = e.MultiLineState.DeskphoneHeld ? "Un-hold" : "Hold";
            }
        }

        // Connect Call (simulated my softphone call)...
        private void button4_Click(object sender, EventArgs e)
        {
            if (!m_oncall)
            {
                int callid = GetNewCallId();
                string contact = contactCombo.Items[contactCombo.SelectedIndex].ToString();
                if (callDirectionCombo.SelectedItem.ToString() == "Incoming")
                {
                    // simulate incoming call
                    if (m_spokes.IncomingCall(callid, contact))
                        SetCallControlGUIEnabled(true);
                }
                else
                {
                    // simulate outgoing call
                    if (m_spokes.OutgoingCall(callid, contact))
                        SetCallControlGUIEnabled(true);
                }
            }
            else
            {
                MessageBox.Show("Already on a simulated softphone call, call id = "+m_callId);
            }
        }

        // End current simulated my softphone call, as known by call id...
        private void button1_Click(object sender, EventArgs e)
        {
            if (m_spokes.EndCall(m_callId))
                SetCallControlGUIEnabled(false);
        }

        private void requestHeadsetSerialBtn_Click(object sender, EventArgs e)
        {
            headsetSerialNumberTxt.Text = "";
            baseSerialNumberTxt.Text = "";
            m_spokes.RequestAllSerialNumbers();
        }

        private void mobileDialBtn_Click(object sender, EventArgs e)
        {
            m_spokes.DialMobileCall(mobileNumberTextBox.Text);
        }

        private void mobileEndBtn_Click(object sender, EventArgs e)
        {
            m_spokes.EndMobileCall();
        }

        // Generate new internal call id for a simulated my softphone call...
        private int GetNewCallId()
        {
            m_callId++;
            callIdTextbox.Text = m_callId.ToString();
            return m_callId;
        }

        #region GUI Helper Methods
        private void ExtraGUIInit()
        {
            contactCombo.SelectedIndex = 0;
            callIdTextbox.Text = m_callId.ToString();
            endCallBtn.Enabled = false;
            mobileDialBtn.Enabled = true;
            mobileEndBtn.Enabled = false;
            mobileEndBtn.Text = "End";

            callDirectionCombo.SelectedIndex = 0;
        }

        // Method to update a device status label, ensure cross-thread support, as Spokes events come in on a different thread
        private void UpdateDeviceStatusGUIItem(Control stateLbl, string statustext)
        {
            if (stateLbl.InvokeRequired)
            {
                UpdateLabelCallback d = new UpdateLabelCallback(UpdateDeviceStatusGUIItem);
                this.Invoke(d, new object[] { stateLbl, statustext });
            }
            else
            {
                stateLbl.Text = statustext;
            }
        }

        // Method to update a device status picture box, ensure cross-thread support, as Spokes events come in on a different thread
        private void UpdateDevicePictureBox(PictureBox statePictureBox, Bitmap bitmap)
        {
            if (statePictureBox.InvokeRequired)
            {
                UpdatePictureBoxCallback d = new UpdatePictureBoxCallback(UpdateDevicePictureBox);
                this.Invoke(d, new object[] { statePictureBox, bitmap });
            }
            else
            {
                statePictureBox.Image = bitmap;
            }
        }

        // Method to enable or disable a GUI item, ensure cross-thread support, as Spokes events come in on a different thread
        // For PictureBox the optional pictureBoxImage allows you to specify a greyed out or normal image!
        private void SetEnable(Control guiitem, bool enable, Bitmap pictureBoxImage = null)
        {
            if (guiitem.InvokeRequired)
            {
                SetEnableCallback d = new SetEnableCallback(SetEnable);
                this.Invoke(d, new object[] { guiitem, enable, pictureBoxImage });
            }
            else
            {
                guiitem.Enabled = enable;
                if (pictureBoxImage != null)
                {
                    try
                    {
                        PictureBox pbox = (PictureBox)guiitem;
                        pbox.Image = pictureBoxImage;
                        pbox.Update();
                    }
                    catch (Exception) { }
                }
            }
        }

        private void SetMuteStateGUIEnabled(bool hasDeviceConnected)
        {
            SetEnable(muteStateLbl, hasDeviceConnected);
            SetEnable(muteStateHdrLbl, hasDeviceConnected);
            SetEnable(muteStatePictureBox, hasDeviceConnected,
                !hasDeviceConnected ? Properties.Resources.mute_grey : null);
        }

        private void SetCallStateGUIEnabled(bool hasDeviceConnected)
        {
            SetEnable(callStateLbl, hasDeviceConnected);
            SetEnable(callDirectionLbl, hasDeviceConnected);
            SetEnable(callSourceLbl, hasDeviceConnected);
            SetEnable(callStateHdrLbl, hasDeviceConnected);
            SetEnable(callStatePictureBox, hasDeviceConnected,
                !hasDeviceConnected ? Properties.Resources.call_grey : null);
        }

        private void SetDockedGUIEnabled(bool hasDocking)
        {
            SetEnable(dockedStateLbl, hasDocking);
            SetEnable(dockedHdrLbl, hasDocking);
            SetEnable(dockedPictureBox, hasDocking,
                !hasDocking ? Properties.Resources.dock_grey : null); //: m_productname.ToUpper().Contains("BT300") ? Properties.Resources.docked : null);
        }

        private void SetMobileCallerGUIEnabled(bool hasCallerId, bool hasMobCallState)
        {
            SetEnable(mobileStateLbl, hasCallerId || hasMobCallState);
            SetEnable(mobileCallDirectionLbl, hasCallerId || hasMobCallState);
            SetEnable(mobileCallerIdLbl, hasCallerId || hasMobCallState);
            SetEnable(mobileDialBtn, hasCallerId);
            SetEnable(mobileEndBtn, hasCallerId);
            SetEnable(mobileHdrLbl, hasCallerId || hasMobCallState);
            SetEnable(mobileStatePictureBox, hasCallerId || hasMobCallState,
                !hasCallerId && !hasMobCallState ? Properties.Resources.mobile_grey : !m_onmobcall ? Properties.Resources.mobile_idle : null );
        }

        private void SetProximityStateGUIEnabled(bool hasProximity)
        {
            SetEnable(proximityStateLbl, hasProximity);
            SetEnable(proxHdrLbl, hasProximity);
            SetEnable(proximityPictureBox, hasProximity,
                !hasProximity ? Properties.Resources.proximity_grey : null);
        }

        private void SetWearingStateGUIEnabled(bool hasWearingSensor)
        {
            SetEnable(wearingStateLbl, hasWearingSensor);
            SetEnable(wearingHdrLbl, hasWearingSensor);
            SetEnable(wearingStatePictureBox, hasWearingSensor,
                !hasWearingSensor ? Properties.Resources.wearing_grey : null);
        }

        // Enable or disable call control GUI elements depending if we are on a call or not.
        // Ensure cross-thread support, as Spokes events come in on a different thread
        private void SetCallControlGUIEnabled(bool oncall)
        {
            if (endCallBtn.InvokeRequired)
            {
                SetCallControlGUIEnableCallback d = new SetCallControlGUIEnableCallback(SetCallControlGUIEnabled);
                this.Invoke(d, new object[] { oncall });
            }
            else
            {
                m_oncall = oncall;
                endCallBtn.Enabled = oncall;
                connectCallBtn.Enabled = !oncall;
            }
        }

        // Enable or disable call control GUI elements depending if we are on a call or not.
        // Ensure cross-thread support, as Spokes events come in on a different thread
        private void SetMobileCallControlGUIEnabled(MobileCallState state)
        {
            if (endCallBtn.InvokeRequired)
            {
                SetMobileCallControlGUIEnableCallback d = new SetMobileCallControlGUIEnableCallback(SetMobileCallControlGUIEnabled);
                this.Invoke(d, new object[] { state });
            }
            else
            {
                switch (state)
                {
                    case MobileCallState.Idle:
                        mobileDialBtn.Enabled = m_spokes.DeviceCapabilities.HasMobCallerId;
                        mobileDialBtn.Text = "Dial";
                        mobileEndBtn.Enabled = false;
                        mobileEndBtn.Text = "End";
                        break;
                    case MobileCallState.Ringing:
                        mobileDialBtn.Enabled = true;
                        mobileDialBtn.Text = "Answer";
                        mobileEndBtn.Enabled = true;
                        mobileEndBtn.Text = "Reject";
                        break;
                    case MobileCallState.OnCall:
                        mobileDialBtn.Enabled = false;
                        mobileDialBtn.Text = "Dial";
                        mobileEndBtn.Enabled = true;
                        mobileEndBtn.Text = "End";
                        break;
                }
            }
        }
        #endregion

        #region DebugLogger implementation
        public void DebugPrint(string methodname, string str)
        {
            if (!IsDisposed)  // avoid trying to log if we are disposed!
                LogMessage(methodname, str);
        }

        // Add a log entry to the log text box, ensure cross-thread support, as Spokes events come in on a different thread
        public void LogMessage(string callingmethodname, string message)
        {
            if (!m_debugmode || eventslog == null) return; // log is not open!
            if (eventslog.eventLogTextBox.InvokeRequired)
            {
                LogMessageCallback d = new LogMessageCallback(LogMessage);
                this.Invoke(d, new object[] { callingmethodname, message });
            }
            else
            {
                string datetime = DateTime.Now.ToString("HH:mm:ss.fff");
                if (m_debugmode && eventslog != null && !eventslog.IsDisposed)
                {
                    eventslog.eventLogTextBox.AppendText(String.Format("{0}: {1}(): {2}\r\n", datetime, callingmethodname, message));
                    eventslog.eventLogTextBox.SelectionStart = eventslog.Text.Length;
                    //eventslog.eventLogTextBox.ScrollToCaret();
                }
            }
        }
        #endregion

        private void pictureBox5_Click(object sender, EventArgs e)
        {

        }

        private void pictureBox6_Click(object sender, EventArgs e)
        {

        }

        private void pcLineActiveBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineActive(Multiline_LineType.PC, !m_lastMultiLineState.MultiLineState.PCActive);
        }

        private void mobLineActiveBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineActive(Multiline_LineType.Mobile, !m_lastMultiLineState.MultiLineState.MobileActive);
        }

        private void deskLineActiveBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineActive(Multiline_LineType.Deskphone, !m_lastMultiLineState.MultiLineState.DeskphoneActive);
        }

        private void pcLineHoldBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineHold(Multiline_LineType.PC, !m_lastMultiLineState.MultiLineState.PCHeld);
        }

        private void mobLineHoldBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineHold(Multiline_LineType.Mobile, !m_lastMultiLineState.MultiLineState.MobileHeld);
        }

        private void deskLineHoldBtn_Click(object sender, EventArgs e)
        {
            m_spokes.SetLineHold(Multiline_LineType.Deskphone, !m_lastMultiLineState.MultiLineState.DeskphoneHeld);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            ClearEventsLogReference(); // close events log first

            // Wearing sensor:
            m_spokes.PutOn -= spokes_DevicePutOn;
            m_spokes.TakenOff -= spokes_DeviceTakenOff;

            // Proximity:
            m_spokes.Near -= spokes_Near;
            m_spokes.Far -= spokes_Far;
            m_spokes.InRange -= spokes_InRange;
            m_spokes.OutOfRange -= spokes_OutOfRange;
            m_spokes.Docked -= spokes_Docked;
            m_spokes.UnDocked -= spokes_UnDocked;

            // Mobile caller id:
            m_spokes.MobileCallerId -= spokes_MobileCallerId;
            m_spokes.OnMobileCall -= m_spokes_OnMobileCall;
            m_spokes.NotOnMobileCall -= m_spokes_NotOnMobileCall;

            // Serial Number:
            m_spokes.SerialNumber -= spokes_SerialNumber;

            // Call control:
            m_spokes.CallAnswered -= spokes_CallAnswered;
            m_spokes.CallEnded -= spokes_CallEnded;
            m_spokes.CallSwitched -= spokes_CallSwitched;
            m_spokes.OnCall -= spokes_OnCall;
            m_spokes.NotOnCall -= spokes_NotOnCall;
            m_spokes.MuteChanged -= spokes_DeviceMuteChanged;

            // Device attach/detach:
            m_spokes.Attached -= spokes_DeviceAttached;
            m_spokes.Detached -= spokes_DeviceDetached;
            m_spokes.CapabilitiesChanged -= spokes_DeviceCapabilitiesChanged;

            // Multiline:
            m_spokes.MultiLineStateChanged -= m_spokes_MultiLineStateChanged;

            m_spokes.Disconnect();
        }

        private void button1_Click_1(object sender, EventArgs e)
        {
            if (eventslog == null)
            {
                eventslog = new EventsLogForm(this);
            }
            eventslog.Show();
            eventslog.WindowState = FormWindowState.Normal;
            eventslog.BringToFront();
            m_debugmode = true;
            LogMessage(MethodInfo.GetCurrentMethod().Name, "Opened Events Log.");
        }

        internal void ClearEventsLogReference()
        {
            m_debugmode = false;
            eventslog = null;
        }
    }
}
