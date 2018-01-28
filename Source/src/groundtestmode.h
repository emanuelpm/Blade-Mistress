#ifndef GROUNDTESTMODE_H
#define GROUNDTESTMODE_H

#include ".\puma\gamemode.h"
#include "inventory.h"
#include ".\network\NetWorldMessages.h"


enum
{
   RM_SUBMODE_LOADING,
   RM_SUBMODE_START,
   RM_SUBMODE_RACING,
   RM_SUBMODE_PLAYER_FINISHED,
   RM_SUBMODE_PAUSE,
   RM_SUBMODE_QUIT_QUESTION,
   RM_SUBMODE_MAX
};

enum
{
	ACTION_TYPE_DROP,
	ACTION_TYPE_TRADE,
	ACTION_TYPE_SECURE_TRADE,
	ACTION_TYPE_GIVE,
	ACTION_TYPE_GIVE5,
	ACTION_TYPE_GIVE20,
	ACTION_TYPE_GIVEALL,
	ACTION_TYPE_BUY,
	ACTION_TYPE_BUY5,
	ACTION_TYPE_BUY20,
	ACTION_TYPE_BUYALL,
	ACTION_TYPE_SELL,
	ACTION_TYPE_USE,
	ACTION_TYPE_ACTIVATE,
	ACTION_TYPE_WIELD,
	ACTION_TYPE_DEACTIVATE,
	ACTION_TYPE_UNWIELD,
	ACTION_TYPE_ATTACK,
	ACTION_TYPE_INCLUDE,
	ACTION_TYPE_INCLUDE5,
	ACTION_TYPE_INCLUDE20,
	ACTION_TYPE_INCLUDEALL,
	ACTION_TYPE_EXCLUDE,
	ACTION_TYPE_EXCLUDE5,
	ACTION_TYPE_EXCLUDE20,
	ACTION_TYPE_EXCLUDEALL,
	ACTION_TYPE_COMBINE,
	ACTION_TYPE_TELEPORT,
	ACTION_TYPE_TELEPORT_BACK,
	ACTION_TYPE_HEAL,
	ACTION_TYPE_OPEN,
	ACTION_TYPE_LISTEN,
	ACTION_TYPE_QUEST,
	ACTION_TYPE_FEED0,
	ACTION_TYPE_FEED1,
	ACTION_TYPE_CONTROL,
	ACTION_TYPE_RELEASE,
	ACTION_TYPE_SELLALLGREEN,
	ACTION_TYPE_SELLALLMEAT,
	ACTION_TYPE_SELLALLBOMBS,
	ACTION_TYPE_SELLALLKEYS,
	ACTION_TYPE_RETRACT,
	ACTION_TYPE_RETRACT5,
	ACTION_TYPE_RETRACT20,
	ACTION_TYPE_RETRACTALL,
	ACTION_TYPE_SET_BOMB,
	ACTION_TYPE_BANK,
	ACTION_TYPE_MAX
};

class PumaMesh;
class	ChatWindow;
class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;
class BBOMob;
class UIRectTextBox;

class GroundTestMode : public GameMode
{
public:

	GroundTestMode(int playerID, int doid, char *doname);
	virtual ~GroundTestMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);
	void        InjectTextLine(char *newText, DWORD color = 0xffffffff);
	void        InjectSingleTextLine(char *newText, DWORD color = 0xffffffff);
	void			HandleInvenInfo(char* rawData, int size);
	int 			ShootRayIntoScene(PumaInputEvent *pie, int justTest = FALSE);
	void			RemoveActionButtons(int eraseGiveMoneyDialog = TRUE);
	void			UpdateActionButtons(int buttonIndex, int isPlayer, int mx, int my, int type = -1);
	void			AddInfoText(char *text, DWORD color = 0xffffff00);
//	int         GetCardinalDirection(float angle, float *percent = NULL);
	int         GetCardinalDirection(float angle);
	void        ProcessListPress(int type);
	void        SetTipText(void);

	void			StartAddingMenuButtons(int startX, int startY);
	void			AddMenuButton(char *text, int style, LPDIRECT3DTEXTURE8 art = NULL, 
											  int left = -50, int right = 103, int font = 1);
	void			AddMenuSpacer(void);
	void			AddMenuTop   (void);
	void			AddMenuBottom(void);
	void        RemoveNonInvWindows(int secureStop = TRUE);
	void			HandleExtendedInfo(char* rawData, int size);
	void			ClearEverythingFromScreen(void);
	int         TypingBoxOn(void);

	DoublyLinkedList ships;

	unsigned long flashCounter;

	LPDIRECT3DTEXTURE8 mouseArt, skyArt, compassArt, barArt[2], 
		                buttonArt, uiArt, uiPlayerListArt[4], uiOtherListArt,
							 uiPopUpArt, uiPopUpLongArt, uiPopUpSpaceArt, 
							 uiPopUpTop, uiPopUpBottom, uiCompassArt,
							 uiPlayerFrameArt, uiTradeListArt, radarArt[9];

	UIRectWindow *fullWindow, *actionWindow, *listWindow, *charInvWindow;
	UIRectWindow *otherWindow, *secureYouWindow, *secureHerWindow, *bankWindow;
	UIRectWindow *bugReportWindow;
	ChatWindow *chatWin;
	UIRectEditLine *edLine;
	UIRectTextBox *monsterHealthBox, *compassBox, *compassBox2;


	InventoryRef playerInv[18], otherInv[18], bankInv[18], yourInv[18], herInv[18];
	int actionButtonTypes[18];
	int playerInvIndex, otherInvIndex, selectedIndex, bankInvIndex;
	int secureYouInvIndex, secureHerInvIndex;
	int playerListMode; // GTM_BUTTON_LIST_*
	int giveMoneyMode, namePetMode;
	int currentTip, chatChannelValues;
	long money, partnerPtr, monsterHealthTimer;
	DWORD moveRequestTimer;
	BBOMob *lastSelectedMOB;
	float camSpinDelta, camRaiseDelta, rmbDownPos, rmbDownPosY;

	int ambCurX, ambCurY, ambIndex, mapState;

	char securePartnerName[32];
	DWORD hoverTime;
	long itemInfoListMode, itemInfoPtr;


};

#endif
