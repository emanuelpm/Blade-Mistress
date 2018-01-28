// BMLauncherDlg.h : header file
//

#if !defined(AFX_BMLAUNCHERDLG_H__8FC58C78_2C10_46E7_9A18_E4EF1A4953A9__INCLUDED_)
#define AFX_BMLAUNCHERDLG_H__8FC58C78_2C10_46E7_9A18_E4EF1A4953A9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "LauncherConfig.h"
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// CBMLauncherDlg dialog

class CBMLauncherDlg : public CDialog
{
// Construction
public:
	CBMLauncherDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CBMLauncherDlg)
	enum { IDD = IDD_BMLAUNCHER_DIALOG };
	CButton	m_btnLaunch;
	CButton	m_btnExit;
	int		m_rdoResolution;
	int		m_rdoWindowType;
	int		m_rdoServer;
	CString	m_txtAccount;
	CString	m_txtPassword;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBMLauncherDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CBMLauncherDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnButtonExit();
	afx_msg void OnButtonLaunch();
	afx_msg void OnRadioServerDefault();
	afx_msg void OnRadioFullscreen();
	afx_msg void OnRadioWindowed();
	afx_msg void OnRadio640();
	afx_msg void OnRadio800();
	afx_msg void OnRadio1024();
	afx_msg void OnDestroy();
	afx_msg void OnChangeEditAccount();
	afx_msg void OnUpdateEditAccount();
	afx_msg void OnRadio1600();
	afx_msg void OnButtonUpdate();
	afx_msg void OnMenuAbout();
	afx_msg void OnMenuBlog();
	afx_msg void OnMenuExit();
	afx_msg void OnMenuForum();
	afx_msg void OnMenuLaunch();
	afx_msg void OnMenuReadme();
	afx_msg void OnMenuWebsite();
	afx_msg void OnMenuWiki();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	BOOL m_hasUpdated;
	LauncherConfig config;
public:
	CEdit m_accountControl;
	CEdit m_passwordControl;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BMLAUNCHERDLG_H__8FC58C78_2C10_46E7_9A18_E4EF1A4953A9__INCLUDED_)
