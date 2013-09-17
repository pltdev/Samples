// WrapperSampleDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WrapperSample.h"
#include "WrapperSampleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

int _nextid = 0; // used to generate call id's

// CWrapperSampleAppDlg dialog

CWrapperSampleAppDlg::CWrapperSampleAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWrapperSampleAppDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	_attached = _mute = _oncall = _ringing = FALSE;
	m_device = "";
	m_spokes = NULL;
}

void CWrapperSampleAppDlg::OnCancel( )
{
	if (m_spokes!=NULL && m_spokes->m_bIsConnected)
		m_spokes->Disconnect();
	CDialog::OnCancel();
}

void CWrapperSampleAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT, Text);
}

BEGIN_MESSAGE_MAP(CWrapperSampleAppDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_LOG_MESSAGE, OnLogMessage)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ATTACH, &CWrapperSampleAppDlg::OnBnClickedAttach)
	ON_BN_CLICKED(IDC_MUTE, &CWrapperSampleAppDlg::OnBnClickedMute)
	ON_BN_CLICKED(IDC_CLEAR, &CWrapperSampleAppDlg::OnBnClickedClear)
	ON_BN_CLICKED(IDC_INBOUND, &CWrapperSampleAppDlg::OnBnClickedInbound)
	ON_BN_CLICKED(IDC_OUTBOUND, &CWrapperSampleAppDlg::OnBnClickedOutbound)
	ON_BN_CLICKED(IDC_ANSWER, &CWrapperSampleAppDlg::OnBnClickedAnswer)
	ON_BN_CLICKED(IDC_HUP, &CWrapperSampleAppDlg::OnBnClickedHup)
END_MESSAGE_MAP()

// Initialise static constants:
string CWrapperSampleAppDlg::APP_NAME = "SpokesWrapperMiniSample";

// CWrapperSampleAppDlg message handlers

BOOL CWrapperSampleAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CoInitialize(NULL);

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	UpdateUI();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CWrapperSampleAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWrapperSampleAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

