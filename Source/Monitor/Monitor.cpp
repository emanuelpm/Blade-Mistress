// Monitor.cpp : Defines the entry point for the application.
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

#include "client.h"
#include "NetWorldMessages.h"
#include "version.h"

#define MAX_LOADSTRING 100

Client *client = NULL;

char screenText[2048];


// =============================================
// This function gets called when any of the
// client managed sockets have an event get 
// signalled.
// =============================================

//***********************************************************************************
int GlobalUDPNetCallback(	UDPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
							int size, void * context)
{
		// varaibles.
	Client *	client		= NULL;
//	Client *	client		= NULL;
	int			bytesParsed = 0;
/*
	if(clientWorld)
	{
		client	= (Client *)clientWorld->radio;
	}
		
	if(clientWorld1)
	{
		client	= (Client *)clientWorld1->radio;
	}
*/

		// try the client first.
	if(client != NULL)
	{
		bytesParsed = client->UDPNetCallback(socket, events, buffer, size, client);
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

//***************************************************************
int HandleMessages(void)
{
	char messData[4000];
	int  dataSize;
	int found;

	found = 0;

	std::vector<TagID> tempReceiptList;
	int					fromSocket = 0;

	client->GetNextMsg(NULL, dataSize);
	
	while (dataSize > 0)
	{
		if (dataSize > 4000)
			dataSize = 4000;
		
		client->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);

		++found;
		
		client->GetNextMsg(NULL, dataSize);
	}

	return found;

}

//***************************************************************
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	MessTestPing testPing;
//	MessPlayerReturning messPlayerReturning;

	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MONITOR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	client = new Client();

	DWORD lastTime = 0, beepTime = 0;
	int cyclesCompleted = 0;
	int trying = FALSE;
	int failCount = 0;
	int sendMessageCount = -1;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MONITOR);

      // Enter the message loop
//      MSG msg;
   ZeroMemory( &msg, sizeof(msg) );
   while( msg.message!=WM_QUIT )
   {
		Sleep(200);

      if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
      {
         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
		else
      {
			if (trying && HandleMessages() > 0)
			{
				trying = FALSE;
				client->disconnect();
				failCount = 0;
			}
			DWORD thisTime = timeGetTime();

			if (lastTime < thisTime - 1000 * 20) // every 20 seconds
			{
				int notOkay;
				notOkay = trying;
				// check status here
				client->disconnect();
				client->connect("aggressive.dyn.dhs.org", 3678);
				sendMessageCount = 0;

//				Sleep(2000); // 2 seconds

				trying = TRUE;

				lastTime = thisTime;
				// re-write screenText
				sprintf(screenText,"        Blade Mistress status monitor V%f\n\n\n", VERSION_NUMBER);

				if (notOkay)
				{
					sprintf(&(screenText[strlen(screenText)]), "******* NO CONNECTION ********");
//					Beep(500,200);
					SetWindowText(hWnd, "Mon - NO CONNECTION");
					++failCount;
				}
				else
				{
					sprintf(&(screenText[strlen(screenText)]), "connected okay");
					SetWindowText(hWnd, "Mon - okay");
				}
				cyclesCompleted = 0;

				// tell screen to re-draw
				InvalidateRect(hWnd, NULL, TRUE); 
			}

			if (beepTime < thisTime - 1000 * 2) // every 2 seconds
			{
				beepTime = thisTime;
				if (failCount > 3)
					Beep(500,200);

				if (sendMessageCount > -1)
				{
					++sendMessageCount;
					if (sendMessageCount > 1)
					{
						client->SendMsg(sizeof(testPing),(void *)&testPing);
						sendMessageCount = -1;
					}
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
	}
	*/
	client->disconnect();
	delete client;
	WSACleanup();

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
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MONITOR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCSTR)IDC_MONITOR;
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
//   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 380, 110, NULL, NULL, hInstance, NULL);

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
			RECT rt;
			GetClientRect(hWnd, &rt);
			DrawText(hdc, screenText, strlen(screenText), &rt, DT_LEFT);
			EndPaint(hWnd, &ps);
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
   }
   return 0;
}

// Mesage handler for about box.
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

