#ifndef QUEST_SYSTEM_H
#define QUEST_SYSTEM_H

#include ".\helper\linklist.h"
#include "BBO.h"
#include "inventory.h"

class BBOSAvatar;
class BBOSMonster;

enum
{
	QUEST_PART_TARGET,
	QUEST_PART_VERB,
	QUEST_PART_TIME,
	QUEST_PART_MODIFIER,
	QUEST_PART_MAX
};

enum
{
	QUEST_TARGET_MONSTER_TYPE,
	QUEST_TARGET_LOCATION,
	QUEST_TARGET_SPACE,
	QUEST_TARGET_PLAYER,
	QUEST_TARGET_WEAPON,
	QUEST_TARGET_TOTEM,
	QUEST_TARGET_STAFF,
	QUEST_TARGET_NPC,
	QUEST_TARGET_EGG,
	QUEST_TARGET_DUST,
	QUEST_TARGET_MAX
};

enum
{
	QUEST_VERB_KILL,
	QUEST_VERB_GOTO,
	QUEST_VERB_GIVEGOLD,
	QUEST_VERB_VISIT,
	QUEST_VERB_CRAFT,
	QUEST_VERB_IMBUE,
	QUEST_VERB_RETRIEVE,
	QUEST_VERB_COLLECT,
	QUEST_VERB_GROUP,
	QUEST_VERB_ESCAPE,
	QUEST_VERB_REWARD,
	QUEST_VERB_MAX
};

enum
{
	QUEST_PLAYER_TYPE_FIGHTER,
	QUEST_PLAYER_TYPE_MAGE,
	QUEST_PLAYER_TYPE_CRAFTER,
	QUEST_PLAYER_TYPE_BALANCED,
	QUEST_PLAYER_TYPE_YOUNG,
	QUEST_PLAYER_TYPE_POOR,
	QUEST_PLAYER_TYPE_MAX
};

enum
{
	QUEST_WEAPON_TYPE_SWORD,
	QUEST_WEAPON_TYPE_KATANA,
	QUEST_WEAPON_TYPE_MACE  ,
	QUEST_WEAPON_TYPE_CHAOS ,
	QUEST_WEAPON_TYPE_CLAWS ,
	QUEST_WEAPON_TYPE_DOUBLE,
//	QUEST_WEAPON_TYPE_STAFF1,
	QUEST_WEAPON_TYPE_MAX
};

enum
{
	QUEST_REWARD_TYPE_GOLD,
	QUEST_REWARD_TYPE_STAFF,
	QUEST_REWARD_TYPE_TOTEM,
	QUEST_REWARD_TYPE_DUST,
	QUEST_REWARD_TYPE_INGOT,
	QUEST_REWARD_TYPE_EGG,
	QUEST_REWARD_TYPE_FAVOR,
	QUEST_REWARD_TYPE_EXPLOSIVE,
	QUEST_REWARD_TYPE_FUSE,
	QUEST_REWARD_TYPE_MAX
};

//*****************************************************************
class QuestPart : public DataObject
{
public:
	QuestPart(int type, char *doname);
	virtual ~QuestPart();

	int type;
	int monsterType, monsterSubType;
	int x,y, range, mapType, mapSubType;
	int playerType;
	char otherName[64];
	InventoryObject *object;
};

//*****************************************************************
class Quest : public DataObject
{
public:
	Quest(void);
	virtual ~Quest();
	void EmptyOut(void);

	void ShortDesc(char *buffer, int goLong = FALSE);
	void LongDesc (char *buffer);
	void Load(FILE *fp, float version);
	void Save(FILE *fp);

	void CopyTo(Quest *dest);

	QuestPart   *GetVerb  (void);
	QuestPart   *GetTarget(void);
	SharedSpace *GetDungeonOfQuest(QuestPart *target);
	int          CheckValidity(void);

	DoublyLinkedList parts;
	LongTime timeLeft;
	int completeVal; // -1 means no quest here
	int questSetVal; // -1 means not part of a quest set
	unsigned long questSource; // which tree, or NPC
	char *questText;
};


//*****************************************************************
struct QuestPartDesc
{
	char tagName[32];
	char shortText[64];
	char longText[128];
};


//*****************************************************************
class QuestScriptSet : public DataObject
{
public:
	QuestScriptSet(int type = 0, char *doname = "QUEST_SCRIPT_SET");
	virtual ~QuestScriptSet();

	DoublyLinkedList quests;
};

//*****************************************************************
class QuestWitch : public DataObject
{
public:
	QuestWitch(int type = 0, char *doname = "QUEST_WITCH");
	virtual ~QuestWitch();

	DoublyLinkedList questSets;
	unsigned long witchID;
	char statement[1024];
	int isOpen;
};

//*****************************************************************
class QuestManager
{
public:
	QuestManager();
	virtual ~QuestManager();

	void Load(void);
	void HandleQuestLine(char *tempText, Quest *q);

	void WitchTalk(BBOSNpc *witch, SharedSpace *sp);
	void WitchGiveQuest(BBOSNpc *witch, BBOSAvatar *curAvatar, SharedSpace *sp);
	void ProcessWitchQuest(BBOSAvatar *curAvatar, SharedSpace *sp, Quest *destQ);

	DoublyLinkedList sets;
};



extern char questPlayerTypeDesc  [QUEST_PLAYER_TYPE_MAX][64];
extern char questWeaponTypeDesc  [QUEST_WEAPON_TYPE_MAX][64];
extern char questRetrieveTypeDesc[MAGIC_MAX][64];

extern QuestManager *questMan;


#endif
