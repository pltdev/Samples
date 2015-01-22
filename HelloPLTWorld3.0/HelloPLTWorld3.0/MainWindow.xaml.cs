using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Plantronics.UC.SpokesWrapper;
using System.Reflection;

namespace HelloPLTWorld3._0
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, DebugLogger
    {
        private Spokes m_spokes;
        private int linenum = 0;
        private int callid = 0;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // get Spokes singleton
            m_spokes = Spokes.Instance;

            // set us as the Spokes debug logger
            m_spokes.SetLogger(this);

            // register event handlers...

            // detect when Plantronics device is added or removed from PC:
            m_spokes.Attached += new Spokes.AttachedEventHandler(m_spokes_Attached);
            m_spokes.Detached += new Spokes.DetachedEventHandler(m_spokes_Detached);

            // detect when we are on a VoIP call:
            m_spokes.OnCall += new Spokes.OnCallEventHandler(m_spokes_OnCall);
            m_spokes.NotOnCall += new Spokes.NotOnCallEventHandler(m_spokes_NotOnCall);

            // detect when user answers/ends a call using headset buttons:
            m_spokes.CallAnswered += new Spokes.CallAnsweredEventHandler(m_spokes_CallAnswered);
            m_spokes.CallEnded += new Spokes.CallEndedEventHandler(m_spokes_CallEnded);

            // detect when we are on a Mobile call:
            m_spokes.OnMobileCall += new Spokes.OnMobileCallEventHandler(m_spokes_OnMobileCall);
            m_spokes.NotOnMobileCall += new Spokes.NotOnMobileCallEventHandler(m_spokes_NotOnMobileCall);
            m_spokes.MobileCallerId += new Spokes.MobileCallerIdEventHandler(m_spokes_MobileCallerId);

            // detect if we are wearing the headset:
            m_spokes.PutOn += new Spokes.PutOnEventHandler(m_spokes_PutOn);
            m_spokes.TakenOff += new Spokes.TakenOffEventHandler(m_spokes_TakenOff);

            // detect if headset is docked:
            m_spokes.Docked += new Spokes.DockedEventHandler(m_spokes_Docked);
            m_spokes.UnDocked += new Spokes.DockedEventHandler(m_spokes_UnDocked);

#if doubloon
            // NEW CC events
            m_spokes.Connected += new Spokes.ConnectedEventHandler(m_spokes_Connected);
            m_spokes.Disconnected += new Spokes.DisconnectedEventHandler(m_spokes_Disconnected);
#endif
            // now connect to Spokes
            m_spokes.Connect("Hello Spokes 3.0 World");
        }

#if doubloon
        // NEW CC events
        void m_spokes_Disconnected(object sender, ConnectedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ New DA Series - Headset was Disconnected");
        }

        void m_spokes_Connected(object sender, ConnectedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ New DA Series - Headset was Connected");
        }
