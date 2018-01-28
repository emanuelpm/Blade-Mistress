
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "./puma/pumaanim.h"
#include "designCharmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragBar.h"
#include "./puma/UIR_EditLine.h"

#include "clientOnly.h"

#include ".\network\NetWorldMessages.h"
#include "simpleMessageMode.h"
#include "groundTestMode.h"

#include "BBOServer.h"
#include ".\network\client.h"
#include "avatarTexture.h"
#include "particle2.h"

enum 
{
	DCM_BUTTON_SUBMIT,
	DCM_BUTTON_NAME,

	DCM_BUTTON_NEXT_FACE,
	DCM_BUTTON_HAIR_RED,
	DCM_BUTTON_HAIR_GREEN,
	DCM_BUTTON_HAIR_BLUE,

	DCM_BUTTON_NEXT_TOP,
	DCM_BUTTON_TOP_RED,
	DCM_BUTTON_TOP_GREEN,
	DCM_BUTTON_TOP_BLUE,

	DCM_BUTTON_NEXT_BOTTOM,
	DCM_BUTTON_BOTTOM_RED,
	DCM_BUTTON_BOTTOM_GREEN,
	DCM_BUTTON_BOTTOM_BLUE,

	DCM_BUTTON_QUIT,

	DCM_BUTTON_PHYSICAL,
	DCM_BUTTON_MAGICAL,
	DCM_BUTTON_CREATIVE,
	DCM_BUTTON_EXTRA,

	DCM_BUTTON_PHYSICAL_MORE,
	DCM_BUTTON_MAGICAL_MORE,
	DCM_BUTTON_CREATIVE_MORE,

	DCM_BUTTON_PHYSICAL_LESS,
	DCM_BUTTON_MAGICAL_LESS,
	DCM_BUTTON_CREATIVE_LESS,

	DCM_BUTTON_DESCRIPTION,
	DCM_BUTTON_TEXT
};

const int CLOTHES_CHANGE_SHIFT = 150;

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

//statsToChange tempStatChanges;

// static pointer that the process function can use
DesignCharMode *curDesignCharMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL DesignCharModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	UIRectEditLine *edName;
	UIRectDragBar	  *dragBar;
