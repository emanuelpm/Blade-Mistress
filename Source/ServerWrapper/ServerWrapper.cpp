// ServerWrapper.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wininet.h>

	// System Headers.
#include <winsock2.h>
#include <mmsystem.h>

#include "..\src\BBOServer.h"
#include "..\src\BBO-Savatar.h"
#include "..\src\version.h"

#include "..\src\Dungeon-Map.h"

#define MAX_LOADSTRING 100

BBOServer *server = NULL;

char headerText[2048];

void UpdateScreen(void);
void DrawScreen(HWND hWnd, HDC hdc);
void SortRecords(void);

//*********************************************************!!!!!!
struct playerRecord
{
	char userName[32],avatarName[32], placeName[32];
	int socketVal, x,y, health, maxHealth;
	long lifeTime;
	int isInWorld, isPaid;
	long money, totalVal;
};

const int NUM_OF_PLAYER_RECORDS = 200;
const int NUM_OF_SCREEN_RECORDS = 35;

const int PLAYER_INFO_LEFT = 4;
const int PLAYER_INFO_TOP = 40;
const int PLAYER_TEXT_HEIGHT = 14;

playerRecord recordArray[NUM_OF_PLAYER_RECORDS];
int validRecordNum = 0;
int recordStartIndex = 0;
int sortType = 0;
COLORREF infoColor[7];

int historyCounter = 0, historyTicker = 0;
int playerNumHistoryList[300];
int cyclesHistoryList[300];


// =============================================
// This function gets called when any of the
// server managed sockets have an event get 
// signalled.
// =============================================

//***********************************************************************************
int GlobalUDPNetCallback(	UDPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
							int size, void * context)
{
		// varaibles.
	Server *	server		= NULL;
//	Client *	client		= NULL;
	int			bytesParsed = 0;
/*
	if(serverWorld)
	{
		server	= (Server *)serverWorld->radio;
	}
		
	if(clientWorld1)
	{
		client	= (Client *)clientWorld1->radio;
	}
*/

		// try the server first.
	if(server != NULL)
	{
		bytesParsed = server->UDPNetCallback(socket, events, buffer, size, server);
	}
/*
	if((bytesParsed == 0) && (client != NULL))
	{
		bytesParsed = client->UDPNetCallback(socket, events, buffer, size, client);
	}
  */
	if(bytesParsed == 0)
	{
		bytesParsed = size;
	}

	return bytesParsed;
}

// Global Variables:
HINSTANCE hInst;								// current instance
HWND hWnd;
TCHAR szTitle[MAX_LOADSTRING];								// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];								// The title bar text

// Foward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int cyclesCompleted = 0, lastIters = 0;

//***********************************************************************************
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_SERVERWRAPPER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

#ifdef NO_IOCP
	server = new BBOServer(FALSE);
#else
	server = new BBOServer();
#endif

	DWORD lastTime = 0;

	sprintf(headerText,"BBO server app 0.5\nby Thom Robertson\n");
//					SortRecords();
	for (int i = 0; i < 7; ++i)
		infoColor[i] = RGB(255, 255, 255);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SERVERWRAPPER);

      // Enter the message loop
//      MSG msg;
   ZeroMemory( &msg, sizeof(msg) );
   while( msg.message!=WM_QUIT )
   {
      if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
		else
      {
		   if (server)
			{
			   server->Tick();
				++cyclesCompleted;

				if (0 == server->pleaseKillMe)
					PostQuitMessage(0);
			}
			DWORD thisTime = timeGetTime();
			if (lastTime < thisTime - 1000 * 10) // every ten seconds
			{
#ifdef NO_IOCP
				server->lserver->CollectGarbage();
#endif
				lastTime = thisTime;

				UpdateScreen();

				// tell screen to re-draw
				InvalidateRect(hWnd, NULL, TRUE); 

				++historyTicker;
				if (historyTicker >= 6 * 5) // every 5 minutes
//				if (historyTicker >= 1) // every 5 minutes
				{
					// get another point for the data graph
					historyTicker = 0;
					playerNumHistoryList[historyCounter] = validRecordNum;
//					playerNumHistoryList[historyCounter] = validRecordNum + rand() % 30;
					cyclesHistoryList[historyCounter] = lastIters;
					++historyCounter;
					if (historyCounter >= 300)
						historyCounter = 0;
				}
			}
		}
	}
   

	// Main message loop:
