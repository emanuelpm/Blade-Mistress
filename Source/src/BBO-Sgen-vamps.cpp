
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Sgen-vamps.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "Ground-Map.h"


//******************************************************************
BBOSGenVamps::BBOSGenVamps(int xa, int ya) : BBOSGenerator(xa,ya)
{

	for (int i = 0; i < NUM_OF_MONSTERS; ++i)
	{
		for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
		{
		   max[i][j]   = 0;
			if (25 == i)
			   max[i][j]   = (5-j) * (5-j) * 7;

//			if (11 == i && 1 == j) // dokk's centurion
//			   max[i][j]   = 3;
//			if (16 == i && 1 == j) // dokk
//			   max[i][j]   = 1;


		}
	}
}

//******************************************************************
BBOSGenVamps::~BBOSGenVamps()
{
}

//******************************************************************
void BBOSGenVamps::Tick(SharedSpace *ss)
{
	BBOSMob *curMob = NULL;
	BBOSAvatar *curAvatar = NULL;

	DWORD delta;
	DWORD now = timeGetTime();

	delta = now - lastSpawnTime;

	if (0 == lastSpawnTime || now < lastSpawnTime)
	{
		lastSpawnTime = now - rand() % 2000;
	}

	// spawn
	if (delta > 2000)	
//	if (delta > 10)	
	{
		lastSpawnTime = now;

		// vampires only come out at night!
		
		int oneHour = 60 * 6;
		if (bboServer->dayTimeCounter < 2.75f * oneHour ||
			 bboServer->dayTimeCounter > 3.75f * oneHour)
			return;
		  
		for (int i = 0; i < NUM_OF_MONSTERS; ++i)
		{
			for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
			{
				if (count[i][j] < max[i][j] && monsterData[i][j].name[0])
				{
					int mx, my, good;
					GroundMap *rm = (GroundMap *) ss;  // ASSUMPTION!!!

					do
					{

						float ang = rnd(0, PI*2);
						mx = 220 + sin(ang) * monsterData[i][j].townDist * rnd(1, 2);
						my = 220 + cos(ang) * monsterData[i][j].townDist * rnd(1, 2);
						good = TRUE;

						if (!rm->CanMove(mx,my,mx,my))
							good = FALSE;

					} while (!good);
					
					BBOSMonster *monster = new BBOSMonster(i,j, this);
					monster->cellX = mx;
					monster->cellY = my;
					monster->targetCellX = mx;
					monster->targetCellY = my;
					monster->spawnX = mx;
					monster->spawnY = my;
					ss->mobList->Add(monster);

					MessMobAppear mobAppear;
					mobAppear.mobID = (unsigned long) monster;
					mobAppear.type = monster->WhatAmI();
					mobAppear.monsterType = monster->type;
					mobAppear.subType = monster->subType;
					mobAppear.x = monster->cellX;
					mobAppear.y = monster->cellY;
					ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
								 sizeof(mobAppear), &mobAppear);

					return;
				}
			}
		}
	}
}



/* end of file */



