// BMUpdater.cpp : implementation file
//

#include "stdafx.h"
#include "BMLauncher.h"
#include "BMUpdater.h"
#include "AutoUpdate.h"
#include "UpdateServer.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

unsigned __stdcall RunThread( void* params )
{	
	CBMUpdater::ThreadParams tp = *(CBMUpdater::ThreadParams*)params;

	AutoUpdate updater(*tp.m_pUpdateServer);
	updater.setDialog( tp.hWnd );
	updater.Update();

	SendMessage( tp.hWnd, WM_USER+2, NULL, NULL );

	_endthread( );

	if( updater.IsThereNewLauncher() )
		SendMessage( tp.hWnd, WM_USER+5, NULL, NULL );
	
	return 0;
}




/////////////////////////////////////////////////////////////////////////////
// CBMUpdater dialog


CBMUpdater::CBMUpdater(CWnd* pParent /*=NULL*/)
	: CDialog(CBMUpdater::IDD, pParent)
	, m_threadParams(NULL)
{
	//{{AFX_DATA_INIT(CBMUpdater)
	//}}AFX_DATA_INIT

	m_isUpdating = false;
}

CBMUpdater::CBMUpdater(const UpdateServer& updateServer, CWnd* pParent /*= NULL*/)
	: CDialog(CBMUpdater::IDD, pParent)
	, m_threadParams(&updateServer)
	, m_isUpdating(false)
{
	//{{AFX_DATA_INIT(CBMUpdater)
	//}}AFX_DATA_INIT
}


void CBMUpdater::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMUpdater)
	DDX_Control(pDX, IDC_TEXT, m_textControl);
	DDX_Control(pDX, ID_CLOSE, m_closeButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBMUpdater, CDialog)
	//{{AFX_MSG_MAP(CBMUpdater)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_CLOSE, OnClose)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMUpdater message handlers



void CBMUpdater::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
}

void CBMUpdater::OnClose() 
{
	EndDialog( 0 );
}



void CBMUpdater::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	unsigned int tmp;

	if( !m_isUpdating ) 
	{
		m_isUpdating = TRUE;
		m_threadParams.hWnd = this->GetSafeHwnd();

		hThread = (HANDLE) _beginthreadex( 
					NULL,
					0,
					&RunThread,
					&m_threadParams,
					0,
					&tmp
				);

		threadID = tmp;
	}
	
}


void CBMUpdater::AddToTextBox(CString str)
{
	int nLen = m_textControl.GetWindowTextLength();
	m_textControl.SetSel(nLen, nLen);   
	m_textControl.ReplaceSel(str);

	UpdateData( FALSE );
}

LRESULT CBMUpdater::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_USER+1 )
	{
		AddToTextBox( CString( (char*)wParam ) );
	}
	else if( message == WM_USER+2 )
	{
		GetDlgItem( ID_CLOSE )->EnableWindow( TRUE );
	}
	else if( message == WM_USER+5 )			// there is a new launcher avilable
	{
		//WinExec( "BMLInstaller", SW_SHOW );
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

void CBMUpdater::OnDestroy() 
{
	//_endthreadex( threadID );

	CloseHandle( hThread );
	CDialog::OnDestroy();
}
