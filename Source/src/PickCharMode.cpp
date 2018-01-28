
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "./puma/pumaanim.h"
#include "pickCharmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragEdge.h"
#include "./puma/UIR_EditLine.h"

#include "./network/NetWorldMessages.h"
#include "simpleMessageMode.h"
#include "instructionMode.h"
#include "groundTestMode.h"
#include "designCharmode.h"

#include "clientOnly.h"

#include "BBOServer.h"
#include "./network/client.h"
#include "avatarTexture.h"
#include "particle2.h"
#include ".\helper\crypto.h"

enum 
{
//	PCM_BUTTON_NEW_NAME,
//	PCM_BUTTON_NEW_CHAR,
	PCM_BUTTON_CONFIRM_YES,
	PCM_BUTTON_CONFIRM2_YES,
	PCM_BUTTON_CONFIRM_NO,
	PCM_BUTTON_ENTER_GAME,
	PCM_BUTTON_QUIT,
	PCM_BUTTON_TEXT,
	PCM_BUTTON_BEGIN_TEXT,
	PCM_BUTTON_DELETE,
	PCM_BUTTON_CHANGE_AV_TEX,
	PCM_BUTTON_TIME_INFO,
	PCM_BUTTON_TIME_EDLINE,
	PCM_BUTTON_CHANGE_CLOTHES,
	PCM_BUTTON_CHAR_0,
	PCM_BUTTON_CHAR_1,
	PCM_BUTTON_CHAR_2,
	PCM_BUTTON_CHAR_3,
	PCM_BUTTON_CHAR_4,
	PCM_BUTTON_CHAR_5,
	PCM_BUTTON_CHAR_6,
	PCM_BUTTON_CHAR_7
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


char *pcmText1 = "Simple avatars OFF";

// static pointer that the process function can use
PickCharMode *curPickCharMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL PickCharModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	char tempText[1280];
//	int hIndex;
//	float fval;
//	long val;  
	UIRectEditLine *edName;
	MessAvatarRequestStats request;
	MessEnterGame enterGame;
	MessAvatarDelete messDelete;
	MessKeyCode messKeyCode;
   UIRectTextButton *tButt;
   UIRectTextBox *tBox;

	switch(curUIRect->WhatAmI())	
	{
	case PCM_BUTTON_TIME_EDLINE :
      if (UIRECT_DONE == type)
      {
			edName = (UIRectEditLine *) curUIRect;
			if (edName->text[0])
			{
				sprintf(messKeyCode.string,edName->text);
				codePad = pcmText1;
				CryptoString(messKeyCode.string);
				button2Sound->PlayNo3D();
				lclient->SendMsg(sizeof(messKeyCode),(void *)&messKeyCode);
				curPickCharMode->showCodeEdLine = FALSE;
			}
			return 1;
      }
		break;

	case PCM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curPickCharMode->retState = GMR_POP_ME;
      }
		break;
	case PCM_BUTTON_ENTER_GAME :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			enterGame.characterIndex = curPickCharMode->curCharacterIndex;
			lclient->SendMsg(sizeof(enterGame),(void *)&enterGame);
      }
		break;
	case PCM_BUTTON_DELETE :
      if (UIRECT_MOUSE_LUP == type && curPickCharMode->charExists)
      {
			button1Sound->PlayNo3D();
			curPickCharMode->confirming = 1;
		   tBox = (UIRectTextBox *) curPickCharMode->confirmWindow->childRectList.Find(PCM_BUTTON_TEXT);
			sprintf(tempText,"Do you really want to\nerase %s\ncompletely and forever?",curPickCharMode->lastName);
			tBox->SetText(tempText);
      }
		break;

   case PCM_BUTTON_CHANGE_AV_TEX:
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
		   tButt = (UIRectTextButton *) curUIRect;
			if (useSimpleAvatarTextureFlag)
			{
				useSimpleAvatarTextureFlag = FALSE;
			   tButt->SetText(pcmText1);
			}
			else
			{
				useSimpleAvatarTextureFlag = TRUE;
			   tButt->SetText("Simple avatars ON");
			}
		}
		break;

	case PCM_BUTTON_CONFIRM_NO :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			curPickCharMode->confirming = FALSE;
      }
		break;
	case PCM_BUTTON_CONFIRM_YES :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			curPickCharMode->confirming = 2;
		   tBox = (UIRectTextBox *) curPickCharMode->confirm2Window->childRectList.Find(PCM_BUTTON_TEXT);
			sprintf(tempText,"LAST CHANCE! Click on DELETE to\nerase %s\nwithout hope of recovering her!",curPickCharMode->lastName);
			tBox->SetText(tempText);
      }
		break;

	case PCM_BUTTON_CONFIRM2_YES :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			messDelete.characterIndex = curPickCharMode->curCharacterIndex;
			lclient->SendMsg(sizeof(messDelete),(void *)&messDelete);
			curPickCharMode->confirming = FALSE;
      }
		break;
		/*
	case PCM_BUTTON_NEW_CHAR :
      if (UIRECT_MOUSE_LUP == type)
      {
			if (!curPickCharMode->statsToChange)
				curPickCharMode->statsToChange = new MessAvatarStats();
			curPickCharMode->statsToChange->hairB = 
				curPickCharMode->statsToChange->hairR = 
				curPickCharMode->statsToChange->hairG = 128;
			curPickCharMode->statsToChange->artIndex = 0;
			sprintf(curPickCharMode->lastName, curPickCharMode->newName->text);
			sprintf(curPickCharMode->statsToChange->name,curPickCharMode->lastName);
		
			newGameMode = new DesignCharMode(curPickCharMode->statsToChange, 0,"DESIGN_MODE_CHAR");
			curPickCharMode->retState = GMR_NEW_MODE;
      }
		break;
		*/

	case PCM_BUTTON_CHANGE_CLOTHES :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
