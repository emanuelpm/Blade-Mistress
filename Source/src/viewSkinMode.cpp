
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "./puma/pumaanim.h"
#include "viewSkinmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragBar.h"
#include "./puma/UIR_EditLine.h"

#include "clientOnly.h"


//***************************************************************************************
D3DCOLOR GetPixel(_D3DFORMAT fmt, char *line, int x)
{
	D3DCOLOR pixel, *pixelPtr;
	char *p;
	unsigned short *s;
	unsigned char r,g,b;

	switch (fmt)
	{
	case D3DFMT_A8R8G8B8:
		pixelPtr = (D3DCOLOR *)line;
		return pixelPtr[x];
		break;

	case D3DFMT_R8G8B8  :
		p = line + 3 * x;
		pixel = D3DCOLOR_ARGB(255, p[2],p[1],p[0]);
		return pixel;
		break;

   case D3DFMT_X8R8G8B8:
		p = line + 4 * x;
		pixel = D3DCOLOR_ARGB(255, p[2],p[1],p[0]);
		return pixel;
		break;

   case D3DFMT_R5G6B5  :
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>11) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x002f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 64.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_X1R5G5B5:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>10) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x001f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 32.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_A1R5G5B5:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>10) & 0x001f;
		g = (unsigned char) ((*s)>>5)  & 0x001f;
		b = (unsigned char) ((*s))     & 0x001f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 32.0f * 255.0f), 
			                   (int)(g / 32.0f * 255.0f), (int)(b / 32.0f * 255.0f));
		return pixel;
		break;

   case D3DFMT_A4R4G4B4:
		p = line + 2 * x;
		s = (unsigned short *) p;
		r = (unsigned char) ((*s)>>8)  & 0x000f;
		g = (unsigned char) ((*s)>>4)  & 0x000f;
		b = (unsigned char) ((*s))     & 0x000f;
		pixel = D3DCOLOR_ARGB(255, (int)(r / 16.0f * 255.0f), 
			                   (int)(g / 16.0f * 255.0f), (int)(b / 16.0f * 255.0f));
		return pixel;
		break;
	}

	return 0;
}

//***************************************************************************************
void SetPixel(D3DCOLOR color, _D3DFORMAT fmt, char *line, int x)
{
	D3DCOLOR *pixelPtr;
	unsigned char *p, *p2;
	unsigned char r,g,b;
	unsigned short *s;
	unsigned char a;
	float rf, gf, bf;

	switch (fmt)
	{
	case D3DFMT_A8R8G8B8:
		pixelPtr = (D3DCOLOR *)line;
		pixelPtr[x] = color;
		break;

	case D3DFMT_R8G8B8  :
		p = (unsigned char *) line + 3 * x;
		p2 = (unsigned char *) &color;
		p[2] = p2[2];
		p[1] = p2[1];
		p[0] = p2[0];
		break;

   case D3DFMT_X8R8G8B8:
		p = (unsigned char *) line + 4 * x;
		p2 = (unsigned char *) &color;
		p[3] = 255;
		p[2] = p2[2];
		p[1] = p2[1];
		p[0] = p2[0];
		break;

   case D3DFMT_R5G6B5  :
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x2f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 11) + (g << 5) + (b);
		break;

   case D3DFMT_X1R5G5B5:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x1f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 10) + (g << 5) + (b);
		break;

   case D3DFMT_A1R5G5B5:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x1f;
		gf = (float) p2[1] / 255.0f * 0x1f;
		bf = (float) p2[0] / 255.0f * 0x1f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 10) + (g << 5) + (b);
		break;

   case D3DFMT_A4R4G4B4:
		p = (unsigned char *) line + 2 * x;
		s = (unsigned short *) p;
		p2 = (unsigned char *) &color;
		a = 255;
		rf = (float) p2[2] / 255.0f * 0x0f;
		gf = (float) p2[1] / 255.0f * 0x0f;
		bf = (float) p2[0] / 255.0f * 0x0f;
		r = (unsigned char) rf;
		g = (unsigned char) gf;
		b = (unsigned char) bf;
		*s = (r << 8) + (g << 4) + (b);
		break;
	}

}

