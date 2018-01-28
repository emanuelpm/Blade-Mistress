
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "groundTestmode.h"
#include "optionMode.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"
#include "BBOSquare.h"
#include "MonsterData.h"

#include "./puma/UIR_ChatWindow.h"
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

#include ".\helper\autolog.h"

enum 
{
	GTM_BUTTON_QUIT,
	GTM_BUTTON_TEXT,
	GTM_BUTTON_COMPASS,
	GTM_BUTTON_COMPASS2,
	GTM_BUTTON_STATS,
	GTM_BUTTON_CHATBOX,
	GTM_BUTTON_REPORTBUG,
	GTM_BUTTON_MONSTERHEALTH,
	GTM_BUTTON_MOVELEFT,
	GTM_BUTTON_MOVERIGHT,
	GTM_BUTTON_MOVEUP,

	GTM_BUTTON_BUGWIN,
	GTM_BUTTON_BUGTYPE_CRASH,
	GTM_BUTTON_BUGTYPE_HANG,
	GTM_BUTTON_BUGTYPE_ART,
	GTM_BUTTON_BUGTYPE_GAMEPLAY,
	GTM_BUTTON_BUGTYPE_OTHER,
	GTM_BUTTON_BUGDOING,
	GTM_BUTTON_BUGPLAYLENGTH,
	GTM_BUTTON_BUGREPEATABLE,
	GTM_BUTTON_BUGINFO,
	GTM_BUTTON_BUGSUBMIT,
	GTM_BUTTON_BUGCANCEL,

	GTM_BUTTON_INFOWIN,
	GTM_BUTTON_INFODRAG,

	GTM_BUTTON_STATSWIN,
	GTM_BUTTON_STATSDRAG,
	GTM_BUTTON_STATSNAME,
	GTM_BUTTON_STATSINFO,
	GTM_BUTTON_STATSHEALTH,
	GTM_BUTTON_STATSITEMS,
	GTM_BUTTON_STATSSKILLS,
	GTM_BUTTON_STATSUP,
	GTM_BUTTON_STATSDOWN,
	GTM_BUTTON_STATSLINE0,
	GTM_BUTTON_STATSLINE1,
	GTM_BUTTON_STATSLINE2,
	GTM_BUTTON_STATSLINE3,
	GTM_BUTTON_STATSLINE4,
	GTM_BUTTON_STATSLINE5,
	GTM_BUTTON_STATSLINE6,
	GTM_BUTTON_STATSLINE7,
	GTM_BUTTON_STATSLINE8,
	GTM_BUTTON_STATSLINE9,
	GTM_BUTTON_STATSLINE10,
	GTM_BUTTON_STATSTEXT,

	GTM_BUTTON_OTHERWIN,
	GTM_BUTTON_OTHERNAME,
	GTM_BUTTON_OTHERUP,
	GTM_BUTTON_OTHERDOWN,
	GTM_BUTTON_OTHERLINE0,
	GTM_BUTTON_OTHERLINE1,
	GTM_BUTTON_OTHERLINE2,
	GTM_BUTTON_OTHERLINE3,
	GTM_BUTTON_OTHERLINE4,
	GTM_BUTTON_OTHERLINE5,
	GTM_BUTTON_OTHERLINE6,
	GTM_BUTTON_OTHERLINE7,
	GTM_BUTTON_OTHERLINE8,
	GTM_BUTTON_OTHERLINE9,

	GTM_BUTTON_BANKWIN,
	GTM_BUTTON_BANKNAME,

	GTM_BUTTON_YOU_SECURE_WIN,
	GTM_BUTTON_YOU_SECURE_NAME,
	GTM_BUTTON_YOU_SECURE_UP,
	GTM_BUTTON_YOU_SECURE_DOWN,
	GTM_BUTTON_YOU_SECURE_LINE0,
	GTM_BUTTON_YOU_SECURE_LINE1,
	GTM_BUTTON_YOU_SECURE_LINE2,
	GTM_BUTTON_YOU_SECURE_LINE3,
	GTM_BUTTON_YOU_SECURE_LINE4,
	GTM_BUTTON_YOU_SECURE_LINE5,
	GTM_BUTTON_YOU_SECURE_LINE6,
	GTM_BUTTON_YOU_SECURE_LINE7,
	GTM_BUTTON_YOU_SECURE_LINE8,
	GTM_BUTTON_YOU_SECURE_LINE9,

	GTM_BUTTON_HER_SECURE_WIN,
	GTM_BUTTON_HER_SECURE_NAME,
	GTM_BUTTON_HER_SECURE_UP,
	GTM_BUTTON_HER_SECURE_DOWN,
	GTM_BUTTON_HER_SECURE_LINE0,
	GTM_BUTTON_HER_SECURE_LINE1,
	GTM_BUTTON_HER_SECURE_LINE2,
	GTM_BUTTON_HER_SECURE_LINE3,
	GTM_BUTTON_HER_SECURE_LINE4,
	GTM_BUTTON_HER_SECURE_LINE5,
	GTM_BUTTON_HER_SECURE_LINE6,
	GTM_BUTTON_HER_SECURE_LINE7,
	GTM_BUTTON_HER_SECURE_LINE8,
	GTM_BUTTON_HER_SECURE_LINE9,

	GTM_BUTTON_SECURE_DECLINE,
	GTM_BUTTON_SECURE_ACCEPT,

	GTM_BUTTON_DONAME,
	GTM_BUTTON_DO1,
	GTM_BUTTON_DO2,
	GTM_BUTTON_DO3,
	GTM_BUTTON_DO4,
	GTM_BUTTON_DO5,
	GTM_BUTTON_DO6,
	GTM_BUTTON_DO7,
	GTM_BUTTON_DO8,
	GTM_BUTTON_DO9,
	GTM_BUTTON_DO10,
	GTM_BUTTON_DO11,
	GTM_BUTTON_DO12,
	GTM_BUTTON_DO13,
	GTM_BUTTON_DO14,
	GTM_BUTTON_DO15,
	GTM_BUTTON_DO16,
	GTM_BUTTON_DO17,
	GTM_BUTTON_DO18,
	GTM_BUTTON_DO_SPACE,

	GTM_BUTTON_BOTTOMNAME,

	GTM_BUTTON_DUNEDIT_FLOOR,
	GTM_BUTTON_DUNEDIT_WALL,
	GTM_BUTTON_DUNEDIT_OUTER,
	GTM_BUTTON_DUNEDIT_CAMERA,
//	GTM_BUTTON_LIST_INV,       // now in BBO.h
//	GTM_BUTTON_LIST_WRK,
//	GTM_BUTTON_LIST_SKL,
	GTM_BUTTON_GIVEMONEYTITLE,
	GTM_BUTTON_GIVEMONEYEDLINE,
	GTM_BUTTON_NAMEPET_TITLE,
	GTM_BUTTON_NAMEPET_EDLINE,

	GTM_BUTTON_ADMIN_RELEASE,
	GTM_BUTTON_ADMIN_INFO,

	GTM_BUTTON_ITEM_INFO_TEXT,

	GTM_BUTTON_TIP_TEXT,

	GTM_BUTTON_CHANNEL1,
	GTM_BUTTON_CHANNEL2,
	GTM_BUTTON_CHANNEL3,

	GTM_BUTTON_CHATLINE_0
};

//PlasmaTexture *bboClient->groundMap;
extern BBOServer *server;
extern Client *	lclient;

extern int curMapType, playerIsAdmin;

extern int skillHotKeyArray[19];

extern int POMSezQuit;

BBOClient *bboClient;

char dirSet[17] = "N NWW SWS SEE NE";

//long screenHealth, screenMaxHealth, moveLockoutTimer, monsterHealthLength;
long moveLockoutTimer;
int logDoneCounter;

long gGamePlayerLevel = 0, gGamePlayerHP = 0, gGamePlayerMaxHP = 0;
char gGamePlayerAge = 1, gTeleportHappenedFlag = FALSE;

DWORD textColorList[6];

// static pointer that the process function can use
GroundTestMode *curgroundTestMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL groundTestModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	UIRectEditLine *tEdit;
	UIRectTextBox *tBox;
   UIRectTextButton *tButt;
//	char tempText[128];
	int i, tempX;
