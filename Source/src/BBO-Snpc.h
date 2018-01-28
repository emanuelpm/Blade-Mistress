#ifndef BBO_SNPC_H
#define BBO_SNPC_H

#include "BBO-Smob.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "tradergoods.h"
#include "sharedSpace.h"

class BBOSNpc : public BBOSMob
{
public:

	BBOSNpc(int npcType = SMOB_TRADER);
	virtual ~BBOSNpc();
	void Tick(SharedSpace *ss);
	int  PlayerInMySquare(SharedSpace *ss);

	Inventory *inventory;
	int level, townIndex;
	DWORD lastReplenishTime;

	static TraderGoods *goods;
	static int goodsRefCount;

};

#endif
