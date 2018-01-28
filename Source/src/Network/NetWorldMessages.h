/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

#ifndef NETWORLDMESSAGES_H
#define NETWORLDMESSAGES_H

#include "../BBO.h"

enum
{
    MESS_INVENTORY_GROUND,
    MESS_INVENTORY_PLAYER,
    MESS_WORKBENCH_PLAYER,
    MESS_SKILLS_PLAYER,
    MESS_WIELD_PLAYER,
    MESS_INVENTORY_MONSTER,
    MESS_INVENTORY_TRADER,
    MESS_INVENTORY_TRAINER,
    MESS_INVENTORY_HER_SECURE,
    MESS_INVENTORY_YOUR_SECURE,
    MESS_INVENTORY_SAME,
    MESS_INVENTORY_TOWER,
    MESS_INVENTORY_BANK,
    MESS_INVENTORY_MAX
};

enum
{
    MESS_ADMIN_ACTIVATE,
    MESS_ADMIN_TAKE_CONTROL,
    MESS_ADMIN_RELEASE_CONTROL,
    MESS_ADMIN_MAX
};

enum
{
    MESS_SECURE_STOP,
    MESS_SECURE_NO_AGREEMENT,
    MESS_SECURE_REFUSE,
    MESS_SECURE_ACCEPT,
    MESS_SECURE_MAX
};

const unsigned int MESS_CHANGE_NOTHING = 0x00;
const unsigned int MESS_CHANGE_EDITING = 0x01;
const unsigned int MESS_CHANGE_TOWER   = 0x02;
const unsigned int MESS_CHANGE_TEMP    = 0x04;


const int MESSINFOTEXTLEN = 100;

//#include "NetWorldObject.h" // needed for the definition of ObjectID.
//***********************************************************************************
// net games need a garanteed unique id for each object in the game.  This ObjectID
// class attempts that by combining the GetTickCount() at the time of creation, the
// pointer value of the original object (its "this" pointer), and the first four
// letters of the originating player's handle, stuffed into a long word.
struct ObjectID
{
    ObjectID(void) { creationTime = originalPointer = playerTag = 0; };
    unsigned long creationTime;
    unsigned long originalPointer;
    unsigned long playerTag;
};


#pragma pack(push)
#pragma pack(1)

//********************************
enum
{
   NWMESS_EMPTY,
   NWMESS_ACCEPT,
   NWMESS_CONNECT,
   NWMESS_CLOSE,
   NWMESS_PLAYER_CHAT_LINE,
   NWMESS_AVATAR_APPEAR,
   NWMESS_AVATAR_DISAPPEAR,
   NWMESS_AVATAR_MOVE,
   NWMESS_AVATAR_MOVE_REQUEST,
   NWMESS_AVATAR_STATS,

   NWMESS_AVATAR_REQUEST_STATS,
   NWMESS_PLAYER_NEW,
   NWMESS_PLAYER_RETURNING,
   NWMESS_GENERAL_YES,
   NWMESS_GENERAL_NO,
   NWMESS_ENTER_GAME,
    NWMESS_EXIT_GAME,
   NWMESS_BUG_REPORT,
   NWMESS_INVENTORY_INFO,
   NWMESS_INVENTORY_REQUEST_INFO,

   NWMESS_INVENTORY_CHANGE,
   NWMESS_INVENTORY_TRANSFER_REQUEST,
   NWMESS_INVENTORY_FINISH,
    NWMESS_INFO_TEXT,
   NWMESS_MOB_APPEAR,
   NWMESS_MOB_APPEAR_CUSTOM,
   NWMESS_MOB_DISAPPEAR,
   NWMESS_MOB_BEGIN_MOVE,
   NWMESS_MOB_BEGIN_MOVE_SPECIAL,
   NWMESS_MOB_STATS,

   NWMESS_MOB_REQUEST_STATS,
    NWMESS_WIELD,
    NWMESS_UNWIELD,
    NWMESS_BLADE_DESC,
    NWMESS_MONSTER_ATTACK,
    NWMESS_MONSTER_DEATH,
    NWMESS_AVATAR_ATTACK,
    NWMESS_AVATAR_DEATH,
    NWMESS_AVATAR_HEALTH,
    NWMESS_MONSTER_HEALTH,

