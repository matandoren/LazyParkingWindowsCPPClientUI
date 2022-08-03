// ParkingLotDlg.cpp : implementation file
//

#include "pch.h"
#include "LazyParkingWindowsCPPClientUI.h"
#include "ParkingLotDlg.h"
#include "afxdialogex.h"
#include "ChangePasswordDlg.h"
#include "ReserveParkingSpotDlg.h"
#include "InputBox.h"
#include "AddDriverOrUpdateDriverExpirationDlg.h"

#ifdef _DEBUG
#include <chrono>
#define START_MEASURE auto startTime = std::chrono::steady_clock::now();
#define END_MEASURE TRACE("OnPaint took: %lf seconds\n", std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count());
#else
#define START_MEASURE
#define END_MEASURE
#endif

// ParkingLotDlg dialog
IMPLEMENT_DYNAMIC(ParkingLotDlg, CDialogEx)
#define TOP_MARGIN_OF_LEFT_PARKING_ROW 3
#define LEFT_MARGIN_OF_LEFT_PARKING_ROW 6
#define TOP_MARGIN_OF_RIGHT_PARKING_ROW 130
#define LEFT_MARGIN_OF_RIGHT_PARKING_ROW 200
#define PARKING_SPOT_WIDTH 42
#define PARKING_SPOT_HEIGHT 82
#define FLOOR_INDICATOR_FONT_SIZE -30
#define WHITE_EDGES_FILTER_THRESHOLD 0xA8
#define POINT_FONT_SIZE_FOR_RESERVATIONS 100
#define FLOOR_IMAGE_FILE_PATH "floor.jpg"
#define CAR_IMAGE_FILE_PATH "car.png"
#define MOUSE_CLICK_TOLERANCE 3
#define MAX_RESERVATION_STRING_LENGTH 20

struct AddDriverReplyStruct
{
	bool m_NoPermission;
	bool m_DateExpired;
	std::string m_Username;
	int m_CardKey;

	AddDriverReplyStruct()
	{
		m_NoPermission = false;
		m_DateExpired = false;
		m_CardKey = 0;
	}
};

static void OnParkingStatusUpdateWrapper(void* i_OptionalData, int i_ParkingSpotID, bool i_IsOccupied, bool i_IsReserved, const std::string& i_ReservedFor);
static void OnCancelReservationReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_IDNotFound);
static void OnReserveParkingSpotReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_IDNotFound, bool i_DateExpired);
static void OnOpenGateReplyWrapper(void* i_OptionalData, bool i_IsSuccessful);
static void OnDeleteDriverReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound);
static void OnUpdateDriverExpirationReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound, bool i_DateExpired);
static void OnAddDriverReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_DateExpired, const std::string& i_Username, int i_CardKey);

ParkingLotDlg::ParkingLotDlg(int i_PermissionLevel, CString i_Username, MODEL_DESCRIPTOR i_ModelDescriptor, int i_LowestFloor, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PARKINGLOT_DIALOG, pParent), r_PermissionLevel(i_PermissionLevel), r_Username(i_Username), r_ModelDescriptor(i_ModelDescriptor), r_LowestFloor(i_LowestFloor),
	m_PaintingRect(MAP_RECT_LEFT, MAP_RECT_TOP, MAP_RECT_RIGHT, MAP_RECT_BOTTOM), m_UpButton(true), m_DownButton(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	m_CurrentFloorOffset = 0;
	m_PaintWholeMap = true;
	m_IsLMBDown = false;
	m_ClickedParkingSpotID = 0; // invalid id
	m_ExpiredUsersReportDlgPtr = nullptr;
	m_ParkingStatusUpdateCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_CancelReservationReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_ReserveParkingSpotReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_OpenGateReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_DeleteDriverReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_UpdateDriverExpirationReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
	m_AddDriverReplyCallbackDescriptor = LAZYPARKINGBLAPI_INVALID_DESCRIPTOR;
}

