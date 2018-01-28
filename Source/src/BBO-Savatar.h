#ifndef BBO_SAVATAR_H
#define BBO_SAVATAR_H

#include "BBO-Smob.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "totemData.h"
#include "sharedSpace.h"
#include "QuestSystem.h"

class BBOSMonster;

enum
{
    SAMARITAN_REL_OLDER,
    SAMARITAN_REL_PEER,
    SAMARITAN_REL_YOUNGER,
    SAMARITAN_REL_GUILD,
    SAMARITAN_REL_MAX
};

enum
{
    SAMARITAN_TYPE_THANKS,
    SAMARITAN_TYPE_PLEASE,
    SAMARITAN_TYPE_WELCOME,
    SAMARITAN_TYPE_GIFT,
    SAMARITAN_TYPE_CASH,
    SAMARITAN_TYPE_MAX
};

enum
{
    ACCOUNT_TYPE_PLAYER = 0,
    ACCOUNT_TYPE_ADMIN = 1,
    ACCOUNT_TYPE_MODERATOR = 2,
    ACCOUNT_TYPE_TRIAL_MODERATOR = 3,
    ACCOUNT_TYPE_HIDDEN_ADMIN = 4,
    ACCOUNT_TYPE_BANNED = 10,
    ACCOUNT_TYPE_MAX
};

enum
{
    AVATAR_STATUS_AVAILABLE = 0,
    AVATAR_STATUS_AFK,
    AVATAR_STATUS_BUSY
};

const float CLEVEL_VAL_DODGE        = 0.6f;
const float CLEVEL_VAL_MAGIC        = 0.1f;
const float CLEVEL_VAL_SHATTER		= 0.05f;
const float CLEVEL_VAL_SMITH        = 0.03f;
const float CLEVEL_VAL_BOMB         = 0.1f;
const float CLEVEL_VAL_SMITH_EXPERT = 0.2f;

const int QUEST_SLOTS = 6;

// Thank You's received from Elder Players
// Thank You's received from Peers
// Thank You's received from Younger Players
// Thank You's received from Guildies
//  
// Thank You's sent to Elder Players
// Thank You's sent to Peers
// Thank You's sent Younger Players
// Thank You's sent to Guildies
//  
// Your Welcomes received from Elder Players
// Your Welcomes received from Peers
// Your Welcomes received from Younger Players
// Your Welcomes received from Guildies
//  
// Your Welcomes sent to Elder Players
// Your Welcomes sent to Peers
// Your Welcomes sent to Younger Players
// Your Welcomes sent to Guildies
//  
// Charitable Trades sent to Elder Players
// Charitable Trades sent to Peers
// Charitable Trades sent to Younger Players
// Charitable Trades sent to Guildies
//  
// Charitable Trades received from Elder Players
// Charitable Trades received from Peers
// Charitable Trades received from Younger Players
// Charitable Trades received from Guildies
 


//******************************************************************
class SkillObject : public DataObject
{
public:

    SkillObject(int doid, char *doname);  // doid = level of skill, doname = skill type
    virtual ~SkillObject();
    long exp, expUsed;
};

//******************************************************************
struct PetDragonInfo
{
    char name[16];
    unsigned char type, quality, state, lifeStage; // type = -1, means no dragon here.
    DWORD age; // 5 minute increments
    unsigned char lastEatenType, lastEatenSubType;
    DWORD lastAttackTime;
    float healthModifier;
    LongTime lastEatenTime;
};

//******************************************************************
struct BBOSCharacterInfo
{
    BBOSCharacterInfo(void);
    ~BBOSCharacterInfo();


    char name[32];
    int faceIndex, topIndex, bottomIndex;
    unsigned char hairR  , hairG  , hairB;   // hair
    unsigned char topR   , topG   , topB;    // hair
    unsigned char bottomR, bottomG, bottomB; // hair
    Inventory *inventory, *workbench, *skills, *wield;

    long physical, magical, creative;
    long healthMax; // max = physical * 10 + (long)cLevel;
    long health;
    float cLevel, oldCLevel;

    int lastX, lastY, spawnX, spawnY;
    unsigned short imageFlags;
    long lifeTime; // in 5 minute increments

    LongTime lastSavedTime;

    PetDragonInfo petDragonInfo[2];

    int karmaGiven   [SAMARITAN_REL_MAX][SAMARITAN_TYPE_MAX];
    int karmaReceived[SAMARITAN_REL_MAX][SAMARITAN_TYPE_MAX];

    Quest quests[QUEST_SLOTS];
    int witchQuestIndex;
    char witchQuestName[64];

    int age;


};
/*
struct BBOSCharacterInfo
{
    char name[32];
    int artIndex;
    unsigned char hairR, hairG, hairB; // hair
    Inventory inventory, workbench, skills, wield;

    long physical, magical, creative;
    long health, healthMax; // max = physical * 10;
};
*/



//******************************************************************
class BBOSAvatar : public BBOSMob
{
public:

