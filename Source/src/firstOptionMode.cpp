
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wininet.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "firstOptionmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_EditLine.h"
#include "./puma/UIR_DragBar.h"

#include "./network/NetWorldMessages.h"

#include "BBOServer.h"
#include "./network/client.h"
#include "clientOnly.h"

#include "SimpleMessageMode.h"
#include "PickCharMode.h"
#include "legalMode.h"
#include "startNewCharMode.h"
#include "particle2.h"

#include "version.h"
#include ".\helper\crypto.h"

#include ".\helper\sendMail.h"

#include "./helper/PasswordHash.h"

enum 
{
    FOM_BUTTON_LOGIN,
    FOM_BUTTON_NAME,
    FOM_BUTTON_PASS,
    FOM_BUTTON_QUIT,
    FOM_BUTTON_SERVERS,
    FOM_BUTTON_TEXT,
    FOM_BUTTON_TEMP,
    FOM_BUTTON_SERVER_0
};

extern BBOServer *server;
extern Client *	lclient;
extern char ipAddress[128];
extern int createServerFlag;

PumaMesh *wall = NULL;
PumaMesh *rug = NULL;
PumaMesh *candle = NULL;
PumaMesh *cstick = NULL;
PumaMesh *back = NULL;
PumaMesh *door = NULL;
PumaMesh *drapes = NULL;
PumaMesh *arms = NULL;
Particle2Type   *candleParticle = NULL;
DWORD lastCandleParticleTick;
float candleAttenuation;
int candleAttTick;

char nameBuffer[NUM_OF_CHARS_FOR_USERNAME], passBuffer[NUM_OF_CHARS_FOR_PASSWORD];
int cmdBypass = FALSE;

char *fomText1 = "Connecting...";
char *fomText2 = "Sending hello...";


PumaSound *button1Sound;
PumaSound *button2Sound;

PumaSound *songSound;

// static pointer that the process function can use
FirstOptionMode *curFirstOptionMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL FirstOptionModeProcess(UIRect *curUIRect, int type, long x, short y)
{
    UIRectEditLine *edName, *edPass;
    MessPlayerReturning messPlayerReturning;

//	char tempText[128];
//	int hIndex;
//	float fval;
//	long val;  

    switch(curUIRect->WhatAmI())	
    {
    case FOM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
          PostQuitMessage( 0 );
            return 1;
      }
        break;
    case FOM_BUTTON_PASS :
      if (UIRECT_DONE == type)
      {
            button2Sound->PlayNo3D();
            edName = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_NAME);
            edPass = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_PASS);
            sprintf_s(nameBuffer, NUM_OF_CHARS_FOR_USERNAME, "%s", edName->text);
            sprintf_s(passBuffer, NUM_OF_CHARS_FOR_PASSWORD, "%s", edPass->text);
            if (nameBuffer[0] && passBuffer[0])
            {
                curFirstOptionMode->chooseServer = TRUE;
            }
            return 1;
      }
        break;
    case FOM_BUTTON_NAME :
      if (UIRECT_DONE == type)
      {
            button2Sound->PlayNo3D();
            edName = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_NAME);
            edPass = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_PASS);
            sprintf_s(nameBuffer, NUM_OF_CHARS_FOR_USERNAME, "%s", edName->text);
            sprintf_s(passBuffer, NUM_OF_CHARS_FOR_PASSWORD, "%s", edPass->text);
            if (nameBuffer[0] && passBuffer[0])
            {
                curFirstOptionMode->chooseServer = TRUE;
            }
            return 1;
      }
        break;
    case FOM_BUTTON_SERVER_0 :
      if (UIRECT_MOUSE_LUP == type && !curFirstOptionMode->tryingConnection)
      {
            button1Sound->PlayNo3D();

            if (curFirstOptionMode->isConnected)
            {
                lclient->disconnect();
                curFirstOptionMode->isConnected = FALSE;
                curFirstOptionMode->tryingConnection = FALSE;
                curFirstOptionMode->messSent = FALSE;
            }
#ifdef _TEST_SERVER
            if (!lclient->connect(ipAddress, 9178))
#else
            if (!lclient->connect(ipAddress, 3678))
#endif
            {
                newGameMode = new SimpleMessageMode("The server is not responding.  It may be temporarily stopped,\n or your local network could have a firewall that blocks access.\nPlease check if your network blocks access, and try again later.",0,"SIMPLE_MESSAGE_MODE");
                curFirstOptionMode->retState = GMR_NEW_MODE;
                break;
            }
            curFirstOptionMode->isConnected = TRUE;
            if (!curFirstOptionMode->tryingConnection)
            {
               UIRectTextBox *tBox = (UIRectTextBox *) curFirstOptionMode->tempWindow->childRectList.Find(FOM_BUTTON_TEMP);
                tBox->SetText(fomText1);

                curFirstOptionMode->tryingConnection = TRUE;
                curFirstOptionMode->messSent = FALSE;
                curFirstOptionMode->connectTime = timeGetTime();
            }


//			newGameMode = new FirstOptionMode(0,"LOGIN_MODE");
//			curFirstOptionMode->retState = GMR_NEW_MODE;
            /*
            edName = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_NAME);
            edPass = (UIRectEditLine *) curFirstOptionMode->fullWindow->childRectList.Find(FOM_BUTTON_PASS);
            if (edName->text[0] && edPass->text[0])
            {
                CorrectString(edName->text);
                CorrectString(edPass->text);
                sprintf(messPlayerReturning.name,edName->text);
                sprintf(messPlayerReturning.pass,edPass->text);
                messPlayerReturning.version = VERSION_NUMBER;
                lclient->SendMsg(sizeof(messPlayerReturning),(void *)&messPlayerReturning);
            }
              */
      }
        break;
    }
    return 0;  // didn't deal with this message
}