enum 
{
	VSM_BUTTON_MESH,
	VSM_BUTTON_TEXTURE,
	VSM_BUTTON_RELOAD,
	VSM_BUTTON_ANIM_TOGGLE,
	VSM_BUTTON_FLASH_SIZE,

	VSM_BUTTON_SCALE_UP,
	VSM_BUTTON_SCALE_DOWN,

	VSM_BUTTON_QUIT,
	VSM_BUTTON_DESCRIPTION,
	VSM_BUTTON_COORDS,
	VSM_BUTTON_TEXT
};


// static pointer that the process function can use
ViewSkinMode *curViewSkinMode = NULL;

char lastLoadedTextureName[1028];


// process function for UIRects in this mode
int FAR PASCAL ViewSkinModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	char tempText[1028]; 
   UIRectTextButton *tButt;
   OPENFILENAME ofn;
//	MessAvatarRequestStats request;

	if (UIRECT_MOUSE_TEST_POS == type)
	{
	   UIRectTextBox *tBox;

		tBox = (UIRectTextBox *) 
			    curViewSkinMode->fullWindow->childRectList.Find(VSM_BUTTON_DESCRIPTION);
		if (!tBox)
			return 0;

		switch(curUIRect->WhatAmI())	
		{
		case VSM_BUTTON_QUIT:
			tBox->SetText("Click here to quit.");
			break;
		}

		return 1;
	}
								 
	switch(curUIRect->WhatAmI())	
	{
	case VSM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
	      PostQuitMessage( 0 );
			return 1;
      }
		break;

	case VSM_BUTTON_ANIM_TOGGLE :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			if (curViewSkinMode->animateFlag)
				curViewSkinMode->animateFlag = FALSE;
			else
				curViewSkinMode->animateFlag = TRUE;
			return 1;
      }
		break;

	case VSM_BUTTON_FLASH_SIZE :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			tButt = (UIRectTextButton *) 
			    curViewSkinMode->fullWindow->childRectList.Find(VSM_BUTTON_FLASH_SIZE);
			++(curViewSkinMode->flashSize);
			if (curViewSkinMode->flashSize > 4)
				curViewSkinMode->flashSize = 1;
			sprintf(tempText,"Flash Size: %d",curViewSkinMode->flashSize);
			tButt->SetText(tempText);
			return 1;
      }
		break;

	case VSM_BUTTON_SCALE_UP :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			if (curViewSkinMode->anim)
				curViewSkinMode->anim->Scale(puma->m_pd3dDevice, 1.3f, 1.3f, 1.3f);
			return 1;
      }
		break;

	case VSM_BUTTON_SCALE_DOWN :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			if (curViewSkinMode->anim)
				curViewSkinMode->anim->Scale(puma->m_pd3dDevice, 0.7f, 0.7f, 0.7f);
			return 1;
      }
		break;

	case VSM_BUTTON_TEXTURE :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ShowCursor(TRUE);

			ofn.lStructSize = sizeof(OPENFILENAME);
			tempText[0] = 0;
			ofn.lpstrFile= tempText;
			ofn.nMaxFile = sizeof(tempText);
			ofn.lpstrFilter = "PNG files\0*.png\0\0";
			ofn.hwndOwner = (HWND)puma->m_hWnd;
