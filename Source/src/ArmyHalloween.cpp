
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmyHalloween.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "TotemData.h"


//******************************************************************
//******************************************************************
ArmyHalloween::ArmyHalloween(SharedSpace *s, int centerX, int centerY, int sX, int sY, int numHalloween) : BBOSArmy(s, centerX, centerY)
{
	// create an army of scary halloween monsters!

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = -1; // special, Mas Hallow the giant skeleton
	am->subType = 0;
	am->targetX = centerX;
	am->targetY = centerY;
	atEase.Append(am);

	// add leutenants
	for (int i = 0; i < 4; ++i)
	{
		am = new ArmyMember();
		am->quality = 6;
		am->type = -2;
		am->subType = 5; // demon spiders
		am->targetX = centerX + spaceOffset[i+4][0];
		am->targetY = centerY + spaceOffset[i+4][1];
		atEase.Append(am);
	}

	for (int i = 0; i < 4; ++i)
	{
		am = new ArmyMember();
		am->quality = 6;
		am->type = 0;
		am->subType = 3; // banshees
		am->targetX = centerX + spaceOffset[i][0];
		am->targetY = centerY + spaceOffset[i][1];
		atEase.Append(am);
	}

	// add privates
	for (int i = 0; i < 30; ++i)
	{
		am = new ArmyMember();
		am->quality = 3;
		am->type = 4;
		am->subType = 0; // skeletons
		am->targetX = centerX + spaceOffset[i % 8][0] * 3 + (rand() % 3) - 1;
		am->targetY = centerY + spaceOffset[i % 8][1] * 3 + (rand() % 3) - 1;
		atEase.Append(am);
	}

}

//******************************************************************
ArmyHalloween::~ArmyHalloween()
{

}

//******************************************************************
BBOSMonster *ArmyHalloween::MakeSpecialMonster(ArmyMember *curMember)
{
	if (-1 == curMember->type)
	{
		// create Mas Hallow
		BBOSMonster *monster = new BBOSMonster(4,0, this);  // make common skeleton

		sprintf(monster->uniqueName,"Mas Hallow");
		monster->sizeCoeff = 3.0f;

		monster->health = monster->maxHealth = 10000;
		monster->damageDone         = 45;
		monster->toHit              = 48;
		monster->defense            = 48;
		monster->dropAmount         = 40;

		// add pumpkin totem
		InventoryObject *iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		InvTotem *extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 17; // pumpkin

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14.0f + 1.0f;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14.0f + 1.0f + (extra->quality-12) * 1600.0f;
		iObject->amount = 2;
		UpdateTotem(iObject);
		monster->inventory->objects.Append(iObject);

		return monster;
	}

	if (-2 == curMember->type)
	{
		// create Super spiders
		BBOSMonster *monster = new BBOSMonster(8,5, this);  // make demon spider

		sprintf(monster->uniqueName,"Pumpkin Spider");
		monster->sizeCoeff = 1.3f;

		monster->health = monster->maxHealth = 3000;
		monster->damageDone         = 20;
		monster->toHit              = 30;
		monster->defense            = 30;
		monster->dropAmount         = 20;

		monster->r = 128;
		monster->g = 255;
		monster->b = 26;

		if (5 == rand() % 20)
		{
			// add pumpkin totem
			InventoryObject *iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
			InvTotem *extra = (InvTotem *)iObject->extra;
			extra->type     = 0;
			extra->quality  = 17; // pumpkin

			iObject->mass = 0.0f;
			iObject->value = extra->quality * extra->quality * 14.0f + 1.0f;
			if (extra->quality > 12)
				iObject->value = extra->quality * extra->quality * 14.0f + 1.0f + (extra->quality-12) * 1600.0f;
			iObject->amount = 1;
			UpdateTotem(iObject);
			monster->inventory->objects.Append(iObject);
		}
		return monster;
	}


	return NULL;
}


/* end of file */



