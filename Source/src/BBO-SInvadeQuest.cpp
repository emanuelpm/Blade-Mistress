
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Savatar.h"
#include "BBO-SinvadeQuest.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

enum
{ 
	INVADEQUEST_READY,
	INVADEQUEST_ON,
	INVADEQUEST_RECOVERING,
	INVADEQUEST_MAX
};

int totalInvadeQuestMembers = 0;

int invaderType[4] = {2, 22, 25, 1};

//******************************************************************
//******************************************************************
InvadeQuestMember::InvadeQuestMember(void) : DataObject(0,"ARMY_MEMBER")
{
	monster = NULL;
	deadTimer = 0;
	isDead = TRUE;
	++totalInvadeQuestMembers;
}

//******************************************************************
InvadeQuestMember::~InvadeQuestMember()
{
	--totalInvadeQuestMembers;
}


//******************************************************************
//******************************************************************
BBOSInvadeQuest::BBOSInvadeQuest(SharedSpace *s) : BBOSAutoQuest(s)
{
	questState = INVADEQUEST_READY;

	for (int i = 0; i < 40; ++i)
	{
		InvadeQuestMember *am = new InvadeQuestMember();
		am->quality = 6;
		am->type = 8;
		am->subType = 0; // base creature
		am->targetX = centerX;
		am->targetY = centerY;
		monsterList.Append(am);
	}

}

//******************************************************************
BBOSInvadeQuest::~BBOSInvadeQuest()
{

}

//******************************************************************
void BBOSInvadeQuest::Tick(SharedSpace *unused)
{
	char tempText[1024];

	DWORD delta;
	DWORD now = timeGetTime();

	// process at intervals
	delta = now - lastSpawnTime;
	if (delta < 1000 * 10)  // 10 seconds	
		return;

	++questCounter;

	lastSpawnTime = now;

	if (INVADEQUEST_READY == questState)
	{
		int oneHour = 60 * 6;
		if (bboServer->dayTimeCounter > 1 * oneHour)
		{
			questState = INVADEQUEST_ON;
			questCounter = 0;
			monsterType = invaderType[rand() % 4];

			int index = rand() % NUM_OF_TOWNS;
			if (2 == index) // change fingle to binu
				++index;
			centerX = townList[index].x+1;
			centerY = townList[index].y+1;

			totalInvadeQuestMembersKilled = 0;
			killedOnSpot = 0;

			sprintf(&(tempText[2]),"A strange power gathers in %s!", townList[index].name);
			tempText[0] = NWMESS_PLAYER_CHAT_LINE;
			tempText[1] = TEXT_COLOR_DATA;
	  		bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);
		}
	}
	else if (INVADEQUEST_RECOVERING == questState)
	{
		int oneHour = 60 * 6;
		if (bboServer->dayTimeCounter < 1 * oneHour)
		{
			questState = INVADEQUEST_READY;
			questCounter = 0;
		}
	}
	else if (INVADEQUEST_ON == questState)
	{

		// for each dead monster   **************
		InvadeQuestMember *curMember = (InvadeQuestMember *) monsterList.First();
		while (curMember)
		{
			// time to re-spawn?
			if (curMember->isDead && (0 == curMember->deadTimer || 
				 curMember->deadTimer < now ))
			{
				ResurrectMonster(curMember);
			}
			else
			{
				// if the monster exists
				BBOSMonster *monster = NULL;
				if (!curMember->isDead)
					monster = (BBOSMonster *) ss->mobList->IsInList(curMember->monster, TRUE);
				if (IsValidMonster(monster, curMember))
				{
					// if the monster needs to move
					if ((rand() % 2) && !monster->isMoving  && !monster->curTarget &&
						 monster->magicEffectAmount[MONSTER_EFFECT_BIND] <= 0 && 
						 (monster->cellX != centerX || 
						  monster->cellY != centerY))
					{
						// move it!
						if (abs(monster->cellX - centerX) > 
							 abs(monster->cellY - centerY))
						{
							if (monster->cellX > centerX)
								monster->targetCellX = monster->cellX - 1;
							else
								monster->targetCellX = monster->cellX + 1;
						}
						else
						{
							if (monster->cellY > centerY)
								monster->targetCellY = monster->cellY - 1;
							else
								monster->targetCellY = monster->cellY + 1;
						}

						int dontMove = FALSE;
						for (int i = 0; i < NUM_OF_TOWNS; ++i)
						{
							if (townList[i].x == monster->targetCellX && townList[i].y == monster->targetCellY)
								dontMove = TRUE;
						}

						if (!dontMove)
						{
							monster->isMoving = TRUE;
							monster->moveStartTime = timeGetTime();

							MessMobBeginMove bMove;
							bMove.mobID = (unsigned long) monster;
							bMove.x = monster->cellX;
							bMove.y = monster->cellY;
							bMove.targetX = monster->targetCellX;
							bMove.targetY = monster->targetCellY;
							ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, sizeof(bMove), &bMove);
						}
						else
						{
							monster->targetCellX = monster->cellX;
							monster->targetCellY = monster->cellY;
						}
					}

					if (7 == rand() % 50)
					{
						switch(rand() % 3)
						{
						case 0:
						default:
							sprintf(&(tempText[2]),"The creature says, Filthy humans, get out of our way!");
							break;
						case 1:
							sprintf(&(tempText[2]),"The creature says, We must summon the Revenants!");
							break;
						case 2:
							sprintf(&(tempText[2]),"The creature says, You won't keep the Revenents away!");
							break;
						}
						tempText[0] = NWMESS_PLAYER_CHAT_LINE;
						tempText[1] = TEXT_COLOR_SHOUT;
						ss->SendToEveryoneNearBut(0,monster->cellX, monster->cellY,
				  		                    strlen(tempText) + 1,(void *)&tempText);

					}

				}
				else
				{
					KillMember(curMember, &monsterList);
				}
			}

			curMember = (InvadeQuestMember *) monsterList.Next();
		}

		if (killedOnSpot > 5)
		{
			monsterPower = 0;
			//   finds every player near
			BBOSAvatar *chosenAvatar = NULL;
			BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
			while (curMob)
			{
				if (abs(curMob->cellX - centerX) < 5 && abs(curMob->cellY - centerY) < 5) 
				{
					BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;

					//	totals up their power
					//		power = dodge skill + age + sword damage + sword to-hit + sword magic
					monsterPower += 
							  curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime / 20;

					monsterPower += 
						curAvatar->GetDodgeLevel();

					monsterPower += curAvatar->BestSwordRating();
				}

				curMob = (BBOSMob *) ss->avatars->Next();
			}

			questState = INVADEQUEST_RECOVERING;
			sprintf(&(tempText[2]),"The sky splits open.  The Revenants have been summoned!");
			tempText[0] = NWMESS_PLAYER_CHAT_LINE;
			tempText[1] = TEXT_COLOR_DATA;
	  		bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

			monsterPower /= 3;

			CreateMonster();
			CreateMonster();
			CreateMonster();
		}

