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
using System.Timers;
using System.ComponentModel;
using System.Runtime.InteropServices;

/*******
 * 
 * ScreenLockSample
 * 
 * A demo app that shows how a range of Plantronics Contextual Intelligence CI triggers
 * provided by the Plantronics SDK can be used to automatically lock the PC screen.
 * 
 * Lewis Collins
 * 
 * VERSION HISTORY:
 * ********************************************************************************
 * Version 1.0.0.0
 * Date: 2nd March 2015
 * Compatible/tested with Spokes SDK version(s): 3.4.50954.14395
 * Changed by: Lewis Collins
 *   Changes:
 *   Initial version.
 * ********************************************************************************
 *
 **/

namespace ScreenLockSample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, DebugLogger
    {
        // Plantronics API singleton
        Spokes m_spokes;
        Timer m_lockTimer;
        int m_lockSeconds = 0;
        string m_lockReason = "";

        public MainWindow()
        {
            InitializeComponent();

            m_lockTimer = new Timer();
            m_lockTimer.Interval = 1000;
            m_lockTimer.Elapsed += new ElapsedEventHandler(m_lockTimer_Elapsed);
        }

        void m_lockTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            if (m_lockSeconds > 0)
            {
                m_lockSeconds--;
                UpdateStatusLabel("Screen will lock in: " + m_lockSeconds + ", " + m_lockReason);
            }
            else
            {
                m_lockTimer.Stop();
                LockNow();
            }
        }

        private void UpdateStatusLabel(string statustext)
        {
            statusLabel.Dispatcher.Invoke(new Action(delegate()
            {
                statusLabel.Content = statustext;
            }));
        }

        [DllImport("user32.dll")]
        public static extern bool LockWorkStation();

        private void LockNow()
        {
            UpdateStatusLabel("Screen locked: " + m_lockReason);
            SetLockButtonEnabled(false);

            if (!LockWorkStation())
            {
                throw new Win32Exception(Marshal.GetLastWin32Error()); // or any other thing
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            // Obtain Plantronics API singleton
            m_spokes = Spokes.Instance;

            // Register for Plantronics API events of interest

            // Have I got a Plantronics device?
            m_spokes.Attached += new Spokes.AttachedEventHandler(m_spokes_Attached);
            m_spokes.Detached += new Spokes.DetachedEventHandler(m_spokes_Detached);

            // Am I wearing Plantronics device?
            m_spokes.PutOn += new Spokes.PutOnEventHandler(m_spokes_PutOn);
            m_spokes.TakenOff += new Spokes.TakenOffEventHandler(m_spokes_TakenOff);

            // Is Plantronics device docked?
            m_spokes.Docked += new Spokes.DockedEventHandler(m_spokes_Docked);
            m_spokes.UnDocked += new Spokes.DockedEventHandler(m_spokes_UnDocked);

            // Is Plantronics device near or far
            m_spokes.Near += new Spokes.NearEventHandler(m_spokes_Near);
            m_spokes.Far += new Spokes.FarEventHandler(m_spokes_Far);

            // Is Plantronics device connected (InRange) or disconnected (OutOfRange)
            m_spokes.InRange += new Spokes.InRangeEventHandler(m_spokes_InRange);
            m_spokes.OutOfRange += new Spokes.OutOfRangeEventHandler(m_spokes_OutOfRange);

            // Did Plantronics device disconnect from paired mobile device (receive ProximityDisabled event when that occurs)
            m_spokes.ProximityDisabled += new Spokes.ProximityDisabledEventHandler(m_spokes_ProximityDisabled);
            m_spokes.ProximityEnabled += new Spokes.ProximityEnabledEventHandler(m_spokes_ProximityEnabled);

            // Finally connect to Plantronics API
            m_spokes.Connect("ScreenLockSample");
        }

        void m_spokes_UnDocked(object sender, DockedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset is un-docked");
            DoCancelLockIfCheckBoxSet(dockedcheckBox);
        }

        void m_spokes_Docked(object sender, DockedStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset is docked");
            if (!e.m_isInitialStateEvent)
            {
                DoLockIfCheckBoxSet(dockedcheckBox, "Headset was docked", GetSecondsDelay());
            }
        }

        void m_spokes_OutOfRange(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset out of range");
            DoLockIfCheckBoxSet(outofrangecheckBox, "Headset was out of range");
        }

        void m_spokes_InRange(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset in range");
            if (m_lockSeconds == 0)
            {
                UpdateStatusLabel("Idle");
                SetLockButtonEnabled(false);
            }
        }

        void m_spokes_Far(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset far from PC");
            DoLockIfCheckBoxSet(farcheckBox, "Headset was far from PC", GetSecondsDelay());
        }

        void m_spokes_Near(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset near to PC");
            DoCancelLockIfCheckBoxSet(farcheckBox);
        }

        void m_spokes_ProximityDisabled(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset proximity disabled. (Interpret as Mobile disconnected from headset - Mobile out of range / reverse proximity)");
            DoLockIfCheckBoxSet(mobileoutofrangecheckBox, "Mobile was out of range of headset");
        }

        void m_spokes_ProximityEnabled(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset proximity enabled");
            if (m_lockSeconds == 0)
            {
                UpdateStatusLabel("Idle");
                SetLockButtonEnabled(false);
            }
        }

        void m_spokes_TakenOff(object sender, WearingStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset taken off");
            if (!e.m_isInitialStateEvent)
            {
                DoLockIfCheckBoxSet(takenOffcheckBox, "Headset was taken off", GetSecondsDelay());
            }
        }

        void m_spokes_PutOn(object sender, WearingStateArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset put on");
            if (!e.m_isInitialStateEvent)
            {
                DoCancelLockIfCheckBoxSet(takenOffcheckBox);
            }
        }

        void m_spokes_Detached(object sender, EventArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset detached");
            if (m_lockSeconds == 0)
            {
                UpdateStatusLabel("Idle");
                SetLockButtonEnabled(false);
            }
        }

        void m_spokes_Attached(object sender, AttachedArgs e)
        {
            DebugPrint(MethodInfo.GetCurrentMethod().Name, "Headset attached: " + e.m_device.ProductName);
            if (m_lockSeconds == 0)
            {
                UpdateStatusLabel("Idle");
                SetLockButtonEnabled(false);
            }
        }

        private void SetLockButtonEnabled(bool enabled)
        {
            cancellockbutton.Dispatcher.Invoke(new Action(delegate()
            {
                cancellockbutton.IsEnabled = enabled;
            }));
        }

        private void DoLockIfCheckBoxSet(CheckBox aCheckBox, string reason, int seconds = 0)
        {
            aCheckBox.Dispatcher.Invoke(new Action(delegate()
            {
                if (aCheckBox.IsChecked == true)
                {
                    m_lockReason = reason;
                    StartLockTimer(seconds);
                    UpdateStatusLabel("Screen will lock in: " + seconds + ", " + m_lockReason);
                    cancellockbutton.IsEnabled = true;
                }
            }));
        }

        private void StartLockTimer(int seconds)
        {
            m_lockTimer.Start();
            m_lockSeconds = seconds;
        }

        private void DoCancelLockIfCheckBoxSet(CheckBox aCheckBox)
        {
            aCheckBox.Dispatcher.Invoke(new Action(delegate()
            {
                if (aCheckBox.IsChecked == true)
                {
                    StopLockTimer();
                    UpdateStatusLabel("Idle");
                    SetLockButtonEnabled(false);
                }
            }));
        }

        private void DoCancelLock()
        {
            StopLockTimer();
            UpdateStatusLabel("Idle");
            SetLockButtonEnabled(false);
        }

        private void StopLockTimer()
        {
            m_lockTimer.Stop();
            m_lockSeconds = 0;
        }

        private int GetSecondsDelay()
        {
            int seconds = 0;
            takeoffdocklockdelaycomboBox.Dispatcher.Invoke(new Action(delegate()
            {
                switch (takeoffdocklockdelaycomboBox.Text)
                {
                    case "Immediate":
                        seconds = 0;
                        break;
                    case "15":
                        seconds = 15;
                        break;
                    case "30":
                        seconds = 30;
                        break;
                    default:
                        seconds = 0;
                        break;
                }
            }));
            return seconds;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            // unregisted events
            m_spokes.Attached -= m_spokes_Attached;
            m_spokes.Detached -= m_spokes_Detached;
            m_spokes.PutOn -= m_spokes_PutOn;
            m_spokes.TakenOff -= m_spokes_TakenOff;
            m_spokes.Near -= m_spokes_Near;
            m_spokes.Far -= m_spokes_Far;
            m_spokes.InRange -= m_spokes_InRange;
            m_spokes.OutOfRange -= m_spokes_OutOfRange;
            m_spokes.ProximityDisabled -= m_spokes_ProximityDisabled;
            m_spokes.ProximityEnabled -= m_spokes_ProximityEnabled;

            // disconnect Plantronics API
            m_spokes.Disconnect();
        }

        // Output debug log information from API etc.
        public void DebugPrint(string methodname, string str)
        {
            Log_textBox.Dispatcher.Invoke(new Action(delegate()
            {
                string datetime = DateTime.Now.ToString("HH:mm:ss.fff");
                Log_textBox.AppendText(String.Format("{0}: {1}(): {2}\r\n", datetime, methodname, str));
                Log_textBox.ScrollToEnd();
            }));
        }

        private void cancellockbutton_Click(object sender, RoutedEventArgs e)
        {
            DoCancelLock();
        }
    }
}
