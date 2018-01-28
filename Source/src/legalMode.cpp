
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wininet.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "legalmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
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
#include "startNewCharMode.h"

#include "version.h"



char *legalText1 = "PLEASE READ THE FOLLOWING LEGAL INFORMATION CAREFULLY\n\n\
DISCLAIMER OF WARRANTIES\n\n\
YOU AGREE THAT BLADE MISTRESS ONLINE HAS MADE NO EXPRESS WARRANTIES TO YOU REGARDING\n\
THE SOFTWARE AND THAT THE SOFTWARE IS BEING PROVIDED TO YOU ON AN 'AS IS' BASIS WITHOUT\n\
ANY WARRANTIES OR REPRESENTATIONS OF ANY KIND. BLADE MISTRESS ONLINE DISCLAIMS ALL\n\
WARRANTIES WORLDWIDE WITH REGARD TO THE SOFTWARE, EXPRESS OR IMPLIED, INCLUDING ANY\n\
WARRANTY OF SOFTWARE PERFORMANCE, AND ANY IMPLIED WARRANTIES OF FITNESS FOR A SPECIFIC\n\
PURPOSE, ACCURACY, MERCHANTABILITY OF COMPUTER PROGRAM, MERCHANTABLE QUALITY OR\n\
NONINFRINGEMENT OF THIRD PARTY RIGHTS. SOME STATES OR JURISDICTIONS DO NOT ALLOW THE\n\
EXCLUSION OF IMPLIED WARRANTIES SO THE ABOVE LIMITATIONS MAY NOT APPLY TO YOU.\n\n\
THIS PRODUCT COULD INCLUDE INACCURACIES IN THE INFORMATION THEREIN OR TYPOGRAPHICAL\n\
ERRORS. BLADE MISTRESS ONLINE DISCLAIMS ALL LIABILITY RELATED TO SUCH INACCURACIES\n\
OR ERRORS. BUYER'S SOLE REMEDY FOR ALL CLAIMS RELATED TO THE PRODUCT SHALL BE THE\n\
AMOUNT PAID FOR THE PRODUCT.\n\n\
LIMIT OF LIABILITY\n\n\
In no event will the developers or publishers of Blade Mistress\n\
be liable to you for any loss of use,\n\
interruption of business, or any direct, indirect, special, incidental or consequential\n\
damages of any kind (including lost profits) regardless of the form of action whether in\n\
contract, tort (including negligence), strict product liability or otherwise, even if\n\
the developers or publishers have been advised of the possibility of such damages. Some states\n\
or jurisdictions do not allow the exclusion or limitation of incidental or consequential\n\
damages, so the above limitation or exclusion may not apply to you.";


char *legalText2 = "COPYRIGHTS AND TRADEMARKS\n\n\
BLADE MISTRESS, the BLADE MISTRESS logo, and all material on the BLADE MISTRESS website\n\
are protected by the Copyright Laws of the United States and International Treaty Provisions.\n\
Copyright 2002 Blade Mistress Online, http://www.blademistress.com.\n\
BLADE MISTRESS Copyright 2002.\n\
All rights reserved.\n\n\
The trademark 'BLADE MISTRESS' is protected by US Trademark law.\n\n\
Distribution and use of BLADE MISTRESS and/or registration at the\n\
Blade Mistress Online web site is conditioned on your acceptance of the terms described here.\n\
If these terms are not acceptable, remove all BLADE MISTRESS files from your computer.\n\
Individual purchasers of gameplay time on BLADE MISTRESS will receive a full refund\n\
of unused paid gameplay time.\n\
By downloading BLADE MISTRESS software and/or by registering at the\n\
BLADE MISTRESS web site,\n\
you agree to the above terms and conditions. If you do not agree with such terms and\n\
conditions do not install or download the software.\n\n\
This Agreement is governed by the laws of Texas.\n\n\
This License Agreement may be modified in the future by Blade Mistress Online.\n\n\
By clicking on the 'YES' button below you are agreeing to be bound by this Agreement and the\n\
Terms of Service agreement on the Blade Mistress web site. If you do not agree to these terms,\n\
Blade Mistress Online will not license this Product to you and you should not continue to download,\n\
register or use the Product or anything at Blade Mistress Online's web site.\n\n\
By continuing, you certify that you are over 18 years of age or have your parent's\n\
or guardian's permission to play this game.";


enum 
{
	LM_BUTTON_YES,
	LM_BUTTON_NO,
	LM_BUTTON_TEXT
};

extern BBOServer *server;
extern Client *	lclient;
extern char ipAddress[128];
extern int createServerFlag;

extern PumaMesh *wall;