ParkingLotDlg::~ParkingLotDlg()
{
	RemoveParkingStatusUpdateEventHandler(r_ModelDescriptor, m_ParkingStatusUpdateCallbackDescriptor);
	RemoveCancelReservationReplyEventHandler(r_ModelDescriptor, m_CancelReservationReplyCallbackDescriptor);
	RemoveReserveParkingSpotReplyEventHandler(r_ModelDescriptor, m_ReserveParkingSpotReplyCallbackDescriptor);
	RemoveOpenGateReplyEventHandler(r_ModelDescriptor, m_OpenGateReplyCallbackDescriptor);
	RemoveDeleteDriverReplyEventHandler(r_ModelDescriptor, m_DeleteDriverReplyCallbackDescriptor);
	RemoveUpdateDriverExpirationReplyEventHandler(r_ModelDescriptor, m_UpdateDriverExpirationReplyCallbackDescriptor);
	RemoveAddDriverReplyEventHandler(r_ModelDescriptor, m_AddDriverReplyCallbackDescriptor);
	m_FloorIndicatorFont.DeleteObject();
}

void ParkingLotDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_UP, m_UpButton);
	DDX_Control(pDX, IDC_BUTTON_DOWN, m_DownButton);
}

BOOL ParkingLotDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Set font for the floor indicator
	LOGFONT newLogFont;
	GetDlgItem(IDC_FLOOR)->GetFont()->GetLogFont(&newLogFont);
	newLogFont.lfHeight = FLOOR_INDICATOR_FONT_SIZE;
	m_FloorIndicatorFont.CreateFontIndirectW(&newLogFont);
	GetDlgItem(IDC_FLOOR)->SetFont(&m_FloorIndicatorFont);
	// Set initial text for the floor indicator
	CString lowestFloorString;
	lowestFloorString.Format(_T("%d"), r_LowestFloor);
	GetDlgItem(IDC_FLOOR)->SetWindowText(lowestFloorString);
	// Initialize the down button as disabled since the dialog always launches in the lowest floor
	GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(false);
	// Load images from files
	m_FloorImage.Load(_T(FLOOR_IMAGE_FILE_PATH));
	m_CarImage.Load(_T(CAR_IMAGE_FILE_PATH));
	// Fix the transparent pixels of m_CarImage (CImage's alpha blending calculation is bugged so in order for it to work properly, the image has to be adjusted)
	// Since I am already at it, I also try to remove some of the white edges
	int pitch = m_CarImage.GetPitch();
	int width = m_CarImage.GetWidth();
	int height = m_CarImage.GetHeight();
	unsigned int* pixelPtr = (unsigned int*)m_CarImage.GetBits();
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (((pixelPtr[j] & 0xFF) >= WHITE_EDGES_FILTER_THRESHOLD) && (((pixelPtr[j] >> 8) & 0xFF) >= WHITE_EDGES_FILTER_THRESHOLD) && (((pixelPtr[j] >> 16) & 0xFF) >= WHITE_EDGES_FILTER_THRESHOLD))
			{
				pixelPtr[j] = 0;
			}
		}

		pixelPtr -= width;
	}

	if (AddParkingStatusUpdateEventHandler(r_ModelDescriptor, OnParkingStatusUpdateWrapper, this, m_ParkingStatusUpdateCallbackDescriptor) != eStatusCode_Success)
	{
		return FALSE;
	}

	// Permission level dependent initialization
	if (r_PermissionLevel >= OPERATOR_PERMISSION_LEVEL)
	{
		GetDlgItem(IDOPENGATE)->EnableWindow(true);
		GetDlgItem(IDOPENGATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDEXPIREDREPORT)->EnableWindow(true);
		GetDlgItem(IDEXPIREDREPORT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDREMOVEDRIVER)->EnableWindow(true);
		GetDlgItem(IDREMOVEDRIVER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDCHANGEEXPIRATION)->EnableWindow(true);
		GetDlgItem(IDCHANGEEXPIRATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDADDDRIVER)->EnableWindow(true);
		GetDlgItem(IDADDDRIVER)->ShowWindow(SW_SHOW);
		AddCancelReservationReplyEventHandler(r_ModelDescriptor, OnCancelReservationReplyWrapper, this, m_CancelReservationReplyCallbackDescriptor);
		AddReserveParkingSpotReplyEventHandler(r_ModelDescriptor, OnReserveParkingSpotReplyWrapper, this, m_ReserveParkingSpotReplyCallbackDescriptor);
		AddOpenGateReplyEventHandler(r_ModelDescriptor, OnOpenGateReplyWrapper, this, m_OpenGateReplyCallbackDescriptor);
		AddDeleteDriverReplyEventHandler(r_ModelDescriptor, OnDeleteDriverReplyWrapper, this, m_DeleteDriverReplyCallbackDescriptor);
		AddUpdateDriverExpirationReplyEventHandler(r_ModelDescriptor, OnUpdateDriverExpirationReplyWrapper, this, m_UpdateDriverExpirationReplyCallbackDescriptor);
		AddAddDriverReplyEventHandler(r_ModelDescriptor, OnAddDriverReplyWrapper, this, m_AddDriverReplyCallbackDescriptor);
		if (m_CancelReservationReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR
			|| m_ReserveParkingSpotReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR
			|| m_OpenGateReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR
			|| m_DeleteDriverReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR
			|| m_UpdateDriverExpirationReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR
			|| m_AddDriverReplyCallbackDescriptor == LAZYPARKINGBLAPI_INVALID_DESCRIPTOR)
		{
			return FALSE;
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

BEGIN_MESSAGE_MAP(ParkingLotDlg, CDialogEx)
	ON_WM_PAINT()
	ON_MESSAGE(WM_PARKINGSTATUSUPDATEMESSAGE, ParkingLotDlg::OnParkingStatusUpdateMessage)
	ON_MESSAGE(WM_CANCELRESERVATIONREPLYMESSAGE, ParkingLotDlg::OnCancelReservationReplyMessage)
	ON_MESSAGE(WM_RESERVEPARKINGSPOTREPLYMESSAGE, ParkingLotDlg::OnReserveParkingSpotReplyMessage)
	ON_MESSAGE(WM_OPENGATEREPLYMESSAGE, ParkingLotDlg::OnOpenGateReplyMessage)
	ON_MESSAGE(WM_DELETEDRIVERREPLYMESSAGE, ParkingLotDlg::OnDeleteDriverReplyMessage)
	ON_MESSAGE(WM_UPDATEDRIVEREXPIRATIONREPLYMESSAGE, ParkingLotDlg::OnUpdateDriverExpirationReplyMessage)
	ON_MESSAGE(WM_ADDDRIVERREPLYMESSAGE, ParkingLotDlg::OnAddDriverReplyMessage)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_UP, &ParkingLotDlg::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &ParkingLotDlg::OnBnClickedButtonDown)
	ON_BN_CLICKED(IDCHANGEPW, &ParkingLotDlg::OnBnClickedChangepw)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDOPENGATE, &ParkingLotDlg::OnBnClickedOpengate)
	ON_BN_CLICKED(IDREMOVEDRIVER, &ParkingLotDlg::OnBnClickedRemovedriver)
	ON_BN_CLICKED(IDCHANGEEXPIRATION, &ParkingLotDlg::OnBnClickedChangeexpiration)
	ON_BN_CLICKED(IDADDDRIVER, &ParkingLotDlg::OnBnClickedAdddriver)
	ON_BN_CLICKED(IDEXPIREDREPORT, &ParkingLotDlg::OnBnClickedExpiredreport)