//	MessAvatarRequestStats request;

	if (UIRECT_MOUSE_TEST_POS == type)
	{
	   UIRectTextBox *tBox;

		tBox = (UIRectTextBox *) 
			    curDesignCharMode->fullWindow->childRectList.Find(DCM_BUTTON_DESCRIPTION);
		if (!tBox)
			return 0;

		switch(curUIRect->WhatAmI())	
		{
		case DCM_BUTTON_SUBMIT:
			if (0 == curDesignCharMode->WhatAmI())
				tBox->SetText("Click here when you are done\nmaking your new character.");
			else
				tBox->SetText("Click here when you are done\nchanging the clothes of\nyour character.");
			break;
		case DCM_BUTTON_NAME:
			tBox->SetText("Type in the name\nof your new character here.");
			break;

		case DCM_BUTTON_NEXT_FACE:
			tBox->SetText("Click here to see the next\nface on your new character.");
			break;
		case DCM_BUTTON_HAIR_RED:
			tBox->SetText("Click and drag this slider bar\nto change how much red is\nin your character's hair.");
			break;
		case DCM_BUTTON_HAIR_GREEN:
			tBox->SetText("Click and drag this slider bar\nto change how much green is\nin your character's hair.");
			break;
		case DCM_BUTTON_HAIR_BLUE:
			tBox->SetText("Click and drag this slider bar\nto change how much blue is\nin your character's hair.");
			break;

		case DCM_BUTTON_NEXT_TOP:
			tBox->SetText("Click here to see the next\nshirt on your new character.");
			break;
		case DCM_BUTTON_TOP_RED:
			tBox->SetText("Click and drag this slider bar\nto change how much red is\nin your character's shirt.");
			break;
		case DCM_BUTTON_TOP_GREEN:
			tBox->SetText("Click and drag this slider bar\nto change how much green is\nin your character's shirt.");
			break;
		case DCM_BUTTON_TOP_BLUE:
			tBox->SetText("Click and drag this slider bar\nto change how much blue is\nin your character's shirt.");
			break;

		case DCM_BUTTON_NEXT_BOTTOM:
			tBox->SetText("Click here to see the next\nleggings on your new character.");
			break;
		case DCM_BUTTON_BOTTOM_RED:
			tBox->SetText("Click and drag this slider bar\nto change how much red is\nin your character's leggings.");
			break;
		case DCM_BUTTON_BOTTOM_GREEN:
			tBox->SetText("Click and drag this slider bar\nto change how much green is\nin your character's leggings.");
			break;
		case DCM_BUTTON_BOTTOM_BLUE:
			tBox->SetText("Click and drag this slider bar\nto change how much blue is\nin your character's leggings.");
			break;

		case DCM_BUTTON_QUIT:
			tBox->SetText("Click here if you don't want\nto make a new character after all.");
			break;


		case DCM_BUTTON_PHYSICAL:
			tBox->SetText("This is your current Physical\nrating.  This number defines how\nmuch damage you can take, and\nhow much you do to monsters.");
			break;

		case DCM_BUTTON_MAGICAL:
			tBox->SetText("This is your current Magical\nrating.  This number defines how\nwell you can create magical items,\nand what kinds of pets you can have.");
			break;

		case DCM_BUTTON_CREATIVE:
			tBox->SetText("This is your current Creative\nrating.  This number defines how\nwell you can craft things\nlike swords and explosives.");
			break;

		case DCM_BUTTON_EXTRA:
			tBox->SetText("These are your unassigned rating points.\n\nBe sure to add all of your points\nto your Physical, Magical,\nand Creative ratings.");
			break;


		case DCM_BUTTON_CREATIVE_MORE:
			tBox->SetText("Click to add one free point\nto your Creative rating.\n\nCharacters with high Creative can\nmake better swords, explosives,\nand other items.");
			break;

		case DCM_BUTTON_PHYSICAL_MORE:
			tBox->SetText("Click to add one free point\nto your Physical rating.\n\nCharacters with high Physical can\nmake short work of tough monsters\nand thus gather the most loot.");
			break;

		case DCM_BUTTON_MAGICAL_MORE:
			tBox->SetText("Click to add one free point\nto your Magical rating.\n\nCharacters with high Magical can\nmake better magic totems, and tame\nmore powerful pets.\n(less than 4, and you\ncan't have ANY pets.)");
			break;


		case DCM_BUTTON_PHYSICAL_LESS:
			tBox->SetText("Click to remove one point\nfrom your Physical rating.\n\nCharacters with low Physical\nhave a hard time battling monsters,\nand need to focus on making and\nselling things to make money.");
			break;

		case DCM_BUTTON_MAGICAL_LESS:
			tBox->SetText("Click to remove one point\nfrom your Magical rating.\n\nCharacters with low Magical\nhave a hard time making magical items,\nand cannot have powerful pets.\nIn fact, if Magical is below 4,\nthis character cannot have ANY pets.");
			break;

		case DCM_BUTTON_CREATIVE_LESS:
			tBox->SetText("Click to remove one point\nfrom your Creative rating.\n\nCharacters with low Creative\nhave a hard time making\nswords and explosives.");
			break;

		}

		return 1;
	}

	switch(curUIRect->WhatAmI())	
	{
	case DCM_BUTTON_SUBMIT :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();

			if (0 == curDesignCharMode->WhatAmI())
			{
				edName = (UIRectEditLine *) curDesignCharMode->fullWindow->childRectList.Find(DCM_BUTTON_NAME);
				CorrectString(edName->text);
				sprintf(curDesignCharMode->statsToChange->name, edName->text);
			}
			else
				curDesignCharMode->statsToChange->faceIndex = 255;  // signal for clothes change
			newGameMode = NULL;
			curDesignCharMode->retState = GMR_POP_ME;
      }
		break;
	case DCM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();

			curDesignCharMode->statsToChange->topIndex = 255;  // signal for cancel
			newGameMode = NULL;
			curDesignCharMode->retState = GMR_POP_ME;
      }
		break;

	case DCM_BUTTON_NEXT_FACE :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();

			curDesignCharMode->statsToChange->faceIndex++;
			if (curDesignCharMode->statsToChange->faceIndex >= NUM_OF_FACES)
				curDesignCharMode->statsToChange->faceIndex = 0;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

			curDesignCharMode->animSpin = 0;
      }
		break;

	case DCM_BUTTON_HAIR_RED :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->hairR	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_HAIR_GREEN :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->hairG	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_HAIR_BLUE :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->hairB	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;

	case DCM_BUTTON_NEXT_TOP :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();

			curDesignCharMode->statsToChange->topIndex++;
			if (curDesignCharMode->statsToChange->topIndex >= NUM_OF_TOPS)
				curDesignCharMode->statsToChange->topIndex = 0;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

      }
		break;

	case DCM_BUTTON_TOP_RED :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->topR	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_TOP_GREEN :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->topG	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_TOP_BLUE :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->topB	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;

	case DCM_BUTTON_NEXT_BOTTOM :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();

			curDesignCharMode->statsToChange->bottomIndex++;
			if (curDesignCharMode->statsToChange->bottomIndex >= NUM_OF_BOTTOMS)
				curDesignCharMode->statsToChange->bottomIndex = 0;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

      }
		break;

	case DCM_BUTTON_BOTTOM_RED :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->bottomR	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_BOTTOM_GREEN :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->bottomG	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;
	case DCM_BUTTON_BOTTOM_BLUE :
		if (UIRECT_DONE == type)
		{
			button2Sound->PlayNo3D();

			dragBar = (UIRectDragBar *) curUIRect;
			curDesignCharMode->statsToChange->bottomB	= dragBar->GetXVal() * 220.0f + 35;

			curDesignCharMode->avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

		}
		break;

	case DCM_BUTTON_PHYSICAL_MORE :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_PHYSICAL,1);
		}
		break;
	case DCM_BUTTON_PHYSICAL_LESS :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_PHYSICAL,-1);
		}
		break;

	case DCM_BUTTON_MAGICAL_MORE :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_MAGICAL,1);
		}
		break;
	case DCM_BUTTON_MAGICAL_LESS :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_MAGICAL,-1);
		}
		break;

	case DCM_BUTTON_CREATIVE_MORE :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_CREATIVE,1);
		}
		break;
	case DCM_BUTTON_CREATIVE_LESS :
      if (UIRECT_MOUSE_LUP == type)
		{
			button1Sound->PlayNo3D();
			curDesignCharMode->UpdateStats(DCM_BUTTON_CREATIVE,-1);
		}
		break;




		/*
	case DCM_BUTTON_PREV_CHAR :
      if (UIRECT_MOUSE_LUP == type)
      {
			curDesignCharMode->curCharacterIndex--;
			if (curDesignCharMode->curCharacterIndex < 0)
				curDesignCharMode->curCharacterIndex = NUM_OF_CHARS_PER_USER - 1;

			request.characterIndex = curDesignCharMode->curCharacterIndex;
			lclient->SendMsg(sizeof(request),(void *)&request);
      }
		break;
		  */
	}
	return 0;  // didn't deal with this message
}



