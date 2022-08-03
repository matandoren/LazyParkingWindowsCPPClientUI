// ExpiredUsersReportDlg.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "ExpiredUsersReportDlg.h"
#include "AddDriverOrUpdateDriverExpirationDlg.h"
#include "afxdialogex.h"


// ExpiredUsersReportDlg dialog
static void OnRequestExpiredUsersReplyWrapper(void* i_OptionalData, int i_NumberOfExpiredUsers);

IMPLEMENT_DYNAMIC(ExpiredUsersReportDlg, CDialogEx)

ExpiredUsersReportDlg::ExpiredUsersReportDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EXPIRED_USERS_DIALOG, pParent)
{
	m_RequestExpiredUsersReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_MostRecentlySelectedIdx = -1; // invalid index - indicate that no item is currently selected
}

ExpiredUsersReportDlg::~ExpiredUsersReportDlg()
{
	RemoveRequestExpiredUsersReplyEventHandler(theApp.GetModelDescriptor(), m_RequestExpiredUsersReplyCallbackDescriptor);
}

void ExpiredUsersReportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(ExpiredUsersReportDlg, CDialogEx)
	ON_MESSAGE(WM_REQUESTEXPIREDUSERSREPLYMESSAGE, ExpiredUsersReportDlg::OnRequestExpiredUsersReplyMessage)
	ON_BN_CLICKED(IDC_DELETE_DRIVER_BUTTON, &ExpiredUsersReportDlg::OnBnClickedDeleteDriverButton)
	ON_MESSAGE(WM_DELETEDRIVERREPLYMESSAGE_EXPIREDUSERSREPORT, ExpiredUsersReportDlg::OnDeleteDriverOrUpdateDriverExpirationReplyMessage)
	ON_BN_CLICKED(IDC_UPDATE_EXPIRATION_BUTTON, &ExpiredUsersReportDlg::OnBnClickedUpdateExpirationButton)
	ON_MESSAGE(WM_UPDATEDRIVEREXPIRATIONREPLYMESSAGE_EXPIREDUSERSREPORT, ExpiredUsersReportDlg::OnDeleteDriverOrUpdateDriverExpirationReplyMessage)
END_MESSAGE_MAP()


// ExpiredUsersReportDlg message handlers

BOOL ExpiredUsersReportDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	CListCtrl* listCtrl = static_cast<CListCtrl*>(GetDlgItem(IDC_EXPIRED_USERS_LIST));
	CRect clientRect;
	int usernameColWidth;

	listCtrl->GetClientRect(clientRect);
	usernameColWidth = (clientRect.right - clientRect.left) / 2;
	listCtrl->InsertColumn(0, _T("Username"), LVCFMT_LEFT | LVCFMT_FIXED_WIDTH, usernameColWidth);
	listCtrl->InsertColumn(1, _T("Expiration Date"), LVCFMT_LEFT | LVCFMT_FIXED_WIDTH, clientRect.right - clientRect.left - usernameColWidth);
	AddRequestExpiredUsersReplyEventHandler(theApp.GetModelDescriptor(), OnRequestExpiredUsersReplyWrapper, this, m_RequestExpiredUsersReplyCallbackDescriptor);
	RequestExpiredUsers(theApp.GetModelDescriptor());

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void ExpiredUsersReportDlg::OnBnClickedDeleteDriverButton()
{
	CListCtrl* listCtrl = static_cast<CListCtrl*>(GetDlgItem(IDC_EXPIRED_USERS_LIST));
	POSITION selectedItemIterator = listCtrl->GetFirstSelectedItemPosition();

	if (selectedItemIterator != NULL)
	{
		m_MostRecentlySelectedIdx = listCtrl->GetNextSelectedItem(selectedItemIterator);
		CString itemText = listCtrl->GetItemText(m_MostRecentlySelectedIdx, 0);
		if (MessageBox(_T("Are you sure you want to remove ") + itemText + _T("?"), _T("Remove Driver"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
		{

			std::string username(CT2CA(itemText, CP_UTF8));
			DeleteDriver(theApp.GetModelDescriptor(), username);
		}
	}
}

void ExpiredUsersReportDlg::OnBnClickedUpdateExpirationButton()
{
	CListCtrl* listCtrl = static_cast<CListCtrl*>(GetDlgItem(IDC_EXPIRED_USERS_LIST));
	POSITION selectedItemIterator = listCtrl->GetFirstSelectedItemPosition();

	if (selectedItemIterator != NULL)
	{
		m_MostRecentlySelectedIdx = listCtrl->GetNextSelectedItem(selectedItemIterator);
		CString itemText = listCtrl->GetItemText(m_MostRecentlySelectedIdx, 0);
		AddDriverOrUpdateDriverExpirationDlg dlg(/* i_IsAddDriverDlg = */false, itemText);
		if (dlg.DoModal() == IDOK)
		{
			std::string username(CT2CA(itemText, CP_UTF8));
			UpdateDriverExpiration(theApp.GetModelDescriptor(), username, dlg.m_IsNoExpiration ? 0 : dlg.m_ExpirationDate.GetTime());
		}
	}
}

LRESULT ExpiredUsersReportDlg::OnRequestExpiredUsersReplyMessage(WPARAM wParam, LPARAM lParam)
{
	CListCtrl* listCtrl = static_cast<CListCtrl*>(GetDlgItem(IDC_EXPIRED_USERS_LIST));
	std::string username;
	time_t expirationDate;
	MODEL_DESCRIPTOR modelDescriptor = theApp.GetModelDescriptor();

	for (int i = 0; i < lParam; i++)
	{
		GetNextExpiredUser(modelDescriptor, username, expirationDate);
		CString strText(username.c_str());
		strText.SetString(strText);
		listCtrl->InsertItem(i, strText);
		CTime cTimeExpirationDate(expirationDate);
		strText.SetString(cTimeExpirationDate.Format(_T("%d-%m-%Y")));
		listCtrl->SetItemText(i, 1, strText);
	}

	return TRUE;
}

LRESULT ExpiredUsersReportDlg::OnDeleteDriverOrUpdateDriverExpirationReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (m_MostRecentlySelectedIdx >= 0)
	{
		static_cast<CListCtrl*>(GetDlgItem(IDC_EXPIRED_USERS_LIST))->DeleteItem(m_MostRecentlySelectedIdx);
		m_MostRecentlySelectedIdx = -1;
	}

	return TRUE;
}

// BL callback methods
void ExpiredUsersReportDlg::OnRequestExpiredUsersReply(int i_NumberOfExpiredUsers)
{
	this->PostMessage(WM_REQUESTEXPIREDUSERSREPLYMESSAGE, 0, i_NumberOfExpiredUsers);
}

// BL callback method wrappers
void OnRequestExpiredUsersReplyWrapper(void* i_OptionalData, int i_NumberOfExpiredUsers)
{
	ExpiredUsersReportDlg* theExpiredUsersReportDlg = static_cast<ExpiredUsersReportDlg*>(i_OptionalData);

	theExpiredUsersReportDlg->OnRequestExpiredUsersReply(i_NumberOfExpiredUsers);
}