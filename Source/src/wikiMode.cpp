
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <wininet.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "wikiMode.h"
#include "groundTestMode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_ArtTextButton.h"
#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragBar.h"
#include "./puma/UIR_EditLine.h"
#include "./puma/UIR_BigInfoWindow.h"

#include "clientOnly.h"

#include "BBOClient.h"

#include "BBOServer.h"

enum 
{
	WIKM_BUTTON_RETURN,
	WIKM_BUTTON_BIGBOX,
	WIKM_BUTTON_SCROLLBOX,
	WIKM_BUTTON_SCROLLBAR,
	WIKM_BUTTON_LINK,
	WIKM_BUTTON_TEXT,
	WIKM_BUTTON_MAX
};

//PlasmaTexture *bboClient->groundMap;
extern BBOServer *server;
extern Client *	lclient;

extern BBOClient *bboClient;

extern GroundTestMode *curgroundTestMode;
extern DWORD textColorList[6];


// static pointer that the process function can use
WikiMode *curWikiMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL wikiModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	MessInfoFlags infoFlagsMess;
	UIRectTextButton *tButt;

	if (UIRECT_MOUSE_LUP == type)
	{

		switch(curUIRect->WhatAmI())	
		{
		case WIKM_BUTTON_RETURN :
			button1Sound->PlayNo3D();
			newGameMode = NULL;
			curWikiMode->retState = GMR_POP_ME;
			return 1;
			break;

		case WIKM_BUTTON_LINK :
			button1Sound->PlayNo3D();
			tButt = (UIRectTextButton *) curUIRect;
			curWikiMode->linkUsed = tButt;
			return 1;
			break;

		}
	}
	return 0;  // didn't deal with this message
}

//******************************************************************
//******************************************************************
WikiMode::WikiMode(int doid, char *doname) : GameMode(doid,doname)
{
	curWikiMode = this;
}

//******************************************************************
WikiMode::~WikiMode()
{
	curWikiMode = NULL;
}

//******************************************************************
int WikiMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int WikiMode::Activate(void) // do this when the mode becomes the forbboClient->ground mode.
{
	aLog.Log("WikiMode::Activate ********************\n");

	uiAlpha = 255;
//	char tempText[1024];

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

	scrollWin = new UIRectScrollWindow(WIKM_BUTTON_SCROLLBOX, -1,-1,-1,-1);
   scrollWin->fillStyle = UIRECT_WINDOW_STYLE_ALL;
	scrollWin->fillArt = NULL;
   bigInfo->AddChild(scrollWin);

	UIRectDragBar *dBar = new UIRectDragBar(WIKM_BUTTON_SCROLLBAR, 20, -1,-1,-1);
   dBar->fillStyle = UIRECT_WINDOW_STYLE_ALL;
	dBar->fillArt = NULL;
	dBar->yWidth = 20;
   scrollWin->AddChild(dBar);


//   UIRectTextBox *tBox;

/*
	tBox = new UIRectTextBox(WIKM_BUTTON_TEXT, 0,0,100,100); 
	tBox->process = wikiModeProcess;
	tBox->SetText("TEST\n\n\n\nTsst again");
	tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tBox->fillStyle = UIRECT_WINDOW_STYLE_ALL;
	tBox->fillArt = NULL;
	tBox->font = 2;
   scrollWin->AddChild(tBox);
  */
   tButt = new UIRectTextButton(WIKM_BUTTON_RETURN,
		             centerX-100, puma->ScreenH() - 40,
		             centerX+102, puma->ScreenH() - 40+26);
   tButt->SetText("Return to Options");
   tButt->process = wikiModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ALL;
//	tButt->fillArt = uiPopUpLongArt;
   fullWindow->AddChild(tButt);

	LoadPage("http://www.blademistress.com/wiki/pmwiki.php");

   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff

	return(0);
}

//******************************************************************
int WikiMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	delete fullWindow;
	SAFE_RELEASE(uiPopUpLongArt);
	SAFE_RELEASE(uiPopUpArt);
   SAFE_RELEASE(mouseArt);

	return(0);
}

//******************************************************************
int WikiMode::Tick(void)
{
//	char tempText[1024];

   D3DXMATRIX matWorld, mat2;

	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

	linkUsed = NULL;

	bboClient->Tick();

	HandleMessages();

	WindowServicer();
	// processing

	bboClient->curCamAngle += 0.001f;

	// start drawing

	puma->StartRenderingFrame(0,0,30);

	bboClient->Draw();

	BigInfoWindow *big = (BigInfoWindow *) fullWindow->childRectList.First();

   fullWindow->Draw();

	puma->DrawMouse(mouseArt);

	puma->FinishRenderingFrame();

	if (linkUsed)
	{
		LoadPage(linkUsed->WhoAmI());
	}



	return(0);
}


