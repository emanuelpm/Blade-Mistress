
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "BBO-Savatar.h"
#include "BBO-Snpc.h"
#include "BBO-Smonster.h"
#include "BBO-Sgenerator.h"
#include "BBO-Stower.h"
#include "BBO-Schest.h"
#include "monsterData.h"
#include ".\helper\GeneralUtils.h"
#include "version.h"

#include "Dungeon-Map.h"

char rdNamePre[7][64] = 
{
	{"Tomb"},
	{"Crypt"},
	{"Dungeon"},
	{"Pit"},
	{"Warrens"},
	{"Crack"},
	{"Labrinth"}
};

char rdNameMain[8][64] = 
{
	{"Tarath"},
	{"Daar"},
	{"Tal"},
	{"Maal"},
	{"Chiang"},
	{"Tien"},
	{"Brael"},
	{"Rudon"},
};

char rdNamePost[11][64] = 
{
	{"Del"},
	{"Shan"},
	{"Toor"},
	{"Beon"},
	{"Wid"},
	{"Quel"},
	{"Pola"},
	{"Hoth"},
	{"Goll"},
	{"Il"},
	{"Jhor"}
};

char rdNameAdjective[5][64] = 
{
	{"Secret"},
	{"Hidden"},
	{"Secluded"},
	{"Dank"},
	{"Twisted"}
};

//******************************************************************
DungeonMap::DungeonMap(int doid, char *doname, NetWorldRadio * ls) : SharedSpace(doid,doname, ls)
{

	leftWall = topWall = NULL;
	masterName[0] = 0;  // no master currently
	masterPass[0] = 0;  // no master currently
	records = NULL;
	pathMap = NULL;
	specialFlags = 0;

	sprintf(name,"%s of %s %s", 
		     rdNamePre [rand() % 7], 
			  rdNameMain[rand() % 8], 
			  rdNamePost[rand() % 11]);

	isLocked = FALSE;
}

//******************************************************************
DungeonMap::~DungeonMap()
{

	if (leftWall)
		delete[] leftWall;
	if (topWall)
		delete[] topWall;
	if (pathMap)
		delete[] pathMap;

	if (records)
		delete[] records;
}

//******************************************************************
void DungeonMap::InitNew(int w, int h, int eX, int eY, int rating)
{
	SharedSpace::InitNew(w, h, eX, eY);

	sizeX = width  = w;
	sizeY = height = h;
	enterX = eX;
	enterY =	eY;
//	sprintf(name,"New Dungeon");
	dungeonRating = rating;

	floorIndex = rand() % NUM_OF_DUNGEON_FLOOR_TYPES;
	outerWallIndex = 1 + (rand() % (NUM_OF_DUNGEON_WALL_TYPES-1));
	int wIndex     = 1 + (rand() % (NUM_OF_DUNGEON_WALL_TYPES-1));

	leftWall = new unsigned char[width * height];
	topWall  = new unsigned char[width * height];
	pathMap  = new int[width * height];

//	srand(0);

	for (int i = 0; i < w * h; ++i)
	{
		if (rand() % 3)
			leftWall[i] = 0;
		else
			leftWall[i] = wIndex;
	}

	for (int i = 0; i < w * h; ++i)
	{
		if (rand() % 3)
			topWall[i] = 0;
		else
			topWall[i] = wIndex;
	}

	groundInventory = new Inventory[w * h];

	if (!SetPathMap())
	{
		while (!ForceDungeonContiguous())
			SetPathMap();
	}

	if (SPECIAL_DUNGEON_TEMPORARY & specialFlags)
	{
		Randomize(2);

		topWall [width+width-1] = wIndex;
		leftWall[width-1] = 0;
		topWall [width+width-2] = wIndex;
		leftWall[width-2] = 0;
		topWall [width+width-3] = wIndex;
		leftWall[width-3] = 0;
		topWall [width+width-4] = 0;
		leftWall[width-4] = 0;

		if (!SetPathMap())
		{
			while (!ForceDungeonContiguous())
				SetPathMap();
		}
	}
}