/*
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (server)
			server->Tick();
	}
*/
	delete server;
	WSACleanup();


#ifdef _DETECT_LEAKS
	DumpUnfreed();
#endif

	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage is only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
//***********************************************************************************
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SERVERWRAPPER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_SERVERWRAPPER;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
//***********************************************************************************
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   DWORD m_dwWindowStyle;     // Saved window style for mode switches
        m_dwWindowStyle = WS_OVERLAPPEDWINDOW;

   RECT rc;
   SetRect( &rc, 0, 0, 1000, 620 );
   AdjustWindowRect( &rc, m_dwWindowStyle, TRUE );

   hWnd = CreateWindow(szWindowClass, szTitle, m_dwWindowStyle,
                       4,8,
                       (rc.right-rc.left), (rc.bottom-rc.top),
							  NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
//***********************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	TCHAR szHello[MAX_LOADSTRING];
	LoadString(hInst, IDS_HELLO, szHello, MAX_LOADSTRING);

	switch (message) 
	{
		case WM_COMMAND:
			wmId    = LOWORD(wParam); 
			wmEvent = HIWORD(wParam); 
			// Parse the menu selections:
			switch (wmId)
			{
				case IDM_ABOUT:
				   DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
				   break;
				case IDM_EXIT:
				   DestroyWindow(hWnd);
				   break;
				default:
				   return DefWindowProc(hWnd, message, wParam, lParam);
			}
			break;
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);
			DrawScreen(hWnd, hdc);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case 40:	// down arrow
				{
					++recordStartIndex;
					InvalidateRect(hWnd, NULL, TRUE); 
					break;
				}
				case 38: // up arrow
				{
					if (recordStartIndex > 0)
					{
						--recordStartIndex;
						InvalidateRect(hWnd, NULL, TRUE); 
					}
					break;
				}
				case 37: // left? arrow
				{
					--sortType;
					SortRecords();
					InvalidateRect(hWnd, NULL, TRUE); 
					break;
				}
				case 39: // right? arrow
				{
					++sortType;
					SortRecords();
					InvalidateRect(hWnd, NULL, TRUE); 
					break;
				}
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
//***********************************************************************************
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
				return TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
			{
				EndDialog(hDlg, LOWORD(wParam));
				return TRUE;
			}
			break;
	}
    return FALSE;
}

