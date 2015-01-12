// WrapperSampleDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "atlbase.h"
#include "..\..\wrappers\SpokesWrapper3.x.h"

// GUI Callbacks
#define WM_LOG_MESSAGE    WM_APP + 0x10
struct log_message
{
	std::string methodname;
	std::string message;
};

// CWrapperSampleAppDlg dialog
class CWrapperSampleAppDlg : public CDialog, public IDebugLogger, public ISpokesEvents
{
// Construction
public:
	CWrapperSampleAppDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_WRAPPERSAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	// The Spokes object singleton
    Spokes * m_spokes;

	static std::string APP_NAME; // my application name string to pass to Spokes
	std::string m_device; // Plantronics Product Name

	// Spokes IDebugLogger implementation
    virtual void DebugPrint(std::string methodname, std::string str)
    {
		// need to post message to main window thread so it can update the GUI...
		// pass a pointer to the message structure. Note
		// it is the responsibility of the receiver to delete the message stucture from heap.
		log_message * msg = new log_message;
		msg->methodname = methodname;
		msg->message = str;
		::PostMessage(GetSafeHwnd(), WM_LOG_MESSAGE, (WPARAM)(void*)msg, 0);
    }

	// Callbacks from ISpokesEvents class
private:
	virtual void Spokes_Attached(AttachedArgs * e);
	virtual void Spokes_Detached(EventArgs * e);
	virtual void Spokes_CallAnswered(CallAnsweredArgs * e);
	virtual void Spokes_CallEnded(CallEndedArgs * e);
	virtual void Spokes_CallSwitched(EventArgs * e);
	virtual void Spokes_MuteChanged(MuteChangedArgs * e);
	virtual void Spokes_LineActiveChanged(LineActiveChangedArgs * e);
	virtual void Spokes_OnCall(OnCallArgs * e);
	virtual void Spokes_NotOnCall(EventArgs * e);
	virtual void Spokes_MultiLineStateChanged(EventArgs * e);
	virtual void Spokes_Connected(EventArgs * e);
	virtual void Spokes_Disconnected(EventArgs * e);

private:
	// GUI Callbacks
	LRESULT OnLogMessage(WPARAM wParam, LPARAM lParam);  

// Implementation
protected:
	HICON m_hIcon;
	bool _attached;
	bool _mute;
	bool _oncall;
	bool _ringing;
	bool _held;
	void UpdateDevice();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAttach();
public:
	afx_msg void OnBnClickedRing();
public:
	afx_msg void OnBnClickedCall();
public:
	afx_msg void OnBnClickedSpeaker();
public:
	afx_msg void OnBnClickedMute();
public:
	afx_msg void OnBnClickedClear();
public:
	CEdit Text;
public:

private:
	//void AppendInfoText(CString text);
	void UpdateUI();
	afx_msg void OnBnClickedInbound();
	afx_msg void OnBnClickedOutbound();
	afx_msg void OnBnClickedAnswer();
	afx_msg void OnBnClickedHup();
public:
	afx_msg void OnBnClickedHoldcall();
	afx_msg void OnBnClickedResumecall();
};