//	float fval;
//	long val;  
	MessBugReport bugReport;
	MessInventoryRequestInfo requestInfo;
	MessInventoryTransferRequest transferRequest;
	MessWield messWield;
	MessUnWield messUnWield;
	MessAvatarAttack messAA;
	MessAvatarMoveRequest bMoveReq;
	MessInventoryChange messChange;
	MessTryCombine messTryCombine;
	MessDungeonChange messDungeonChange;
	MessRequestTownMageService messTMS;
	MessChestInfo chestInfo;
	MessTalkToTree messTree;
	MessFeedPetRequest feedPetRequest;
	MessAdminMessage adminMess;
	MessSellAll sellAllMess;
	MessSecureTrade secureTrade;
	MessSetBomb setBomb;
	MessChatChannel chatChannel;


	//***********

	if (UIRECT_KEY_DOWN == type)
	{
//		case WM_KEYDOWN:
		;
	}

	switch(curUIRect->WhatAmI())	
	{
	case GTM_BUTTON_CHATLINE_0:
		if (UIRECT_DONE == type)
		{
			tEdit = (UIRectEditLine *) curUIRect;
			if (strlen(tEdit->text) > 0)
			{
				Chronos::BStream *	stream		= NULL;
				stream	= new Chronos::BStream(300);

				*stream << (unsigned char) NWMESS_PLAYER_CHAT_LINE; 
//				*stream << tEdit->text;
				stream->write(tEdit->text, strlen(tEdit->text));
				*stream << (unsigned char) 0; 
				lclient->SendMsg(stream->used(), stream->buffer());

				delete stream;
			}
			button2Sound->PlayNo3D();
			tEdit->SetText("");
			tEdit->hasTypingFocus = FALSE;
			return 1;
		}
		break;

	case GTM_BUTTON_GIVEMONEYEDLINE:
		if (UIRECT_DONE == type)
		{
			tEdit = (UIRectEditLine *) curUIRect;
			if (strlen(tEdit->text) > 0)
			{
				if (curgroundTestMode->fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
				{
					MessAvatarSendMoney messMoney;
					messMoney.amount = atol(tEdit->text);
					messMoney.avatarID = -1;
					messMoney.targetAvatarID = -2;
					messMoney.x = -3;
					messMoney.y = -4;

					lclient->SendMsg(sizeof(messMoney),(void *)&messMoney);
					button2Sound->PlayNo3D();
				}
				else if (curgroundTestMode->fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				{
					MessAvatarSendMoney messMoney;
					messMoney.amount = atol(tEdit->text);
					messMoney.avatarID = -1;
					messMoney.targetAvatarID = -3;
					messMoney.x = -3;
					messMoney.y = -5;

					lclient->SendMsg(sizeof(messMoney),(void *)&messMoney);
					button2Sound->PlayNo3D();
				}
				else if (bboClient->selectedMOB != bboClient->playerAvatar &&
					 bboClient->playerAvatar && bboClient->selectedMOB &&
					 SMOB_AVATAR == bboClient->selectedMOB->type)
				{
					MessAvatarSendMoney messMoney;
					messMoney.amount = atol(tEdit->text);
					messMoney.avatarID = bboClient->playerAvatar->avatarID;
					messMoney.targetAvatarID = bboClient->selectedMOB->avatarID;
					messMoney.x = bboClient->playerAvatar->cellX;
					messMoney.y = bboClient->playerAvatar->cellY;

					lclient->SendMsg(sizeof(messMoney),(void *)&messMoney);
					button2Sound->PlayNo3D();
				}

//				MessPlayerChatLine chatMess;
//				sprintf(chatMess.text,"Giving %d gold!",atoi(tEdit->text));
//				lclient->SendMsg(sizeof(chatMess),(void *)&chatMess);
			}
			tEdit->SetText("");
			tEdit->hasTypingFocus = FALSE;
			curgroundTestMode->giveMoneyMode = FALSE;
			return 1;
		}
		break;

	case GTM_BUTTON_NAMEPET_EDLINE:
		if (UIRECT_DONE == type)
		{
			tEdit = (UIRectEditLine *) curUIRect;
			if (strlen(tEdit->text) > 0)
			{
				if (bboClient->playerAvatar)
				{
					MessPetName messPN;
					CopyStringSafely(tEdit->text,16,messPN.text,16);
					lclient->SendMsg(sizeof(messPN),(void *)&messPN);
					button2Sound->PlayNo3D();
				}

//				MessPlayerChatLine chatMess;
//				sprintf(chatMess.text,"Giving %d gold!",atoi(tEdit->text));
//				lclient->SendMsg(sizeof(chatMess),(void *)&chatMess);
			}
			tEdit->SetText("");
			tEdit->hasTypingFocus = FALSE;
			curgroundTestMode->namePetMode = FALSE;
			return 1;
		}
		break;

	case GTM_BUTTON_MOVELEFT:
		if (UIRECT_MOUSE_LUP == type)
			keysPressed &= ~(KEY_LEFT);
		if (UIRECT_MOUSE_LDOWN == type)
			keysPressed |= KEY_LEFT;
		return 1;
		break;

	case GTM_BUTTON_MOVERIGHT:
		if (UIRECT_MOUSE_LUP == type)
			keysPressed &= ~(KEY_RIGHT);
		if (UIRECT_MOUSE_LDOWN == type)
			keysPressed |= KEY_RIGHT;
		return 1;
		break;

	}



	//***********




	if (UIRECT_MOUSE_LDOWN == type)
	{
		switch(curUIRect->WhatAmI())	
		{
		case GTM_BUTTON_STATSLINE0:
		case GTM_BUTTON_STATSLINE1:
		case GTM_BUTTON_STATSLINE2:
		case GTM_BUTTON_STATSLINE3:
		case GTM_BUTTON_STATSLINE4:
		case GTM_BUTTON_STATSLINE5:
		case GTM_BUTTON_STATSLINE6:
		case GTM_BUTTON_STATSLINE7:
		case GTM_BUTTON_STATSLINE8:
		case GTM_BUTTON_STATSLINE9:
		case GTM_BUTTON_STATSLINE10:

			i = curUIRect->WhatAmI() - GTM_BUTTON_STATSLINE0;

			// work out what we should do in response to this button press.
			tButt = (UIRectTextButton *) curUIRect;
			if (tButt->text[0])
			{
				curgroundTestMode->UpdateActionButtons(i, TRUE, x, y, GTM_BUTTON_STATSLINE0);
			}

			return 1;
			break;

		case GTM_BUTTON_OTHERLINE0:
		case GTM_BUTTON_OTHERLINE1:
		case GTM_BUTTON_OTHERLINE2:
		case GTM_BUTTON_OTHERLINE3:
		case GTM_BUTTON_OTHERLINE4:
		case GTM_BUTTON_OTHERLINE5:
		case GTM_BUTTON_OTHERLINE6:
		case GTM_BUTTON_OTHERLINE7:
		case GTM_BUTTON_OTHERLINE8:
		case GTM_BUTTON_OTHERLINE9:

			i = curUIRect->WhatAmI() - GTM_BUTTON_OTHERLINE0;

			// work out what we should do in response to this button press.
			tButt = (UIRectTextButton *) curUIRect;
			if (tButt->text[0])
			{
				curgroundTestMode->UpdateActionButtons(i, FALSE, x, y, GTM_BUTTON_OTHERLINE0);
			}

			return 1;
			break;

		case GTM_BUTTON_YOU_SECURE_LINE0:
		case GTM_BUTTON_YOU_SECURE_LINE1:
		case GTM_BUTTON_YOU_SECURE_LINE2:
		case GTM_BUTTON_YOU_SECURE_LINE3:
		case GTM_BUTTON_YOU_SECURE_LINE4:
		case GTM_BUTTON_YOU_SECURE_LINE5:
		case GTM_BUTTON_YOU_SECURE_LINE6:
		case GTM_BUTTON_YOU_SECURE_LINE7:
		case GTM_BUTTON_YOU_SECURE_LINE8:
		case GTM_BUTTON_YOU_SECURE_LINE9:

			i = curUIRect->WhatAmI() - GTM_BUTTON_YOU_SECURE_LINE0;

			// work out what we should do in response to this button press.
			tButt = (UIRectTextButton *) curUIRect;
			if (tButt->text[0])
			{
				curgroundTestMode->UpdateActionButtons(i, FALSE, x, y, GTM_BUTTON_YOU_SECURE_LINE0);
			}

			return 1;
			break;

		case GTM_BUTTON_QUIT :
		case GTM_BUTTON_REPORTBUG :
		case GTM_BUTTON_BUGSUBMIT :
		case GTM_BUTTON_BUGCANCEL :
		case GTM_BUTTON_BUGWIN:
		case GTM_BUTTON_BUGTYPE_CRASH:
		case GTM_BUTTON_BUGTYPE_HANG:
		case GTM_BUTTON_BUGTYPE_ART:
		case GTM_BUTTON_BUGTYPE_GAMEPLAY:
		case GTM_BUTTON_BUGTYPE_OTHER:
		case GTM_BUTTON_BUGDOING:
		case GTM_BUTTON_BUGPLAYLENGTH:
		case GTM_BUTTON_BUGREPEATABLE:
		case GTM_BUTTON_BUGINFO:
		case GTM_BUTTON_STATSUP:
		case GTM_BUTTON_STATSDOWN:
		case GTM_BUTTON_LIST_INV:
		case GTM_BUTTON_LIST_WRK:
		case GTM_BUTTON_LIST_SKL:
		case GTM_BUTTON_LIST_WLD:
		case GTM_BUTTON_DUNEDIT_WALL:
		case GTM_BUTTON_DUNEDIT_OUTER:
		case GTM_BUTTON_DUNEDIT_FLOOR:
		case GTM_BUTTON_DUNEDIT_CAMERA:
		case GTM_BUTTON_OTHERUP:
		case GTM_BUTTON_OTHERDOWN:
		case GTM_BUTTON_YOU_SECURE_UP:
		case GTM_BUTTON_YOU_SECURE_DOWN:
		case GTM_BUTTON_HER_SECURE_UP:
		case GTM_BUTTON_HER_SECURE_DOWN:
		case GTM_BUTTON_MOVEUP:
		case GTM_BUTTON_DO1:
		case GTM_BUTTON_DO2:
		case GTM_BUTTON_DO3:
		case GTM_BUTTON_DO4:
		case GTM_BUTTON_DO5:
		case GTM_BUTTON_DO6:
		case GTM_BUTTON_DO7:
		case GTM_BUTTON_DO8:
		case GTM_BUTTON_DO9:
		case GTM_BUTTON_DO10:
		case GTM_BUTTON_DO11:
		case GTM_BUTTON_DO12:
		case GTM_BUTTON_DO13:
		case GTM_BUTTON_DO14:
		case GTM_BUTTON_DO15:
		case GTM_BUTTON_DO16:
		case GTM_BUTTON_DO17:
		case GTM_BUTTON_DO18:
		case GTM_BUTTON_CHATLINE_0:
		case GTM_BUTTON_CHATBOX:
		case GTM_BUTTON_SECURE_DECLINE:
		case GTM_BUTTON_SECURE_ACCEPT:
		case GTM_BUTTON_YOU_SECURE_NAME:

			return 1;
			break;

		case GTM_BUTTON_CHANNEL1:
			chatChannel.value = 1;
			lclient->SendMsg(sizeof(chatChannel),(void *)&chatChannel);
			return 1;
		case GTM_BUTTON_CHANNEL2:
			chatChannel.value = 2;
			lclient->SendMsg(sizeof(chatChannel),(void *)&chatChannel);
			return 1;
		case GTM_BUTTON_CHANNEL3:
			chatChannel.value = 4;
			lclient->SendMsg(sizeof(chatChannel),(void *)&chatChannel);
			return 1;

		}
	}


	//***********




	if (UIRECT_MOUSE_LUP == type)
	{

		switch(curUIRect->WhatAmI())	
		{
		case GTM_BUTTON_QUIT :
			// DebugOutput("1");
			button1Sound->PlayNo3D();
			newGameMode = new OptionMode(1,"OPTION_MODE");
			curgroundTestMode->retState = GMR_NEW_MODE;
			// DebugOutput("2");
//			newGameMode = NULL;
//			curgroundTestMode->retState = GMR_POP_ME;
			return 1;
			break;

		case GTM_BUTTON_REPORTBUG :
			if (!curgroundTestMode->fullWindow->childRectList.Find(curgroundTestMode->bugReportWindow))
			{
				button1Sound->PlayNo3D();
				curgroundTestMode->fullWindow->AddChild(curgroundTestMode->bugReportWindow);
				curgroundTestMode->fullWindow->Arrange();
			}
			return 1;
			break;

		case GTM_BUTTON_BUGSUBMIT :
			curgroundTestMode->fullWindow->childRectList.Remove(curgroundTestMode->bugReportWindow);
			curgroundTestMode->fullWindow->Arrange();

			// send off bug report message!
			tButt = (UIRectTextButton *) 
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGTYPE_ART);
			bugReport.art = tButt->isDepressed;
			tButt->isDepressed = FALSE;
			tButt = (UIRectTextButton *) 
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGTYPE_CRASH);
			bugReport.crash = tButt->isDepressed;
			tButt->isDepressed = FALSE;
			tButt = (UIRectTextButton *) 
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGTYPE_HANG);
			bugReport.hang = tButt->isDepressed;
			tButt->isDepressed = FALSE;
			tButt = (UIRectTextButton *) 
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGTYPE_GAMEPLAY);
			bugReport.gameplay = tButt->isDepressed;
			tButt->isDepressed = FALSE;
			tButt = (UIRectTextButton *) 
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGTYPE_OTHER);
			bugReport.other = tButt->isDepressed;
			tButt->isDepressed = FALSE;

			tEdit = (UIRectEditLine *)
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGDOING);
			memcpy(bugReport.doing,tEdit->text,111);
			bugReport.doing[110] = 0;
			tEdit->SetText("");
			tEdit = (UIRectEditLine *)
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGPLAYLENGTH);
			memcpy(bugReport.playLength,tEdit->text,111);
			bugReport.playLength[110] = 0;
			tEdit->SetText("");
			tEdit = (UIRectEditLine *)
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGREPEATABLE);
			memcpy(bugReport.repeatable,tEdit->text,111);
			bugReport.repeatable[110] = 0;
			tEdit->SetText("");
			tEdit = (UIRectEditLine *)
					  curgroundTestMode->bugReportWindow->childRectList.Find(GTM_BUTTON_BUGINFO);
			memcpy(bugReport.info,tEdit->text,111);
			bugReport.info[110] = 0;
			tEdit->SetText("");

			lclient->SendMsg(sizeof(bugReport),(void *)&bugReport);
			button1Sound->PlayNo3D();

			return 1;
			break;

		case GTM_BUTTON_BUGCANCEL :
			curgroundTestMode->fullWindow->childRectList.Remove(curgroundTestMode->bugReportWindow);
			curgroundTestMode->fullWindow->Arrange();
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_STATSUP:
			if (curgroundTestMode->fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN))
			{
				requestInfo.type = MESS_INVENTORY_SAME;
				requestInfo.offset = curgroundTestMode->playerInvIndex - 10;

				if (GTM_BUTTON_LIST_WRK == curgroundTestMode->playerListMode)
					requestInfo.which = MESS_WORKBENCH_PLAYER;
				else if (GTM_BUTTON_LIST_INV == curgroundTestMode->playerListMode)
				{
					requestInfo.which = MESS_INVENTORY_PLAYER;
					requestInfo.offset = curgroundTestMode->playerInvIndex - 8;
				}
				else if (GTM_BUTTON_LIST_SKL == curgroundTestMode->playerListMode)
					requestInfo.which = MESS_SKILLS_PLAYER;
				else //if (GTM_BUTTON_LIST_WLD == playerListMode)
					requestInfo.which = MESS_WIELD_PLAYER;

				lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
				button1Sound->PlayNo3D();
				return 1;
			}
			break;

		case GTM_BUTTON_STATSDOWN:
			if (curgroundTestMode->fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN))
			{
				requestInfo.type = MESS_INVENTORY_SAME;
				requestInfo.offset = curgroundTestMode->playerInvIndex + 10;

				if (GTM_BUTTON_LIST_WRK == curgroundTestMode->playerListMode)
					requestInfo.which = MESS_WORKBENCH_PLAYER;
				else if (GTM_BUTTON_LIST_INV == curgroundTestMode->playerListMode)
				{
					requestInfo.which = MESS_INVENTORY_PLAYER;
					requestInfo.offset = curgroundTestMode->playerInvIndex + 8;
				}
				else if (GTM_BUTTON_LIST_SKL == curgroundTestMode->playerListMode)
					requestInfo.which = MESS_SKILLS_PLAYER;
				else //if (GTM_BUTTON_LIST_WLD == playerListMode)
					requestInfo.which = MESS_WIELD_PLAYER;

				lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
				button1Sound->PlayNo3D();
				return 1;
			}
			break;

		case GTM_BUTTON_LIST_INV:
		case GTM_BUTTON_LIST_WRK:
		case GTM_BUTTON_LIST_SKL:
		case GTM_BUTTON_LIST_WLD:
			curgroundTestMode->ProcessListPress(curUIRect->WhatAmI());
			break;

		case GTM_BUTTON_DUNEDIT_WALL:
			tempX = curgroundTestMode->GetCardinalDirection(bboClient->curCamAngle);

			messDungeonChange.x = bboClient->playerAvatar->cellX;
			messDungeonChange.y = bboClient->playerAvatar->cellY;
			messDungeonChange.floor = messDungeonChange.outer = 0;
			messDungeonChange.left = messDungeonChange.top = 
				messDungeonChange.reset = 0;


			switch((int)tempX)
			{
			case 0:
			case 1:
				messDungeonChange.top = 1;
				break;
			case 6:
			case 7:
				messDungeonChange.left = 1;
				break;
			case 2:
			case 3:
				messDungeonChange.x++;
				messDungeonChange.left = 1;
				break;
			case 4:
			case 5:
				messDungeonChange.y++;
				messDungeonChange.top = 1;
				break;
			}

			lclient->SendMsg(sizeof(messDungeonChange),(void *)&messDungeonChange);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_DUNEDIT_OUTER:
			tempX = curgroundTestMode->GetCardinalDirection(bboClient->curCamAngle);

			messDungeonChange.x = bboClient->playerAvatar->cellX;
			messDungeonChange.y = bboClient->playerAvatar->cellY;
			messDungeonChange.floor = messDungeonChange.left = 
				messDungeonChange.top = messDungeonChange.reset = 0;
			messDungeonChange.outer = 1;

			lclient->SendMsg(sizeof(messDungeonChange),(void *)&messDungeonChange);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_DUNEDIT_FLOOR:
			tempX = curgroundTestMode->GetCardinalDirection(bboClient->curCamAngle);

			messDungeonChange.x = bboClient->playerAvatar->cellX;
			messDungeonChange.y = bboClient->playerAvatar->cellY;
			messDungeonChange.outer = messDungeonChange.left = 
				messDungeonChange.top = messDungeonChange.reset = 0;
			messDungeonChange.floor = 1;

			lclient->SendMsg(sizeof(messDungeonChange),(void *)&messDungeonChange);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_DUNEDIT_CAMERA:
			if (bboClient->isEditingDungeon)
			{
				++(bboClient->cameraType);
				if (bboClient->cameraType > 2)
					bboClient->cameraType = 0;
			}
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_OTHERUP:
			requestInfo.type = MESS_INVENTORY_SAME;
			requestInfo.offset = curgroundTestMode->otherInvIndex - 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_OTHERDOWN:
			requestInfo.type = MESS_INVENTORY_SAME;
			requestInfo.offset = curgroundTestMode->otherInvIndex + 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_HER_SECURE_UP:
			requestInfo.type = MESS_INVENTORY_HER_SECURE;
			requestInfo.offset = curgroundTestMode->secureHerInvIndex - 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_HER_SECURE_DOWN:
			requestInfo.type = MESS_INVENTORY_HER_SECURE;
			requestInfo.offset = curgroundTestMode->secureHerInvIndex + 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_YOU_SECURE_UP:
			requestInfo.type = MESS_INVENTORY_YOUR_SECURE;
			requestInfo.offset = curgroundTestMode->secureYouInvIndex - 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_YOU_SECURE_DOWN:
			requestInfo.type = MESS_INVENTORY_YOUR_SECURE;
			requestInfo.offset = curgroundTestMode->secureYouInvIndex + 8;
			requestInfo.which = MESS_INVENTORY_TRADER;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_MOVEUP:

			if (moveLockoutTimer > 0)
				break;

			tempX = curgroundTestMode->GetCardinalDirection(bboClient->curCamAngle);
			bMoveReq.avatarID = bboClient->playerAvatarID;
			bMoveReq.x = tempX;
			lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
			curgroundTestMode->RemoveNonInvWindows();
			curgroundTestMode->RemoveActionButtons();

			button1Sound->PlayNo3D();
			return 1;
			break;

		case GTM_BUTTON_SECURE_DECLINE :
			secureTrade.type = MESS_SECURE_STOP;
			lclient->SendMsg(sizeof(secureTrade),(void *)&secureTrade);
			return 1;
			break;

		case GTM_BUTTON_SECURE_ACCEPT :
			secureTrade.type = MESS_SECURE_ACCEPT;
			lclient->SendMsg(sizeof(secureTrade),(void *)&secureTrade);
			((UIRectTextButton *)curUIRect)->SetText("Waiting...");
			return 1;
			break;

		case GTM_BUTTON_YOU_SECURE_NAME:
		case GTM_BUTTON_BANKNAME:
			curgroundTestMode->giveMoneyMode = TRUE;
			return 1;
			break;

		case GTM_BUTTON_DO1:
		case GTM_BUTTON_DO2:
		case GTM_BUTTON_DO3:
		case GTM_BUTTON_DO4:
		case GTM_BUTTON_DO5:
		case GTM_BUTTON_DO6:
		case GTM_BUTTON_DO7:
		case GTM_BUTTON_DO8:
		case GTM_BUTTON_DO9:
		case GTM_BUTTON_DO10:
		case GTM_BUTTON_DO11:
		case GTM_BUTTON_DO12:
		case GTM_BUTTON_DO13:
		case GTM_BUTTON_DO14:
		case GTM_BUTTON_DO15:
		case GTM_BUTTON_DO16:
		case GTM_BUTTON_DO17:
		case GTM_BUTTON_DO18:

			button1Sound->PlayNo3D();
			i = curUIRect->WhatAmI() - GTM_BUTTON_DO1;
			// work out what we should do in response to this button press.
			switch(curgroundTestMode->actionButtonTypes[i])
			{
			case ACTION_TYPE_DROP     :
				break;

			case ACTION_TYPE_TRADE    :
				if (bboClient->selectedMOB)
				{
					curgroundTestMode->RemoveNonInvWindows();

					if (bboClient->selectedMOB != bboClient->playerAvatar &&
						 bboClient->selectedMOB->type != SMOB_AVATAR &&
						 bboClient->selectedMOB->type != SMOB_MONSTER &&
						 bboClient->selectedMOB->type != SMOB_ITEM_SACK)
					{
						requestInfo.type = MESS_INVENTORY_TRADER;
						requestInfo.offset = 0;
						requestInfo.which = bboClient->selectedMOB->mobID;
						lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
						curgroundTestMode->RemoveActionButtons();
					}
					else if (bboClient->selectedMOB != bboClient->playerAvatar &&
						 SMOB_AVATAR == bboClient->selectedMOB->type)
					{
						curgroundTestMode->RemoveActionButtons();
						curgroundTestMode->giveMoneyMode = TRUE;
					}
					else if (bboClient->selectedMOB->type == SMOB_ITEM_SACK)
					{
						requestInfo.type = MESS_INVENTORY_GROUND;
						requestInfo.offset = 0;
						lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
						curgroundTestMode->RemoveActionButtons();
					}
				}
				else
				{
					requestInfo.type = MESS_INVENTORY_GROUND;
					requestInfo.offset = 0;
					lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
					curgroundTestMode->RemoveActionButtons();
				}
				break;

			case ACTION_TYPE_SECURE_TRADE    :
				if (bboClient->selectedMOB)
				{
					if (bboClient->selectedMOB != bboClient->playerAvatar &&
						 SMOB_AVATAR == bboClient->selectedMOB->type)
					{
						curgroundTestMode->RemoveActionButtons();

						// Start secure trading!
						requestInfo.type = MESS_INVENTORY_YOUR_SECURE;
						requestInfo.offset = 0;
						requestInfo.which = bboClient->selectedMOB->avatarID;

						lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
						button1Sound->PlayNo3D();
					}
				}
				break;

			case ACTION_TYPE_GIVE     :
				transferRequest.amount = 1;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.isPlayerInfo = TRUE;
				transferRequest.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = NULL;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_GIVE5     :
				transferRequest.amount = 5;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.isPlayerInfo = TRUE;
				transferRequest.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = NULL;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_GIVE20     :
				transferRequest.amount = 20;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.isPlayerInfo = TRUE;
				transferRequest.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = NULL;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_GIVEALL     :
				transferRequest.amount = 255;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.isPlayerInfo = TRUE;
				transferRequest.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = NULL;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_BUY      :
				transferRequest.amount = 1;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->otherInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_BUY5      :
				transferRequest.amount = 5;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->otherInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_BUY20      :
				transferRequest.amount = 20;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->otherInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_BUYALL      :
				transferRequest.amount = 255;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->otherInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_RETRACT      :
				transferRequest.amount = 1;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->yourInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_RETRACT5      :
				transferRequest.amount = 5;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->yourInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_RETRACT20      :
				transferRequest.amount = 20;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->yourInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_RETRACTALL      :
				transferRequest.amount = 255;
				transferRequest.isPlayerInfo = FALSE;
				transferRequest.playerListType = curgroundTestMode->playerListMode;
				transferRequest.ptr = curgroundTestMode->yourInv[curgroundTestMode->selectedIndex].ptr;
				transferRequest.partner = curgroundTestMode->partnerPtr;
				lclient->SendMsg(sizeof(transferRequest),(void *)&transferRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_SELL     :
				break;

			case ACTION_TYPE_USE      :
				break;

			case ACTION_TYPE_ACTIVATE :
				setBomb.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				lclient->SendMsg(sizeof(setBomb),(void *)&setBomb);
				curgroundTestMode->RemoveActionButtons();
				break;

				break;

			case ACTION_TYPE_DEACTIVATE:
				break;

			case ACTION_TYPE_WIELD    :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 1;
				messChange.srcListType = GTM_BUTTON_LIST_INV;
				messChange.dstListType = GTM_BUTTON_LIST_WLD;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				/*
				messWield.bladeID = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				lclient->SendMsg(sizeof(messWield),(void *)&messWield);
				curgroundTestMode->RemoveActionButtons();
				*/
				break;

			case ACTION_TYPE_UNWIELD  :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 1;
				messChange.srcListType = GTM_BUTTON_LIST_WLD;
				messChange.dstListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				/*
				messUnWield.bladeID = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				lclient->SendMsg(sizeof(messUnWield),(void *)&messUnWield);
				curgroundTestMode->RemoveActionButtons();
				*/
				break;

			case ACTION_TYPE_ATTACK:
				if (bboClient->selectedMOB)
				{
//					if (bboClient->selectedMOB != bboClient->playerAvatar &&
//						 bboClient->selectedMOB->type == SMOB_MONSTER)
					if (bboClient->selectedMOB->type == SMOB_MONSTER)
					{
						messAA.mobID = bboClient->selectedMOB->mobID;
						lclient->SendMsg(sizeof(messAA),(void *)&messAA);
					}
					else if (bboClient->controlledMonsterID != -1 &&
						 bboClient->selectedMOB->type == SMOB_AVATAR)
					{
						messAA.mobID = bboClient->selectedMOB->avatarID;
						lclient->SendMsg(sizeof(messAA),(void *)&messAA);
					}
				}
				break;

			case ACTION_TYPE_CONTROL:
				if (bboClient->selectedMOB)
				{
					if (bboClient->selectedMOB != bboClient->playerAvatar &&
						 bboClient->selectedMOB->type == SMOB_MONSTER)
					{
						adminMess.mobID = bboClient->selectedMOB->mobID;
						adminMess.messageType = MESS_ADMIN_TAKE_CONTROL;
						lclient->SendMsg(sizeof(adminMess),(void *)&adminMess);
					}
				}
				break;

			case ACTION_TYPE_RELEASE:
				if (bboClient->selectedMOB)
				{
					adminMess.mobID = bboClient->selectedMOB->mobID;
					adminMess.messageType = MESS_ADMIN_RELEASE_CONTROL;
					lclient->SendMsg(sizeof(adminMess),(void *)&adminMess);
				}
				break;

			case ACTION_TYPE_OPEN:
				if (bboClient->selectedMOB)
				{
					if (bboClient->selectedMOB->type == SMOB_CHEST)
					{
						// if in tower, NOT dungeon...
						if (bboClient->dungeon && (SPACE_GUILD == curMapType))
						{
							requestInfo.type = MESS_INVENTORY_TOWER;
							requestInfo.offset = 0;
							lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
							curgroundTestMode->RemoveActionButtons();
						}
						else
						{
							chestInfo.mobID = bboClient->selectedMOB->mobID;
							lclient->SendMsg(sizeof(chestInfo),(void *)&chestInfo);
						}
					}
				}
				break;

			case ACTION_TYPE_LISTEN:
				if (bboClient->selectedMOB)
				{
					if (bboClient->selectedMOB->type == SMOB_TREE)
					{
						messTree.which = 0; // listen
						lclient->SendMsg(sizeof(messTree),(void *)&messTree);
					}
					else if (bboClient->selectedMOB->type == SMOB_TOWNMAGE)
					{
						messTree.which = 5; // listen
						lclient->SendMsg(sizeof(messTree),(void *)&messTree);
					}
					else if (bboClient->selectedMOB->type == SMOB_WITCH)
					{
						messTree.which = 2; // listen
						lclient->SendMsg(sizeof(messTree),(void *)&messTree);
						bboClient->witchTalkCounter = 120;
					}
				}
				break;

			case ACTION_TYPE_QUEST:
				if (bboClient->selectedMOB)
				{
					if (bboClient->selectedMOB->type == SMOB_TREE)
					{
						messTree.which = 1; // quest
						lclient->SendMsg(sizeof(messTree),(void *)&messTree);
					}
					else if (bboClient->selectedMOB->type == SMOB_WITCH)
					{
						messTree.which = 3; // quest
						lclient->SendMsg(sizeof(messTree),(void *)&messTree);
						bboClient->witchTalkCounter = 120;
					}
				}
				break;

			case ACTION_TYPE_INCLUDE     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 1;
				messChange.dstListType = GTM_BUTTON_LIST_WRK;
				messChange.srcListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_INCLUDE5     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 5;
				messChange.dstListType = GTM_BUTTON_LIST_WRK;
				messChange.srcListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_INCLUDE20     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 20;
				messChange.dstListType = GTM_BUTTON_LIST_WRK;
				messChange.srcListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_INCLUDEALL     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 255;
				messChange.dstListType = GTM_BUTTON_LIST_WRK;
				messChange.srcListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_EXCLUDE     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 1;
				messChange.srcListType = GTM_BUTTON_LIST_WRK;
				messChange.dstListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_EXCLUDE5     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 5;
				messChange.srcListType = GTM_BUTTON_LIST_WRK;
				messChange.dstListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_EXCLUDE20     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 20;
				messChange.srcListType = GTM_BUTTON_LIST_WRK;
				messChange.dstListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_EXCLUDEALL     :
				messChange.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				messChange.amount = 255;
				messChange.srcListType = GTM_BUTTON_LIST_WRK;
				messChange.dstListType = GTM_BUTTON_LIST_INV;
				lclient->SendMsg(sizeof(messChange),(void *)&messChange);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_COMBINE     :
				messTryCombine.skillID = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				lclient->SendMsg(sizeof(messTryCombine),(void *)&messTryCombine);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_TELEPORT     :
				messTMS.which = TMSERVICE_TELEPORT;
				lclient->SendMsg(sizeof(messTMS),(void *)&messTMS);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_TELEPORT_BACK     :
				messTMS.which = TMSERVICE_TELEPORT_BACK;
				lclient->SendMsg(sizeof(messTMS),(void *)&messTMS);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_HEAL     :
				messTMS.which = TMSERVICE_HEAL;
				lclient->SendMsg(sizeof(messTMS),(void *)&messTMS);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_BANK     :
				messTMS.which = TMSERVICE_BANK;
				lclient->SendMsg(sizeof(messTMS),(void *)&messTMS);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_FEED0     :
			case ACTION_TYPE_FEED1     :
				feedPetRequest.ptr  = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				feedPetRequest.which = curgroundTestMode->actionButtonTypes[i] - ACTION_TYPE_FEED0;
				lclient->SendMsg(sizeof(feedPetRequest),(void *)&feedPetRequest);
				curgroundTestMode->RemoveActionButtons();
				break;

			case ACTION_TYPE_SELLALLGREEN     :
				if (bboClient->selectedMOB)
				{
					if (SMOB_TRADER == bboClient->selectedMOB->type)
					{
						sellAllMess.which = bboClient->selectedMOB->mobID;
						sellAllMess.type = INVOBJ_SIMPLE;
						lclient->SendMsg(sizeof(sellAllMess),(void *)&sellAllMess);
					}
				}
				curgroundTestMode->RemoveActionButtons();
				curgroundTestMode->ClearEverythingFromScreen();
				break;

			case ACTION_TYPE_SELLALLMEAT     :
				if (bboClient->selectedMOB)
				{
					if (SMOB_TRADER == bboClient->selectedMOB->type)
					{
						sellAllMess.which = bboClient->selectedMOB->mobID;
						sellAllMess.type = INVOBJ_MEAT;
						lclient->SendMsg(sizeof(sellAllMess),(void *)&sellAllMess);
					}
				}
				curgroundTestMode->RemoveActionButtons();
				curgroundTestMode->ClearEverythingFromScreen();
				break;

			case ACTION_TYPE_SELLALLBOMBS     :
				if (bboClient->selectedMOB)
				{
					if (SMOB_TRADER == bboClient->selectedMOB->type)
					{
						sellAllMess.which = bboClient->selectedMOB->mobID;
						sellAllMess.type = INVOBJ_BOMB;
						lclient->SendMsg(sizeof(sellAllMess),(void *)&sellAllMess);
					}
				}
				curgroundTestMode->RemoveActionButtons();
				curgroundTestMode->ClearEverythingFromScreen();
				break;

			case ACTION_TYPE_SELLALLKEYS     :
				if (bboClient->selectedMOB)
				{
					if (SMOB_TRADER == bboClient->selectedMOB->type)
					{
						sellAllMess.which = bboClient->selectedMOB->mobID;
						sellAllMess.type = INVOBJ_EARTHKEY;
						lclient->SendMsg(sizeof(sellAllMess),(void *)&sellAllMess);
					}
				}
				curgroundTestMode->RemoveActionButtons();
				curgroundTestMode->ClearEverythingFromScreen();
				break;

			case ACTION_TYPE_SET_BOMB     :
				setBomb.ptr = curgroundTestMode->playerInv[curgroundTestMode->selectedIndex].ptr;
				lclient->SendMsg(sizeof(setBomb),(void *)&setBomb);
				curgroundTestMode->RemoveActionButtons();
				break;

			}

			return 1;
			break;
		}
	}

	//***********

	if (UIRECT_MOUSE_TEST_POS == type)
	{
		curgroundTestMode->hoverTime = timeGetTime();
		curgroundTestMode->itemInfoListMode = curgroundTestMode->itemInfoPtr = -1;

		tBox = (UIRectTextBox *) 
			    curgroundTestMode->fullWindow->childRectList.Find(GTM_BUTTON_ITEM_INFO_TEXT);
		if (!tBox)
			return 0;

		switch(curUIRect->WhatAmI())	
		{
		case GTM_BUTTON_STATSLINE0:
		case GTM_BUTTON_STATSLINE1:
		case GTM_BUTTON_STATSLINE2:
		case GTM_BUTTON_STATSLINE3:
		case GTM_BUTTON_STATSLINE4:
		case GTM_BUTTON_STATSLINE5:
		case GTM_BUTTON_STATSLINE6:
		case GTM_BUTTON_STATSLINE7:
		case GTM_BUTTON_STATSLINE8:
		case GTM_BUTTON_STATSLINE9:
		case GTM_BUTTON_STATSLINE10:

			if (GTM_BUTTON_LIST_WRK == curgroundTestMode->playerListMode)
				curgroundTestMode->itemInfoListMode = MESS_WORKBENCH_PLAYER;
			else if (GTM_BUTTON_LIST_INV == curgroundTestMode->playerListMode)
				curgroundTestMode->itemInfoListMode = MESS_INVENTORY_PLAYER;
			else if (GTM_BUTTON_LIST_SKL == curgroundTestMode->playerListMode)
				curgroundTestMode->itemInfoListMode = MESS_SKILLS_PLAYER;
			else //if (GTM_BUTTON_LIST_WLD == playerListMode)
				curgroundTestMode->itemInfoListMode = MESS_WIELD_PLAYER;

			curgroundTestMode->itemInfoPtr = 
				curgroundTestMode->playerInv[curUIRect->WhatAmI() - GTM_BUTTON_STATSLINE0].ptr;

			tBox->SetText(((UIRectTextButton *)curUIRect)->text);
			break;

		case GTM_BUTTON_OTHERLINE0:
		case GTM_BUTTON_OTHERLINE1:
		case GTM_BUTTON_OTHERLINE2:
		case GTM_BUTTON_OTHERLINE3:
		case GTM_BUTTON_OTHERLINE4:
		case GTM_BUTTON_OTHERLINE5:
		case GTM_BUTTON_OTHERLINE6:
		case GTM_BUTTON_OTHERLINE7:
		case GTM_BUTTON_OTHERLINE8:
		case GTM_BUTTON_OTHERLINE9:
			curgroundTestMode->itemInfoListMode = MESS_INVENTORY_TRADER;
			curgroundTestMode->itemInfoPtr = 
				curgroundTestMode->otherInv[curUIRect->WhatAmI() - GTM_BUTTON_OTHERLINE0].ptr;

			tBox->SetText(((UIRectTextButton *)curUIRect)->text);
			break;

		case GTM_BUTTON_YOU_SECURE_LINE0:
		case GTM_BUTTON_YOU_SECURE_LINE1:
		case GTM_BUTTON_YOU_SECURE_LINE2:
		case GTM_BUTTON_YOU_SECURE_LINE3:
		case GTM_BUTTON_YOU_SECURE_LINE4:
		case GTM_BUTTON_YOU_SECURE_LINE5:
		case GTM_BUTTON_YOU_SECURE_LINE6:
		case GTM_BUTTON_YOU_SECURE_LINE7:
		case GTM_BUTTON_YOU_SECURE_LINE8:
		case GTM_BUTTON_YOU_SECURE_LINE9:
			curgroundTestMode->itemInfoListMode = MESS_INVENTORY_YOUR_SECURE;
			curgroundTestMode->itemInfoPtr = 
				curgroundTestMode->yourInv[curUIRect->WhatAmI() - GTM_BUTTON_YOU_SECURE_LINE0].ptr;

			tBox->SetText(((UIRectTextButton *)curUIRect)->text);
			break;

		case GTM_BUTTON_HER_SECURE_LINE0:
		case GTM_BUTTON_HER_SECURE_LINE1:
		case GTM_BUTTON_HER_SECURE_LINE2:
		case GTM_BUTTON_HER_SECURE_LINE3:
		case GTM_BUTTON_HER_SECURE_LINE4:
		case GTM_BUTTON_HER_SECURE_LINE5:
		case GTM_BUTTON_HER_SECURE_LINE6:
		case GTM_BUTTON_HER_SECURE_LINE7:
		case GTM_BUTTON_HER_SECURE_LINE8:
		case GTM_BUTTON_HER_SECURE_LINE9:
			curgroundTestMode->itemInfoListMode = MESS_INVENTORY_HER_SECURE;
			curgroundTestMode->itemInfoPtr = 
				curgroundTestMode->herInv[curUIRect->WhatAmI() - GTM_BUTTON_HER_SECURE_LINE0].ptr;

			tBox->SetText(((UIRectTextButton *)curUIRect)->text);
			break;

		default:
			tBox->SetText(" ");
			break;
		}

		return 1;
	}



	return 0;  // didn't deal with this message
}