    NWMESS_AVATAR_NAMES,
    NWMESS_AVATAR_DELETE,
    NWMESS_TRY_COMBINE,
    NWMESS_CHANGE_MAP,
    NWMESS_REQUEST_DUNGEON_INFO,
    NWMESS_DUNGEON_INFO,
    NWMESS_DUNGEON_CHANGE,
    NWMESS_REQUEST_AVATAR_INFO,
    NWMESS_REQUEST_TOWNMAGE_SERVICE,
    NWMESS_CAVE_INFO,

    NWMESS_AVATAR_SEND_MONEY,
    NWMESS_CHEST_INFO,
    NWMESS_AVATAR_APPEAR_SPECIAL,
    NWMESS_BOOT,
    NWMESS_TALK_TO_TREE,
    NWMESS_TEST_PING,
    NWMESS_GENERIC_EFFECT,
    NWMESS_CHANT,
    NWMESS_PET,
    NWMESS_FEED_PET_REQUEST,

    NWMESS_PET_NAME,
    NWMESS_PET_ATTACK,
    NWMESS_ADMIN_MESSAGE,
    NWMESS_INFO_FLAGS,
    NWMESS_SELL_ALL,
    NWMESS_SECURE_TRADE,
    NWMESS_EXTENDED_INFO,
    NWMESS_EXTENDED_INFO_REQUEST,
    NWMESS_AVATAR_GUILD_NAME,
    NWMESS_AVATAR_GUILD_EDIT,

    NWMESS_EXPLOSION,
    NWMESS_SET_BOMB,
    NWMESS_TOKEN_DISAPPEAR,
    NWMESS_MAGIC_ATTACK,
    NWMESS_ADMIN_INFO,
    NWMESS_ACCOUNT_TIME_INFO,
    NWMESS_KEYCODE,
    NWMESS_KEYCODE_RESPONSE,
    NWMESS_AVATAR_NEW_CLOTHES,
    NWMESS_CHAT_CHANNEL,

    NWMESS_TIME_OF_DAY,
    NWMESS_WEATHER_STATE,
    NWMESS_SECURE_PARTNER_NAME,
    NWMESS_GROUND_EFFECT,
    NWMESS_MONSTER_SPECIAL_ATTACK,
    NWMESS_MONSTER_CHANGE_FORM,
   NWMESS_MAX

};

struct MessBase
{
	unsigned char idTag;
};

//********************************
struct MessEmpty : public MessBase
{
   MessEmpty(void) {idTag = NWMESS_EMPTY; };
};

//********************************
struct MessAccept : public MessBase
{
   MessAccept(void) {idTag = NWMESS_ACCEPT; };

    unsigned char IP[4];
};

//********************************
struct MessConnect : public MessBase
{
   MessConnect(void) {idTag = NWMESS_CONNECT; };
};

//********************************
struct MessClose : public MessBase
{
   MessClose(void) {idTag = NWMESS_CLOSE; };
};

//********************************
struct MessPlayerChatLine : public MessBase
{
   MessPlayerChatLine(void) {idTag = NWMESS_PLAYER_CHAT_LINE; };

   char text[110 + 32 + 3]; // up to 32 handle chars, plus up to 110 text chars, plus 3
};

//********************************
struct MessInfoText : public MessBase
{
   MessInfoText(void) {idTag = NWMESS_INFO_TEXT; };

   char text[MESSINFOTEXTLEN]; // up to MESSINFOTEXTLEN chars
};

//********************************
struct MessAvatarAppear : public MessBase
{
   MessAvatarAppear(void) {idTag = NWMESS_AVATAR_APPEAR; };

    int avatarID;
    unsigned char x,y;
};

//********************************
struct MessAvatarAppearSpecial : public MessBase
{
   MessAvatarAppearSpecial(void) {idTag = NWMESS_AVATAR_APPEAR_SPECIAL; };

    int avatarID;
    unsigned char x,y;
    unsigned short typeOfAppearance;
};

//********************************
struct MessAvatarDisappear : public MessBase
{
   MessAvatarDisappear(void) {idTag = NWMESS_AVATAR_DISAPPEAR; };

    int avatarID;
    unsigned char x,y;
};

//********************************
struct MessAvatarStats : public MessBase
{
   MessAvatarStats(void) {idTag = NWMESS_AVATAR_STATS; };

    int avatarID;
    int faceIndex, topIndex, bottomIndex;
    unsigned char hairR  , hairG  , hairB;   // hair
    unsigned char topR   , topG   , topB;    // hair
    unsigned char bottomR, bottomG, bottomB; // hair
    unsigned short imageFlags;
    char name[32];
    char physical, magical, creative;
    long cLevel, cash;
    char age;
};