CString BoolToString(bool val)
{
	return val ? "TRUE" : "FALSE";
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWrapperSampleAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
void CWrapperSampleAppDlg::UpdateUI()
{
	TRACE("UpdateUI. _attached=%d _ringing=%d _inCall=%d _mute=%d\n",_attached,_ringing,_oncall,_mute); //_infoCount=%d    _infoCount,
	if (_attached) // && _infoCount > 0)
	{
		GetDlgItem(IDC_INBOUND)->EnableWindow(TRUE);
		GetDlgItem(IDC_OUTBOUND)->EnableWindow(TRUE);
		GetDlgItem(IDC_HUP)->EnableWindow(_oncall); //countActiveCalls() > 0);
		GetDlgItem(IDC_ANSWER)->EnableWindow(_ringing); //countRingingCalls() > 0);
		GetDlgItem(IDC_MUTE)->EnableWindow(_oncall); //countActiveCalls() > 0);
	}
	else
	{
		GetDlgItem(IDC_INBOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_OUTBOUND)->EnableWindow(FALSE);
		GetDlgItem(IDC_ANSWER)->EnableWindow(FALSE);
		GetDlgItem(IDC_HUP)->EnableWindow(FALSE);
		GetDlgItem(IDC_MUTE)->EnableWindow(FALSE);
	}
	((CButton *) GetDlgItem(IDC_MUTE))->SetCheck(_mute);
}
void CWrapperSampleAppDlg::OnBnClickedAttach()
{
	CWaitCursor crs;
	int check = ((CButton*)GetDlgItem(IDC_ATTACH))->GetCheck();
	if (check == BST_CHECKED)
	{
		// Get the Spokes object singleton
		m_spokes = Spokes::GetInstance();

		m_spokes->SetLogger(this); // tell spokes to log debug output to me using DebugLogger interface

		m_spokes->SetEventHandler(this); // tell Spokes I want to receive Spokes events (we override the ones we want to receive using ISpokesEvents interface)

		// Now connect to attached device, if any
		m_spokes->Connect(APP_NAME.c_str());
	}
	else if (check == BST_UNCHECKED)
	{
		//::PltRelease(this);
		if (m_spokes->m_bIsConnected)
		{
			m_spokes->Disconnect();

			_attached = false;
		}
	}
	UpdateUI();
}

void CWrapperSampleAppDlg::OnBnClickedMute()
{
	CWaitCursor crs;
	int check = ((CButton*)GetDlgItem(IDC_MUTE))->GetCheck();
	if (_oncall)
	{
		m_spokes->SetMute(check == BST_CHECKED);
	}
}

void CWrapperSampleAppDlg::OnBnClickedClear()
{
	Text.SetWindowText(_T(""));
}

LRESULT CWrapperSampleAppDlg::OnLogMessage(WPARAM wParam, LPARAM lParam)
{
	log_message * mymsg = (log_message*)(void*)wParam;

	CString curlogstr;
	Text.GetWindowText(curlogstr);

	CString newEntry;

    //get time stamp
	SYSTEMTIME systime;
	GetSystemTime(&systime);

	newEntry.Format("%02d:%02d:%02d.%03d: %s: %s\r\n",
		systime.wHour, systime.wMinute, systime.wSecond, systime.wMilliseconds,
		mymsg->methodname.c_str(), mymsg->message.c_str() );

	delete mymsg; // delete the inbound payload off the heap!

	curlogstr += newEntry;
	Text.SetWindowText(curlogstr);

	// scroll to end
	int nLength = Text.GetWindowTextLength();
	// put the selection at the end of text
	Text.SetSel(nLength, nLength);

	return 0;
}

// New Spokes Wrapper callbacks...
void CWrapperSampleAppDlg::Spokes_Attached(AttachedArgs * e) 
{ 
	m_device = e->m_strDeviceName;
	_attached = true;
	UpdateUI();

	CString buffer;
	buffer.Format("PltAttachedCallback(device=%s)",m_device.c_str());
	DebugPrint(__FUNCTION__, buffer.GetBuffer());
}

void CWrapperSampleAppDlg::Spokes_Detached(EventArgs * e)
{ 
	m_device = "";
	_attached = false;
	UpdateUI();

	DebugPrint(__FUNCTION__,"PltRemovedCallback");
}

void CWrapperSampleAppDlg::Spokes_CallAnswered(CallAnsweredArgs * e)
{ 
	CString buffer;
	buffer.Format(">>> User has answered call, call id: %d, call source = %s",e->m_iCallId,e->m_strCallSource.c_str());
	DebugPrint(__FUNCTION__, buffer.GetBuffer());

   // if this was My Softphone's call then activate the audio link to headset
	if (e->m_iCallId > 0 && e->m_strCallSource.compare(APP_NAME) == 0)
    {
        m_spokes->ConnectAudioLinkToDevice(true);
        m_spokes->SetMute(false);
		_oncall = true;
    }
    else
	{
		buffer.Format(">>> Ignoring spurious call event, call id: %d, call source = %s",e->m_iCallId,e->m_strCallSource.c_str());
		DebugPrint(__FUNCTION__, buffer.GetBuffer());
		_oncall = false;
	}
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_CallEnded(CallEndedArgs * e)
{ 
	CString buffer;
	buffer.Format(">>> User has ended call, call id: %d",e->m_iCallId);
    DebugPrint(__FUNCTION__, buffer.GetBuffer());

    // if this was My Softphone's call then terminate the audio link to headset
	if (e->m_iCallId > 0 && e->m_strCallSource.compare(APP_NAME) == 0)
    {
        m_spokes->SetMute(false);
        m_spokes->ConnectAudioLinkToDevice(false);
    }
    else
	{
		buffer.Format(">>> Ignoring spurious call event, call id: %d, callsource: %s", e->m_iCallId, e->m_strCallSource.c_str());
		DebugPrint(__FUNCTION__, buffer.GetBuffer());
	}
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_CallSwitched(EventArgs * e)
{ 
    DebugPrint(__FUNCTION__, ">>> User has switched calls");
    m_spokes->ConnectAudioLinkToDevice(true);
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_MuteChanged(MuteChangedArgs * e)
{ 
	CString buffer;
	buffer.Format(">>> Device Mute Changed mute = %s", e->m_bMuteOn ? "On" : "Off");
    DebugPrint(__FUNCTION__, buffer.GetBuffer());
	_mute = e->m_bMuteOn;
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_LineActiveChanged(LineActiveChangedArgs * e)
{ 
	CString buffer;
	buffer.Format(">>> Line Active Changed active = %s", e->m_bLineActive ? "Active" : "Inactive");
    DebugPrint(__FUNCTION__, buffer.GetBuffer());
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_OnCall(OnCallArgs * e)
{
	CString buffer;
	buffer.Format(">>> User is On a Call, call souce: %s, direction: %s, state: %d",e->m_strCallSource.c_str(),e->m_bIncoming?"Incoming":"Outgoing",e->m_State);
	DebugPrint(__FUNCTION__, buffer.GetBuffer());

    // if this was My Softphone's call then terminate the audio link to headset
	if (e->m_strCallSource.compare(APP_NAME) == 0)
    {
		_ringing = e->m_State == SpokesCallState_Ringing;
		_oncall = e->m_State == SpokesCallState_OnCall;
    }
    else
	{
		buffer.Format(">>> Ignoring spurious on call event, callsource: %s", e->m_strCallSource.c_str());
		DebugPrint(__FUNCTION__, buffer.GetBuffer());
		_oncall = !(e->m_State == SpokesCallState_OnCall); // if i am on another softphone, my softphone call not active
	}
	UpdateUI();
}

void CWrapperSampleAppDlg::Spokes_NotOnCall(EventArgs * e)
{ 
	CString buffer;
	buffer.Format(">>> User is Not On a Call");
	DebugPrint(__FUNCTION__, buffer.GetBuffer());

	_oncall = false;
	UpdateUI();
}

void CWrapperSampleAppDlg::OnBnClickedInbound()
{
	if (_attached)
	{
		CWaitCursor crs;
		_nextid++;
		m_spokes->IncomingCall(_nextid, "");
		UpdateUI();
	}
}

void CWrapperSampleAppDlg::OnBnClickedOutbound()
{
	if (_attached)
	{
		CWaitCursor crs;
		_nextid++;
		m_spokes->OutgoingCall(_nextid, "");
		UpdateUI();
	}
}

void CWrapperSampleAppDlg::OnBnClickedAnswer()
{
	if (_attached)
	{
		CWaitCursor crs;

		if (_ringing)
		{
			// answering
			m_spokes->AnswerCall(_nextid);
		}

		UpdateUI();
	}
}

void CWrapperSampleAppDlg::OnBnClickedHup()
{
	if (_attached)
	{
		CWaitCursor crs;
		if (_oncall)
		{
			m_spokes->EndCall(_nextid);
		}

		UpdateUI();
	}
}