//***********************************************************************************
void UpdateScreen(void)
{

	int totalPlayers = 0;
	// re-write headerText
	sprintf(headerText,"BBO %1.2f   ", VERSION_NUMBER);

	sprintf(&(headerText[strlen(headerText)]),
				"Iters %5.1f    ", (float) cyclesCompleted/10.0f);
	lastIters = cyclesCompleted/10;
	cyclesCompleted = 0;

	DWORD timeSinceConnect = (timeGetTime() - server->lastConnectTime) / 1000 / 60;

	sprintf(&(headerText[strlen(headerText)]),
				"Last conn: %ld", timeSinceConnect);

	validRecordNum = 0;

#ifdef NO_IOCP
	srand(1);

	for (int testNum = 0; testNum < 50; ++testNum)
	{
		sprintf(recordArray[validRecordNum].userName,"Test %d WWWWWWW  iii  JJJ", testNum);
		recordArray[validRecordNum].socketVal = testNum + 1000;

		sprintf(recordArray[validRecordNum].placeName,"Logging in...%d ",testNum);
		sprintf(recordArray[validRecordNum].avatarName,"AvName %d", testNum);
		recordArray[validRecordNum].health = 100;
		recordArray[validRecordNum].lifeTime = 20000+testNum;
		recordArray[validRecordNum].maxHealth = 100+testNum;
		recordArray[validRecordNum].x = rand() % 128;
		recordArray[validRecordNum].y = rand() % 128;
		recordArray[validRecordNum].isInWorld = TRUE;
		recordArray[validRecordNum].isPaid    = FALSE;

		recordArray[validRecordNum].money    = testNum * 10;
		recordArray[validRecordNum].totalVal = testNum * 15;

		++validRecordNum;
	}
#endif

	BBOSAvatar *curAvatar = (BBOSAvatar *) server->incoming->First();
	while (curAvatar && validRecordNum < NUM_OF_PLAYER_RECORDS)
	{
		sprintf(recordArray[validRecordNum].userName,curAvatar->name);
		recordArray[validRecordNum].socketVal = curAvatar->socketIndex;

		sprintf(recordArray[validRecordNum].placeName,"Logging in...");
		sprintf(recordArray[validRecordNum].avatarName," ");
		recordArray[validRecordNum].health = 0;
		recordArray[validRecordNum].lifeTime = 0;
		recordArray[validRecordNum].maxHealth = 0;
		recordArray[validRecordNum].x = 0;
		recordArray[validRecordNum].y = 0;
		recordArray[validRecordNum].money    = 0;
		recordArray[validRecordNum].totalVal = 0;
		recordArray[validRecordNum].isPaid   = 0;

		if (curAvatar->hasPaid)
			recordArray[validRecordNum].isPaid    = 1;
		if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
			recordArray[validRecordNum].isPaid    = 2;
		if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
			recordArray[validRecordNum].isPaid    = 3;

		++totalPlayers;
		++validRecordNum;
		curAvatar = (BBOSAvatar *) server->incoming->Next();
	}

	SharedSpace *sp = (SharedSpace *) server->spaceList->First();
	while (sp && validRecordNum < NUM_OF_PLAYER_RECORDS)
	{
		BBOSAvatar *curAvatar = (BBOSAvatar *) sp->avatars->First();
		while (curAvatar && validRecordNum < NUM_OF_PLAYER_RECORDS)
		{
			++totalPlayers;

			sprintf(recordArray[validRecordNum].userName,curAvatar->name);
			recordArray[validRecordNum].socketVal = curAvatar->socketIndex;
			
			sprintf(recordArray[validRecordNum].placeName,"Logging in...");
			sprintf(recordArray[validRecordNum].avatarName," ");
			recordArray[validRecordNum].health = 0;
			recordArray[validRecordNum].lifeTime = 0;
			recordArray[validRecordNum].maxHealth = 0;
			recordArray[validRecordNum].x = 0;
			recordArray[validRecordNum].y = 0;
			recordArray[validRecordNum].money    = 0;
			recordArray[validRecordNum].totalVal = 0;
			recordArray[validRecordNum].isPaid   = 0;

			if (curAvatar->hasPaid)
				recordArray[validRecordNum].isPaid    = 1;
			if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
				recordArray[validRecordNum].isPaid    = 2;
			if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
				recordArray[validRecordNum].isPaid    = 3;

			if (curAvatar->curCharacterIndex > -1)
			{

				long minutes;
				minutes = (curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime * 5);

				sprintf(recordArray[validRecordNum].avatarName, 
					       curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
				recordArray[validRecordNum].health = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
				recordArray[validRecordNum].maxHealth = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
				recordArray[validRecordNum].lifeTime = minutes;
				recordArray[validRecordNum].x = curAvatar->cellX;
				recordArray[validRecordNum].y = curAvatar->cellY;

				recordArray[validRecordNum].isInWorld = FALSE;

				recordArray[validRecordNum].money = 
					curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money / 1000;

				recordArray[validRecordNum].totalVal = curAvatar->InventoryValue();

				if (SPACE_DUNGEON == sp->WhatAmI())
					sprintf(recordArray[validRecordNum].placeName,((DungeonMap *) sp)->name);
				else if (SPACE_REALM == sp->WhatAmI())
					sprintf(recordArray[validRecordNum].placeName,sp->WhoAmI());
				else if (SPACE_LABYRINTH == sp->WhatAmI())
					sprintf(recordArray[validRecordNum].placeName, "Labyrinth");
				else if (SPACE_GUILD == sp->WhatAmI())
					sprintf(recordArray[validRecordNum].placeName,sp->WhoAmI());
				else
				{
					sprintf(recordArray[validRecordNum].placeName," ");
					recordArray[validRecordNum].isInWorld = TRUE;
				}
			}
			++validRecordNum;

			curAvatar = (BBOSAvatar *) sp->avatars->Next();
		}

		sp = (SharedSpace *) server->spaceList->Next();
	}

	SortRecords();
}

//***********************************************************************************
void DrawScreen(HWND hWnd, HDC hdc)
{
	char tempText[1024];

	RECT rt;
	GetClientRect(hWnd, &rt);

   HRGN clipRect = CreateRectRgn(rt.left, rt.top, rt.right, rt.bottom);
//   HRGN oldRect = (HRGN) SelectObject(hdc, clipRect);
	HBRUSH blackBrush = CreateSolidBrush(RGB(0,0,0));
 
	FillRgn(hdc, clipRect, blackBrush);

   COLORREF oldColor, curColor;
   HFONT infoFont, win32OldFont, headerFont, bigFont;
	COLORREF paidColor  = RGB(0, 255, 128);
	COLORREF adminColor = RGB(255, 0, 255);
	COLORREF modColor   = RGB(255, 255, 0);
	
	SetBkMode( hdc, TRANSPARENT);
  
	infoFont   = CreateFont( PLAYER_TEXT_HEIGHT,
    					    0,0,0, 0,
    					    0,0,0,0,OUT_TT_ONLY_PRECIS ,0,0,0, "Helvetica");
	headerFont = CreateFont( 36,
    					    0,0,0, 0,
    					    0,0,0,0,OUT_TT_ONLY_PRECIS ,0,0,0, "Helvetica");
	bigFont = CreateFont( 56,
    					    0,0,0, 0,
    					    0,0,0,0,OUT_TT_ONLY_PRECIS ,0,0,0, "Helvetica");



   win32OldFont = (HFONT)SelectObject( hdc, headerFont); 

	RECT tempRect;

	// draw header text
	curColor = RGB(155, 255, 155);
	oldColor = SetTextColor(hdc, curColor); 

	tempRect.left = 0;
	tempRect.top = 0;
	tempRect.right = 1000;
	tempRect.bottom = 50;
	DrawText(hdc, headerText, strlen(headerText), &tempRect, DT_LEFT);

	// draw number of players
	curColor = RGB(255, 200, 200);
	SetTextColor(hdc, curColor); 
   SelectObject( hdc, bigFont); 

	tempRect.left = rt.right - 60;
	tempRect.top = 0;
	tempRect.right = rt.right;
	tempRect.bottom = 100;
	sprintf(tempText,"%d", validRecordNum);
	DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);

	curColor = RGB(255, 255, 255);
	SetTextColor(hdc, curColor); 

   SelectObject( hdc, infoFont); 

/*
XX	sprintf(&(headerText[strlen(headerText)]),
		"    TOTAL PLAYERS   %d ", totalPlayers);
	sprintf(&(headerText[strlen(headerText)]),
		"TOTAL CONNECTIONS   %d (%d)", 
		       ((IOCPServer *)(server->lserver))->curConnections(),
		       ((IOCPServer *)(server->lserver))->curSockets());
*/	

	// draw player info
	curColor = RGB(155, 155, 255);
	SetTextColor(hdc, curColor); 

	tempRect.left   = PLAYER_INFO_LEFT;
	tempRect.top    = PLAYER_INFO_TOP + PLAYER_TEXT_HEIGHT * 0;
	tempRect.right  = PLAYER_INFO_LEFT + 550;
	tempRect.bottom = PLAYER_INFO_TOP + PLAYER_TEXT_HEIGHT * 0 + PLAYER_TEXT_HEIGHT;
	sprintf(tempText,"#     User                        Avatar                    Money    Wealth       Pos         Life         Area");
	DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);

	curColor = RGB(255, 255, 255);
	SetTextColor(hdc, curColor); 

	for (int i = 0; i < 35; ++i)
	{
		int index = i + recordStartIndex;
		if (index < validRecordNum)
		{
			tempRect.left   = PLAYER_INFO_LEFT;
			tempRect.top    = PLAYER_INFO_TOP + PLAYER_TEXT_HEIGHT * (i+1);
			tempRect.right  = PLAYER_INFO_LEFT + 90;
			tempRect.bottom = PLAYER_INFO_TOP + PLAYER_TEXT_HEIGHT * (i+1) + PLAYER_TEXT_HEIGHT;

			if (!(index % 5))
			{
				sprintf(tempText,"%d",index);
				DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);
			}
			tempRect.left   += 20;
			tempRect.right  += 20;

			SetTextColor(hdc, infoColor[0]); 
			if (1 == recordArray[index].isPaid)
				SetTextColor(hdc, paidColor); 
			if (2 == recordArray[index].isPaid)
				SetTextColor(hdc, adminColor); 
			if (3 == recordArray[index].isPaid)
				SetTextColor(hdc, modColor); 

			DrawText(hdc, recordArray[index].userName, 
				      strlen(recordArray[index].userName), &tempRect, DT_LEFT);
			tempRect.left   += 95;
			tempRect.right  += 95;

			SetTextColor(hdc, infoColor[1]); 
			DrawText(hdc, recordArray[index].avatarName, 
				      strlen(recordArray[index].avatarName), &tempRect, DT_LEFT);
			tempRect.left   += 95;
			tempRect.right  += 95;

			SetTextColor(hdc, infoColor[2]); 
			sprintf(tempText,"%ldk",recordArray[index].money);
			DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);
			tempRect.left   += 45;
			tempRect.right  += 45;

			SetTextColor(hdc, infoColor[3]); 
			sprintf(tempText,"%ldk",recordArray[index].totalVal);
			DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);
			tempRect.left   += 45;
			tempRect.right  += 45;

			SetTextColor(hdc, infoColor[4]); 
			sprintf(tempText,"%dx%d",recordArray[index].x, recordArray[index].y);
			DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);
			tempRect.left   += 50;
			tempRect.right  += 50;

			SetTextColor(hdc, infoColor[5]); 
			sprintf(tempText,"%ld",recordArray[index].lifeTime);
			DrawText(hdc, tempText, strlen(tempText), &tempRect, DT_LEFT);
			tempRect.left   += 40;
			tempRect.right  += 40;

			SetTextColor(hdc, infoColor[6]); 
			DrawText(hdc, recordArray[index].placeName, 
				      strlen(recordArray[index].placeName), &tempRect, DT_LEFT);
			tempRect.left   += 50;
			tempRect.right  += 50;
		}
	}

	// make map
   HRGN mapRect = CreateRectRgn(rt.right-256, rt.top+100, rt.right, rt.top+100+256);