//********************************
struct MessAvatarNames : public MessBase
{
   MessAvatarNames(void) {idTag = NWMESS_AVATAR_NAMES; };

    char name[NUM_OF_CHARS_PER_USER][32];

};

//********************************
struct MessAvatarRequestStats : public MessBase
{
   MessAvatarRequestStats(void) {idTag = NWMESS_AVATAR_REQUEST_STATS; };

    char characterIndex;

};

//********************************
struct MessAvatarDelete : public MessBase
{
   MessAvatarDelete(void) {idTag = NWMESS_AVATAR_DELETE; };

    char characterIndex;

};

//********************************
struct MessAvatarMoveRequest : public MessBase
{
   MessAvatarMoveRequest(void) {idTag = NWMESS_AVATAR_MOVE_REQUEST; };

    int avatarID;
    unsigned char x; //cardinal direction
//	,y;
//	unsigned char targetX, targetY;

};

//********************************
struct MessAvatarMove : public MessBase
{
   MessAvatarMove(void) {idTag = NWMESS_AVATAR_MOVE; };

    int avatarID;
    unsigned char x,y;
    unsigned char targetX, targetY;
};

//********************************
struct MessPlayerNew : public MessBase
{
   MessPlayerNew(void) 
   {
	   idTag = NWMESS_PLAYER_NEW; 
	   uniqueId=0;
	   memset(name, 0, sizeof(name));
	   memset(pass, 0, sizeof(pass));
   };

   char name[NUM_OF_CHARS_FOR_USERNAME];
   char pass[NUM_OF_CHARS_FOR_PASSWORD];
    int uniqueId;
};

//********************************
struct MessPlayerReturning : public MessBase
{
   MessPlayerReturning(void) 
   {
	   idTag = NWMESS_PLAYER_RETURNING; 
	   uniqueId=0;

	   memset(name, 0, sizeof(name));
	   memset(pass, 0, sizeof(pass));
   };

   char name[NUM_OF_CHARS_FOR_USERNAME];
   char pass[NUM_OF_CHARS_FOR_PASSWORD];
    int uniqueId;
    float version;
};

//********************************
struct MessGeneralYes : public MessBase
{
   MessGeneralYes(void) {idTag = NWMESS_GENERAL_YES; };

    int subType;
};

//********************************
struct MessGeneralNo : public MessBase
{
   MessGeneralNo(void) {idTag = NWMESS_GENERAL_NO; };

    int subType;
};

//********************************
struct MessEnterGame : public MessBase
{
   MessEnterGame(void) {idTag = NWMESS_ENTER_GAME; };

    char characterIndex;
};

//********************************
struct MessExitGame : public MessBase
{
   MessExitGame(void) {idTag = NWMESS_EXIT_GAME; };
};

//********************************
struct MessBugReport : public MessBase
{
   MessBugReport(void) {idTag = NWMESS_BUG_REPORT; };

    char crash,hang,art,gameplay,other;
    char doing[111];
    char playLength[111];
    char repeatable[111];
    char info[111];
};

//********************************
struct MessInventoryInfo : public MessBase
{
   MessInventoryInfo(void) {idTag = NWMESS_INVENTORY_INFO; };

   char  text[11][32];
   char  type[11];
//   char  status[10], type[10];
//	short position[10];
    long  ptr[11], value[11], amount[11], invPtr;
    float f1[11], f2[11];
   int   offset;
   char  isPlayerInfo;
   long  money;
   char traderType;
};

//********************************
struct MessInventoryRequestInfo : public MessBase
{
   MessInventoryRequestInfo(void) {idTag = NWMESS_INVENTORY_REQUEST_INFO; };

   int offset;
   char type;
    long which;
};

//********************************
// for moving items from one inventory list to another
struct MessInventoryChange : public MessBase
{
   MessInventoryChange(void) {idTag = NWMESS_INVENTORY_CHANGE; };

//   char isPlayerInfo;
   long ptr;
    unsigned char amount;
    unsigned char srcListType;
    unsigned char dstListType;
};

//********************************
struct MessInventoryTransferRequest : public MessBase
{
   MessInventoryTransferRequest(void) {idTag = NWMESS_INVENTORY_TRANSFER_REQUEST; };

   unsigned char amount;
   long ptr, partner;
    unsigned char playerListType;
   char isPlayerInfo;
};

//********************************
struct MessInventoryFinish : public MessBase
{
   MessInventoryFinish(void) {idTag = NWMESS_INVENTORY_FINISH; };

