#ifndef MAPLIST_H
#define MAPLIST_H

#include ".\helper\linklist.h"
#include "BBO.h"

class BBOSMob;

struct MapListState
{
	int searchX, searchY, searchRadius, curRowIndex;
	BBOSMob *curMob;
};

class MapList
{
public:
	MapList(int w, int h);
	virtual ~MapList();

	void Add(BBOSMob *mob);
	void Remove(BBOSMob *mob);

	void Move(BBOSMob *mob);

	void Tick(void);

	BBOSMob *GetFirst(int x, int y, int radius = 0);
	BBOSMob *GetNext (void);
	void SetToLast(void);
	BBOSMob *IsInList(BBOSMob *target, int tryAll = FALSE);

	MapListState GetState(void);
	void SetState(MapListState heldState);

	DoublyLinkedList *rows;
	int rowW, rowH;

	MapListState state;

	int curSortRow;

};


#endif