VideoDataObject *pleaseWaitArt = NULL;



//******************************************************************
FirstOptionMode::FirstOptionMode(int doid, char *doname) : GameMode(doid,doname)
{

    if (nameBuffer[0] && passBuffer[0])
        cmdBypass = TRUE;


    curFirstOptionMode = this;
    isConnected = FALSE;
    tryingConnection = FALSE;
    chooseServer = FALSE;
    messSent = FALSE;

    WSAData wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (createServerFlag)
        server = new BBOServer(FALSE);

    lclient = new Client();

    wall = new PumaMesh();
//	wall->LoadFromASC(puma->m_pd3dDevice,"dat\\NEWall.ASE");
//	wall->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	wall->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-wall.MEC");
    wall->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-wall.MEC");
    wall->LoadTexture(puma->m_pd3dDevice,	"dat\\wall.png");
    
    rug = new PumaMesh();
//	rug->LoadFromASC(puma->m_pd3dDevice,"dat\\NEFloor.ASE");
//	rug->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	rug->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-rug.MEC");
    rug->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-rug.MEC");
    rug->LoadTexture(puma->m_pd3dDevice,	"dat\\fr-rug.png");
    
    cstick = new PumaMesh();
//	cstick->LoadFromASC(puma->m_pd3dDevice,"dat\\NEStand.ASE");
//	cstick->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	cstick->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-cstick.MEC");
    cstick->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-cstick.MEC");
    cstick->LoadTexture(puma->m_pd3dDevice,	"dat\\gold.png");
    
    candle = new PumaMesh();
//	candle->LoadFromASC(puma->m_pd3dDevice,"dat\\NECandle.ASE");
//	candle->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	candle->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-candle.MEC");
    candle->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-candle.MEC");
    candle->LoadTexture(puma->m_pd3dDevice,	"dat\\candle.png");
    
    back = new PumaMesh();
//	back->LoadFromASC(puma->m_pd3dDevice,"dat\\NEBack.ASE");
//	back->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	back->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-back.MEC");
    back->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-back.MEC");
    back->LoadTexture(puma->m_pd3dDevice,	"dat\\fr-back.png");
    
    drapes = new PumaMesh();
//	drapes->LoadFromASC(puma->m_pd3dDevice,"dat\\NECurtain.ASE");
//	drapes->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	drapes->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-drapes.MEC");
    drapes->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-drapes.MEC");
    drapes->LoadTexture(puma->m_pd3dDevice,	"dat\\curtain.png");
    
    door = new PumaMesh();
//	door->LoadFromASC(puma->m_pd3dDevice,"dat\\NEEntrance.ASE");
//	door->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	door->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-door.MEC");
    door->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-door.MEC");
    door->LoadTexture(puma->m_pd3dDevice,	"dat\\door.png");

    arms = new PumaMesh();
//	arms->LoadFromASC(puma->m_pd3dDevice,"dat\\NEWeapons.ASE");
//	arms->Scale(puma->m_pd3dDevice, 1.0f, 1.0f, 1.0f);
//	arms->SaveCompressed(puma->m_pd3dDevice,"dat\\fr-arms.MEC");
    arms->LoadCompressed(puma->m_pd3dDevice,"dat\\fr-arms.MEC");
    arms->LoadTexture(puma->m_pd3dDevice,	"dat\\weapons.png");

    PumaSound::SetupSoundSystem();

    songSound = new PumaSound("dat\\openSong.wav");
    button1Sound = new PumaSound("dat\\button1.wav");
    button2Sound = new PumaSound("dat\\button2.wav");

    candleParticle = new Particle2Type(300, "dat\\flare-white.png", 01.2f);
    candleParticle->lowAngle = 0;
    candleParticle->highAngle = D3DX_PI*2;
    candleParticle->lowAzimuth = D3DX_PI/3;
    candleParticle->highAzimuth = D3DX_PI/2;
    candleParticle->highLife = 30;
    candleParticle->lowLife = 10;
    candleParticle->lowSpeed = 0.1f;
    candleParticle->highSpeed = 0.3f;

    lastCandleParticleTick = 0;

    songSound->PlayNo3D();

    pleaseWaitArt = new VideoDataObject("dat\\TimeLeftBox.png", 0);
    vdMan->AddObject(pleaseWaitArt);

}

