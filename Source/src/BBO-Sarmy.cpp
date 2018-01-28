
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Sarmy.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
//#include "dxutil.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

int spaceOffset[8][2] =
{
	{ 1, 0},
	{-1, 0},
	{ 0,-1},
	{ 0, 1},

	{-1,-1},
	{ 1,-1},
	{ 1, 1},
	{-1, 1}
};

int totalArmyMembers = 0;

//******************************************************************
//******************************************************************
ArmyMember::ArmyMember(void) : DataObject(0,"ARMY_MEMBER")
{
	monster = NULL;
	deadTimer = 0;
	isDead = TRUE;
	++totalArmyMembers;
}

//******************************************************************
ArmyMember::~ArmyMember()
{
	--totalArmyMembers;
}


//******************************************************************
//******************************************************************
BBOSArmy::BBOSArmy(SharedSpace *s, int cX, int cY) : BBOSGenerator(0,0)
{
	ss = s;
	centerX = cX;
	centerY = cY;

	do_id = 1; // this is an army, not a generator
	lastSpawnTime = timeGetTime();

	for (int f = 0; f < 3; ++f)
		focusValue[f] = 0;  // means this focus isn't active

//	totalArmyMembers = 0;
	totalArmyMembersKilled = 0;
	totalArmyMembersBorn = 0;
	totalArmyMembersInLists = 0;
}

//******************************************************************
BBOSArmy::~BBOSArmy()
{

}

//******************************************************************
void BBOSArmy::MonsterEvent(BBOSMonster *theMonster, int eventType, int x, int y)
{
	if (theMonster->controllingAvatar) // forget about controlled avatars.
		return;

	if (-10 == x)
	{
		x = theMonster->cellX;
		y = theMonster->cellY;
	}
	if (ARMY_EVENT_ATTACKED == eventType || ARMY_EVENT_DIED == eventType)
	{
		// find the associated member
		DoublyLinkedList *srcListPtr;
		ArmyMember * theMember = FindMemberByMonster(theMonster, &srcListPtr);
		if (!theMember)
		{
			char tempText[1024];
			sprintf(tempText,"No member for %s at %d %d\n",
				     theMonster->Name(), theMonster->cellX,theMonster->cellY);
			LogOutput("army-monster-disconnect.txt", tempText);
		
			return; // I don't know this guy!
		}
		int fIndex = -1;
		//if this is at an existing focus
		for (int f = 0; f < 3; ++f)
		{
			if (focusValue[f] > 0 && 
				 x == focusX[f] && 
				 y == focusY[f])
				fIndex = f;
		}

		int newVal = theMember->quality;
		if (ARMY_EVENT_DIED == eventType)
		{
			// move the member the player just killed
			/*
			int nX, nY;
			int found = FALSE;
			while (!found)
			{
				nX = theMember->targetX + (rand() % 3) -1;
				nY = theMember->targetY + (rand() % 3) -1;
				if (abs(nX - centerX) < 5 && abs(nY - centerY) < 5)
					found = TRUE;
			}

			theMember->targetX = nX;
			theMember->targetY = nY;
			*/
			// make this focus more attackable
			newVal *= 2;
			KillMember(theMember, srcListPtr);
		}

		if (fIndex > -1)
		{
			// does the focusValue need to be bumped up?
			if (newVal > focusValue[fIndex])
			{
				// bump it up.
				focusValue[fIndex] = newVal;
			}
		}
		// else
		{
			for (int f = 0; f < 3; ++f)
			{
				if (focusValue[f] <= 0)
					fIndex = f;
			}
			//is there an inactive focus to use?
			if (fIndex > -1)
			{
				//use it.
				focusValue[fIndex] = newVal;
				focusX[fIndex] = x;
				focusY[fIndex] = y;
			}
			//else
			{
				// find an active focus with a lower value than this situation
				int hijack = -1;
				for (int f = 0; f < 3; ++f)
				{
					if (focusValue[f] < newVal)
						hijack = f;
				}

				// if found
				if (hijack > -1)
				{
					// switch the focus to here
					focusValue[hijack] = newVal;
					focusX[hijack] = x;
					focusY[hijack] = y;
					fIndex = hijack;
				}
			}

		}

		lastFocusEventTime[fIndex] = timeGetTime();

		int fighters = focus[fIndex].ItemsInList();
		int noneFound = FALSE;
		// if this focus does NOT have enough members
		while (!noneFound && fighters < focusValue[fIndex])
		{
			noneFound = TRUE;

			// try to get some from the atEase list
			ArmyMember * recruit = ClosestMember(focusX[fIndex], focusY[fIndex], 
				                                  &atEase, theMember->quality);
			if (recruit)
			{
				atEase.Remove(recruit);
				focus[fIndex].Append(recruit);
				++fighters;
				noneFound = FALSE;
			}
			// if we don't have any troops atEase
			else
			{
				for (int f = 0; f < 3; ++f)
				{
					// is there a lower focusValue focus?
					if (f != fIndex && focusValue[f] < focusValue[fIndex])
					{
						// steal some from it
						recruit = ClosestMember(focusX[fIndex], focusY[fIndex], 
														&focus[f], theMember->quality);
						if (recruit)
						{
							focus[f].Remove(recruit);
							focus[fIndex].Append(recruit);
							++fighters;
							noneFound = FALSE;
						}
					}
				}
			}

//			fighters = focus[fIndex].ItemsInList();
		}

		// finally, if I'm hurt bad, back off and come back
		if (ARMY_EVENT_ATTACKED == eventType)
		{
			if (theMonster->health *2 < theMonster->maxHealth && !theMonster->isMoving)
			{
				if (rand() % 2)
					theMonster->targetCellX = x + ((rand()%2)*2-1);
				else
					theMonster->targetCellY = y + ((rand()%2)*2-1);
				theMonster->isMoving = TRUE;
				theMonster->moveStartTime = timeGetTime();

				MessMobBeginMove bMove;
				bMove.mobID = (unsigned long) theMonster;
				bMove.x = theMonster->cellX;
				bMove.y = theMonster->cellY;
				bMove.targetX = theMonster->targetCellX;
				bMove.targetY = theMonster->targetCellY;
				ss->SendToEveryoneNearBut(0, (float)theMonster->cellX, (float)theMonster->cellY, sizeof(bMove), &bMove);
			}
		}
	}
}