    BBOSAvatar(void);
    virtual ~BBOSAvatar();
    void Tick(SharedSpace *ss);
    int  LoadAccount(char *name, char *pass, int isNew, int justLoad = FALSE);
    void SaveAccount(void);
    void LoadContacts(FILE *fp, float version);
    void SaveContacts(FILE *fp);
    void BuildStatsMessage(MessAvatarStats *mStats);
    void GiveInfoFor(int x, int y, SharedSpace *ss);
    void UpdateClient(SharedSpace *ss, int clearAll = FALSE);
    void IntroduceMyself(SharedSpace *ss, unsigned short special = 0);
    int  GetDodgeLevel(void);
    void MakeCharacterValid(int i);

    void Combine(SharedSpace *ss);
    int  DetectSize(char *string, int type);
    void DetectIngotTypes(InvBlade *ib, int &type1, int &type2);

    void HandleMeatRot(SharedSpace *ss);
    int  HandleOneMeatRot(InventoryObject *iObject);
    void PetAging(SharedSpace *ss);
    void AnnounceDisappearing(SharedSpace *sp, int type);
    void AnnounceSpecial(SharedSpace *sp, int type);

    int  IsContact(char *name, int type);
    void CompareWith(BBOSAvatar *other, int &myRelationship, int &otherRelationship);
    int  IsAGuildMate(BBOSAvatar *other);
    void LogKarmaExchange(BBOSAvatar *receiver, 
                                             int myRel, int receiverRel, int exchangeType, 
                                             char *originalText = NULL);

    void MoveControlledMonster(SharedSpace *ss, int deltaX, int deltaY);

    void CompleteSecureTrading(SharedSpace *ss);
    void AbortSecureTrading(SharedSpace *ss);
    void StateNoAgreement(SharedSpace *ss);
    void AssertGuildStatus(SharedSpace *ss, int full = FALSE, int socketTarget = 0);
    void UseStaffOnMonster(SharedSpace *ss, InvStaff *iStaff, BBOSMonster *curMonster);
    void DoBladestaffExtra(SharedSpace *ss, InvBlade *ib, 
                                              long damValue, BBOSMonster *targetMonster);
    
    long  InventoryValue(void);
    float BestSwordRating(void);

    void QuestMovement   (SharedSpace *ss);
    void QuestTime       (SharedSpace *ss);
    void QuestTalk       (SharedSpace *ss);
    void QuestCraftWeapon(SharedSpace *ss, InventoryObject *io, 
                                 float challenge, float work, char *skillName);
    void QuestImbue      (SharedSpace *ss, InventoryObject *io, 
                                 float challenge, float work, char *skillName);
    void QuestMonsterKill(SharedSpace *ss, BBOSMonster *deadMonster);
    void QuestGiveGold   (SharedSpace *ss, BBOSAvatar *other, long amount);
    void QuestPickupItem (SharedSpace *ss, InventoryObject *io);
    void QuestPrayer     (SharedSpace *ss);
    void QuestSpaceChange(SharedSpace *spaceFrom, SharedSpace *spaceTo);
    int  QuestReward     (SharedSpace *ss);

    int  SacrificeQuestItem(int type, int subType);
    int  SacrificeQuestItemFrom(int type, int subType, Inventory *inv);

    int PhysicalStat(void);
    int MagicalStat (void);
    int CreativeStat(void);

    void AddMastery( SharedSpace *ss );
    int GetMasteryForType( int type );

    void SetUniqueId(int id) {uniqueId = id;}
    int GetUniqueId() {return uniqueId;}

    int socketIndex;
    int curCharacterIndex;
    char name[NUM_OF_CHARS_FOR_USERNAME];
    char pass[NUM_OF_CHARS_FOR_PASSWORD];
    int passLen;
    unsigned long infoFlags, chatChannels;
    unsigned char IP[4];

    DWORD lastSaveTime, lastHealTime, lastTenTime, combineStartTime, lastMinuteTime;
    int activeCounter;
    int isCombining;
    char combineSkillName[64];

    BBOSCharacterInfo charInfoArray[NUM_OF_CHARS_PER_USER];

    char updateMap[MAP_SIZE_WIDTH*MAP_SIZE_HEIGHT];	 // turn this into a bit field and SAVE MEMORY!

    BBOSMonster *curTarget;

    char lastTellName[32];

    int lastPlayerInvSent, lastOtherInvSent;
    int indexList[MESS_INVENTORY_MAX];

    TotemEffects totemEffects;

    int accountType, isInvisible, kickOff, isInvulnerable, hasPaid;

    int timeOnCounter, kickMeOffNow;

    int specLevel[3];

    int chantType;
    DWORD chantTime;

    DoublyLinkedList *contacts;

    BBOSMonster *controlledMonster;

    Inventory *trade, *bank;
    BBOSAvatar *tradingPartner;
    int agreeToTrade;

    LongTime accountExperationTime;
    LongTime accountRestrictionTime;
    int restrictionType;

    DWORD moveTimeCost;

    int isReferralDone, patronCount;

    int uniqueId;

    bool bHasLoaded;

    // For player status
    int status;
    char status_text[145];

};

#endif
