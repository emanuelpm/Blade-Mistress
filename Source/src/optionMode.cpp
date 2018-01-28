
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "optionMode.h"
#include "instructionMode.h"
#include "wikiMode.h"
#include "creditsMode.h"
#include "groundTestMode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"
#include "simpleGrass.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragEdge.h"
#include "./puma/UIR_EdgeBar.h"
#include "./puma/UIR_EditLine.h"
#include "./puma/UIR_BigInfoWindow.h"

#include "clientOnly.h"

#include "BBOClient.h"

#include "BBOServer.h"

enum 
{
	OM_BUTTON_RETURN,
	OM_BUTTON_HELP,
	OM_BUTTON_WIKI,
	OM_BUTTON_CREDITS,
	OM_BUTTON_QUIT,
	OM_BUTTON_GRASS_DENSITY,
	OM_BUTTON_GROUND_DRAW_VALUE,
	OM_BUTTON_WASD,

	OM_BUTTON_INFO_HITS,
	OM_BUTTON_INFO_MISSES,
	OM_BUTTON_INFO_LOOTS,
	OM_BUTTON_INFO_FOG,
	OM_BUTTON_INFO_MIST,
	OM_BUTTON_INFO_CLICKMOVE,
	OM_BUTTON_INFO_3DSOUND,
	OM_BUTTON_INFO_UISOUND,
	OM_BUTTON_INFO_FILTER,
	OM_BUTTON_INFO_HOTKEY_CTRL,
	OM_BUTTON_INFO_WEATHER,
	OM_BUTTON_INFO_GLAMOUR,
	OM_BUTTON_TEXT,
	OM_BUTTON_MAX
};

//PlasmaTexture *bboClient->groundMap;
extern BBOServer *server;
extern Client *	lclient;

extern long gGamePlayerLevel, gGamePlayerHP, gGamePlayerMaxHP;
extern char gGamePlayerAge;

extern int playerInDungeon;

extern BBOClient *bboClient;

extern GroundTestMode *curgroundTestMode;

extern SimpleGrass *simpleGrass[4];

unsigned long playerInfoFlags = 0xffffffff;
unsigned long localInfoFlags  = 0x0fffffff;

// static pointer that the process function can use
OptionMode *curOptionMode = NULL;