BOOL GetURLPageAndStoreToDisk(LPSTR pURLPage, LPSTR pOutputFile)
   {
HINTERNET hInternetSession;   
HINTERNET hURL;
char cBuffer[1024];      // Assume small page for sample.
BOOL bResult;
DWORD dwBytesRead;
HANDLE hOutputFile;

// Make internet connection.
hInternetSession = InternetOpen(
                  "Microsoft Internet Explorer",   // agent
                  INTERNET_OPEN_TYPE_PRECONFIG,      // access
                  NULL, NULL, 0);            // defaults

// Make connection to desired page.
hURL = InternetOpenUrl(
         hInternetSession,               // session handle
         pURLPage,   // URL to access
         NULL, 0, 0, 0);               // defaults

// read page into memory buffer
bResult = InternetReadFile(hURL, (LPSTR)cBuffer,
            (DWORD)1024, &dwBytesRead);

// close down connections
InternetCloseHandle(hURL);
InternetCloseHandle(hInternetSession);

// create output file
hOutputFile = CreateFile(pOutputFile,GENERIC_WRITE,0, NULL, CREATE_ALWAYS,
                         FILE_ATTRIBUTE_NORMAL, NULL);
// write out data
DWORD bytesWritten;
bResult = WriteFile(hOutputFile, cBuffer, dwBytesRead, &bytesWritten, NULL);
 
// close down file
CloseHandle(hOutputFile);

// return success
return(TRUE);
}

//******************************************************************
FirstOptionMode::~FirstOptionMode()
{

    vdMan->DeleteObject(pleaseWaitArt);

    delete songSound;
    delete button2Sound;
    delete button1Sound;

    PumaSound::ShutdownSoundSystem();

    delete candleParticle;
    delete wall;
    delete drapes;
    delete back;
    delete rug;
    delete door;
    delete arms;
    delete cstick;
    delete candle;

    delete lclient;
    if (createServerFlag)
    {
        _sleep(2000);
        delete server;
        server = NULL;
    }

    
    WSACleanup();

}

//******************************************************************
int FirstOptionMode::Init(void)  // do this when instantiated.
{
    return(0);
}

