
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "loginmode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "./puma/UIR_TextButton.h"
#include "./puma/UIR_TextBox.h"
#include "./puma/UIR_ScrollWindow.h"
#include "./puma/UIR_DragEdge.h"
#include "./puma/UIR_EditLine.h"

#include "./network/NetWorldMessages.h"
#include "simpleMessageMode.h"
#include "groundTestMode.h"

#include "BBOServer.h"
#include "./network/client.h"
#include "./helper/PasswordHash.h"
#include ".\helper\crypto.h"

enum 
{
    LM_BUTTON_NEW_NAME,
    LM_BUTTON_NEW_PASS,
    LM_BUTTON_NEW_SUBMIT,
    LM_BUTTON_OLD_NAME,
    LM_BUTTON_OLD_PASS,
    LM_BUTTON_OLD_SUBMIT,
    LM_BUTTON_QUIT,
    LM_BUTTON_TEXT
};

extern BBOServer *server;
extern Client *	lclient;

extern int playerAvatarID;


// static pointer that the process function can use
LoginMode *curLoginMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL LoginModeProcess(UIRect *curUIRect, int type, long x, short y)
{ 
    UIRectEditLine *edName, *edPass;
    MessPlayerNew messPlayerNew;
    MessPlayerReturning messPlayerReturning;


    switch(curUIRect->WhatAmI())	
    {
    case LM_BUTTON_QUIT :
      if (UIRECT_MOUSE_LUP == type)
      {
            newGameMode = NULL;
            curLoginMode->retState = GMR_POP_ME;
      }
        break;

    case LM_BUTTON_NEW_SUBMIT :
      if (UIRECT_MOUSE_LUP == type)
      {
            edName = (UIRectEditLine *) curLoginMode->fullWindow->childRectList.Find(LM_BUTTON_NEW_NAME);
            edPass = (UIRectEditLine *) curLoginMode->fullWindow->childRectList.Find(LM_BUTTON_NEW_PASS);
            if (edName->text[0] && edPass->text[0])
            {
                sprintf_s(messPlayerNew.name, NUM_OF_CHARS_FOR_USERNAME, "%s", edName->text);

                unsigned char salt[256];
                sprintf_s((char*)&salt[0], 256, "%s-%s", "BladeMistress", edName->text);

                // Hash the password
				unsigned char hashPass[HASH_BYTE_SIZE] = { 0 };
                if (!PasswordHash::CreateStandaloneHash((const unsigned char*)edPass->text, salt, 6969, hashPass))
                {
                    newGameMode = new SimpleMessageMode("There was an error sending your credentials to the server.\nPlease contact your server admin.", 0, "SIMPLE_MESSAGE_MODE");
                    curLoginMode->retState = GMR_NEW_MODE;
                    return 0;
                }

				// Send hashed password
				memcpy(messPlayerNew.pass, hashPass, HASH_BYTE_SIZE);
				messPlayerNew.pass[HASH_BYTE_SIZE] = 0;

                messPlayerNew.uniqueId = GetUniqueComputerId();

                char *fomText1 = "Connecting...";
                codePad = fomText1;
                CryptoString(messPlayerNew.name);

                lclient->SendMsg(sizeof(messPlayerNew),(void *)&messPlayerNew);
            }

      }
        break;

    case LM_BUTTON_OLD_SUBMIT :
      if (UIRECT_MOUSE_LUP == type)
      {
            edName = (UIRectEditLine *) curLoginMode->fullWindow->childRectList.Find(LM_BUTTON_OLD_NAME);
            edPass = (UIRectEditLine *) curLoginMode->fullWindow->childRectList.Find(LM_BUTTON_OLD_PASS);
            if (edName->text[0] && edPass->text[0])
            {
                sprintf_s(messPlayerReturning.name, NUM_OF_CHARS_FOR_USERNAME, "%s", edName->text);

                unsigned char salt[256];
                sprintf_s((char*)&salt[0], 256, "%s-%s", "BladeMistress", edName->text);

                // Hash the password
				unsigned char hashPass[HASH_BYTE_SIZE] = { 0 };
                if (!PasswordHash::CreateStandaloneHash((const unsigned char*)edPass->text, salt, 6969, hashPass))
                {
                    newGameMode = new SimpleMessageMode("There was an error sending your credentials to the server.\nPlease contact your server admin.", 0, "SIMPLE_MESSAGE_MODE");
                    curLoginMode->retState = GMR_NEW_MODE;
                    return 0;
                }

				// Send hashed password
				memcpy(messPlayerNew.pass, hashPass, HASH_BYTE_SIZE);
				messPlayerNew.pass[HASH_BYTE_SIZE] = 0;

                messPlayerReturning.uniqueId = GetUniqueComputerId();

                char *fomText1 = "Connecting...";
                codePad = fomText1;
                CryptoString(messPlayerNew.name);

                lclient->SendMsg(sizeof(messPlayerReturning),(void *)&messPlayerReturning);
            }

      }
        break;
    }
    return 0;  // didn't deal with this message
}



