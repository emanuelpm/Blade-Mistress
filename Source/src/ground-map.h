#ifndef GROUND_MAP_H
#define GROUND_MAP_H

#include "SharedSpace.h"

class GroundMap : public SharedSpace
{
public:
	GroundMap(int doid, char *doname, NetWorldRadio * lserver);
	virtual ~GroundMap();

	void InitNew(int w, int h, int eX, int eY);
	void Save(void);
	void Load(void);
	int CanMove(int srcX, int srcY, int dstX, int dstY);
	int Color    (int x, int y);

	virtual void DoMonsterDrop(BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type);

	virtual Inventory *GetGroundInventory(int x, int y);

	unsigned char map[MAP_SIZE_WIDTH][MAP_SIZE_HEIGHT];
//	long toughestMonsterPoints[MAP_SIZE_WIDTH][MAP_SIZE_HEIGHT];
};

#endif
