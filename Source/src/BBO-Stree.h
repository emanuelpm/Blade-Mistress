#ifndef BBO_STREE_H
#define BBO_STREE_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include "BBO-Smonster.h"
#include "BBO.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSTree : public BBOSMob
{
public:

	BBOSTree(int x, int y);
	virtual ~BBOSTree();
	void Tick(SharedSpace *ss);

	int index;
//	unsigned char isOpen;
//	SharedSpace *ss;
//	DWORD openTime;
};

#endif
