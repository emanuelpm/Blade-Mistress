// BMLauncher.h : main header file for the BMLAUNCHER application
//

#if !defined(AFX_BMLAUNCHER_H__D5E69505_53E2_46D9_B005_7DB75FF4B03A__INCLUDED_)
#define AFX_BMLAUNCHER_H__D5E69505_53E2_46D9_B005_7DB75FF4B03A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBMLauncherApp:
// See BMLauncher.cpp for the implementation of this class
//

class CBMLauncherApp : public CWinApp
{
public:
	CBMLauncherApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMLauncherApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CBMLauncherApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMLAUNCHER_H__D5E69505_53E2_46D9_B005_7DB75FF4B03A__INCLUDED_)