// static pointer that the process function can use
LegalMode *curLegalMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL LegalModeProcess(UIRect *curUIRect, int type, long x, short y)
{

	switch(curUIRect->WhatAmI())	
	{
	case LM_BUTTON_NO :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curLegalMode->retState = GMR_POP_ME;
//	      PostQuitMessage( 0 );
			return 1;
      }
		break;

	case LM_BUTTON_YES :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			if (curLegalMode->WhatAmI() == 0)
				newGameMode = new LegalMode(1, "SECOND_LEGAL_MODE");
			else
				newGameMode = new PickCharMode(0, "PICK_CHAR_MODE");
			curLegalMode->retState = GMR_POP_ME;
//	      PostQuitMessage( 0 );
			return 1;
      }
		break;
	}
	return 0;  // didn't deal with this message
}



//******************************************************************
LegalMode::LegalMode(int doid, char *doname) : GameMode(doid,doname)
{
}

//******************************************************************
LegalMode::~LegalMode()
{

}

//******************************************************************
int LegalMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int LegalMode::Activate(void) // do this when the mode becomes the forground mode.
{

	aLog.Log("LegalMode::Activate ********************\n");

	curLegalMode = this;
	uiAlpha = 100;

	SetEnvironment();

	mouseArt = new VideoDataObject("dat\\mouseart.png"  , 0xffff00ff);
	vdMan->AddObject(mouseArt);
//	backArt  = new VideoDataObject("dat\\menu-back1.png", 0xffff00ff);
//	vdMan->AddObject(backArt);
	uiBackground  = new VideoDataObject("dat\\UIstone.png", 0xffff00ff);
	vdMan->AddObject(uiBackground);

   // ********** start building UIRects

	// first, set up the colors.
	UIRectWindow::topLeftColorGlobal     = D3DCOLOR_ARGB(uiAlpha, 246, 242, 230);
	UIRectWindow::bottomRightColorGlobal = D3DCOLOR_ARGB(uiAlpha, 158, 156, 130);
	UIRectWindow::fillColorGlobal        = D3DCOLOR_ARGB(uiAlpha, 50, 50, 50);


   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectArtTextButton *tButt;

   tButt = new UIRectArtTextButton(LM_BUTTON_YES, 
		            puma->ScreenW()-255, puma->ScreenH() - 127,
		            puma->ScreenW(),     puma->ScreenH());
   tButt->SetText("Yes, I agree");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = LegalModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButtHigh.png", 0);
	tButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButtDown.png", 0);
	tButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButtUp.png", 0);
	tButt->textOffsetX = 45;
	tButt->textOffsetY = 82;
   fullWindow->AddChild(tButt);

   tButt = new UIRectArtTextButton(LM_BUTTON_NO,
		              0, puma->ScreenH() - 127,
		            255, puma->ScreenH());
   tButt->SetText("No, I don't");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = LegalModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButt2High.png", 0);
	tButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButt2Down.png", 0);
	tButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButt2Up.png", 0);
	tButt->textOffsetX = -45;
	tButt->textOffsetY = 82;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;

   tBox = new UIRectTextBox(LM_BUTTON_TEXT,20,2,puma->ScreenW() - 20,puma->ScreenH() - 80);
	if (WhatAmI() == 0)
	   tBox->SetText(legalText1);
	else
	   tBox->SetText(legalText2);
   tBox->process = LegalModeProcess;
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);


   fullWindow->Arrange();

   // ********** finished building UIRects



	return(0);
}

//******************************************************************
int LegalMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;

//   SAFE_RELEASE(wallArt);
	vdMan->DeleteObject(uiBackground);
//	vdMan->DeleteObject(backArt);
	vdMan->DeleteObject(mouseArt);

	curLegalMode = NULL;
	return(0);
}

//******************************************************************
int LegalMode::Tick(void)
{

   D3DXMATRIX matWorld, mat2;

	HandleMessages();

	WindowServicer();
	// processing


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


	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	wall->Draw(puma->m_pd3dDevice);

//	backArt->pTexture->GetLevelDesc(0, &desc);

//	RECT dRect = {0,0,desc.Width,desc.Height};
//	puma->DrawRect(backArt->pTexture,dRect,0xffffffff);

   fullWindow->Draw();

	puma->DrawMouse(mouseArt->pTexture);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long LegalMode::WindowServicer(void)
{

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
void LegalMode::HandleMessages(void)
{
	char messData[4000];
	int  dataSize;
	FILE *source = NULL;

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
		case 0:
			;
		   break;
		}
		
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//******************************************************************
void LegalMode::SetEnvironment(void)
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
	puma->pumaCamera->spacePoint.location.x = 40.0f;
	puma->pumaCamera->spacePoint.location.y = -41.0f;
	puma->pumaCamera->spacePoint.location.z = 115.0f;

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