//******************************************************************
void BBOSArmy::Tick(SharedSpace *unused)
{
	DWORD delta;
	DWORD now = timeGetTime();

	// process at intervals
	delta = now - lastSpawnTime;
	if (delta < 1000 * 3)  // 3 seconds	
		return;

	lastSpawnTime = now;


	// clear out the dead here **************
/*
	// for each focus
	for (int f = 0; f < 3; ++f)
	{
		// for each monster in the focus
		ArmyMember *curMember = (ArmyMember *) focus[f].First();
		while (curMember)
		{
			// if the monster doesn't exist, put the ArmyMember in the dead list
			BBOSMonster *monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster);
			if (!IsValidMonster(monster, curMember))
			{
				KillMember(curMember, &focus[f]);
				curMember = (ArmyMember *) focus[f].First();
			}
			else
				curMember = (ArmyMember *) focus[f].Next();
		}
	}

	// for each monster in the atEase list
	ArmyMember *curMember = (ArmyMember *) atEase.First();
	while (curMember)
	{
		// if the monster doesn't exist, put the ArmyMember in the dead list
		BBOSMonster *monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster);
		if (!IsValidMonster(monster, curMember))
		{
			KillMember(curMember, &atEase);
			curMember = (ArmyMember *) atEase.First();
		}
		else
		{
			// TEST!!!!
//			if (4 == rand() % 30)
//				monster->isDead = TRUE;

			curMember = (ArmyMember *) atEase.Next();
		}
	}
*/
	// for each dead monster   **************
	ArmyMember *curMember = (ArmyMember *) atEase.First();
	while (curMember)
	{
		// time to re-spawn?
		if (curMember->isDead && (0 == curMember->deadTimer || 
			 curMember->deadTimer < now ))
		{
			ResurrectMonster(curMember);
		}
		curMember = (ArmyMember *) atEase.Next();

	}

	// for each focus
	for (int f = 0; f < 3; ++f)
	{
		// for each monster in the focus
		curMember = (ArmyMember *) focus[f].First();
		while (curMember)
		{
			//if focus is not active, remove curMember
			if (0 == focusValue[f])
			{
				focus[f].Remove(curMember);
				atEase.Append(curMember);
				curMember = (ArmyMember *) focus[f].First();
			}
			else
			{
				// if the monster exists
				BBOSMonster *monster = NULL;
				if (!curMember->isDead)
					monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster, TRUE);
				if (IsValidMonster(monster, curMember))
				{
					// ELSE if the monster needs to move
					if (!monster->isMoving  && 
						 monster->magicEffectAmount[MONSTER_EFFECT_BIND] <= 0 && 
						 (monster->cellX != focusX[f] || 
						  monster->cellY != focusY[f]))
					{
						// move it!
						if (abs(monster->cellX - focusX[f]) > 
							 abs(monster->cellY - focusY[f]))
						{
							if (monster->cellX > focusX[f])
								monster->targetCellX = monster->cellX - 1;
							else
								monster->targetCellX = monster->cellX + 1;
						}
						else
						{
							if (monster->cellY > focusY[f])
								monster->targetCellY = monster->cellY - 1;
							else
								monster->targetCellY = monster->cellY + 1;
						}

						monster->isMoving = TRUE;
						monster->moveStartTime = timeGetTime();

						MessMobBeginMove bMove;
						bMove.mobID = (unsigned long) monster;
						bMove.x = monster->cellX;
						bMove.y = monster->cellY;
						bMove.targetX = monster->targetCellX;
						bMove.targetY = monster->targetCellY;
						ss->SendToEveryoneNearBut(0, (float)monster->cellX, (float)monster->cellY, sizeof(bMove), &bMove);
					}

				}
				else
				{
					KillMember(curMember, &focus[f]);
					focus[f].Remove(curMember);
					atEase.Append(curMember);
//					ResurrectMonster(curMember);
				}
				curMember = (ArmyMember *) focus[f].Next();
			}
		}
	}

	// for each monster atEase
	curMember = (ArmyMember *) atEase.First();
	while (curMember)
	{
		// if the monster exists
		BBOSMonster *monster = NULL;
		if (!curMember->isDead)
			monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster, TRUE);
		if (IsValidMonster(monster, curMember))
		{
			// ELSE if the monster needs to move
			if (!monster->isMoving && 
				 monster->magicEffectAmount[MONSTER_EFFECT_BIND] <= 0 && 
				 (monster->cellX != curMember->targetX || 
				  monster->cellY != curMember->targetY))
			{
				// move it!
				if (abs(monster->cellX - curMember->targetX) > 
					 abs(monster->cellY - curMember->targetY))
				{
					if (monster->cellX > curMember->targetX)
						monster->targetCellX = monster->cellX - 1;
					else
						monster->targetCellX = monster->cellX + 1;
				}
				else
				{
					if (monster->cellY > curMember->targetY)
						monster->targetCellY = monster->cellY - 1;
					else
						monster->targetCellY = monster->cellY + 1;
				}

				monster->isMoving = TRUE;
				monster->moveStartTime = timeGetTime();

				MessMobBeginMove bMove;
				bMove.mobID = (unsigned long) monster;
				bMove.x = monster->cellX;
				bMove.y = monster->cellY;
				bMove.targetX = monster->targetCellX;
				bMove.targetY = monster->targetCellY;
				ss->SendToEveryoneNearBut(0, (float)monster->cellX, (float)monster->cellY, sizeof(bMove), &bMove);
			}
		}	
		else if (!curMember->isDead)
		{
			KillMember(curMember, &atEase);
//			ResurrectMonster(curMember);
		}

		curMember = (ArmyMember *) atEase.Next();
	}

	// for each focus
	for (int f = 0; f < 3; ++f)
	{
		//if focus is active
		if (0 != focusValue[f])
		{
			delta = now - lastFocusEventTime[f];
			if (delta > 1000 * 25 * 1)  // 25 seconds
			{
				focusValue[f] = 0; // deactivating focus
			}
		}
	}

	totalArmyMembersInLists = 0;

	// for each monster atEase
	curMember = (ArmyMember *) atEase.First();
	while (curMember)
	{
		++totalArmyMembersInLists;
		curMember = (ArmyMember *) atEase.Next();
	}
	// for each focus
	for (int f = 0; f < 3; ++f)
	{
		// for each monster in the focus
		curMember = (ArmyMember *) focus[f].First();
		while (curMember)
		{
			++totalArmyMembersInLists;
			curMember = (ArmyMember *) focus[f].Next();
		}
	}

}