//******************************************************************
void DungeonMap::Randomize(int ratio)
{

	floorIndex = rand() % NUM_OF_DUNGEON_FLOOR_TYPES;
	outerWallIndex = 1 + (rand() % (NUM_OF_DUNGEON_WALL_TYPES-1));
	int wIndex     = 1 + (rand() % (NUM_OF_DUNGEON_WALL_TYPES-1));

	for (int i = 0; i < width * height; ++i)
	{
		if (rand() % ratio)
			leftWall[i] = 0;
		else
			leftWall[i] = wIndex;
	}

	for (int i = 0; i < width * height; ++i)
	{
		if (rand() % ratio)
			topWall[i] = 0;
		else
			topWall[i] = wIndex;
	}

	if (!SetPathMap())
	{
		while (!ForceDungeonContiguous())
			SetPathMap();
	}

}

//******************************************************************
void DungeonMap::Save(FILE *fp)
{
	// save general info
	fprintf(fp,"%s\n", name);
	fprintf(fp,"%d %d %d %d %d\n", dungeonRating, width, height, enterX, enterY);

	// save layout
	fprintf(fp,"%d %d\n", (int) floorIndex, (int) outerWallIndex);

	int lineBreak = 0;
	for (int i = 0; i < width * height; ++i)
	{
		fprintf(fp,"%d %d ", (int) leftWall[i], (int) topWall[i]);
		if (++lineBreak > 20 || i == width * height - 1)
		{
			fprintf(fp,"\n");
			lineBreak = 0;
		}
	}
	// save non-wandering monsters
	BBOSMob *curMob = (BBOSMob *) mobList->GetFirst(0,0,1000);
	while (curMob)
	{
		if (SMOB_MONSTER == curMob->WhatAmI())
		{
			BBOSMonster *monster = (BBOSMonster *) curMob;
			if (!monster->isWandering)
			{
				fprintf(fp,"MONSTER %d %d %d %d\n", 
					     monster->type, monster->subType, 
						  monster->cellX, monster->cellY);
			}
		}
		curMob = (BBOSMob *) mobList->GetNext();
	}

	fprintf(fp,"END\n"); 

	// save master info
	if (masterName[0])
		fprintf(fp,"%s\n", masterName);
	else
		fprintf(fp,"NOMASTER\n");

	if (masterPass[0])
		fprintf(fp,"%s\n", masterPass);
	else
		fprintf(fp,"NOMASTER\n");

	fprintf(fp,"%d %d %d %d %d %d\n", 
			masterTimeout.value.wYear,
			masterTimeout.value.wMonth,
			masterTimeout.value.wDay,
			masterTimeout.value.wDayOfWeek,
			masterTimeout.value.wHour,
			masterTimeout.value.wMinute);
}