//			ofn.Flags = OFN_NOCHANGEDIR;

   		if (GetOpenFileName(&ofn))
			{
				if (curViewSkinMode->texture)
				{
				   SAFE_RELEASE(curViewSkinMode->texture);
				   SAFE_RELEASE(curViewSkinMode->modelTexture);
				}

				D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, tempText,
						0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
						D3DX_DEFAULT, 0x000000ff, NULL, NULL, &curViewSkinMode->texture);

				D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, tempText,
						0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
						D3DX_DEFAULT, 0x000000ff, NULL, NULL, &curViewSkinMode->modelTexture);

				sprintf(lastLoadedTextureName,tempText);
				curViewSkinMode->oldColorX = -1000;


			}
      
			ShowCursor(FALSE);



			return 1;
      }
		break;

	case VSM_BUTTON_RELOAD :
      if (UIRECT_MOUSE_LUP == type && lastLoadedTextureName[0])
      {
//			button1Sound->PlayNo3D();
			if (curViewSkinMode->texture)
			{
			   SAFE_RELEASE(curViewSkinMode->texture);
			   SAFE_RELEASE(curViewSkinMode->modelTexture);
			}

			D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, lastLoadedTextureName,
					0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
					D3DX_DEFAULT, 0x000000ff, NULL, NULL, &curViewSkinMode->texture);

			D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, lastLoadedTextureName,
					0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
					D3DX_DEFAULT, 0x000000ff, NULL, NULL, &curViewSkinMode->modelTexture);

			return 1;
      }
		break;

	case VSM_BUTTON_MESH :
      if (UIRECT_MOUSE_LUP == type)
      {
//			button1Sound->PlayNo3D();
			memset(&ofn, 0, sizeof(OPENFILENAME));
			ShowCursor(TRUE);

			ofn.lStructSize = sizeof(OPENFILENAME);
			tempText[0] = 0;
			ofn.lpstrFile= tempText;
			ofn.nMaxFile = sizeof(tempText);
			ofn.lpstrFilter = "PUMA anim files\0*.anc\0\0";
			ofn.hwndOwner = (HWND)puma->m_hWnd;
//			ofn.Flags = OFN_NOCHANGEDIR;

   		if (GetOpenFileName(&ofn))
			{
				if (curViewSkinMode->anim)
				{
					delete curViewSkinMode->anim;
				}

				curViewSkinMode->anim = new PumaAnim(0,"AVATAR");
				curViewSkinMode->anim->LoadCompressed(puma->m_pd3dDevice, tempText);
			}
      
			ShowCursor(FALSE);



			return 1;
      }
		break;



	}
	return 0;  // didn't deal with this message
}



//******************************************************************
ViewSkinMode::ViewSkinMode(int doid, char *doname) : GameMode(doid,doname)
{
	curViewSkinMode = this;
	animCounter = 0;
	animateFlag = TRUE;
	flashCounter = 0;
	flashSize = 1;
	mouseDrag = FALSE;
	mouseDrag2 = FALSE;

	texture = modelTexture = NULL;
	lastLoadedTextureName[0] = 0;
	oldColorX = -1000;

	meshX = 50;
	meshY = 0;
}

//******************************************************************
ViewSkinMode::~ViewSkinMode()
{
}

//******************************************************************
int ViewSkinMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int ViewSkinMode::Activate(void) // do this when the mode becomes the forground mode.
{
	aLog.Log("ViewSkinMode::Activate ********************\n");

	animSpin = 0;
	animPitch = 0;

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

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

   // ********** start building UIRects
   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(VSM_BUTTON_TEXTURE, 2, puma->ScreenH() - 20, 100, puma->ScreenH() - 1);
   tButt->SetText("Load Texture");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

	UIRectStep(0,-1);
   tButt = new UIRectTextButton(VSM_BUTTON_RELOAD, -2,-2,-2,-2);
   tButt->SetText("Re-load Texture");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;
	UIRectStep(0,-1);
	tBox = new UIRectTextBox(VSM_BUTTON_COORDS, -2,-2,-2,-2);
	tBox->process = ViewSkinModeProcess;
	tBox->SetText("");
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->font = 2;
   fullWindow->AddChild(tBox);

	UIRectStep(1,2);
   tButt = new UIRectTextButton(VSM_BUTTON_MESH, -2,-2,-2,-2);
   tButt->SetText("Load Mesh");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

	UIRectStep(1,0);
   tButt = new UIRectTextButton(VSM_BUTTON_ANIM_TOGGLE, -2,-2,-2,-2);
   tButt->SetText("Toggle Animation");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

	UIRectStep(1,0);
   tButt = new UIRectTextButton(VSM_BUTTON_FLASH_SIZE, -2,-2,-2,-2);
   tButt->SetText("Flash Size: 1");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

   tButt = new UIRectTextButton(VSM_BUTTON_QUIT, puma->ScreenW() - 40, puma->ScreenH() - 20, puma->ScreenW()-1, puma->ScreenH()-1);
   tButt->SetText("Quit");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

	tBox = new UIRectTextBox(VSM_BUTTON_DESCRIPTION, puma->ScreenW()/2 + 20, 100,
		                      puma->ScreenW() - 20, puma->ScreenH() - 140);
	tBox->process = ViewSkinModeProcess;
	tBox->SetText("");
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->font = 2;
   fullWindow->AddChild(tBox);
	UIRectStep(0,1);

   tButt = new UIRectTextButton(VSM_BUTTON_SCALE_UP, puma->ScreenW() - 120, 1, puma->ScreenW() - 1, 21);
   tButt->SetText("Scale Up");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);

	UIRectStep(0,1);
   tButt = new UIRectTextButton(VSM_BUTTON_SCALE_DOWN, -2,-2,-2,-2);
   tButt->SetText("Scale Down");
   tButt->process = ViewSkinModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tButt);



   fullWindow->Arrange();

   // ********** finished building UIRects

	anim = new PumaAnim(0,"AVATAR");
	anim->LoadCompressed(puma->m_pd3dDevice, "SVFiles\\dragon-stand.anc");
