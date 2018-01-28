#ifndef DUNGEON_MAP_H
#define DUNGEON_MAP_H

#include "SharedSpace.h"

const int SPECIAL_DUNGEON_NOT        = 0x00;
const int SPECIAL_DUNGEON_MODERATED  = 0x01;
const int SPECIAL_DUNGEON_TEMPORARY	 = 0x02;

class BBOSChest;

class DungeonMonsterRecord
{
public:
	unsigned char x,y;
	unsigned char type,subType;
};					

class DungeonMap : public SharedSpace
{
public:
	DungeonMap(int doid, char *doname, NetWorldRadio * lserver);
	virtual ~DungeonMap();

	void InitNew(int w, int h, int eX, int eY, int rating);
	void Save(FILE *fp);
	void Load(FILE *fp, float version);
	int CanMove(int srcX, int srcY, int dstX, int dstY);
	int SetPathMap(void);
	int ChangeWall(int isLeft, int x, int y);
	int NewPathMapValue(int curValue, int neighborValue);
	int ForceDungeonContiguous(void);

	virtual void DoMonsterDrop(BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type);
	void DoChestDrop(BBOSChest *chest);

	virtual Inventory *GetGroundInventory(int x, int y);

	void Randomize(int ratio);
	int  CanEdit(BBOSAvatar *ca);

	int width, height;
	int enterX, enterY;
	unsigned char *leftWall, *topWall;
	unsigned char floorIndex, outerWallIndex;
	int *pathMap;

	char name[64];

	char masterName[64];
	char masterPass[64];
	int  specialFlags;
	LongTime masterTimeout;

	DungeonMonsterRecord *records;
	int dungeonRating; // 0-4, corresponds to monster.dungeonType

	float tempPower;
	int isLocked;

};

extern char rdNamePre      [7][64];
extern char rdNameAdjective[5][64];


#endif