//******************************************************************
void DungeonMap::Load(FILE *fp, float version)
{
	if (leftWall)
		delete[] leftWall;
	if (topWall)
		delete[] topWall;

	// load general info
	LoadLineToString(fp, name);

	int tempInt, temp1, temp2, temp3, temp4; //, temp5;

	fscanf(fp,"%d %d %d %d %d", &dungeonRating, &width, &height, &enterX, &enterY);

	SharedSpace::InitNew(width, height, 0,0);

	leftWall = new unsigned char[width * height];
	topWall  = new unsigned char[width * height];
	pathMap  = new int[width * height];

	// load layout
	fscanf(fp,"%d %d", &temp1, &temp2);
	floorIndex = temp1;
	outerWallIndex = temp2;

	for (int i = 0; i < width * height; ++i)
	{
		fscanf(fp,"%d %d", &temp1, &temp2);
		leftWall[i] = temp1;
		topWall[i] = temp2;
//		leftWall[i] = 0;          // TEST!!!!!!!!
//		topWall[i] = 0;
	}

	// load non-wandering monsters
	char tempText[128];

	fscanf(fp,"\n");
	fscanf(fp,"%s", tempText);
	while (!strcmp("MONSTER",tempText))
	{
		fscanf(fp,"%d %d %d %d\n", &temp1, &temp2, &temp3, &temp4);
		BBOSMonster *monster = new BBOSMonster(temp1,temp2, NULL);

		if (temp3 < 0)
			temp3 = 0;
		if (temp3 >= width)
			temp3 = width-1;

		if (temp4 < 0)
			temp4 = 0;
		if (temp4 >= height)
			temp4 = height-1;

		monster->cellX = temp3;
		monster->cellY = temp4;
		monster->targetCellX = temp3;
		monster->targetCellY = temp4;
		monster->spawnX = temp3;
		monster->spawnY = temp4;

		if (0 == temp3 && 0 == temp4)
		{
			int mx, my;
			do
			{
				mx = rand() % (width);
				my = rand() % (height);
			} while (mx < 4 && my < 4);

			monster->cellX = mx;
			monster->cellY = my;
			monster->targetCellX = mx;
			monster->targetCellY = my;
			monster->spawnX = mx;
			monster->spawnY = my;

		}
		mobList->Add(monster);


//		assert(monster->cellX != 0 || monster->cellY != 0);

		fscanf(fp,"%s", tempText);

	}

	// load master info
	fscanf(fp,"\n");
	LoadLineToString(fp, masterName);
	if (!strcmp("NOMASTER",masterName))
		masterName[0] = 0;

	LoadLineToString(fp, masterPass);
	if (!strcmp("NOMASTER",masterPass))
		masterPass[0] = 0;

	fscanf(fp,"%d", &tempInt);
	masterTimeout.value.wYear         = tempInt;
	fscanf(fp,"%d", &tempInt);
	masterTimeout.value.wMonth        = tempInt;
	fscanf(fp,"%d", &tempInt);
	masterTimeout.value.wDay          = tempInt;
	fscanf(fp,"%d", &tempInt);
	masterTimeout.value.wDayOfWeek    = tempInt;
	fscanf(fp,"%d", &tempInt);
	masterTimeout.value.wHour         = tempInt;
	fscanf(fp,"%d\n", &tempInt);
	masterTimeout.value.wMinute       = tempInt;

	masterTimeout.value.wSecond       = 0;

	masterTimeout.value.wMilliseconds = 0;

	int col = dungeonRating;
	if (0 == col)
		col = 1;

	// add wandering monsters
	for (int m = 0; m < (height * width) / 12;)
	{
		int t  = rand() % (NUM_OF_MONSTERS/2);
		int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

		if (monsterData[t][t2].name[0] && monsterData[t][t2].dungeonType >= col - 3 &&
			 monsterData[t][t2].dungeonType <= col - 1)
		{
			int mx, my;
			do
			{
				mx = rand() % (width);
				my = rand() % (height);
			} while (mx < 4 && my < 4);

			BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
			monster->isWandering = TRUE;
			monster->cellX = mx;
			monster->cellY = my;
			monster->targetCellX = mx;
			monster->targetCellY = my;
			monster->spawnX = mx;
			monster->spawnY = my;
			mobList->Add(monster);

			assert(monster->cellX != 0 || monster->cellY != 0);

			++m;
		}
	}

	groundInventory = new Inventory[width * height];

	// save non-wandering monsters
	BBOSMob *curMob = (BBOSMob *) mobList->GetFirst(0,0,1000);
	while (curMob)
	{
		if (SMOB_MONSTER == curMob->WhatAmI())
		{
			BBOSMonster *monster = (BBOSMonster *) curMob;
			if (0 == monster->cellX && 0 == monster->cellY)
			{
				monster = (BBOSMonster *) curMob;
			}
		}
		curMob = (BBOSMob *) mobList->GetNext();
	}

}

