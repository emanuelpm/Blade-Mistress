#ifndef BBO_SARMY_DEAD_H
#define BBO_SARMY_DEAD_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyDead : public BBOSArmy
{
public:

	ArmyDead(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numHalloween);
	virtual ~ArmyDead();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