//			curPickCharMode->curCharacterIndex = curUIRect->WhatAmI() - PCM_BUTTON_CHAR_0;
		   tButt = (UIRectTextButton *) curUIRect;
			if (curPickCharMode->charExists)
			{
				if (!curPickCharMode->statsToChange)
					curPickCharMode->statsToChange = new MessAvatarStats();

				*(curPickCharMode->statsToChange) = curPickCharMode->lastStats;
			
				newGameMode = new DesignCharMode(curPickCharMode->statsToChange, 1,"DESIGN_MODE_CHAR");
				curPickCharMode->retState = GMR_NEW_MODE;
			}
      }
		break;


	case PCM_BUTTON_CHAR_0 :
	case PCM_BUTTON_CHAR_1 :
	case PCM_BUTTON_CHAR_2 :
	case PCM_BUTTON_CHAR_3 :
	case PCM_BUTTON_CHAR_4 :
	case PCM_BUTTON_CHAR_5 :
	case PCM_BUTTON_CHAR_6 :
	case PCM_BUTTON_CHAR_7 :
      if (UIRECT_MOUSE_LUP == type)
      {
			button1Sound->PlayNo3D();
			curPickCharMode->curCharacterIndex = curUIRect->WhatAmI() - PCM_BUTTON_CHAR_0;
		   tButt = (UIRectTextButton *) curUIRect;
			if (!strcmp(tButt->text,"Create New Character"))
			{
				if (!curPickCharMode->statsToChange)
					curPickCharMode->statsToChange = new MessAvatarStats();

				curPickCharMode->statsToChange->hairB = 
					curPickCharMode->statsToChange->hairR = 
					curPickCharMode->statsToChange->hairG = 0;
				curPickCharMode->statsToChange->faceIndex = 0;

				curPickCharMode->statsToChange->topB = 
					curPickCharMode->statsToChange->topR = 
					curPickCharMode->statsToChange->topG = 0;
				curPickCharMode->statsToChange->topIndex = 0;

				curPickCharMode->statsToChange->bottomB = 
					curPickCharMode->statsToChange->bottomR = 
					curPickCharMode->statsToChange->bottomG = 0;
				curPickCharMode->statsToChange->bottomIndex = 0;

				curPickCharMode->statsToChange->creative =
					curPickCharMode->statsToChange->magical =
					curPickCharMode->statsToChange->physical = 4;
//				sprintf(curPickCharMode->lastName, curPickCharMode->newName->text);
//				sprintf(curPickCharMode->statsToChange->name,curPickCharMode->lastName);
			
				newGameMode = new DesignCharMode(curPickCharMode->statsToChange, 0,"DESIGN_MODE_CHAR");
				curPickCharMode->retState = GMR_NEW_MODE;
			}
			else
			{
				request.characterIndex = curPickCharMode->curCharacterIndex;
				lclient->SendMsg(sizeof(request),(void *)&request);
			}
      }
		break;



	}
	return 0;  // didn't deal with this message
}



//******************************************************************
PickCharMode::PickCharMode(int doid, char *doname) : GameMode(doid,doname)
{
	curPickCharMode = this;
	curCharacterIndex = 0;
	charExists = FALSE;
	statsToChange = NULL;
	animCounter = 0;
	animSpin = 0.02f;
	avTexture = NULL;
	numChars = 0;

	useSimpleAvatarTextureFlag = FALSE;
}

//******************************************************************
PickCharMode::~PickCharMode()
{
	if (statsToChange)
		delete statsToChange;
}


