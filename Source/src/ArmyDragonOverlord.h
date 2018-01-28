#ifndef BBO_SARMY_OVERLORD_H
#define BBO_SARMY_OVERLORD_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyOverlord : public BBOSArmy
{
public:

	ArmyOverlord(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numOverlord);
	virtual ~ArmyOverlord();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	void KillMember(ArmyMember * curMember, DoublyLinkedList *srcList);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
