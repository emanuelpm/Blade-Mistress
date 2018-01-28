
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

#include "Ground-Map.h"

//******************************************************************
GroundMap::GroundMap(int doid, char *doname, NetWorldRadio * ls) : SharedSpace(doid,doname, ls)
{
//	for (int i = 0; i < MAP_SIZE_WIDTH; ++i)
//		for (int j = 0; j < MAP_SIZE_HEIGHT; ++j)
//			toughestMonsterPoints[i][j] = 0;
}

//******************************************************************
GroundMap::~GroundMap()
{
}

//******************************************************************
void GroundMap::InitNew(int w, int h, int eX, int eY)
{
	FILE *fp;

	fp = fopen("dat\\terrain-bytes.raw","rb");
	fread(map, 1, MAP_SIZE_WIDTH * MAP_SIZE_HEIGHT, fp);
	fclose(fp);

	sizeX = MAP_SIZE_WIDTH;
	sizeY = MAP_SIZE_HEIGHT;

	groundInventory = new Inventory[MAP_SIZE_WIDTH * MAP_SIZE_HEIGHT];

	SharedSpace::InitNew(MAP_SIZE_WIDTH, MAP_SIZE_HEIGHT, 0,0);
}

//******************************************************************
void GroundMap::Save(void)
{
}

//******************************************************************
void GroundMap::Load(void)
{
}

//******************************************************************
int GroundMap::CanMove(int srcX, int srcY, int dstX, int dstY)
{
	// can't move out of bounds
	if (dstX < 2 || dstX >= MAP_SIZE_WIDTH-2)
		return FALSE;
	if (dstY < 2 || dstY >= MAP_SIZE_HEIGHT-2)
		return FALSE;

	unsigned char color = map[dstY][dstX];
	if (1 == color)
		return FALSE;
	return TRUE;

}

//******************************************************************
int GroundMap::Color    (int x, int y)
{
	return (int) map[y][x];
}

//*******************************************************************************
void GroundMap::DoMonsterDrop(BBOSMonster *monster)
{

	Inventory *inv = &(groundInventory[MAP_SIZE_WIDTH*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDrop(inv, monster);

}

//*******************************************************************************
void GroundMap::DoMonsterDropSpecial(BBOSMonster *monster, int type)
{

	Inventory *inv = &(groundInventory[MAP_SIZE_WIDTH*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDropSpecial(inv, monster, type);

}

//*******************************************************************************
Inventory *GroundMap::GetGroundInventory(int x, int y)
{
	Inventory *inv = &(groundInventory[MAP_SIZE_WIDTH*y + x]);

	return inv;
}


/* end of file */



