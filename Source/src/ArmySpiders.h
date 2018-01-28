#ifndef BBO_SARMY_SPIDERS_H
#define BBO_SARMY_SPIDERS_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmySpiders : public BBOSArmy
{
public:

	ArmySpiders(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numSpiders);
	virtual ~ArmySpiders();
//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
