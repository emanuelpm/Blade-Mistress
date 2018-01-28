#ifndef BBO_SBOMB_H
#define BBO_SBOMB_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSAvatar;

class BBOSBomb : public BBOSMob
{
public:

	BBOSBomb(BBOSAvatar *dropper);
	virtual ~BBOSBomb();
	void Tick(SharedSpace *ss);

	float power;
	int type;
	unsigned long flags;
	int r,g,b;
	DWORD detonateTime;

	BBOSAvatar *dropper;
};

#endif