//******************************************************************
LoginMode::LoginMode(int doid, char *doname) : GameMode(doid,doname)
{
    curLoginMode = this;

}

//******************************************************************
LoginMode::~LoginMode()
{
}

//******************************************************************
int LoginMode::Init(void)  // do this when instantiated.
{
    return(0);
}

//******************************************************************
int LoginMode::Activate(void) // do this when the mode becomes the forground mode.
{

    // init camera position
    puma->pumaCamera->spacePoint.angle = -1.0f * D3DX_PI/2;
    puma->pumaCamera->spacePoint.azimuth = -0.37f;
    puma->pumaCamera->spacePoint.location.x = 165.0f;
    puma->pumaCamera->spacePoint.location.y = -66.0f;
    puma->pumaCamera->spacePoint.location.z = -125.0f;

    // Set up a white, directional light.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
//	D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -4,0,-2);
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, 4,0,2);
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

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(LM_BUTTON_NEW_SUBMIT,
                               puma->ScreenW() - 100,100  ,puma->ScreenW() - 2,150);
   tButt->SetText("New Account");
   tButt->process = LoginModeProcess;
   fullWindow->AddChild(tButt);
   UIRectStep(0,2);

   tButt = new UIRectTextButton(LM_BUTTON_OLD_SUBMIT, -2, -2, -2, -2);
   tButt->SetText("Existing Account");
   tButt->process = LoginModeProcess;
   fullWindow->AddChild(tButt);
   UIRectStep(0,2);

   tButt = new UIRectTextButton(LM_BUTTON_QUIT, -2, -2, -2, -2);
   tButt->SetText("Cancel Login");
   tButt->process = LoginModeProcess;
   fullWindow->AddChild(tButt);

   UIRectTextBox *tBox;

    //***************
    UIRectEditLine *edLine	  = new UIRectEditLine(LM_BUTTON_NEW_NAME, 
                                    puma->ScreenW() - 300, 100  ,puma->ScreenW() - 102, 120,31);
    edLine->process = LoginModeProcess;
    edLine->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
    edLine->SetText("");
    fullWindow->AddChild(edLine);

    edLine	  = new UIRectEditLine(LM_BUTTON_NEW_PASS, 
                                    puma->ScreenW() - 300, 122  ,puma->ScreenW() - 102, 145,11);
    edLine->process = LoginModeProcess;
    edLine->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
    edLine->SetText("");
    fullWindow->AddChild(edLine);

   tBox = new UIRectTextBox(LM_BUTTON_TEXT,puma->ScreenW() - 400, 100  ,puma->ScreenW() - 302, 120);
   tBox->SetText("User Name");
   tBox->process = LoginModeProcess;
   fullWindow->AddChild(tBox);

   tBox = new UIRectTextBox(LM_BUTTON_TEXT,puma->ScreenW() - 400, 122  ,puma->ScreenW() - 302, 145);
   tBox->SetText("Password");
   tBox->process = LoginModeProcess;
   fullWindow->AddChild(tBox);

    //***************
    edLine	  = new UIRectEditLine(LM_BUTTON_OLD_NAME, 
                                    puma->ScreenW() - 300, 200  ,puma->ScreenW() - 102, 220,31);
    edLine->process = LoginModeProcess;
    edLine->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
    edLine->SetText("");
    fullWindow->AddChild(edLine);

    edLine	  = new UIRectEditLine(LM_BUTTON_OLD_PASS, 
                                    puma->ScreenW() - 300, 222  ,puma->ScreenW() - 102, 245,11);
    edLine->process = LoginModeProcess;
    edLine->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
    edLine->SetText("");
    fullWindow->AddChild(edLine);

   tBox = new UIRectTextBox(LM_BUTTON_TEXT,puma->ScreenW() - 400, 200  ,puma->ScreenW() - 302, 220);
   tBox->SetText("User Name");
   tBox->process = LoginModeProcess;
   fullWindow->AddChild(tBox);

   tBox = new UIRectTextBox(LM_BUTTON_TEXT,puma->ScreenW() - 400, 222  ,puma->ScreenW() - 302, 245);
   tBox->SetText("Password");
   tBox->process = LoginModeProcess;
   fullWindow->AddChild(tBox);

    //***************
   tBox = new UIRectTextBox(LM_BUTTON_TEXT,2,puma->ScreenH() - 100,630,puma->ScreenH()-2);
   tBox->SetText("If you want to start a new account, enter your user name and password in the\nupper two boxes, then click NEW ACCOUNT.");
   tBox->process = LoginModeProcess;
   fullWindow->AddChild(tBox);

   fullWindow->Arrange();

   // ********** finished building UIRects



    return(0);
}

