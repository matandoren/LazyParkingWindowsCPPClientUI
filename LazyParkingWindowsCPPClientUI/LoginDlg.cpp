
// LoginDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "LoginDlg.h"
#include "afxdialogex.h"
#include "ParkingLotDlg.h"
#include "InputBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define LOGO_IMAGE_FILE_PATH "lazy_parking_logo.jpg"
#define LOGO_RECT_TOP 7
#define LOGO_RECT_LEFT 7


// LoginDlg dialog

static void OnLogingReplyWrapper(void* i_OptionalData, int i_PermissionLevel, bool i_UsernameNotFound, bool i_WrongPW);

LoginDlg::LoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LOGIN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_LogingReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
}

LoginDlg::~LoginDlg()
{
	RemoveLoginReplyEventHandler(theApp.GetModelDescriptor(), m_LogingReplyCallbackDescriptor);
}

void LoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(LoginDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_LOGINREPLYMESSAGE, LoginDlg::OnLoginReplyMessage)
	ON_BN_CLICKED(IDC_ADVANCEDSETTINGSMFCMENUBUTTON, &LoginDlg::OnBnClickedAdvancedsettingsmfcmenubutton)
END_MESSAGE_MAP()


// CMFCExpDlg message handlers

BOOL LoginDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_LogoImage.Load(_T(LOGO_IMAGE_FILE_PATH));
	if (AddLoginReplyEventHandler(theApp.GetModelDescriptor(), OnLogingReplyWrapper, this, m_LogingReplyCallbackDescriptor) != eStatusCode_Success)
	{
		return FALSE;
	}

	m_Menu.LoadMenu(IDR_ADVANCEDSETTINGSMENU);
	CMFCMenuButton* menuButtonPtr = static_cast<CMFCMenuButton*>(GetDlgItem(IDC_ADVANCEDSETTINGSMFCMENUBUTTON));
	menuButtonPtr->m_hMenu = m_Menu.GetSubMenu(0)->GetSafeHmenu();
	menuButtonPtr->m_bDefaultClick = false;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void LoginDlg::OnPaint()
{
	if (!IsIconic())
	{
		CPaintDC dc(this); // device context for painting
		m_LogoImage.Draw(dc.m_hDC, LOGO_RECT_LEFT, LOGO_RECT_TOP);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR LoginDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL LoginDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if (wParam == IDOK)
	{
		CWnd* tempWindowPtr = GetFocus();
		if (tempWindowPtr != nullptr && tempWindowPtr->GetDlgCtrlID() == IDC_EDIT1)
		{
			PostMessage(WM_NEXTDLGCTL);
		}
		else
		{
			CString tempCString;

			GetDlgItem(IDC_EDIT1)->GetWindowText(tempCString);
			std::string username(CT2CA(tempCString, CP_UTF8));
			GetDlgItem(IDC_EDIT2)->GetWindowText(tempCString);
			std::string password(CT2CA(tempCString, CP_UTF8));
			eStatusCode returnValue = Login(theApp.GetModelDescriptor(), username, password);
			switch (returnValue)
			{
			case eStatusCode_BadArgumentsOrModelInImproperState:
				MessageBox(_T("The username and password must not be empty!"), _T("LOGIN ERROR"), MB_ICONERROR);
				break;
			case eStatusCode_SocketConnectionException:
				MessageBox(_T("Failed to connect to the server!"), _T("LOGIN ERROR"), MB_ICONERROR);
				break;
			}
		}

		return true;
	}

	return CDialogEx::OnCommand(wParam, lParam);
}

LRESULT LoginDlg::OnLoginReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam >= 256)
	{
		AfxMessageBox(_T("Wrong username or password!"), MB_ICONERROR);
	}
	else
	{
		CString username;
		GetDlgItem(IDC_EDIT1)->GetWindowText(username);
		ParkingLotDlg dlg(wParam, username, theApp.GetModelDescriptor());
		GetDlgItem(IDC_EDIT2)->SetWindowText(nullptr);
		this->ShowWindow(SW_HIDE);
		dlg.DoModal();
		Logout(theApp.GetModelDescriptor());
		this->ShowWindow(SW_SHOW);
	}

	return TRUE;
}

void LoginDlg::OnLoginReply(int i_PermissionLevel, bool i_UsernameNotFound, bool i_WrongPW)
{
	// pack the parameters into wparam
	WPARAM wParam = static_cast<WPARAM>(i_PermissionLevel);
	// while the i_PermissionLevel parameter is an int type, in practice it currently only holds the values 0, 1, or 2 so it only takes 2 bits to encode it
	// but for futureproofing it will be encoded in 8 bits
	wParam |= i_UsernameNotFound << 8;
	wParam |= i_WrongPW << 9;
	this->PostMessage(WM_LOGINREPLYMESSAGE, wParam);
}

void OnLogingReplyWrapper(void* i_OptionalData, int i_PermissionLevel, bool i_UsernameNotFound, bool i_WrongPW)
{
	LoginDlg* theCMFCExpDlgObject = static_cast<LoginDlg*>(i_OptionalData);

	theCMFCExpDlgObject->OnLoginReply(i_PermissionLevel, i_UsernameNotFound, i_WrongPW);
}

void LoginDlg::OnBnClickedAdvancedsettingsmfcmenubutton()
{
	InputBox changeIPDlg(_T("New IP Address:"), _T("Change Server IP Address"));
	InputBox changePortDlg(_T("New Port:"), _T("Change Server Port"));

	switch (static_cast<CMFCMenuButton*>(GetDlgItem(IDC_ADVANCEDSETTINGSMFCMENUBUTTON))->m_nMenuResult)
	{
	case ID_ADVANCEDSETTINGS_CHANGESERVERIPADDRESS:
		if (changeIPDlg.DoModal() == IDOK)
		{
			if (!changeIPDlg.m_Input.IsEmpty())
			{
				std::string newIP(CT2CA(changeIPDlg.m_Input, CP_UTF8));
				if (SetIP(theApp.GetModelDescriptor(), newIP) == eStatusCode_Success)
				{
					MessageBox(_T("Server IP address changed"), _T("Change Server IP Address"), MB_ICONINFORMATION);
				}
				else
				{
					MessageBox(_T("Failed to change server IP address"), _T("Change Server IP Address"), MB_ICONERROR);
				}
			}
		}

		break;
	case ID_ADVANCEDSETTINGS_CHANGESERVERPORT:
		if (changePortDlg.DoModal() == IDOK)
		{
			if (!changePortDlg.m_Input.IsEmpty())
			{
				std::string newPort(CT2CA(changePortDlg.m_Input, CP_UTF8));
				if (SetPort(theApp.GetModelDescriptor(), newPort) == eStatusCode_Success)
				{
					MessageBox(_T("Server Port changed"), _T("Change Server Port"), MB_ICONINFORMATION);
				}
				else
				{
					MessageBox(_T("Failed to change server port"), _T("Change Server Port"), MB_ICONERROR);
				}
			}
		}

		break;
	}
}