END_MESSAGE_MAP()

void ParkingLotDlg::OnPaint()
{
	if (!IsIconic())
	{
		START_MEASURE
		CPaintDC dc(this); // device context for painting
		int firstIDOnFloor = (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS) * m_CurrentFloorOffset + 1;
		int lastIDOnFloor = firstIDOnFloor + (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS) - 1;
		CFont font;
		font.CreatePointFont(POINT_FONT_SIZE_FOR_RESERVATIONS, _T("Arial"));
		CFont* oldFont = dc.SelectObject(&font);

		if (m_PaintWholeMap) // There is a need to paint the whole map
		{
			m_FloorImage.Draw(dc.m_hDC, MAP_RECT_LEFT, MAP_RECT_TOP);
			for (int i = firstIDOnFloor; i <= lastIDOnFloor; i++)
			{
				paintParkingSpot(dc, i, false);
			}

			m_RepaintList.clear();
			m_PaintWholeMap = false;
		}
		else
		{
			while (!m_RepaintList.empty())
			{
				paintParkingSpot(dc, m_RepaintList.front(), true);
				m_RepaintList.pop_front();
			}
		}

		dc.SelectObject(oldFont);
		END_MEASURE
	}
}

bool ParkingLotDlg::isSpotInCurrentFloor(int i_ParkingSpotID)
{
	return (i_ParkingSpotID - 1) / (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS) == m_CurrentFloorOffset;
}

