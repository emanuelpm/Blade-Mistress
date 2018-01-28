
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-SNewbGenerator.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "Realm-Map.h"


//******************************************************************
BBOSNewbGenerator::BBOSNewbGenerator(int xa, int ya) : BBOSGenerator(xa,ya)
{
	x = xa;
	y = ya;

	for (int i = 0; i < NUM_OF_MONSTERS; ++i)
	{
		for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
		{
			count[i][j] = 0;
		   max[i][j]   = 0;
			if (monsterData[i][j].defense <= 2)
			   max[i][j]   = 20;
		}
	}
}

//******************************************************************
BBOSNewbGenerator::~BBOSNewbGenerator()
{
}

//******************************************************************
void BBOSNewbGenerator::Tick(SharedSpace *ss)
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
					if (MONSTER_PLACE_DUNGEON != monsterData[i][j].placementFlags &&
						 MONSTER_PLACE_SPIRITS != monsterData[i][j].placementFlags &&
						 SPACE_GROUND == ss->WhatAmI())
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
							else
							{
								if (1 == color && 
									 !(MONSTER_PLACE_GRASS & monsterData[i][j].placementFlags)
									)
									good = FALSE;
								if (2 == color && 
									 !(MONSTER_PLACE_GRASS & monsterData[i][j].placementFlags)
									)
									good = FALSE;
								if (3 == color && 
									 !(MONSTER_PLACE_SWAMP & monsterData[i][j].placementFlags)
									)
									good = FALSE;
								if (4 == color && 
									 !(MONSTER_PLACE_DESERT & monsterData[i][j].placementFlags)
									)
									good = FALSE;
								if (5 == color && 
									 !(MONSTER_PLACE_SNOW & monsterData[i][j].placementFlags)
									)
									good = FALSE;
							}

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

							// placing special monsters for Realm of Spirits
							if (11 == i && 1 == j) // dokk's centurion
							{
								good = TRUE;
							   switch (count[i][j])
								{
								case 0:
								default:
									mx = 62;
									my = 6;
									break;
								case 1:
									mx = 61;
									my = 5;
									break;
								case 2:
									mx = 62;
									my = 4;
									break;
								}
							}
							if (16 == i && 1 == j) // dokk
							{
								good = TRUE;
								mx = 62;
								my = 3;
							}

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
void BBOSNewbGenerator::FindMonsterPoint(int town, int &mx, int &my, int townDist)
{
	// pick a town, as long as it's Fingle :)
	int t = 2;

	mx = townList[t].x;
	my = townList[t].y;

	// start walking away from it, by...

	int tries, dist = 0;

	tries = 0;
	while (tries < 20)
	{
		// pick a point around the current point
		float angle = rnd(0,PI*2);
		float dx = mx + sin(angle) * townDist;
		float dy = my + cos(angle) * townDist;

		int newDist = 1000, tempDist;
		for (int i = 0; i < NUM_OF_TOWNS; ++i)
		{
			tempDist = Distance(dx, dy, townList[i].x, townList[i].y);
			if (tempDist < newDist)
				newDist = tempDist;
		}

		// is it further away from every town?
		if (newDist >= townDist-1)
		{
			// go there
			mx = dx;
			my = dy;
			return;
		}
		else
		{
			++tries;
		}
	}


}





/* end of file */



