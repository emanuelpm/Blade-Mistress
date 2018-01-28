#ifndef BBO_SCHEST_H
#define BBO_SCHEST_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include "BBO-Smonster.h"
#include "BBO.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSChest : public BBOSMob
{
public:

	BBOSChest(int x, int y);
	virtual ~BBOSChest();
	void Tick(SharedSpace *ss);

	int x, y;
	unsigned char isOpen;
	SharedSpace *ss;
	DWORD openTime;
};

#endif
