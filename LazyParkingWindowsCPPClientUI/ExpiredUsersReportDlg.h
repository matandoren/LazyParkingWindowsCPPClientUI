#pragma once

#include "AppSpecificWindowsMessagesDefines.h"
// ExpiredUsersReportDlg dialog

class ExpiredUsersReportDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ExpiredUsersReportDlg)

public:
	ExpiredUsersReportDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~ExpiredUsersReportDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EXPIRED_USERS_DIALOG };
#endif

private:
	CALLBACK_DESCRIPTOR m_RequestExpiredUsersReplyCallbackDescriptor;
	int m_MostRecentlySelectedIdx;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg LRESULT OnRequestExpiredUsersReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteDriverOrUpdateDriverExpirationReplyMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void OnRequestExpiredUsersReply(int i_NumberOfExpiredUsers);
	afx_msg void OnBnClickedDeleteDriverButton();
	afx_msg void OnBnClickedUpdateExpirationButton();
};