//******************************************************************
int FirstOptionMode::Activate(void) // do this when the mode becomes the forground mode.
{

    aLog.Log("FirstOptionMode::Activate ********************\n");

    chooseServer = FALSE;

    if (isConnected)
    {
        lclient->disconnect();
        isConnected = FALSE;
        tryingConnection = FALSE;
        messSent = FALSE;
    }


    uiAlpha = 100;

//	GetURLPageAndStoreToDisk("http://www.aggressive.to/puma.zip", "c:\puma.zip");

    SetEnvironment();

    mouseArt = new VideoDataObject("dat\\mouseart.png"  , 0xffff00ff);
    vdMan->AddObject(mouseArt);
//	backArt  = new VideoDataObject("dat\\support\\dr.png", 0);
    backArt  = new VideoDataObject("dat\\newLogo1.png", 0);
    vdMan->AddObject(backArt);
    backArt2 = new VideoDataObject("dat\\newLogo2.png", 0);
    vdMan->AddObject(backArt2);
    urFrame  = new VideoDataObject("dat\\URframe.png", 0);
    vdMan->AddObject(urFrame);
//	backArt  = new VideoDataObject("dat\\newLogo1.png", 0xff000000);
    uiBackground  = new VideoDataObject("dat\\UIstone.png", 0xffff00ff);
    vdMan->AddObject(uiBackground);

    login1  = new VideoDataObject("dat\\login1.png", 0);
    vdMan->AddObject(login1);

    login2  = new VideoDataObject("dat\\login2.png", 0);
    vdMan->AddObject(login2);


    cornerArt[0] = new VideoDataObject("dat\\cornerButtUp.png", 0);
    vdMan->AddObject(cornerArt[0]);
    cornerArt[1] = new VideoDataObject("dat\\cornerButtDown.png", 0);
    vdMan->AddObject(cornerArt[1]);
    cornerArt[2] = new VideoDataObject("dat\\cornerButtHigh.png", 0);
    vdMan->AddObject(cornerArt[2]);
    cornerArt[3] = new VideoDataObject("dat\\cornerButt2Up.png", 0);
    vdMan->AddObject(cornerArt[3]);
    cornerArt[4] = new VideoDataObject("dat\\cornerButt2Down.png", 0);
    vdMan->AddObject(cornerArt[4]);
    cornerArt[5] = new VideoDataObject("dat\\cornerButt2High.png", 0);
    vdMan->AddObject(cornerArt[5]);

    serverArt[0] = new VideoDataObject("dat\\serverChoice1.png", 0);
    vdMan->AddObject(serverArt[0]);
    serverArt[1] = new VideoDataObject("dat\\serverChoice2.png", 0);
    vdMan->AddObject(serverArt[1]);
    serverArt[2] = new VideoDataObject("dat\\serverHigh1.png", 0);
    vdMan->AddObject(serverArt[2]);
    serverArt[3] = new VideoDataObject("dat\\serverHigh2.png", 0);
    vdMan->AddObject(serverArt[3]);
    
   // ********** start building UIRects

    // first, set up the colors.
    UIRectWindow::topLeftColorGlobal     = D3DCOLOR_ARGB(uiAlpha, 246, 242, 230);
    UIRectWindow::bottomRightColorGlobal = D3DCOLOR_ARGB(uiAlpha, 158, 156, 130);
    UIRectWindow::fillColorGlobal        = D3DCOLOR_ARGB(uiAlpha, 50, 50, 50);


   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   tempWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   tempWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tempWindow->isActive  = TRUE;

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(FOM_BUTTON_QUIT, 65, puma->ScreenH() - 42, 155 ,puma->ScreenH() - 4);
   tButt->SetText("Quit to Windows");
   tButt->process = FirstOptionModeProcess;
    tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;
/*
   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,200,80,600,160);
   sprintf(tempText,"Welcome to the Blade Mistress online game!\nThis is the Blade Mistress Client V%2.2f\n\nLet's get started!",VERSION_NUMBER);
   tBox->SetText(tempText);
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tBox->fillArt = uiBackground->pTexture;
    tBox->font = 2;
   fullWindow->AddChild(tBox);

   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,2,240,300,296);
   sprintf(tempText,"STEP 1:  Type in your user name and password.\nIf this is your first time playing,\npick a user name and password that you'll remember,\nand the game will make a new account for you.");
   tBox->SetText(tempText);
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);

    //***************
    uiAlpha = 255;
  */
    uiAlpha = 100;
     /*
   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,2, 300  ,98, 330);
   tBox->SetText("User Name");
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);
    UIRectStep(0,1);

   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,-2,-2,-2,-2);
   tBox->SetText("Password");
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);

   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,320, 180  ,630, 228);
   tBox->SetText("STEP 2: Click on one of the buttons below,\n to choose a game world to play in.");
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);
 */
    /*
    UIRectScrollWindow *serverListWindow;

    serverListWindow = new UIRectScrollWindow(FOM_BUTTON_SERVERS, 320 ,232  ,630,470);
    serverListWindow->process = FirstOptionModeProcess;
    serverListWindow->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
    fullWindow->AddChild(serverListWindow);

    UIRectDragBar *dragBar;

    dragBar = new UIRectDragBar(-1, 16,-1,-1,-1);
    dragBar->process = FirstOptionModeProcess;
    dragBar->isYFree = TRUE;
    dragBar->yWidth  = 12;
    dragBar->fillArt = uiBackground->pTexture;
    serverListWindow->AddChild(dragBar);

    // add all known servers
   tButt = new UIRectTextButton(FOM_BUTTON_SERVER_0, -1, 0,-1,30);
   tButt->SetText("Beta Game");
   tButt->process = FirstOptionModeProcess;
    tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
    tButt->font = 1;
    tButt->fillArt = uiBackground->pTexture;
   serverListWindow->AddChild(tButt);
      */

    // tempWindow stuff
    
   tBox = new UIRectTextBox(FOM_BUTTON_TEMP,
                     puma->ScreenW()/2 - 100, puma->ScreenH()/2 + 120,
                     puma->ScreenW()/2 + 100, puma->ScreenH()/2 + 150);
   tBox->SetText("----------");
   tBox->process = FirstOptionModeProcess;
    tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
//	tBox->fillArt = uiBackground->pTexture;
   tempWindow->AddChild(tBox);
    
   fullWindow->Arrange();

   // ********** finished building UIRects



   if (cmdBypass && !tryingConnection)
   {
        button1Sound->PlayNo3D();

        if (isConnected)
        {
            lclient->disconnect();
            isConnected = FALSE;
            tryingConnection = FALSE;
            messSent = FALSE;
        }

#ifdef _TEST_SERVER
        if (!lclient->connect(ipAddress, 9178))
#else
        if (!lclient->connect(ipAddress, 3678))
#endif
        {
            newGameMode = new SimpleMessageMode("The server is not responding.  It may be temporarily stopped,\n or your local network could have a firewall that blocks access.\nPlease check if your network blocks access, and try again later.",0,"SIMPLE_MESSAGE_MODE");
            retState = GMR_NEW_MODE;
            return 0;
        }
        isConnected = TRUE;
        if (!tryingConnection)
        {
           UIRectTextBox *tBox = (UIRectTextBox *) tempWindow->childRectList.Find(FOM_BUTTON_TEMP);
            tBox->SetText(fomText1);

            tryingConnection = TRUE;
            messSent = FALSE;
            connectTime = timeGetTime();
        }
    }


    return(0);
}

//******************************************************************
int FirstOptionMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
    cmdBypass = FALSE;

    delete tempWindow;
    delete fullWindow;

    for (int i = 0; i< 6; ++i)
        vdMan->DeleteObject(cornerArt[i]);

    for (int i = 0; i< 4; ++i)
        vdMan->DeleteObject(serverArt[i]);

    vdMan->DeleteObject(login1);
    vdMan->DeleteObject(login2);

    vdMan->DeleteObject(uiBackground);
    vdMan->DeleteObject(urFrame);
    vdMan->DeleteObject(backArt2);
    vdMan->DeleteObject(backArt);
    vdMan->DeleteObject(mouseArt);


    return(0);
}

