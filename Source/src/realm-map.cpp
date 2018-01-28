
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

#include "realm-Map.h"

//******************************************************************
RealmMap::RealmMap(int doid, char *doname, NetWorldRadio * ls) : SharedSpace(doid,doname, ls)
{
//	for (int i = 0; i < width; ++i)
//		for (int j = 0; j < height; ++j)
//			toughestMonsterPoints[i][j] = 0;
	type = 0;
}

//******************************************************************
RealmMap::~RealmMap()
{
	delete[] map;

}

//******************************************************************
void RealmMap::InitNew(char *terrainDataFile, int w, int h, int eX, int eY)
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

}

//******************************************************************
void RealmMap::Save(void)
{
}

//******************************************************************
void RealmMap::Load(void)
{
}

//******************************************************************
int RealmMap::CanMove(int srcX, int srcY, int dstX, int dstY)
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
int RealmMap::Color    (int x, int y)
{
	return (int) map[y*width + x];
}

//*******************************************************************************
void RealmMap::DoMonsterDrop(BBOSMonster *monster)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDrop(inv, monster);

}

//*******************************************************************************
void RealmMap::DoMonsterDropSpecial(BBOSMonster *monster, int type)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDropSpecial(inv, monster,type);

}

//*******************************************************************************
Inventory *RealmMap::GetGroundInventory(int x, int y)
{
	Inventory *inv = &(groundInventory[width*y + x]);

	return inv;
}


/* end of file */