int POMSezQuit = FALSE;
// process function for UIRects in this mode
int FAR PASCAL optionModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	MessInfoFlags infoFlagsMess;


	if (UIRECT_MOUSE_LUP == type)
	{

		switch(curUIRect->WhatAmI())	
		{
		case OM_BUTTON_RETURN :
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curOptionMode->retState = GMR_POP_ME;
			return 1;
			break;

		case OM_BUTTON_HELP :
			button1Sound->PlayNo3D();
			newGameMode = new InstructionMode(1,"Instrctuon_mode");
			curOptionMode->retState = GMR_NEW_MODE;
			return 1;
			break;

		case OM_BUTTON_WIKI :
			button1Sound->PlayNo3D();
			newGameMode = new WikiMode(1,"Wiki_mode");
//			newGameMode = new CreditMode(1,"Credits_mode");
			curOptionMode->retState = GMR_NEW_MODE;
			return 1;
			break;

		case OM_BUTTON_CREDITS :
			button1Sound->PlayNo3D();
			newGameMode = new CreditMode(1,"Credits_mode");
			curOptionMode->retState = GMR_NEW_MODE;
			return 1;
			break;

		case OM_BUTTON_GRASS_DENSITY :
			button1Sound->PlayNo3D();
			--grassDensity;
			if (grassDensity < 0)
				grassDensity = 6;

			curOptionMode->RefreshSettingsButtons();

			delete simpleGrass[0];
			delete simpleGrass[1];
			delete simpleGrass[2];
			delete simpleGrass[3];

			simpleGrass[0] = new SimpleGrass(200/3*grassDensity+5, "dat\\grass-bill-1.png", 0.2f);
			simpleGrass[1] = new SimpleGrass(200/3*grassDensity+5, "dat\\grass-bill-2.png", 0.25f);
			simpleGrass[2] = new SimpleGrass(200/3*grassDensity+5, "dat\\grass-bill-3.png", 0.3f);
			simpleGrass[3] = new SimpleGrass(200/3*grassDensity+5, "dat\\grass-bill-4.png", 0.3f);

			return 1;
			break;

		case OM_BUTTON_GROUND_DRAW_VALUE :
			button1Sound->PlayNo3D();
			terrainDrawValue -= 10.0f;
			if (terrainDrawValue < 0.0f)
				terrainDrawValue = 90.0f;

			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_HITS :
			button1Sound->PlayNo3D();
			if (playerInfoFlags & INFO_FLAGS_HITS)
				playerInfoFlags &= ~(INFO_FLAGS_HITS);
			else
				playerInfoFlags |= INFO_FLAGS_HITS;

			infoFlagsMess.flags = playerInfoFlags;
			lclient->SendMsg(sizeof(infoFlagsMess),(void *)&infoFlagsMess);
			return 1;
			break;

		case OM_BUTTON_INFO_MISSES :
			button1Sound->PlayNo3D();
			if (playerInfoFlags & INFO_FLAGS_MISSES)
				playerInfoFlags &= ~(INFO_FLAGS_MISSES);
			else
				playerInfoFlags |= INFO_FLAGS_MISSES;

			infoFlagsMess.flags = playerInfoFlags;
			lclient->SendMsg(sizeof(infoFlagsMess),(void *)&infoFlagsMess);
			return 1;
			break;

		case OM_BUTTON_INFO_LOOTS :
			button1Sound->PlayNo3D();
			if (playerInfoFlags & INFO_FLAGS_LOOT_TAKEN)
				playerInfoFlags &= ~(INFO_FLAGS_LOOT_TAKEN);
			else
				playerInfoFlags |= INFO_FLAGS_LOOT_TAKEN;

			infoFlagsMess.flags = playerInfoFlags;
			lclient->SendMsg(sizeof(infoFlagsMess),(void *)&infoFlagsMess);
			return 1;
			break;

		case OM_BUTTON_INFO_FOG :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_FOG_ON)
				localInfoFlags &= ~(LOCAL_FLAGS_FOG_ON);
			else
				localInfoFlags |= LOCAL_FLAGS_FOG_ON;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_MIST :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_MIST_ON)
				localInfoFlags &= ~(LOCAL_FLAGS_MIST_ON);
			else
				localInfoFlags |= LOCAL_FLAGS_MIST_ON;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_FILTER :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_FILTER_ON)
				localInfoFlags &= ~(LOCAL_FLAGS_FILTER_ON);
			else
				localInfoFlags |= LOCAL_FLAGS_FILTER_ON;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_HOTKEY_CTRL :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_HOTKEY_CTRL)
				localInfoFlags &= ~(LOCAL_FLAGS_HOTKEY_CTRL);
			else
				localInfoFlags |= LOCAL_FLAGS_HOTKEY_CTRL;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_WEATHER :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_WEATHER)
				localInfoFlags &= ~(LOCAL_FLAGS_WEATHER);
			else
				localInfoFlags |= LOCAL_FLAGS_WEATHER;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_CLICKMOVE :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON)
				localInfoFlags &= ~(LOCAL_FLAGS_CLICKMOVE_ON);
			else
				localInfoFlags |= LOCAL_FLAGS_CLICKMOVE_ON;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_3DSOUND :
			if (PumaSound::playSounds)
				PumaSound::playSounds = FALSE;
			else
			{
				PumaSound::playSounds = TRUE;
				button1Sound->PlayNo3D();
			}
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_UISOUND :
			if (PumaSound::playUISounds)
				PumaSound::playUISounds = FALSE;
			else
			{
				PumaSound::playUISounds = TRUE;
				button1Sound->PlayNo3D();
			}
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_INFO_GLAMOUR :
			if (localInfoFlags & LOCAL_FLAGS_GLAMOUR)
				localInfoFlags &= ~(LOCAL_FLAGS_GLAMOUR);
			else
				localInfoFlags |= LOCAL_FLAGS_GLAMOUR;
			curOptionMode->RefreshSettingsButtons();
			return 1;
			break;

		case OM_BUTTON_QUIT :
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curOptionMode->retState = GMR_POP_ME;
			POMSezQuit = TRUE;
			return 1;
			break;

		case OM_BUTTON_WASD :
			button1Sound->PlayNo3D();
			if (localInfoFlags & LOCAL_FLAGS_WASD)
				localInfoFlags &= ~(LOCAL_FLAGS_WASD);
			else
				localInfoFlags |= LOCAL_FLAGS_WASD;
			curOptionMode->RefreshSettingsButtons();
			return 1;
		}
	}
	return 0;  // didn't deal with this message
}