//******************************************************************
DesignCharMode::DesignCharMode(MessAvatarStats *s, 
										 int doid, char *doname) : GameMode(doid,doname)
{
	curDesignCharMode = this;
	statsToChange = s;
	animCounter = 0;
}

//******************************************************************
DesignCharMode::~DesignCharMode()
{
}

//******************************************************************
int DesignCharMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int DesignCharMode::Activate(void) // do this when the mode becomes the forground mode.
{
	aLog.Log("DesignCharMode::Activate ********************\n");

	animSpin = 0;
	colorBoxLeft = puma->ScreenW()*0.10f;

	SetEnvironment();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\newLogo1.png",
							0,0,0,0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &backArt);
	
	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\newLogo2.png",
							0,0,0,0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &backArt2);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\URframe.png",
							0,0,0,0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &urFrame);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIstone.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffff00ff, NULL, NULL, &uiArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\CharCrea1.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &controlBack1);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\CharCrea2.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &controlBack2);

	puma->LoadTexture("dat\\cornerButtUp.png"   , &cornerArt[0], 0);
	puma->LoadTexture("dat\\cornerButtDown.png" , &cornerArt[1], 0);
	puma->LoadTexture("dat\\cornerButtHigh.png" , &cornerArt[2], 0);
	puma->LoadTexture("dat\\cornerButt2Up.png"  , &cornerArt[3], 0);
	puma->LoadTexture("dat\\cornerButt2Down.png", &cornerArt[4], 0);
	puma->LoadTexture("dat\\cornerButt2High.png", &cornerArt[5], 0);

	puma->LoadTexture("dat\\CharCreaKnob.png"    , &sliderArt  , 0);
	puma->LoadTexture("dat\\CharCreaStatBack.png", &statBackArt, 0);

	// ********** start building UIRects
   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(DCM_BUTTON_SUBMIT,
		puma->ScreenW()- 155, puma->ScreenH() - 42, 
		puma->ScreenW() - 65 ,puma->ScreenH() - 4);
   tButt->SetText("Submit");
   tButt->process = DesignCharModeProcess;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
