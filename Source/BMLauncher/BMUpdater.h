#if !defined(AFX_BMUPDATER_H__CD271981_FF30_4E11_9821_D8470AFF3969__INCLUDED_)
#define AFX_BMUPDATER_H__CD271981_FF30_4E11_9821_D8470AFF3969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMUpdater.h : header file
//

#include <process.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <wininet.h>
#include "..\src\helper\linklist.h"
#include "..\src\helper\crc.h"

struct UpdateServer;

/////////////////////////////////////////////////////////////////////////////
// CBMUpdater dialog

class CBMUpdater : public CDialog
{
// Construction
public:
	CBMUpdater(CWnd* pParent = NULL);   // standard constructor

	CBMUpdater(const UpdateServer& updateServer, CWnd* pParent = NULL);

	void AddToTextBox(CString str);

// Dialog Data
	//{{AFX_DATA(CBMUpdater)
	enum { IDD = IDD_UPDATE_DIALOG };
	CRichEditCtrl	m_textControl;
	CButton	m_closeButton;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMUpdater)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

public:
	struct ThreadParams
	{
		ThreadParams(const UpdateServer* updateServer)
			: m_pUpdateServer(updateServer)
		{

		}

		HWND hWnd;
		const UpdateServer* m_pUpdateServer;
	};

// Implementation
protected:
	BOOL m_isUpdating;
	HANDLE hThread;
	unsigned int threadID;

	ThreadParams m_threadParams;

	// Generated message map functions
	//{{AFX_MSG(CBMUpdater)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_BMUPDATER_H__CD271981_FF30_4E11_9821_D8470AFF3969__INCLUDED_)
