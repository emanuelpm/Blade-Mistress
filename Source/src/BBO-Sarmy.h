#ifndef BBO_SARMY_H
#define BBO_SARMY_H

#include "BBO-Sgenerator.h"

enum
{
	ARMY_EVENT_ATTACKED,
	ARMY_EVENT_DIED,
	ARMY_EVENT_MAX
};

extern int spaceOffset[8][2];

//******************************************************************
class ArmyMember : public DataObject
{
public:

	ArmyMember(void);
	virtual ~ArmyMember();

	int quality;
	BBOSMonster *monster;
	int type, subType;
	int targetX, targetY;
	DWORD deadTimer, monsterCreationTime;
	int isDead;

};

//******************************************************************
class BBOSArmy : public BBOSGenerator
{
public:

	BBOSArmy(SharedSpace *ss, int centerX, int centerY);
	virtual ~BBOSArmy();
	virtual void Tick(SharedSpace *unused);

	virtual void MonsterEvent(BBOSMonster *theMonster, int eventType, int x = -10, int y = -10);

	virtual BBOSMonster *MakeSpecialMonster(ArmyMember *member);

	ArmyMember * ClosestMember(int x, int y, DoublyLinkedList *srcList,
		                        int highestLevel = 1000);
	ArmyMember * FindMemberByMonster(BBOSMonster *monster,
														 DoublyLinkedList **srcList);
	virtual void KillMember(ArmyMember * curMember, DoublyLinkedList *srcList);

	int IsValidMonster(BBOSMonster *monster, ArmyMember *curMember);

	void ResurrectMonster(ArmyMember *curMember);

	DoublyLinkedList atEase, focus[3];
	int focusX[3];
	int focusY[3];
	int focusValue[3];
	DWORD lastFocusEventTime[3];

	int spawnX, spawnY;
	int centerX, centerY;

	SharedSpace *ss;

	int totalArmyMembersKilled;
	int totalArmyMembersBorn;
	int totalArmyMembersInLists;


};

#endif
