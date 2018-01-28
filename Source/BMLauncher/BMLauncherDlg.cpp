// BMLauncherDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BMLauncher.h"
#include "BMLauncherDlg.h"
#include "BMUpdater.h"
#include "BMReadmeDlg.h"
#include "BMAboutDlg.h"
#include "UpdateServer.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


enum ServerType
{
	kServerType_Default,

	kServerType_MAX
};

static UpdateServer _updateServers[kServerType_MAX];

/////////////////////////////////////////////////////////////////////////////
// CBMLauncherDlg dialog

CBMLauncherDlg::CBMLauncherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBMLauncherDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBMLauncherDlg)
	m_rdoResolution = -1;
	m_rdoWindowType = -1;
	m_rdoServer = -1;
	m_txtAccount = _T("");
	m_txtPassword = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_hasUpdated = false;

	strcpy_s(_updateServers[kServerType_Default].pszServerName, UpdateServer::iServerNameSize, "Default");
	strcpy_s(_updateServers[kServerType_Default].pszServerURL, UpdateServer::iServerUrlSize, "http://updates.blademistress.com/");
}

void CBMLauncherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBMLauncherDlg)
	DDX_Control(pDX, IDC_BUTTON_LAUNCH, m_btnLaunch);
	DDX_Control(pDX, IDC_BUTTON_EXIT, m_btnExit);
	DDX_Radio(pDX, IDC_RADIO_640, m_rdoResolution);
	DDX_Radio(pDX, IDC_RADIO_FULLSCREEN, m_rdoWindowType);
	DDX_Radio(pDX, IDC_RADIO_SERVER_DEFAULT, m_rdoServer);
	DDX_Text(pDX, IDC_EDIT_ACCOUNT, m_txtAccount);
	DDV_MaxChars(pDX, m_txtAccount, 80);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_txtPassword);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_EDIT_ACCOUNT, m_accountControl);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_passwordControl);
}

BEGIN_MESSAGE_MAP(CBMLauncherDlg, CDialog)
	//{{AFX_MSG_MAP(CBMLauncherDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_LAUNCH, OnButtonLaunch)
	ON_BN_CLICKED(IDC_RADIO_SERVER_DEFAULT, OnRadioServerDefault)
	ON_BN_CLICKED(IDC_RADIO_FULLSCREEN, OnRadioFullscreen)
	ON_BN_CLICKED(IDC_RADIO_WINDOWED, OnRadioWindowed)
	ON_BN_CLICKED(IDC_RADIO_640, OnRadio640)
	ON_BN_CLICKED(IDC_RADIO_800, OnRadio800)
	ON_BN_CLICKED(IDC_RADIO_1024, OnRadio1024)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_RADIO_1600, OnRadio1600)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, OnButtonUpdate)
	ON_COMMAND(ID_MENU_ABOUT, OnMenuAbout)
	ON_COMMAND(ID_MENU_EXIT, OnMenuExit)
	ON_COMMAND(ID_MENU_FORUM, OnMenuForum)
	ON_COMMAND(ID_MENU_LAUNCH, OnMenuLaunch)
	ON_COMMAND(ID_MENU_README, OnMenuReadme)
	ON_COMMAND(ID_MENU_WEBSITE, OnMenuWebsite)
	ON_COMMAND(ID_MENU_WIKI, OnMenuWiki)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBMLauncherDlg message handlers

BOOL CBMLauncherDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	
	// Load the config file
	config.LoadConfig();

	m_passwordControl.SetLimitText( 12 );
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBMLauncherDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBMLauncherDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}

	//CBMUpdater updater;
	
	if( !m_hasUpdated )
	{
		m_hasUpdated = true;
		
		//updater.DoModal();

		//if( config.getUserName() != "User Name" && config.getUserName() != "" && config.getUserName() != NULL )
			//m_passwordControl.SetFocus();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBMLauncherDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CBMLauncherDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

	config.LoadConfig();

	m_rdoServer = config.getLastServer();
	m_rdoWindowType = config.getLastMode();
	m_rdoResolution = config.getLastResolution();
	m_txtAccount = config.getUserName();

	UpdateData( FALSE );
}

void CBMLauncherDlg::OnButtonExit() 
{
	DestroyWindow();
}