//******************************************************************
int PickCharMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int PickCharMode::Activate(void) // do this when the mode becomes the forground mode.
{
	aLog.Log("PickCharMode::Activate ********************\n");

	confirming = FALSE;
	showCodeEdLine = TRUE;

	SetEnvironment();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\URframe.png",
							0,0,0,0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &urFrame);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIstone.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffff00ff, NULL, NULL, &uiArt);

	puma->LoadTexture("dat\\CharInfoPanelRight.png", &infoArtRight, 0);
	puma->LoadTexture("dat\\CharInfoPanelLeft.png" , &infoArtLeft , 0);
	puma->LoadTexture("dat\\CharPickEdgeHigh.png"  , &edgeHigh    , 0);
	puma->LoadTexture("dat\\CharPickEdgeLow.png"   , &edgeLow     , 0);
	puma->LoadTexture("dat\\TimeLeftBox.png"       , &uiPopUpLongArt, 0);

	// ********** start building UIRects
   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

//   UIRectTextButton *tButt;
   UIRectArtTextButton *tArtButt;

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_ENTER_GAME,
                               puma->ScreenW() - 112, puma->ScreenH() - 31,
										 puma->ScreenW() - 0,   puma->ScreenH() - 0);
   tArtButt->SetText("Enter Game");
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt3High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt3Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt3Up.png"  , 0);
   fullWindow->AddChild(tArtButt);
	enter = tArtButt;

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_QUIT, 
                               puma->ScreenW() - 368, puma->ScreenH() - 31,
										 puma->ScreenW() - 368+114, puma->ScreenH() - 0);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->SetText("Exit Server");
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt1High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt1Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt1Up.png"  , 0);
   fullWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CHANGE_AV_TEX, 
		              0, puma->ScreenH() - 64,
		            255, puma->ScreenH());
	tArtButt->artOffsetY -= 63;
	if (useSimpleAvatarTextureFlag)
	{
	   tArtButt->SetText("Simple avatars ON");
	}
	else
	{
	   tArtButt->SetText("Simple avatars OFF");
	}
   tArtButt->process = PickCharModeProcess;
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerButt2High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerButt2Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerButt2Up.png", 0);
	tArtButt->textOffsetX = -45;
	tArtButt->textOffsetY = 18;
   fullWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_DELETE, 
                               puma->ScreenW() - 254, puma->ScreenH() - 31,
										 puma->ScreenW() - 254+142, puma->ScreenH() - 0);
   tArtButt->SetText("Delete Character");
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt2High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt2Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt2Up.png"  , 0);
   fullWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CHANGE_CLOTHES, 
                               puma->ScreenW() - 130 - 153, 3,
										 puma->ScreenW() - 130      , 26 + 3);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
   tArtButt->SetText("Change Clothes");
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\UIPopUp.png", 0); // 153x26
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\UIPopUp.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\UIPopUp.png"  , 0);
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   fullWindow->AddChild(tArtButt);
	
   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CHAR_0,
		              44, 30, 44+256, 30+35);
   tArtButt->SetText("Create New");
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharPickHigh.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharPickUp.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharPickUp.png", 0);
//	tArtButt->textOffsetX = -45;
//	tArtButt->textOffsetY = 82;
   fullWindow->AddChild(tArtButt);

	UIRectStep(0,1);
	
	for (int i = 1; i < 8; ++i)
	{
	   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CHAR_0+i, -2,-2,-2,-2);
		tArtButt->SetText("Create New");
		tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tArtButt->process = PickCharModeProcess;
		tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
		tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharPickHigh.png", 0);
		tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharPickUp.png", 0);
		tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharPickUp.png", 0);
	//	tArtButt->textOffsetX = -45;
	//	tArtButt->textOffsetY = 82;
		fullWindow->AddChild(tArtButt);

		UIRectStep(0,1);
	}

   UIRectTextBox *tBox;

	//***************
	/*
	newName	  = new UIRectEditLine(PCM_BUTTON_NEW_NAME, 
									puma->ScreenW()/2 - 60, 100  ,puma->ScreenW()/2 + 60, 120,31);
	newName->process = PickCharModeProcess;
	newName->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
	newName->SetText("Andra");
//	fullWindow->AddChild(edLine);
	
   newChar = new UIRectTextButton(PCM_BUTTON_NEW_CHAR, puma->ScreenW()/2 - 60, 122  ,puma->ScreenW()/2 + 60, 150);
   newChar->SetText("Start New Character");
   newChar->process = PickCharModeProcess;
//   fullWindow->AddChild(tButt);
	*/
	//***************
   tBox = new UIRectTextBox(PCM_BUTTON_TEXT,
		              puma->ScreenW() - 359,puma->ScreenH() - 102,
						  puma->ScreenW() -  5,puma->ScreenH() - 35);
   tBox->SetText("Character Description.");
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tBox->process = PickCharModeProcess;
	tBox->fillArt = uiArt;
   fullWindow->AddChild(tBox);