//	anim->Scale(puma->m_pd3dDevice, 0.6f, 0.6f, 0.6f);

	return(0);
}

//******************************************************************
int ViewSkinMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
	delete anim;

	delete fullWindow;

   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int ViewSkinMode::Tick(void)
{

   D3DXMATRIX matWorld, mat2, mat3, matTrans;

	if (animateFlag)
		++animCounter;
	++flashCounter;

	WindowServicer();
	// processing

   UIRectTextBox *tBox;
	char tempText[1024];

	tBox = (UIRectTextBox *) 
		    fullWindow->childRectList.Find(VSM_BUTTON_COORDS);
	if (tBox)
	{
		sprintf(tempText,"%03d x %03d", puma->mouseX, puma->mouseY);
		tBox->SetText(tempText);
	}



	//******** put dot on texture
	D3DLOCKED_RECT tLock, srcLock;
	D3DSURFACE_DESC desc;
	if (modelTexture)
	{
		modelTexture->GetLevelDesc(0, &desc);
		modelTexture->LockRect(0, &tLock, NULL, 0);
		texture->LockRect(0, &srcLock, NULL, 0);

		D3DCOLOR green = D3DCOLOR_RGBA(0,255,0,255);
		D3DCOLOR red   = D3DCOLOR_RGBA(255,0,0,255);

		if (puma->mouseX >= 0 && puma->mouseX < desc.Width &&
			 puma->mouseY >= 0 && puma->mouseY < desc.Height)
		{
			if (oldColorX != -1000)
			{
				for (int i = oldColorY - flashSize/2; 
					  i < oldColorY + (flashSize+1)/2; ++i)
				{
					for (int j = oldColorX - flashSize/2; 
						  j < oldColorX + (flashSize+1)/2; ++j)
					{
						if (i >= 0 && j >= 0 && j < desc.Width && i < desc.Height)
						{
							char *dstPixel   = ((char *)tLock.pBits  + tLock.Pitch * i);
							char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch * i);
							D3DCOLOR orgColor = GetPixel(desc.Format, srcPixel, j);
							SetPixel(orgColor, desc.Format, dstPixel, j);
						}
					}
				}
			}

			oldColorX = puma->mouseX;
			oldColorY = puma->mouseY;

			for (int i = puma->mouseY - flashSize/2; 
			     i < puma->mouseY + (flashSize+1)/2; ++i)
			{
				for (int j = puma->mouseX - flashSize/2; 
				     j < puma->mouseX + (flashSize+1)/2; ++j)
				{
					if (i >= 0 && j >= 0 && j < desc.Width && i < desc.Height)
					{
						char *dstPixel   = ((char *)tLock.pBits  + tLock.Pitch * i);

						if (flashCounter & 1)
							SetPixel(red, desc.Format, dstPixel, j);
						else
							SetPixel(green, desc.Format, dstPixel, j);
					}
				}
			}
		}
		else if (oldColorX != -1000)
		{
			for (int i = oldColorY - flashSize/2; 
				  i < oldColorY + (flashSize+1)/2; ++i)
			{
				for (int j = oldColorX - flashSize/2; 
					  j < oldColorX + (flashSize+1)/2; ++j)
				{
					if (i >= 0 && j >= 0 && j < desc.Width && i < desc.Height)
					{
						char *dstPixel   = ((char *)tLock.pBits  + tLock.Pitch * i);
						char *srcPixel   = ((char *)srcLock.pBits  + srcLock.Pitch * i);
						D3DCOLOR orgColor = GetPixel(desc.Format, srcPixel, j);
						SetPixel(orgColor, desc.Format, dstPixel, j);
					}
				}
			}
			oldColorX = -1000;
		}

		texture->UnlockRect(0);
		modelTexture->UnlockRect(0);
	}
	//****** end putting dot on texture



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

	// Turn on the zbuffer
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );

	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

