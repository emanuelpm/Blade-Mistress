#ifndef REALM_MAP_H
#define REALM_MAP_H

#include "SharedSpace.h"

class RealmMap : public SharedSpace
{
public:
	RealmMap(int doid, char *doname, NetWorldRadio * lserver);
	virtual ~RealmMap();

	void InitNew(char *terrainDataFile, int w, int h, int eX, int eY);
	void Save(void);
	void Load(void);
	int CanMove(int srcX, int srcY, int dstX, int dstY);
	int Color    (int x, int y);

	virtual void DoMonsterDrop(BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type);

	virtual Inventory *GetGroundInventory(int x, int y);

	int width, height;
	int enterX, enterY;
	int type;

	unsigned char *map;
};

#endif
