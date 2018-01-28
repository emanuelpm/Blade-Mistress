
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "BBO-Savatar.h"
//#include "BBO-Snpc.h"
#include "BBO-Smonster.h"
//#include "BBO-Sgenerator.h"
//#include "BBO-Stower.h"
#include "monsterData.h"
#include ".\network\server.h"
#include "version.h"

#include "labyrinth-Map.h"

//******************************************************************
LabyrinthMap::LabyrinthMap(int doid, char *doname, NetWorldRadio * ls) : SharedSpace(doid,doname, ls)
{
//	for (int i = 0; i < width; ++i)
//		for (int j = 0; j < height; ++j)
//			toughestMonsterPoints[i][j] = 0;
	type = 0;
	monsterMap = NULL;
}

//******************************************************************
LabyrinthMap::~LabyrinthMap()
{
	delete[] map;

	if (monsterMap)
		delete[] monsterMap;

}

//******************************************************************
void LabyrinthMap::InitNew(char *terrainDataFile, int w, int h, int eX, int eY)
{
	FILE *fp;

	SharedSpace::InitNew(w, h, eX, eY);

	sizeX = width  = w;
	sizeY = height = h;
	enterX = eX;
	enterY =	eY;

	map = new unsigned char[width * height];

	fp = fopen(terrainDataFile,"rb");
	fread(map, 1, width * height, fp);
	fclose(fp);

	groundInventory = new Inventory[width * height];

	monsterMap = new float[width * height];

}

//******************************************************************
void LabyrinthMap::Save(void)
{
}

//******************************************************************
void LabyrinthMap::Load(void)
{
}

//******************************************************************
int LabyrinthMap::CanMove(int srcX, int srcY, int dstX, int dstY)
{
	// can't move out of bounds
	if (dstX < 0 || dstX >= width-1)
		return FALSE;
	if (dstY < 0 || dstY >= height-1)
		return FALSE;

	unsigned char color = map[dstY * width + dstX];
	if (0 == color)
		return FALSE;
	color = map[dstY * width + dstX+1];
	if (0 == color)
		return FALSE;
	color = map[(dstY+1) * width + dstX];
	if (0 == color)
		return FALSE;
	color = map[(dstY+1) * width + dstX+1];
	if (0 == color)
		return FALSE;




	if (dstX < width-1)
	{
		color = map[dstY * width + dstX + 1];
		if (0 == color)
			return FALSE;
	}

	if (dstY < height-1)
	{
		color = map[(dstY+1) * width + dstX];
		if (0 == color)
			return FALSE;
	}

	return TRUE;

}

//******************************************************************
int LabyrinthMap::Color    (int x, int y)
{
	return (int) map[y*width + x];
}

//*******************************************************************************
void LabyrinthMap::DoMonsterDrop(BBOSMonster *monster)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDrop(inv, monster);

}

//*******************************************************************************
void LabyrinthMap::DoMonsterDropSpecial(BBOSMonster *monster, int type)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDropSpecial(inv, monster,type);

}

//*******************************************************************************
Inventory *LabyrinthMap::GetGroundInventory(int x, int y)
{
	Inventory *inv = &(groundInventory[width*y + x]);

	return inv;
}


//*******************************************************************************
void LabyrinthMap::UpdateMonsterMap(void)
{
	DWORD now = timeGetTime();

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			monsterMap[width * y + x] = -1; // unconnected state
		}
	}

	monsterMap[6*width+5] = 0; // start in a square we know is valid


	// map out the valid positions
	int done = FALSE;
	while (!done)
	{
		done = TRUE;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (-1 == monsterMap[width * y + x])  // if unconnected
				{
					if (CanMove(x,y, x-1, y))
					{
						if (monsterMap[width * y + x-1] > -1)
						{
							monsterMap[width * y + x] = 0;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x+1, y))
					{
						if (monsterMap[width * y + x+1] > -1)
						{
							monsterMap[width * y + x] = 0;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x, y+1))
					{
						if (monsterMap[width * (y+1) + x] > -1)
						{
							monsterMap[width * y + x] = 0;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x, y-1))
					{
						if (monsterMap[width * (y-1) + x] > -1)
						{
							monsterMap[width * y + x] = 0;
							done = FALSE;
						}
					}
				}
			}
		}
	}

	// add the monster count
	BBOSMob *curMob = (BBOSMob *) mobList->GetFirst(0,0,1000);
	while (curMob)
	{
		if (SMOB_MONSTER == curMob->WhatAmI())
		{
			BBOSMonster *curMonster = (BBOSMonster *) curMob;
			if (26 == curMonster->type) // if bat
				monsterMap[curMonster->cellY*width+curMonster->cellX] += 1;
		}
		curMob = (BBOSMob *) mobList->GetNext();
	}

	// average until no square is == 0
	done = FALSE;
	while (!done)
	{
		done = TRUE;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (0 == monsterMap[width * y + x])  // if unprocessed
				{
					done = FALSE;

					float bestNeighbor = -2;

					if (monsterMap[width * y + x-1] > bestNeighbor)
						bestNeighbor = monsterMap[width * y + x-1];
					if (monsterMap[width * y + x+1] > bestNeighbor)
						bestNeighbor = monsterMap[width * y + x+1];

					if (monsterMap[width * (y+1) + x] > bestNeighbor)
						bestNeighbor = monsterMap[width * (y+1) + x];
					if (monsterMap[width * (y-1) + x] > bestNeighbor)
						bestNeighbor = monsterMap[width * (y-1) + x];

					if (bestNeighbor >= 0)
						monsterMap[width * y + x] = bestNeighbor * 0.8f;
//					else
//						monsterMap[width * y + x] = 0.001f;
				}
			}
		}
	}

	DWORD now2 = timeGetTime();

	FILE *fp = fopen("labyMap.txt","w");
	if (fp)
	{
		fprintf(fp,"\n******** %ld\n", now2 - now);
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (monsterMap[width * y + x] < 0)
					fprintf(fp,"--- ");
				else
					fprintf(fp,"%03d ", (int)(monsterMap[width * y + x]*100));
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
	}

}
  

/* end of file */



