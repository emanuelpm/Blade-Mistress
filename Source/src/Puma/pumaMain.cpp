//***************************************************************************************
// Main file designed to be the logical main for using PUMA.
//***************************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <crtdbg.h>

#include "puma.h"
#include "pumamesh.h"
#include "../loginmode.h"
#include "../helper/GeneralUtils.h"
#include "d3dutil.h"

#include "UIR_TextButton.h"
#include "UIR_TextBox.h"
#include "UIR_ScrollWindow.h"
#include "UIR_DragEdge.h"
#include "UIR_EditLine.h"

#include "..\network\NetWorldMessages.h"

#include "..\BBOServer.h"
#include "..\network\client.h"

//#include "loginmode.h"
#include "..\firstOptionMode.h"

#include "..\helper\autolog.h"

#include "..\memory_helper.h"





// The following macros set and clear, respectively, given bits
// of the C runtime library debug flag, as specified by a bitmask.
#ifdef   _DEBUG
#define  SET_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag((a) | _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#define  CLEAR_CRT_DEBUG_FIELD(a) \
            _CrtSetDbgFlag(~(a) & _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG))
#else
#define  SET_CRT_DEBUG_FIELD(a)   ((void) 0)
#define  CLEAR_CRT_DEBUG_FIELD(a) ((void) 0)
#endif







extern char nameBuffer[128], passBuffer[128];



BOOL bActive, flippedOut;
unsigned int lastTimeChecked;

int gScreenW = 640;
int gScreenH = 480;
//int gScreenW = 1024;
//int gScreenH = 768;


BBOServer *server = NULL;
Client *	lclient;

char ipAddress[128];
int createServerFlag = FALSE;


// =============================================
// This function gets called when any of the
// server managed sockets have an event get 
// signalled.
// =============================================

int GlobalUDPNetCallback(	UDPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
                            int size, void * context)
{
        // varaibles.
    Server *	server		= NULL;
    Client *	client		= NULL;
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

    if((bytesParsed == 0) && (client != NULL))
    {
        bytesParsed = client->UDPNetCallback(socket, events, buffer, size, client);
    }

    if(bytesParsed == 0)
    {
        bytesParsed = size;
    }

    return bytesParsed;
}

//***************************************************************************************
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
   switch( msg )
   {
      case WM_DESTROY:
         PostQuitMessage( 0 );
         return 0;

       case WM_KEYDOWN:
//			if (123 == wParam)
//	         PostQuitMessage( 0 );
            break;

       case WM_ACTIVATE:
            bActive = LOWORD(wParam);
            if (bActive != WA_INACTIVE)
            {
//				SetTimer( puma->m_hWnd, TIMER_ID, TIMER_RATE, NULL );
                flippedOut = FALSE;
            }
            else
            {
                flippedOut = TRUE;
//		  		KillTimer( puma->m_hWnd , TIMER_ID);
            }

        break;
        case WM_MOUSEMOVE:
            if (puma->m_bWindowed)
            {
                puma->mouseX = LOWORD(lParam);  // horizontal position of cursor  
                puma->mouseY = HIWORD(lParam);  // vertical position of cursor    
                if (puma->mouseX < 0)
                    puma->mouseX = 0;
                if (puma->mouseX >= puma->ScreenW())
                    puma->mouseX = puma->ScreenW()-1;
                
                if (puma->mouseY < 0)
                    puma->mouseY = 0;
                if (puma->mouseY >= puma->ScreenH())
                    puma->mouseY = puma->ScreenH()-1;
            }
            break;
   }

   return DefWindowProc( hWnd, msg, wParam, lParam );
}


int windowMode = FALSE;

