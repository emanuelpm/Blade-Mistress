
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "simpleMessageMode.h"
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
#include "particle2.h"


enum 
{
	SMM_BUTTON_DONE,
	SMM_BUTTON_TEXT
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
SimpleMessageMode *curSimpleMessageMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL SimpleMessageModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	MessPlayerNew messPlayerNew;

	switch(curUIRect->WhatAmI())	
	{
	case SMM_BUTTON_DONE :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curSimpleMessageMode->retState = GMR_POP_ME;
      }
		break;

	}
	return 0;  // didn't deal with this message
}



//******************************************************************
SimpleMessageMode::SimpleMessageMode(char *str, int doid, char *doname) : GameMode(doid,doname)
{
	curSimpleMessageMode = this;
	sprintf(string,str);
}

//******************************************************************
SimpleMessageMode::~SimpleMessageMode()
{
}

//******************************************************************
int SimpleMessageMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int SimpleMessageMode::Activate(void) // do this when the mode becomes the forground mode.
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

   tButt = new UIRectArtTextButton(SMM_BUTTON_DONE,
		              0, puma->ScreenH() - 127,
		            255, puma->ScreenH());
   tButt->SetText("Continue");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = SimpleMessageModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButt2High.png", 0);
	tButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButt2Down.png", 0);
	tButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButt2Up.png", 0);
	tButt->textOffsetX = -45;
	tButt->textOffsetY = 82;
   fullWindow->AddChild(tButt);
//   UIRectTextButton *tButt;

   UIRectTextBox *tBox;

	//***************
   tBox = new UIRectTextBox(SMM_BUTTON_TEXT,70,100,puma->ScreenW()-70,puma->ScreenH()-100);
   tBox->SetText(string);
   tBox->process = SimpleMessageModeProcess;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tBox);

   fullWindow->Arrange();

   // ********** finished building UIRects



	return(0);
}

//******************************************************************
int SimpleMessageMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;

//   SAFE_RELEASE(backArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int SimpleMessageMode::Tick(void)
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
	float tweakF;

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
long SimpleMessageMode::WindowServicer(void)
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
void SimpleMessageMode::HandleMessages(void)
{
	char messData[4000];
	int  dataSize;
	FILE *source = NULL;

	MessPlayerChatLine chatMess;


	DoublyLinkedList *list = NULL;

	std::vector<TagID> tempReceiptList;
	int					fromSocket = 0;

	lclient->GetNextMsg(NULL, dataSize);
	
	while (dataSize > 0)
	{
		if (dataSize > 4000)
			dataSize = 4000;
		
		lclient->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);
/*		
		MessEmpty *empty = (MessEmpty *)messData;
		switch (messData[0])
		{

		case NWMESS_PLAYER_CHAT_LINE:
			InjectTextLine(&(messData[1]));
		   break;
		}
		*/
		lclient->GetNextMsg(NULL, dataSize);
	}

}


//******************************************************************
void SimpleMessageMode::SetEnvironment(void)
{

	puma->pumaCamera->m_fFarPlane   = 300.0f;
	puma->pumaCamera->Calculate();
	puma->pumaCamera->AssertView(puma->m_pd3dDevice);

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



