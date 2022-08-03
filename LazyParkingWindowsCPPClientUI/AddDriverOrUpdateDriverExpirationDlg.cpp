// AddDriverOrUpdateDriverExpirationDlg.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "AddDriverOrUpdateDriverExpirationDlg.h"
#include "afxdialogex.h"


// AddDriverOrUpdateDriverExpiration dialog

IMPLEMENT_DYNAMIC(AddDriverOrUpdateDriverExpirationDlg, CDialogEx)

AddDriverOrUpdateDriverExpirationDlg::AddDriverOrUpdateDriverExpirationDlg(bool i_IsAddDriverDlg, const CString& i_FixedUsername /*= _T("") */, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADDDRIVER_OR_UPDATEEXPIRATION_DIALOG, pParent), r_IsAddDriverDlg(i_IsAddDriverDlg), m_NameOrUsername(i_FixedUsername)
{
	m_IsNoExpiration = false;
}

AddDriverOrUpdateDriverExpirationDlg::~AddDriverOrUpdateDriverExpirationDlg()
{
}

void AddDriverOrUpdateDriverExpirationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AddDriverOrUpdateDriverExpirationDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_NO_DRIVER__EXPIRATION, &AddDriverOrUpdateDriverExpirationDlg::OnBnClickedCheckNoReservation)
END_MESSAGE_MAP()


// AddDriverOrUpdateDriverExpiration message handlers


BOOL AddDriverOrUpdateDriverExpirationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	if (!r_IsAddDriverDlg)
	{
		SetWindowText(_T("Change Driver Expiration Date"));
		GetDlgItem(IDC_NAME_OR_USERNAME_LABEL)->SetWindowText(_T("Username:"));
		if (!m_NameOrUsername.IsEmpty())
		{
			GetDlgItem(IDC_NAME_OR_USERNAME)->SetWindowText(m_NameOrUsername);
			GetDlgItem(IDC_NAME_OR_USERNAME)->EnableWindow(false);
		}

		GetDlgItem(IDC_PASSWORD)->EnableWindow(false);
		GetDlgItem(IDC_PASSWORD)->ShowWindow(false);
		GetDlgItem(IDC_PW_LABEL)->ShowWindow(false);
	}
	else
	{
		SetWindowText(_T("Add Driver"));
		GetDlgItem(IDC_NAME_OR_USERNAME_LABEL)->SetWindowText(_T("Name:"));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void AddDriverOrUpdateDriverExpirationDlg::OnOK()
{
	GetDlgItem(IDC_NAME_OR_USERNAME)->GetWindowText(m_NameOrUsername);
	if (r_IsAddDriverDlg)
	{
		GetDlgItem(IDC_PASSWORD)->GetWindowText(m_Password);
	}

	if (!m_IsNoExpiration)
	{
		((CDateTimeCtrl*)GetDlgItem(IDC_DRIVER_EXPIRATION_DATETIMEPICKER))->GetTime(m_ExpirationDate);
	}

	CDialogEx::OnOK();
}


void AddDriverOrUpdateDriverExpirationDlg::OnBnClickedCheckNoReservation()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_NO_DRIVER__EXPIRATION))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_DRIVER_EXPIRATION_DATETIMEPICKER)->EnableWindow(false);
		m_IsNoExpiration = true;
	}
	else
	{
		GetDlgItem(IDC_DRIVER_EXPIRATION_DATETIMEPICKER)->EnableWindow(true);
		m_IsNoExpiration = false;
	}
}
