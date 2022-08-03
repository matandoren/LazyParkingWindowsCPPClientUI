#pragma once


// ReserveParkingSpotDlg dialog

class ReserveParkingSpotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ReserveParkingSpotDlg)

public:
	const int r_ParkingSpotID;
	CString m_ReservedFor;
	bool m_IsNoExpiration;
	CTime m_ExpirationDate;

	ReserveParkingSpotDlg(int i_ParkingSpotID, CWnd* pParent = nullptr);   // standard constructor
	virtual ~ReserveParkingSpotDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_RESERVE_SPOT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckNoReservation();
	virtual void OnOK();
};
