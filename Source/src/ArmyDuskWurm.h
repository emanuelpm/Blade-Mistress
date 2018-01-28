#ifndef BBO_SARMY_DUSKWURM_H
#define BBO_SARMY_DUSKWURM_H

#include "BBO-Sarmy.h"

//******************************************************************
class ArmyDuskWurm : public BBOSArmy
{
public:

	ArmyDuskWurm(SharedSpace *s, 
		         int centerX, int centerY, 
					int spawnX , int spawnY, 
					int numHalloween);
	virtual ~ArmyDuskWurm();
	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

//	virtual void Tick(SharedSpace *ss);

//	virtual void MonsterEvent(SharedSpace *ss, BBOSMonster *theMonster, int eventType);
};

#endif