//******************************************************************
ArmyMember * BBOSArmy::ClosestMember(int x, int y, 
												 DoublyLinkedList *srcList, int highestLevel)
{
	ArmyMember *candidate = NULL;
	int leastDist = 1000;

//	if (22 == rand() % 30)
//		highestLevel *= 100;

	// for each monster in srcList
	ArmyMember *curMember = (ArmyMember *) srcList->First();
	while (curMember)
	{
		// if the monster exists
		BBOSMonster *monster = NULL;
		if (!curMember->isDead)
			monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster, TRUE);
		if (IsValidMonster(monster, curMember))
		{
			if (highestLevel >= curMember->quality && !curMember->isDead)
			{
				if (abs(monster->cellX - x) < abs(monster->cellY - y))
				{
					if (abs(monster->cellY - y) <= leastDist)
					{
						candidate = curMember;
						leastDist = abs(monster->cellY - y);
					}
				}
				else
				{
					if (abs(monster->cellX - x) <= leastDist)
					{
						candidate = curMember;
						leastDist = abs(monster->cellX - x);
					}
				}
			}
		}	
		curMember = (ArmyMember *) srcList->Next();
	}

	return candidate;
}

//******************************************************************
ArmyMember * BBOSArmy::FindMemberByMonster(BBOSMonster *monster,
														 DoublyLinkedList **srcList)
{
	ArmyMember *curMember = (ArmyMember *) atEase.First();
	while (curMember)
	{
		if (curMember->monster == monster &&
				IsValidMonster(monster, curMember))
		{
			*srcList = &atEase;
			return curMember;
		}
		curMember = (ArmyMember *) atEase.Next();
	}

	for (int f = 0; f < 3; ++f)
	{
		curMember = (ArmyMember *) focus[f].First();
		while (curMember)
		{
			if (curMember->monster == monster &&
					IsValidMonster(monster, curMember))
			{
				*srcList = &focus[f];
				return curMember;
			}
			curMember = (ArmyMember *) focus[f].Next();
		}
	}

	*srcList = NULL;
	return NULL;
}

