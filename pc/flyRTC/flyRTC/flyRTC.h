
// flyRTC.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

// CflyRTCApp:
// See flyRTC.cpp for the implementation of this class
//

class CflyRTCApp : public CWinApp
{
public:
	CflyRTCApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
	
};

extern CflyRTCApp theApp;