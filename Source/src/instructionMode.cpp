
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "InstructionMode.h"
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

char *inst1Text = "Welcome to Blade Mistress!\n\n\
Turn using the left and right arrow keys, or by holding the right mouse button\n\
down.  To move forward a square in the direction you are facing, press the up arrow\n\
key or click on the square of land in front of you.\n\n\
When you are in the same square as a monster, you will automatically fight it until\n\
one combatant is defeated, or you move to another square.  Defeated monsters leave\n\
bags of treasure behind.  You can click-and-hold on this bag, and choose the\n\
Trade option.  Then an inventory box with the monster's treasure will appear in the\n\
upper-left of the screen.  You can click-and-hold on these treasure items to Get them.\n\n\
When you are in the same square as a Merchant (a smaller person in blue), you can\n\
click-and-hold on them, and choose the Trade option.  You can then buy things from them,\n\
like swords, and click-and-hold items from your inventory (in the upper right) and sell them.\n\
If you move into a large cave-like object, you will enter a dungeon.  Here you won't find\n\
any merchants to trade with, but you will find plenty of monsters to fight.  Exit the dungeon\n\
by moving to the 0,0 position, where you entered.\n\n\
You have four buttons in the upper-right corner; they show you your Inventory, Used, Workbench, and Skills lists.\n\
Inventory is all the things you hold.  You can sell and use these items.  You can also Use items, which\n\
places them into the Used list.  Items in the Used list are wielded, or activated.\n\
You can also Include items from Inventory into your Workbench, if you want to do crafting or\n\
magic on them.  Your Skills, learned from a Trainer, are usually used to affect items you put in\n\
the workbench.\n\n\
Hit Enter, and type a message.  Send the message by pressing Enter again. This message will go out to\n\
everyone near you, and you can see what they have to say back in the chat window in the lower-left corner.";

char *inst2Text = "Trainers live in towns, like Merchants, but are in green.  Trainers teach you skills.  These skills\n\
are used in different ways, depending on the skill, but there are three basic types.\n\
Dodging is a skill that helps you in combat.  You don't have to do anything special to use it.\n\
Swordcrafting allows you to make swords from ingots.  Buy several ingots, Include them into your\n\
workbench, and use your Swordcrafting skill's Combine option.  If did it right, you'll make a sword!\n\
Magic uses eight different magic skills, and totems.  Buy a totem, Include it in your workbench, and\n\
use the Combine option of your different magic skills to imbue the totem with magic.  Use different\n\
amounts of different magic skills to make different kinds of totem.";

char *inst3Text = "That code WAS valid, but it has already been used up.\n\
please purchase another code.\n\n\
You can purchase a code which will give you 31 more days of playing time, by\n\
going to the www.blademistress.com website, and following the PAYMENT link.\n\n\
You can choose to pay with a credit card, Paypal, check, money order, or even cash!\n\
After receiving your payment, DigitalRiver will e-mail you your code right away, so\n\
make sure you give them your correct e-mail address.\n\n\
Once you have the e-mail with the code in it, log on to the game and type in the code\n\
in the Character Selection screen.\n\n\
Currently 31 days of time costs $5us.\n\n\
Thank you for playing Blade Mistress!";



enum 
{
	INSTM_BUTTON_RETURN,
	INSTM_BUTTON_TEXT
};

//PlasmaTexture *bboClient->groundMap;
extern BBOServer *server;
extern Client *	lclient;

extern int playerInDungeon;

extern BBOClient *bboClient;

extern GroundTestMode *curgroundTestMode;


// static pointer that the process function can use
InstructionMode *curInstructionMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL instructionModeProcess(UIRect *curUIRect, int type, long x, short y)
{

	if (UIRECT_MOUSE_LUP == type)
	{

		switch(curUIRect->WhatAmI())	
		{
		case INSTM_BUTTON_RETURN :
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curInstructionMode->retState = GMR_POP_ME;
			return 1;
			break;

		}
	}
	return 0;  // didn't deal with this message
}

//******************************************************************
//******************************************************************
InstructionMode::InstructionMode(int doid, char *doname) : GameMode(doid,doname)
{
	curInstructionMode = this;
}

//******************************************************************
InstructionMode::~InstructionMode()
{
	curInstructionMode = NULL;
}

//******************************************************************
int InstructionMode::Init(void)  // do this when instantiated.
{
	return(0);
}

const int INSTM_UI_ALPHA = 100;

//******************************************************************
int InstructionMode::Activate(void) // do this when the mode becomes the forbboClient->ground mode.
{
	aLog.Log("InstructionMode::Activate ********************\n");

	uiAlpha = INSTM_UI_ALPHA;

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

   tButt = new UIRectTextButton(INSTM_BUTTON_RETURN,puma->ScreenW()/2-76,puma->ScreenH()/2+200,
		                                           puma->ScreenW()/2+77,puma->ScreenH()/2+226);
   tButt->SetText("Return");
   tButt->process = instructionModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ALL;
	tButt->fillArt = uiPopUpArt;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;
   tBox = new UIRectTextBox(INSTM_BUTTON_TEXT,2,20,
		                                          puma->ScreenW()-2,puma->ScreenH()/2+190);
	if (2 == WhatAmI())
	   tBox->SetText(inst2Text);
	else if (3 == WhatAmI())
	   tBox->SetText(inst3Text);
	else
	   tBox->SetText(inst1Text);

   tBox->process = instructionModeProcess;
//	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(tBox);


   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff

	return(0);
}

//******************************************************************
int InstructionMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;
   SAFE_RELEASE(uiPopUpArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int InstructionMode::Tick(void)
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
long InstructionMode::WindowServicer(void)
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
void InstructionMode::HandleMessages(void)
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