//   SelectObject(hdc, clipRect);
	HBRUSH mapBackBrush = CreateSolidBrush(RGB(0,40,0));
	FillRgn(hdc, mapRect, mapBackBrush);

   HPEN greenPen   = CreatePen(PS_SOLID, 1, RGB(0,255,0));
   HPEN bluePen  = CreatePen(PS_SOLID, 1, RGB(0,0,255));
   HPEN redPen     = CreatePen(PS_SOLID, 1, RGB(255,0,0));
   HPEN dullGreenPen   = CreatePen(PS_SOLID, 1, RGB(0,128,0));
   HPEN dullbluePen  = CreatePen(PS_SOLID, 1, RGB(0,0,128));
   HPEN dullRedPen     = CreatePen(PS_SOLID, 1, RGB(128,0,0));
   HPEN oldPen;

   oldPen = (HPEN)SelectObject(hdc, redPen);

	// draw player dots
   SelectObject(hdc, greenPen);
	for (int i = 0; i < validRecordNum; ++i)
	{
      MoveToEx(hdc, rt.right-256 + recordArray[i].x/2*2, 
			      rt.top+100 + recordArray[i].y/2*2, NULL);
      LineTo  (hdc, rt.right-256 + recordArray[i].x/2*2, 
			      rt.top+100 + recordArray[i].y/2*2+1);
	}

	// draw town dots
   SelectObject(hdc, bluePen);
	for (int i = 0; i < 24; ++i)
	{
      MoveToEx(hdc, rt.right-256 + townList[i].x/2*2, 
			      rt.top+100 + townList[i].y/2*2-1, NULL);
      LineTo  (hdc, rt.right-256 + townList[i].x/2*2, 
			      rt.top+100 + townList[i].y/2*2);
	}

	// draw monster dots
   SelectObject(hdc, redPen);
	SharedSpace *sp = (SharedSpace *) server->spaceList->First();
	while (sp)
	{
		if (SPACE_GROUND == sp->WhatAmI())
		{
			BBOSMob *curMob = (BBOSMob *) sp->mobList->GetFirst(0,0,1000);
			while (curMob)
			{
				if (SMOB_MONSTER == curMob->WhatAmI())
				{
				   MoveToEx(hdc, rt.right-256 + curMob->cellX/2*2-1, 
							   rt.top+100 + curMob->cellY/2*2-1, NULL);
			      LineTo  (hdc, rt.right-256 + curMob->cellX/2*2, 
							   rt.top+100 + curMob->cellY/2*2);
				}

				curMob = (BBOSMob *) sp->mobList->GetNext();
			}
		}
		sp = (SharedSpace *) server->spaceList->Next();
	}

	// draw graph

   SelectObject(hdc, dullGreenPen);
	MoveToEx(hdc, rt.right-10-300,rt.top+360+100, NULL);
	LineTo  (hdc, rt.right-10    ,rt.top+360+100);
	MoveToEx(hdc, rt.right-10    ,rt.top+360    , NULL);
	LineTo  (hdc, rt.right-10    ,rt.top+360+100);

	for (int i = 0; i < 300-1; i += 24)
	{
		MoveToEx(hdc, rt.right-10-i    ,rt.top+360+30    , NULL);
		LineTo  (hdc, rt.right-10-i    ,rt.top+360+70);
	}

   SelectObject(hdc, greenPen);
	int curCount = historyCounter;
	for (int i = 0; i < 300-1; ++i)
	{
		int lastCount = curCount-1;
		if (lastCount < 0)
			lastCount = 299;

		int y1 = playerNumHistoryList[curCount];
		if (y1 < 0 || y1 > 300)
			y1 = 0;
		y1 = rt.top+360+100-y1;
		int y2 = playerNumHistoryList[lastCount];
		if (y2 < 0 || y2 > 300)
			y2 = 0;
		y2 = rt.top+360+100-y2;

		MoveToEx(hdc, rt.right-10-i   ,y1, NULL);
		LineTo  (hdc, rt.right-10-i-1 ,y2);

		--curCount;
		if (curCount < 0)
			curCount = 299;

	}

   SelectObject(hdc, redPen);
	curCount = historyCounter;
	for (int i = 0; i < 300-1; ++i)
	{
		int lastCount = curCount-1;
		if (lastCount < 0)
			lastCount = 299;

		int y1 = cyclesHistoryList[curCount];
		if (y1 < 0)
			y1 = 0;
		if (y1 > 100)
			y1 = 100;
		y1 = rt.top+360+100-y1;
		int y2 = cyclesHistoryList[lastCount];
		if (y2 < 0)
			y2 = 0;
		if (y2 > 100)
			y2 = 100;
		y2 = rt.top+360+100-y2;

		MoveToEx(hdc, rt.right-10-i   ,y1, NULL);
		LineTo  (hdc, rt.right-10-i-1 ,y2);

		--curCount;
		if (curCount < 0)
			curCount = 299;

	}



	// reset GDI objects
