#ifndef BBO_STOWER_H
#define BBO_STOWER_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include "BBO-Smonster.h"
#include "BBO.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSTower : public BBOSMob
{
public:

	BBOSTower(int x, int y);
	virtual ~BBOSTower();
	void Tick(SharedSpace *ss);

	int x, y;
	SharedSpace *ss;
	int isGuildTower;
};

#endif