//******************************************************************
int DungeonMap::CanMove(int srcX, int srcY, int dstX, int dstY)
{
	// can't move out of bounds
	if (dstX < 0 || dstX >= width)
		return FALSE;
	if (dstY < 0 || dstY >= height)
		return FALSE;
/*
	// can't move diagonally
	if (dstX != srcX && dstY != srcY)
		return FALSE;

	// can't move more than one square
	int dist = abs(dstX - srcX) + abs(dstY - srcY);
	if (dist > 1)
		return FALSE;
*/
	// can't move left if there's a wall in the way
	if (dstX < srcX && leftWall[srcY * width + srcX])
		return FALSE;

	// can't move right if there's a wall in the way
	if (dstX > srcX && leftWall[dstY * width + dstX])
		return FALSE;

	// can't move up if there's a wall in the way
	if (dstY < srcY && topWall[srcY * width + srcX])
		return FALSE;

	// can't move down if there's a wall in the way
	if (dstY > srcY && topWall[dstY * width + dstX])
		return FALSE;

	int dist = abs(dstX - srcX) + abs(dstY - srcY);
	if (dist > 1)
	{

	}
	return TRUE;
}

//*******************************************************************************
void DungeonMap::DoMonsterDrop(BBOSMonster *monster)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDrop(inv, monster);

}

//*******************************************************************************
void DungeonMap::DoMonsterDropSpecial(BBOSMonster *monster, int type)
{

	Inventory *inv = &(groundInventory[width*monster->cellY + monster->cellX]);

	SharedSpace::DoMonsterDropSpecial(inv, monster, type);

}

//*******************************************************************************
void DungeonMap::DoChestDrop(BBOSChest *chest)
{
	char tempText[1028];

	Inventory *inv = &(groundInventory[width*chest->cellY + chest->cellX]);

	if (SPECIAL_DUNGEON_TEMPORARY & specialFlags)
	{
		int da = Bracket((int)tempPower/25,0,10);

		InventoryObject *iObject;
		sprintf(tempText, "%s Heart Gem", dropAdj2[da]);
		iObject = new InventoryObject(
					INVOBJ_GEOPART,0,tempText);
		InvGeoPart *exIn = (InvGeoPart *)iObject->extra;
		exIn->type     = 1; // heart gem
		exIn->power    = 1 + da;

		iObject->mass = 0.0f;
		iObject->value = 400 + da * 100;
		iObject->amount = 1;
		inv->AddItemSorted(iObject);

		// possibly add a depth plaque
		int curDepth = tempPower/5;
		curDepth = curDepth * 5 + 20;
		int lastDepth = 1;
		FILE *fp = fopen("deepestEarthKey.dat","r");
		if (fp)
		{
			fscanf(fp,"%d",&lastDepth);
			fclose(fp);
		}

		if (lastDepth < 1)
			lastDepth = 1;

		if (lastDepth < curDepth)
		{
			sprintf(tempText,"First To %dm Cup", curDepth);
			iObject = new InventoryObject(INVOBJ_SIMPLE,0,tempText);
			iObject->mass = 1.0f;
			iObject->value = 10 * curDepth;
			iObject->amount = 1;
			inv->AddItemSorted(iObject);

			FILE *fp = fopen("deepestEarthKey.dat","w");
			if (fp)
			{
				fprintf(fp,"%d",curDepth);
				fclose(fp);
			}

			FILE *source = fopen("depthCupLog.txt","a");
		
			/* Display operating system-style date and time. */
			_strdate( tempText );
			fprintf(source, "%s, ", tempText );
			_strtime( tempText );
			fprintf(source, "%s, ", tempText );

			fprintf(source,"%d, %d\n", curDepth, name);
			fclose(source);
		}
	}
	else
	{
		int count = dungeonRating + (rand() % 3) + 1;
		if (0 == masterName[0])
			count = 1;

		for (int i = 0; i < count; ++i)
		{
	//		char tempText[1024];
	//		sprintf(tempText,"%s %s", dropAdj[adj], dropName[noun]);
	//		++adj;
	//		++noun;
			InventoryObject *iObject;
			if (rand() % 10)
			{
				iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Golden Necklace");
				iObject->value = 1000;
			}
			else
			{
				iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
				InvIngredient *exIn = (InvIngredient *)iObject->extra;
				exIn->type     = INGR_BLUE_DUST;
				exIn->quality  = 1;
			
				iObject->value = 1000;
			}
			iObject->mass = 1.0f;
			iObject->amount = 1;
			inv->AddItemSorted(iObject);
		}

//		char tempText[1028];
		FILE *source = fopen("chests.txt","a");
		
		/* Display operating system-style date and time. */
		_strdate( tempText );
		fprintf(source, "%s, ", tempText );
		_strtime( tempText );
		fprintf(source, "%s, ", tempText );

		if (masterName[0])
		{
			fprintf(source,"LOOT, %s, 0, 0, %d\n",
				 masterName, count * 250);
		}
		else
		{
			fprintf(source,"LOOT, NO MISTRESS, 0, 0, %d\n", count * 5);
		}
		fclose(source);
	}

	// tell client about an item sack
	MessMobAppear messMA;
	messMA.mobID = (unsigned long) inv;
	messMA.type = SMOB_ITEM_SACK;
	messMA.x = chest->cellX;
	messMA.y = chest->cellY;
	SendToEveryoneNearBut(0, chest->cellX, chest->cellY,
					sizeof(messMA),(void *)&messMA);

}