void ParkingLotDlg::calculatePaintingRect(int i_ParkingSpotID)
{
	i_ParkingSpotID = (i_ParkingSpotID - 1) % (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS);
	m_PaintingRect.left = MAP_RECT_LEFT + (i_ParkingSpotID < NUM_OF_LEFT_ROW_SPOTS ? LEFT_MARGIN_OF_LEFT_PARKING_ROW : LEFT_MARGIN_OF_RIGHT_PARKING_ROW);
	m_PaintingRect.top = MAP_RECT_TOP + (i_ParkingSpotID < NUM_OF_LEFT_ROW_SPOTS ? TOP_MARGIN_OF_LEFT_PARKING_ROW : TOP_MARGIN_OF_RIGHT_PARKING_ROW) 
		+ (i_ParkingSpotID % NUM_OF_LEFT_ROW_SPOTS) * PARKING_SPOT_WIDTH;
	m_PaintingRect.right = m_PaintingRect.left + PARKING_SPOT_HEIGHT;
	m_PaintingRect.bottom = m_PaintingRect.top + PARKING_SPOT_WIDTH;
}

void ParkingLotDlg::paintParkingSpot(CPaintDC& i_DC, int i_ParkingSpotID, bool i_PaintBackground)
{
	if (isSpotInCurrentFloor(i_ParkingSpotID))
	{
		bool isOccupied, isReserved;
		std::string reservedFor;

		GetParkingSpotStatus(r_ModelDescriptor, i_ParkingSpotID, isOccupied, isReserved, reservedFor);
		calculatePaintingRect(i_ParkingSpotID);
		if (i_PaintBackground && !isReserved)
		{
			m_FloorImage.Draw(i_DC.m_hDC, m_PaintingRect.left, m_PaintingRect.top, PARKING_SPOT_HEIGHT, PARKING_SPOT_WIDTH,
				m_PaintingRect.left - MAP_RECT_LEFT, m_PaintingRect.top - MAP_RECT_TOP, PARKING_SPOT_HEIGHT, PARKING_SPOT_WIDTH);
		}

		if (isReserved)
		{
			CString cstring(("Reserved for:\n" + reservedFor).c_str());
			i_DC.FillSolidRect(&m_PaintingRect, 0x0000FFFF); // draw a yellow rectangle for the background
			i_DC.DrawText(cstring, &m_PaintingRect, DT_CENTER | DT_VCENTER | DT_WORDBREAK);
		}

		if (isOccupied)
		{
			m_CarImage.Draw(i_DC.m_hDC, m_PaintingRect.left, m_PaintingRect.top);
		}
	}
}

bool ParkingLotDlg::isValidName(const CString& i_Name)
{
	int firstNameLength, lastNameLength, lastNameStartIdx;

	for (firstNameLength = 0; firstNameLength < i_Name.GetLength() && ((i_Name.GetAt(firstNameLength) >= _T('a') && i_Name.GetAt(firstNameLength) <= _T('z'))
		|| (i_Name.GetAt(firstNameLength) >= _T('A') && i_Name.GetAt(firstNameLength) <= _T('Z'))); firstNameLength++)
	{
	}

	if (firstNameLength == i_Name.GetLength() || i_Name.GetAt(firstNameLength) != _T(' '))
	{
		return false;
	}
	
	lastNameStartIdx = firstNameLength + 1;
	for (lastNameLength = 0; lastNameStartIdx + lastNameLength < i_Name.GetLength() && ((i_Name.GetAt(lastNameStartIdx + lastNameLength) >= _T('a') && i_Name.GetAt(lastNameStartIdx + lastNameLength) <= _T('z'))
		|| (i_Name.GetAt(lastNameStartIdx + lastNameLength) >= _T('A') && i_Name.GetAt(lastNameStartIdx + lastNameLength) <= _T('Z'))); lastNameLength++)
	{
	}

	if (lastNameStartIdx + lastNameLength < i_Name.GetLength() || firstNameLength < 2 || lastNameLength < 2 || (firstNameLength + lastNameLength) > 20)
	{
		return false;
	}

	return true;
}

// BL callback methods
void ParkingLotDlg::OnParkingStatusUpdate(int i_ParkingSpotID, bool i_IsOccupied, bool i_IsReserved, const std::string& i_ReservedFor)
{
	WPARAM wParam = static_cast<WPARAM>(i_ParkingSpotID);
	this->PostMessage(WM_PARKINGSTATUSUPDATEMESSAGE, wParam);
}