//********************************************************************************
void ParseCommandLine(char * commandLine)
{
    unsigned int linePoint = 0; //, argPoint;

    while (linePoint < strlen(commandLine) && commandLine[linePoint])
    {
        if (!strnicmp( &(commandLine[linePoint]) , "-" , 1) ||
            !strnicmp( &(commandLine[linePoint]) , "/" , 1) )
        {
            linePoint += 1;
            
            if (!strnicmp( &(commandLine[linePoint]) , "W" , 1))
            {
                // draw in window
            windowMode = TRUE;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "X" , 1))
            {
                // draw in window
                gScreenW = 800;
                gScreenH = 600;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "Z" , 1))
            {
                // draw in window
                gScreenW = 1024;
                gScreenH = 768;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "Q" , 1))
            {
                // draw in window
                gScreenW = 1600;
                gScreenH = 1200;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "P" , 1))
            {
                // password on commandline

                sscanf_s(&commandLine[linePoint+1], "%s", passBuffer, _countof(passBuffer));

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "N" , 1))
            {
                // password on commandline

                sscanf_s(&commandLine[linePoint+1], "%s", nameBuffer, _countof(nameBuffer));

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else if (!strnicmp( &(commandLine[linePoint]) , "S" , 1))
            {
                // turn on server
                createServerFlag = TRUE;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
            else
            {
                // copy address
//				linePoint++;
                int start = linePoint;

                while (	!isspace(commandLine[linePoint]) && commandLine[linePoint])
                {
                    linePoint++;
                }
                sprintf(ipAddress, &commandLine[start]);
                ipAddress[linePoint-start] = 0;

                while (	isspace(commandLine[linePoint]) && linePoint < strlen(commandLine) &&
                        commandLine[linePoint])
                {
                    linePoint++;
                }
            }
        }
        else
            linePoint++;
    }
}

//********************************************************************************
int MaintainDeviceIntegrity(HWND hWnd)
{
   HRESULT hr;
//	D3DPRESENT_PARAMETERS   d3dpp;

//	return S_OK;

//	aLog.Log("MaintainDeviceIntegrity Start**\n");

    // Test the cooperative level to see if it's okay to render
    if( FAILED( hr = puma->m_pd3dDevice->TestCooperativeLevel() ) )
    {
       // If the device was lost, do not render until we get it back
       if( D3DERR_DEVICELOST == hr )
            return D3DERR_DEVICELOST;

       // Check if the device needs to be resized.
       if( D3DERR_DEVICENOTRESET == hr )
       {
//			vdMan->EraseAll();

            for (int i = 0; i < NUM_OF_PUMA_FONTS; i++)
            {
                if (puma->fonts[i].dxfont)
                    puma->fonts[i].dxfont->InvalidateDeviceObjects();
            }


           // Resize the device
           if( SUCCEEDED( puma->m_pd3dDevice->Reset( &puma->m_d3dpp ) ) )
            {
               // Initialize the app's device-dependent objects

                aLog.Log("MaintainDeviceIntegrity **\n");

                
                for (int i = 0; i < NUM_OF_PUMA_FONTS; i++)
                {
                    if (puma->fonts[i].dxfont)
                        puma->fonts[i].dxfont->RestoreDeviceObjects();
                }
                
                puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
                puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

                if (currentGameMode)
                    currentGameMode->SetEnvironment();

//				vdMan->RecoverAll();
               return S_OK;
            }

           PostMessage( hWnd, WM_CLOSE, 0, 0 );
       }
       return hr;
    }

   return S_OK;
}

//***************************************************************************************
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
    
   SET_CRT_DEBUG_FIELD( _CRTDBG_LEAK_CHECK_DF );

    aLog.Start();

    sprintf(ipAddress, "blademistress.dyndns.org");
    nameBuffer[0] = passBuffer[0] = 0;

    ParseCommandLine(lpCmdLine);

    puma = new CPuma();
   puma->m_dwCreationWidth   = gScreenW;
   puma->m_dwCreationHeight  = gScreenH;
    puma->Create(hInst, windowMode);