//******************************************************************
void DisplayTextInfo(char *text)
{
	if (curgroundTestMode)
		curgroundTestMode->AddInfoText(text);

}

const int GTM_UI_ALPHA = 100;


//******************************************************************
//******************************************************************
GroundTestMode::GroundTestMode(int playerID, int doid, char *doname) : GameMode(doid,doname)
{
	curgroundTestMode = this;
	POMSezQuit = FALSE;
	bboClient = new BBOClient();
	bboClient->playerAvatarID = playerID;
//	screenHealth = screenMaxHealth = 1;

	OptionMode::LoadLocalSettings();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIstone.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffff00ff, NULL, NULL, &uiArt);

	chatWin = new ChatWindow(GTM_BUTTON_CHATBOX, 0,puma->ScreenH() - 100,440,puma->ScreenH());
	chatWin->process = groundTestModeProcess;
	chatWin->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;

	uiAlpha = 255;
	UIRectDragEdge *dragEdge = new UIRectDragEdge(GTM_BUTTON_CHATBOX    , -1,-1,-1,8);
	dragEdge->process = groundTestModeProcess;
   dragEdge->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
//	dragEdge->fillArt = uiArt;
	chatWin->AddChild(dragEdge);

   UIRectWindow *spacer = new UIRectWindow(GTM_BUTTON_TEXT, -1,33,-1,-1);
   spacer->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	chatWin->AddChild(spacer);

	uiAlpha = 0;
	UIRectTextBox *textBox;

	for (int i = 1; i < 50; ++i)
	{
		textBox = new UIRectTextBox(GTM_BUTTON_CHATLINE_0+i, -1,12,-1,-1);
		textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		textBox->SetText("");
		textBox->process = groundTestModeProcess;
//		textBox->textFlags = DT_LEFT;
		chatWin->AddChild(textBox);
	}

	edLine	  = new UIRectEditLine(GTM_BUTTON_CHATLINE_0, -1,40,-1,-1,110);
	edLine->process = groundTestModeProcess;
	edLine->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	edLine->SetText("");
	chatWin->AddChild(edLine);
	chatWin->edLine = edLine;

	logDoneCounter = 0;

	for (int i = 0; i < 6; ++i)
		textColorList[i] = D3DCOLOR_ARGB(255, 255, 255, 255);

	// load text colors
	FILE *fp = fopen("textColors.txt","r");
	if (fp)
	{
		int r,g,b;
		char text[64];

		for (int i = 0; i < 6; ++i)
		{
			fscanf(fp, "%s %d %d %d\n",text,&r,&g,&b);
			if (!stricmp(text,"ANNOUNCE"))
				textColorList[0] = D3DCOLOR_ARGB(255, r, g, b);
			else if (!stricmp(text,"TELL"))
				textColorList[1] = D3DCOLOR_ARGB(255, r, g, b);
			else if (!stricmp(text,"SHOUT"))
				textColorList[2] = D3DCOLOR_ARGB(255, r, g, b);
			else if (!stricmp(text,"DATA"))
				textColorList[3] = D3DCOLOR_ARGB(255, r, g, b);
			else if (!stricmp(text,"GUILD"))
				textColorList[4] = D3DCOLOR_ARGB(255, r, g, b);
			else if (!stricmp(text,"EMOTE"))
				textColorList[5] = D3DCOLOR_ARGB(255, r, g, b);
		}
		fclose(fp);
	}

	securePartnerName[0] = 0;
	chatChannelValues = 0;
	mapState = 0;
}

//******************************************************************
GroundTestMode::~GroundTestMode()
{
   SAFE_RELEASE(uiArt);

	delete chatWin;
	delete bboClient;

	MessExitGame exitGame;
	lclient->SendMsg(sizeof(exitGame),(void *)&exitGame);

	curgroundTestMode = NULL;
}

//******************************************************************
int GroundTestMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int GroundTestMode::Activate(void) // do this when the mode becomes the forbboClient->ground mode.
{
	aLog.Log("GroundTestMode::Activate ********************\n");

	uiAlpha = GTM_UI_ALPHA;

	playerInvIndex = otherInvIndex = 0;
	secureYouInvIndex = secureHerInvIndex =0;
	lastSelectedMOB = NULL;
	moveLockoutTimer = 0;
	camSpinDelta = camRaiseDelta = rmbDownPos = rmbDownPosY = 0;
	moveRequestTimer = timeGetTime();
	giveMoneyMode = FALSE;
	namePetMode = FALSE;

	SetEnvironment();

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bar-red.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &barArt[0]);
	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bar-green.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &barArt[1]);

	puma->LoadTexture("dat\\CharInvBack.png"  ,&uiPlayerListArt[0],0);
	puma->LoadTexture("dat\\CharWieldBack.png",&uiPlayerListArt[1],0);
	puma->LoadTexture("dat\\CharWorkBack.png" ,&uiPlayerListArt[2],0);
	puma->LoadTexture("dat\\CharSkillBack.png",&uiPlayerListArt[3],0);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIOtherList.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &uiOtherListArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIPopUp.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &uiPopUpArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIPopUpSpace.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &uiPopUpSpaceArt);

	hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\UIPopUpLong.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0, NULL, NULL, &uiPopUpLongArt);

	puma->LoadTexture("dat\\UIPopUpTop.png"   , &uiPopUpTop   , 0);
	puma->LoadTexture("dat\\UIPopUpBottom.png", &uiPopUpBottom, 0);
	puma->LoadTexture("dat\\mainCompass.png"  , &compassArt   , 0);

	puma->LoadTexture("dat\\CharFrame.png"    , &uiPlayerFrameArt, 0);

	puma->LoadTexture("dat\\UITradeList.png"  , &uiTradeListArt, 0);

	puma->LoadTexture("dat\\radar1.png"  , &radarArt[0], 0);
	puma->LoadTexture("dat\\radar2.png"  , &radarArt[1], 0);
	puma->LoadTexture("dat\\radar3.png"  , &radarArt[2], 0);
	puma->LoadTexture("dat\\radar4.png"  , &radarArt[3], 0);
	puma->LoadTexture("dat\\radar1A.png"  , &radarArt[4], 0);
	puma->LoadTexture("dat\\radar2A.png"  , &radarArt[5], 0);
	puma->LoadTexture("dat\\radar3A.png"  , &radarArt[6], 0);
	puma->LoadTexture("dat\\radar4A.png"  , &radarArt[7], 0);
	puma->LoadTexture("dat\\dot.png"  , &radarArt[8], 0xff000000);

   // ********** start building UIRects
	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

	actionWindow = NULL;

   fullWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   charInvWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
   charInvWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   charInvWindow->isActive  = TRUE;

   UIRectTextButton *tButt;
   UIRectArtTextButton *tArtButt;
	UIRectTextBox *textBox;

   textBox = new UIRectTextBox(GTM_BUTTON_COMPASS,0,0,132,25);
   textBox->SetText("Location");
   textBox->process = groundTestModeProcess;
	textBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(textBox);
	compassBox = textBox;

   textBox = new UIRectTextBox(GTM_BUTTON_COMPASS2,47,31,110,43);
   textBox->SetText("Level");
   textBox->process = groundTestModeProcess;
	textBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(textBox);
	compassBox2 = textBox;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_QUIT,
		            puma->ScreenW()-128, puma->ScreenH() - 64,
		            puma->ScreenW(),     puma->ScreenH());
   tArtButt->SetText("Options");
   tArtButt->process = groundTestModeProcess;
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE,    "dat\\cornerSmlHigh.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED, "dat\\cornerSmlDown.png", 0);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL,    "dat\\cornerSmlUp.png", 0);
	tArtButt->textOffsetX = 15;
	tArtButt->textOffsetY = 35;
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->AddChild(tArtButt);

	fullWindow->AddChild(chatWin);

	// ******** bug reporting window
	uiAlpha = 255;
	bugReportWindow = new UIRectWindow(GTM_BUTTON_BUGWIN, 0,190,640,350);
	bugReportWindow->process = groundTestModeProcess;
//	chatWin->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
//	fullWindow->AddChild(bugReportWindow);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGTYPE_CRASH, 2,2  ,88,25);
   tButt->SetText("Crash Bug");
   tButt->process = groundTestModeProcess;
	tButt->isMomentary = FALSE;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);
   UIRectStep(1,0);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGTYPE_HANG, -2,-2  ,-2,-2);
   tButt->SetText("Program Hang");
   tButt->process = groundTestModeProcess;
	tButt->isMomentary = FALSE;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);
   UIRectStep(1,0);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGTYPE_ART, -2,-2  ,-2,-2);
   tButt->SetText("Art/Sound");
   tButt->process = groundTestModeProcess;
	tButt->isMomentary = FALSE;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);
   UIRectStep(1,0);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGTYPE_GAMEPLAY, -2,-2  ,-2,-2);
   tButt->SetText("Gameplay");
   tButt->process = groundTestModeProcess;
	tButt->isMomentary = FALSE;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);
   UIRectStep(1,0);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGTYPE_OTHER, -2,-2  ,-2,-2);
   tButt->SetText("Other");
   tButt->process = groundTestModeProcess;
	tButt->isMomentary = FALSE;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);

	UIRectEditLine *tempEdLine;
	int i = 27;
	tempEdLine	  = new UIRectEditLine(GTM_BUTTON_BUGDOING, 150,i,-1,i+20,110);
	tempEdLine->process = groundTestModeProcess;
	tempEdLine->SetText("");
	bugReportWindow->AddChild(tempEdLine);
	textBox = new UIRectTextBox(GTM_BUTTON_TEXT, -1,i,148,i+20);
	textBox->SetText("What were you doing?");
	textBox->process = groundTestModeProcess;
	bugReportWindow->AddChild(textBox);

	i += 22;
	tempEdLine	  = new UIRectEditLine(GTM_BUTTON_BUGREPEATABLE, 150,i,-1,i+20,110);
	tempEdLine->process = groundTestModeProcess;
	tempEdLine->SetText("");
	bugReportWindow->AddChild(tempEdLine);
	textBox = new UIRectTextBox(GTM_BUTTON_TEXT, -1,i,148,i+20);
	textBox->SetText("Is it repeat-able?");
	textBox->process = groundTestModeProcess;
	bugReportWindow->AddChild(textBox);

	i += 22;
	tempEdLine	  = new UIRectEditLine(GTM_BUTTON_BUGPLAYLENGTH, 150,i,-1,i+20,110);
	tempEdLine->process = groundTestModeProcess;
	tempEdLine->SetText("");
	bugReportWindow->AddChild(tempEdLine);
	textBox = new UIRectTextBox(GTM_BUTTON_TEXT, -1,i,148,i+20);
	textBox->SetText("How long playing?");
	textBox->process = groundTestModeProcess;
	bugReportWindow->AddChild(textBox);

	i += 22;
	tempEdLine	  = new UIRectEditLine(GTM_BUTTON_BUGINFO, 150,i,-1,i+20,110);
	tempEdLine->process = groundTestModeProcess;
	tempEdLine->SetText("");
	bugReportWindow->AddChild(tempEdLine);
	textBox = new UIRectTextBox(GTM_BUTTON_TEXT, -1,i,148,i+20);
	textBox->SetText("What was the bug?");
	textBox->process = groundTestModeProcess;
	bugReportWindow->AddChild(textBox);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGSUBMIT, 500,120  ,-1,-1);
   tButt->SetText("Submit");
   tButt->process = groundTestModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);

   tButt = new UIRectTextButton(GTM_BUTTON_BUGCANCEL, -1,120  ,150,-1);
   tButt->SetText("Cancel");
   tButt->process = groundTestModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   bugReportWindow->AddChild(tButt);

	uiAlpha = GTM_UI_ALPHA;

//	UIRectEdgeBar *edgeBar;

	// ******** text window
	/*
	UIRectWindow *textWindow = new UIRectWindow(
		             GTM_BUTTON_INFOWIN, centerX-300,puma->ScreenH() - 80,
						 centerX+300,puma->ScreenH(), "WINDOW", fullWindow);
	textWindow->process = groundTestModeProcess;
	fullWindow->AddChild(textWindow);

	uiAlpha = 255;
	edgeBar = new UIRectEdgeBar(GTM_BUTTON_INFODRAG, -1,-1,-1,8,textWindow);
	edgeBar->process = groundTestModeProcess;
	textWindow->AddChild(edgeBar);
	uiAlpha = GTM_UI_ALPHA;
  */
	// ******** stats window
//   UIRectArtTextButton *tArtButt;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_LIST_INV,
		             puma->ScreenW() - 206, 4, puma->ScreenW() - 206 + 38, 4+38);
//   tArtButt->SetText("Inven");
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharInvDown.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\CharInvHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   charInvWindow->AddChild(tArtButt);
//   UIRectStep(1,0);
   tArtButt = new UIRectArtTextButton(GTM_BUTTON_LIST_WLD,
		             puma->ScreenW() - 154, 3, puma->ScreenW() - 154 + 38, 3+38);
//   tArtButt->SetText("Used");
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharWieldDown.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\CharWieldHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   charInvWindow->AddChild(tArtButt);
//   UIRectStep(1,0);
   tArtButt = new UIRectArtTextButton(GTM_BUTTON_LIST_WRK,
		             puma->ScreenW() - 101, 2, puma->ScreenW() - 101 + 38, 2+38);
//   tArtButt->SetText("Wkbnch");
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharWorkDown.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\CharWorkHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   charInvWindow->AddChild(tArtButt);
//   UIRectStep(1,0);
   tArtButt = new UIRectArtTextButton(GTM_BUTTON_LIST_SKL,
		             puma->ScreenW() - 52, 2, puma->ScreenW() - 52 + 38, 2+38);
//   tArtButt->SetText("Skills");
	tArtButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\CharSkillDown.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\CharSkillHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   tArtButt->process = groundTestModeProcess;
   charInvWindow->AddChild(tArtButt);
	/*
   UIRectStep(1,0);
   tButt = new UIRectTextButton(GTM_BUTTON_LIST_WLD, -2,-2,-2,-2);
   tButt->SetText("");
   tButt->fillStyle = UIRECT_WINDOW_STYLE_ONLY_BORDER;
   tButt->process = groundTestModeProcess;
   fullWindow->AddChild(tButt);
	*/


	playerListMode = 0;

	const int STATS_SMALL_H = 14;
	listWindow = new UIRectWindow(
		             GTM_BUTTON_STATSWIN, puma->ScreenW() - 255, 0,
						 puma->ScreenW(), 255, "WINDOW", fullWindow);
	listWindow->process = groundTestModeProcess;
   listWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	listWindow->fillArt = uiPlayerListArt[0];
//	fullWindow->AddChild(listWindow);

//	uiAlpha = 255;
//	edgeBar = new UIRectEdgeBar(GTM_BUTTON_STATSDRAG, -1,-1,8,-1,listWindow);
//	edgeBar->process = groundTestModeProcess;
//	listWindow->AddChild(edgeBar);
//	uiAlpha = GTM_UI_ALPHA;

	int j = 3;
	/*
	textBox = new UIRectTextBox(GTM_BUTTON_STATSNAME, -1,j,-1,j+20);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_ONLY_FILLED;
	textBox->SetText("Your Name Here");
	textBox->font = 1;
	textBox->process = groundTestModeProcess;
	listWindow->AddChild(textBox);
	j+= 20;
	*/
	j+= 20;
//	j+= STATS_SMALL_H;
/*
	textBox = new UIRectTextBox(GTM_BUTTON_STATSHEALTH, -1,j,-1,j+5);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_ONLY_FILLED;
	textBox->SetText("Health: 10/10");
	textBox->process = groundTestModeProcess;
	listWindow->AddChild(textBox);
	j+= 5;
*/
/*
   tButt = new UIRectTextButton(GTM_BUTTON_STATSITEMS, -1,j ,100,j+20);
   tButt->SetText("Items");
   tButt->process = groundTestModeProcess;
   listWindow->AddChild(tButt);
	tButt->font = 1;
   UIRectStep(1,0);
	j+= 20;

   tButt = new UIRectTextButton(GTM_BUTTON_STATSSKILLS, -2,-2  ,-2,-2);
   tButt->SetText("Skills");
   tButt->process = groundTestModeProcess;
	tButt->font = 1;
   listWindow->AddChild(tButt);
*/

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_STATSUP, 18,29,18+20,30+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListUpHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIUpButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   listWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_STATSDOWN, 18,155,18+20,155+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListDownHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIDownButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   listWindow->AddChild(tArtButt);

	j = 49;
	for (i = 0; i < 11; ++i)
	{
	   tButt = new UIRectTextButton(GTM_BUTTON_STATSLINE0 + i, 41,j ,250,j+STATS_SMALL_H);
		tButt->SetText("Item - - - - - - -");
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tButt->color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 255, 240, 190);
	   tButt->process = groundTestModeProcess;
		listWindow->AddChild(tButt);
		j+= STATS_SMALL_H+1;
	}

	textBox = new UIRectTextBox(GTM_BUTTON_STATSINFO, 75,48,210,66);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText("");
	textBox->font = 2;
	textBox->process = groundTestModeProcess;
	listWindow->AddChild(textBox);
//	uiAlpha = 255;
	/*
	textBox = new UIRectTextBox(GTM_BUTTON_STATSTEXT, puma->ScreenW() - 200,j + 4,
										 puma->ScreenW(),puma->ScreenH() - 80);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText("Welcome to\n* Blade Mistress *\nby Thom Robertson\n\n");
//	textBox->font = 1;
	textBox->process = groundTestModeProcess;
	fullWindow->AddChild(textBox);
	*/
//	uiAlpha = GTM_UI_ALPHA;
/*
   tButt = new UIRectTextButton(GTM_BUTTON_STATSCANCEL, -1,j  ,130,j+20);
   tButt->SetText("Cancel");
   tButt->process = groundTestModeProcess;
   listWindow->AddChild(tButt);
	tButt->font = 1;
   UIRectStep(1,0);
	j+= 20;

   tButt = new UIRectTextButton(GTM_BUTTON_STATSCONFIRM, -2,-2  ,-2,-2);
   tButt->SetText("Confirm");
   tButt->process = groundTestModeProcess;
	tButt->font = 1;
   listWindow->AddChild(tButt);
*/
	/*
	GTM_BUTTON_STATSCANCEL,
	GTM_BUTTON_STATSCONFIRM,
	*/
	textBox = new UIRectTextBox(
		             GTM_BUTTON_ITEM_INFO_TEXT, puma->ScreenW() - 209, 186 + 55,
						 puma->ScreenW(), 186 + 55 + 150);
	textBox->process = groundTestModeProcess;
   textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText(" ");
	fullWindow->AddChild(textBox);

	textBox = new UIRectTextBox(GTM_BUTTON_TIP_TEXT, 4,240, 285, 425);
	textBox->process = groundTestModeProcess;
   textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText(" ");
	fullWindow->AddChild(textBox);

	currentTip = 0;
	// load tip index
	FILE *fp = fopen("tipInfo.dat","r");
	if (fp)
	{
		fscanf(fp, "%d",&currentTip);
		fclose(fp);
	}

	SetTipText();

	// ******** other list window
	otherWindow = new UIRectWindow(
		             GTM_BUTTON_OTHERWIN, 0, 6, 255, 255 + 6, "WINDOW", fullWindow);
	otherWindow->process = groundTestModeProcess;
	otherWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	otherWindow->fillArt = uiOtherListArt;
//	fullWindow->AddChild(otherWindow);

	j = 3;
	textBox = new UIRectTextBox(GTM_BUTTON_OTHERNAME,  4+58,j+45,209 - 8+58,j+20+45);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText("Trader");
	textBox->font = 2;
	textBox->process = groundTestModeProcess;
	otherWindow->AddChild(textBox);
	j+= 20;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_OTHERUP, 18,29,18+20,30+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListUpHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIUpButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   otherWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_OTHERDOWN, 18,155,18+20,155+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListDownHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIDownButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   otherWindow->AddChild(tArtButt);

	j = 29+53;
	for (i = 2; i < 11; ++i)
	{
	   tButt = new UIRectTextButton(GTM_BUTTON_OTHERLINE0 + i-2, 41,j ,250,j+STATS_SMALL_H);
//	   tButt = new UIRectTextButton(GTM_BUTTON_OTHERLINE0 + i, 14,j ,200,j+STATS_SMALL_H);
		tButt->SetText("Item - - - - - - -");
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tButt->color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 255, 240, 190);
	   tButt->process = groundTestModeProcess;
		otherWindow->AddChild(tButt);
		j+= STATS_SMALL_H+1;
	}


	// ******** bank list window
	bankWindow = new UIRectWindow(
		             GTM_BUTTON_BANKWIN, 0, 6, 255, 255 + 6, "WINDOW", fullWindow);
	bankWindow->process = groundTestModeProcess;
	bankWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	bankWindow->fillArt = uiOtherListArt;
