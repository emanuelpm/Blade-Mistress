#ifndef BBO_SARMY_DRAGONS_H
#define BBO_SARMY_DRAGONS_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyDragons : public BBOSArmy
{
public:

	ArmyDragons(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numHalloween);
	virtual ~ArmyDragons();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