//******************************************************************
int FirstOptionMode::Tick(void)
{

    if (!chooseServer)
    {
        if (!fullWindow->childRectList.Find(FOM_BUTTON_NAME) && !cmdBypass)
        {
            UIRectEditLine *edLine	  = new UIRectEditLine(FOM_BUTTON_NAME, 
                                  puma->ScreenW()/2-21 + 12,puma->ScreenH()/2+86,
                                  puma->ScreenW()/2-21 + 12+190,puma->ScreenH()/2+89+20,
                                        31);
            edLine->process = FirstOptionModeProcess;
            edLine->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
            edLine->SetText(nameBuffer);
            edLine->font = 1;
            fullWindow->AddChild(edLine);
            UIRectShift(0,47);

            edLine	  = new UIRectEditLine(FOM_BUTTON_PASS,-2,-2,-2,-2,11); 
            edLine->process = FirstOptionModeProcess;
            edLine->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
            edLine->SetText(passBuffer);
            edLine->font = 1;
            edLine->isSecure = TRUE;
            fullWindow->AddChild(edLine);

            char tempText[1024];
            UIRectTextBox *tBox = new UIRectTextBox(FOM_BUTTON_TEXT,
                         80,puma->ScreenH()/2-70,puma->ScreenW()-80,puma->ScreenH()/2+60);
            sprintf(tempText,"Welcome to the Blade Mistress online game!\nThis is the Blade Mistress Client V%2.3f\n\nPlease type in your account name and password.\nIf this is your first time, then type in whatever\naccount name and password you would like to have.\nWhen you are done, press ENTER.",VERSION_NUMBER);
            tBox->SetText(tempText);
//			tBox->SetText("TEST");
            tBox->process = FirstOptionModeProcess;
            tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
            tBox->fillArt = uiBackground->pTexture;
            tBox->font = 2;
            fullWindow->AddChild(tBox);

           fullWindow->Arrange();
        }

        UIRectScrollWindow *serverListWindow = 
                   (UIRectScrollWindow *)fullWindow->childRectList.Find(FOM_BUTTON_SERVERS);
        if (serverListWindow)
        {
            fullWindow->childRectList.Remove(serverListWindow);
            delete serverListWindow;
        }
    }
    else
    {
        if (!fullWindow->childRectList.Find(FOM_BUTTON_SERVERS) && !cmdBypass)
        {
            UIRectScrollWindow *serverListWindow;

            serverListWindow = new UIRectScrollWindow(FOM_BUTTON_SERVERS,
                puma->ScreenW()/2-256+62, puma->ScreenH()/2-80 +57,
                puma->ScreenW()/2-256+62+315, puma->ScreenH()/2-80 +57+100);
            serverListWindow->process = FirstOptionModeProcess;
            serverListWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
            fullWindow->AddChild(serverListWindow);
                     /*
            UIRectDragBar *dragBar;

            dragBar = new UIRectDragBar(-1, 16,-1,-1,-1);
            dragBar->process = FirstOptionModeProcess;
            dragBar->isYFree = TRUE;
            dragBar->yWidth  = 12;
            dragBar->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
            dragBar->fillArt = uiBackground->pTexture;
            serverListWindow->AddChild(dragBar);
                        */
            // add all known servers
           UIRectTextButton *tButt;

            tButt = new UIRectTextButton(FOM_BUTTON_SERVER_0, -1, 0,-1,30);
            tButt->SetText("Primary Server");
            tButt->process = FirstOptionModeProcess;
            tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
            tButt->font = 1;
            tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
//			tButt->fillArt = uiBackground->pTexture;
            serverListWindow->AddChild(tButt);

           fullWindow->Arrange();
        }

        UIRectEditLine *edLine = 
                   (UIRectEditLine *)fullWindow->childRectList.Find(FOM_BUTTON_NAME);
        if (edLine)
        {
            fullWindow->childRectList.Remove(edLine);
            delete edLine;
        }
        edLine = 
                   (UIRectEditLine *)fullWindow->childRectList.Find(FOM_BUTTON_PASS);
        if (edLine)
        {
            fullWindow->childRectList.Remove(edLine);
            delete edLine;
        }
        UIRectTextBox *tBox = 
                   (UIRectTextBox *)fullWindow->childRectList.Find(FOM_BUTTON_TEXT);
        if (tBox)
        {
            fullWindow->childRectList.Remove(tBox);
            delete tBox;
        }
    }

    if (lastCandleParticleTick == 0)
        lastCandleParticleTick = timeGetTime();
    else
    {
        DWORD now = timeGetTime();
        float delta = (now-lastCandleParticleTick) / 30.0f;
        lastCandleParticleTick = now;
        candleParticle->Tick(delta);
    }

   D3DXMATRIX matWorld, mat2, matTrans;

   UIRectTextButton *tButt;
    UIRectScrollWindow *serverListWindow;

//	edName = (UIRectEditLine *) fullWindow->childRectList.Find(FOM_BUTTON_NAME);
//	edPass = (UIRectEditLine *) fullWindow->childRectList.Find(FOM_BUTTON_PASS);
    serverListWindow = (UIRectScrollWindow *) fullWindow->childRectList.Find(FOM_BUTTON_SERVERS);

    if (serverListWindow && nameBuffer[0] && passBuffer[0])
    {
        tButt = (UIRectTextButton *) serverListWindow->childRectList.Find(FOM_BUTTON_SERVER_0);
        if (tButt)
            tButt->isDisabled = FALSE;
    }
    else if (serverListWindow)
    {
        tButt = (UIRectTextButton *) serverListWindow->childRectList.Find(FOM_BUTTON_SERVER_0);
        if (tButt)
            tButt->isDisabled = TRUE;
    }


//	puma->HandleCamera();

    HandleMessages();

    WindowServicer();
    // processing

    D3DXVECTOR3 pOut;
    pOut.x = 14.3f; 
    pOut.y = 60.5f;
    pOut.z = -37;
    candleParticle->SetEmissionPoint(pOut);
    candleParticle->Emit(3, D3DCOLOR_ARGB(155, 255, 130, 0)); 

    ++candleAttTick;
    if (!(candleAttTick%5))
        candleAttenuation = rnd(0.01f,0.02f);

   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
    D3DUtil_InitLight( light, D3DLIGHT_POINT, -4,0,2);
    light.Position = pOut;
    light.Range = 100;
    light.Falloff = 1.0f;
    light.Attenuation1 = candleAttenuation;
   light.Diffuse.r  = 1.0f;
   light.Diffuse.g  = 1.0f;
   light.Diffuse.b  = 0.5f;
   light.Ambient.r  = 0.0f;
   light.Ambient.g  = 0.0f;
   light.Ambient.b  = 0.0f;
   puma->m_pd3dDevice->SetLight( 1, &light );
   puma->m_pd3dDevice->LightEnable( 1, TRUE );

    // start drawing

    if (!mouseArt->isReady)
        return 0;

    puma->StartRenderingFrame(11,63,60);

    D3DXMatrixIdentity( &matWorld );
    D3DXMatrixRotationY( &matWorld, puma->pumaCamera->spacePoint.angle); //camAngle[1]);
//	puma->pumaCamera->spacePoint.angle += 0.02;
    puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    D3DXMatrixIdentity( &matWorld );
    matWorld._41 = puma->pumaCamera->spacePoint.location.x;
    matWorld._42 = puma->pumaCamera->spacePoint.location.y;
    matWorld._43 = puma->pumaCamera->spacePoint.location.z;

    puma->m_pd3dDevice->SetTransform( D3DTS_VIEW , &matWorld );
    matTrans = matWorld;

    D3DXMatrixIdentity( &matWorld );
    puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    wall->Draw(puma->m_pd3dDevice);
    drapes->Draw(puma->m_pd3dDevice);
    back->Draw(puma->m_pd3dDevice);
    candle->Draw(puma->m_pd3dDevice);
    cstick->Draw(puma->m_pd3dDevice);
    door->Draw(puma->m_pd3dDevice);
    arms->Draw(puma->m_pd3dDevice);
    rug->Draw(puma->m_pd3dDevice);

    candleParticle->PrepareToDraw();
    candleParticle->Draw(matTrans);

    // Turn on the zbuffer
    puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );

    puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
    puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_NONE );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_NONE );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

    D3DSURFACE_DESC desc;
    backArt2->pTexture->GetLevelDesc(0, &desc);

    RECT dRect2 = {255,0,255+desc.Width-1,desc.Height-1};
