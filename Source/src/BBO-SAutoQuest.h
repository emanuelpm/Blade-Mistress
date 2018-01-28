#ifndef BBO_SAUTOQUEST_H
#define BBO_SAUTOQUEST_H

#include "BBO-Sgenerator.h"

enum
{
	AUTO_EVENT_ATTACKED,
	AUTO_EVENT_DIED,
	AUTO_EVENT_MAX
};

//******************************************************************
class BBOSAutoQuest : public BBOSGenerator
{
public:

	BBOSAutoQuest(SharedSpace *ss);
	virtual ~BBOSAutoQuest();
	virtual void Tick(SharedSpace *unused);
	virtual void MonsterEvent(BBOSMonster *theMonster, int eventType, int x = -10, int y = -10);

	SharedSpace *ss;

	int totalMembersKilled;
	int totalMembersBorn;

	int questState;
	long questCounter;
	DWORD lastTickTime;

};

#endif