//	tButt->fillArt = uiArt;
   fullWindow->AddChild(tButt);

   tButt = new UIRectTextButton(DCM_BUTTON_QUIT, 65, puma->ScreenH() - 42, 155 ,puma->ScreenH() - 4);
   tButt->SetText("Cancel");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tButt->process = DesignCharModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
//	tButt->fillArt = uiArt;
   fullWindow->AddChild(tButt);

	UIRectEditLine *edName;
	if (0 == WhatAmI())
	{
		edName = new UIRectEditLine(DCM_BUTTON_NAME, 
								colorBoxLeft + 84, 15  ,colorBoxLeft + 217, 33,31);
		edName->SetText("");
		edName->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		edName->process = DesignCharModeProcess;
		fullWindow->AddChild(edName);
	}

	int shift = 0;
	if (1 == WhatAmI()) // if just changing clothes
		shift = CLOTHES_CHANGE_SHIFT;

   UIRectTextBox *tBox;

	tBox = new UIRectTextBox(DCM_BUTTON_DESCRIPTION, puma->ScreenW()/2 + 20, 100,
		                      puma->ScreenW() - 20, puma->ScreenH() - 140);
	tBox->process = DesignCharModeProcess;
	tBox->SetText("");
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->fillArt = NULL;
	tBox->font = 2;
   fullWindow->AddChild(tBox);
	UIRectStep(0,1);

	UIRectArtTextButton *tArtButt;
	UIRectDragBar *dBar;

	if (0 == WhatAmI())
	{
		// ********************** face and hair selections
		tArtButt = new UIRectArtTextButton(DCM_BUTTON_NEXT_FACE, 
								colorBoxLeft+0, 43  ,colorBoxLeft + 246, 43+35);
		tArtButt->SetText("Next Face");
		tArtButt->process = DesignCharModeProcess;
		tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tArtButt->SetArt(UIRECT_BCOL_ACTIVE,"dat\\CharCreaHigh.png",0);
		tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharCreaDown.png",0);
		fullWindow->AddChild(tArtButt);

		dBar = new UIRectDragBar(DCM_BUTTON_HAIR_RED, colorBoxLeft + 49, 84  ,colorBoxLeft + 244, 113);
		dBar->process = DesignCharModeProcess;
		dBar->isXFree = TRUE;
		dBar->xWidth  = 29;
		dBar->SetXVal(0.5f);
		dBar->fillArt = sliderArt;
		statsToChange->hairR	= dBar->GetXVal() * 220.0f + 35;
		fullWindow->AddChild(dBar);
		UIRectStep(0,1);

		dBar = new UIRectDragBar(DCM_BUTTON_HAIR_GREEN, -2,-2,-2,-2);
		dBar->process = DesignCharModeProcess;
		dBar->isXFree = TRUE;
		dBar->xWidth  = 29;
		dBar->SetXVal(0.4f);
		dBar->fillArt = sliderArt;
		statsToChange->hairG	= dBar->GetXVal() * 220.0f + 35;
		fullWindow->AddChild(dBar);
		UIRectStep(0,1);

		dBar = new UIRectDragBar(DCM_BUTTON_HAIR_BLUE, -2,-2,-2,-2);
		dBar->process = DesignCharModeProcess;
		dBar->isXFree = TRUE;
		dBar->xWidth  = 29;
		dBar->SetXVal(0.2f);
		dBar->fillArt = sliderArt;
		statsToChange->hairB	= dBar->GetXVal() * 220.0f + 35;
		fullWindow->AddChild(dBar);

		UIRectStep(0,2);
	}

	// ********************** top selections
   tArtButt = new UIRectArtTextButton(DCM_BUTTON_NEXT_TOP, 
	                  colorBoxLeft+0, 191 -shift ,colorBoxLeft + 246, 191+35-shift);
   tArtButt->SetText("Next Shirt");
   tArtButt->process = DesignCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,"dat\\CharCreaHigh.png",0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharCreaDown.png",0);
   fullWindow->AddChild(tArtButt);

	float inVal;

	dBar = new UIRectDragBar(DCM_BUTTON_TOP_RED, colorBoxLeft + 49, 232-shift ,
		                      colorBoxLeft + 244, 260-shift);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->topR - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);

	UIRectStep(0,1);

	dBar = new UIRectDragBar(DCM_BUTTON_TOP_GREEN, -2,-2,-2,-2);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->topG - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);
	UIRectStep(0,1);

	dBar = new UIRectDragBar(DCM_BUTTON_TOP_BLUE, -2,-2,-2,-2);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->topB - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);

	UIRectStep(0,2);


	// ********************** bottom selections
   tArtButt = new UIRectArtTextButton(DCM_BUTTON_NEXT_BOTTOM, 
	                  colorBoxLeft+0, 337 -shift ,colorBoxLeft + 246, 337+35-shift);
   tArtButt->SetText("Next Pants");
   tArtButt->process = DesignCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,"dat\\CharCreaHigh.png",0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharCreaDown.png",0);
   fullWindow->AddChild(tArtButt);

	dBar = new UIRectDragBar(DCM_BUTTON_BOTTOM_RED, colorBoxLeft + 49, 377 -shift ,
		                      colorBoxLeft + 244, 406-shift);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->bottomR - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);
	UIRectStep(0,1);

	dBar = new UIRectDragBar(DCM_BUTTON_BOTTOM_GREEN, -2,-2,-2,-2);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->bottomG - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);
	UIRectStep(0,1);

	dBar = new UIRectDragBar(DCM_BUTTON_BOTTOM_BLUE, -2,-2,-2,-2);
	dBar->process = DesignCharModeProcess;
	dBar->isXFree = TRUE;
	dBar->xWidth  = 29;
	dBar->fillArt = sliderArt;
	inVal = statsToChange->bottomB - 35;
	if (inVal < 0)
		inVal = 0;
	dBar->SetXVal(inVal / 220.0f);
   fullWindow->AddChild(dBar);