//*******************************************************************************
Inventory *DungeonMap::GetGroundInventory(int x, int y)
{
	Inventory *inv = &(groundInventory[width * y + x]);

	return inv;
}

//*******************************************************************************
int DungeonMap::NewPathMapValue(int curValue, int neighborValue)
{
	if (neighborValue != -1)
	{
		if (-1 == curValue || curValue > neighborValue + 1)
		{
			curValue = neighborValue + 1;
		}
	}

	return curValue;
}

//*******************************************************************************
int DungeonMap::SetPathMap(void)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pathMap[width * y + x] = -1; // unconnected state
		}
	}

	pathMap[0] = 0; // start in door square

	int done = FALSE;
	while (!done)
	{
		done = TRUE;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (-1 == pathMap[width * y + x])  // if unconnected
				{
					// see if we can connect to a connected square
/*
					if (CanMove(x,y, x, y-1))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * y + x-1] + 1)
						{
							pathMap[width * y + x] = pathMap[width * y + x-1] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x, y+1))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * y + x+1] + 1)
						{
							pathMap[width * y + x] = pathMap[width * y + x+1] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x-1, y))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * (y-1) + x] + 1)
						{
							pathMap[width * y + x] = pathMap[width * (y-1) + x] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x+1, y))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * (y+1) + x] + 1)
						{
							pathMap[width * y + x] = pathMap[width * (y+1) + x] + 1;
							done = FALSE;
						}
					}
*/
					if (CanMove(x,y, x-1, y))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * y + x-1]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x+1, y))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * y + x+1]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x, y-1))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * (y-1) + x]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x, y+1))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * (y+1) + x]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
				}
			}
		}
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (-1 == pathMap[width * y + x]) // unconnected state
				return 0;
		}
	}

	return 1;
}

//*******************************************************************************
int DungeonMap::ChangeWall(int isLeft, int x, int y)
{
	if (x >= 0 && 
		 x < width &&
	    y >= 0 && 
		 y < height)
	{

		unsigned char *theWall;
		if (isLeft)
			theWall = &leftWall[y * width + x];
		else
			theWall = &topWall[y * width + x];

		*theWall += 1;
		if (*theWall >= NUM_OF_DUNGEON_WALL_TYPES)
			*theWall = 0;
		else if (!SetPathMap())
		{
			*theWall = 0;
			return -1;
		}

		return (*theWall) + 1;
	}

	return -2;
}
  

//*******************************************************************************
int DungeonMap::ForceDungeonContiguous(void)
{

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (-1 == pathMap[width * y + x]) // unconnected state
			{
				leftWall[y * width + x] = 0;
				topWall [y * width + x] = 0;
				return 0;
			}
		}
	}

	return 1;
}


//*******************************************************************************
int DungeonMap::CanEdit(BBOSAvatar *ca)
{

	if ( IsSame(masterName, ca->name) && IsSame(masterPass, ca->pass) )
		return TRUE;

	if (SPECIAL_DUNGEON_MODERATED & specialFlags && 
		 (ACCOUNT_TYPE_MODERATOR == ca->accountType ||
		  ACCOUNT_TYPE_ADMIN     == ca->accountType))
		return TRUE;

	return FALSE;
}

/* end of file */



