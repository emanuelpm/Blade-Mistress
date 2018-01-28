#ifndef BBO_SGENERATOR_H
#define BBO_SGENERATOR_H

//#include "pumamesh.h"
#include "BBO-Smonster.h"
#include "BBO.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

const int MONSTER_SPAWN_RANGE = 15;

class BBOSGenerator : public DataObject
{
public:

	BBOSGenerator(int x, int y);
	virtual ~BBOSGenerator();
	virtual void Tick(SharedSpace *ss);
	virtual void FindMonsterPoint(int town, int &mx, int &my, int townDist);

	int count[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES], 
		   max[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];

	int x, y;
	DWORD lastSpawnTime;
};

#endif
