// BMReadmeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BMLauncher.h"
#include "BMReadmeDlg.h"
#include <fstream>
#include <ios>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBMReadmeDlg dialog


CBMReadmeDlg::CBMReadmeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBMReadmeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBMReadmeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBMReadmeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMReadmeDlg)
	DDX_Control(pDX, IDC_TEXT, m_textControl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMReadmeDlg, CDialog)
	//{{AFX_MSG_MAP(CBMReadmeDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMReadmeDlg message handlers

BOOL CBMReadmeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	std::fstream in;
	in.open( "readme.txt", std::ios::in );
	
	if (in.is_open())
	{
		char tmp[256];
		while( !in.eof() ) 
		{
			in.getline( tmp, 256 );
			int nLen = m_textControl.GetWindowTextLength();
			m_textControl.SetSel(nLen, nLen);   
			m_textControl.ReplaceSel( CString(tmp) + CString( "\r\n" ) );
		}
	}	

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