//******************************************************************
//******************************************************************
OptionMode::OptionMode(int doid, char *doname) : GameMode(doid,doname)
{
	curOptionMode = this;
}

//******************************************************************
OptionMode::~OptionMode()
{
	SaveLocalSettings();
	curOptionMode = NULL;
}

//******************************************************************
int OptionMode::Init(void)  // do this when instantiated.
{
	return(0);
}

const int OM_UI_ALPHA = 100;

//******************************************************************
int OptionMode::Activate(void) // do this when the mode becomes the forbboClient->ground mode.
{
	aLog.Log("OptionMode::Activate ********************\n");

	uiAlpha = OM_UI_ALPHA;
	char tempText[1024];

	SetEnvironment();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

	puma->LoadTexture("dat\\UIPopUp.png"   , &uiPopUpArt   , 0);	// 153x26
	puma->LoadTexture("dat\\UIPopUpLong.png", &uiPopUpLongArt, 0); // 202x26

   // ********** start building UIRects
	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

//	actionWindow = NULL;

   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectTextButton *tButt;
//   UIRectArtTextButton *tButt;

	BigInfoWindow *bigInfo = new BigInfoWindow(-2, centerX - 310, centerY - 210, 
		                            centerX + 310, centerY + 210);
   fullWindow->AddChild(bigInfo);

	int centerX2 = (bigInfo->box.right - bigInfo->box.left)/2;
	int centerY2 = (bigInfo->box.bottom - bigInfo->box.top)/2;

   UIRectTextBox *tBox;

	tBox = new UIRectTextBox(OM_BUTTON_TEXT, 10,10, 
		             bigInfo->box.right-bigInfo->box.left - 20, 45);
	tBox->process = optionModeProcess;

	sprintf(tempText,"Level %ld      Health %ld/%ld     ", gGamePlayerLevel, gGamePlayerHP, gGamePlayerMaxHP);
	sprintf(&(tempText[strlen(tempText)]),"%s ", ageTextArray[gGamePlayerAge]);
	if (CanHaveROP(gGamePlayerAge, gGamePlayerLevel))
		sprintf(&(tempText[strlen(tempText)]),"READY");
	tBox->SetText(tempText);
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->fillArt = NULL;
	tBox->font = 2;
   bigInfo->AddChild(tBox);

   tButt = new UIRectTextButton(OM_BUTTON_RETURN,
		             centerX2-200,     centerY2-150,
		             centerX2-200+202, centerY2-150+26);
   tButt->SetText("Return to Game");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,2);
   tButt = new UIRectTextButton(OM_BUTTON_HELP,-2,-2,-2,-2);
   tButt->SetText("View Instructions");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_WIKI,-2,-2,-2,-2);
   tButt->SetText("Community Info");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_CREDITS,-2,-2,-2,-2);
   tButt->SetText("View Credits");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_GRASS_DENSITY,-2,-2,-2,-2);
   tButt->SetText("Grass");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_GROUND_DRAW_VALUE,-2,-2,-2,-2);
   tButt->SetText("Terrain Detail");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,2);
   tButt = new UIRectTextButton(OM_BUTTON_WASD,-2,-2,-2,-2);
   tButt->SetText("WASD moving enabled");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,4);
   tButt = new UIRectTextButton(OM_BUTTON_QUIT,-2,-2,-2,-2);
   tButt->SetText("Quit Game");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpLongArt;
   bigInfo->AddChild(tButt);

	// options
   tButt = new UIRectTextButton(OM_BUTTON_INFO_HITS,
		             centerX2+50,     centerY2-160,
		             centerX2+50+153, centerY2-160+26);
   tButt->SetText("Show hits");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_MISSES,-2,-2,-2,-2);
   tButt->SetText("Show Misses");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_LOOTS,-2,-2,-2,-2);
   tButt->SetText("Show loots");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_FOG,-2,-2,-2,-2);
   tButt->SetText("Show fog");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_MIST,-2,-2,-2,-2);
   tButt->SetText("Show mist");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_CLICKMOVE,-2,-2,-2,-2);
   tButt->SetText("Click-to-move ON");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_3DSOUND,-2,-2,-2,-2);
   tButt->SetText("3D sound");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_UISOUND,-2,-2,-2,-2);
   tButt->SetText("UI sound");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_FILTER,-2,-2,-2,-2);
   tButt->SetText("Language Filter");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   /*
   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_HOTKEY_CTRL,-2,-2,-2,-2);
   tButt->SetText("HotKeys use CTRL");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_WEATHER,-2,-2,-2,-2);
   tButt->SetText("HotKeys use CTRL");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);
	*/
   UIRectStep(0,1);
   tButt = new UIRectTextButton(OM_BUTTON_INFO_GLAMOUR,-2,-2,-2,-2);
   tButt->SetText("Weapon Effects");
   tButt->process = optionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	tButt->fillArt = uiPopUpArt;
   bigInfo->AddChild(tButt);

	RefreshSettingsButtons();

   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff

	POMSezQuit = FALSE;

	return(0);
}