//	RECT dRect = {puma->ScreenW()/2 + 30,30,desc.Width + puma->ScreenW()/2 + 30,desc.Height + 30};
    puma->DrawRect(backArt2->pTexture,dRect2,0xffffffff);

    backArt->pTexture->GetLevelDesc(0, &desc);

    RECT dRect = {0,0,desc.Width-1,desc.Height-1};
//	RECT dRect = {puma->ScreenW()/2 + 30,30,desc.Width + puma->ScreenW()/2 + 30,desc.Height + 30};
    puma->DrawRect(backArt->pTexture,dRect,0xffffffff);

    urFrame->pTexture->GetLevelDesc(0, &desc);

    RECT dRect3 = {puma->ScreenW() - desc.Width+1, 0, puma->ScreenW(), desc.Height-1};
//	RECT dRect = {puma->ScreenW()/2 + 30,30,desc.Width + puma->ScreenW()/2 + 30,desc.Height + 30};
    puma->DrawRect(urFrame->pTexture,dRect3,0xffffffff);

    if (!chooseServer)
    {
        login1->pTexture->GetLevelDesc(0, &desc);
        RECT dRect5 = {puma->ScreenW()/2,puma->ScreenH()/2+80,
                            puma->ScreenW()/2-desc.Width+1,puma->ScreenH()/2+80+desc.Height};
        puma->DrawRect(login1->pTexture,dRect5,0xffffffff);

        login2->pTexture->GetLevelDesc(0, &desc);
        RECT dRect6 = {puma->ScreenW()/2-21,puma->ScreenH()/2+80,
                            puma->ScreenW()/2+desc.Width-0-21,puma->ScreenH()/2+80+desc.Height};
        puma->DrawRect(login2->pTexture,dRect6,0xffffffff);
    }
    else
    {
        serverArt[0]->pTexture->GetLevelDesc(0, &desc);
        RECT dRect5 = {puma->ScreenW()/2,puma->ScreenH()/2-80,
                            puma->ScreenW()/2-desc.Width,puma->ScreenH()/2-80+desc.Height};
        puma->DrawRect(serverArt[0]->pTexture,dRect5,0xffffffff);

        serverArt[1]->pTexture->GetLevelDesc(0, &desc);
        RECT dRect6 = {puma->ScreenW()/2,puma->ScreenH()/2-80,
                            puma->ScreenW()/2+desc.Width,puma->ScreenH()/2-80+desc.Height};
        puma->DrawRect(serverArt[1]->pTexture,dRect6,0xffffffff);
    }
    

    int bIndex = 0;
   tButt = (UIRectTextButton *) fullWindow->childRectList.Find(FOM_BUTTON_QUIT);
    if (tButt)
    {
        if (tButt->isDepressed)
            bIndex = 4;
        else if (tButt->isActive)
            bIndex = 5;
        else
            bIndex = 3;
        cornerArt[bIndex]->pTexture->GetLevelDesc(0, &desc);
        RECT cRect = {0,puma->ScreenH()-128,255,puma->ScreenH()};
        puma->DrawRect(cornerArt[bIndex]->pTexture,cRect,0xffffffff);
    }



    if (!tryingConnection)
    {
       fullWindow->Draw();
        puma->DrawMouse(mouseArt->pTexture);
    }
    else
    {
        if (connectTime + 1000 * 7 < timeGetTime())
        {
            lclient->disconnect();
            isConnected = FALSE;
            tryingConnection = FALSE;
            messSent = FALSE;
            newGameMode = new SimpleMessageMode("The server is not responding.  It may be temporarily stopped,\n or your local network could have a firewall that blocks access.\nPlease check if your network blocks access, and try again later.",0,"SIMPLE_MESSAGE_MODE");
            retState = GMR_NEW_MODE;
        }
//		else if (!messSent && connectTime + 1000 * 2 < timeGetTime())
//		{
//		}

       tempWindow->Draw();
    }
    puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

    return(0);
}