//	fullWindow->AddChild(bankWindow);

	j = 3;
	tArtButt = new UIRectArtTextButton(GTM_BUTTON_BANKNAME,  4+58,j+45,209 - 8+58,j+20+45);
	tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->SetText("Trader");
	tArtButt->font = 2;
	tArtButt->process = groundTestModeProcess;
	bankWindow->AddChild(tArtButt);
	j+= 20;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_OTHERUP, 18,29,18+20,30+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListUpHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIUpButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   bankWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_OTHERDOWN, 18,155,18+20,155+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListDownHigh.png",0xff000000);
//	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\UIDownButtonD.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   bankWindow->AddChild(tArtButt);

	j = 29+53;
	for (i = 2; i < 11; ++i)
	{
	   tButt = new UIRectTextButton(GTM_BUTTON_OTHERLINE0 + i-2, 41,j ,250,j+STATS_SMALL_H);
//	   tButt = new UIRectTextButton(GTM_BUTTON_OTHERLINE0 + i, 14,j ,200,j+STATS_SMALL_H);
		tButt->SetText("Item - - - - - - -");
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tButt->color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 255, 240, 190);
	   tButt->process = groundTestModeProcess;
		bankWindow->AddChild(tButt);
		j+= STATS_SMALL_H+1;
	}

	// ******** her secure list window
	secureHerWindow = new UIRectWindow(
		             GTM_BUTTON_HER_SECURE_WIN, -5, 20, 240, 260, "WINDOW", fullWindow);
	secureHerWindow->process = groundTestModeProcess;
	secureHerWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	secureHerWindow->fillArt = uiTradeListArt;
//	secureHerWindow->fillColor = D3DCOLOR_ARGB(055, 155, 155, 255);

	j = 3;
	textBox = new UIRectTextBox(GTM_BUTTON_HER_SECURE_NAME,  4+50,j+42,209 - 8+50,j+20+42);
	textBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	textBox->SetText("");
	textBox->font = 2;
	textBox->process = groundTestModeProcess;
	secureHerWindow->AddChild(textBox);
	j+= 20;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_HER_SECURE_UP, 18,29,18+20,29+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListUpHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   secureHerWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_HER_SECURE_DOWN, 18,155,18+20,155+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListDownHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   secureHerWindow->AddChild(tArtButt);

	j = 29+53;
	for (i = 2; i < 11; ++i)
	{
	   tButt = new UIRectTextButton(GTM_BUTTON_HER_SECURE_LINE0 + i-2, 41,j ,200,j+STATS_SMALL_H);
		tButt->SetText("Item - - - - - - -");
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tButt->color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 255, 240, 190);
	   tButt->process = groundTestModeProcess;
		secureHerWindow->AddChild(tButt);
		j+= STATS_SMALL_H+1;
	}

	// ******** my secure list window
	secureYouWindow = new UIRectWindow(
		             GTM_BUTTON_YOU_SECURE_WIN, 189, 20, 430, 260, "WINDOW", fullWindow);
   secureYouWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	secureYouWindow->process = groundTestModeProcess;
	secureYouWindow->fillArt = uiTradeListArt;
	secureYouWindow->fillColor = D3DCOLOR_ARGB(155, 155, 155, 255);

	j = 3;
	tArtButt = new UIRectArtTextButton(GTM_BUTTON_YOU_SECURE_NAME,  4+50,j+42,209 - 8+50,j+20+42);
	tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->SetText("");
	tArtButt->font = 2;
	tArtButt->process = groundTestModeProcess;
	secureYouWindow->AddChild(tArtButt);
	j+= 20;

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_YOU_SECURE_UP, 18,29,18+20,29+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListUpHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   secureYouWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_YOU_SECURE_DOWN, 18,155,18+20,155+81);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\ListDownHigh.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   secureYouWindow->AddChild(tArtButt);

	j = 29+53;
	for (i = 2; i < 11; ++i)
	{
	   tButt = new UIRectTextButton(GTM_BUTTON_YOU_SECURE_LINE0 + i-2, 41,j ,200,j+STATS_SMALL_H);
		tButt->SetText("Item - - - - - - -");
		tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		tButt->color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 255, 240, 190);
	   tButt->process = groundTestModeProcess;
		secureYouWindow->AddChild(tButt);
		j+= STATS_SMALL_H+1;
	}

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_CHANNEL1,
		             50, 0, 50+49,32-12);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\chan1on.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\chan1off.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL   ,"dat\\chan1off.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->artOffsetY = -11;
	tArtButt->isMomentary = FALSE;
	tArtButt->isDepressed = chatChannelValues & 1;
   fullWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_CHANNEL2,
		             50+49, 0, 50+49+46,32-12);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\chan2on.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\chan2off.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL   ,"dat\\chan2off.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->artOffsetY = -11;
	tArtButt->isMomentary = FALSE;
	tArtButt->isDepressed = chatChannelValues & 2;
   fullWindow->AddChild(tArtButt);

   tArtButt = new UIRectArtTextButton(GTM_BUTTON_CHANNEL3,
		             50+49+46, 0, 50+49+46+33,32-12);
   tArtButt->process = groundTestModeProcess;
	tArtButt->SetArt(UIRECT_BCOL_DEPRESSED,"dat\\chan3on.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_ACTIVE   ,"dat\\chan3off.png",0xff000000);
	tArtButt->SetArt(UIRECT_BCOL_NORMAL   ,"dat\\chan3off.png",0xff000000);
   tArtButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tArtButt->artOffsetY = -11;
	tArtButt->isMomentary = FALSE;
	tArtButt->isDepressed = chatChannelValues & 4;
   fullWindow->AddChild(tArtButt);

   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff


	return(0);
}

//******************************************************************
int GroundTestMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

	curgroundTestMode->RemoveNonInvWindows();
	delete otherWindow;
	delete secureHerWindow;
	delete secureYouWindow;

	if (fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN))
		fullWindow->childRectList.Remove(listWindow);
	delete listWindow;


//	// DebugOutput("3");
	if (fullWindow->childRectList.Find(chatWin))
		fullWindow->childRectList.Remove(chatWin);
//	// DebugOutput("4");

	if (!fullWindow->childRectList.Find(bugReportWindow))
		delete bugReportWindow;
	delete fullWindow;
	delete charInvWindow;
//	// DebugOutput("5");

	if (actionWindow)
		delete actionWindow;
//	// DebugOutput("6");

	for (int i = 0; i < 4; ++i)
	{
		SAFE_RELEASE(uiPlayerListArt[i]);
	}
	for (int i = 0; i < 9; ++i)
	{
		SAFE_RELEASE(radarArt[i]);
	}

	SAFE_RELEASE(uiTradeListArt);
	SAFE_RELEASE(uiPlayerFrameArt);

	SAFE_RELEASE(uiPopUpTop);
	SAFE_RELEASE(uiPopUpBottom);
	SAFE_RELEASE(uiPopUpLongArt);
	SAFE_RELEASE(uiPopUpArt);
	SAFE_RELEASE(uiPopUpSpaceArt);
	SAFE_RELEASE(uiOtherListArt);
//	// DebugOutput("7");


   SAFE_RELEASE(barArt[1]);
   SAFE_RELEASE(barArt[0]);
   SAFE_RELEASE(compassArt);
   SAFE_RELEASE(mouseArt);
//	// DebugOutput("8");


	return(0);
}

//******************************************************************
int GroundTestMode::Tick(void)
{

	++logDoneCounter;
	if (logDoneCounter > 300)
		aLog.Finish();

	char tempText[1024];

	if (gTeleportHappenedFlag)
	{
		RemoveNonInvWindows();
		RemoveActionButtons();
		gTeleportHappenedFlag = FALSE;
	}
//	// DebugOutput("GTMTick -1\n");

   D3DXMATRIX matWorld, mat2;
//	float tweakF;

	if (itemInfoPtr != -1 && hoverTime + 2000 < timeGetTime())
	{
		// send a request for more info on this item
		MessExtendedInfoRequest mess;
		mess.itemPtr  = itemInfoPtr;
		mess.listType = itemInfoListMode;
		lclient->SendMsg(sizeof(mess),&mess);

		itemInfoPtr = -1;
	}

	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

	if ((SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType) && bboClient->isEditingDungeon)
	{
		if (!fullWindow->childRectList.Find(GTM_BUTTON_DUNEDIT_WALL))
		{
			UIRectTextButton *tButt;
			// 153x26
			tButt = new UIRectTextButton(GTM_BUTTON_DUNEDIT_WALL,
				                          puma->ScreenW() - 188 , puma->ScreenH() - 149,
												  puma->ScreenW()-188+153, puma->ScreenH()-149+20);
			tButt->SetText("Change Wall");
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);
			UIRectStep(0,1);

			tButt = new UIRectTextButton(GTM_BUTTON_DUNEDIT_OUTER, -2,-2,-2,-2);
			tButt->SetText("Change Outer Wall");
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);
			UIRectStep(0,1);

			tButt = new UIRectTextButton(GTM_BUTTON_DUNEDIT_FLOOR, -2,-2,-2,-2);
			tButt->SetText("Change Floor");
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);
			UIRectStep(0,1);

			tButt = new UIRectTextButton(GTM_BUTTON_DUNEDIT_CAMERA, -2,-2,-2,-2);
			tButt->SetText("Change Camera");
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);

		   fullWindow->Arrange();

		}

	}
	else
	{
		UIRectTextButton *tButt;

		tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_DUNEDIT_WALL);
		if (tButt)
		{
			fullWindow->childRectList.Remove(tButt);
			delete tButt;

			tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_DUNEDIT_OUTER);
			if (tButt)
			{
				fullWindow->childRectList.Remove(tButt);
				delete tButt;
			}

			tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_DUNEDIT_FLOOR);
			if (tButt)
			{
				fullWindow->childRectList.Remove(tButt);
				delete tButt;
			}

			tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_DUNEDIT_CAMERA);
			if (tButt)
			{
				fullWindow->childRectList.Remove(tButt);
				delete tButt;
			}
		}
	}

	if (giveMoneyMode)
	{
		if (!fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYTITLE))
		{
			UIRectEditLine *tEdit;
			UIRectTextBox *tBox;
			tBox = new UIRectTextBox(GTM_BUTTON_GIVEMONEYTITLE,
				                          puma->ScreenW()/2 - 100, puma->ScreenH()/2 - 13 + 60,
												  puma->ScreenW()/2 +  99, puma->ScreenH()/2 + 10 + 60);

			if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				tBox->SetText("Enter new bank amount");
			else
				tBox->SetText("Enter amount to give");
			tBox->process = groundTestModeProcess;
			tBox->font = 2;
			tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tBox->fillArt = uiPopUpLongArt;
			fullWindow->AddChild(tBox);
			UIRectStep(0,1);

			tEdit = new UIRectEditLine(GTM_BUTTON_GIVEMONEYEDLINE, -2,-2,-2,-2);
			tEdit->SetText("0");
			tEdit->process = groundTestModeProcess;
			tEdit->least = -10000;
			tEdit->most  =  10000;
			tEdit->fillArt = NULL;
			tEdit->font = 1;
			fullWindow->AddChild(tEdit);

			tEdit->hasTypingFocus = TRUE;
			tEdit->suppressEnter = TRUE;

		   fullWindow->Arrange();

		}

	}
	else
	{
		UIRectEditLine *tEdit;
		UIRectTextBox *tBox;

		tBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYTITLE);
		if (tBox)
		{
			fullWindow->childRectList.Remove(tBox);
			delete tBox;

			tEdit = (UIRectEditLine *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
			if (tEdit)
			{
				fullWindow->childRectList.Remove(tEdit);
				delete tEdit;
			}
		}
	}

	if (bboClient->needToName)
	{
		namePetMode = TRUE;
		bboClient->needToName = FALSE;
	}

	if (namePetMode)
	{
		if (!fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_TITLE))
		{
			UIRectEditLine *tEdit;
			UIRectTextBox *tBox;
			tBox = new UIRectTextBox(GTM_BUTTON_NAMEPET_TITLE,
				                          puma->ScreenW()/2 - 100, puma->ScreenH()/2 - 13 + 30,
												  puma->ScreenW()/2 +  99, puma->ScreenH()/2 + 10 + 30);
			tBox->SetText("Name your new pet");
			tBox->process = groundTestModeProcess;
			tBox->font = 2;
			tBox->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tBox->fillArt = uiPopUpLongArt;
			fullWindow->AddChild(tBox);
			UIRectStep(0,1);

			tEdit = new UIRectEditLine(GTM_BUTTON_NAMEPET_EDLINE, -2,-2,-2,-2);
			tEdit->SetText("");
			tEdit->process = groundTestModeProcess;
			tEdit->fillArt = NULL;
			tEdit->font = 1;
			fullWindow->AddChild(tEdit);

			tEdit->hasTypingFocus = TRUE;
			tEdit->suppressEnter = TRUE;

		   fullWindow->Arrange();

		}
	}
	else
	{
		UIRectEditLine *tEdit;
		UIRectTextBox *tBox;

		tBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_TITLE);
		if (tBox)
		{
			fullWindow->childRectList.Remove(tBox);
			delete tBox;

			tEdit = (UIRectEditLine *) fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_EDLINE);
			if (tEdit)
			{
				fullWindow->childRectList.Remove(tEdit);
				delete tEdit;
			}
		}
	}

	if (playerIsAdmin)
	{
		/*
		if (!fullWindow->childRectList.Find(GTM_BUTTON_ADMIN_INFO))
		{
			UIRectTextBox *tBox;
			tBox = new UIRectTextBox(GTM_BUTTON_ADMIN_INFO,
				                          puma->ScreenW() - 160 , puma->ScreenH() - 205,
												  puma->ScreenW(), puma->ScreenH() - 125);
			tBox->SetText("Admin Info");
			tBox->process = groundTestModeProcess;
			tBox->font = 2;
			fullWindow->AddChild(tBox);
		}
		*/
	}

	if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
	{
		if (!fullWindow->childRectList.Find(GTM_BUTTON_SECURE_DECLINE))
		{
			UIRectTextButton *tButt;
			tButt = new UIRectTextButton(GTM_BUTTON_SECURE_DECLINE,
								35, 186 + 70 + 2, 35 + 153, 186 + 70 + 2 + 26);
			tButt->SetText("Decline");
		   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);
			UIRectStep(0,1);

			tButt = new UIRectTextButton(GTM_BUTTON_SECURE_ACCEPT,
								205 + 25, 186 + 70 + 2, 205 + 25 + 153, 186 + 70 + 2 + 26);
			tButt->SetText("ACCEPT");
		   tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
			tButt->process = groundTestModeProcess;
			tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
			tButt->fillArt = uiPopUpArt;
			fullWindow->AddChild(tButt);
			UIRectStep(0,1);

		   fullWindow->Arrange();
		}
	}
	else
	{
		UIRectTextButton *tButt;

		tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_SECURE_DECLINE);
		if (tButt)
		{
			fullWindow->childRectList.Remove(tButt);
			delete tButt;
		}
		tButt = (UIRectTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_SECURE_ACCEPT);
		if (tButt)
		{
			fullWindow->childRectList.Remove(tButt);
			delete tButt;
		}
	}

//	if (server)
//		server->Tick();
//	// DebugOutput("GTMTick -2\n");

	bboClient->Tick();
//	// DebugOutput("GTMTick -2.5\n");

	HandleMessages();

//	// DebugOutput("GTMTick -3\n");

	WindowServicer();
	// processing

//  	// DebugOutput("GTMTick -4\n");

	if (keysPressed & KEY_LEFT)
	{
		camSpinDelta -= 0.01f;
		if (camSpinDelta < -0.1f)
			 camSpinDelta = -0.1f;
	}
	else if (keysPressed & KEY_RIGHT)
	{
		camSpinDelta += 0.01f;
		if (camSpinDelta > 0.1f)
			 camSpinDelta = 0.1f;
	}
	else
	{
		if (camSpinDelta > 0)
		{
			camSpinDelta -= 0.02f;
			if (camSpinDelta < 0)
				camSpinDelta = 0;
		}
		else
		{
			camSpinDelta += 0.02f;
			if (camSpinDelta > 0)
				camSpinDelta = 0;
		}
	}

	bboClient->curCamAngle += camSpinDelta;

	bboClient->curCamAngle -= rmbDownPos * 0.6f;




	if (keysPressed & KEY_FIRE4)
	{
		camRaiseDelta -= 0.005f;
		if (camRaiseDelta < -0.05f)
			 camRaiseDelta = -0.05f;
	}
	else if (keysPressed & KEY_FIRE3)
	{
		camRaiseDelta += 0.005f;
		if (camRaiseDelta > 0.05f)
			 camRaiseDelta = 0.05f;
	}
	else
	{
		if (camRaiseDelta > 0)
		{
			camRaiseDelta -= 0.01f;
			if (camRaiseDelta < 0)
				camRaiseDelta = 0;
		}
		else
		{
			camRaiseDelta += 0.01f;
			if (camRaiseDelta > 0)
				camRaiseDelta = 0;
		}
	}

	bboClient->camPitch += camRaiseDelta;

	bboClient->camPitch -= rmbDownPosY * 0.1f;
	if (bboClient->camPitch > 0.20f)
		bboClient->camPitch = 0.20f;
	if (bboClient->camPitch < -0.60f)
		bboClient->camPitch = -0.60f;


/*
	if (monsterHealthTimer > 0)
	{
		--monsterHealthTimer;
		if (monsterHealthTimer <= 0)
		{
			monsterHealthBox->SetText("");
			monsterHealthLength = 0;
		}
	}
  */
	if (moveLockoutTimer > 0)
	{
		--moveLockoutTimer;
	}


	// re-position channel buttons on top of chat window
	ChatWindow *chat = (ChatWindow *) 
		            fullWindow->childRectList.Find(GTM_BUTTON_CHATBOX);
	if (chat)
	{
		UIRectArtTextButton *tButt;

		tButt = (UIRectArtTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL1);
		if (tButt)
		{
			tButt->box.top    = chat->box.top - 20;
			tButt->box.bottom = chat->box.top + 2;
		}
		tButt = (UIRectArtTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL2);
		if (tButt)
		{
			tButt->box.top    = chat->box.top - 20;
			tButt->box.bottom = chat->box.top + 2;
		}
		tButt = (UIRectArtTextButton *) fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL3);
		if (tButt)
		{
			tButt->box.top    = chat->box.top - 20;
			tButt->box.bottom = chat->box.top + 2;
		}

		fullWindow->Arrange();
	}

	// start drawing

	puma->StartRenderingFrame(0,0,30);

//	// DebugOutput("GTMTick -5\n");

	bboClient->Draw();

//	aLog.Log("GroundTestMode::Tick 1 **\n");

	D3DSURFACE_DESC desc;
	compassArt->GetLevelDesc(0, &desc);
	RECT dRect3 = {0, 0, desc.Width, desc.Height};
	puma->DrawRect(compassArt,dRect3,0xffffffff);

