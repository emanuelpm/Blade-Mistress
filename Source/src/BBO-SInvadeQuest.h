#ifndef BBO_SINVADEQUEST_H
#define BBO_SINVADEQUEST_H

#include "BBO-SAutoQuest.h"

//******************************************************************
class InvadeQuestMember : public DataObject
{
public:

	InvadeQuestMember(void);
	virtual ~InvadeQuestMember();

	int quality;
	BBOSMonster *monster;
	int type, subType;
	int targetX, targetY;
	DWORD deadTimer, monsterCreationTime;
	int isDead;

};

//******************************************************************
class BBOSInvadeQuest : public BBOSAutoQuest
{
public:

	BBOSInvadeQuest(SharedSpace *ss);
	virtual ~BBOSInvadeQuest();
	virtual void Tick(SharedSpace *unused);
	virtual void MonsterEvent(BBOSMonster *theMonster, int eventType, int x = -10, int y = -10);
	InvadeQuestMember * FindMemberByMonster(BBOSMonster *monster,
														 DoublyLinkedList **srcList);
	virtual void KillMember(InvadeQuestMember * curMember, DoublyLinkedList *srcList);
	int IsValidMonster(BBOSMonster *monster, InvadeQuestMember *curMember);
	void ResurrectMonster(InvadeQuestMember *curMember);

	void CreateMonster(void);

	int centerX, centerY;
	int monsterType;
	float monsterPower;

	DoublyLinkedList monsterList;

	int totalInvadeQuestMembersKilled, killedOnSpot;
	int totalInvadeQuestMembersBorn;
	int totalInvadeQuestMembersInLists;

};

#endif
