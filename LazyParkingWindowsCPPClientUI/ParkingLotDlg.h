#pragma once

#include <forward_list>
#include "AppSpecificWindowsMessagesDefines.h"
#include "ArrowButton.h"
#include "ExpiredUsersReportDlg.h"

// ParkingLotDlg dialog
#define NUM_OF_FLOORS 2
#define NUM_OF_LEFT_ROW_SPOTS 19
#define NUM_OF_RIGHT_ROW_SPOTS 14
#define MAP_RECT_TOP 7
#define MAP_RECT_LEFT 7
#define MAP_RECT_BOTTOM (809 + 7)
#define MAP_RECT_RIGHT (294 + 7)

class ParkingLotDlg : public CDialogEx
{
	DECLARE_DYNAMIC(ParkingLotDlg)

public:
	ParkingLotDlg(int i_PermissionLevel, CString i_Username, MODEL_DESCRIPTOR i_ModelDescriptor, int i_LowestFloor = 1, CWnd* pParent = nullptr);   // standard constructor
	virtual ~ParkingLotDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PARKINGLOT_DIALOG };
#endif
protected:
	HICON m_hIcon;

private:
	CImage m_FloorImage;
	CImage m_CarImage;
	const int r_PermissionLevel;
	const int r_LowestFloor;
	int m_CurrentFloorOffset;
	const MODEL_DESCRIPTOR r_ModelDescriptor;
	CALLBACK_DESCRIPTOR m_ParkingStatusUpdateCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_CancelReservationReplyCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_ReserveParkingSpotReplyCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_OpenGateReplyCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_DeleteDriverReplyCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_UpdateDriverExpirationReplyCallbackDescriptor;
	CALLBACK_DESCRIPTOR m_AddDriverReplyCallbackDescriptor;
	CRect m_PaintingRect;
	std::forward_list<int> m_RepaintList;
	bool m_PaintWholeMap;
	ArrowButton m_UpButton;
	ArrowButton m_DownButton;
	CFont m_FloorIndicatorFont;
	const CString r_Username;
	bool m_IsLMBDown;
	CPoint m_MouseDownCoords;
	int m_ClickedParkingSpotID;
	ExpiredUsersReportDlg* m_ExpiredUsersReportDlgPtr;

	void paintParkingSpot(CPaintDC& i_DC, int i_ParkingSpotID, bool i_PaintBackground);
	void calculatePaintingRect(int i_ParkingSpotID);
	bool isSpotInCurrentFloor(int i_ParkingSpotID);
	bool isValidName(const CString& i_Name);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg LRESULT OnParkingStatusUpdateMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCancelReservationReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReserveParkingSpotReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOpenGateReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeleteDriverReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUpdateDriverExpirationReplyMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAddDriverReplyMessage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

public:
	void OnParkingStatusUpdate(int i_ParkingSpotID, bool i_IsOccupied, bool i_IsReserved, const std::string& i_ReservedFor);
	void OnCancelReservationReply(bool i_NoPermission, bool i_IDNotFound);
	void OnReserveParkingSpotReply(bool i_NoPermission, bool i_IDNotFound, bool i_DateExpired);
	void OnOpenGateReply(bool i_IsSuccessful);
	void OnDeleteDriverReply(bool i_NoPermission, bool i_UsernameNotFound);
	void OnUpdateDriverExpirationReply(bool i_NoPermission, bool i_UsernameNotFound, bool i_DateExpired);
	void OnAddDriverReply(bool i_NoPermission, bool i_DateExpired, const std::string& i_Username, int i_CardKey);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	virtual void OnCancel();
	afx_msg void OnBnClickedChangepw();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedOpengate();
	afx_msg void OnBnClickedRemovedriver();
	afx_msg void OnBnClickedChangeexpiration();
	afx_msg void OnBnClickedAdddriver();
	afx_msg void OnBnClickedExpiredreport();
};
