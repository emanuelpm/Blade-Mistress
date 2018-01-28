// BMAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BMLauncher.h"
#include "BMAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMAboutDlg dialog


CBMAboutDlg::CBMAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBMAboutDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBMAboutDlg)
	m_text = _T("");
	//}}AFX_DATA_INIT
}


void CBMAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMAboutDlg)
	DDX_Text(pDX, IDC_LAUNCHER_TEXT, m_text);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CBMAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMAboutDlg message handlers

BOOL CBMAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_text = "Blade Mistress Updater V1.1";

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
