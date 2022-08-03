
// LoginDlg.h : header file
//

#pragma once
#include "AppSpecificWindowsMessagesDefines.h"

// LoginDlg dialog
class LoginDlg : public CDialogEx
{
// Construction
public:
	LoginDlg(CWnd* pParent = nullptr);	// standard constructor
	~LoginDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGIN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

private:
	CImage m_LogoImage;
	CALLBACK_DESCRIPTOR m_LogingReplyCallbackDescriptor;
	CMenu m_Menu;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnLoginReplyMessage(WPARAM wParam, LPARAM lParam);
	BOOL OnCommand(WPARAM wParam, LPARAM lParam) override;
	DECLARE_MESSAGE_MAP()

public:
	void OnLoginReply(int i_PermissionLevel, bool i_UsernameNotFound, bool i_WrongPW);
	afx_msg void OnBnClickedAdvancedsettingsmfcmenubutton();
};
