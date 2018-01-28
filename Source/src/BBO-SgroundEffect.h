#ifndef BBO_SGROUNDEFFECT_H
#define BBO_SGROUNDEFFECT_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

class BBOSAvatar;

class BBOSGroundEffect : public BBOSMob
{
public:

	BBOSGroundEffect(void);
	virtual ~BBOSGroundEffect();
	void Tick(SharedSpace *ss);

	int type, amount;
	int r,g,b;
};

#endif
