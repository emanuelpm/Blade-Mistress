#ifndef BBO_SCHAINQUEST_H
#define BBO_SCHAINQUEST_H

#include "BBO-SAutoQuest.h"

//******************************************************************
class BBOSChainQuest : public BBOSAutoQuest
{
public:

	BBOSChainQuest(SharedSpace *ss);
	virtual ~BBOSChainQuest();
	virtual void Tick(SharedSpace *unused);
	virtual void MonsterEvent(BBOSMonster *theMonster, int eventType, int x = -10, int y = -10);

	void CreateMonster(void);

	int monsterType;
	float monsterPower;




};

#endif