//	animSpin += 0.02f;

	D3DXMatrixIdentity( &matWorld );

	D3DXMatrixRotationX( &mat2, animPitch);
	D3DXMatrixRotationY( &matWorld, animSpin);
	D3DXMatrixMultiply( &matWorld, &matWorld, &mat2 );

//	D3DXMatrixRotationY( &matWorld, animSpin); //camAngle[1]);
	matWorld._41 = meshX;
	matWorld._42 = meshY;
	matWorld._43 = -30.0f;
//	matWorld._41 = 50.0f;
//	matWorld._42 = 0.0f;
//	matWorld._43 = -30.0f;
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if (texture)
	{
		D3DSURFACE_DESC desc;
		texture->GetLevelDesc(0, &desc);
		RECT dRect = {0,0,desc.Width,desc.Height};
		puma->DrawRect(texture,dRect,0xffffffff);
	}

	if (modelTexture)
		puma->m_pd3dDevice->SetTexture( 0, modelTexture );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	anim->Draw(puma->m_pd3dDevice, (animCounter/5) % anim->numOfFrames);
/*
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	D3DSURFACE_DESC desc;
	backArt->GetLevelDesc(0, &desc);

	RECT dRect = {0,0,desc.Width,desc.Height};
	puma->DrawRect(backArt,dRect,0xffffffff);

	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
*/

   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long ViewSkinMode::WindowServicer(void)
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
			mouseDrag = TRUE;
			mouseStartX = pie->mouseX;
			mouseStartY = pie->mouseY;
			break;

		case PUMA_INPUT_LMBUP:
		   fullWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			mouseDrag = FALSE;
			break;

		case PUMA_INPUT_RMBDOWN:
		   fullWindow->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
			mouseDrag2 = TRUE;
			mouseStartX = pie->mouseX;
			mouseStartY = pie->mouseY;
			break;
		case PUMA_INPUT_RMBUP:
		   fullWindow->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
			mouseDrag2 = FALSE;
			break;
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

   fullWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);
	if (mouseDrag)
	{
		animSpin += (mouseStartX - puma->mouseX) * 0.01f;
		animPitch += (mouseStartY - puma->mouseY) * 0.01f;
		if (animPitch > D3DX_PI/3)
			animPitch = D3DX_PI/3;
		if (animPitch < -D3DX_PI/3)
			animPitch = -D3DX_PI/3;

		mouseStartX = puma->mouseX;
		mouseStartY = puma->mouseY;
	}
	else if (mouseDrag2)
	{
		meshX -= (mouseStartX - puma->mouseX) * 0.1f;
		meshY += (mouseStartY - puma->mouseY) * 0.1f;

		mouseStartX = puma->mouseX;
		mouseStartY = puma->mouseY;
	}

   fullWindow->Action(UIRECT_MOUSE_TEST_POS, puma->mouseX, puma->mouseY);
	return 0;
}



/* end of file */



