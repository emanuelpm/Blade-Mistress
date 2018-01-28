#if !defined(AFX_BMREADMEDLG_H__788B93F7_0E72_4E52_A75C_B7E42FC471E8__INCLUDED_)
#define AFX_BMREADMEDLG_H__788B93F7_0E72_4E52_A75C_B7E42FC471E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BMReadmeDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBMReadmeDlg dialog

class CBMReadmeDlg : public CDialog
{
// Construction
public:
	CBMReadmeDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBMReadmeDlg)
	enum { IDD = IDD_README_DIALOG };
	CRichEditCtrl	m_textControl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMReadmeDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBMReadmeDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMREADMEDLG_H__788B93F7_0E72_4E52_A75C_B7E42FC471E8__INCLUDED_)
