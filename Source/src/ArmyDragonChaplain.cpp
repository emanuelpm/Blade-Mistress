
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmyDragonChaplain.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "TotemData.h"


//******************************************************************
//******************************************************************
ArmyChaplain::ArmyChaplain(SharedSpace *s, int centerX, int centerY, int sX, int sY, int numChaplain) : BBOSArmy(s, centerX, centerY)
{
	// create an army of chaplain + acolytes!

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = -1; // special, Dragon Chaplain
	am->subType = 0;
	am->targetX = centerX;
	am->targetY = centerY;
	atEase.Append(am);

	// add leutenants
	for (int i = 0; i < 8; ++i)
	{
		am = new ArmyMember();
		am->quality = 1;
		am->type = -2;
		am->subType = 5; // Dragon Acolytes
		am->targetX = centerX + spaceOffset[i][0];
		am->targetY = centerY + spaceOffset[i][1];
		atEase.Append(am);
	}

}

//******************************************************************
ArmyChaplain::~ArmyChaplain()
{

}

//******************************************************************
BBOSMonster *ArmyChaplain::MakeSpecialMonster(ArmyMember *curMember)
{
	if (-1 == curMember->type)
	{
		// create Dragon Chaplain
		BBOSMonster *monster = new BBOSMonster(7,4, this);  // make overlord dragon

		sprintf(monster->uniqueName,"Dragon Chaplain");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 15000;
		monster->damageDone         = 24;
		monster->toHit              = 50;
		monster->defense            = 60;
		monster->dropAmount         = 20;

		// add 2 red dusts (formerly green)
		InventoryObject *iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
		InvIngredient *exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_RED_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 2;
		monster->inventory->objects.Append(iObject);

		monster->r = 100;
		monster->g = 255;
		monster->b = 100;

		return monster;
	}

	if (-2 == curMember->type)
	{
		// create Dragon Prelates
		BBOSMonster *monster = new BBOSMonster(7,3, this);  // make Dragon Archon

		sprintf(monster->uniqueName,"Dragon Prelate");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 2500;
		monster->damageDone         = 22;
		monster->toHit              = 45;
		monster->defense            = 45;
		monster->dropAmount         = 2;

		monster->r = 100;
		monster->g = 255;
		monster->b = 100;

		return monster;
	}


	return NULL;
}

//******************************************************************
void ArmyChaplain::KillMember(ArmyMember * curMember, DoublyLinkedList *srcList)
{
	++totalArmyMembersKilled;

	curMember->monster = (BBOSMonster *)30;
//	srcList->Remove(curMember);
	curMember->isDead = TRUE;

	if (curMember->quality < 10)
		curMember->deadTimer = timeGetTime() + 1000 * 120; // Prelates respawn almost instantly
	else
		curMember->deadTimer = timeGetTime() + 
			curMember->quality * (curMember->quality + 1) * 1000 * 50;
}


/* end of file */



