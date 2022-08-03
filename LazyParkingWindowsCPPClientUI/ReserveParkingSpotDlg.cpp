// ReserveParkingSpotDlg.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "ReserveParkingSpotDlg.h"
#include "afxdialogex.h"


// ReserveParkingSpotDlg dialog

IMPLEMENT_DYNAMIC(ReserveParkingSpotDlg, CDialogEx)

ReserveParkingSpotDlg::ReserveParkingSpotDlg(int i_ParkingSpotID, CWnd* pParent /*=nullptr*/)
	: r_ParkingSpotID(i_ParkingSpotID), CDialogEx(IDD_RESERVE_SPOT_DIALOG, pParent)
{
	m_IsNoExpiration = false;
}

ReserveParkingSpotDlg::~ReserveParkingSpotDlg()
{
}

void ReserveParkingSpotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ReserveParkingSpotDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_NO_RESERVATION__EXPIRATION, &ReserveParkingSpotDlg::OnBnClickedCheckNoReservation)
END_MESSAGE_MAP()


// ReserveParkingSpotDlg message handlers


BOOL ReserveParkingSpotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CString label;
	label.Format(_T("You are reserving parking spot %d"), r_ParkingSpotID);
	GetDlgItem(IDC_RESERVE_SPOT_LABEL)->SetWindowText(label);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ReserveParkingSpotDlg::OnBnClickedCheckNoReservation()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_NO_RESERVATION__EXPIRATION))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_RESERVATION_EXPIRATION_DATETIMEPICKER)->EnableWindow(false);
		m_IsNoExpiration = true;
	}
	else
	{
		GetDlgItem(IDC_RESERVATION_EXPIRATION_DATETIMEPICKER)->EnableWindow(true);
		m_IsNoExpiration = false;
	}
}


void ReserveParkingSpotDlg::OnOK()
{
	GetDlgItem(IDC_RESERVEDFOR)->GetWindowText(m_ReservedFor);
	if (!m_IsNoExpiration)
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_RESERVATION_EXPIRATION_DATETIMEPICKER))->GetTime(m_ExpirationDate);
	}
	
	CDialogEx::OnOK();
}