//	aLog.Log("GroundTestMode::Tick 2 **\n");

	int centerCellX, centerCellY;
	centerCellX = bboClient->playerAvatar->cellX;
	centerCellY = bboClient->playerAvatar->cellY;

	if (256 == bboClient->curMapSizeY && mapState)
	{
		if (1 == mapState)
		{
			radarArt[0]->GetLevelDesc(0, &desc);
			RECT dRectRadar0 = {centerX, centerY - desc.Height, centerX + desc.Width, centerY};
			puma->DrawRect(radarArt[0],dRectRadar0,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[1]->GetLevelDesc(0, &desc);
			RECT dRectRadar1 = {centerX - desc.Width, centerY - desc.Height, centerX, centerY};
			puma->DrawRect(radarArt[1],dRectRadar1,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[3]->GetLevelDesc(0, &desc);
			RECT dRectRadar3 = {centerX - desc.Width, centerY, centerX, centerY + desc.Height};
			puma->DrawRect(radarArt[3],dRectRadar3,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[2]->GetLevelDesc(0, &desc);
			RECT dRectRadar2 = {centerX, centerY, centerX + desc.Width, centerY + desc.Height};
			puma->DrawRect(radarArt[2],dRectRadar2,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));
		}
		else
		{
			radarArt[4]->GetLevelDesc(0, &desc);
			RECT dRectRadar0 = {centerX, centerY - desc.Height, centerX + desc.Width, centerY};
			puma->DrawRect(radarArt[4],dRectRadar0,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[5]->GetLevelDesc(0, &desc);
			RECT dRectRadar1 = {centerX - desc.Width, centerY - desc.Height, centerX, centerY};
			puma->DrawRect(radarArt[5],dRectRadar1,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[7]->GetLevelDesc(0, &desc);
			RECT dRectRadar3 = {centerX - desc.Width, centerY, centerX, centerY + desc.Height};
			puma->DrawRect(radarArt[7],dRectRadar3,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));

			radarArt[6]->GetLevelDesc(0, &desc);
			RECT dRectRadar2 = {centerX, centerY, centerX + desc.Width, centerY + desc.Height};
			puma->DrawRect(radarArt[6],dRectRadar2,0xffffffff);//D3DCOLOR_ARGB(50,55,0,55));
		}

		if ((bboClient->flashCounter/4)&1)
		{
			radarArt[8]->GetLevelDesc(0, &desc);
			RECT dRectDot = {centerX + 128 - centerCellX, 
				              centerY - 128 + centerCellY,
								  centerX + 128 - centerCellX + desc.Width,
								  centerY - 128 + centerCellY + desc.Height};
			puma->DrawRect(radarArt[8],dRectDot, D3DCOLOR_ARGB(255,255,0,0));
		}
	}
/*	
	uiPopUpLongArt->GetLevelDesc(0, &desc);
	RECT dRect5 = {puma->ScreenW()-255 - 150, 0, puma->ScreenW()-150, 32};
	puma->DrawRect(uiPopUpLongArt,dRect5,0xffffffff);

	sprintf(tempText,"Level %ld",	gGamePlayerLevel);
	dRect5.top    += 5;
	dRect5.bottom -= 3;
	dRect5.left   += 8;
	CD3DFont *f = puma->GetDXFont(2);
   f->DrawText( dRect5, NULL, D3DCOLOR_ARGB(255,255,255,255), tempText, DT_LEFT);

	sprintf(tempText,"Health %ld",	gGamePlayerHP);
	dRect5.left   += 70;
	if (gGamePlayerHP*3 < gGamePlayerMaxHP)
	   f->DrawText( dRect5, NULL, D3DCOLOR_ARGB(255,255,0,0), tempText, DT_LEFT);
	else if (gGamePlayerHP < gGamePlayerMaxHP)
	   f->DrawText( dRect5, NULL, D3DCOLOR_ARGB(255,255,255,0), tempText, DT_LEFT);
	else
	   f->DrawText( dRect5, NULL, D3DCOLOR_ARGB(255,0,255,0), tempText, DT_LEFT);
*/
//	RECT tRect = {100,100,228,228};
//	puma->DrawRect(bboClient->groundMap->pTexture, tRect, 0xffffffff);

//	// DebugOutput("GTMTick 0\n");
   fullWindow->Draw();

//	aLog.Log("GroundTestMode::Tick 3 **\n");

	uiPlayerFrameArt->GetLevelDesc(0, &desc);
	RECT dRect4 = {puma->ScreenW()-255, 0, puma->ScreenW(), 255};
	puma->DrawRect(uiPlayerFrameArt,dRect4,0xffffffff);

//	aLog.Log("GroundTestMode::Tick 4 **\n");

   charInvWindow->Draw();

//	aLog.Log("GroundTestMode::Tick 5 **\n");

	if (actionWindow)
		actionWindow->Draw();

//	aLog.Log("GroundTestMode::Tick 6 **\n");

//	// DebugOutput("GTMTick 1\n");
//	float percent;
//	int i = GetCardinalDirection(bboClient->curCamAngle, &percent);
	int i = GetCardinalDirection(bboClient->curCamAngle);
	i *= 2;
//	percent = 1.0f - percent;

//	aLog.Log("GroundTestMode::Tick 7 **\n");

	// draw rosetta  ***
	if (bboClient->controlledMonsterID != -1)
	{
		BBOMob *curMob = (BBOMob *) bboClient->monsterList->First();
		while (curMob)
		{
			if (curMob->mobID == bboClient->controlledMonsterID)
			{
				centerCellX = curMob->cellX;
				centerCellY = curMob->cellY;
				curMob = (BBOMob *) bboClient->monsterList->Last();
			}
			curMob = (BBOMob *) bboClient->monsterList->Next();
		}
	}

	sprintf(tempText,"%dN %dE %c%c",	
		     bboClient->curMapSizeY - centerCellY, bboClient->curMapSizeX - centerCellX, 
			  dirSet[i], dirSet[i+1]);
	compassBox->SetText(tempText);

	int showLevel = TRUE;
	if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
		showLevel = FALSE;
	if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
		showLevel = FALSE;
	else if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
		showLevel = FALSE;
	else if (fullWindow->childRectList.Find(GTM_BUTTON_HER_SECURE_WIN))
		showLevel = FALSE;
	if (showLevel)
		sprintf(tempText,"Level %ld",	gGamePlayerLevel);
	else
		sprintf(tempText,"");
	compassBox2->SetText(tempText);

//			tempX = curgroundTestMode->GetCardinalDirection(bboClient->curCamAngle);

//			messDungeonChange.x = bboClient->playerAvatar->cellX;
//			messDungeonChange.y = bboClient->playerAvatar->cellY;

//	aLog.Log("GroundTestMode::Tick 8 **\n");

	
	if (moveLockoutTimer <= 0)
	{
//		puma->DrawRect(rosetteArt[0], puma->ScreenW()/2 - 54, puma->ScreenH() - 50, 
//		            puma->ScreenW()/2 - 54 + 128, puma->ScreenH() - 50 + 32, 0xffffffff);

		// if forward key is still pressed, try to go forward!
		if ( keysPressed & KEY_ACCEL)
		{
			if (moveRequestTimer + 1000 < timeGetTime())
			{
				moveRequestTimer= timeGetTime();

				int tempX = GetCardinalDirection(bboClient->curCamAngle);
				MessAvatarMoveRequest bMoveReq;
				bMoveReq.avatarID = bboClient->playerAvatarID;
				bMoveReq.x = tempX;
				lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
				RemoveNonInvWindows();
				curgroundTestMode->RemoveActionButtons();
			}
		}

	}

/*	
	int positionX = puma->ScreenW() - 71 - 50 - 9;
	positionX += percent * 100;

	puma->DrawRect(rosetteArt[i+1], positionX, puma->ScreenH() - 40, 
	            positionX + 32, puma->ScreenH() - 40 + 32, 0xffffffff);
					*/
	// ***
	/*
//	// DebugOutput("GTMTick 2\n");
//	screenHealth = screenMaxHealth = 1;
	// draw health bar  ***
	puma->DrawRect(barArt[0], centerX +192,        puma->ScreenH() - 21 - 8, 
		                                  centerX + 192 + 128, puma->ScreenH() - 21, 0xffffffff);
	percent = (float) screenHealth / (float) screenMaxHealth * 120.0f;
	puma->DrawRect(barArt[1], centerX +192,        puma->ScreenH() - 21 - 8, 
		                                  centerX + 192 + percent, puma->ScreenH() - 21, 0xffffffff);
		
	// ***

	// draw monster's health bar  ***

//	// DebugOutput("GTMTick 3\n");
	if (monsterHealthLength > 0)
	{
		puma->DrawRect(barArt[0], centerX +192,        puma->ScreenH() - 41 - 8, 
		                                  centerX + 192 + 128, puma->ScreenH() - 41, 0xffffffff);
		puma->DrawRect(barArt[1], centerX +192,        puma->ScreenH() - 41 - 8, 
		                                  centerX + 192 + monsterHealthLength, puma->ScreenH() - 41, 0xffffffff);
	}
	// ***

//	puma->DrawRect(buttonArt, centerX +58, puma->ScreenH() - 35, 
//		                                  centerX + 58 + 140, puma->ScreenH() - 35 + 32, 0xffffffff);
	*/
//	// DebugOutput("GTMTick 4\n");
	puma->DrawMouse(mouseArt);

//	aLog.Log("GroundTestMode::Tick 9 **\n");

	puma->FinishRenderingFrame();

//	aLog.Log("GroundTestMode::Tick 10 **\n");

//	// DebugOutput("GTMTick 5\n");
//	DEBUG_MSG("Debug!");

	return(0);
}

//*******************************************************************************
int GroundTestMode::TypingBoxOn(void)
{
	UIRectEditLine *edMoney = (UIRectEditLine *)
		    fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
	UIRectEditLine *edPetName = (UIRectEditLine *)
		    fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_EDLINE);

 	if (edLine->hasTypingFocus)
		return TRUE;
 	if (edMoney && edMoney->hasTypingFocus)
		return TRUE;
 	if (edPetName && edPetName->hasTypingFocus)
		return TRUE;

	return FALSE;
}

//*******************************************************************************
long GroundTestMode::WindowServicer(void)
{

	if (POMSezQuit)
	{
		newGameMode = NULL;
		curgroundTestMode->retState = GMR_POP_ME;
	}

//	AFOFleet *fleet;
	UIRectEditLine::blinkTimer++;

	MessAvatarMoveRequest bMoveReq;

	UIRectWindow *theWindow = fullWindow;
	if (actionWindow)
		theWindow = actionWindow;

	assert(theWindow->box.top > -100);


	//int         GetCardinalDirection(float angle);
	if (bboClient->drunkenWalk && moveRequestTimer + 1000 < timeGetTime())
	{
		moveRequestTimer= timeGetTime();

		bMoveReq.avatarID = bboClient->playerAvatarID;
		bMoveReq.x = rand() % 4;
		lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
	}



	// check input
//	puma->PumpInput();
	int tempX, i; //, tempY;
	int result;
	MessTryCombine messTryCombine;
//	char tempText[1024];

	PumaInputEvent *pie = puma->TakeNextInputEvent();
	while (pie)
	{
		switch(pie->WhatAmI())
		{
		case PUMA_INPUT_KEYDOWN:
			if ( ( ( 87 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 38 == pie->scancode ) && !(extraKeyFlags)) // if up
				keysPressed |= KEY_ACCEL;

			if ( ( ( 83 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 40 == pie->scancode ) && !(extraKeyFlags)) // if down
				keysPressed |= KEY_BRAKE;

			if ( ( ( 68 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 39 == pie->scancode ) && !(extraKeyFlags)) // if right
				keysPressed |= KEY_LEFT;

			if ( ( ( 65 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 37 == pie->scancode ) && !(extraKeyFlags)) // if left
				keysPressed |= KEY_RIGHT;

			if (104 == pie->scancode ) // keypad 8
				keysPressed |= KEY_FIRE3;

			if (98 == pie->scancode ) // keypad 2
				keysPressed |= KEY_FIRE4;

			if (16 == pie->scancode)
				extraKeyFlags |= EXTRA_KEY_SHIFT_DOWN;

			if (17 == pie->scancode)
				extraKeyFlags |= EXTRA_KEY_CTRL_DOWN;

			if (39 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if right
				theWindow->Action(UIRECT_FORWARD_ARROW, 0, pie->scancode);
			else if (37 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if left
				theWindow->Action(UIRECT_BACK_ARROW, 0, pie->scancode);
			else if (39 == pie->scancode && extraKeyFlags & EXTRA_KEY_CTRL_DOWN) // if right
				theWindow->Action(UIRECT_JUMP_FORWARD_ARROW, 0, pie->scancode);
			else if (37 == pie->scancode && extraKeyFlags & EXTRA_KEY_CTRL_DOWN) // if left
				theWindow->Action(UIRECT_JUMP_BACK_ARROW, 0, pie->scancode);
			else if (38 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if up
			{
				theWindow->Action(UIRECT_UP_ARROW, 0, pie->scancode);
				if (!edLine->hasTypingFocus)
				{
					edLine->hasTypingFocus = TRUE;
					edLine->suppressEnter = TRUE;
//					delete pie; // IMPORTANT to delete the message after using it!
//					return 0;
				}
			}
			else if (40 == pie->scancode && extraKeyFlags & EXTRA_KEY_SHIFT_DOWN) // if down
			{
				theWindow->Action(UIRECT_DOWN_ARROW, 0, pie->scancode);
			}
			else 
				theWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

//         theWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

			break;

		case PUMA_INPUT_KEYUP:

			if (16 == pie->scancode)
				extraKeyFlags &= ~(EXTRA_KEY_SHIFT_DOWN);

			if (17 == pie->scancode)
				extraKeyFlags &= ~(EXTRA_KEY_CTRL_DOWN);

			if ( ( ( 87 == pie->scancode && !edLine->hasTypingFocus ) || 38 == pie->scancode ) && 
				 moveLockoutTimer <= 0  && !(extraKeyFlags)) // if up
			{
				if ((keysPressed & KEY_ACCEL) && moveRequestTimer + 1000 < timeGetTime())
				{
					moveRequestTimer= timeGetTime();

					tempX = GetCardinalDirection(bboClient->curCamAngle);
					bMoveReq.avatarID = bboClient->playerAvatarID;
					bMoveReq.x = tempX;
					lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
					RemoveNonInvWindows();
					RemoveActionButtons();
				}

				keysPressed &= ~(KEY_ACCEL);

			}
			else if (( 87 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 38 == pie->scancode) // if up
			{
				keysPressed &= ~(KEY_ACCEL);
			}

			if (( 83 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 40 == pie->scancode ) // if down
			{
				keysPressed &= ~(KEY_BRAKE);
//				if (bboClient->showAttack)
//					bboClient->showAttack = 0;
//				else
//					bboClient->showAttack = 1;
			}
			if (( 68 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 39 == pie->scancode ) // if right
			{
				keysPressed &= ~(KEY_LEFT);
			}
			if (( 65 == pie->scancode && !edLine->hasTypingFocus && (localInfoFlags & LOCAL_FLAGS_WASD)) || 37 == pie->scancode ) // if left
			{
				keysPressed &= ~(KEY_RIGHT);
			}

			if (104 == pie->scancode ) // keypad 8
				keysPressed &= ~(KEY_FIRE3);

			if (98 == pie->scancode ) // keypad 2
				keysPressed &= ~(KEY_FIRE4);

			if (101 == pie->scancode ) // keypad 5
				bboClient->camPitch = -0.32f;

			if (13 == pie->asciiValue || 39 == pie->asciiValue ) // ENTER, '
			{
				UIRectEditLine *edMoney = (UIRectEditLine *)
					    fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
				UIRectEditLine *edPetName = (UIRectEditLine *)
					    fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_EDLINE);
				if (edMoney)
				{
					if (!edMoney->hasTypingFocus)
					{
						edMoney->hasTypingFocus = TRUE;
						edMoney->suppressEnter = TRUE;
						delete pie; // IMPORTANT to delete the message after using it!
						return 0;
					}
				}
				else if (edPetName)
				{
					if (!edPetName->hasTypingFocus)
					{
						edPetName->hasTypingFocus = TRUE;
						edPetName->suppressEnter = TRUE;
						delete pie; // IMPORTANT to delete the message after using it!
						return 0;
					}
				}
				else if (!edLine->hasTypingFocus)
				{
					edLine->hasTypingFocus = TRUE;
					edLine->suppressEnter = TRUE;
					delete pie; // IMPORTANT to delete the message after using it!
					return 0;
				}
			}
			if ('f' == pie->asciiValue && !TypingBoxOn()) // 'f'
			{
				if (bboClient->showFrameRate)
					bboClient->showFrameRate = FALSE;
				else
					bboClient->showFrameRate = TRUE;
			}
			if (109 == pie->asciiValue && !TypingBoxOn()) // 'm'
			{
				++mapState;
				if (mapState > 2)
					mapState = 0;
			}
			if (117 == pie->scancode && !TypingBoxOn()) // F6
			{
				if (bboClient->drunkenWalk)
					bboClient->drunkenWalk = FALSE;
				else
					bboClient->drunkenWalk = TRUE;
			}
			if (116 == pie->asciiValue && !TypingBoxOn()) // 't'
			{
				++currentTip;
				SetTipText();

				// save tip index
				FILE *fp = fopen("tipInfo.dat","w");
				if (fp)
				{
					fprintf(fp, "%d ",currentTip);
					fclose(fp);
				}
			}
			if (113 == pie->scancode && !TypingBoxOn()) // 'F2'
			{
				currentTip = 0;
				SetTipText();

				// save tip index
				FILE *fp = fopen("tipInfo.dat","w");
				if (fp)
				{
					fprintf(fp, "%d ",currentTip);
					fclose(fp);
				}
			}
			if (27 == pie->asciiValue) // ESC
			{
				ClearEverythingFromScreen();
			}

			if (('1' == pie->asciiValue || 45 == pie->scancode) && !TypingBoxOn()) // INS
			{
				ProcessListPress(GTM_BUTTON_LIST_INV);
			}

			if (('3' == pie->asciiValue || 33 == pie->scancode) && !TypingBoxOn()) // PGUP
			{
				ProcessListPress(GTM_BUTTON_LIST_WRK);
			}

			if (('4' == pie->asciiValue || 34 == pie->scancode) && !TypingBoxOn()) // PGDN
			{
				ProcessListPress(GTM_BUTTON_LIST_SKL);
			}

			if (('2' == pie->asciiValue || 36 == pie->scancode) && !TypingBoxOn()) // HOME
			{
				ProcessListPress(GTM_BUTTON_LIST_WLD);
			}
			
			/*
			if (!(localInfoFlags & LOCAL_FLAGS_HOTKEY_CTRL) || 
				 extraKeyFlags & EXTRA_KEY_CTRL_DOWN)
			{
				for (i = 0; 0 == pie->asciiValue && !TypingBoxOn() && i < 19; ++i)
				{
					if (skillHotKeyArray[i] == pie->scancode)
					{
						messTryCombine.skillID = skillHotKeyArray[i];
						lclient->SendMsg(sizeof(messTryCombine),(void *)&messTryCombine);
					}
				}
			}
			*/
			if ((47 == pie->asciiValue || 59 == pie->asciiValue) && 
				 !TypingBoxOn()) // '/', ';'
			{
				UIRectEditLine *edMoney = (UIRectEditLine *)
					    fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
				UIRectEditLine *edNamePet = (UIRectEditLine *)
					    fullWindow->childRectList.Find(GTM_BUTTON_NAMEPET_EDLINE);
				if (!edMoney && !edNamePet)
				{
					edLine->hasTypingFocus = TRUE;
					edLine->suppressEnter = TRUE;
					edLine->SetText("/");
					if(59 == pie->asciiValue)
						edLine->SetText("/emote ");
					delete pie; // IMPORTANT to delete the message after using it!
					return 0;
				}
			}
			
         theWindow->Action(UIRECT_KEY_UP,   0, pie->scancode);
	      theWindow->Action(UIRECT_KEY_CHAR, 0, pie->asciiValue);

			break;

		case PUMA_INPUT_LMBDOWN:
		   result = charInvWindow->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
			if (!result)
			   result = theWindow->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
			if (!result)
			{
				if (ShootRayIntoScene(pie))
					UpdateActionButtons(-1,-1, pie->mouseX, pie->mouseY);
			}
			break;
		case PUMA_INPUT_LMBUP:
		   result = charInvWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			if (!result)
			   result = theWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			RemoveActionButtons(FALSE);
			theWindow = fullWindow;
			break;
		case PUMA_INPUT_RMBDOWN:
			theWindow = fullWindow;
			if (actionWindow)
				theWindow = actionWindow;
		   theWindow->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
			rmbDownPos  = (pie->mouseX - puma->ScreenW()/2) / (float)puma->ScreenW()/2;
			rmbDownPosY = (pie->mouseY - puma->ScreenH()/2) / (float)puma->ScreenH()/2;
			break;
		case PUMA_INPUT_RMBUP:
			theWindow = fullWindow;
			if (actionWindow)
				theWindow = actionWindow;
		   theWindow->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
			rmbDownPos = 0;
			rmbDownPosY = 0;
			break;
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

	theWindow = fullWindow;
	if (actionWindow)
		theWindow = actionWindow;

   charInvWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);
   theWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);
	if (rmbDownPos != 0)
	{
		rmbDownPos  = (puma->mouseX - puma->ScreenW()/2) / (float)puma->ScreenW()/2;
		rmbDownPosY = (puma->mouseY - puma->ScreenH()/2) / (float)puma->ScreenH()/2;
		if (0 == rmbDownPos)
			rmbDownPos = 0.001f;
	}
	else if (!actionWindow && !giveMoneyMode)
	{
		PumaInputEvent *p2 = new PumaInputEvent(PUMA_INPUT_LMBUP,"LMB UP");
		p2->vPickRayDir  = puma->vPickRayDir;
		p2->vPickRayOrig = puma->vPickRayOrig;

		ShootRayIntoScene(p2, TRUE);

		delete p2;
	}

   theWindow->Action(UIRECT_MOUSE_TEST_POS, puma->mouseX, puma->mouseY);

	return 0;
}


//***************************************************************
void GroundTestMode::HandleMessages(void)
{
	char messData[4000];//, tempText[1024];
	char tempText[1024];
	int  dataSize;
	FILE *source = NULL;
//	int correct, found, i,j;
//	BBOMob *lastAvatar;
//	long oldScreenHealth;

	MessPlayerChatLine chatMess;
	MessInfoText     *infoText;
	MessAdminMessage *adminMess;
	MessSecureTrade  *secureTradePtr;
	MessAdminInfo    *adminInfo;
	MessChatChannel  *chatChannelPtr;
	MessSecurePartnerName *partNamePtr;
//	MessAvatarHealth *messHealth;
//	MessMonsterHealth *messMH;

//	UIRectWindow *listWindow;
	UIRectTextBox *textBox;
	UIRectTextButton *tButt;
	UIRectArtTextButton *tArtButt;


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
				InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_TELL == messData[1])
			{
				textColor = textColorList[1];
				InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_SHOUT == messData[1])
			{
				textColor = textColorList[2];
				InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_DATA == messData[1])
			{
				textColor = textColorList[3];
				InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_GUILD == messData[1])
			{
				textColor = textColorList[4];
				InjectTextLine(&(messData[2]), textColor);
			}
			else if (TEXT_COLOR_EMOTE == messData[1])
			{
				textColor = textColorList[5];
				InjectTextLine(&(messData[2]), textColor);
			}
			else
				InjectTextLine(&(messData[1]), textColor);
		   break;

		case NWMESS_INVENTORY_INFO:
			HandleInvenInfo(messData, dataSize);
		   break;

		case NWMESS_EXTENDED_INFO:
			HandleExtendedInfo(messData, dataSize);
		   break;

		case NWMESS_INFO_TEXT:
			infoText = (MessInfoText *) messData;
			AddInfoText(infoText->text);
		   break;

		case NWMESS_ADMIN_MESSAGE:
			adminMess      = (MessAdminMessage *) messData;
			if (MESS_ADMIN_ACTIVATE == adminMess->messageType)
			{
				playerIsAdmin = TRUE;
			}
			if (MESS_ADMIN_RELEASE_CONTROL == adminMess->messageType)
			{
				bboClient->controlledMonsterID = -1;
			}
		   break;

		case NWMESS_SECURE_TRADE:
			secureTradePtr = (MessSecureTrade *) messData;
			switch (secureTradePtr->type)
			{
			case MESS_SECURE_STOP:
			default:
				if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
				{
					fullWindow->childRectList.Remove(secureYouWindow);
				}
				if (fullWindow->childRectList.Find(GTM_BUTTON_HER_SECURE_WIN))
				{
					fullWindow->childRectList.Remove(secureHerWindow);
				}
				break;

			case MESS_SECURE_NO_AGREEMENT:
				tButt = (UIRectTextButton *) 
					         fullWindow->childRectList.Find(GTM_BUTTON_SECURE_ACCEPT);
				if (tButt)
				{
					tButt->SetText("ACCEPT");
				}
				break;
			}

		   break;

		case NWMESS_ADMIN_INFO:
			textBox = (UIRectTextBox *) 
				         fullWindow->childRectList.Find(GTM_BUTTON_ADMIN_INFO);
			if (textBox)
			{
				adminInfo = (MessAdminInfo *) messData;

				sprintf(tempText,"Players: %d\nLast Connection: %d", 
					 adminInfo->numPlayers, adminInfo->lastConnectTime);
				textBox->SetText(tempText);
			}
			break;

		case NWMESS_SECURE_PARTNER_NAME:
			partNamePtr = (MessSecurePartnerName *) messData;

			sprintf(securePartnerName, partNamePtr->name);
			break;

		case NWMESS_CHAT_CHANNEL:
			chatChannelPtr = (MessChatChannel *) messData;

			tArtButt = (UIRectArtTextButton *) 
				          fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL1);
			if (tArtButt)
				tArtButt->isDepressed = chatChannelPtr->value & 1;

			tArtButt = (UIRectArtTextButton *) 
				          fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL2);
			if (tArtButt)
				tArtButt->isDepressed = chatChannelPtr->value & 2;

			tArtButt = (UIRectArtTextButton *) 
				          fullWindow->childRectList.Find(GTM_BUTTON_CHANNEL3);
			if (tArtButt)
				tArtButt->isDepressed = chatChannelPtr->value & 4;

			chatChannelValues = chatChannelPtr->value;
			break;

/*
		case NWMESS_MONSTER_HEALTH:
			messMH      = (MessMonsterHealth *) messData;
//			sprintf(tempText,"Monster");
//			monsterHealthBox->SetText(tempText);
			monsterHealthTimer = 600;
			monsterHealthLength = messMH->health * 120 / messMH->healthMax;
		   break;
*/
		default:
//			oldScreenHealth = screenHealth;
			bboClient->HandleMessage(messData, dataSize);
			if (bboClient->avatarNameChange)
			{
//				listWindow = (UIRectWindow *) fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN);
//				textBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_BOTTOMNAME);
//				textBox->SetText(bboClient->avatarNameChange);
				bboClient->avatarNameChange = NULL;
			}
			/*
			if (oldScreenHealth != screenHealth)
			{
				listWindow = (UIRectWindow *) fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN);
				textBox = (UIRectTextBox *) listWindow->childRectList.Find(GTM_BUTTON_STATSHEALTH);
				sprintf(tempText,"Health: %d/%d", screenHealth, screenMaxHealth);
				textBox->SetText(tempText);
			}
			*/

			break;

		}
		lclient->GetNextMsg(NULL, dataSize);
	}

}

//*******************************************************************************
void GroundTestMode::InjectTextLine(char *newText, DWORD color)
{
	if (localInfoFlags & LOCAL_FLAGS_FILTER_ON)
		CleanString(newText, 0);

	CD3DFont *font = puma->GetDXFont(0);

	int size = font->GetTextFitNum( newText, 436, TRUE );

	if (size == strlen(newText))
		InjectSingleTextLine(newText, color);
	else
	{
		char textBuff[1024];
		memcpy(textBuff, newText, size);
		textBuff[size] = 0;
		int fullSize = size;

		while  (size > 0)
		{
			InjectSingleTextLine(textBuff, color);

			size = font->GetTextFitNum( &(newText[fullSize]), 436, TRUE );

			if (size > 0)
			{
				memcpy(textBuff, &(newText[fullSize]), size);
				textBuff[size] = 0;
				fullSize += size;
			}


		}

	}


}


//*******************************************************************************
void GroundTestMode::InjectSingleTextLine(char *newText, DWORD color)
{

	UIRectTextBox *uiRect, *lastUIRect, *textBox;
	int numOfLines = 0;

	UIRectWindow *fullBox = (UIRectWindow *)chatWin;
	//fullWindow->childRectList.Find(GTM_BUTTON_CHATBOX);

	textBox = (UIRectTextBox *) 
		 fullBox->childRectList.Find((GTM_BUTTON_CHATLINE_0+1) + numOfLines);
	while (textBox)
	{
		++numOfLines;
		textBox = (UIRectTextBox *) 
			 fullBox->childRectList.Find((GTM_BUTTON_CHATLINE_0+1) + numOfLines);
	}

//	numOfLines = 0;
	for (int i = numOfLines-2; i >= 0; --i)
	{
		uiRect = (UIRectTextBox *) 
			 fullBox->childRectList.Find((GTM_BUTTON_CHATLINE_0+1) + i);
		lastUIRect = (UIRectTextBox *) 
			 fullBox->childRectList.Find((GTM_BUTTON_CHATLINE_0+1) + i + 1);

		lastUIRect->SetText(uiRect->text);
		lastUIRect->textColor = uiRect->textColor;
	}

	uiRect->SetText(newText);
	uiRect->textColor = color;
}


//*******************************************************************************
void GroundTestMode::HandleInvenInfo(char* rawData, int size)
{

	// fill out a MessInventoryInfo structure
	MessInventoryInfo infoData;
	Chronos::BStream *	stream		= NULL;
	stream	= new Chronos::BStream(sizeof(MessInventoryInfo));

	stream->write(rawData, size);
	stream->setPosition(0);

	int offSet = 0;
	int listSize = 9;

	unsigned char temp;
	*stream >> temp;
	assert( NWMESS_INVENTORY_INFO == temp);

	*stream >> temp;
	infoData.isPlayerInfo = temp;

	*stream >> temp;
	infoData.traderType = temp;

	if (infoData.isPlayerInfo && MESS_INVENTORY_PLAYER == infoData.traderType)
	{
		offSet = 2;
	}
	if (infoData.isPlayerInfo)
	{
		listSize = 11;
	}
	for (int j = offSet; j < listSize; ++j)
	{

		*stream >> infoData.type[j];
		if (-1 == infoData.type[j])
		{
			infoData.text[j][0] = 0; // blank means no object in this slot.
		}
		else
		{
			// get string
			for (int k = 0; k < 1000; ++k)
			{
				*stream >> infoData.text[j][k];
				if (0 == infoData.text[j][k])
					k = 1000;
			}

			*stream >> (long) infoData.ptr[j];
			*stream >> (long) infoData.amount[j];
			*stream >> (long) infoData.value[j];

			long tempLong;
			int tempInt;
			unsigned long tempULong;

			switch(infoData.type[j])
			{
			case INVOBJ_BLADE:
				*stream >> (long) tempLong;
				infoData.f1[j] = (float)tempLong;
				*stream >> (long) tempLong;
				infoData.f2[j] = (float)tempLong;
				break;

			case INVOBJ_POTION:
				*stream >> tempInt;
				infoData.f1[j] = (float)tempInt;
				*stream >> tempInt;
				infoData.f2[j] = (float)tempInt;
				break;

			case INVOBJ_SKILL:
				*stream >> tempULong;
				infoData.f1[j] = (float)tempULong;
				*stream >> tempULong;
				infoData.f2[j] = (float)tempULong;
				break;

			case INVOBJ_TOTEM:
				*stream >> infoData.f1[j];
				*stream >> (long) tempLong;
				infoData.f2[j] = (float)tempLong;
				break;

			case INVOBJ_STAFF:
				*stream >> infoData.f1[j];
				*stream >> (int) tempInt;
				infoData.f2[j] = (float)tempInt;
				break;
			}
		}
	}

	*stream >> infoData.invPtr;
	*stream >> infoData.money;
	*stream >> infoData.offset;

	delete stream;

	// point to it!
	MessInventoryInfo * info = &infoData;


	char tempText[1024];
//	UIRectWindow *listWindow = (UIRectWindow *) fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN);
	UIRectTextBox *textBox;
	UIRectTextButton *tButt;

	if (info->isPlayerInfo)
	{
		playerInvIndex = info->offset;
		money = info->money;
//		moneyDelta = info->moneyDelta;

		if (GTM_BUTTON_LIST_INV == playerListMode)
		{
			tButt = (UIRectTextButton *) listWindow->childRectList.Find(GTM_BUTTON_STATSLINE0 + 0);
			tButt->SetText("");
			tButt = (UIRectTextButton *) listWindow->childRectList.Find(GTM_BUTTON_STATSLINE0 + 1);
			tButt->SetText("");
//			offSet = 2;

			textBox = (UIRectTextBox *) listWindow->childRectList.Find(GTM_BUTTON_STATSINFO);
			sprintf(tempText,"%ld", money);
			textBox->SetText(tempText);
			textBox->textColor = D3DCOLOR_ARGB(255, 255, 240, 190);
			listWindow->fillArt = uiPlayerListArt[0];
		}
		else if (GTM_BUTTON_LIST_WRK == playerListMode)
		{
			listWindow->fillArt = uiPlayerListArt[2];
			textBox = (UIRectTextBox *) listWindow->childRectList.Find(GTM_BUTTON_STATSINFO);
			textBox->SetText("");
		}
		else if (GTM_BUTTON_LIST_SKL == playerListMode)
		{
			listWindow->fillArt = uiPlayerListArt[3];
			textBox = (UIRectTextBox *) listWindow->childRectList.Find(GTM_BUTTON_STATSINFO);
			textBox->SetText("");
		}
		else
		{
			listWindow->fillArt = uiPlayerListArt[1];
			textBox = (UIRectTextBox *) listWindow->childRectList.Find(GTM_BUTTON_STATSINFO);
			textBox->SetText("");
		}

		for (int i = offSet; i < listSize; ++i)
		{
			tButt = (UIRectTextButton *) listWindow->childRectList.Find(GTM_BUTTON_STATSLINE0 + i);
			if (0 == info->text[i][0])
				tempText[0] = 0;
			else if (info->amount[i] > 1)
				sprintf(tempText,"%s (%ld)", info->text[i], info->amount[i]);
			else
				sprintf(tempText,"%s", info->text[i]);
			tButt->SetText(tempText);
//			if (INVSTATUS_TRADED_AWAY == info->status[i])
//				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 70, 70, 70);
//			else if (INVSTATUS_NOT_MINE_YET == info->status[i])
//				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 50, 255, 50);
//			else
			tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 255);
			if (INVOBJ_SIMPLE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 255, 150);
			if (INVOBJ_BLADE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 150);
			if (INVOBJ_STAFF == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 150, 255);
			if (INVOBJ_TOTEM == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 255);
			if (INVOBJ_EGG == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 235, 150);
			if (INVOBJ_MEAT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 210, 115, 0);

			if (INVOBJ_POTION == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 150);
			if (INVOBJ_INGOT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 255, 255);
			if (INVOBJ_INGREDIENT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 115, 200);
			if (INVOBJ_EXPLOSIVE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 100, 255, 100);
			if (INVOBJ_FUSE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,  50, 255, 100);
			if (INVOBJ_BOMB == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,   0, 255, 100);
			if (INVOBJ_FAVOR == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,  50, 100, 255);
			if (INVOBJ_GEOPART == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 150, 55);
			if (INVOBJ_EARTHKEY == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 55);


			playerInv[i].type   = info->type[i];
			playerInv[i].ptr    = info->ptr[i];
//			playerInv[i].status = info->status[i];

			if (INVOBJ_BLADE == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else if (info->amount[i] > 1)
					sprintf(tempText,"%s (%d,%d)(%ld)", info->text[i], (int)info->f1[i],
					        (int)info->f2[i], info->amount[i]);
				else
					sprintf(tempText,"%s (%d,%d)", info->text[i], (int)info->f1[i], (int)info->f2[i]);
				tButt->SetText(tempText);
			}
			if (INVOBJ_SKILL == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else
					sprintf(tempText,"%s %d (%ld)", info->text[i], (int) info->f1[i], (long)info->f2[i]);
				tButt->SetText(tempText);
			}
			if (INVOBJ_POTION == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else
					sprintf(tempText,"%s (%d)", info->text[i], info->amount[i]);
				tButt->SetText(tempText);
			}
			if (INVOBJ_TOTEM == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else
				{
					float timeLeft = (info->f2[i]/60*-1);
					if (timeLeft < 1.0f)
						sprintf(tempText,"%s (%d,%dm)", info->text[i], -1 * (int)info->f1[i],
						        (int)(timeLeft * 60.0f));
					else
						sprintf(tempText,"%s (%d,%dh)", info->text[i], -1 * (int)info->f1[i], 
						        (int)timeLeft);
					if (info->amount[i] > 1)
						sprintf(&(tempText[strlen(tempText)])," %d", info->amount[i]);  
				}
				tButt->SetText(tempText);
			}
			if (INVOBJ_STAFF == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else
				{
					if (- 1000 == info->f2[i])
						sprintf(tempText,"%s (%d)", info->text[i], -1 * (int)info->f1[i]);
					else
						sprintf(tempText,"%s (%d,%dc)", info->text[i], -1 * (int)info->f1[i], 
						        (int)info->f2[i]);
					if (info->amount[i] > 1)
						sprintf(&(tempText[strlen(tempText)])," %d", info->amount[i]);  
				}
				tButt->SetText(tempText);
			}
		}
	}
	else
	{
		UIRectWindow *tempListWindow;
		int buttonBase = -30;

		InventoryRef * tempIRef = otherInv;

		if (MESS_INVENTORY_TRADER  == infoData.traderType ||
		    MESS_INVENTORY_TRAINER == infoData.traderType ||
		    MESS_INVENTORY_TOWER   == infoData.traderType ||
		    MESS_INVENTORY_GROUND  == infoData.traderType)
		{
			RemoveNonInvWindows();

			if (!fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
			{
				fullWindow->childRectList.Append(otherWindow);
				fullWindow->Arrange();
			}

			partnerPtr = info->invPtr;
			textBox = (UIRectTextBox *) otherWindow->childRectList.Find(GTM_BUTTON_OTHERNAME);
			switch(info->traderType)
			{
			case MESS_INVENTORY_TRADER:
				textBox->SetText("Trader");
				break;
			case MESS_INVENTORY_TRAINER:
				textBox->SetText("Trainer");
				break;
			case MESS_INVENTORY_TOWER:
				textBox->SetText("Tower Chest");
				break;
			case MESS_INVENTORY_GROUND:
				textBox->SetText("Ground");
				break;
			case MESS_INVENTORY_BANK:
				textBox->SetText("Account Storage");
				break;
			default:
				textBox->SetText("");
				break;
			}

			otherInvIndex = info->offset;
			tempListWindow = otherWindow;
			buttonBase = GTM_BUTTON_OTHERLINE0;
		}
		else if (MESS_INVENTORY_BANK  == infoData.traderType)
		{
			RemoveNonInvWindows();

			if (!fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
			{
				fullWindow->childRectList.Append(bankWindow);
				fullWindow->Arrange();
			}

			partnerPtr = info->invPtr;
		   UIRectArtTextButton *tArtButt;
			tArtButt = (UIRectArtTextButton *) bankWindow->childRectList.Find(GTM_BUTTON_BANKNAME);
			sprintf(tempText,"Bank: %ld", info->money);
			tArtButt->SetText(tempText);

			otherInvIndex = info->offset;
			tempListWindow = bankWindow;
			buttonBase = GTM_BUTTON_OTHERLINE0;
		}
		else if (MESS_INVENTORY_HER_SECURE == infoData.traderType)
		{
//			RemoveNonInvWindows();  EDED

			if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
				fullWindow->childRectList.Remove(otherWindow);
			if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				fullWindow->childRectList.Remove(bankWindow);

			if (!fullWindow->childRectList.Find(GTM_BUTTON_HER_SECURE_WIN))
			{
				fullWindow->childRectList.Append(secureHerWindow);
				fullWindow->Arrange();
			}

			secureHerInvIndex = info->offset;
			tempListWindow = secureHerWindow;
			buttonBase = GTM_BUTTON_HER_SECURE_LINE0;
			tempIRef = herInv;

			textBox = (UIRectTextBox *) tempListWindow->childRectList.Find(GTM_BUTTON_HER_SECURE_NAME);
			sprintf(tempText,"%s: %ld", securePartnerName, info->money);
			textBox->SetText(tempText);
			textBox->textColor = D3DCOLOR_ARGB(255, 255, 240, 190);
		}
		else if (MESS_INVENTORY_YOUR_SECURE == infoData.traderType)
		{
//			RemoveNonInvWindows();

			if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
				fullWindow->childRectList.Remove(otherWindow);
			if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				fullWindow->childRectList.Remove(bankWindow);

			if (!fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
			{
				fullWindow->childRectList.Append(secureYouWindow);
				fullWindow->Arrange();
			}
			secureYouInvIndex = info->offset;
			tempListWindow = secureYouWindow;
			buttonBase = GTM_BUTTON_YOU_SECURE_LINE0;
			tempIRef = yourInv;

		   UIRectArtTextButton *tArtButt;
			tArtButt = (UIRectArtTextButton *) tempListWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_NAME);
			sprintf(tempText,"You: %ld", info->money);
			tArtButt->SetText(tempText);
//			tArtButt->textColor = D3DCOLOR_ARGB(255, 255, 240, 190);
		}
		else
			return;

		// set name of trader in GTM_BUTTON_OTHERNAME
//		info->
		for (int i = 0; i < 9; ++i)
		{
			tButt = (UIRectTextButton *) tempListWindow->childRectList.Find(buttonBase + i);
			if (0 == info->text[i][0])
				tempText[0] = 0;
			else if (info->amount[i] > 1)
				sprintf(tempText,"%s %ldg (%ld)", info->text[i], (long) info->value[i], info->amount[i]);
			else
				sprintf(tempText,"%s %ldg", info->text[i], (long) info->value[i]);
			tButt->SetText(tempText);
//			if (INVSTATUS_TRADED_AWAY == info->status[i])
//				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 70, 70, 70);
//			else if (INVSTATUS_NOT_MINE_YET == info->status[i])
//				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 50, 255, 50);
//			else
			tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 255);
			if (INVOBJ_SIMPLE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 255, 150);
			if (INVOBJ_BLADE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 150);
			if (INVOBJ_STAFF == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 150, 255);
			if (INVOBJ_TOTEM == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 255);
			if (INVOBJ_EGG == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 235, 150);
			if (INVOBJ_MEAT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 210, 115, 0);

			if (INVOBJ_POTION == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 255, 150);
			if (INVOBJ_INGOT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 255, 255);
			if (INVOBJ_INGREDIENT == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 115, 200);
			if (INVOBJ_EXPLOSIVE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 100, 255, 100);
			if (INVOBJ_FUSE == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,  50, 255, 100);
			if (INVOBJ_BOMB == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,   0, 255, 100);
			if (INVOBJ_FAVOR == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255,  50, 100, 255);
			if (INVOBJ_GEOPART == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 150, 150, 55);
			if (INVOBJ_EARTHKEY == info->type[i])
				tButt->color[UIRECT_BCOL_NORMAL] = D3DCOLOR_ARGB(255, 255, 150, 55);

			tempIRef[i].type   = info->type[i];
			tempIRef[i].ptr    = info->ptr[i];
//			otherInv[i].status = info->status[i];

			if (INVOBJ_BLADE == info->type[i])
			{
				if (0 == info->text[i][0])
					tempText[0] = 0;
				else if (info->amount[i] > 1)
					sprintf(tempText,"%s %ldg (%d,%d)(%ld)", info->text[i], (long) info->value[i], (int)info->f1[i],
					        (int)info->f2[i], info->amount[i]);
				else
					sprintf(tempText,"%s %ldg (%d,%d)", info->text[i], (long) info->value[i], (int)info->f1[i], (int)info->f2[i]);
				tButt->SetText(tempText);
			}
		}
	}
}

	
//*******************************************************************************
int GroundTestMode::ShootRayIntoScene(PumaInputEvent *pie, int justTest)
{
	DoublyLinkedList *list = bboClient->monsterList;

	BBOMob *curMob = (BBOMob *) list->First();
	BBOMob *candidate = NULL;
   FLOAT dist1, dist2, dist3, dist4, lastDist = 1000.0f;

	int centerCellX, centerCellY;
	centerCellX = bboClient->playerAvatar->cellX;
	centerCellY = bboClient->playerAvatar->cellY;

	if (bboClient->controlledMonsterID != -1)
	{
		BBOMob *curMob = (BBOMob *) bboClient->monsterList->First();
		while (curMob)
		{
			if (curMob->mobID == bboClient->controlledMonsterID)
			{
				centerCellX = curMob->cellX;
				centerCellY = curMob->cellY;
				curMob = (BBOMob *) bboClient->monsterList->Last();
			}
			curMob = (BBOMob *) bboClient->monsterList->Next();
		}
	}

	// for each mob
	while (curMob)
	{
		// if the mob is in the same square as the player's mob
		if (centerCellX == curMob->cellX &&
		    centerCellY == curMob->cellY)
		{
			float sizeX = 0.2f;
			float sizeY = 3;

			if (SMOB_TREE == curMob->type)
			{
				sizeX = 0.3f;
				sizeY = 5;
			}

			// make two quads superimposed on the mob's position
			D3DXVECTOR3 v1,v2,v3,v4;
			v1 = curMob->spacePoint.location;
			v1.y += sizeY;
			v1.x += sizeX;
			v2 = curMob->spacePoint.location;
			v2.y += 0;
			v2.x += sizeX;
			v3 = curMob->spacePoint.location;
			v3.y += sizeY;
			v3.x -= sizeX;
			v4 = curMob->spacePoint.location;
			v4.y += 0;
			v4.x -= sizeX;

			BOOL result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v1,v2,v3, &dist1);
			BOOL result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v2,v3,v4, &dist2);

			v1 = curMob->spacePoint.location;
			v1.y += sizeY;
			v1.z += sizeX;
			v2 = curMob->spacePoint.location;
			v2.y += 0;
			v2.z += sizeX;
			v3 = curMob->spacePoint.location;
			v3.y += sizeY;
			v3.z -= sizeX;
			v4 = curMob->spacePoint.location;
			v4.y += 0;
			v4.z -= sizeX;

			BOOL result3 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v1,v2,v3, &dist3);
			BOOL result4 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v2,v3,v4, &dist4);

			// if the mouse ray intersects
			if (result1 || result2 || result3 || result4)
			{
				float d = dist1;
				if (result2)
					d = dist2;
				if (result3)
					d = dist3;
				if (result4)
					d = dist4;

				// if the distance is less than the current distance
				if (d < lastDist)
				{
					// this mob is the new candidata
					candidate = curMob;
					lastDist = d;
				}
			}

			v1 = curMob->spacePoint.location;
			v1.y += sizeY;
			v1.x += sizeX;
			v2 = curMob->spacePoint.location;
			v2.y += 0;
			v2.x += sizeX;
			v3 = curMob->spacePoint.location;
			v3.y += sizeY;
			v3.x -= sizeX;
			v4 = curMob->spacePoint.location;
			v4.y += 0;
			v4.x -= sizeX;

			result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v1,v3,v2, &dist1);
			result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v2,v4,v3, &dist2);

			v1 = curMob->spacePoint.location;
			v1.y += sizeY;
			v1.z += sizeX;
			v2 = curMob->spacePoint.location;
			v2.y += 0;
			v2.z += sizeX;
			v3 = curMob->spacePoint.location;
			v3.y += sizeY;
			v3.z -= sizeX;
			v4 = curMob->spacePoint.location;
			v4.y += 0;
			v4.z -= sizeX;

			result3 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v1,v3,v2, &dist3);
			result4 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, v2,v4,v3, &dist4);

			// if the mouse ray intersects
			if (result1 || result2 || result3 || result4)
			{
				float d = dist1;
				if (result2)
					d = dist2;
				if (result3)
					d = dist3;
				if (result4)
					d = dist4;

				// if the distance is less than the current distance
				if (d < lastDist)
				{
					// this mob is the new candidata
					candidate = curMob;
					lastDist = d;
				}
			}
		}

		curMob = (BBOMob *) list->Next();
	}
		
	// if there's a valid candidate
	if (candidate)
	{
		// make it the highlighted one.
		bboClient->selectedMOB = candidate;
		return TRUE;
	}
	else
	{
		// clicked on the ground?

		// clicked on current tile?
		BOOL result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, 
			            bboClient->clickVerts[(0*2+0)*3+0],
							bboClient->clickVerts[(0*2+0)*3+2],
							bboClient->clickVerts[(0*2+0)*3+1], &dist1);
		BOOL result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir,
			            bboClient->clickVerts[(0*2+1)*3+0],
							bboClient->clickVerts[(0*2+1)*3+2],
							bboClient->clickVerts[(0*2+1)*3+1], &dist1);

		if (result1 | result2)
		{
			bboClient->selectedMOB = NULL;
			return TRUE;
		}

		// clicked on north tile?
		result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, 
			            bboClient->clickVerts[(1*2+0)*3+0],
							bboClient->clickVerts[(1*2+0)*3+2],
							bboClient->clickVerts[(1*2+0)*3+1], &dist1);
		result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir,
			            bboClient->clickVerts[(1*2+1)*3+0],
							bboClient->clickVerts[(1*2+1)*3+2],
							bboClient->clickVerts[(1*2+1)*3+1], &dist1);

		if (result1 | result2)
		{
			if (moveLockoutTimer <= 0)
			{
				if ((SPACE_DUNGEON == curMapType))
					bboClient->lSquareMan->AddDungeonSquare(
					        centerCellX,
					        centerCellY-1);
				else if ((SPACE_REALM == curMapType))
					bboClient->lSquareMan->AddRealmSquare(
					        centerCellX,
					        centerCellY-1);
				else
					bboClient->lSquareMan->AddSquare(
					        centerCellX,
					        centerCellY-1);
			}

			// go north!
			if (moveLockoutTimer <= 0 && !justTest && 
				 (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON))
			{
				if (moveRequestTimer + 1000 < timeGetTime())
				{
					moveRequestTimer= timeGetTime();

					MessAvatarMoveRequest bMoveReq;
					bMoveReq.avatarID = bboClient->playerAvatarID;
					bMoveReq.x = 0;
					lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
					RemoveNonInvWindows();
					RemoveActionButtons();
				}
			}
			bboClient->selectedMOB = NULL;
			return FALSE;
		}

		// clicked on east tile?
		result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, 
			            bboClient->clickVerts[(2*2+0)*3+0],
							bboClient->clickVerts[(2*2+0)*3+2],
							bboClient->clickVerts[(2*2+0)*3+1], &dist1);
		result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir,
			            bboClient->clickVerts[(2*2+1)*3+0],
							bboClient->clickVerts[(2*2+1)*3+2],
							bboClient->clickVerts[(2*2+1)*3+1], &dist1);

		if (result1 | result2)
		{
			// go east!
			if (moveLockoutTimer <= 0)
			{
				if ((SPACE_DUNGEON == curMapType))
					bboClient->lSquareMan->AddDungeonSquare(
					        centerCellX-1,
					        centerCellY);
				else if ((SPACE_REALM == curMapType))
					bboClient->lSquareMan->AddRealmSquare(
					        centerCellX-1,
					        centerCellY);
				else
					bboClient->lSquareMan->AddSquare(
					        centerCellX-1,
					        centerCellY);
			}

			if (moveLockoutTimer <= 0 && !justTest && 
				 (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON))
			{
				if (moveRequestTimer + 1000 < timeGetTime())
				{
					moveRequestTimer= timeGetTime();

					MessAvatarMoveRequest bMoveReq;
					bMoveReq.avatarID = bboClient->playerAvatarID;
					bMoveReq.x = 6;
					lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
					RemoveNonInvWindows();
					RemoveActionButtons();
				}
			}
			bboClient->selectedMOB = NULL;
			return FALSE;
		}

		// clicked on south tile?
		result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, 
			            bboClient->clickVerts[(3*2+0)*3+0],
							bboClient->clickVerts[(3*2+0)*3+2],
							bboClient->clickVerts[(3*2+0)*3+1], &dist1);
		result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir,
			            bboClient->clickVerts[(3*2+1)*3+0],
							bboClient->clickVerts[(3*2+1)*3+2],
							bboClient->clickVerts[(3*2+1)*3+1], &dist1);

		if (result1 | result2)
		{
			// go south!
			if (moveLockoutTimer <= 0)
			{
				if ((SPACE_DUNGEON == curMapType))
					bboClient->lSquareMan->AddDungeonSquare(
					        centerCellX,
					        centerCellY+1);
				else if ((SPACE_REALM == curMapType))
					bboClient->lSquareMan->AddRealmSquare(
					        centerCellX,
					        centerCellY+1);
				else
					bboClient->lSquareMan->AddSquare(
					        centerCellX,
					        centerCellY+1);
			}

			if (moveLockoutTimer <= 0 && !justTest && 
				 (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON))
			{
				if (moveRequestTimer + 1000 < timeGetTime())
				{
					moveRequestTimer= timeGetTime();

					MessAvatarMoveRequest bMoveReq;
					bMoveReq.avatarID = bboClient->playerAvatarID;
					bMoveReq.x = 4;
					lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
					RemoveNonInvWindows();
					RemoveActionButtons();
				}
			}
			bboClient->selectedMOB = NULL;
			return FALSE;
		}

		// clicked on west tile?
		result1 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir, 
			            bboClient->clickVerts[(4*2+0)*3+0],
							bboClient->clickVerts[(4*2+0)*3+2],
							bboClient->clickVerts[(4*2+0)*3+1], &dist1);
		result2 = IntersectTriangle(pie->vPickRayOrig, pie->vPickRayDir,
			            bboClient->clickVerts[(4*2+1)*3+0],
							bboClient->clickVerts[(4*2+1)*3+2],
							bboClient->clickVerts[(4*2+1)*3+1], &dist1);

		if (result1 | result2)
		{
			// go west!
			if (moveLockoutTimer <= 0)
			{
				if ((SPACE_DUNGEON == curMapType))
					bboClient->lSquareMan->AddDungeonSquare(
					        centerCellX+1,
					        centerCellY);
				else if ((SPACE_REALM == curMapType))
					bboClient->lSquareMan->AddRealmSquare(
					        centerCellX+1,
					        centerCellY);
				else
					bboClient->lSquareMan->AddSquare(
					        centerCellX+1,
					        centerCellY);
			}

			if (moveLockoutTimer <= 0 && !justTest && 
				 (localInfoFlags & LOCAL_FLAGS_CLICKMOVE_ON))
			{
				if (moveRequestTimer + 1000 < timeGetTime())
				{
					moveRequestTimer= timeGetTime();

					MessAvatarMoveRequest bMoveReq;
					bMoveReq.avatarID = bboClient->playerAvatarID;
					bMoveReq.x = 2;
					lclient->SendMsg(sizeof(bMoveReq),&bMoveReq);
					RemoveNonInvWindows();
					RemoveActionButtons();
				}
			}
			bboClient->selectedMOB = NULL;
			return FALSE;
		}

	}

	return FALSE;

}

