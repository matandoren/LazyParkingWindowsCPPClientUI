// ChangePasswordDlg.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "ChangePasswordDlg.h"
#include "afxdialogex.h"


// ChangePasswordDlg dialog

IMPLEMENT_DYNAMIC(ChangePasswordDlg, CDialogEx)

static void OnChangePasswordReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound);

ChangePasswordDlg::ChangePasswordDlg(int i_PermissionLevel, const CString& i_InitialUsername, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CHANGEPW_DIALOG, pParent), r_PermissionLevel(i_PermissionLevel), r_InitialUsername(i_InitialUsername)
{
	m_ChangePasswordReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
}

ChangePasswordDlg::~ChangePasswordDlg()
{
	RemoveChangePasswordReplyEventHandler(theApp.GetModelDescriptor(), m_ChangePasswordReplyCallbackDescriptor);
}

void ChangePasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ChangePasswordDlg, CDialogEx)
	ON_BN_CLICKED(IDC_CHECK_CHANGE_OTHER, &ChangePasswordDlg::OnBnClickedCheckChangeOther)
	ON_MESSAGE(WM_CHANGEPASSWORDREPLYMESSAGE, ChangePasswordDlg::OnChangePasswordReplyMessage)
	ON_BN_CLICKED(IDOK, &ChangePasswordDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// ChangePasswordDlg message handlers


BOOL ChangePasswordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	GetDlgItem(IDC_USERNAME)->SetWindowText(r_InitialUsername);
	if (r_PermissionLevel >= OPERATOR_PERMISSION_LEVEL)
	{
		GetDlgItem(IDC_CHECK_CHANGE_OTHER)->EnableWindow(true);
		GetDlgItem(IDC_CHECK_CHANGE_OTHER)->ShowWindow(SW_SHOW);
	}

	if (AddChangePasswordReplyEventHandler(theApp.GetModelDescriptor(), OnChangePasswordReplyWrapper, this, m_ChangePasswordReplyCallbackDescriptor) != eStatusCode_Success)
	{
		return FALSE;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void ChangePasswordDlg::OnBnClickedCheckChangeOther()
{
	if (((CButton*)GetDlgItem(IDC_CHECK_CHANGE_OTHER))->GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_USERNAME)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_USERNAME)->EnableWindow(false);
		GetDlgItem(IDC_USERNAME)->SetWindowText(r_InitialUsername);
	}
}

void ChangePasswordDlg::OnChangePasswordReply(bool i_NoPermission, bool i_UsernameNotFound)
{
	// pack the bools into wParam
	WPARAM wParam = static_cast<WPARAM>(i_NoPermission);
	
	wParam |= i_UsernameNotFound << 1;
	this->PostMessage(WM_CHANGEPASSWORDREPLYMESSAGE, wParam);
}

LRESULT ChangePasswordDlg::OnChangePasswordReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam & 1)
	{
		MessageBox(_T("You do not have permission to change this user's password!"), _T("CHANGE PASSWORD ERROR"), MB_ICONERROR);
	}
	else if (wParam >> 1)
	{
		MessageBox(_T("Username not found!"), _T("CHANGE PASSWORD ERROR"), MB_ICONERROR);
	}
	else
	{
		MessageBox(_T("Password changed successfully"), _T("Change Password"), MB_ICONINFORMATION);
		EndDialog(IDOK);
	}

	return TRUE;
}

void OnChangePasswordReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound)
{
	ChangePasswordDlg* theChangePasswordDlgObject = static_cast<ChangePasswordDlg*>(i_OptionalData);

	theChangePasswordDlgObject->OnChangePasswordReply(i_NoPermission, i_UsernameNotFound);
}

void ChangePasswordDlg::OnBnClickedOk()
{
	CString tempCString;

	GetDlgItem(IDC_USERNAME)->GetWindowText(tempCString);
	std::string username(CT2CA(tempCString, CP_UTF8));
	GetDlgItem(IDC_PW)->GetWindowText(tempCString);
	std::string password(CT2CA(tempCString, CP_UTF8));
	if (ChangePassword(theApp.GetModelDescriptor(), username, password) != eStatusCode_Success)
	{
		MessageBox(_T("The password must not be empty!"), _T("CHANGE PASSWORD ERROR"), MB_ICONERROR);
	}
}
