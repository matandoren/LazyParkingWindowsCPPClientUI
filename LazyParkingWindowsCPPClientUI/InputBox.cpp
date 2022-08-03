// InputBox.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "InputBox.h"
#include "afxdialogex.h"


// InputBox dialog

IMPLEMENT_DYNAMIC(InputBox, CDialogEx)

InputBox::InputBox(const CString& i_Message, const CString& i_Title, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_INPUT_BOX_DIALOG, pParent), r_Message(i_Message), r_Title(i_Title)
{

}

InputBox::~InputBox()
{
}

void InputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(InputBox, CDialogEx)
END_MESSAGE_MAP()


// InputBox message handlers


BOOL InputBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetWindowText(r_Title);
	GetDlgItem(IDC_MESSAGE)->SetWindowText(r_Message);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void InputBox::OnOK()
{
	GetDlgItem(IDC_INPUT)->GetWindowText(m_Input);

	CDialogEx::OnOK();
}
