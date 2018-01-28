#ifndef SHARED_SPACE_H
#define SHARED_SPACE_H

#include ".\helper\linklist.h"
#include "BBO.h"
#include "inventory.h"
#include "MapList.h"
#include ".\network\NetWorldMessages.h"


class BBOSAvatar;
class BBOSMonster;

enum
{
	SPACE_GROUND,
	SPACE_DUNGEON,
	SPACE_LABYRINTH,
	SPACE_GUILD,
	SPACE_REALM,
	SPACE_MAX

};


class SharedSpace : public DataObject
{
public:
	SharedSpace(int doid, char *doname,	NetWorldRadio *lserver);
	virtual ~SharedSpace();

	virtual void InitNew(int w, int h, int eX, int eY);
	virtual void Save(void);
	virtual void Load(void);
//	virtual void Tick(DWORD now);

	virtual int CanMove(int srcX, int srcY, int dstX, int dstY);

	void SendToEveryoneBut(int handleToExclude, int size, const void *dataPtr);
	void SendToEveryoneNearBut(int handleToExclude, float x, float y, int size, 
		                        const void *dataPtr, int radius = 5, unsigned long flags = 0);
	void IgnorableSendToEveryone(BBOSAvatar *srcAvatar, int size, const void *dataPtr);
	void IgnorableSendToEveryoneNear(BBOSAvatar *srcAvatar, float x, float y, int size, 
		                        const void *dataPtr, int radius = 5);
	void SendToEveryFriend(BBOSAvatar *srcAvatar, int size, const void *dataPtr);


	virtual void DoMonsterDrop(Inventory *inv, BBOSMonster *monster);
	virtual void DoMonsterDropSpecial(Inventory *inv, BBOSMonster *monster, int type);
	virtual void DoMonsterDrop(BBOSMonster *monster) = NULL;
	virtual void DoMonsterDropSpecial(BBOSMonster *monster, int type) = NULL;
	void         DropItemToProperPlace(Inventory *inv, 
													 BBOSMonster *monster, 
													 InventoryObject *iObject,
													 float coeff);

	virtual Inventory *GetGroundInventory(int x, int y) = NULL;

	int SX(int realX);
	int SY(int realY);

	int sizeX, sizeY;

	DoublyLinkedList *avatars, *generators;
	MapList *mobList;
	NetWorldRadio *	lserver;

	Inventory *groundInventory;

};


extern char dropAdj[10][32];
extern char dropAdj2[11][32];
extern char dropName[10][32];

#endif