//*******************************************************************************
long WikiMode::WindowServicer(void)
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
void WikiMode::HandleMessages(void)
{
	char messData[4000];//, tempText[1024];
	int  dataSize;
	FILE *source = NULL;
	MessPlayerChatLine chatMess;
//	MessInfoText *infoText;
//	MessInfoFlags       *infoFlagsPtr;
	DWORD textColor = 0xffffffff;

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
			textColor = D3DCOLOR_RGBA(255,255,255,255);
			if (TEXT_COLOR_ANNOUNCE == messData[1])
			{
				textColor = textColorList[0];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_TELL == messData[1])
			{
				textColor = textColorList[1];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_SHOUT == messData[1])
			{
				textColor = textColorList[2];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_DATA == messData[1])
			{
				textColor = textColorList[3];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_GUILD == messData[1])
			{
				textColor = textColorList[4];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_EMOTE == messData[1])
			{
				textColor = textColorList[5];
				curgroundTestMode->InjectTextLine(&(messData[2]), textColor);
			}
			else
				curgroundTestMode->InjectTextLine(&(messData[1]), textColor);
		   break;

		default:
			bboClient->HandleMessage(messData, dataSize);
			break;

		}
		lclient->GetNextMsg(NULL, dataSize);
	}

}


//******************************************************************
void WikiMode::SetEnvironment(void)
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

// make a big buffer
char wikiPageBuffer[1024 * 100];
int wikiBoxIndex;

//******************************************************************
void WikiMode::LoadPage(char *pageURL)
{
	// open the file and try to load it into the big buffer
	HINTERNET hInternetSession;   
	HINTERNET hURL;
//	char tempText[1024];
	BOOL bResult;
	DWORD dwBytesRead;
   UIRectTextBox *tBox;

	int gotFile = FALSE;
	wikiBoxIndex = 0;

	// Make internet connection.
	hInternetSession = InternetOpen(
							"Microsoft Internet Explorer",   // agent
							INTERNET_OPEN_TYPE_PRECONFIG,      // access
							NULL, NULL, 0);            // defaults

	int openTries = 0;
	while (!hInternetSession && openTries < 20)
	{
		++openTries;
		Sleep(100);
		hInternetSession = InternetOpen(
								"Microsoft Internet Explorer",   // agent
								INTERNET_OPEN_TYPE_PRECONFIG,      // access
								NULL, NULL, 0);            // defaults
	}

	if (hInternetSession)
	{

		// Make connection to desired page.
		hURL = InternetOpenUrl(
					hInternetSession,             // session handle
					pageURL,                      // URL to access
					NULL, 0, 0, 0);               // defaults

		int connectTries = 0;
		while (!hURL && connectTries < 20)
		{
			++connectTries;
			Sleep(100);
		}

		if (hURL)
		{
			int writePtr = 0;
			do
			{
				// read page into memory buffer
				bResult = InternetReadFile(hURL, (LPSTR)&wikiPageBuffer[writePtr],
								(DWORD)4096, &dwBytesRead);

				writePtr += dwBytesRead;
			} while(!bResult || dwBytesRead != 0); 

			wikiPageBuffer[writePtr] = 0; // terminate the buffer
			gotFile = TRUE;

			// close down connections
			InternetCloseHandle(hURL);

		}

		// close down connections
		InternetCloseHandle(hInternetSession);
	}

	// clear out the window
	UIRect *r = (UIRect *) scrollWin->childRectList.First();
	while (r)
	{
		if (WIKM_BUTTON_SCROLLBAR == r->WhatAmI())
		{
			UIRectDragBar *dBar = (UIRectDragBar *) r;
			dBar->yPos = 0;
			dBar->ModifyParent();
			scrollWin->childRectList.Find(r);
			r = (UIRect *) scrollWin->childRectList.Next();
		}
		else
		{
			scrollWin->childRectList.Remove(r);
			delete r;
			r = (UIRect *) scrollWin->childRectList.First();
		}
	}


	// failure?
	if (!gotFile)
	{

		tBox = new UIRectTextBox(WIKM_BUTTON_TEXT, -1,0,-1,100); 
		tBox->process = wikiModeProcess;
		tBox->SetText("Could not download the information page.");
//		tBox->textFlags = D3DFONT_LEFT;
		tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tBox->fillArt = NULL;
		tBox->font = 2;
		scrollWin->AddChild(tBox);
		fullWindow->Arrange();
		return;
	}

	// parse the buffer

	char tBuff[1024], tBuff2[512];
	int copyFlag = TRUE, tBuffIndex;


	tBuffIndex = 0;
	for (int i = 0; i < strlen(wikiPageBuffer);)
	{
		if ('<' == wikiPageBuffer[i])
		{
			tBuff[tBuffIndex] = 0;
			if (tBuffIndex > 0)
			{
				AddTextBox(tBuff, NULL, FALSE);
			}
			tBuffIndex = 0;
			++i;
			if ('a' == wikiPageBuffer[i])
			{
				// get the hyperlink
				i += 8;
				int i2 = 0;
				while ('\'' != wikiPageBuffer[i])
				{
					tBuff2[i2++] = wikiPageBuffer[i++];
				}
				tBuff2[i2] = 0; // terminate the URL text

				++i;
				++i;
				if ('<' != wikiPageBuffer[i])
				{
					i2 = 0;
					while ('<' != wikiPageBuffer[i])
					{
						tBuff[i2++] = wikiPageBuffer[i++];
					}
					tBuff[i2] = 0; // terminate the link description text

					++i;
					++i;
					++i;
					++i;

					AddTextBox(tBuff, tBuff2, TRUE);
				}
				tBuffIndex = 0;
			}
			else if (IsSame(&wikiPageBuffer[i], "head"))
			{
				while(!IsSame(&wikiPageBuffer[i], "/head"))
					++i;
				++i;
				++i;
				++i;
				++i;
				++i;
				++i;

			}
			else
			{
				++i;

				// skip all other <> pairs
				int depth = 1;
				while (depth > 0)
				{
					if ('>' == wikiPageBuffer[i])
						--depth;
					if ('<' == wikiPageBuffer[i])
						++depth;
					++i;
				}
				tBuffIndex = 0;
			}

		}
		else if (13 == wikiPageBuffer[i] || 10 == wikiPageBuffer[i])
		{
			++i;
			tBuff[tBuffIndex] = 0;
			if (tBuffIndex > 0)
			{
				AddTextBox(tBuff, NULL, FALSE);
				/*
				for (int f = 0; f < strlen(tBuff); ++f)
				{
					if (!iswtspc(tBuff[f]))
					{
					}
					*/
			}
			tBuffIndex = 0;
		}
		else
			tBuff[tBuffIndex++] = wikiPageBuffer[i++];


	}


   fullWindow->Arrange();

}