   char cancel;
};

//********************************
struct MessMobAppear : public MessBase
{
   MessMobAppear(void) {idTag = NWMESS_MOB_APPEAR; };

    unsigned long mobID;
    unsigned char type;
    unsigned short monsterType, subType;
    unsigned char staticMonsterFlag;
    unsigned char x,y;
};

//********************************
struct MessMobAppearCustom : public MessBase
{
   MessMobAppearCustom(void) {idTag = NWMESS_MOB_APPEAR_CUSTOM; };

    unsigned long mobID;
    unsigned char type;
    unsigned short monsterType, subType;
    unsigned char staticMonsterFlag;
    unsigned char x,y;
    unsigned char r,g,b,a;
    float sizeCoeff;
    char name[32];
};

//********************************
struct MessMobDisappear : public MessBase
{
   MessMobDisappear(void) {idTag = NWMESS_MOB_DISAPPEAR; };

    unsigned long mobID;
    unsigned char x,y;
};

//********************************
struct MessMobStats : public MessBase
{
   MessMobStats(void) {idTag = NWMESS_MOB_STATS; };

    unsigned long mobID;
    char name[32];

};

//********************************
struct MessMobRequestStats : public MessBase
{
   MessMobRequestStats(void) {idTag = NWMESS_MOB_REQUEST_STATS; };

    unsigned long mobID;

};

//********************************
struct MessMobBeginMove : public MessBase
{
   MessMobBeginMove(void) {idTag = NWMESS_MOB_BEGIN_MOVE; };

    unsigned long mobID;
    unsigned char x,y;
    unsigned char targetX, targetY;

};

//********************************
struct MessMobBeginMoveSpecial : public MessBase
{
   MessMobBeginMoveSpecial(void) {idTag = NWMESS_MOB_BEGIN_MOVE_SPECIAL; };

    unsigned long mobID;
    unsigned char x,y;
    unsigned char targetX, targetY;
    int specialType;

};

//********************************
struct MessWield : public MessBase
{
   MessWield(void) {idTag = NWMESS_WIELD; };

    long bladeID;
   unsigned char r,g,b;
};

//********************************
struct MessUnWield : public MessBase
{
   MessUnWield(void) {idTag = NWMESS_UNWIELD; };

    long bladeID;
};

//********************************
struct MessBladeDesc : public MessBase
{
   MessBladeDesc(void) {idTag = NWMESS_BLADE_DESC; };

    float size;
    long bladeID;
    int avatarID;
    unsigned short black, green, red, blue, white, trailType;
   unsigned char meshType, r,g,b;
};

//********************************
struct MessAvatarAttack : public MessBase
{
   MessAvatarAttack(void) {idTag = NWMESS_AVATAR_ATTACK; };

    int avatarID;
    unsigned long mobID;
    long damage; // -1 means miss
    long health, healthMax;  // along for the ride to the client
};

//********************************
struct MessMonsterAttack : public MessBase
{
   MessMonsterAttack(void) {idTag = NWMESS_MONSTER_ATTACK; };

    unsigned long mobID;
    int avatarID;
    long damage; // -1 means miss
};

//********************************
struct MessAvatarDeath : public MessBase
{
   MessAvatarDeath(void) {idTag = NWMESS_AVATAR_DEATH; };

    int avatarID;
};

//********************************
struct MessAvatarHealth : public MessBase
{
   MessAvatarHealth(void) {idTag = NWMESS_AVATAR_HEALTH; };

    int avatarID;
    long health, healthMax;
};

//********************************
struct MessMonsterHealth : public MessBase
{
   MessMonsterHealth(void) {idTag = NWMESS_MONSTER_HEALTH; };

    unsigned long mobID;
    long health, healthMax;
};

//********************************
struct MessMonsterDeath : public MessBase
{
   MessMonsterDeath(void) {idTag = NWMESS_MONSTER_DEATH; };

    unsigned long mobID;
};

//********************************
struct MessTryCombine : public MessBase
{
   MessTryCombine(void) {idTag = NWMESS_TRY_COMBINE; };

    long skillID;
};


//********************************
struct MessChangeMap : public MessBase
{
   MessChangeMap(void) {idTag = NWMESS_CHANGE_MAP; };

    char oldType, newType;
    int sizeX, sizeY;
    long dungeonID;
    unsigned char realmID;
    unsigned int flags;
};

//********************************
struct MessRequestDungeonInfo : public MessBase
{
   MessRequestDungeonInfo(void) {idTag = NWMESS_REQUEST_DUNGEON_INFO; };