//	UIRectEditLine *tEdit;
	tBox = new UIRectTextBox(PCM_BUTTON_TIME_INFO,
		                          33, puma->ScreenH() - 160,
										  33+202, puma->ScreenH()-160 + 56);
	tBox->SetText("Getting experation time...\nEnter time code for more time");
	tBox->process = PickCharModeProcess;
//	tBox->font = 1;
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tBox->fillArt = uiPopUpLongArt;
	fullWindow->AddChild(tBox);
/*	UIRectStep(0,1);

	tEdit = new UIRectEditLine(PCM_BUTTON_TIME_EDLINE, -2,
		puma->ScreenH()-160 + 57, -2, puma->ScreenH()-160 + 57 + 25, 6+8);
	tEdit->SetText("");
	tEdit->process = PickCharModeProcess;
	tEdit->fillArt = NULL;
	tEdit->font = 1;
	fullWindow->AddChild(tEdit);
  */
   fullWindow->Arrange();

   // ********** start building UIRects for confirm window
   confirmWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   confirmWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   confirmWindow->isActive  = TRUE;
//	confirmWindow->fillArt = uiArt;

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CONFIRM_NO,
                               puma->ScreenW() - 112, puma->ScreenH() - 31,
										 puma->ScreenW() - 0,   puma->ScreenH() - 0);
   tArtButt->SetText("No way!");
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt3High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt3Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt3Up.png"  , 0);
   confirmWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CONFIRM_YES, 
                               puma->ScreenW() - 368, puma->ScreenH() - 31,
										 puma->ScreenW() - 368+114, puma->ScreenH() - 0);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->SetText("Yes, I'm sure");
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt1High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt1Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt1Up.png"  , 0);
   confirmWindow->AddChild(tArtButt);

   tBox = new UIRectTextBox(PCM_BUTTON_TEXT,
		              puma->ScreenW() - 359,puma->ScreenH() - 102,
						  puma->ScreenW() -  5,puma->ScreenH() - 35);
   tBox->SetText("Do you?");
   tBox->process = PickCharModeProcess;
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->fillArt = uiArt;
   confirmWindow->AddChild(tBox);

   confirmWindow->Arrange();

   // ********** start building UIRects for confirm2 window
   confirm2Window = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   confirm2Window->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   confirm2Window->isActive  = TRUE;
//	confirm2Window->fillArt = uiArt;

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CONFIRM2_YES,
                               puma->ScreenW() - 112, puma->ScreenH() - 31,
										 puma->ScreenW() - 0,   puma->ScreenH() - 0);
   tArtButt->SetText("DELETE");
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt3High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt3Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt3Up.png"  , 0);
   confirm2Window->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(PCM_BUTTON_CONFIRM_NO, 
                               puma->ScreenW() - 368, puma->ScreenH() - 31,
										 puma->ScreenW() - 368+114, puma->ScreenH() - 0);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->SetText("do not delete");
   tArtButt->process = PickCharModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\CharInfoButt1High.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\CharInfoButt1Down.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\CharInfoButt1Up.png"  , 0);
   confirm2Window->AddChild(tArtButt);

   tBox = new UIRectTextBox(PCM_BUTTON_TEXT,
		              puma->ScreenW() - 359,puma->ScreenH() - 102,
						  puma->ScreenW() -  5,puma->ScreenH() - 35);
   tBox->SetText("Do you?");
   tBox->process = PickCharModeProcess;
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->fillArt = uiArt;
   confirm2Window->AddChild(tBox);

   confirm2Window->Arrange();

   // ********** finished building UIRects

	anim = new PumaAnim(0,"avatar");
//	anim->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar-front-end.ase");
//	anim->Scale(puma->m_pd3dDevice, 0.3f, 0.3f, 0.3f);
//	anim->SaveCompressed(puma->m_pd3dDevice, "dat\\avatar-front-end.anc");
//	anim->Save(puma->m_pd3dDevice, "dat\\avatar-front-end.ani");
	anim->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-front-end.anc");
	anim->Scale( puma->m_pd3dDevice, 60.0f, 60.0f, 60.0f );
//	anim->Load(puma->m_pd3dDevice, "dat\\avatar-front-end.ani");

