#ifndef LABYRINTH_MAP_H
#define LABYRINTH_MAP_H

#include "SharedSpace.h"

class LabyrinthMap : public SharedSpace
{
public:
	LabyrinthMap(int doid, char *doname, NetWorldRadio * lserver);
	virtual ~LabyrinthMap();

	void InitNew(char *terrainDataFile, int w, int h, int eX, int eY);
	void Save(void);
	void Load(void);
	int CanMove(int srcX, int srcY, int dstX, int dstY);
	int Color    (int x, int y);

	virtual void DoMonsterDrop(BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type);

	virtual Inventory *GetGroundInventory(int x, int y);

	void UpdateMonsterMap(void);

	int width, height;
	int enterX, enterY;
	int type;

	unsigned char *map;

	float *monsterMap;
};

#endif
