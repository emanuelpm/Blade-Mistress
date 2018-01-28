
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wininet.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "robotmode.h"
#include "robot.h"
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

//#include "SimpleMessageMode.h"
//#include "PickCharMode.h"
//#include "legalMode.h"
//#include "startNewCharMode.h"

#include "version.h"

enum 
{
	FOM_BUTTON_LOGIN,
	FOM_BUTTON_NAME,
	FOM_BUTTON_PASS,
	FOM_BUTTON_QUIT,
	FOM_BUTTON_SERVERS,
	FOM_BUTTON_TEXT,
	FOM_BUTTON_SERVER_0
};


// static pointer that the process function can use
RobotMode *curRobotMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL RobotModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	switch(curUIRect->WhatAmI())	
	{
	case FOM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
	      PostQuitMessage( 0 );
			return 1;
      }
		break;
	}
	return 0;  // didn't deal with this message
}



//******************************************************************
RobotMode::RobotMode(int doid, char *doname) : GameMode(doid,doname)
{
	curRobotMode = this;
	isConnected = FALSE;
	tryingConnection = FALSE;

	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	Robot *r = new Robot(0,"ROBOT");
	robotList.Append(r);

	for (int i = 1; i < 20; ++i)
	{
		r = new Robot(i,"ROBOT");
		robotList.Append(r);
	}


}


//******************************************************************
RobotMode::~RobotMode()
{

	Robot *r = (Robot *) robotList.First();
	while (r)
	{
		robotList.Remove(r);
		delete r;

		r = (Robot *) robotList.First();
	}

	WSACleanup();

}

//******************************************************************
int RobotMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int RobotMode::Activate(void) // do this when the mode becomes the forground mode.
{

	uiAlpha = 100;

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

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(FOM_BUTTON_QUIT, 2, puma->ScreenH() - 40, 90 ,puma->ScreenH() - 2);
   tButt->SetText("Quit to Windows");
   tButt->process = RobotModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tButt->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;

   tBox = new UIRectTextBox(FOM_BUTTON_TEXT,320,2,puma->ScreenW() - 2,25);
   tBox->SetText("Blade Mistress robots!");
   tBox->process = RobotModeProcess;
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tBox->fillArt = uiBackground->pTexture;
   fullWindow->AddChild(tBox);

   fullWindow->Arrange();

   // ********** finished building UIRects



	return(0);
}

//******************************************************************
int RobotMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;

//   SAFE_RELEASE(wallArt);
	vdMan->DeleteObject(uiBackground);
//	vdMan->DeleteObject(backArt);
	vdMan->DeleteObject(mouseArt);

	return(0);
}

//******************************************************************
int RobotMode::Tick(void)
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

   fullWindow->Draw();

	puma->DrawMouse(mouseArt->pTexture);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long RobotMode::WindowServicer(void)
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
void RobotMode::HandleMessages(void)
{
	Robot *r = (Robot *) robotList.First();
	while (r)
	{
		r->Tick();

		r = (Robot *) robotList.Next();
	}
}



/* end of file */