//	avTexture = NULL;
//	avTexture = new AvatarTexture();
//	avTexture->Generate(puma->m_pd3dDevice,0,255,255,255,0,255,255,255,0,255,255,255);

	if (!statsToChange)
	{
		MessAvatarRequestStats request;
		request.characterIndex = curCharacterIndex;
		lclient->SendMsg(sizeof(request), &request);
	}
	else if (255 == statsToChange->faceIndex)
	{
		MessAvatarNewClothes clothes;
		clothes.avatarID    = curCharacterIndex;
		clothes.bottomB     = statsToChange->bottomB    ;
		clothes.bottomG     = statsToChange->bottomG    ;
		clothes.bottomR     = statsToChange->bottomR    ;
		clothes.bottomIndex = statsToChange->bottomIndex;
		clothes.topB        = statsToChange->topB       ;
		clothes.topG        = statsToChange->topG       ;
		clothes.topR        = statsToChange->topR       ;
		clothes.topIndex    = statsToChange->topIndex   ;
		lclient->SendMsg(sizeof(clothes), &clothes);
	}
	else if (255 == statsToChange->topIndex)
	{
		MessAvatarRequestStats request;
		request.characterIndex = curCharacterIndex;
		lclient->SendMsg(sizeof(request), &request);
	}
	else
	{
		statsToChange->avatarID = curCharacterIndex;
		lclient->SendMsg(sizeof(MessAvatarStats), statsToChange);
	}

	return(0);
}

