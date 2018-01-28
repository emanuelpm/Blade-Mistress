#ifndef ROBOT_H
#define ROBOT_H

//***************************************************************
// base class for a game mode which likes to belong to a stack.
//***************************************************************

#include ".\helper\dataobje.h"
#include ".\helper\linklist.h"
#include ".\network\client.h"
#include "clientOnly.h"

class Robot : public DataObject
{
public:

	Robot(int doid, char *doname);
	virtual ~Robot();

	virtual void Tick(void);

	Client *	lclient;

	int state, stateTimer, avatarID;

};


extern DoublyLinkedList robotList;

#endif