//*******************************************************************************
void GroundTestMode::RemoveActionButtons(int eraseGiveMoneyDialog)
{
//	UIRect *uiRect;

	if (actionWindow)
	{
		delete actionWindow;
		actionWindow = NULL;
	}

	UIRectEditLine *tEdit;
	UIRectTextBox *tBox;

	if (eraseGiveMoneyDialog)
	{
		tBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYTITLE);
		if (tBox)
		{
			fullWindow->childRectList.Remove(tBox);
			delete tBox;

			tEdit = (UIRectEditLine *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
			if (tEdit)
			{
				fullWindow->childRectList.Remove(tEdit);
				delete tEdit;
			}
		}

		giveMoneyMode = FALSE;
		namePetMode = FALSE;
	}
								  /*
	// remove all previous action buttons
	for (int i = GTM_BUTTON_DONAME; i <= GTM_BUTTON_DO9; ++i)
	{
		uiRect = (UIRect*) fullWindow->childRectList.Find(i);
		if (uiRect)
		{
			fullWindow->childRectList.Remove(uiRect);
			delete uiRect;
		}
	}
	*/
}

//*******************************************************************************
void GroundTestMode::UpdateActionButtons(int buttonIndex, int isPlayer, 
													  int mx, int my, int type)
{
	UIRectTextButton *tButt; 
//	UIRectEditLine *tEdit;
//	UIRectTextBox *tBox;
//	UIRect *uiRect;

	int curY = my + 10;
	int centerX = puma->ScreenW()/2;
	int centerY = puma->ScreenH()/2;

	if (mx < 50)
		mx = 50;
	if (mx > puma->ScreenW() - 100)
		mx = puma->ScreenW() - 100;

	StartAddingMenuButtons(mx, curY);

	int oldAlpha = uiAlpha;
	uiAlpha = 255;

	if (buttonIndex < 0)
	{
//		if (lastSelectedMOB != bboClient->selectedMOB)
		{
			// remove all previous action buttons
			RemoveActionButtons();

//			RemoveNonInvWindows();

		   actionWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
		   actionWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
		   actionWindow->isActive  = TRUE;

			AddMenuTop();
			// add new action buttons
			if (bboClient->selectedMOB)
			{
				if (bboClient->selectedMOB != bboClient->playerAvatar)
				{
					if (bboClient->selectedMOB->avatarID != -1)
					{
						// interact with another player
						//AddMenuButton("Give Money", ACTION_TYPE_TRADE);
						AddMenuButton("Secure Trade", ACTION_TYPE_SECURE_TRADE);
						if (bboClient->controlledMonsterID != -1)
						{
							AddMenuButton("Attack", ACTION_TYPE_ATTACK);
						}
					}
					else if (bboClient->selectedMOB->type == SMOB_MONSTER)
					{
						// interact with a monster
						AddMenuButton("Attack", ACTION_TYPE_ATTACK);

						if (playerIsAdmin)
						{
							AddMenuButton("Control", ACTION_TYPE_CONTROL);
							AddMenuButton("Release", ACTION_TYPE_RELEASE);
						}
					}
					else if (bboClient->selectedMOB->type == SMOB_CHEST)
					{
						if (0 == bboClient->selectedMOB->chestType)
						{
							// interact with a chest
							AddMenuButton("Open", ACTION_TYPE_OPEN);
						}
					}
					else if (bboClient->selectedMOB->type == SMOB_TRAINER)
					{
						// interact with a trainer
						AddMenuButton("Learn", ACTION_TYPE_TRADE);
					}
					else if (bboClient->selectedMOB->type == SMOB_TREE)
					{
						// interact with a tree
						AddMenuButton("Listen", ACTION_TYPE_LISTEN);
						AddMenuButton("Ask For Quest", ACTION_TYPE_QUEST);
					}
					else if (bboClient->selectedMOB->type == SMOB_WITCH)
					{
						// interact with a witch
						AddMenuButton("Listen", ACTION_TYPE_LISTEN);
						AddMenuButton("Ask For Quest", ACTION_TYPE_QUEST);
					}
					else if (bboClient->selectedMOB->type == SMOB_TOWNMAGE)
					{
						// interact with a town mage
						AddMenuButton("Listen", ACTION_TYPE_LISTEN, uiPopUpLongArt,-70,132,2);
						AddMenuButton("Heal (180g)", ACTION_TYPE_HEAL, 
							           uiPopUpLongArt,-70,132,2);
						AddMenuButton("Teleport Forward (400g)", ACTION_TYPE_TELEPORT, 
							           uiPopUpLongArt,-70,132,2);
						AddMenuButton("Teleport Back (400g)", ACTION_TYPE_TELEPORT_BACK, 
							           uiPopUpLongArt,-70,132,2);
						AddMenuButton("Use Account Storage", ACTION_TYPE_BANK, 
							           uiPopUpLongArt,-70,132,2);
					}
					else if (bboClient->selectedMOB->type != SMOB_TOWER && 
					         bboClient->selectedMOB->type != SMOB_WARP_POINT && 
						      bboClient->selectedMOB->type != SMOB_TOKEN)
					{
						// interact with a trader
						AddMenuButton("Trade", ACTION_TYPE_TRADE);
						AddMenuSpacer();
						AddMenuButton("Sell All Simple Loot", ACTION_TYPE_SELLALLGREEN,
							           NULL, -50,103,2);
						AddMenuButton("Sell All Meat", ACTION_TYPE_SELLALLMEAT,
							           NULL, -50,103,2);
						AddMenuButton("Sell All Bombs", ACTION_TYPE_SELLALLBOMBS,
							           NULL, -50,103,2);
						AddMenuButton("Sell All EarthKeys", ACTION_TYPE_SELLALLKEYS,
							           NULL, -50,103,2);
					}
				}
			}
			else
			{
				// interact with ground
				AddMenuButton("Check Ground", ACTION_TYPE_TRADE);
			}

			lastSelectedMOB = bboClient->selectedMOB;
			AddMenuBottom();
		}
	}
	else
	{
		// remove all previous action buttons
		RemoveActionButtons();

	   actionWindow = new UIRectWindow(-1, 0,0,puma->ScreenW(),puma->ScreenH());
	   actionWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	   actionWindow->isActive  = TRUE;

//		UIRectWindow *listWindow = (UIRectWindow *) actionWindow->childRectList.Find(GTM_BUTTON_STATSWIN);

		AddMenuTop();
		if (isPlayer)
		{
			// interact with selected item in player's inventory
			tButt = (UIRectTextButton *) 
				listWindow->childRectList.Find(GTM_BUTTON_STATSLINE0 + buttonIndex);

			int bIndex2 = 0;

//			selectedIndex = buttonIndex + playerInvIndex; // last item button index clicked on
			selectedIndex = buttonIndex; // last item button index clicked on

			if (GTM_BUTTON_LIST_INV == playerListMode)
			{
				AddMenuButton("Include", ACTION_TYPE_INCLUDE);
				AddMenuButton("Include 5", ACTION_TYPE_INCLUDE5);
				AddMenuButton("Include 20", ACTION_TYPE_INCLUDE20);
				AddMenuButton("Include All", ACTION_TYPE_INCLUDEALL);
				AddMenuSpacer();
				AddMenuButton("Use", ACTION_TYPE_WIELD);
			}
			else if (GTM_BUTTON_LIST_WRK == playerListMode)
			{
				AddMenuButton("Exclude", ACTION_TYPE_EXCLUDE);
				AddMenuButton("Exclude 5", ACTION_TYPE_EXCLUDE5);
				AddMenuButton("Exclude 20", ACTION_TYPE_EXCLUDE20);
				AddMenuButton("Exclude All", ACTION_TYPE_EXCLUDEALL);
				AddMenuBottom();
				return; // items in workbench can only be excluded.
			}
			else if (GTM_BUTTON_LIST_SKL == playerListMode)
			{
				AddMenuButton("Combine", ACTION_TYPE_COMBINE);
				AddMenuBottom();
				return; // items in skill list can only be used for combining.
			}
			else if (GTM_BUTTON_LIST_WLD == playerListMode)
			{
				AddMenuButton("Stop Using", ACTION_TYPE_UNWIELD);
				AddMenuBottom();
				return; // items in wield list can only be un-wielded
			}

			switch(playerInv[buttonIndex].type)
			{

			case INVOBJ_SIMPLE:
			case INVOBJ_INGOT:
			case INVOBJ_FAVOR:
			case INVOBJ_INGREDIENT:
			case INVOBJ_TOTEM:
			case INVOBJ_EGG:
			case INVOBJ_MEAT:
			case INVOBJ_EXPLOSIVE:
			case INVOBJ_FUSE:
			case INVOBJ_POTION:
			case INVOBJ_BOMB:
			case INVOBJ_STAFF:
			case INVOBJ_GEOPART:
			case INVOBJ_EARTHKEY:
				if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
				{
					AddMenuSpacer();
					AddMenuButton("Trade", ACTION_TYPE_GIVE);
					AddMenuButton("Trade 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Trade 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Trade All", ACTION_TYPE_GIVEALL);
				}
				else if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
				{
					AddMenuSpacer();
					AddMenuButton("Sell", ACTION_TYPE_GIVE);
					AddMenuButton("Sell 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Sell 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Sell All", ACTION_TYPE_GIVEALL);
				}
				else if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				{
					AddMenuSpacer();
					AddMenuButton("Stash", ACTION_TYPE_GIVE);
					AddMenuButton("Stash 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Stash 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Stash All", ACTION_TYPE_GIVEALL);
				}

				if (INVOBJ_MEAT == playerInv[buttonIndex].type)
				{
					char tempText[1024];

					if (bboClient->playerAvatar->pet[0].petDragonAnims[0] ||
					    bboClient->playerAvatar->pet[1].petDragonAnims[0])
						AddMenuSpacer();

					if (bboClient->playerAvatar->pet[0].petDragonAnims[0])
					{
						sprintf(tempText, " Feed %s", bboClient->playerAvatar->pet[0].petName);
						AddMenuButton(tempText, ACTION_TYPE_FEED0);
					}
					if (bboClient->playerAvatar->pet[1].petDragonAnims[0])
					{
						sprintf(tempText, " Feed %s", bboClient->playerAvatar->pet[1].petName);
						AddMenuButton(tempText, ACTION_TYPE_FEED1);
					}
				}

				if (INVOBJ_BOMB == playerInv[buttonIndex].type)
				{
					char tempText[1024];
					AddMenuSpacer();
					sprintf(tempText, "Set");
					AddMenuButton(tempText, ACTION_TYPE_SET_BOMB);
				}

				if (INVOBJ_POTION == playerInv[buttonIndex].type)
				{
					char tempText[1024];
					AddMenuSpacer();
					sprintf(tempText, "Activate");
					AddMenuButton(tempText, ACTION_TYPE_ACTIVATE);
				}

				if (INVOBJ_EARTHKEY == playerInv[buttonIndex].type)
				{
					char tempText[1024];
					AddMenuSpacer();
					sprintf(tempText, "Activate");
					AddMenuButton(tempText, ACTION_TYPE_ACTIVATE);
				}

				break;

			case INVOBJ_BLADE:
				if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
				{
					AddMenuSpacer();
					AddMenuButton("Trade", ACTION_TYPE_GIVE);
					AddMenuButton("Trade 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Trade 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Trade All", ACTION_TYPE_GIVEALL);
				}
				else if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
				{
					AddMenuSpacer();
					AddMenuButton("Sell", ACTION_TYPE_GIVE);
					AddMenuButton("Sell 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Sell 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Sell All", ACTION_TYPE_GIVEALL);
				}
				else if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
				{
					AddMenuSpacer();
					AddMenuButton("Stash", ACTION_TYPE_GIVE);
					AddMenuButton("Stash 5", ACTION_TYPE_GIVE5);
					AddMenuButton("Stash 20", ACTION_TYPE_GIVE20);
					AddMenuButton("Stash All", ACTION_TYPE_GIVEALL);
				}
				break;
			}

		}
		else
		{
			if (GTM_BUTTON_YOU_SECURE_LINE0 == type)
			{
				// interact with selected item in your trading window
				tButt = (UIRectTextButton *) 
					otherWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_LINE0 + buttonIndex);

				AddMenuButton("Retract", ACTION_TYPE_RETRACT);
				AddMenuButton("Retract 5", ACTION_TYPE_RETRACT5);
				AddMenuButton("Retract 20", ACTION_TYPE_RETRACT20);
				AddMenuButton("Retract All", ACTION_TYPE_RETRACTALL);
			}
			else
			{
				// interact with selected item in other inventory
				tButt = (UIRectTextButton *) 
					otherWindow->childRectList.Find(GTM_BUTTON_OTHERLINE0 + buttonIndex);

				AddMenuButton("Get", ACTION_TYPE_BUY);
				AddMenuButton("Get 5", ACTION_TYPE_BUY5);
				AddMenuButton("Get 20", ACTION_TYPE_BUY20);
				AddMenuButton("Get All", ACTION_TYPE_BUYALL);
			}
			selectedIndex = buttonIndex; // last item button index clicked on
		}
		AddMenuBottom();
	}

	uiAlpha = oldAlpha;

}