//******************************************************************
int PickCharMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
	if (avTexture)
	{
		delete avTexture;
		avTexture = NULL;
	}
	delete anim;
	charExists = 0;

	delete confirm2Window;
	delete confirmWindow;
	delete fullWindow;

	SAFE_RELEASE(uiPopUpLongArt);
   SAFE_RELEASE(edgeLow);
   SAFE_RELEASE(edgeHigh);
   SAFE_RELEASE(infoArtLeft);
   SAFE_RELEASE(infoArtRight);

   SAFE_RELEASE(uiArt);
   SAFE_RELEASE(urFrame);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int PickCharMode::Tick(void)
{
	if (0 == numChars)
	{
		if (!fullWindow->childRectList.Find(PCM_BUTTON_BEGIN_TEXT))
		{
			UIRectTextBox *tBox;
			tBox = new UIRectTextBox(PCM_BUTTON_BEGIN_TEXT,
				                          puma->ScreenW()/2 +2, 30,
												  puma->ScreenW()   -2, puma->ScreenH()/2);
		   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
			tBox->SetText("Let's make a new\ncharacter for you.\n\nJust click on one\nof the buttons to the left.");
			tBox->process = PickCharModeProcess;
			tBox->font = 2;
			tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
//			tBox->fillArt = uiArt;
			fullWindow->AddChild(tBox);

		   fullWindow->Arrange();
		}
	}
	else
	{
		UIRectTextBox *tBox;

		tBox = (UIRectTextBox *) fullWindow->childRectList.Find(PCM_BUTTON_BEGIN_TEXT);
		if (tBox)
		{
			fullWindow->childRectList.Remove(tBox);
			delete tBox;
		}
	}



	if (showCodeEdLine)
	{
		if (!fullWindow->childRectList.Find(PCM_BUTTON_TIME_EDLINE))
		{
			UIRectEditLine *tEdit = new UIRectEditLine(PCM_BUTTON_TIME_EDLINE, 33,
				puma->ScreenH()-160 + 57, 33+202, puma->ScreenH()-160 + 57 + 25, 6+8);
			tEdit->SetText("");
			tEdit->process = PickCharModeProcess;
			tEdit->fillArt = NULL;
			tEdit->font = 1;
			fullWindow->AddChild(tEdit);

		   fullWindow->Arrange();
		}
	}
	else
	{
		UIRectEditLine *tEdit;

		tEdit = (UIRectEditLine *) fullWindow->childRectList.Find(PCM_BUTTON_TIME_EDLINE);
		if (tEdit)
		{
			fullWindow->childRectList.Remove(tEdit);
			delete tEdit;
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

   D3DXMATRIX matWorld, mat2, mat3, matTrans;
//	float tweakF;
	++animCounter;

/*	puma->pumaCamera->spacePoint.angle = -12.67f;
	puma->pumaCamera->spacePoint.azimuth = -0.37f;
	puma->pumaCamera->spacePoint.location.x = 40.0f;
	puma->pumaCamera->spacePoint.location.y = -41.0f;
	puma->pumaCamera->spacePoint.location.z = 115.0f;*/

	SpacePoint *sp = &puma->pumaCamera->spacePoint;

	float diff;

	diff = ((-0.104f) - sp->angle);
	sp->angle      += diff/10; 
	diff = ((-0.314f) - sp->azimuth);
	sp->azimuth    += diff/10; 
	diff = ((-17.4f) - sp->location.x);
	sp->location.x += diff/10;
	diff = ((-15.0f) - sp->location.y);
	sp->location.y += diff/10;
	diff = ((157.0f) - sp->location.z);
	sp->location.z += diff/10;

//	sp->angle = -0.104f;
//	sp->azimuth = -0.314f;


//	if (server)
//		server->Tick();
	HandleMessages();

	WindowServicer();
	// processing

	if (GMR_POP_ME == retState)
		return(0);

	if (charExists)
	{
		enter->isDisabled = FALSE;
	}
	else
	{
		enter->isDisabled = TRUE;
	}

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

//	puma->HandleCamera();

//	D3DXMatrixMultiply( &mat3, &mat3, &mat2);
//	D3DXMatrixMultiply( &matWorld, &matWorld, &mat3 );
//	D3DXMATRIX matWorld;
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

	if (charExists)
	{
		puma->m_pd3dDevice->SetTexture( 0, avTexture->currentBitmap );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		anim->Draw(puma->m_pd3dDevice, (animCounter/5) % anim->numOfFrames);
	}

	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

	D3DSURFACE_DESC desc;
	urFrame->GetLevelDesc(0, &desc);
	RECT dRect3 = {puma->ScreenW() - desc.Width, 0, puma->ScreenW(), desc.Height};
	puma->DrawRect(urFrame,dRect3,0xffffffff);

	infoArtRight->GetLevelDesc(0, &desc);
	RECT dRect4 = {puma->ScreenW() - desc.Width, puma->ScreenH() - desc.Height+14, 
						puma->ScreenW(),              puma->ScreenH()+14};
	puma->DrawRect(infoArtRight,dRect4,0xffffffff);

	infoArtLeft->GetLevelDesc(0, &desc);
	RECT dRect5 = {puma->ScreenW() - desc.Width-256, puma->ScreenH() - desc.Height+14, 
						puma->ScreenW()-256,              puma->ScreenH()+14};
	puma->DrawRect(infoArtLeft,dRect5,0xffffffff);

	if (!confirming)
	{
		edgeHigh->GetLevelDesc(0, &desc);

		RECT dRect2 = {44, 5, 44+256, 5+32};
		puma->DrawRect(edgeHigh,dRect2,0xffffffff);

		edgeLow->GetLevelDesc(0, &desc);

		RECT dRect = {44, 308, 44+256, 308+32};
		puma->DrawRect(edgeLow,dRect,0xffffffff);

	}
/*
	D3DSURFACE_DESC desc;
	backArt->GetLevelDesc(0, &desc);

	RECT dRect = {0,0,desc.Width,desc.Height};
	puma->DrawRect(backArt,dRect,0xffffffff);
*/
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	if (1 == confirming)
	   confirmWindow->Draw();
	else if (2 == confirming)
	   confirm2Window->Draw();
	else
	   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long PickCharMode::WindowServicer(void)
{

	UIRectEditLine::blinkTimer++;

	// check input
	puma->PumpInput();
//	int tempX, tempY;

	UIRectWindow *master = fullWindow;
	if (1 == confirming)
		master = confirmWindow;
	else if (2 == confirming)
		master = confirm2Window;



	PumaInputEvent *pie = puma->TakeNextInputEvent();
	while (pie)
	{
		switch(pie->WhatAmI())
		{
		case PUMA_INPUT_KEYDOWN:
         master->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

			break;

		case PUMA_INPUT_KEYUP:
         master->Action(UIRECT_KEY_UP,   0, pie->scancode);
//		 if (pie->asciiValue >= 32 && pie->asciiValue <= 126 && 
//			 pie->asciiValue != 34 && pie->asciiValue != 44)
	         master->Action(UIRECT_KEY_CHAR, 0, pie->asciiValue);

			break;

		case PUMA_INPUT_LMBDOWN:
		   master->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_LMBUP:
		   master->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBDOWN:
		   master->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBUP:
		   master->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
			break;
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

   master->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);


    return 0;
}


//***************************************************************
void PickCharMode::HandleMessages(void)
{
	char messData[4000], tempText[1024];
	int  dataSize;
	FILE *source = NULL;
	int i; //correct, found, i,j;

	int hairR, hairG, hairB, faceIndex;
	int bottomR, bottomG, bottomB, bottomIndex;
	int topR, topG, topB, topIndex;

	MessGeneralNo   *messNoPtr;
	MessGeneralYes  *messYesPtr;
	MessAvatarStats *messAvatarStatsPtr;
	MessAvatarNames *messAvatarNamesPtr;
	MessAccountTimeInfo *messTimeInfoPtr;
	MessKeyCodeResponse *keyCodeResPtr;

	UIRectTextBox *tBox;
   UIRectTextButton *tButt;

	DoublyLinkedList *list = NULL;

	std::vector<TagID> tempReceiptList;
	int					fromSocket = 0;

	LongTime now;
	int diff;

	lclient->GetNextMsg(NULL, dataSize);
	
	while (dataSize > 0)
	{
		if (dataSize > 4000)
			dataSize = 4000;
		
		lclient->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);
		
		MessEmpty *empty = (MessEmpty *)messData;
		switch (messData[0])
		{

		case NWMESS_GENERAL_YES:
			messYesPtr = (MessGeneralYes *) messData;

			newGameMode = new GroundTestMode(messYesPtr->subType, 0, "GROUND_TEST_MODE");
			retState = GMR_NEW_MODE;
//			retState = GMR_POP_ME;
			return; // don't get any more messages.
		   break;

		case NWMESS_GENERAL_NO:
			messNoPtr = (MessGeneralNo *) messData;
			
			if (-1 == messNoPtr->subType)
			{
				newGameMode = new SimpleMessageMode("User names must start with a letter or number.",0,"SIMPLE_MESSAGE_MODE");
				retState = GMR_NEW_MODE;
				statsToChange = NULL;
			}
			if (7 == messNoPtr->subType)
			{
				newGameMode = new SimpleMessageMode("Sorry, that avatar name is already taken.\nPlease choose another.",0,"SIMPLE_MESSAGE_MODE");
				retState = GMR_NEW_MODE;
				statsToChange = NULL;
			}
			else
			{
				charExists = FALSE;
			   tBox = (UIRectTextBox *) fullWindow->childRectList.Find(PCM_BUTTON_TEXT);
				sprintf(tempText,"Character %d:   Empty; you have not yet\ncreated a character for this slot.\n\nType in a name, then click on\nthe Start New Character button.",curCharacterIndex+1);
				tBox->SetText(tempText);
			}
		   break;

		case NWMESS_AVATAR_STATS:
			messAvatarStatsPtr = (MessAvatarStats *) messData;

			lastStats = *messAvatarStatsPtr;

			hairR = messAvatarStatsPtr->hairR;
			hairG = messAvatarStatsPtr->hairG;
			hairB = messAvatarStatsPtr->hairB;
			faceIndex = messAvatarStatsPtr->faceIndex;

			topR = messAvatarStatsPtr->topR;
			topG = messAvatarStatsPtr->topG;
			topB = messAvatarStatsPtr->topB;
			topIndex = messAvatarStatsPtr->topIndex;

			bottomR = messAvatarStatsPtr->bottomR;
			bottomG = messAvatarStatsPtr->bottomG;
			bottomB = messAvatarStatsPtr->bottomB;
			bottomIndex = messAvatarStatsPtr->bottomIndex;

			sprintf(lastName, messAvatarStatsPtr->name);
			tBox = (UIRectTextBox *) fullWindow->childRectList.Find(PCM_BUTTON_TEXT);
			sprintf(tempText,"Character %d:   %s\nPhysical %d  Magical %d  Creative %d\nLevel %ld        Cash %ld\nClick on the Enter Game button\nto take this character into the game!",
				         curCharacterIndex+1, lastName, messAvatarStatsPtr->physical, 
							messAvatarStatsPtr->magical, messAvatarStatsPtr->creative,
							messAvatarStatsPtr->cLevel, messAvatarStatsPtr->cash);
			tBox->SetText(tempText);

			if (!avTexture)
				avTexture = new AvatarTexture();
			avTexture->Generate(puma->m_pd3dDevice,
										faceIndex, hairR, hairG, hairB,
										topIndex, topR, topG, topB,
										bottomIndex, bottomR, bottomG, bottomB,
										messAvatarStatsPtr->imageFlags);
			charExists = TRUE;

		   break;

		case NWMESS_AVATAR_NAMES:
			messAvatarNamesPtr = (MessAvatarNames *) messData;
			numChars = 0;
			for (i = 0; i < 8; ++i)
			{
				tButt = (UIRectTextButton *) fullWindow->childRectList.Find(i + PCM_BUTTON_CHAR_0);
				if (messAvatarNamesPtr->name[i][0])
				{
					sprintf(tempText, "Select %s", messAvatarNamesPtr->name[i]);
					tButt->SetText(tempText);
					++numChars;
				}
				else
					tButt->SetText("Create New Character");
			}
		   break;

		case NWMESS_ACCOUNT_TIME_INFO:
			messTimeInfoPtr = (MessAccountTimeInfo *) messData;
			accountTime.value.wYear      = messTimeInfoPtr->wYear      ;
			accountTime.value.wMonth     = messTimeInfoPtr->wMonth     ;
			accountTime.value.wDay       = messTimeInfoPtr->wDay       ;
			accountTime.value.wDayOfWeek = messTimeInfoPtr->wDayOfWeek ;
			accountTime.value.wHour      = messTimeInfoPtr->wHour      ;
			accountTime.value.wMinute    = messTimeInfoPtr->wMinute    ;

			tBox = (UIRectTextBox *) fullWindow->childRectList.Find(PCM_BUTTON_TIME_INFO);
			/*
			sprintf(tempText,"Account expiration:\n %02d-%02d-%4d",
				accountTime.value.wMonth, accountTime.value.wDay, accountTime.value.wYear);

			diff = now.MinutesDifference(&accountTime);
			if (diff <= 0)
				sprintf(&(tempText[strlen(tempText)]),". EXPIRED\n");
			else if (diff < 60)
				sprintf(&(tempText[strlen(tempText)]),". %d minutes from now.\n", diff);
			else if (diff < 24 * 60)
				sprintf(&(tempText[strlen(tempText)]),". %d hours from now.\n", diff/60);
			else
				sprintf(&(tempText[strlen(tempText)]),".\n");
			*/

			sprintf(tempText,"Accounts don't expire!\n");

			sprintf(&(tempText[strlen(tempText)]),"Please type in your code below and\npress Enter to unlock more time.");
			tBox->SetText(tempText);

			break;

		case NWMESS_KEYCODE_RESPONSE:
			keyCodeResPtr = (MessKeyCodeResponse *) messData;

			if (1 == keyCodeResPtr->value) // success
			{
				newGameMode = new SimpleMessageMode("Thank you for purchasing 31 more days of play in the amazing\n\
world of Blade Mistess!\n\n\
Your code was processed successfully, and you should be able to start enjoying your\n\
new time right away!\n\n\
Thank you for playing Blade Mistress!",
					0,"SIMPLE_MESSAGE_MODE");
				retState = GMR_NEW_MODE;
			}
			else if (2 == keyCodeResPtr->value) // already used up!
			{
				newGameMode = new SimpleMessageMode("That code WAS valid, but it has already been used up.\n\
please purchase another code.\n\n\
You can purchase a code which will give you 31 more days of playing time, by\n\
going to the www.blademistress.com website, and following the PAYMENT link.\n\n\
You can choose to pay with a credit card, Paypal, check, money order, or even cash!\n\
After receiving your payment, DigitalRiver will e-mail you your code right away, so\n\
make sure you give them your correct e-mail address.\n\n\
Once you have the e-mail with the code in it, log on to the game and type in the code\n\
in the Character Selection screen.\n\n\
Currently 31 days of time costs $5us.\n\n\
Thank you for playing Blade Mistress!",
					0,"SIMPLE_MESSAGE_MODE");
				retState = GMR_NEW_MODE;
			}
			else
			{
				newGameMode = new SimpleMessageMode("That code was not valid.\n\
Blade Mistress payment codes are 6 numbers followed by 8 letters, like 010101ABCDABCD .\n\
please purchase another code.  If you feel your code should have been valid, please\n\
e-mail punisher@blademistress.com.  In the e-mail, please include the date and method\n\
you purchased the code, your account name and password, and the code itself.\n\
we will try to correct the situation as soon as possible.\n\n\
You can purchase a code which will give you 31 more days of playing time, by\n\
going to the www.blademistress.com website, and following the PAYMENT link.\n\n\
You can choose to pay with a credit card, Paypal, check, money order, or even cash!\n\
After receiving your payment, DigitalRiver will e-mail you your code right away, so\n\
make sure you give them your correct e-mail address.\n\n\
Once you have the e-mail with the code in it, log on to the game and type in the code\n\
in the Character Selection screen.\n\n\
Currently 31 days of time costs $5us.\n\n\
Thank you for playing Blade Mistress!",
					0,"SIMPLE_MESSAGE_MODE");
				retState = GMR_NEW_MODE;
			}


			break;
		}
		
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//******************************************************************
void PickCharMode::SetEnvironment(void)
{

	puma->pumaCamera->m_fFarPlane   = 300.0f;
	puma->pumaCamera->Calculate();
	puma->pumaCamera->AssertView(puma->m_pd3dDevice);

	// init camera position
	puma->pumaCamera->spacePoint.angle = -12.67f;
	puma->pumaCamera->spacePoint.angle = NormalizeAngle2(puma->pumaCamera->spacePoint.angle);
	puma->pumaCamera->spacePoint.azimuth = -0.37f;
	puma->pumaCamera->spacePoint.location.x = 40.0f;
	puma->pumaCamera->spacePoint.location.y = -41.0f;
	puma->pumaCamera->spacePoint.location.z = 115.0f;

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



