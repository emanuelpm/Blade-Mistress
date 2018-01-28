#ifndef TRADERGOODS_H
#define TRADERGOODS_H

#include "./helper/linklist.h"
#include "BBO.h"
#include "inventory.h"

class BBOSNpc;

//******************************************************************
class TraderGoods
{
public:

	TraderGoods();
	virtual ~TraderGoods();
	void Replenish(BBOSNpc *npc);

	DoublyLinkedList objects;

};

#endif