//*******************************************************************************
void GroundTestMode::AddInfoText(char *text, DWORD color)
{
	InjectTextLine(text, color);
	return;

//	UIRectWindow *listWindow = (UIRectWindow *) fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN);
	UIRectTextBox *textBox;

	textBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_STATSTEXT);

	CD3DFont *font = puma->GetDXFont(0);

	// how many lines can our text window show?
	int lines = (textBox->absBox.bottom - textBox->absBox.top) / font->RealTextHeight();

	// add text to the end
	char tempText[4096];

	int i = strlen(textBox->text);
//	memcpy(tempText, textBox->text, i);
//	memcpy(&(tempText[i]), text, strlen(text));
//	tempText[strlen(textBox->text)] = chCharCode;

	sprintf(tempText,"%s%s",textBox->text,text);

	// how many lines do we have now?
	i = 0;
	int linesUsed = 0;
	while (i < strlen(tempText))
	{
		if ('\n' == tempText[i])
			++linesUsed;

		++i;
	}

	// while too many lines
	while (linesUsed > lines)
	{
		// delete the top one
		int secondLineStart = 0;
		while ('\n' != tempText[secondLineStart])
			++secondLineStart;

		++secondLineStart;

		memmove(&tempText[0],&tempText[secondLineStart], strlen(&tempText[secondLineStart]) + 1);
		--linesUsed;
	}

	textBox->SetText(tempText);

}


//*******************************************************************************
//int GroundTestMode::GetCardinalDirection(float angle, float *percent)
int GroundTestMode::GetCardinalDirection(float angle)
{
	angle = (float) NormalizeAngle(angle + D3DX_PI/8);

//	angle -= D3DX_PI/8;
//	while (angle < 0)

	float retVal = angle / (D3DX_PI * 2) * 8;
	if (retVal >= 8)
		retVal = 0;

	return (int) retVal;
/*
	if (fabs(0 - angle) < D3DX_PI/4)
	{
		if (percent)
			*percent = ((0 - angle) + D3DX_PI/4) / (D3DX_PI/2);
		return 0;
	}
	if (fabs(D3DX_PI * 2 - angle) < D3DX_PI/4)
	{
		if (percent)
			*percent = ((D3DX_PI * 2 - angle) + D3DX_PI/4) / (D3DX_PI/2);
		return 0;
	}
	if (fabs(D3DX_PI/2 - angle) < D3DX_PI/4)
	{
		if (percent)
			*percent = ((D3DX_PI/2 - angle) + D3DX_PI/4) / (D3DX_PI/2);
		return 1;
	}
	if (fabs(D3DX_PI - angle) < D3DX_PI/4)
	{
		if (percent)
			*percent = ((D3DX_PI - angle) + D3DX_PI/4) / (D3DX_PI/2);
		return 2;
	}
	if (fabs(D3DX_PI*3.0f/2.0f - angle) < D3DX_PI/4)
	{
		if (percent)
			*percent = ((D3DX_PI*3.0f/2.0f - angle) + D3DX_PI/4) / (D3DX_PI/2);
		return 3;
	}

	return -1;  // should never get here
	*/
}

//*******************************************************************************
// handles the button press (or equivilant key hit) of the top-right 4 buttons
void GroundTestMode::ProcessListPress(int type)
{
	button1Sound->PlayNo3D();
	MessInventoryRequestInfo requestInfo;

	int messType = MESS_SKILLS_PLAYER;
	if (GTM_BUTTON_LIST_INV == type)
		messType = MESS_INVENTORY_PLAYER;
	if (GTM_BUTTON_LIST_WRK == type)
		messType = MESS_WORKBENCH_PLAYER;
	if (GTM_BUTTON_LIST_WLD == type)
		messType = MESS_WIELD_PLAYER;

	if (!fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN))
	{
		fullWindow->AddChild(listWindow);
		fullWindow->Arrange();
		requestInfo.type = messType;
		requestInfo.offset = playerInvIndex;
		lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
		playerListMode = type;
	}
	else if (type != playerListMode)
	{
		requestInfo.type = messType;
		requestInfo.offset = playerInvIndex;
		lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);
		playerListMode = type;
	}
	else
	{
		fullWindow->childRectList.Remove(listWindow);
		fullWindow->Arrange();
		playerListMode = 0;
	}
}