//******************************************************************
int OptionMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;
	SAFE_RELEASE(uiPopUpLongArt);
	SAFE_RELEASE(uiPopUpArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int OptionMode::Tick(void)
{
	char tempText[1024];

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

	BigInfoWindow *big = (BigInfoWindow *) fullWindow->childRectList.First();

   UIRectTextBox *tBox = (UIRectTextBox *) big->childRectList.Find(OM_BUTTON_TEXT);

	sprintf(tempText,"Level %ld      Health %ld/%ld     ", gGamePlayerLevel, gGamePlayerHP, gGamePlayerMaxHP);
	sprintf(&(tempText[strlen(tempText)]),"%s ", ageTextArray[gGamePlayerAge-1]);
	if (CanHaveROP(gGamePlayerAge, gGamePlayerLevel))
		sprintf(&(tempText[strlen(tempText)]),"READY");
	tBox->SetText(tempText);

   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

	return(0);
}


//*******************************************************************************
long OptionMode::WindowServicer(void)
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
void OptionMode::HandleMessages(void)
{
	char messData[4000];//, tempText[1024];
	int  dataSize;
	FILE *source = NULL;
	MessPlayerChatLine chatMess;
//	MessInfoText *infoText;
	MessInfoFlags       *infoFlagsPtr;

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

		case NWMESS_INFO_FLAGS:
			infoFlagsPtr = (MessInfoFlags *) messData;
			playerInfoFlags = infoFlagsPtr->flags;
			RefreshSettingsButtons();
			break;

		default:
			bboClient->HandleMessage(messData, dataSize);
			break;

		}
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//***************************************************************
void OptionMode::RefreshSettingsButtons(void)
{
//extern float terrainDrawValue;
//extern int   grassDensity;
	UIRectTextButton *tButt;
	char tempText[128];

	BigInfoWindow *big = (BigInfoWindow *) fullWindow->childRectList.First();

	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_GROUND_DRAW_VALUE);
	if (tButt)
	{
		sprintf(tempText,"Terrain Complexity: %3.1f", terrainDrawValue);
		tButt->SetText(tempText);
	}

	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_GRASS_DENSITY);
	if (tButt)
	{
		sprintf(tempText,"Grass Density: %d", grassDensity);
		tButt->SetText(tempText);
	}

	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_HITS);
	if (tButt)
	{
		if (playerInfoFlags & INFO_FLAGS_HITS)
			sprintf(tempText,"Hits displayed");
		else
			sprintf(tempText,"Hits NOT displayed");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_MISSES);
	if (tButt)
	{
		if (playerInfoFlags & INFO_FLAGS_MISSES)
			sprintf(tempText,"Misses displayed");
		else
			sprintf(tempText,"Misses NOT displayed");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_LOOTS);
	if (tButt)
	{
		if (playerInfoFlags & INFO_FLAGS_LOOT_TAKEN)
			sprintf(tempText,"Items taken displayed");
		else
			sprintf(tempText,"Item taken NOT shown");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_FOG);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_FOG_ON)
			sprintf(tempText,"Fog enabled");
		else
			sprintf(tempText,"Fog DISabled");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_MIST);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_MIST_ON)
			sprintf(tempText,"Mist enabled");
		else
			sprintf(tempText,"Mist DISabled");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_FILTER);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_FILTER_ON)
			sprintf(tempText,"Language Filter enabled");
		else
			sprintf(tempText,"Language Filter DISabled");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_CLICKMOVE);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON)
			sprintf(tempText,"Click-to-move ON");
		else
			sprintf(tempText,"Click-to-move OFF");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_3DSOUND);
	if (tButt)
	{
		if (PumaSound::playSounds)
			sprintf(tempText,"Game sounds enabled");
		else
			sprintf(tempText,"Game sounds DISabled");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_UISOUND);
	if (tButt)
	{
		if (PumaSound::playUISounds)
			sprintf(tempText,"UI sounds enabled");
		else
			sprintf(tempText,"UI sounds DISabled");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_HOTKEY_CTRL);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_HOTKEY_CTRL)
			sprintf(tempText,"Hotkeys used with CTRL key");
		else
			sprintf(tempText,"Hotkeys are just pressed");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_WEATHER);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_WEATHER)
			sprintf(tempText,"Weather is displayed");
		else
			sprintf(tempText,"NO weather displayed");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_INFO_GLAMOUR);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_GLAMOUR)
			sprintf(tempText,"Weapon effects shown");
		else
			sprintf(tempText,"Weapon effects NOT shown");
		tButt->SetText(tempText);
	}
	tButt = (UIRectTextButton *) big->childRectList.Find(OM_BUTTON_WASD);
	if (tButt)
	{
		if (localInfoFlags & LOCAL_FLAGS_WASD)
			sprintf(tempText,"WASD moving enabled");
		else
			sprintf(tempText,"WASD moving DISabled");
		tButt->SetText(tempText);
	}
}

//***************************************************************
void OptionMode::SaveLocalSettings(void)
{
	FILE *fp = fopen("localSettings.txt","w");
	if (fp)
	{
		fprintf(fp, "%f %d %ld ", terrainDrawValue, grassDensity, localInfoFlags);
		fprintf(fp, "%d %d\n", PumaSound::playSounds, PumaSound::playUISounds);
		fclose(fp);
	}
}

//***************************************************************
void OptionMode::LoadLocalSettings(void)
{
	FILE *fp = fopen("localSettings.txt","r");
	if (fp)
	{
		fscanf(fp, "%f %d %ld ", &terrainDrawValue, &grassDensity, &localInfoFlags);
		fscanf(fp, "%d %d\n", &PumaSound::playSounds, &PumaSound::playUISounds);
		fclose(fp);
	}
}

//******************************************************************
void OptionMode::SetEnvironment(void)
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

}
/* end of file */



