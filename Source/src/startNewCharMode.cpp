
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "startNewCharMode.h"
#include "simpleMessageMode.h"
#include "pickCharMode.h"
#include "legalMode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragEdge.h"
#include "./puma/UIR_EditLine.h"

#include "./network/NetWorldMessages.h"

#include "clientOnly.h"

#include "BBOServer.h"
#include "./network/client.h"
#include "./helper/PasswordHash.h"
#include ".\helper\crypto.h"
#include "particle2.h"


enum 
{
	SNCM_BUTTON_YES,
	SNCM_BUTTON_NO,
	SNCM_BUTTON_TEXT
};

extern BBOServer *server;
extern Client *	lclient;

extern int playerAvatarID;
extern PumaMesh *wall;
extern PumaMesh *rug;
extern PumaMesh *candle;
extern PumaMesh *cstick;
extern PumaMesh *back;
extern PumaMesh *door;
extern PumaMesh *drapes;
extern PumaMesh *arms;
extern Particle2Type   *candleParticle;
extern DWORD lastCandleParticleTick;
extern float candleAttenuation;
extern int candleAttTick;


// static pointer that the process function can use
StartNewCharMode *curStartNewCharMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL StartNewCharModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	MessPlayerNew messPlayerNew;

	switch(curUIRect->WhatAmI())	
	{
	case SNCM_BUTTON_NO :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curStartNewCharMode->retState = GMR_POP_ME;
      }
		break;

	case SNCM_BUTTON_YES :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			sprintf(messPlayerNew.name, curStartNewCharMode->name);

			unsigned char salt[256];
			sprintf_s((char*)&salt[0], 256, "%s-%s", "BladeMistress", curStartNewCharMode->name);

			// Hash the password
			unsigned char hashPass[HASH_BYTE_SIZE] = { 0 };
			if (!PasswordHash::CreateStandaloneHash((const unsigned char*)curStartNewCharMode->pass, salt, 6969, hashPass))
			{
				newGameMode = new SimpleMessageMode("There was an error sending your credentials to the server.\nPlease contact your server admin.", 0, "SIMPLE_MESSAGE_MODE");
				curStartNewCharMode->retState = GMR_NEW_MODE;
				return 0;
			}

			// Send hashed password
			memcpy(messPlayerNew.pass, hashPass, HASH_BYTE_SIZE);
			messPlayerNew.pass[HASH_BYTE_SIZE] = 0;

			messPlayerNew.uniqueId = GetUniqueComputerId();
			CryptoString(messPlayerNew.name);

			lclient->SendMsg(sizeof(messPlayerNew),(void *)&messPlayerNew);
      }
		break;

	}
	return 0;  // didn't deal with this message
}



//******************************************************************
StartNewCharMode::StartNewCharMode(char *n, char *p, int doid, char *doname) : GameMode(doid,doname)
{
	curStartNewCharMode = this;
	sprintf(name,n);
	sprintf(pass,p);
}

//******************************************************************
StartNewCharMode::~StartNewCharMode()
{
}

//******************************************************************
int StartNewCharMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int StartNewCharMode::Activate(void) // do this when the mode becomes the forground mode.
{

	SetEnvironment();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);
/*
	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\menu-back1.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &backArt);
  */
   // ********** start building UIRects
   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectArtTextButton *tButt;

   tButt = new UIRectArtTextButton(SNCM_BUTTON_YES, 
		            puma->ScreenW()-255, puma->ScreenH() - 127,
		            puma->ScreenW(),     puma->ScreenH());
   tButt->SetText("Yes, I agree");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = StartNewCharModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButtHigh.png", 0);
	tButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButtDown.png", 0);
	tButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButtUp.png", 0);
	tButt->textOffsetX = 45;
	tButt->textOffsetY = 82;
   fullWindow->AddChild(tButt);

   tButt = new UIRectArtTextButton(SNCM_BUTTON_NO,
		              0, puma->ScreenH() - 127,
		            255, puma->ScreenH());
   tButt->SetText("No, I don't");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = StartNewCharModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButt2High.png", 0);
	tButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButt2Down.png", 0);
	tButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButt2Up.png", 0);
	tButt->textOffsetX = -45;
	tButt->textOffsetY = 82;
   fullWindow->AddChild(tButt);
