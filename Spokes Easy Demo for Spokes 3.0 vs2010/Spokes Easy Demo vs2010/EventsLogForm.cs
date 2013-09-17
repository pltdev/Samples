using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Spokes_Easy_Demo
{
    public partial class EventsLogForm : Form
    {
        Form1 m_mainwin = null;

        public EventsLogForm(Form1 mainwin)
        {
            InitializeComponent();
            m_mainwin = mainwin;
        }

        private void EventsLogForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_mainwin.ClearEventsLogReference();
        }
    }
}
