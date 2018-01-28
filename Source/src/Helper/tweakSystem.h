#ifndef TWEAK_SYSTEM_H
#define TWEAK_SYSTEM_H

#include "linklist.h"

//***************************************************************************************
class TweakObject : public DataObject
{
public:

	TweakObject(int doid, char *doname);
	virtual ~TweakObject();

	float value, low, high;
	char description[128];

};

//***************************************************************************************
class TweakSystem
{
public:

	TweakSystem(void);
	virtual ~TweakSystem();

	float GetTweak(char *tag, float def, float low, float high, char *desc);

	DoublyLinkedList tweakObjects;

};

extern TweakSystem gTweakSystem;



#endif