//*******************************************************************************
long FirstOptionMode::WindowServicer(void)
{

    UIRectEditLine::blinkTimer++;

    // check input
    puma->PumpInput();

    PumaInputEvent *pie = puma->TakeNextInputEvent();
    while (pie)
    {
        switch(pie->WhatAmI())
        {
        case PUMA_INPUT_KEYDOWN:
            if (16 == pie->scancode)
                extraKeyFlags |= EXTRA_KEY_SHIFT_DOWN;

            if (39 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if right
                fullWindow->Action(UIRECT_FORWARD_ARROW, 0, pie->scancode);
            else if (37 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if left
                fullWindow->Action(UIRECT_BACK_ARROW, 0, pie->scancode);
         else 
                fullWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

            break;

        case PUMA_INPUT_KEYUP:
            if (16 == pie->scancode)
                extraKeyFlags &= ~(EXTRA_KEY_SHIFT_DOWN);
         fullWindow->Action(UIRECT_KEY_UP,   0, pie->scancode);
//		 if (pie->asciiValue >= 32 && pie->asciiValue <= 126 && 
//			 pie->asciiValue != 34 && pie->asciiValue != 44)
             fullWindow->Action(UIRECT_KEY_CHAR, 0, pie->asciiValue);

            break;

        case PUMA_INPUT_LMBDOWN:
           fullWindow->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
            break;
        case PUMA_INPUT_LMBUP:
           fullWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
            break;
        case PUMA_INPUT_RMBDOWN:
           fullWindow->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
            break;
        case PUMA_INPUT_RMBUP:
           fullWindow->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
            break;
        }

        delete pie; // IMPORTANT to delete the message after using it!
        pie = puma->TakeNextInputEvent();
    }

   fullWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);


    return 0;
}


//***************************************************************
void FirstOptionMode::HandleMessages(void)
{
    char messData[4000];
    int  dataSize;
    FILE *source = NULL;

    MessPlayerChatLine chatMess;

    MessGeneralNo  *messNoPtr;
    MessGeneralYes *messYesPtr;


    DoublyLinkedList *list = NULL;

    std::vector<TagID> tempReceiptList;
    int					fromSocket = 0;

    lclient->GetNextMsg(NULL, dataSize);
    
    while (dataSize > 0)
    {
        if (dataSize > 4000)
            dataSize = 4000;
        
        lclient->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);

        switch(messData[0])
        {
        case NWMESS_GENERAL_NO:
            messNoPtr = (MessGeneralNo *) messData;
            if (-1 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("User names must start with a letter or number.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (1 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("Sorry, that user name is already taken.\nPlease choose another.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (2 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("Sorry, we could not create that user file.\nPlease try again.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (3 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("The password is incorrect for this user.\nPlease try again.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (4 == messNoPtr->subType)
            {
                if (nameBuffer[0] && passBuffer[0])
                {
                    newGameMode = new StartNewCharMode(nameBuffer, passBuffer, 
                                                        0, "START_NEW_CHAR_MODE");
                    retState = GMR_NEW_MODE;
                }
            }
            if (5 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("The version number of your client does not match the version of this server.\nPlease exit the program and run the AutoUpdater to get the latest version\n of Blade Mistress.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (6 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("There is already a player of that name and password logged in.\nDid your game crash?  Please check to make sure the previous game\nwas completely shut down.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (7 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("This account has been temporarily excluded from play.\n\nTry back later.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (10 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("This account is banned.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (11 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("You must be a patron to join the test server.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
            if (12 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("Your computer is banned.",0,"SIMPLE_MESSAGE_MODE");
                retState = GMR_NEW_MODE;
            }
           break;

        case NWMESS_GENERAL_YES:
            messYesPtr = (MessGeneralYes *) messData;

//			newGameMode = new PickCharMode(0, "PICK_CHAR_MODE");
            newGameMode = new LegalMode(0, "LEGAL_MODE");
            retState = GMR_NEW_MODE;
            return; // don't get any more messages.
           break;

        case NWMESS_EXTENDED_INFO_REQUEST: // used here as a GO-AHEAD message from server
           UIRectTextBox *tBox = (UIRectTextBox *) tempWindow->childRectList.Find(FOM_BUTTON_TEMP);
            tBox->SetText(fomText2);

            if (nameBuffer[0] && passBuffer[0])
            {
                MessPlayerReturning messPlayerReturning;

                CorrectString(nameBuffer);
                CorrectString(passBuffer);
                sprintf_s(messPlayerReturning.name, NUM_OF_CHARS_FOR_USERNAME, "%s", nameBuffer);

                unsigned char salt[256];
                sprintf_s((char*)&salt[0], 256, "%s-%s", "BladeMistress", nameBuffer);

                // Hash the password
				unsigned char hashPass[HASH_BYTE_SIZE] = { 0 };
                if (!PasswordHash::CreateStandaloneHash((const unsigned char*) passBuffer, salt, 6969, hashPass))
                {
                    newGameMode = new SimpleMessageMode("There was an error sending your credentials to the server.\nPlease contact your server admin.", 0, "SIMPLE_MESSAGE_MODE");
                    retState = GMR_NEW_MODE;
                    return;
                }

				// Send hashed password
				memcpy(messPlayerReturning.pass, hashPass, HASH_BYTE_SIZE);
				messPlayerReturning.pass[HASH_BYTE_SIZE] = 0;

                messPlayerReturning.uniqueId = GetUniqueComputerId();
                messPlayerReturning.version = VERSION_NUMBER;	

                codePad = fomText1;
                CryptoString(messPlayerReturning.name);
                
                lclient->SendMsg(sizeof(messPlayerReturning),(void *)&messPlayerReturning);
            }

            messSent = TRUE;
           break;
        }
        
        lclient->GetNextMsg(NULL, dataSize);
    }

}

unsigned char dpwsCounter = 0;

//******************************************************************
void DrawPleaseWaitScreen(void)
{

    ++dpwsCounter;
    if (dpwsCounter > 6)
        dpwsCounter = 0;

    puma->StartRenderingFrame(0,0,40);

    puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
    puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
    puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

    D3DSURFACE_DESC desc;
    pleaseWaitArt->pTexture->GetLevelDesc(0, &desc);

    RECT dRect = {puma->ScreenW()/2 - 101, puma->ScreenH()/2 - 28,
                  puma->ScreenW()/2 + 101, puma->ScreenH()/2 + 28};
    puma->DrawRect(pleaseWaitArt->pTexture,dRect,0xffffffff);

    CD3DFont *pf = puma->GetDXFont(0);
    char tempText[128];
    sprintf(tempText,"Traveling....................");
    tempText[10 + dpwsCounter] = 0;
    RECT rect = {puma->ScreenW()/2 - 40, puma->ScreenH()/2 - 6,
                 puma->ScreenW()/2 + 80, puma->ScreenH()/2 + 12};
    pf->DrawText(rect,NULL,0xffffffff,tempText);

    puma->FinishRenderingFrame();

}

//******************************************************************
void FirstOptionMode::SetEnvironment(void)
{

    puma->pumaCamera->m_fFarPlane   = 300.0f;
    puma->pumaCamera->Calculate();
    puma->pumaCamera->AssertView(puma->m_pd3dDevice);

    // Set up a white, directional light.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -4,0,2);
   light.Ambient.r  = 1.0f;
   light.Ambient.g  = 1.0f;
   light.Ambient.b  = 1.0f;
   puma->m_pd3dDevice->SetLight( 0, &light );
   puma->m_pd3dDevice->LightEnable( 0, TRUE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   // Finally, turn on some ambient light.
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(128,128,128,255)  );

    // init camera position
    /*
    puma->pumaCamera->spacePoint.angle = -1.0f * D3DX_PI/2;
    puma->pumaCamera->spacePoint.azimuth = -0.37f;
    puma->pumaCamera->spacePoint.location.x = 165.0f;
    puma->pumaCamera->spacePoint.location.y = -66.0f;
    puma->pumaCamera->spacePoint.location.z = -125.0f;
    */
    puma->pumaCamera->spacePoint.angle = -12.67f;
    puma->pumaCamera->spacePoint.azimuth = -0.37f;
    puma->pumaCamera->spacePoint.location.x = 20.0f;
    puma->pumaCamera->spacePoint.location.y = -41.0f;
    puma->pumaCamera->spacePoint.location.z = 100.0f;

   ZeroMemory( &neutralMaterial, sizeof(D3DMATERIAL8) );
   neutralMaterial.Diffuse.r = 0.5f;
   neutralMaterial.Diffuse.g = 0.5f;
   neutralMaterial.Diffuse.b = 0.5f;
   neutralMaterial.Diffuse.a = 0.5f;
   neutralMaterial.Ambient.r = 0.5f;
   neutralMaterial.Ambient.g = 0.5f;
   neutralMaterial.Ambient.b = 0.5f;
   neutralMaterial.Ambient.a = 0.5f;

}





/* end of file */



