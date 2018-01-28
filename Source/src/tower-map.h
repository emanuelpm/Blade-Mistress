#ifndef TOWER_MAP_H
#define TOWER_MAP_H

#include "SharedSpace.h"

enum
{
	GUILDSTYLE_NONE,
	GUILDSTYLE_CAUCUS,
	GUILDSTYLE_COUNCIL, 
	GUILDSTYLE_VESTRY, 
	GUILDSTYLE_MONARCHY,
	GUILDSTYLE_TYRANNY,
	GUILDSTYLE_MAX
};

enum
{
	GUILDBILL_INACTIVE,
	GUILDBILL_PROMOTE,
	GUILDBILL_KICKOUT,
	GUILDBILL_CHANGESTYLE,
	GUILDBILL_CHANGENAME,
	GUILDBILL_FIGHTER_SPEC,
	GUILDBILL_MAGE_SPEC,
	GUILDBILL_CRAFTER_SPEC,
	GUILDBILL_DEMOTE,
	GUILDBILL_MAX
};

enum
{
	VOTESTATE_VOTING,
	VOTESTATE_PASSED,
	VOTESTATE_FAILED,
	VOTESTATE_MAX
};

//***************************************************************************************
class MemberRecord : public DataObject
{
public:
	MemberRecord(int doid, char *doname);
	virtual ~MemberRecord();

	int value1, value2;
};

//***************************************************************************************
struct Bill
{
   LongTime expTime;
   int voteState;
   int type; // also used to signal an empty (unused) bill structure
   int sponsorLevel;
   char subject[64], sponsor[64];

   DoublyLinkedList recordedVotes;
};

//***************************************************************************************
class TowerMap : public SharedSpace
{
public:
	TowerMap(int doid, char *doname, NetWorldRadio * lserver);
	virtual ~TowerMap();

	void InitNew(int w, int h);
	void Save(FILE *fp);
	void Load(FILE *fp, float version);
	int CanMove(int srcX, int srcY, int dstX, int dstY);
	int SetPathMap(void);
	int ChangeWall(int isLeft, int x, int y);
	int NewPathMapValue(int curValue, int neighborValue);
	int ForceDungeonContiguous(void);

	virtual Inventory *GetGroundInventory(int x, int y);

	virtual void DoMonsterDrop(BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type);

	int  IsMember(char *avatarName);

	void ProcessVotes(void);

	int width, height;
	int enterX, enterY;
	unsigned char *leftWall, *topWall;
	unsigned char floorIndex, outerWallIndex;
	int *pathMap;

//	char name[64];

	DoublyLinkedList *members;  // MemberRecords
	LongTime lastChangedTime;

	int guildStyle;
//   int guildSpecType, specTypeLevel;
   int specLevel[3];

   Bill bills[4]; // guildStyle determines who gets to sponsor which bill

	Inventory *itemBox;
};

extern char guildStyleNames[GUILDSTYLE_MAX][32];
extern char guildBillNames[GUILDBILL_MAX][32];
extern char guildBillDesc[GUILDBILL_MAX][256];
extern int  commonVotesAllowed[GUILDSTYLE_MAX-1];
extern char guildLevelNames[GUILDSTYLE_MAX-1][3][32];

#endif