void ParkingLotDlg::OnCancelReservationReply(bool i_NoPermission, bool i_IDNotFound)
{
	// pack the bools into wParam
	WPARAM wParam = static_cast<WPARAM>(i_NoPermission);

	wParam |= i_IDNotFound << 1;
	this->PostMessage(WM_CANCELRESERVATIONREPLYMESSAGE, wParam);
}

void ParkingLotDlg::OnReserveParkingSpotReply(bool i_NoPermission, bool i_IDNotFound, bool i_DateExpired)
{
	// pack the bools into wParam
	WPARAM wParam = static_cast<WPARAM>(i_NoPermission);

	wParam |= i_IDNotFound << 1;
	wParam |= i_DateExpired << 2;
	this->PostMessage(WM_RESERVEPARKINGSPOTREPLYMESSAGE, wParam);
}

void ParkingLotDlg::OnOpenGateReply(bool i_IsSuccessful)
{
	this->PostMessage(WM_OPENGATEREPLYMESSAGE, i_IsSuccessful);
}

void ParkingLotDlg::OnDeleteDriverReply(bool i_NoPermission, bool i_UsernameNotFound)
{
	// pack the bools into wParam
	WPARAM wParam = static_cast<WPARAM>(i_NoPermission);

	wParam |= i_UsernameNotFound << 1;
	this->PostMessage(WM_DELETEDRIVERREPLYMESSAGE, wParam);
}

void ParkingLotDlg::OnUpdateDriverExpirationReply(bool i_NoPermission, bool i_UsernameNotFound, bool i_DateExpired)
{
	// pack the bools into wParam
	WPARAM wParam = static_cast<WPARAM>(i_NoPermission);

	wParam |= i_UsernameNotFound << 1;
	wParam |= i_DateExpired << 2;
	this->PostMessage(WM_UPDATEDRIVEREXPIRATIONREPLYMESSAGE, wParam);
}

void ParkingLotDlg::OnAddDriverReply(bool i_NoPermission, bool i_DateExpired, const std::string& i_Username, int i_CardKey)
{
	AddDriverReplyStruct* wParam = new AddDriverReplyStruct;

	wParam->m_NoPermission = i_NoPermission;
	wParam->m_DateExpired = i_DateExpired;
	wParam->m_Username = i_Username;
	wParam->m_CardKey = i_CardKey;
	this->PostMessage(WM_ADDDRIVERREPLYMESSAGE, reinterpret_cast<WPARAM>(wParam));
}

// BL callback method wrappers
void OnParkingStatusUpdateWrapper(void* i_OptionalData, int i_ParkingSpotID, bool i_IsOccupied, bool i_IsReserved, const std::string& i_ReservedFor)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnParkingStatusUpdate(i_ParkingSpotID, i_IsOccupied, i_IsReserved, i_ReservedFor);
}

void OnCancelReservationReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_IDNotFound)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnCancelReservationReply(i_NoPermission, i_IDNotFound);
}

void OnReserveParkingSpotReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_IDNotFound, bool i_DateExpired)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnReserveParkingSpotReply(i_NoPermission, i_IDNotFound, i_DateExpired);
}

void OnOpenGateReplyWrapper(void* i_OptionalData, bool i_IsSuccessful)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnOpenGateReply(i_IsSuccessful);
}

void OnDeleteDriverReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnDeleteDriverReply(i_NoPermission, i_UsernameNotFound);
}

void OnUpdateDriverExpirationReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_UsernameNotFound, bool i_DateExpired)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnUpdateDriverExpirationReply(i_NoPermission, i_UsernameNotFound, i_DateExpired);
}

void OnAddDriverReplyWrapper(void* i_OptionalData, bool i_NoPermission, bool i_DateExpired, const std::string& i_Username, int i_CardKey)
{
	ParkingLotDlg* theParkingLotDlg = static_cast<ParkingLotDlg*>(i_OptionalData);

	theParkingLotDlg->OnAddDriverReply(i_NoPermission, i_DateExpired, i_Username, i_CardKey);
}

// ParkingLotDlg message handlers
LRESULT ParkingLotDlg::OnParkingStatusUpdateMessage(WPARAM wParam, LPARAM lParam)
{
	if (isSpotInCurrentFloor(wParam))
	{
		calculatePaintingRect(wParam);
		m_RepaintList.push_front(wParam);
		this->InvalidateRect(&m_PaintingRect, false);
	}

	return TRUE;
}