void CBMLauncherDlg::OnButtonLaunch() 
{
	char tmp[80];
	char* command = new char[256];
	command[0] = '\0';

	char* args = new char[256];
	args[0] = '\0';

	UpdateData( TRUE );

	m_txtAccount.Trim();

	if( m_txtAccount == "" ) {
		MessageBox( "You must enter in an account name." );
		UpdateData( FALSE );

		m_accountControl.SetFocus();

		return;
	}

	if( m_txtPassword == "" ) {
		MessageBox( "You must enter in a password." );

		m_passwordControl.SetFocus();

		return;
	}

	strcpy( tmp, (LPCSTR) m_txtAccount );

	// Save data to config file
	config.setLastServer( m_rdoServer );
	config.setLastMode( m_rdoWindowType );
	config.setLastResolution( m_rdoResolution );
	config.setUserName( tmp );
	config.SaveConfig();

	command = strcat( command, _updateServers[m_rdoServer].pszServerName );
	command = strcat( command, "\\bbonline.exe" );

	if( m_rdoWindowType == 1 )
		args = strcat( args, " -W" );

	if( m_rdoResolution == 1 )
		args = strcat( args, " -X" );
	else if( m_rdoResolution == 2 )
		args = strcat( args, " -Z" );
	else if( m_rdoResolution == 3 )
		args = strcat( args, " -Q" );

	CString tmpstr( m_txtAccount );
	CString tmpstr2( m_txtPassword );
	tmpstr.Replace( " ", "-" );
	tmpstr2.Replace( " ", "-" );

	args = strcat( args, " -N");
	args = strcat( args, (LPCSTR)tmpstr );
	args = strcat( args, " -P");
	args = strcat( args, (LPCSTR)tmpstr2 );

	if( m_rdoServer == kServerType_Default )
		args = strcat( args, " -game.blademistress.com" );

	char cCurrentPath[FILENAME_MAX];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));
	strcat_s(cCurrentPath, FILENAME_MAX, "\\");
	strcat_s(cCurrentPath, FILENAME_MAX, _updateServers[m_rdoServer].pszServerName);

	STARTUPINFO startInfo;
	memset(&startInfo, 0, sizeof(startInfo));
	startInfo.cb = sizeof(startInfo);

	PROCESS_INFORMATION procInfo;
	if (!CreateProcess( command, args, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, cCurrentPath, &startInfo, &procInfo ))
	{
		DWORD errNumber = GetLastError();
		char errStr[256] = { 0 };
		sprintf_s(errStr, 256, "Unable to launch game.  Error 0x%x.", errNumber);
		MessageBox(errStr, "Error");
	}

	UpdateData( FALSE );

	m_accountControl.SetFocus();
	m_accountControl.SetSel( m_txtAccount.GetLength(), m_txtAccount.GetLength() );

	delete[] command;
	delete[] args;
}

void CBMLauncherDlg::OnDestroy() 
{
	CDialog::OnDestroy();

	UpdateData( FALSE );
}

void CBMLauncherDlg::OnButtonUpdate() 
{
	//WinExec( "autoupdate.exe", SW_SHOW );
	CBMUpdater updater(_updateServers[m_rdoServer]);
	updater.DoModal();
}


void CBMLauncherDlg::OnRadioServerDefault() { m_rdoServer = kServerType_Default; }

void CBMLauncherDlg::OnRadioFullscreen() { m_rdoWindowType = 0; }
void CBMLauncherDlg::OnRadioWindowed() { m_rdoWindowType = 1; }

void CBMLauncherDlg::OnRadio640() { m_rdoResolution = 0; }
void CBMLauncherDlg::OnRadio800() { m_rdoResolution = 1; }
void CBMLauncherDlg::OnRadio1024() { m_rdoResolution = 2; }
void CBMLauncherDlg::OnRadio1600() { m_rdoResolution = 3; }


LRESULT CBMLauncherDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_USER+10 )
		OnButtonExit();

	return CDialog::WindowProc(message, wParam, lParam);
}


void CBMLauncherDlg::OnMenuAbout() 
{
	CBMAboutDlg about;
	about.DoModal();
}

void CBMLauncherDlg::OnMenuLaunch() 
{
	OnButtonLaunch();	
}

void CBMLauncherDlg::OnMenuExit() 
{
	OnButtonExit();	
}

void CBMLauncherDlg::OnMenuReadme() 
{
	CBMReadmeDlg readme;
	readme.DoModal();
}

void CBMLauncherDlg::OnMenuForum() 
{
	ShellExecute( NULL, "open", "http://forums.blademistress.com/", NULL, NULL, SW_SHOWNORMAL );	
}

void CBMLauncherDlg::OnMenuWebsite() 
{
	ShellExecute( NULL, "open", "http://www.blademistress.com", NULL, NULL, SW_SHOWNORMAL );		
}

void CBMLauncherDlg::OnMenuWiki() 
{
	ShellExecute( NULL, "open", "http://wiki.blademistress.com", NULL, NULL, SW_SHOWNORMAL );	
}
