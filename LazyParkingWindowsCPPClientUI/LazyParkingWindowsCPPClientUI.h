
// LazyParkingWindowsCPPClientUI.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include "API.h"

#define OPERATOR_PERMISSION_LEVEL 2
// LazyParkingWindowsCPPClientUIApp:
// See LazyParkingWindowsCPPClientUI.cpp for the implementation of this class
//

class LazyParkingWindowsCPPClientUIApp : public CWinApp
{
private:
	MODEL_DESCRIPTOR m_ModelDescriptor;

public:
	LazyParkingWindowsCPPClientUIApp();

// Overrides
public:
	MODEL_DESCRIPTOR GetModelDescriptor() const;
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern LazyParkingWindowsCPPClientUIApp theApp;