//	UIRectStep(1,-2);

	float statYCoeff = 0.60f;

	if (0 == WhatAmI())
	{
		// statistics
		int statBaseY = puma->ScreenH() - 140;
		//********
		tBox = new UIRectTextBox(DCM_BUTTON_TEXT, puma->ScreenW()*statYCoeff, statBaseY + 0  ,puma->ScreenW()*statYCoeff + 100, statBaseY + 20);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("Physical");
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_PHYSICAL_LESS, 
								puma->ScreenW()*statYCoeff + 100, statBaseY + 0  ,puma->ScreenW()*statYCoeff + 125, statBaseY + 20);
		tButt->SetText("<");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		tBox = new UIRectTextBox(DCM_BUTTON_PHYSICAL, puma->ScreenW()*statYCoeff + 125, statBaseY + 0  ,puma->ScreenW()*statYCoeff + 175, statBaseY + 20);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("0");
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_PHYSICAL_MORE, 
								puma->ScreenW()*statYCoeff + 175, statBaseY + 0  ,puma->ScreenW()*statYCoeff + 200, statBaseY + 20);
		tButt->SetText(">");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		//********
		tBox = new UIRectTextBox(DCM_BUTTON_TEXT, puma->ScreenW()*statYCoeff, statBaseY + 20  ,puma->ScreenW()*statYCoeff + 100, statBaseY + 40);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("Magical");
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_MAGICAL_LESS, 
								puma->ScreenW()*statYCoeff + 100, statBaseY + 20  ,puma->ScreenW()*statYCoeff + 125, statBaseY + 40);
		tButt->SetText("<");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		tBox = new UIRectTextBox(DCM_BUTTON_MAGICAL, puma->ScreenW()*statYCoeff + 125, statBaseY + 20  ,puma->ScreenW()*statYCoeff + 175, statBaseY + 40);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("0");
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_MAGICAL_MORE, 
								puma->ScreenW()*statYCoeff + 175, statBaseY + 20  ,puma->ScreenW()*statYCoeff + 200, statBaseY + 40);
		tButt->SetText(">");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		//********
		tBox = new UIRectTextBox(DCM_BUTTON_TEXT, puma->ScreenW()*statYCoeff, statBaseY + 40  ,puma->ScreenW()*statYCoeff + 100, statBaseY + 60);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("Creative");
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_CREATIVE_LESS, 
								puma->ScreenW()*statYCoeff + 100, statBaseY + 40  ,puma->ScreenW()*statYCoeff + 125, statBaseY + 60);
		tButt->SetText("<");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		tBox = new UIRectTextBox(DCM_BUTTON_CREATIVE, puma->ScreenW()*statYCoeff + 125, statBaseY + 40  ,puma->ScreenW()*statYCoeff + 175, statBaseY + 60);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("0");
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		tButt = new UIRectTextButton(DCM_BUTTON_CREATIVE_MORE, 
								puma->ScreenW()*statYCoeff + 175, statBaseY + 40  ,puma->ScreenW()*statYCoeff + 200, statBaseY + 60);
		tButt->SetText(">");
		tButt->process = DesignCharModeProcess;
		tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tButt->fillArt = uiArt;
		fullWindow->AddChild(tButt);

		//********
		tBox = new UIRectTextBox(DCM_BUTTON_TEXT, puma->ScreenW()*statYCoeff, statBaseY + 60  ,puma->ScreenW()*statYCoeff + 100, statBaseY + 80);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("Unassigned");
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		tBox = new UIRectTextBox(DCM_BUTTON_EXTRA, puma->ScreenW()*statYCoeff + 125, statBaseY + 60  ,puma->ScreenW()*statYCoeff + 175, statBaseY + 80);
		tBox->process = DesignCharModeProcess;
		tBox->SetText("0");
		tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	//	tBox->fillArt = uiArt;
		fullWindow->AddChild(tBox);

		UpdateStats(0,0);
	}