    short x,y; // in 5s
};

//********************************
struct MessDungeonInfo : public MessBase
{
   MessDungeonInfo(void) {idTag = NWMESS_DUNGEON_INFO; };

    short sizeX, sizeY; // total dungeon size
    short x,y; // in 5s
    unsigned char leftWall[DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE], 
                   topWall [DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE];
    unsigned char floor, outerWall;
};

//********************************
struct MessDungeonChange : public MessBase
{
   MessDungeonChange(void) {idTag = NWMESS_DUNGEON_CHANGE; };

    short x,y;
    unsigned char floor, top, left, outer, reset;
};

//********************************
struct MessRequestAvatarInfo : public MessBase
{
   MessRequestAvatarInfo(void) {idTag = NWMESS_REQUEST_AVATAR_INFO; };

    int avatarID;
};

//********************************
struct MessRequestTownMageService : public MessBase
{
   MessRequestTownMageService(void) {idTag = NWMESS_REQUEST_TOWNMAGE_SERVICE; };

    unsigned char which;
};

//********************************
struct MessCaveInfo : public MessBase
{
   MessCaveInfo(void) {idTag = NWMESS_CAVE_INFO; };

    unsigned long mobID;
    unsigned char type, hasMistress;
};

//********************************
struct MessAvatarSendMoney : public MessBase
{
   MessAvatarSendMoney(void) {idTag = NWMESS_AVATAR_SEND_MONEY; };

    int avatarID;
    unsigned char x,y;
    int targetAvatarID;
    long amount;
};

//********************************
struct MessChestInfo : public MessBase
{
   MessChestInfo(void) {idTag = NWMESS_CHEST_INFO; };

    unsigned long mobID;
    unsigned char type;
};

//********************************
struct MessBoot : public MessBase
{
   MessBoot(void) {idTag = NWMESS_BOOT; };
};

//********************************
struct MessTestPing : public MessBase
{
   MessTestPing(void) {idTag = NWMESS_TEST_PING; };
};

//********************************
struct MessTalkToTree : public MessBase
{
   MessTalkToTree(void) {idTag = NWMESS_TALK_TO_TREE; };

   unsigned char which;
};


//********************************
struct MessGenericEffect : public MessBase
{
   MessGenericEffect(void) {idTag = NWMESS_GENERIC_EFFECT; };

    int avatarID;
    unsigned long mobID;
    unsigned char x,y; // used if cant find av or mob to attach to
    unsigned char r,g,b;
    unsigned char type; // type of effect
    short timeLen; // in seconds
};

//********************************
struct MessChant : public MessBase
{
   MessChant(void) {idTag = NWMESS_CHANT; };

    int avatarID;
    unsigned char r,g,b;
};

//********************************
struct MessPet : public MessBase
{
   MessPet(void) {idTag = NWMESS_PET; };

    int avatarID;

    unsigned char type, quality, state, which;
    float size;
    char name[16];
};

//********************************
struct MessFeedPetRequest : public MessBase
{
   MessFeedPetRequest(void) {idTag = NWMESS_FEED_PET_REQUEST; };

   long ptr;
    unsigned char which;
};

//********************************
struct MessPetName : public MessBase
{
   MessPetName(void) {idTag = NWMESS_PET_NAME; };

   char text[24]; // 16, should be
};

//********************************
struct MessPetAttack : public MessBase
{
   MessPetAttack(void) {idTag = NWMESS_PET_ATTACK; };

    int avatarID;
    unsigned long mobID;
    unsigned char which, type;
    long damage; // -1 means miss
    float effect; // 0 means no extra effect (according to type)
};

//********************************
struct MessAdminMessage : public MessBase
{
   MessAdminMessage(void) {idTag = NWMESS_ADMIN_MESSAGE; };

   unsigned char messageType;
    int avatarID;
    unsigned long mobID;

};

//********************************
struct MessInfoFlags : public MessBase
{
   MessInfoFlags(void) {idTag = NWMESS_INFO_FLAGS; };

   unsigned long flags;
};

//********************************
struct MessSellAll : public MessBase
{
   MessSellAll(void) {idTag = NWMESS_SELL_ALL; };

   unsigned char type;
    long which;
};

//********************************
struct MessSecureTrade : public MessBase
{
    MessSecureTrade(void) {idTag = NWMESS_SECURE_TRADE; };

   unsigned char type;
};