//   SelectObject(hdc, oldRect);
   SelectObject( hdc, win32OldFont); 
	SetTextColor(hdc, oldColor); 
   SelectObject(hdc, oldPen);

	// delete GDI objects
   DeleteObject(dullRedPen);
   DeleteObject(dullbluePen);
   DeleteObject(dullGreenPen);
   DeleteObject(redPen);
   DeleteObject(bluePen);
   DeleteObject(greenPen);
	DeleteObject(mapRect);
	DeleteObject(clipRect);
   DeleteObject(mapBackBrush);
   DeleteObject(blackBrush);
   DeleteObject(infoFont);
   DeleteObject(headerFont);
   DeleteObject(bigFont);

}


//***********************************************************************************
void SetInfoColors(int type)
{
	for (int i = 0; i < 7; ++i)
		infoColor[i] = RGB(255, 255, 255);

	switch(type)
	{
	case 0:
	case 1:
	default:
		infoColor[0] = RGB(200, 255, 200);
		break;

	case 2:
	case 3:
		infoColor[1] = RGB(200, 255, 200);
		break;

	case 4:
	case 5:
		infoColor[2] = RGB(200, 255, 200);
		break;

	case 6:
	case 7:
		infoColor[3] = RGB(200, 255, 200);
		break;

	case 8:
	case 9:
	case 10:
	case 11:
		infoColor[4] = RGB(200, 255, 200);
		break;

	case 12:
	case 13:
		infoColor[5] = RGB(200, 255, 200);
		break;

	case 14:
	case 15:
		infoColor[6] = RGB(200, 255, 200);
		break;

	}

}