//	if (!windowMode)
//		puma->ToggleFullscreen();
/*
   D3DAdapterInfo* pAdapterInfo = &(puma->m_Adapters[puma->m_dwAdapter]);
   D3DDeviceInfo*  pDevice  = &(pAdapterInfo->devices[pAdapterInfo->dwCurrentDevice]);

   for( int m=0; m<pDevice->dwNumModes; m++ )
   {
       if( pDevice->modes[m].Width==800 && pDevice->modes[m].Height==600 )
       {
           pDevice->dwCurrentMode = m;
           if( pDevice->modes[m].Format == D3DFMT_R5G6B5 ||
               pDevice->modes[m].Format == D3DFMT_X1R5G5B5 ||
               pDevice->modes[m].Format == D3DFMT_A1R5G5B5 )
           {
               break;
           }
       }
   }

    puma->Resize3DEnvironment();
  */
   if( puma->GetStarted( hInst, puma->m_hWnd, windowMode) )
   {
        puma->pumaCamera->m_fFarPlane   = 300.0f;
        puma->pumaCamera->Calculate();
        puma->pumaCamera->AssertView(puma->m_pd3dDevice);

        SetWindowText(puma->m_hWnd, "Blade Mistress");

      // Show the window
      ShowWindow( puma->m_hWnd, nCmdShow );
      UpdateWindow( puma->m_hWnd );
        SetFocus(puma->m_hWnd);

//		if (windowMode)
//			MoveWindow( puma->m_hWnd,300,200,gScreenW, gScreenH, TRUE);
        // add fonts here
//		puma->CreateFont(0, "Trebuchet MS", 7);
        puma->CreateFont(0, "Verdana", 7);
        puma->CreateFont(1, "Verdana", 14);
        puma->CreateFont(2, "Verdana", 10);

        // add first mode here!

        currentGameMode = new FirstOptionMode(0,"FIRST_OPTION_MODE");
//		currentGameMode = new GeneratorMode(0,"GENERATOR_MODE");
        gameModeStack.Push(currentGameMode);
        currentGameMode->Activate();


      // Enter the message loop
      MSG msg;
      ZeroMemory( &msg, sizeof(msg) );
      while( msg.message!=WM_QUIT )
      {
         if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
         {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
         }
            else if (currentGameMode)
         {
            if (lastTimeChecked < timeGetTime() - (1000/40))
//            if (lastTimeChecked < timeGetTime() - (1000/130))
            {
//					aLog.Log("Main 1 **\n");

               lastTimeChecked = timeGetTime();
                   if (server)
                       server->Tick();

//					aLog.Log("Main 2 **\n");

                    int deviceStatus = MaintainDeviceIntegrity(puma->m_hWnd);
//					aLog.Log("Main 3 **\n");

//               if (!flippedOut)
               if (!flippedOut && S_OK == deviceStatus)
               {
//						aLog.Log("Main 4 **\n");

                        // call Tick() for the top game mode,
                  currentGameMode->Tick();

//						aLog.Log("Main 5 **\n");

                        // and see what the result is.
                        switch (currentGameMode->retState)
                        {
                            case GMR_POP_ME:
                            gameModeStack.Pop();
                            currentGameMode->Deactivate();
                            delete currentGameMode;
                            if (newGameMode)
                            {
                                gameModeStack.Push(newGameMode);
                            }								   
                            if (!gameModeStack.Last())
                            {
                                MessageBox( puma->m_hWnd, "No game mode on stack; expected one!", "Game Failure", MB_OK );
                                DestroyWindow( puma->m_hWnd );
                            }
                            currentGameMode = (GameMode *) gameModeStack.Last();
                            if (currentGameMode)
                            {
                                currentGameMode->Activate();
                                currentGameMode->retState = GMR_NORMAL;
                            }
                            break;

                            case GMR_NEW_MODE:
                            currentGameMode->Deactivate();
                            if (newGameMode)
                            {
                                gameModeStack.Push(newGameMode);
                            }
                            else
                            {
                                 MessageBox( puma->m_hWnd, "No new game mode; expected one!", "Game Failure", MB_OK );
                                  DestroyWindow( puma->m_hWnd );
                            }
                            currentGameMode = (GameMode *) gameModeStack.Last();
                            if (currentGameMode)
                            {
                                currentGameMode->Activate();
                                currentGameMode->retState = GMR_NORMAL;
                            }
                            break;
                        }
               }
//					aLog.Log("Main 6 **\n");

            }    
         }
        }
    
   }

//	aLog.Log("Main 7 **\n");

   // Clean up everything and exit the app
    currentGameMode = (GameMode *) gameModeStack.Last();
    if (currentGameMode)
        currentGameMode->Deactivate();

    while (currentGameMode)
    {
        gameModeStack.Pop();
        delete currentGameMode;
        currentGameMode = (GameMode *) gameModeStack.Last();
    }

    puma->ShutDown();
    delete puma;
//   UnregisterClass( "Puma Main 1.0", wc.hInstance );

    ShowCursor(TRUE);


//   _CrtDumpMemoryLeaks( );


#ifdef _DETECT_LEAKS
    DumpUnfreed();
#endif

    
   return 0;
}


//*******************************************************************************
void DebugOutput(char *text)
{
    FILE *fp = fopen("debug.txt","a");
    fprintf(fp,text);
    fclose(fp);
}

/* end of file */