//********************************
struct MessExtendedInfo : public MessBase
{
    MessExtendedInfo(void) {idTag = NWMESS_EXTENDED_INFO; };
};

//********************************
struct MessExtendedInfoRequest : public MessBase
{
    MessExtendedInfoRequest(void) {idTag = NWMESS_EXTENDED_INFO_REQUEST; };

   unsigned char listType;
    long itemPtr;
};

//********************************
struct AvatarGuildName : public MessBase
{
    AvatarGuildName(void) {idTag = NWMESS_AVATAR_GUILD_NAME; };

    int avatarID;
};

//********************************
struct AvatarGuildEdit : public MessBase
{
    AvatarGuildEdit(void) {idTag = NWMESS_AVATAR_GUILD_EDIT; };

    int avatarID;
};

//********************************
struct MessExplosion : public MessBase
{
   MessExplosion(void) {idTag = NWMESS_EXPLOSION; };

    int avatarID;
    unsigned char x,y; // used if avatarID is -1
    unsigned char r,g,b;
    unsigned char type; // type of bomb
    unsigned long flags;
    float size;
};

//********************************
struct MessSetBomb : public MessBase
{
   MessSetBomb(void) {idTag = NWMESS_SET_BOMB; };

   long ptr;
};

//********************************
struct MessTokenDisappear : public MessBase
{
   MessTokenDisappear(void) {idTag = NWMESS_TOKEN_DISAPPEAR; };

    unsigned long mobID;
    unsigned char type;
    unsigned char x,y;
};

//********************************
struct MessMagicAttack : public MessBase
{
   MessMagicAttack(void) {idTag = NWMESS_MAGIC_ATTACK; };

    int avatarID;
    unsigned long mobID;
    char type;
    long damage; // -1 means miss
};

//********************************
struct MessAdminInfo : public MessBase
{
   MessAdminInfo(void) {idTag = NWMESS_ADMIN_INFO; };

    short numPlayers;
    short iters;
    short lastConnectTime;
};

//********************************
struct MessAccountTimeInfo : public MessBase
{
   MessAccountTimeInfo(void) {idTag = NWMESS_ACCOUNT_TIME_INFO; };

    int wYear, wMonth, wDay, wDayOfWeek, wHour, wMinute;
};

//********************************
struct MessKeyCode : public MessBase
{
   MessKeyCode(void) {idTag = NWMESS_KEYCODE; };

    char string[6+8+10]; // 10 just for grins
};

//********************************
struct MessKeyCodeResponse : public MessBase
{
   MessKeyCodeResponse(void) {idTag = NWMESS_KEYCODE_RESPONSE; };

   unsigned char value;
};

//********************************
struct MessAvatarNewClothes : public MessBase
{
   MessAvatarNewClothes(void) {idTag = NWMESS_AVATAR_NEW_CLOTHES; };

    int avatarID;
    int topIndex, bottomIndex;
    unsigned char topR   , topG   , topB;    // hair
    unsigned char bottomR, bottomG, bottomB; // hair

};

//********************************
struct MessChatChannel : public MessBase
{
   MessChatChannel(void) {idTag = NWMESS_CHAT_CHANNEL; };

   unsigned long value;
};

//********************************
struct MessTimeOfDay : public MessBase
{
   MessTimeOfDay(void) {idTag = NWMESS_TIME_OF_DAY; };

   DWORD value;
};

//********************************
struct MessWeatherState : public MessBase
{
   MessWeatherState(void) {idTag = NWMESS_WEATHER_STATE; };

   int value;
};

//********************************
struct MessSecurePartnerName : public MessBase
{
   MessSecurePartnerName(void) {idTag = NWMESS_SECURE_PARTNER_NAME; };

    char name[32];
};

//********************************
struct MessGroundEffect : public MessBase
{
   MessGroundEffect(void) {idTag = NWMESS_GROUND_EFFECT; };

    unsigned long mobID;
    unsigned char type;
    unsigned char r,g,b;
    unsigned char x,y;
    int amount;
};

//********************************
struct MessMonsterSpecialAttack : public MessBase
{
   MessMonsterSpecialAttack(void) {idTag = NWMESS_MONSTER_SPECIAL_ATTACK; };

    unsigned long mobID;
    int type;
};

//********************************
struct MessMonsterChangeForm : public MessBase
{
   MessMonsterChangeForm(void) {idTag = NWMESS_MONSTER_CHANGE_FORM; };

    unsigned long mobID;
    int form;
};











#pragma pack(pop)
#endif