//***********************************************************************************
int ShouldSwapRecords(int index, int type)
{
	if (index > NUM_OF_PLAYER_RECORDS-2)
		return FALSE;

	playerRecord *first, *second;
	first  = &recordArray[index];
	second = &recordArray[index+1];

	switch(type)
	{
	case 0:
	default:
		if (stricmp(first->userName, second->userName) > 0)
			return TRUE;
		break;

	case 1:
		if (stricmp(first->userName, second->userName) < 0)
			return TRUE;
		break;

	case 2:
		if (stricmp(first->avatarName, second->avatarName) > 0)
			return TRUE;
		break;

	case 3:
		if (stricmp(first->avatarName, second->avatarName) < 0)
			return TRUE;
		break;

	case 4:
		if (first->money > second->money)
			return TRUE;
		break;

	case 5:
		if (first->money < second->money)
			return TRUE;
		break;

	case 6:
		if (first->totalVal > second->totalVal)
			return TRUE;
		break;

	case 7:
		if (first->totalVal < second->totalVal)
			return TRUE;
		break;

	case 8:
		if (first->x > second->x)
			return TRUE;
		break;

	case 9:
		if (first->x < second->x)
			return TRUE;
		break;

	case 10:
		if (first->y > second->y)
			return TRUE;
		break;

	case 11:
		if (first->y < second->y)
			return TRUE;
		break;

	case 12:
		if (first->lifeTime > second->lifeTime)
			return TRUE;
		break;

	case 13:
		if (first->lifeTime < second->lifeTime)
			return TRUE;
		break;

	case 14:
		if (stricmp(first->placeName, second->placeName) > 0)
			return TRUE;
		break;

	case 15:
		if (stricmp(first->placeName, second->placeName) < 0)
			return TRUE;
		break;

	}

	return FALSE;
}

//***********************************************************************************
void SortRecords(void)
{
	if (sortType < 0)
		sortType += 16 * 1000;

	int type = sortType % 16;
	playerRecord *first, *second, temp;

	SetInfoColors(type);

	for (int i = 0; i < validRecordNum-1; ++i)
	{
		for (int j = 0; j < validRecordNum-1; ++j)
		{
			if (ShouldSwapRecords(j, type))
			{
				first  = &recordArray[j];
				second = &recordArray[j+1];

				temp    = *first;
				*first  = *second;
				*second = temp;
			}
		}
	}

//int sortType = 0;
//COLORREF infoColor[7];


}

/* end of file */
