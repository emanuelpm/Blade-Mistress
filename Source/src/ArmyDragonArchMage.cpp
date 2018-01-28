
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmyDragonArchMage.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"
#include "TotemData.h"
#include "inventory.h"
#include "StaffData.h"



//******************************************************************
//******************************************************************
ArmyArchMage::ArmyArchMage(SharedSpace *s, int centerX, int centerY, int sX, int sY, int numArchMage) : BBOSArmy(s, centerX, centerY)
{
	// create an army of archMage + acolytes!

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = -1; // special, Dragon ArchMage
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
ArmyArchMage::~ArmyArchMage()
{

}

//******************************************************************
BBOSMonster *ArmyArchMage::MakeSpecialMonster(ArmyMember *curMember)
{
	if (-1 == curMember->type)
	{
		// create Dragon ArchMage
		BBOSMonster *monster = new BBOSMonster(7,4, this);  // make overlord dragon

		sprintf(monster->uniqueName,"Dragon ArchMage");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 25000;
		monster->damageDone         = 22;
		monster->toHit              = 60;
		monster->defense            = 50;
		monster->dropAmount         = 20;

		// add 3 staffs
		InventoryObject *iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
		InvStaff *extra = (InvStaff *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 3;

		iObject->mass = 0.0f;
		iObject->value = 500 * (3 + 1) * (3 + 1);
		iObject->amount = 3;
		UpdateStaff(iObject, 0);
		monster->inventory->objects.Append(iObject);

		monster->r = 100;
		monster->g = 100;
		monster->b = 255;

		return monster;
	}

	if (-2 == curMember->type)
	{
		// create Dragon Acolytes
		BBOSMonster *monster = new BBOSMonster(7,3, this);  // make Dragon Archon

		sprintf(monster->uniqueName,"Dragon Acolyte");
		monster->sizeCoeff = 1.0f;

		monster->health = monster->maxHealth = 3000;
		monster->damageDone         = 20;
		monster->toHit              = 55;
		monster->defense            = 80;
		monster->dropAmount         = 6;

		monster->r = 100;
		monster->g = 100;
		monster->b = 255;

		return monster;
	}


	return NULL;
}

/* end of file */



