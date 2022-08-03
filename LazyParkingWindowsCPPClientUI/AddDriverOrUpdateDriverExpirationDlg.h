#pragma once


// AddDriverOrUpdateDriverExpirationDlg dialog

class AddDriverOrUpdateDriverExpirationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AddDriverOrUpdateDriverExpirationDlg)

public:
	CString m_NameOrUsername;
	CString m_Password;
	CTime m_ExpirationDate;
	bool m_IsNoExpiration;

	AddDriverOrUpdateDriverExpirationDlg(bool i_IsAddDriverDlg, const CString& i_FixedUsername = _T(""), CWnd* pParent = nullptr); // If i_IsAddDriverDlg is false, then this dialog is update driver expiration dialog
																																   // i_FixedUsername is used only when i_IsAddDriverDlg is false
	virtual ~AddDriverOrUpdateDriverExpirationDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ADDDRIVER_OR_UPDATEEXPIRATION_DIALOG };
#endif

private:
	const bool r_IsAddDriverDlg;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnBnClickedCheckNoReservation();
};