LRESULT ParkingLotDlg::OnCancelReservationReplyMessage(WPARAM wParam, LPARAM lParam)
{
	MessageBox(_T("Reservation cancelled"), _T("Cancel Reservation"), MB_ICONINFORMATION);

	return TRUE;
}

LRESULT ParkingLotDlg::OnReserveParkingSpotReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam > 0)
	{
		MessageBox(_T("Date expired"), _T("Reserve Parking Spot"), MB_ICONERROR);
	}
	else
	{
		MessageBox(_T("Parking spot reserved successfully"), _T("Reserve Parking Spot"), MB_ICONINFORMATION);
	}

	return TRUE;
}

LRESULT ParkingLotDlg::OnOpenGateReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam)
	{
		MessageBox(_T("Gate opened"), _T("Open Gate"), MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(_T("You do not have permission to open the gate"), _T("Open Gate"), MB_ICONERROR);
	}

	return TRUE;
}

LRESULT ParkingLotDlg::OnDeleteDriverReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
	{
		if (m_ExpiredUsersReportDlgPtr != nullptr)
		{
			m_ExpiredUsersReportDlgPtr->PostMessage(WM_DELETEDRIVERREPLYMESSAGE_EXPIREDUSERSREPORT, wParam);
		}

		MessageBox(_T("The driver was removed successfully"), _T("Remove Driver"), MB_ICONINFORMATION);
	}
	else
	{
		MessageBox(_T("The username was not found or you do not have high enough permission to remove this user"), _T("Remove Driver"), MB_ICONERROR);
	}

	return TRUE;
}

LRESULT ParkingLotDlg::OnUpdateDriverExpirationReplyMessage(WPARAM wParam, LPARAM lParam)
{
	if (wParam == 0)
	{
		if (m_ExpiredUsersReportDlgPtr != nullptr)
		{
			m_ExpiredUsersReportDlgPtr->PostMessage(WM_UPDATEDRIVEREXPIRATIONREPLYMESSAGE_EXPIREDUSERSREPORT, wParam);
		}

		MessageBox(_T("The expiration date was updated successfully"), _T("Change Driver Expiration Date"), MB_ICONINFORMATION);
	}
	else if ((wParam >> 1) & 0x1)
	{
		MessageBox(_T("The username was not found"), _T("Change Driver Expiration Date"), MB_ICONERROR);
	}
	else
	{
		MessageBox(_T("The updated expiration date has already expired"), _T("Change Driver Expiration Date"), MB_ICONERROR);
	}

	return TRUE;
}

LRESULT ParkingLotDlg::OnAddDriverReplyMessage(WPARAM wParam, LPARAM lParam)
{
	AddDriverReplyStruct* replyPtr = reinterpret_cast<AddDriverReplyStruct*>(wParam);

	if (replyPtr->m_DateExpired)
	{
		MessageBox(_T("The expiration date has already expired"), _T("Add Driver"), MB_ICONERROR);
	}
	else
	{
		CString message(("Username: " + replyPtr->m_Username + "\nCard Key: ").c_str());

		message.AppendFormat(_T("%d"), replyPtr->m_CardKey);
		MessageBox(message, _T("Add Driver"), MB_ICONINFORMATION);
	}
	
	delete replyPtr;

	return TRUE;
}

void ParkingLotDlg::OnSize(UINT nType, int cx, int cy)
{
	if (nType == SIZE_RESTORED)
	{
		m_PaintWholeMap = true;
	}

	CDialogEx::OnSize(nType, cx, cy);
}


void ParkingLotDlg::OnBnClickedButtonUp()
{
	m_CurrentFloorOffset++;
	// Update the floor indicator
	CString currentFloorString;
	currentFloorString.Format(_T("%d"), r_LowestFloor + m_CurrentFloorOffset);
	GetDlgItem(IDC_FLOOR)->SetWindowText(currentFloorString);
	// Disable the up button if at the top floor
	if (m_CurrentFloorOffset == NUM_OF_FLOORS - 1)
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(false);
	}
	// Enable the down button if not at the bottom floor
	if (m_CurrentFloorOffset > 0)
	{
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(true);
	}
	// Paint the new floor
	m_PaintWholeMap = true;
	m_PaintingRect.left = MAP_RECT_LEFT;
	m_PaintingRect.right = MAP_RECT_RIGHT;
	m_PaintingRect.top = MAP_RECT_TOP;
	m_PaintingRect.bottom = MAP_RECT_BOTTOM;
	this->InvalidateRect(&m_PaintingRect, false);
}


