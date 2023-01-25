// flashcard.h : main header file for the flashcard application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resourceppc.h"

// CflashcardApp:
// See flashcard.cpp for the implementation of this class
//

class CflashcardApp : public CWinApp
{
public:
	CflashcardApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
public:
	afx_msg void OnAppAbout();

	DECLARE_MESSAGE_MAP()
};

extern CflashcardApp theApp;
