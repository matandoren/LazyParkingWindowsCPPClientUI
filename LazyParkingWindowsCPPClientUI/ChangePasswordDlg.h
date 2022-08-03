#pragma once

#include "AppSpecificWindowsMessagesDefines.h"
// ChangePasswordDlg dialog

class ChangePasswordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ChangePasswordDlg)

public:
	ChangePasswordDlg(int i_PermissionLevel, const CString& i_InitialUsername, CWnd* pParent = nullptr);   // standard constructor
	virtual ~ChangePasswordDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHANGEPW_DIALOG };
#endif
private:
	const int r_PermissionLevel;
	const CString& r_InitialUsername;
	CALLBACK_DESCRIPTOR m_ChangePasswordReplyCallbackDescriptor;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnChangePasswordReplyMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckChangeOther();
	void OnChangePasswordReply(bool i_NoPermission, bool i_UsernameNotFound);
	afx_msg void OnBnClickedOk();
};