void ParkingLotDlg::OnBnClickedButtonDown()
{
	m_CurrentFloorOffset--;
	// Update the floor indicator
	CString currentFloorString;
	currentFloorString.Format(_T("%d"), r_LowestFloor + m_CurrentFloorOffset);
	GetDlgItem(IDC_FLOOR)->SetWindowText(currentFloorString);
	// Enable the up button if not at the top floor
	if (m_CurrentFloorOffset < NUM_OF_FLOORS - 1)
	{
		GetDlgItem(IDC_BUTTON_UP)->EnableWindow(true);
	}
	// Disable the down button if at the bottom floor
	if (m_CurrentFloorOffset == 0)
	{
		GetDlgItem(IDC_BUTTON_DOWN)->EnableWindow(false);
	}
	// Paint the new floor
	m_PaintWholeMap = true;
	m_PaintingRect.left = MAP_RECT_LEFT;
	m_PaintingRect.right = MAP_RECT_RIGHT;
	m_PaintingRect.top = MAP_RECT_TOP;
	m_PaintingRect.bottom = MAP_RECT_BOTTOM;
	this->InvalidateRect(&m_PaintingRect, false);
}


void ParkingLotDlg::OnCancel()
{
	if (MessageBox(_T("Are you sure you want to log out?"), _T("Logout"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
	{
		CDialogEx::OnCancel();
	}
}


void ParkingLotDlg::OnBnClickedChangepw()
{
	ChangePasswordDlg dlg(r_PermissionLevel, r_Username);
	dlg.DoModal();
}


void ParkingLotDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_IsLMBDown = false;
	m_ClickedParkingSpotID = 0;

	if (r_PermissionLevel >= OPERATOR_PERMISSION_LEVEL)
	{
		if ((point.x >= (MAP_RECT_LEFT + LEFT_MARGIN_OF_LEFT_PARKING_ROW)) && (point.x <= (MAP_RECT_LEFT + LEFT_MARGIN_OF_LEFT_PARKING_ROW + PARKING_SPOT_HEIGHT))
			&& (point.y >= (MAP_RECT_TOP + TOP_MARGIN_OF_LEFT_PARKING_ROW)) && (point.y < (MAP_RECT_TOP + TOP_MARGIN_OF_LEFT_PARKING_ROW + PARKING_SPOT_WIDTH * NUM_OF_LEFT_ROW_SPOTS)))
		{
			m_ClickedParkingSpotID = (point.y - MAP_RECT_TOP - TOP_MARGIN_OF_LEFT_PARKING_ROW) / PARKING_SPOT_WIDTH + 1 + m_CurrentFloorOffset * (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS);
			m_IsLMBDown = true;
			m_MouseDownCoords = point;
		}

		if ((point.x >= (MAP_RECT_LEFT + LEFT_MARGIN_OF_RIGHT_PARKING_ROW)) && (point.x <= (MAP_RECT_LEFT + LEFT_MARGIN_OF_RIGHT_PARKING_ROW + PARKING_SPOT_HEIGHT))
			&& (point.y >= (MAP_RECT_TOP + TOP_MARGIN_OF_RIGHT_PARKING_ROW)) && (point.y < (MAP_RECT_TOP + TOP_MARGIN_OF_RIGHT_PARKING_ROW + PARKING_SPOT_WIDTH * NUM_OF_RIGHT_ROW_SPOTS)))
		{
			m_ClickedParkingSpotID = NUM_OF_LEFT_ROW_SPOTS + (point.y - MAP_RECT_TOP - TOP_MARGIN_OF_RIGHT_PARKING_ROW) / PARKING_SPOT_WIDTH + 1 + m_CurrentFloorOffset * (NUM_OF_LEFT_ROW_SPOTS + NUM_OF_RIGHT_ROW_SPOTS);
			m_IsLMBDown = true;
			m_MouseDownCoords = point;
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void ParkingLotDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	bool isReserved, isOccupied;
	std::string reservedFor;
	LONG dx = point.x - m_MouseDownCoords.x;
	LONG dy = point.y - m_MouseDownCoords.y;

	if (r_PermissionLevel >= OPERATOR_PERMISSION_LEVEL && m_IsLMBDown && ((dx * dx + dy * dy) <= MOUSE_CLICK_TOLERANCE * MOUSE_CLICK_TOLERANCE))
	{
		GetParkingSpotStatus(r_ModelDescriptor, m_ClickedParkingSpotID, isOccupied, isReserved, reservedFor);
		if (isReserved)
		{
			CString cancelReservationMessage;

			cancelReservationMessage.Format(_T("You are cancelling the reservation for parking spot %d"), m_ClickedParkingSpotID);
			if (MessageBox(cancelReservationMessage, _T("Cancel Reservation"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
			{
				CancelReservation(r_ModelDescriptor, m_ClickedParkingSpotID);
			}
		}
		else
		{
			ReserveParkingSpotDlg dlg(m_ClickedParkingSpotID);
			if (dlg.DoModal() == IDOK)
			{
				std::string reservedFor(CT2CA(dlg.m_ReservedFor, CP_UTF8));
				if (reservedFor.empty() || reservedFor.length() > MAX_RESERVATION_STRING_LENGTH)
				{
					MessageBox(_T("The name of the person/group for whom the parking spot is reserved must not exceed ") _T(_STRINGIZE(MAX_RESERVATION_STRING_LENGTH)
					_T(" characters and must not be empty")), _T("Reserve Parking Spot"), MB_ICONERROR);
				}
				else
				{
					ReserveParkingSpot(r_ModelDescriptor, m_ClickedParkingSpotID, reservedFor, dlg.m_IsNoExpiration ? 0 : dlg.m_ExpirationDate.GetTime());
				}
			}
		}
	}

	m_IsLMBDown = false;
	CDialogEx::OnLButtonUp(nFlags, point);
}


void ParkingLotDlg::OnBnClickedOpengate()
{
	if (MessageBox(_T("Are you sure you want to open the gate?"), _T("Open Gate"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
	{
		OpenGate(r_ModelDescriptor);
	}
}


void ParkingLotDlg::OnBnClickedRemovedriver()
{
	InputBox dlg(_T("Username to remove:"), _T("Remove Driver"));

	if (dlg.DoModal() == IDOK)
	{
		if (MessageBox(_T("Are you sure you want to remove ") + dlg.m_Input + _T("?"), _T("Remove Driver"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
		{
			std::string username(CT2CA(dlg.m_Input, CP_UTF8));
			DeleteDriver(r_ModelDescriptor, username);
		}
	}
}


void ParkingLotDlg::OnBnClickedChangeexpiration()
{
	AddDriverOrUpdateDriverExpirationDlg dlg(/* i_IsAddDriverDlg = */false);
	if (dlg.DoModal() == IDOK)
	{
		if (dlg.m_NameOrUsername.IsEmpty())
		{
			MessageBox(_T("The username must not be empty"), _T("Change Driver Expiration Date"), MB_ICONERROR);
		}
		else
		{
			std::string username(CT2CA(dlg.m_NameOrUsername, CP_UTF8));
			UpdateDriverExpiration(r_ModelDescriptor, username, dlg.m_IsNoExpiration ? 0 : dlg.m_ExpirationDate.GetTime());
		}
	}
}


void ParkingLotDlg::OnBnClickedAdddriver()
{
	AddDriverOrUpdateDriverExpirationDlg dlg(/* i_IsAddDriverDlg = */true);
	if (dlg.DoModal() == IDOK)
	{
		if (!isValidName(dlg.m_NameOrUsername))
		{
			MessageBox(_T("The name must contain only letters, must consist of a first name and a last name, each at least 2 letters long and their combined length must not exceed 20 letters;"
				" there must be exactly one space in the name - between the first name and the last name"), _T("Add Driver"), MB_ICONERROR);
		}
		else if (dlg.m_Password.IsEmpty())
		{
			MessageBox(_T("The password must not be empty"), _T("Add Driver"), MB_ICONERROR);
		}
		else
		{
			std::string username(CT2CA(dlg.m_NameOrUsername, CP_UTF8));
			std::string password(CT2CA(dlg.m_Password, CP_UTF8));
			AddDriver(r_ModelDescriptor, username, password, dlg.m_IsNoExpiration ? 0 : dlg.m_ExpirationDate.GetTime());
		}
	}
}


void ParkingLotDlg::OnBnClickedExpiredreport()
{
	ExpiredUsersReportDlg dlg;
	
	m_ExpiredUsersReportDlgPtr = &dlg;
	dlg.DoModal();
	m_ExpiredUsersReportDlgPtr = nullptr;
}
