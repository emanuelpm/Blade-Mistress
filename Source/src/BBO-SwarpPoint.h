#ifndef BBO_SWARPPOINT_H
#define BBO_SWARPPOINT_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include "BBO-Smonster.h"
#include "BBO.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSWarpPoint : public BBOSMob
{
public:

	BBOSWarpPoint(int x, int y);
	BBOSWarpPoint(int x, int y, int type );
	virtual ~BBOSWarpPoint();
	void Tick(SharedSpace *ss);

	int targetX, targetY;
	int spaceType, spaceSubType, allCanUse;
};

#endif