/*

		if (count[monsterType][0] <= 0)
		{
			if (questCounter < 6 * 10) // less than 10 minutes
			{
				monsterPower *= rnd(1.3f, 1.5f);
				CreateMonster();
			}
			else
			{
				sprintf(&(tempText[2]),"The invasion is over.");
				tempText[0] = NWMESS_PLAYER_CHAT_LINE;
				tempText[1] = TEXT_COLOR_DATA;
	  			bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);
				questState = INVADEQUEST_RECOVERING;
			}
		}
		*/
	}
}


//******************************************************************
void BBOSInvadeQuest::CreateMonster(void)
{
//	char tempText[1024];
	int mx, my;//, good;
	GroundMap *gm = (GroundMap *) ss;  // ASSUMPTION!!!

	mx = centerX;
	my = centerY;

	// create the monster on the spot
	BBOSMonster *monster = new BBOSMonster(monsterType,1,this);
	monster->cellX = monster->targetCellX = monster->spawnX = mx;
	monster->cellY = monster->targetCellY = monster->spawnY = my;
	ss->mobList->Add(monster);
	sprintf(monster->uniqueName, "Revenant");

	//	adjusts its power to match player power
	monster->r               = 255;
	monster->g               = 0;
	monster->b               = 255;
	monster->a               = 255;
	monster->sizeCoeff       = 2.0f;
	monster->health          = 40 * monsterPower;
	monster->maxHealth       = 40 * monsterPower;
	monster->damageDone      = monsterPower/20;
	if (monster->damageDone > 50)
		monster->damageDone = 50;

	monster->toHit           = monsterPower/14;
	monster->defense         = monsterPower/14;
	monster->dropAmount      = monsterPower/100;
	monster->magicResistance = monsterPower/20/100;
	if (monster->magicResistance > 0.99f)
		monster->magicResistance = 0.99f;

	monster->healAmountPerSecond = monsterPower/3;

	// announce it
	MessMobAppearCustom mAppear;
	mAppear.type = SMOB_MONSTER;
	mAppear.mobID = (unsigned long) monster;
	mAppear.x = monster->cellX;
	mAppear.y = monster->cellY;
	mAppear.monsterType = monster->type;
	mAppear.subType = monster->subType;
	CopyStringSafely(monster->Name(), 32, mAppear.name, 32);
	mAppear.a = monster->a;
	mAppear.r = monster->r;
	mAppear.g = monster->g;
	mAppear.b = monster->b;
	mAppear.sizeCoeff = monster->sizeCoeff;
	ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
				 sizeof(mAppear), &mAppear);

	MessGenericEffect messGE;
	messGE.avatarID = -1;
	messGE.mobID    = 0;
	messGE.x        = mx;
	messGE.y        = my;
	messGE.r        = 255;
	messGE.g        = 255;
	messGE.b        = 255;
	messGE.type     = 0;  // type of particles
	messGE.timeLen  = 2; // in seconds
	ss->SendToEveryoneNearBut(0, mx, my,
			          sizeof(messGE),(void *)&messGE);

}

