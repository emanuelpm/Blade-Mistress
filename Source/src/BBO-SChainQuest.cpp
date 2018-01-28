
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-SchainQuest.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

enum
{ 
	CHAINQUEST_READY,
	CHAINQUEST_ON,
	CHAINQUEST_RECOVERING,
	CHAINQUEST_MAX
};

//******************************************************************
//******************************************************************
BBOSChainQuest::BBOSChainQuest(SharedSpace *s) : BBOSAutoQuest(s)
{
	questState = CHAINQUEST_READY;
}

//******************************************************************
BBOSChainQuest::~BBOSChainQuest()
{

}

//******************************************************************
void BBOSChainQuest::Tick(SharedSpace *unused)
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

	if (CHAINQUEST_READY == questState)
	{
		int oneHour = 60 * 6;
		if (bboServer->dayTimeCounter > 2.75f * oneHour &&
			 bboServer->dayTimeCounter < 3.75f * oneHour)
		{
			if (1 == bboServer->weatherState) // if storming
			{
				questState = CHAINQUEST_ON;
				questCounter = 0;
				monsterType = rand() % 9;
				monsterPower = 4; // good start

				sprintf(&(tempText[2]),"This storm is no ordinary storm...");
				tempText[0] = NWMESS_PLAYER_CHAT_LINE;
				tempText[1] = TEXT_COLOR_DATA;
	  			bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

				sprintf(&(tempText[2]),"It is a Demonic Storm.  Chaos is coming!");
				tempText[0] = NWMESS_PLAYER_CHAT_LINE;
				tempText[1] = TEXT_COLOR_DATA;
	  			bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);
			}
		}
	}
	else if (CHAINQUEST_RECOVERING == questState)
	{
		int oneHour = 60 * 6;
		if (bboServer->dayTimeCounter > 2.75f * oneHour &&
			 bboServer->dayTimeCounter < 3.75f * oneHour)
			 ;
		else
		{
			questState = CHAINQUEST_READY;
			questCounter = 0;
		}
	}
	else if (CHAINQUEST_ON == questState)
	{
		if (count[monsterType][0] <= 0)
		{
			if (questCounter < 6 * 10) // less than 10 minutes
			{
				monsterPower *= rnd(1.3f, 1.5f);
				CreateMonster();
			}
			else
			{
				sprintf(&(tempText[2]),"The Demonic Storm has abated.  The weary world can relax, for now.");
				tempText[0] = NWMESS_PLAYER_CHAT_LINE;
				tempText[1] = TEXT_COLOR_DATA;
	  			bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);
				questState = CHAINQUEST_RECOVERING;
			}
		}
	}
}



//******************************************************************
void BBOSChainQuest::CreateMonster(void)
{
	char tempText[1024];
	int mx, my, good;
	GroundMap *gm = (GroundMap *) ss;  // ASSUMPTION!!!

	// find the spot
	do
	{
		mx = rand() % 256;
		my = rand() % 256;

		good = TRUE;

		int color = gm->Color(mx,my);
		if (color > 5 || color < 1)
			good = FALSE;

		if (! ss->CanMove(mx,my,mx,my))
			good = FALSE;

		BBOSMob *tMob = NULL;
		tMob = ss->mobList->GetFirst(mx, my);
		while (tMob)
		{
			if (SMOB_TOWER == tMob->WhatAmI())
				good = FALSE;
			tMob = ss->mobList->GetNext();
		}

	} while (!good);

	// create the monster on the spot
	BBOSMonster *monster = new BBOSMonster(monsterType,0,this);
	monster->cellX = monster->targetCellX = monster->spawnX = mx;
	monster->cellY = monster->targetCellY = monster->spawnY = my;
	ss->mobList->Add(monster);
	sprintf(monster->uniqueName, "Storm Creature");

	//	adjusts its power to match player power
	monster->r               = 255;
	monster->g               = 0;
	monster->b               = 0;
	monster->a               = 255;
	monster->sizeCoeff       = 1.5f + 1.0f * monsterPower /800;
	monster->health          = 40 * monsterPower;
	monster->maxHealth       = 40 * monsterPower;
	monster->damageDone      = monsterPower/20;
	if (monster->damageDone > 50)
		monster->damageDone = 50;

	monster->toHit           = monsterPower/14;
	monster->defense         = monsterPower/14;
	monster->dropAmount      = monsterPower/100;
	monster->magicResistance = monsterPower/20/100;
	if (monster->magicResistance > 0.99)
		monster->magicResistance = 0.99;

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

	sprintf(&(tempText[2]),"A bolt of lightning strikes at %dN %dE!  The world shudders!",
		                     256-my, 256-mx);
	tempText[0] = NWMESS_PLAYER_CHAT_LINE;
	tempText[1] = TEXT_COLOR_DATA;
	bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

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

	// track it
	++totalMembersBorn;
	questCounter = 0;
}

//******************************************************************
void BBOSChainQuest::MonsterEvent(BBOSMonster *theMonster, int eventType, int x, int y)
{
	if (theMonster->controllingAvatar) // forget about controlled avatars.
		return;

	if (AUTO_EVENT_ATTACKED == eventType)
	{
	}
	else if (AUTO_EVENT_DIED == eventType)
	{
		int num = monsterPower/300 + rnd(0, monsterPower) / 300;
		for (int i = 0; i < num; ++i)
			ss->DoMonsterDropSpecial(theMonster,21);
	}
}


/* end of file */



