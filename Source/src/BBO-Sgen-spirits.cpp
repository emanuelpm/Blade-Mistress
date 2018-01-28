
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Sgen-spirits.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "Realm-Map.h"


//******************************************************************
BBOSGenSpirits::BBOSGenSpirits(int xa, int ya) : BBOSGenerator(xa,ya)
{

	for (int i = 0; i < NUM_OF_MONSTERS; ++i)
	{
		for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
		{
		   max[i][j]   = 0;
			if ((10 == i || 11 == i) && 0 == j)
			   max[i][j]   = 20;

//			if (11 == i && 1 == j) // dokk's centurion
//			   max[i][j]   = 3;
//			if (16 == i && 1 == j) // dokk
//			   max[i][j]   = 1;


		}
	}
}

//******************************************************************
BBOSGenSpirits::~BBOSGenSpirits()
{
}

//******************************************************************
void BBOSGenSpirits::Tick(SharedSpace *ss)
{
	BBOSMob *curMob = NULL;
	BBOSAvatar *curAvatar = NULL;

	DWORD delta;
	DWORD now = timeGetTime();

	delta = now - lastSpawnTime;

	if (0 == lastSpawnTime || now < lastSpawnTime)
	{
		lastSpawnTime = now - rand() % 300;
	}

	// spawn
	if (delta > 300)	
	{
		lastSpawnTime = now;

		for (int i = 0; i < NUM_OF_MONSTERS; ++i)
		{
			for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
			{
				if (count[i][j] < max[i][j] && monsterData[i][j].name[0])
				{
					{																 
						int mx, my, good;
						GroundMap *gm = (GroundMap *) ss;  // ASSUMPTION!!!

						do
						{
							FindMonsterPoint(i+j+count[i][j], mx,my,monsterData[i][j].townDist + 4 + (rand() % 3));

							good = TRUE;

							int color = gm->Color(mx,my);
							if (color > 5 || color < 1)
								good = FALSE;

							if (! ss->CanMove(mx,my,mx,my))
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
						if(SPACE_DUNGEON == ss->WhatAmI())
						{
							mobAppear.staticMonsterFlag = FALSE;
							if (!monster->isWandering)
								mobAppear.staticMonsterFlag = TRUE;
						}

						mobAppear.x = monster->cellX;
						mobAppear.y = monster->cellY;
						ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
									 sizeof(mobAppear), &mobAppear);


						return;

					}

					if ((MONSTER_PLACE_SPIRITS & monsterData[i][j].placementFlags) &&
						 SPACE_REALM == ss->WhatAmI() &&
						 REALM_ID_SPIRITS == ((RealmMap *)ss)->type)
					{																 
						int mx, my, good;
						RealmMap *rm = (RealmMap *) ss;  // ASSUMPTION!!!

						do
						{
							mx = (rand() % 60) + 2;
							my = (rand() % 60) + 2;

							good = TRUE;

							if (!rm->CanMove(mx,my,mx,my))
								good = FALSE;

							if (mx > 1 && mx < 6)
								good = FALSE;
							if (my > 1 && my < 6)
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
						if(SPACE_DUNGEON == ss->WhatAmI())
						{
							mobAppear.staticMonsterFlag = FALSE;
							if (!monster->isWandering)
								mobAppear.staticMonsterFlag = TRUE;
						}

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
	

}


//******************************************************************
void BBOSGenSpirits::FindMonsterPoint(int town, int &mx, int &my, int townDist)
{
	// pick a town, any town
	int t = rand() % NUM_OF_TOWNS;

	mx = townList[t].x;
	my = townList[t].y;

	// start walking away from it, by...

	int tries, dist = 0;

	tries = 0;
	while (tries < 20)
	{
		// pick a point around the current point
		int dx = rand() % 3 - 1 + mx;
		int dy = rand() % 3 - 1 + my;

		int newDist = 1000, tempDist;
		for (int i = 0; i < NUM_OF_TOWNS; ++i)
		{
			tempDist = abs(dx - townList[i].x);
			if (tempDist < abs(dy - townList[i].y))
				tempDist = abs(dy - townList[i].y);

			if (tempDist < newDist)
				newDist = tempDist;
		}

		// is it further away from every town?
		if (newDist >= dist)
		{
			// go there
			mx = dx;
			my = dy;
			dist = newDist;

			// is it far enough?
			if (dist >= townDist)
				return;
			tries = 0;
		}
		// if not, try again to move for a few times
		else
			++tries;
	}


}





/* end of file */