//******************************************************************
void BBOSInvadeQuest::MonsterEvent(BBOSMonster *theMonster, int eventType, int x, int y)
{
	if (theMonster->controllingAvatar) // forget about controlled avatars.
		return;

	char tempText[1024];

	if (AUTO_EVENT_ATTACKED == eventType)
	{
	}
	else if (AUTO_EVENT_DIED == eventType)
	{
		if (totalInvadeQuestMembersKilled > 140 && INVADEQUEST_ON == questState)
		{
			questState = INVADEQUEST_RECOVERING;
			sprintf(&(tempText[2]),"The invasion is has been successfully defeated.  The Revenants remain in the Abyss.");
			tempText[0] = NWMESS_PLAYER_CHAT_LINE;
			tempText[1] = TEXT_COLOR_DATA;
	  		bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);
		}

		if (1 == theMonster->subType)
			ss->DoMonsterDropSpecial(theMonster,21);
		else if (centerX == theMonster->cellX && centerY == theMonster->cellY)
			++killedOnSpot;
		/*
		int num = monsterPower/300 + rnd(0, monsterPower) / 300;
		for (int i = 0; i < num; ++i)
			ss->DoMonsterDropSpecial(theMonster,21);
			*/
	}
}

//******************************************************************
InvadeQuestMember * BBOSInvadeQuest::FindMemberByMonster(BBOSMonster *monster,
														 DoublyLinkedList **srcList)
{
	InvadeQuestMember *curMember = (InvadeQuestMember *) monsterList.First();
	while (curMember)
	{
		if (curMember->monster == monster &&
				IsValidMonster(monster, curMember))
		{
			*srcList = &monsterList;
			return curMember;
		}
		curMember = (InvadeQuestMember *) monsterList.Next();
	}

	*srcList = NULL;
	return NULL;
}

//******************************************************************
void BBOSInvadeQuest::KillMember(InvadeQuestMember * curMember, DoublyLinkedList *srcList)
{
	++totalInvadeQuestMembersKilled;

	curMember->monster = (BBOSMonster *)30;
	curMember->isDead = TRUE;
	curMember->deadTimer = timeGetTime() + 2000;
}

//******************************************************************
int BBOSInvadeQuest::IsValidMonster(BBOSMonster *monster, InvadeQuestMember *curMember)
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
	
	if (monster->subType != curMember->subType)
	{
		return FALSE;
	}

	return TRUE;
}

//******************************************************************
void BBOSInvadeQuest::ResurrectMonster(InvadeQuestMember *curMember)
{
	curMember->deadTimer = timeGetTime();
	// create the creature at the spawn point
	BBOSMonster *monster;
	monster = new BBOSMonster(monsterType, curMember->subType, this);

	++totalInvadeQuestMembersBorn;
	/*
	char tempText[1024];
	sprintf(tempText,"Member %ld : Monster born: %d-%d\n", 
		     (long) curMember, monster->type, monster->subType);
	DebugOutput(tempText);
	*/
	float ang = rnd(0, 3.1415f * 2);
	monster->cellX = centerX + sin(ang) * 5;
	monster->cellY = centerY + cos(ang) * 5;
	monster->targetCellX = monster->cellX;
	monster->targetCellY = monster->cellY;
	monster->spawnX = monster->cellX;
	monster->spawnY = monster->cellY;
	ss->mobList->Add(monster);

	MessMobAppear mobAppear;
	mobAppear.mobID = (unsigned long) monster;
	mobAppear.type = monster->WhatAmI();
	mobAppear.monsterType = monster->type;
	mobAppear.subType = monster->subType;
	mobAppear.staticMonsterFlag = FALSE;

	mobAppear.x = monster->cellX;
	mobAppear.y = monster->cellY;
	ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
				 sizeof(mobAppear), &mobAppear);

	// make sure it knows it belongs to this monster's army
	monster->myGenerator = this;
	curMember->monster = monster;
	curMember->monsterCreationTime = monster->creationTime;

	// change the army member to the correct state
	curMember->isDead = FALSE;

}


/* end of file */