//	serverListWindow->AddChild(dragBar);

   fullWindow->Arrange();

   // ********** finished building UIRects

	anim = new PumaAnim(0,"AVATAR");
	anim->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-create.anc");
	anim->Scale( puma->m_pd3dDevice, 60.0f, 60.0f, 60.0f );
//	anim->Scale(puma->m_pd3dDevice, 0.6f, 0.6f, 0.6f);

	avTexture = new AvatarTexture();

	avTexture->Generate(puma->m_pd3dDevice,
				curDesignCharMode->statsToChange->faceIndex,
				curDesignCharMode->statsToChange->hairR, 
				curDesignCharMode->statsToChange->hairG, 
				curDesignCharMode->statsToChange->hairB,

				curDesignCharMode->statsToChange->topIndex,
				curDesignCharMode->statsToChange->topR, 
				curDesignCharMode->statsToChange->topG, 
				curDesignCharMode->statsToChange->topB,

				curDesignCharMode->statsToChange->bottomIndex,
				curDesignCharMode->statsToChange->bottomR, 
				curDesignCharMode->statsToChange->bottomG, 
				curDesignCharMode->statsToChange->bottomB,0);

	return(0);
}

//******************************************************************
int DesignCharMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
	delete avTexture;
	delete anim;

	delete fullWindow;

	for (int i = 0; i < 6; ++i)
	   SAFE_RELEASE(cornerArt[i]);

	SAFE_RELEASE(statBackArt);
   SAFE_RELEASE(sliderArt);

   SAFE_RELEASE(controlBack2);
   SAFE_RELEASE(controlBack1);
   SAFE_RELEASE(uiArt);
   SAFE_RELEASE(backArt);
   SAFE_RELEASE(backArt2);
   SAFE_RELEASE(urFrame);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int DesignCharMode::Tick(void)
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

	++animCounter;

	UIRectTextButton *tButt = (UIRectTextButton *) 
		 fullWindow->childRectList.Find(DCM_BUTTON_SUBMIT);
	UIRectEditLine *edName = (UIRectEditLine *) 
		 fullWindow->childRectList.Find(DCM_BUTTON_NAME);

	long total = 
		statsToChange->physical +
		statsToChange->magical +
		statsToChange->creative;

	if (edName && edName->text[0] && 12 == total)
	{
		tButt->isDisabled = FALSE;
	}
	else if (edName)
	{
		tButt->isDisabled = TRUE;
	}
	else
		tButt->isDisabled = FALSE;

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

	animSpin += 0.02f;

	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY( &matWorld, animSpin); //camAngle[1]);
	matWorld._41 = 50.0f;
	matWorld._42 = 3.0f;
	matWorld._43 = -30.0f;
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	puma->m_pd3dDevice->SetTexture( 0, avTexture->currentBitmap );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	anim->Draw(puma->m_pd3dDevice, (animCounter/5) % anim->numOfFrames);

	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	D3DSURFACE_DESC desc;
