#ifndef BBO_SARMY_HALLOWEEN_H
#define BBO_SARMY_HALLOWEEN_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyHalloween : public BBOSArmy
{
public:

	ArmyHalloween(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numHalloween);
	virtual ~ArmyHalloween();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