//*******************************************************************************
void GroundTestMode::StartAddingMenuButtons(int startX, int startY)
{
	ambCurX = startX;
	ambCurY = startY;
	ambIndex = 0;
}

//*******************************************************************************
void GroundTestMode::AddMenuButton(char *text, int style, LPDIRECT3DTEXTURE8 art, 
											  int left, int right, int font)
{
	UIRectTextButton *tButt = new UIRectTextButton(GTM_BUTTON_DO1+ambIndex,
		                          ambCurX + left , ambCurY, 
										  ambCurX + right, ambCurY + 26);
	tButt->SetText(text);
	tButt->font = font;
	tButt->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tButt->process = groundTestModeProcess;
	tButt->textFlags = D3DFONT_CENTERED | D3DFONT_VERTCENTERED;
	if (art)
		tButt->fillArt = art;
	else
		tButt->fillArt = uiPopUpArt;
	actionWindow->AddChild(tButt);

	actionButtonTypes[ambIndex] = style;

	ambCurY += 26;
	++ambIndex;
}

//*******************************************************************************
void GroundTestMode::AddMenuSpacer(void)
{
	UIRectTextBox *tBox = new UIRectTextBox(GTM_BUTTON_DO_SPACE,
		                          ambCurX -50 , ambCurY, 
										  ambCurX +103, ambCurY + 13);
	tBox->SetText("");
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->process = groundTestModeProcess;
	tBox->fillArt = uiPopUpSpaceArt;
	actionWindow->AddChild(tBox);

	ambCurY += 13;
}

//*******************************************************************************
void GroundTestMode::AddMenuTop(void)
{
	UIRectTextBox *tBox = new UIRectTextBox(GTM_BUTTON_DO_SPACE,
		                          ambCurX -50 , ambCurY, 
										  ambCurX +103, ambCurY + 13);
	tBox->SetText("");
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->process = groundTestModeProcess;
	tBox->fillArt = uiPopUpTop;
	actionWindow->AddChild(tBox);

	ambCurY += 13;
}

//*******************************************************************************
void GroundTestMode::AddMenuBottom(void)
{
	UIRectTextBox *tBox = new UIRectTextBox(GTM_BUTTON_DO_SPACE,
		                          ambCurX -50 , ambCurY, 
										  ambCurX +103, ambCurY + 13);
	tBox->SetText("");
	tBox->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
	tBox->process = groundTestModeProcess;
	tBox->fillArt = uiPopUpBottom;
	actionWindow->AddChild(tBox);

	ambCurY += 13;
}

//*******************************************************************************
void GroundTestMode::RemoveNonInvWindows(int secureStop)
{
	MessSecureTrade mess;

	if (fullWindow->childRectList.Find(GTM_BUTTON_OTHERWIN))
	{
		fullWindow->childRectList.Remove(otherWindow);
	}
	if (fullWindow->childRectList.Find(GTM_BUTTON_BANKWIN))
	{
		fullWindow->childRectList.Remove(bankWindow);
	}
	if (fullWindow->childRectList.Find(GTM_BUTTON_YOU_SECURE_WIN))
	{
		fullWindow->childRectList.Remove(secureYouWindow);

		if (secureStop)
		{
			mess.type = MESS_SECURE_STOP;
			lclient->SendMsg(sizeof(mess),(void *)&mess);
		}
	}
	if (fullWindow->childRectList.Find(GTM_BUTTON_HER_SECURE_WIN))
	{
		fullWindow->childRectList.Remove(secureHerWindow);

		if (secureStop)
		{
			mess.type = MESS_SECURE_STOP;
			lclient->SendMsg(sizeof(mess),(void *)&mess);
		}
	}
}

//*******************************************************************************
void GroundTestMode::HandleExtendedInfo(char* rawData, int size)
{
	char tempText[2048];

	Chronos::BStream *	stream		= NULL;
	stream	= new Chronos::BStream(1024);

	stream->write(rawData, size);
	stream->setPosition(0);

	unsigned char temp;
	*stream >> temp;
	assert( NWMESS_EXTENDED_INFO == temp);

	unsigned char type; //  INVOBJ_
	*stream >> type;

	if (INVOBJ_SKILL == type)
		sprintf(tempText,"Skill: ");
	else
		sprintf(tempText,"Item : ");

	// get name
	for (int k = 7; k < 1000; ++k)
	{
		*stream >> tempText[k];
		if (0 == tempText[k])
			k = 1000;
	}

	int passiveSkill = FALSE;
	if (!strcmp(&tempText[7],"Dodging"))
		passiveSkill = TRUE;

	sprintf(&tempText[strlen(tempText)],"\n");

	float value;
	long amount;
	int status;

	*stream >> value;
	*stream >> amount;
	*stream >> status;

	if (INVOBJ_SKILL != type)
	{
		if (amount > 1)
		{
			sprintf(&tempText[strlen(tempText)],"There are %ld of these.\nA trader would pay %ld for one.\n", 
						  amount, (long) (value * 0.7f));
		}
		else
		{
			sprintf(&tempText[strlen(tempText)],"A trader would pay %ld for it.\n", 
						  (long) (value * 0.7f));
		}
	}
	short damageDone;
	short toHit, quality, charges;
	unsigned char r,g,b;
	float damageVal, challenge;
	int eggType;
	char eggVal;
	int bladeAge;

	long timeLeft, daysLeft, hoursLeft, minutesLeft;
	unsigned long skillPoints;
	unsigned long skillLevel;

	unsigned short poison, heal, slow, blind, shock;

	int bombType, fuseDelay, bladeGlamour;
	float bombQuality;
	float bombPower;

	int potionType, potionSubType;

	int geopartType;
	float geopartQuality;
	float ekPower;
	int ekMonster[2];
	int ekWidth, ekHeight;

	switch(type)
	{
	case INVOBJ_BLADE:
		*stream >> toHit;
		*stream >> damageDone;

		sprintf(&tempText[strlen(tempText)],"It has a to-hit bonus of %d,\n", toHit);
		sprintf(&tempText[strlen(tempText)],"and will do %d points damage,\nconsidering your physical stat\nand other magic effects.\n", damageDone);

		//*stream >> poison;
		//*stream >> heal;
		*stream >> slow;
		*stream >> blind;
		*stream >> shock;
		*stream >> bladeAge;
		*stream >> bladeGlamour;

		if (bladeAge >= 15000)
			sprintf(&tempText[strlen(tempText)],"It is ancient (%d).\n", bladeAge);
		else if (bladeAge >= 10000)
			sprintf(&tempText[strlen(tempText)],"It is venerable (%d).\n", bladeAge);
		else if (bladeAge >= 5000)
			sprintf(&tempText[strlen(tempText)],"It is well-worn (%d).\n", bladeAge);
		else if (bladeAge >= 2500)
			sprintf(&tempText[strlen(tempText)],"It is mature (%d).\n", bladeAge);
		else if (bladeAge >= 1000)
			sprintf(&tempText[strlen(tempText)],"It is young (%d).\n", bladeAge);
		else
			sprintf(&tempText[strlen(tempText)],"It is new (%d).\n", bladeAge);

		/*
		if (poison > 0)
			sprintf(&tempText[strlen(tempText)],"It can poison at level %d.\n", poison);
		if (heal > 0)
			sprintf(&tempText[strlen(tempText)],"It heals in combat at level %d.\n", heal);
		*/
		if (slow > 0)
			sprintf(&tempText[strlen(tempText)],"It can slow at level %d.\n", slow);
		if (blind > 0)
			sprintf(&tempText[strlen(tempText)],"It can blind at level %d.\n", blind);
		
		if (shock > 0)
			sprintf(&tempText[strlen(tempText)],"It can shock at level %d.\n", shock);
		

		if (bladeGlamour >= BLADE_GLAMOUR_TRAILWHITE)
			sprintf(&tempText[strlen(tempText)],"It contains a %s.\n", 
			          dustNames[bladeGlamour-BLADE_GLAMOUR_TRAILWHITE+5]);


//		*stream << ((InvBlade *)io->extra)->toHit;
//		*stream << ((InvBlade *)io->extra)->damageDone;
		break;
	case INVOBJ_TOTEM:
		*stream >> timeLeft;
		*stream >> quality;

		if (timeLeft != -1)
		{
		sprintf(&tempText[strlen(tempText)],"Its effective magic power is %d.\n", quality);

		daysLeft    = timeLeft / 60 / 24;
		hoursLeft   = (timeLeft - daysLeft * 24 * 60) / 60;
		minutesLeft = (timeLeft - daysLeft * 24 * 60 - hoursLeft * 60);

			sprintf(&tempText[strlen(tempText)],
				       "It will last another %ld days, %ld hours,\nand %ld minutes.  ", 
				       daysLeft, hoursLeft, minutesLeft);
			sprintf(&tempText[strlen(tempText)],"This time will pass\neven when you aren't on-line.\n");
			sprintf(&tempText[strlen(tempText)],"It will only have an effect\nif placed in your Used list.\n");
		}
		else
		{
			sprintf(&tempText[strlen(tempText)],
				       "It currently has no magic powers.\nTo imbue it, Include it into your\nworkbench and use magic skills on it.");
		}
//		*stream << (long) ((InvTotem *)io->extra)->timeToDie.MinutesDifference(&ltNow);
//		*stream << (short) (((InvTotem *)io->extra)->quality - ((InvTotem *)io->extra)->imbueDeviation);
		break;
	case INVOBJ_FAVOR:
		sprintf(&tempText[strlen(tempText)],"This is a tangible token of favor from\none of the Great Spirits.\n\nIt has many mysterious uses.\n");
		break;
	case INVOBJ_STAFF:
		*stream >> charges;
		*stream >> quality;

		if (charges != -1)
		{
			sprintf(&tempText[strlen(tempText)],"It has %d imperfections.\n", quality);

			sprintf(&tempText[strlen(tempText)],"It has %d charges left, after\nwhich it will be destroyed.\n", charges);

			sprintf(&tempText[strlen(tempText)],"Wield it like a weapon, by\nplacing it in your Used list.\n");
		}
		else
		{
			sprintf(&tempText[strlen(tempText)],
				       "It currently has no magic powers.\nTo imbue it, Include it into your\nworkbench and use magic skills on it.");
		}
		break;
	case INVOBJ_INGOT:
		*stream >> damageVal;
		*stream >> challenge;
		*stream >> r;
		*stream >> g;
		*stream >> b;

		sprintf(&tempText[strlen(tempText)],"It has a damage value of %d,\n", (int)damageVal);
		sprintf(&tempText[strlen(tempText)],"and requires a work value of %d.\n", (int)challenge);
		sprintf(&tempText[strlen(tempText)],"Its natural color is r%d g%d b%d.\n", 
			          (int)r, (int)g, (int)b);

		sprintf(&tempText[strlen(tempText)],"Make a weapon by Including a number of\ningots in your workbench, then\nusing your Swordsmith skill.");

		break;
	case INVOBJ_SKILL:
		*stream >> skillLevel;
		*stream >> skillPoints;

		sprintf(&tempText[strlen(tempText)],"This skill is at level %ld,\n", skillLevel);
		sprintf(&tempText[strlen(tempText)],"with %ld total skill points.\n", skillPoints);
		
		if (passiveSkill)
			sprintf(&tempText[strlen(tempText)],"This skill gains points through\nnormal use.  There is no need\nto activate or invoke it.");
		else
			sprintf(&tempText[strlen(tempText)],"Use this skill by Combining items\nyou've placed in your workbench.");

		break;
	case INVOBJ_INGREDIENT:
		sprintf(&tempText[strlen(tempText)],"This is an Ingredient, used by\nCombining with other items\nyou've placed in your workbench.");

//		*stream << ((InvIngredient *)io->extra)->type;
//		*stream << ((InvIngredient *)io->extra)->quality;
		break;
	case INVOBJ_EGG:
		*stream >> eggType;
		*stream >> eggVal;

		switch (eggType)
		{
		case DRAGON_TYPE_RED:
			sprintf(&tempText[strlen(tempText)],
				 "This is a Red Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			break;
		case DRAGON_TYPE_GREEN:
			sprintf(&tempText[strlen(tempText)],
				 "This is a Green Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			sprintf(&tempText[strlen(tempText)],
				 "Green Drakes also heal their owners\nin combat.\n");
			break;
		case DRAGON_TYPE_BLUE:
			sprintf(&tempText[strlen(tempText)],
				 "This is a Blue Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			sprintf(&tempText[strlen(tempText)],
				 "Blue Drakes can also magically\nslow their opponents.\n");
			break;
		case DRAGON_TYPE_WHITE:
			sprintf(&tempText[strlen(tempText)],
				 "This is a White Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			sprintf(&tempText[strlen(tempText)],
				 "White Drakes can also magically\nblind their opponents.\n");
			break;
		case DRAGON_TYPE_BLACK:
			sprintf(&tempText[strlen(tempText)],
				 "This is a Black Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			sprintf(&tempText[strlen(tempText)],
				 "Black Drakes can also magically\npoison their opponents.\n");
			break;
		case DRAGON_TYPE_GOLD:
			sprintf(&tempText[strlen(tempText)],
				 "This is a Gold Drake egg.  If hatched,\nit will be tame, and will breath fire\non your enemies.\n");
			sprintf(&tempText[strlen(tempText)],
				 "Gold Drakes can also magically\nslow and blind their opponents\n and heal their owners in combat.\n");
			break;
		}

		if (eggVal > 0)
		{
			sprintf(&tempText[strlen(tempText)],
				 "Unfortunately, your magical stat is\n%d, which is too low to hatch this egg.\n", eggVal);
		}
		else
		{
			sprintf(&tempText[strlen(tempText)],
				 "To hatch it, Include it in your\nworkbench, and use magic on it.\n");
		}



		break;
	case INVOBJ_MEAT:
//		*stream << ((InvMeat *)io->extra)->type;
//		*stream << ((InvMeat *)io->extra)->quality;
//		*stream << ((InvMeat *)io->extra)->age;
		break;

	case INVOBJ_POTION:
		*stream >> potionType;
		*stream >> potionSubType;

		if (POTION_TYPE_RECALL == potionType)
			sprintf(&tempText[strlen(tempText)],"This scroll, when activated, teleports\nyou back to the town it was\npurchased from.  This destroys it.\n");

		break;

	case INVOBJ_EXPLOSIVE:
		*stream >> bombType;
		*stream >> bombQuality;
		*stream >> bombPower;

		sprintf(&tempText[strlen(tempText)],"It has a damage value of %3.2f,\n", bombPower);
		sprintf(&tempText[strlen(tempText)],"and requires a work value of %3.2f.\n", bombQuality);
//		sprintf(&tempText[strlen(tempText)],"Its natural color is r%d g%d b%d.\n", 
//			          (int)r, (int)g, (int)b);

		sprintf(&tempText[strlen(tempText)],"Make a bomb by Including a number of\nexplosives and fuses in your workbench,\nthen using your Explosives skill.");

		break;

	case INVOBJ_FUSE:
		*stream >> bombType;
		*stream >> bombQuality;

		sprintf(&tempText[strlen(tempText)],"It will delay an explosion for %3.2f seconds.\n", bombQuality);

		sprintf(&tempText[strlen(tempText)],"Make a bomb by Including a number of\nexplosives and fuses in your workbench,\nthen using your Explosives skill.");

		break;

	case INVOBJ_GEOPART:
		*stream >> geopartType;
		*stream >> geopartQuality;

		if (0 == geopartType)
			sprintf(&tempText[strlen(tempText)],"This bead has a magic power of %3.2f.\n", geopartQuality);
		else
			sprintf(&tempText[strlen(tempText)],"This has a magic power of %3.2f.\n", geopartQuality);

		sprintf(&tempText[strlen(tempText)],"Make an EarthKey by Including\na number of beads and\nHeart Gems in your workbench,\nplus one or two pieces of monster flesh,\nthen using your Geomancy skill.");

		break;

	case INVOBJ_EARTHKEY:
		*stream >> ekPower;
		*stream >> ekMonster[0];
		*stream >> ekMonster[1];
		*stream >> ekWidth;
		*stream >> ekHeight;

		sprintf(&tempText[strlen(tempText)],"This EarthKey can transport you to a\ndungeon %3.2f meters below.\n", ekPower + 20);
		sprintf(&tempText[strlen(tempText)],"It is %d by %d in size, and\n", ekWidth, ekHeight);
		if (-1 == ekMonster[1])
			sprintf(&tempText[strlen(tempText)],"is a lair of the %s and its kin.\n", monsterData[ekMonster[0]]->name);
		else
			sprintf(&tempText[strlen(tempText)],"is populated by %s and \n%s monsters, and their kin.\n", monsterData[ekMonster[0]]->name, monsterData[ekMonster[1]]->name);
		break;

	case INVOBJ_BOMB:
		*stream >> r;
		*stream >> g;
		*stream >> b;

		*stream >> bombPower;
		*stream >> bombQuality;
		*stream >> fuseDelay;

		sprintf(&tempText[strlen(tempText)],"It's power is %3.2f,\n", bombPower);
		sprintf(&tempText[strlen(tempText)],"and is %3.2f%% stable.\n", bombQuality * 100);
		sprintf(&tempText[strlen(tempText)],"The fuse is set for a %d second delay.\n", fuseDelay);
		sprintf(&tempText[strlen(tempText)],"Its explosive color is r%d g%d b%d.\n", 
			          (int)r, (int)g, (int)b);

		sprintf(&tempText[strlen(tempText)],"Use a bomb by selecting it and\nchoosing the Set option.\nThen run away.");

		break;

	case INVOBJ_SIMPLE:
		if (INVSTATUS_QUEST_ITEM != status)
			sprintf(&tempText[strlen(tempText)],"This item has no use.  Sell it for cash.\n");
		break;
	}

	if (INVSTATUS_QUEST_ITEM == status)
		sprintf(&tempText[strlen(tempText)],"This item was created for a quest.\n");

	UIRectTextBox *tBox = (UIRectTextBox *) 
		    fullWindow->childRectList.Find(GTM_BUTTON_ITEM_INFO_TEXT);
	if (tBox)
		tBox->SetText(tempText);

	delete stream;

}

//*******************************************************************************
void GroundTestMode::SetTipText(void)
{
	UIRectTextBox *tBox = (UIRectTextBox *) 
		    fullWindow->childRectList.Find(GTM_BUTTON_TIP_TEXT);
	if (tBox)
	{
		switch(currentTip)
		{
		default:
			tBox->SetText("");
			break;
		case 0:
			tBox->SetText("Tip #1\nNow you are in the Blade Mistress world!\nThe first thing to try is looking around.  Press the\nright and left arrow keys, and you can look around you!\n\nYou can also hold down the RIGHT mouse button,\nand move the mouse. Using the mouse also lets you\nlook up and down.\n\nPress 't' for the next tip.");
			break;
		case 1:
			tBox->SetText("Tip #2\nYou can MOVE using the up arrow key. When you move,\nyou move to the NEXT BIG SQUARE in front of you, not\njust a few steps, and you don't stop until you get there.\nThis is a fundamental part of Blade Mistress.\nEvery player and monster moves from square to square.\n\nWhen you press the up arrow, you move to\nthe square the screen (not your character) is facing.\n\nPress 't' for the next tip.");
			break;
		case 2:
			tBox->SetText("Tip #3\nThe box under these words is the chat box.  You can\npress ENTER, then type in anything you want to say.\nSend the message off by pressing ENTER again.\nOther players who are in your square,\nor one square away, can hear you.\n\nPress 't' for the next tip.");
			break;
		case 3:
			tBox->SetText("Tip #4\nThe long bar at the top edge of the chat box can be\nraised to show more lines of text.  Just click and\ndrag the bar up and down.\n\nPress 't' for the next tip.");
			break;
		case 4:
			tBox->SetText("Tip #5\nYou start in the town of Fingle.  There are other\nplayers around you, and there are NPCs.  These\nare people without swords, and with puffy shirts.\nBlue shirts are merchants, green shirts are trainers,\nand red shirts are town mages.  You can go up to\nany of them and click on them to\nfind out how they'll help you.\n\nPress 't' for the next tip.");
			break;
		case 5:
			tBox->SetText("Tip #6\nLook in the upper-right corner of the screen, and you'll\nsee four buttons.  Click these to look at your four\ncharacter lists. The Inventory list shows what you are\ncarrying. The Used list shows what you're using right\nnow, like what sword you're wielding.  The Workbench\nlist is where you put crafting ingredients.  The Skills\nlist shows the skills you got from the trainer NPC.\n\nPress 't' for the next tip.");
			break;
		case 6:
			tBox->SetText("Tip #7\nLook in the lower-right corner of the screen.  There's\nan Options button there, for the Options screen.\nIn that screen, you can read more\ninstructions, adjust some detail settings\nof the game, and quit the game.\n\nPress 't' for the next tip.");
			break;
		case 7:
			tBox->SetText("Tip #8\nLook in the upper-left corner of the screen.  There's\na location and direction, and your Character Level.\nThe numbers show you where you are on the\nmap, and the letter shows\nwhich way you're facing.\n\nPress 't' for the next tip.");
			break;
		case 8:
			tBox->SetText("Tip #9\nFighting a monster is EASY.  Just move\ninto its square, and you'll begin to\nbattle it automatically.  If you feel\nyou are losing, just move away.\nIf the monster dies, you'll\npick up its loot automatically, too.\n\nPress 't' for the next tip.");
			break;
		case 9:
			tBox->SetText("That's the last tip!  To see the tips again,\npress F2. Have fun,\nand thanks for playing Blade Mistress!\n\nPress 't' to get rid of this text.");
			break;
		}
	}
}


//*******************************************************************************
void GroundTestMode::ClearEverythingFromScreen(void)
{
	UIRectEditLine *tEdit;
	UIRectTextBox *tBox;

	tBox = (UIRectTextBox *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYTITLE);
	if (tBox)
	{
		fullWindow->childRectList.Remove(tBox);
		delete tBox;

		tEdit = (UIRectEditLine *) fullWindow->childRectList.Find(GTM_BUTTON_GIVEMONEYEDLINE);
		if (tEdit)
		{
			fullWindow->childRectList.Remove(tEdit);
			delete tEdit;
		}
	}

	giveMoneyMode = FALSE;
	namePetMode = FALSE;

	RemoveNonInvWindows();
	if (fullWindow->childRectList.Find(GTM_BUTTON_STATSWIN))
	{
		fullWindow->childRectList.Remove(listWindow);
		fullWindow->Arrange();
		playerListMode = 0;
	}
}

//******************************************************************
void GroundTestMode::SetEnvironment(void)
{

//	aLog.Log("GroundTestMode::SetEnvironment 1 **\n");

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
   light.Diffuse.r  = 1.0f;
   light.Diffuse.g  = 1.0f;
   light.Diffuse.b  = 1.0f;
   light.Ambient.r  = 1.0f;
   light.Ambient.g  = 1.0f;
   light.Ambient.b  = 1.0f;
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

	bboClient->SetEnvironment();

}

/* end of file */