//******************************************************************
int LoginMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

    delete fullWindow;

//   SAFE_RELEASE(backArt);
   SAFE_RELEASE(mouseArt);

    return(0);
}

//******************************************************************
int LoginMode::Tick(void)
{
   D3DXMATRIX matWorld, mat2;
//	if (server)
//		server->Tick();
    HandleMessages();

    WindowServicer();
    // processing


    // start drawing

    puma->StartRenderingFrame(11,63,60);

    D3DSURFACE_DESC desc;
    backArt->GetLevelDesc(0, &desc);

    RECT dRect = {0,0,desc.Width,desc.Height};
    puma->DrawRect(backArt,dRect,0xffffffff);

   fullWindow->Draw();

    puma->DrawMouse(mouseArt);

    puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

    return(0);
}


//*******************************************************************************
long LoginMode::WindowServicer(void)
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
void LoginMode::HandleMessages(void)
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
        
        MessEmpty *empty = (MessEmpty *)messData;
        switch (messData[0])
        {

        case NWMESS_GENERAL_NO:
            messNoPtr = (MessGeneralNo *) messData;
            if (-1 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("User names must start with a letter or number.",0,"LOGIN_MODE");
                retState = GMR_NEW_MODE;
            }
            if (1 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("Sorry, that user name is already taken.\nPlease choose another.",0,"LOGIN_MODE");
                retState = GMR_NEW_MODE;
            }
            if (2 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("Sorry, we could not create that user file.\nPlease try again.",0,"LOGIN_MODE");
                retState = GMR_NEW_MODE;
            }
            if (3 == messNoPtr->subType)
            {
                newGameMode = new SimpleMessageMode("The password is incorrect for this user.\nPlease try again.",0,"LOGIN_MODE");
                retState = GMR_NEW_MODE;
            }
           break;

        case NWMESS_GENERAL_YES:
            messYesPtr = (MessGeneralYes *) messData;

            newGameMode = new GroundTestMode(messYesPtr->subType, 0, "GROUND_TEST_MODE");
            retState = GMR_POP_ME;
            return; // don't get any more messages.
           break;
        }
        
        lclient->GetNextMsg(NULL, dataSize);
    }

}



/* end of file */