//	backArt2->GetLevelDesc(0, &desc);

//	RECT dRect2 = {255,0,255+desc.Width,desc.Height};
//	puma->DrawRect(backArt2,dRect2,0xffffffff);

//	backArt->GetLevelDesc(0, &desc);

//	RECT dRect = {0,0,desc.Width,desc.Height};
//	puma->DrawRect(backArt,dRect,0xffffffff);

	urFrame->GetLevelDesc(0, &desc);

	RECT dRect3 = {puma->ScreenW() - desc.Width, 0, puma->ScreenW(), desc.Height};
	puma->DrawRect(urFrame,dRect3,0xffffffff);

	if (0 == WhatAmI())
	{
		controlBack1->GetLevelDesc(0, &desc);
		RECT dRect4 = {colorBoxLeft, 0  ,
		            colorBoxLeft + desc.Width-1, 0 + desc.Height-1};
		puma->DrawRect(controlBack1,dRect4,0xffffffff);

		controlBack2->GetLevelDesc(0, &desc);
		RECT dRect5 = {colorBoxLeft, 189  ,
							colorBoxLeft + desc.Width-1, 189 + desc.Height-1};
		puma->DrawRect(controlBack2,dRect5,0xffffffff);

		RECT dRect6 = {colorBoxLeft, 336  ,
							colorBoxLeft + desc.Width-1, 336 + desc.Height-1};
		puma->DrawRect(controlBack2,dRect6,0xffffffff);
	}
	else
	{
		controlBack2->GetLevelDesc(0, &desc);
		RECT dRect5 = {colorBoxLeft, 189 - CLOTHES_CHANGE_SHIFT ,
							colorBoxLeft + desc.Width-1, 189 + desc.Height-1 - CLOTHES_CHANGE_SHIFT};
		puma->DrawRect(controlBack2,dRect5,0xffffffff);

		RECT dRect6 = {colorBoxLeft, 336  - CLOTHES_CHANGE_SHIFT ,
							colorBoxLeft + desc.Width-1, 336 + desc.Height-1 - CLOTHES_CHANGE_SHIFT};
		puma->DrawRect(controlBack2,dRect6,0xffffffff);
	}


	/*
	float statYCoeff = 0.60f;

	// statistics
	int statBaseY = puma->ScreenH() - 140;
	//********
	tBox = new UIRectTextBox(DCM_BUTTON_TEXT, puma->ScreenW()*statYCoeff, statBaseY + 0  ,puma->ScreenW()*statYCoeff + 100, statBaseY + 20);
	*/
	if (0 == WhatAmI())
	{
		int backX = puma->ScreenW()*0.60f - 10;
		int backY = puma->ScreenH() - 150;
		statBackArt->GetLevelDesc(0, &desc);
		RECT dRect7 = {backX, backY, backX + desc.Width-1, backY + desc.Height-1};
		puma->DrawRect(statBackArt,dRect7,0xffffffff);
	}

	int bIndex = 0;
   tButt = (UIRectTextButton *) fullWindow->childRectList.Find(DCM_BUTTON_QUIT);
	if (tButt)
	{
		if (tButt->isDepressed)
			bIndex = 4;
		else if (tButt->isActive)
			bIndex = 5;
		else
			bIndex = 3;
		cornerArt[bIndex]->GetLevelDesc(0, &desc);
		RECT cRect = {0,puma->ScreenH()-127,255,puma->ScreenH()};
		puma->DrawRect(cornerArt[bIndex],cRect,0xffffffff);
	}

   tButt = (UIRectTextButton *) fullWindow->childRectList.Find(DCM_BUTTON_SUBMIT);
	if (tButt)
	{
		if (tButt->isDepressed)
			bIndex = 1;
		else if (tButt->isActive)
			bIndex = 2;
		else
			bIndex = 0;
		cornerArt[bIndex]->GetLevelDesc(0, &desc);
		RECT cRect = {puma->ScreenW()-255,puma->ScreenH()-127,puma->ScreenW(),puma->ScreenH()};
		puma->DrawRect(cornerArt[bIndex],cRect,0xffffffff);
	}



	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );


   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long DesignCharMode::WindowServicer(void)
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

   fullWindow->Action(UIRECT_MOUSE_TEST_POS, puma->mouseX, puma->mouseY);


    return 0;
}


