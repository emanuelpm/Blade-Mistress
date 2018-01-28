#ifndef FLOCKPOINTS_H
#define FLOCKPOINTS_H

#include ".\helper\linklist.h"

//******************************************************************
class FlockPoint : public DataObject
{
public:

	FlockPoint(int doid, char *doname);
	virtual ~FlockPoint();

	float x,y; // cell location
	float targetX, targetY, targetAngle; // cell location
	float angle, maxSpeed, maxTurn;
	float size, lastDistMovedSQR;
	int owned, isClose, isCaring;
	void *ownerPtr;

};

//******************************************************************
class FlockSet
{
public:

	FlockSet(void);
	virtual ~FlockSet();

	unsigned long        MakePoint(void *owner);
	FlockPoint *GetPoint(unsigned long index, void *owner);
	void       FreePoint(unsigned long index, void *owner);

	void Tick(float timeDelta, float targetX, float targetY);

	DoublyLinkedList *points;
};

#endif