/*
   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(SNCM_BUTTON_NO, 
		         puma->ScreenW()/2-200,puma->ScreenH()-50, puma->ScreenW()/2-100,puma->ScreenH()-10);
   tButt->SetText("No, cancel");
   tButt->process = StartNewCharModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);
	UIRectStep(3,0);

   tButt = new UIRectTextButton(SNCM_BUTTON_YES, -2,-2,-2,-2);
   tButt->SetText("Yes, proceed");
   tButt->process = StartNewCharModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);
  */
   UIRectTextBox *tBox;

	//***************
   tBox = new UIRectTextBox(SNCM_BUTTON_TEXT,100,100,puma->ScreenW()-100,puma->ScreenH()-100);
   tBox->SetText("That user account does not exist.\nDid you want to create it as\na new account?");
   tBox->process = StartNewCharModeProcess;
	tBox->font = 1;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tBox);

   fullWindow->Arrange();

   // ********** finished building UIRects

	return(0);
}

//******************************************************************
int StartNewCharMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;

//   SAFE_RELEASE(backArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int StartNewCharMode::Tick(void)
{
	if (lastCandleParticleTick == 0)
		lastCandleParticleTick = timeGetTime();
	else
	{
		DWORD now = timeGetTime();
		float delta = (now-lastCandleParticleTick) / 30.0f;
		lastCandleParticleTick = now;
		candleParticle->Tick(delta);
	}

   D3DXMATRIX matWorld, mat2, mat3, matTrans;

//	if (server)
//		server->Tick();
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

	puma->StartRenderingFrame(11,63,60);

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationX( &mat2, puma->pumaCamera->spacePoint.azimuth);
	D3DXMatrixRotationY( &matWorld, puma->pumaCamera->spacePoint.angle); //camAngle[1]);
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat2 );
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

/*
	D3DSURFACE_DESC desc;
	backArt->GetLevelDesc(0, &desc);

	RECT dRect = {0,0,desc.Width,desc.Height};
	puma->DrawRect(backArt,dRect,0xffffffff);
*/
   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long StartNewCharMode::WindowServicer(void)
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
         fullWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

			break;

		case PUMA_INPUT_KEYUP:
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
void StartNewCharMode::HandleMessages(void)
{
	char messData[4000];
	int  dataSize;
	FILE *source = NULL;

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
				newGameMode = new SimpleMessageMode("User names must start with a letter or number.",0,"LOGIN_MODE");
				retState = GMR_POP_ME;
			}
			if (1 == messNoPtr->subType)
			{
				newGameMode = new SimpleMessageMode("Sorry, that user name is already taken.\nPlease choose another.",0,"LOGIN_MODE");
				retState = GMR_POP_ME;
			}
			if (2 == messNoPtr->subType)
			{
				newGameMode = new SimpleMessageMode("Sorry, we could not create that user file.\nPlease try again.",0,"LOGIN_MODE");
				retState = GMR_POP_ME;
			}
			if (3 == messNoPtr->subType)
			{
				newGameMode = new SimpleMessageMode("The password is incorrect for this user.\nPlease try again.",0,"LOGIN_MODE");
				retState = GMR_POP_ME;
			}
		   break;

		case NWMESS_GENERAL_YES:
			messYesPtr = (MessGeneralYes *) messData;

			// newGameMode = new PickCharMode(0, "PICK_CHAR_MODE");
			newGameMode = new LegalMode(0, "LEGAL_MODE");
			retState = GMR_POP_ME;
			return; // don't get any more messages.
		   break;
		}
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//******************************************************************
void StartNewCharMode::SetEnvironment(void)
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
	puma->pumaCamera->spacePoint.angle = -0.104f;
	puma->pumaCamera->spacePoint.azimuth = -0.314f;
	puma->pumaCamera->spacePoint.location.x = -17.4f;
	puma->pumaCamera->spacePoint.location.y = -15.0f;
	puma->pumaCamera->spacePoint.location.z = 157.0f;

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



