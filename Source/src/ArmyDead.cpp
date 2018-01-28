
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmyDead.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "TotemData.h"


//******************************************************************
//******************************************************************
ArmyDead::ArmyDead(SharedSpace *s, int centerX, int centerY, int sX, int sY, int armyIndex) : BBOSArmy(s, centerX, centerY)
{
	// create an army of the dead

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = -armyIndex; // special, general
	am->subType = 0;
	am->targetX = centerX;
	am->targetY = centerY;
	atEase.Append(am);

	// add leutenants
	for (int i = 0; i < 4; ++i)
	{
		am = new ArmyMember();
		am->quality = 4;
		am->type = 19;
		am->subType = 2; 
		am->targetX = centerX + spaceOffset[i+4][0];
		am->targetY = centerY + spaceOffset[i+4][1];
		atEase.Append(am);
	}

	if (1 == armyIndex) // if anubis's army
	{
		for (int i = 0; i < 4; ++i)
		{
			am = new ArmyMember();
			am->quality = 4;
			am->type = 19;
			am->subType = 2; 
			am->targetX = centerX + spaceOffset[i][0];
			am->targetY = centerY + spaceOffset[i][1];
			atEase.Append(am);
		}

	}

	for (int i = 0; i < 8; ++i)
	{
		am = new ArmyMember();
		am->quality = 4;
		am->type = 19;
		am->subType = 1; // sargent
		am->targetX = centerX + spaceOffset[i][0];
		am->targetY = centerY + spaceOffset[i][1];
		atEase.Append(am);
	}

	// add privates
	for (int i = 0; i < 80 - armyIndex * 10; ++i)
	{
		am = new ArmyMember();
		am->quality = 3;
		am->type = 19;
		am->subType = 0; 
		am->targetX = centerX + spaceOffset[i % 8][0] * 3 + (rand() % 3) - 1;
		am->targetY = centerY + spaceOffset[i % 8][1] * 3 + (rand() % 3) - 1;
		atEase.Append(am);
	}

}

//******************************************************************
ArmyDead::~ArmyDead()
{

}

//******************************************************************
BBOSMonster *ArmyDead::MakeSpecialMonster(ArmyMember *curMember)
{
	if (-1 == curMember->type)
	{
		// create anubis
		BBOSMonster *monster = new BBOSMonster(20,0, this); 
		sprintf(monster->uniqueName,monsterData[20][0].name);

		// add totems
		InventoryObject *iObject;
		/*
		InventoryObject *iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		InvTotem *extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 18; // undead

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14 + 1;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
		iObject->amount = 2;
		UpdateTotem(iObject);
		monster->inventory->objects.Append(iObject);
		*/
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
		InvIngredient *exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_BLUE_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_RED_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);
/*
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_GREEN_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);
*/
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_WHITE_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);
/*
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_BLACK_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);
*/
		return monster;
	}

	InventoryObject *iObject;
	InvIngredient *exIn;
	InvTotem *extra;

	if (curMember->type <= -2)
	{
		// create Bone Boss
		BBOSMonster *monster = new BBOSMonster(19, -1 * curMember->type + 1, this); 
		sprintf(monster->uniqueName,monsterData[19][-1 * curMember->type + 1].name);


		// add totem?
		switch(rand() % 3)
		{
		case 0:
			iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
			extra = (InvTotem *)iObject->extra;
			extra->type     = 0;
			extra->quality  = 18; // undead

			iObject->mass = 0.0f;
			iObject->value = extra->quality * extra->quality * 14.0f + 1.0f;
			if (extra->quality > 12)
				iObject->value = extra->quality * extra->quality * 14.0f + 1.0f + (extra->quality-12) * 1600.0f;
			iObject->amount = 1;
			UpdateTotem(iObject);
			monster->inventory->objects.Append(iObject);
			break;

		case 1:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_RED_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			monster->inventory->objects.Append(iObject);
			break;

		case 2:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_WHITE_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			monster->inventory->objects.Append(iObject);
			break;
/*
		case 3:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_GREEN_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			monster->inventory->objects.Append(iObject);
			break;

		case 4:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_BLACK_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			monster->inventory->objects.Append(iObject);
			break;
  */
		}
		return monster;
	}

	return NULL;
}


/* end of file */



