#ifndef BBO_SARMY_ARCHMAGE_H
#define BBO_SARMY_ARCHMAGE_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyArchMage : public BBOSArmy
{
public:

	ArmyArchMage(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numArchMage);
	virtual ~ArmyArchMage();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	void KillMember(ArmyMember * curMember, DoublyLinkedList *srcList);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
