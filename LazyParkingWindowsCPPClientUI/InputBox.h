#pragma once


// InputBox dialog

class InputBox : public CDialogEx
{
	DECLARE_DYNAMIC(InputBox)

public:
	const CString r_Message;
	const CString r_Title;
	CString m_Input;

	InputBox(const CString& i_Message, const CString& i_Title, CWnd* pParent = nullptr);   // standard constructor
	virtual ~InputBox();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUT_BOX_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