#endif

        void m_spokes_CallEnded(object sender, CallEndedArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ VoIP call was Ended: id=" + e.CallId + ", source=" + e.CallSource);
        }

        void m_spokes_CallAnswered(object sender, CallAnsweredArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ VoIP call was Answered: id="+e.CallId+", source="+e.CallSource);
        }

        void m_spokes_MobileCallerId(object sender, MobileCallerIdArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Mobile Caller Id: " + e.MobileCallerId);
        }

        void m_spokes_NotOnMobileCall(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Not on Mobile Call");
        }

        void m_spokes_OnMobileCall(object sender, OnMobileCallArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ On Mobile Call: incoming = " + e.Incoming + ", state = "+e.State);
        }

        void m_spokes_NotOnCall(object sender, NotOnCallArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Not on VoIP Call, call that ended was: id=" + e.CallId + ", source=" + e.CallSource);
        }

        void m_spokes_OnCall(object sender, OnCallArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ On VoIP Call: incoming = " + e.Incoming + ", source = " + e.CallSource + ", state = " + e.State);
        }

        void m_spokes_UnDocked(object sender, DockedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Headset " + (e.m_isInitialStateEvent ? "INITIALLY " : "") + "UnDocked");
        }

        void m_spokes_Docked(object sender, DockedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Headset " + (e.m_isInitialStateEvent ? "INITIALLY " : "") + "Docked");
        }

        void m_spokes_TakenOff(object sender, WearingStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Headset "+(e.m_isInitialStateEvent?"INITIALLY ":"")+"Taken Off");
        }

        void m_spokes_PutOn(object sender, WearingStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Headset " + (e.m_isInitialStateEvent ? "INITIALLY " : "") + "Put On");
        }

        void m_spokes_Detached(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Plantronics Device was detached");
        }

        void m_spokes_Attached(object sender, AttachedArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "@@@ Plantronics Device \"" + e.m_device.ProductName + "\" was attached");
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // unregister event handlers
            m_spokes.Attached -= m_spokes_Attached;
            m_spokes.Detached -= m_spokes_Detached;
            m_spokes.OnCall -= m_spokes_OnCall;
            m_spokes.NotOnCall -= m_spokes_NotOnCall;
            m_spokes.OnMobileCall -= m_spokes_OnMobileCall;
            m_spokes.NotOnMobileCall -= m_spokes_NotOnMobileCall;
            m_spokes.MobileCallerId -= m_spokes_MobileCallerId;
            m_spokes.PutOn -= m_spokes_PutOn;
            m_spokes.TakenOff -= m_spokes_TakenOff;
            m_spokes.Docked -= m_spokes_Docked;
            m_spokes.UnDocked -= m_spokes_UnDocked;
#if doubloon
            // NEW CC events
            m_spokes.Connected -= m_spokes_Connected;
            m_spokes.Disconnected -=m_spokes_Disconnected;
#endif

            // disconnect from Spokes
            m_spokes.Disconnect();
        }

        public void DebugPrint(string methodname, string str)
        {
            eventsLogTextBox.Dispatcher.Invoke(new Action(delegate()
            {
                string datetime = DateTime.Now.ToString("HH:mm:ss.fff");
                eventsLogTextBox.AppendText((String.Format("{0}: {1}: {2}(): {3}\r\n", ++linenum, datetime, methodname, str)));
                eventsLogTextBox.ScrollToEnd();
            }));
        }

        private void incomingcallbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.IncomingCall(++callid);
            UpdateCallIdTextBox();
        }

        private void UpdateCallIdTextBox()
        {
            callIdTextBox.Dispatcher.Invoke(new Action(delegate()
            {
                callIdTextBox.Text = callid.ToString();
            }));
        }

        private void Outgoingcallbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.OutgoingCall(++callid);
            UpdateCallIdTextBox();
        }

        private void mutecallbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.SetMute(true);
        }

        private void unmutecallbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.SetMute(false);
        }

        private void endcallbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.EndCall(callid);
        }

        private void lineonbtn_Click(object sender, RoutedEventArgs e)
        {
            ActivateVoIPLine(true);
        }

        private void ActivateVoIPLine(bool activate)
        {
            if (m_spokes.DeviceCapabilities.HasMultiline)
            {
                m_spokes.SetLineActive(Multiline_LineType.PC, activate);
            }
            else
            {
                m_spokes.ConnectAudioLinkToDevice(activate);
            }
        }

        private void lineoffbtn_Click(object sender, RoutedEventArgs e)
        {
            ActivateVoIPLine(false);
        }

        private void ringonbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.IncomingCall(++callid);
            UpdateCallIdTextBox();
        }

        private void ringoffbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.EndCall(callid);
        }

        private void muteonbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.SetMute(true);
        }

        private void muteoffbtn_Click(object sender, RoutedEventArgs e)
        {
            m_spokes.SetMute(false);
        }

        private void cleareventsbtn_Click(object sender, RoutedEventArgs e)
        {
            eventsLogTextBox.Dispatcher.Invoke(new Action(delegate()
            {
                eventsLogTextBox.Clear();
                linenum = 0;
            }));
        }
    }
}
