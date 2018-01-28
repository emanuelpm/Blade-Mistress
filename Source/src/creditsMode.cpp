
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "CreditsMode.h"
#include "groundTestMode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragEdge.h"
#include "./puma/UIR_EdgeBar.h"
#include "./puma/UIR_EditLine.h"

#include "clientOnly.h"

#include "BBOClient.h"
#include "BBOServer.h"

char *cred1Text = "Blade Mistress was created by these folks:\n\n\
*** Concept, Design and Programming\n\
Bear\n\n\
*** Network code\n\
Erik Dieckert\n\n\
*** Community Manager, designer, deveoper, and Webmaster\n\
Punisher\n\n\
*** Animation\n\
Andrei Baca  andyba@web3dservice.com\n\
website: http://www.web3dservice.com\n\n\
*** 3D Models and textures\n\
Andrei Baca andyba@web3dservice.com\n\
Andrei Coval ankart@web3dservice.com\n\
Innokenty Shevchenko kesha@web3dservice.com\n\n\
*** Avatar and Monster skinning, plus other art\n\
Marion\n\n\
*** initial testing group\n\
Frank Chang, Richard Ham, Bruce Cooner, Mike Steele, David Haddock, Erik Dieckert, and Marion\n\n\
*** additional weapon and staff art by\n\
JaneDoe\n\n\
*** Special thanks\n\
Andra";




enum 
{
	CRDM_BUTTON_RETURN,
	CRDM_BUTTON_TEXT
};

//PlasmaTexture *bboClient->groundMap;
extern BBOServer *server;
extern Client *	lclient;

extern int playerInDungeon;

extern BBOClient *bboClient;

extern GroundTestMode *curgroundTestMode;


// static pointer that the process function can use
CreditMode *curCreditMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL creditModeProcess(UIRect *curUIRect, int type, long x, short y)
{

	if (UIRECT_MOUSE_LUP == type)
	{

		switch(curUIRect->WhatAmI())	
		{
		case CRDM_BUTTON_RETURN :
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curCreditMode->retState = GMR_POP_ME;
			return 1;
			break;

		}
	}
	return 0;  // didn't deal with this message
}

//******************************************************************
//******************************************************************
CreditMode::CreditMode(int doid, char *doname) : GameMode(doid,doname)
{
	curCreditMode = this;
}

//******************************************************************
CreditMode::~CreditMode()
{
	curCreditMode = NULL;
}

//******************************************************************
int CreditMode::Init(void)  // do this when instantiated.
{
	return(0);
}

const int CRDM_UI_ALPHA = 100;

//******************************************************************
int CreditMode::Activate(void) // do this when the mode becomes the forbboClient->ground mode.
{
	uiAlpha = CRDM_UI_ALPHA;

	// Set up a white, directional light.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
   ZeroMemory( &light, sizeof(D3DLIGHT8) );
   light.Type      = D3DLIGHT_DIRECTIONAL;
   light.Diffuse.r  = 0.6f;
   light.Diffuse.g  = 0.6f;
   light.Diffuse.b  = 0.6f;
   light.Ambient.r  = 0.6f;
   light.Ambient.g  = 0.6f;
   light.Ambient.b  = 0.6f;
   vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
   light.Range      = 1000.0f;
   puma->m_pd3dDevice->SetLight( 0, &light );
   puma->m_pd3dDevice->LightEnable( 0, TRUE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   vecDir = D3DXVECTOR3(-1.0f, -1.0f, -0.5f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
   puma->m_pd3dDevice->SetLight( 1, &light );

   // Finally, turn on some ambient light.
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(128,128,128,255)  );

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

	puma->LoadTexture("dat\\UIPopUp.png"   , &uiPopUpArt   , 0);	// 153x26

   // ********** start building UIRects
	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

//	actionWindow = NULL;

   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(CRDM_BUTTON_RETURN,puma->ScreenW()/2-76,puma->ScreenH()/2+200,
		                                           puma->ScreenW()/2+77,puma->ScreenH()/2+226);
   tButt->SetText("Return");
   tButt->process = creditModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ALL;
	tButt->fillArt = uiPopUpArt;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;

   tBox = new UIRectTextBox(CRDM_BUTTON_TEXT,2,20,
		                                          puma->ScreenW()-2,puma->ScreenH()/2+190);
   tBox->SetText(cred1Text);
   tBox->process = creditModeProcess;
//	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(tBox);


   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff

	return(0);
}

//******************************************************************
int CreditMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;
   SAFE_RELEASE(uiPopUpArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int CreditMode::Tick(void)
{

//	DebugOutput("GTMTick -1\n");

   D3DXMATRIX matWorld, mat2;
//	float tweakF;

	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;


	bboClient->Tick();
//	DebugOutput("GTMTick -2.5\n");

	HandleMessages();

//	DebugOutput("GTMTick -3\n");

	WindowServicer();
	// processing

	bboClient->curCamAngle += 0.001f;

	// start drawing

	puma->StartRenderingFrame(0,0,30);

	bboClient->Draw();

   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

	return(0);
}


//*******************************************************************************
long CreditMode::WindowServicer(void)
{

	UIRectWindow *theWindow = fullWindow;
//	int tempX;
	int result;

	PumaInputEvent *pie = puma->TakeNextInputEvent();
	while (pie)
	{
		switch(pie->WhatAmI())
		{
		case PUMA_INPUT_KEYDOWN:
         theWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

			break;

		case PUMA_INPUT_KEYUP:
         theWindow->Action(UIRECT_KEY_UP,   0, pie->scancode);
	      theWindow->Action(UIRECT_KEY_CHAR, 0, pie->asciiValue);

			break;

		case PUMA_INPUT_LMBDOWN:
		   result = theWindow->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_LMBUP:
		   result = theWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBDOWN:
		   theWindow->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBUP:
		   theWindow->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
			break;
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

   theWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);

	return 0;
}


//***************************************************************
void CreditMode::HandleMessages(void)
{
	char messData[4000];//, tempText[1024];
	int  dataSize;
	FILE *source = NULL;
	MessPlayerChatLine chatMess;
//	MessInfoText *infoText;

	DoublyLinkedList *list = NULL;

	std::vector<TagID> tempReceiptList;
	int					fromSocket = 0;

	lclient->GetNextMsg(NULL, dataSize);
	
	while (dataSize > 0)
	{
		if (dataSize > 4000)
			dataSize = 4000;
		
		lclient->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);
		
		MessEmpty *empty = (MessEmpty *)messData;
		switch (messData[0])
		{
		case NWMESS_PLAYER_CHAT_LINE:
			if (curgroundTestMode)
				curgroundTestMode->InjectTextLine(&(messData[1]));
		   break;

		default:
			bboClient->HandleMessage(messData, dataSize);
			break;

		}
		lclient->GetNextMsg(NULL, dataSize);
	}

}

/* end of file */



