#if !defined(AFX_BMABOUTDLG_H__1EE230B6_E014_46BB_81DA_656ECE12794B__INCLUDED_)
#define AFX_BMABOUTDLG_H__1EE230B6_E014_46BB_81DA_656ECE12794B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMAboutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBMAboutDlg dialog

class CBMAboutDlg : public CDialog
{
// Construction
public:
	CBMAboutDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBMAboutDlg)
	enum { IDD = IDD_ABOUT_DIALOG };
	CString	m_text;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBMAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMABOUTDLG_H__1EE230B6_E014_46BB_81DA_656ECE12794B__INCLUDED_)