//******************************************************************
void BBOSArmy::KillMember(ArmyMember * curMember, DoublyLinkedList *srcList)
{
	++totalArmyMembersKilled;

	curMember->monster = (BBOSMonster *)30;
//	srcList->Remove(curMember);
	curMember->isDead = TRUE;
	curMember->deadTimer = timeGetTime() + 
		curMember->quality * (curMember->quality + 1) * 1000 * 50;
//		curMember->quality * (curMember->quality + 1) * 1000 * 1;
	/*
	char tempText[1024];
	sprintf(tempText,"Member %ld : into dead pile\n", 
				     (long) curMember);
	DebugOutput(tempText);
	 */
}

//******************************************************************
BBOSMonster *BBOSArmy::MakeSpecialMonster(ArmyMember *curMember)
{
	BBOSMonster *monster = new BBOSMonster(curMember->type, curMember->subType, this);
	return monster;
}

//******************************************************************
int BBOSArmy::IsValidMonster(BBOSMonster *monster, ArmyMember *curMember)
{
	if (!monster)
		return FALSE;
	if (monster->creationTime != curMember->monsterCreationTime) 
	{
		return FALSE;
	}

	if	(curMember->type < 0)
	{
		if (monster->uniqueName[0]) // special monsters should have unique names
			return TRUE;
		else
			return FALSE;
	}
	
	if (monster->type != curMember->type || 
		 monster->subType != curMember->subType)
	{
		return FALSE;
	}

	return TRUE;
}

//******************************************************************
void BBOSArmy::ResurrectMonster(ArmyMember *curMember)
{
	curMember->deadTimer = timeGetTime();
	// create the creature at the spawn point
	BBOSMonster *monster;
	if (curMember->type >= 0)
		monster = new BBOSMonster(curMember->type, curMember->subType, this);
	else
		monster = MakeSpecialMonster(curMember);

	++totalArmyMembersBorn;
	/*
	char tempText[1024];
	sprintf(tempText,"Member %ld : Monster born: %d-%d\n", 
		     (long) curMember, monster->type, monster->subType);
	DebugOutput(tempText);
	*/
	monster->cellX = spawnX + rand() % 3 -1;
	monster->cellY = spawnY + rand() % 3 -1;
	monster->targetCellX = monster->cellX;
	monster->targetCellY = monster->cellY;
	monster->spawnX = monster->cellX;
	monster->spawnY = monster->cellY;
	ss->mobList->Add(monster);

	MessMobAppear mobAppear;
	mobAppear.mobID = (unsigned long) monster;
	mobAppear.type = (unsigned char)monster->WhatAmI();
	mobAppear.monsterType = monster->type;
	mobAppear.subType = monster->subType;
	mobAppear.staticMonsterFlag = FALSE;

	mobAppear.x = monster->cellX;
	mobAppear.y = monster->cellY;
	ss->SendToEveryoneNearBut(0, (float)monster->cellX, (float)monster->cellY, 
				 sizeof(mobAppear), &mobAppear);

	// make sure it knows it belongs to this monster's army
	monster->myGenerator = this;
	curMember->monster = monster;
	curMember->monsterCreationTime = monster->creationTime;

	// change the army member to the correct state
	curMember->isDead = FALSE;

}

/* end of file */



