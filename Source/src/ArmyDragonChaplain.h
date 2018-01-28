#ifndef BBO_SARMY_CHAPLAIN_H
#define BBO_SARMY_CHAPLAIN_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyChaplain : public BBOSArmy
{
public:

	ArmyChaplain(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numChaplain);
	virtual ~ArmyChaplain();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

	void KillMember(ArmyMember * curMember, DoublyLinkedList *srcList);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
