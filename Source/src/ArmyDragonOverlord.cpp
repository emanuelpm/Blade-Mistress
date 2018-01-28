
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmyDragonOverlord.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "TotemData.h"
#include "inventory.h"
#include "StaffData.h"



//******************************************************************
//******************************************************************
ArmyOverlord::ArmyOverlord(SharedSpace *s, int centerX, int centerY, int sX, int sY, int numOverlord) : BBOSArmy(s, centerX, centerY)
{
	// create an army of overlord + acolytes!

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = -1; // special, Dragon Overlord
	am->subType = 0;
	am->targetX = centerX;
	am->targetY = centerY;
	atEase.Append(am);

	// add leutenants
	for (int i = 0; i < 8; ++i)
	{
		am = new ArmyMember();
		am->quality = 4;
		am->type = -2;
		am->subType = 5; // Dragon Acolytes
		am->targetX = centerX + spaceOffset[i][0];
		am->targetY = centerY + spaceOffset[i][1];
		atEase.Append(am);
	}

}

//******************************************************************
ArmyOverlord::~ArmyOverlord()
{

}

//******************************************************************
BBOSMonster *ArmyOverlord::MakeSpecialMonster(ArmyMember *curMember)
{
	if (-1 == curMember->type)
	{
		// create Dragon Overlord
		BBOSMonster *monster = new BBOSMonster(7,4, this);  // make overlord dragon

		sprintf(monster->uniqueName,"Dragon Overlord");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 80000;
		monster->damageDone         = 40;
		monster->toHit              = 90;
		monster->defense            = 135;
		monster->dropAmount         = 20;

		monster->magicResistance = 0.5f;

		// add 3 spotted eggs
		for (int i = 0; i < 3; ++i)
		{
			int color = rand() % 6;
			InventoryObject *iObject = new InventoryObject(
						INVOBJ_EGG,0,dragonInfo[2][color].eggName);
			iObject->mass = 1.0f;
			iObject->value = 1000;

			InvEgg *im = (InvEgg *) iObject->extra;
			im->type   = color;
			im->quality = 2;

			monster->inventory->objects.Append(iObject);
		}

		monster->r = 255;
		monster->g = 100;
		monster->b = 100;

		return monster;
	}

	if (-2 == curMember->type)
	{
		// create Dragon Acolytes
		BBOSMonster *monster = new BBOSMonster(7,3, this);  // make Dragon Archon

		sprintf(monster->uniqueName,"Dragon Knight");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 6000;
		monster->damageDone         = 32;
		monster->toHit              = 65;
		monster->defense            = 115;
		monster->dropAmount         = 10;

		monster->magicResistance = 0.25f;

		monster->r = 255;
		monster->g = 100;
		monster->b = 100;

		return monster;
	}


	return NULL;
}

/* end of file */