//******************************************************************
void WikiMode::AddTextBox(char *text, char *altText, int isButton)
{
	char tempText[1024];

	// parse the buffer, one line at a time, converting it into textBoxes
	CD3DFont *font = puma->GetDXFont(0);
	int lineStart = 0;
	int buffSize = strlen(text);

	while (lineStart < buffSize)
	{
		int size = font->GetTextFitNum( &(text[lineStart]), 580, TRUE );

		if (size > 0)
		{
			memcpy(tempText, &(text[lineStart]), size);
			tempText[size] = 0;
			lineStart += size;

			if (!isButton)
			{
				UIRectTextBox *tBox = new UIRectTextBox(WIKM_BUTTON_TEXT +wikiBoxIndex, 
												 -1, wikiBoxIndex * 15, -1, wikiBoxIndex * 15 + 15); 
				tBox->process = wikiModeProcess;
				tBox->SetText(tempText);
				tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
				tBox->fillArt = NULL;
				tBox->font = 0;
				scrollWin->AddChild(tBox);
			}
			else
			{
				// make sure we're looking at a wiki link ONLY.

				int isWikiPage = FALSE;
				for (int u = 0; u < strlen(altText); ++u)
				{
					if (IsSame(&altText[u], "tress.com/wiki"))
					{
						isWikiPage = TRUE;
						u = 10000;
					}
				}

				if (isWikiPage)
				{
					UIRectTextButton *tButt = new UIRectTextButton(WIKM_BUTTON_LINK, 
													 -1, wikiBoxIndex * 15, -1, wikiBoxIndex * 15 + 15); 
					tButt->process = wikiModeProcess;
					tButt->SetText(tempText);
					tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
					tButt->fillArt = NULL;
					tButt->font = 0;
					sprintf(tButt->do_name, altText);
					scrollWin->AddChild(tButt);
				}
				else
					--wikiBoxIndex; // compensate; we didn't want this to change.
			}
			++wikiBoxIndex;
		}
		else if (lineStart < buffSize)
		{
			size = 580;
			if (size > buffSize-lineStart)
				size = buffSize-lineStart;

			memcpy(tempText, &(text[lineStart]), size);
			tempText[size] = 0;
			lineStart += size;

			if (!isButton)
			{
				UIRectTextBox *tBox = new UIRectTextBox(WIKM_BUTTON_TEXT +wikiBoxIndex, 
					                      -1, wikiBoxIndex * 15, -1, wikiBoxIndex * 15 + 15); 
				tBox->process = wikiModeProcess;
				tBox->SetText(tempText);
				tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
				tBox->fillArt = NULL;
				tBox->font = 0;
				scrollWin->AddChild(tBox);
			}

			++wikiBoxIndex;
		}
		else
		{
			lineStart = buffSize;
		}
	}

}

/* end of file */