//***************************************************************
void DesignCharMode::HandleMessages(void)
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
		
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//***************************************************************
void DesignCharMode::UpdateStats(int type, int change)
{
	char tempText[1024];

	long total = 
		statsToChange->physical +
		statsToChange->magical +
		statsToChange->creative;

	// make any changes  ************
	switch (type)
	{
	case DCM_BUTTON_PHYSICAL:
		if (total + change <= 12)
		{
			statsToChange->physical += change;
			if (statsToChange->physical < 1)
				 statsToChange->physical = 1;
		}
		break;

	case DCM_BUTTON_MAGICAL	:
		if (total + change <= 12)
		{
			statsToChange->magical += change;
			if (statsToChange->magical < 1)
				 statsToChange->magical = 1;
		}
		break;

	case DCM_BUTTON_CREATIVE:
		if (total + change <= 12)
		{
			statsToChange->creative += change;
			if (statsToChange->creative < 1)
				 statsToChange->creative = 1;
		}
		break;

	}

	// and update text boxes  ************
	UIRectTextBox *tBox = (UIRectTextBox *) fullWindow->childRectList.Find(DCM_BUTTON_PHYSICAL);
	tBox->process = DesignCharModeProcess;
	sprintf(tempText,"%d",statsToChange->physical);
	tBox->SetText(tempText);

	tBox = (UIRectTextBox *) fullWindow->childRectList.Find(DCM_BUTTON_MAGICAL);
	tBox->process = DesignCharModeProcess;
	sprintf(tempText,"%d",statsToChange->magical);
	tBox->SetText(tempText);

	tBox = (UIRectTextBox *) fullWindow->childRectList.Find(DCM_BUTTON_CREATIVE);
	tBox->process = DesignCharModeProcess;
	sprintf(tempText,"%d",statsToChange->creative);
	tBox->SetText(tempText);

	total = 
		statsToChange->physical +
		statsToChange->magical +
		statsToChange->creative;

	tBox = (UIRectTextBox *) fullWindow->childRectList.Find(DCM_BUTTON_EXTRA);
	tBox->process = DesignCharModeProcess;
	sprintf(tempText,"%d",12 - total);
	tBox->SetText(tempText);
}

//******************************************************************
void DesignCharMode::SetEnvironment(void)
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



