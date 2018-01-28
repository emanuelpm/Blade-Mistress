
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Snpc.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "totemData.h"
#include "staffData.h"

TraderGoods * BBOSNpc::goods = NULL;
int BBOSNpc::goodsRefCount = 0;



//******************************************************************
BBOSNpc::BBOSNpc(int npcType) : BBOSMob(npcType,"NPC")
{
	if (goodsRefCount < 1)
	{
		goods = new TraderGoods();
	}
	++goodsRefCount;

	if (SMOB_TRAINER == npcType)
		inventory = new Inventory(MESS_INVENTORY_TRAINER, this);
	else
		inventory = new Inventory(MESS_INVENTORY_TRADER, this);


//	isTrader = isTrad;
//	if (!isTrader)
//		do_id = SMOB_TRAINER;

	cellX = cellY = 41;
	targetCellX = targetCellY = 41;
	moveStartTime = 0;
	lastReplenishTime = 0;
	isMoving = FALSE;

	if (SMOB_TRAINER == WhatAmI())
	{
		InventoryObject *iObject;

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Dodging");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Sword Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Katana Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Chaos Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Mace Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Bladestaff Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Claw Mastery");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Explosives");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Swordsmith");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);
		   
		iObject = new InventoryObject(INVOBJ_SKILL,0,"Katana Expertise");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Chaos Expertise");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Mace Expertise");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Bladestaff Expertise");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Claw Expertise");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Weapon Dismantle");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Bear Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Wolf Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Eagle Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Snake Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Frog Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Sun Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Moon Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Turtle Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Evil Magic");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Geomancy");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);

		iObject = new InventoryObject(INVOBJ_SKILL,0,"Totem Shatter");
		iObject->mass = 0.0f;
		iObject->value = 1.0f;
		iObject->amount = 100;
		inventory->objects.Append(iObject);
	}


}

//******************************************************************
BBOSNpc::~BBOSNpc()
{
	delete inventory;

	--goodsRefCount;
	if (goodsRefCount < 1)
	{
		delete goods;
	}

}

//******************************************************************
void BBOSNpc::Tick(SharedSpace *ss)
{
	DWORD delta;
	DWORD now = timeGetTime();

	if (SMOB_TRADER == WhatAmI())
	{
		delta = now - lastReplenishTime;

		if (0 == lastReplenishTime || now < lastReplenishTime) // || now == moveStartTime)
		{
			delta = 1000 * 60 * 4 * 8 + 1;
		}

		// replenish inventory!
		if (delta > 1000 * 60 * 4 * level)	
		{
			if (PlayerInMySquare(ss))
			{
				lastReplenishTime += 1000 * 10;
				return;
			}

			lastReplenishTime = now;

			goods->Replenish(this);

			InventoryObject *iObject;

			// TEST ********
			/*
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
			InvIngredient *exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_BLUE_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 100;
			iObject->amount = 100;
			inventory->objects.Append(iObject);
			
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_RED_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 100;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_WHITE_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 100;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_BLACK_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 100;
			iObject->amount = 100;
			inventory->objects.Append(iObject);
			
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_GREEN_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 100;
			iObject->amount = 100;
			inventory->objects.Append(iObject);
			*/

			// add some random totems here
			for (int i = rand() % 3; i < 7; ++i)
			{
				int type = rand() % TOTEM_MAX;
				int qual = rand() % (TOTEM_SELLABLE_QUALITY_MAX / 5) + (TOTEM_SELLABLE_QUALITY_MAX / 5) * (level-1);
				if (qual >= TOTEM_SELLABLE_QUALITY_MAX)
					qual = TOTEM_SELLABLE_QUALITY_MAX - 1;  // pumpkin and beyond; don't sell

//				char name[128];
//				sprintf(name,"%s %s Totem", totemQualityName[qual], totemTypeName[type]);

				iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
				InvTotem *extra = (InvTotem *)iObject->extra;
				extra->type     = type;
				extra->quality  = qual;

				iObject->mass = 0.0f;
				iObject->value = qual * qual * 14 + 1;
				if (qual > 12)
					iObject->value = qual * qual * 14 + 1 + (qual-12) * 1600;
				iObject->amount = rand() % 10 + 1;
				UpdateTotem(iObject);
				inventory->AddItemSorted(iObject);
			}

			// add bomb making supplies here
			
			{
				iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Charcoal Brick");
				InvExplosive *extra = (InvExplosive *)iObject->extra;
				extra->type     = 0;
				extra->quality  = 1.0f; // explosive power of this one item
				iObject->value  = 10;
				iObject->amount = rand() % 30 + 50;
				inventory->AddItemSorted(iObject);
			}
			{
				iObject = new InventoryObject(INVOBJ_FUSE,0,"Twisted Paper");
				InvFuse *extra = (InvFuse *)iObject->extra;
				extra->type     = 0;
				extra->quality  = 1.0f;
				iObject->value  = 10;
				iObject->amount = rand() % 30 + 50;
				inventory->AddItemSorted(iObject);
			}

			// add dragonscales
			{
				iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Ancient Dragonscale");
				iObject->value  = 100000;
				iObject->amount = rand() % 30 + 40;
				inventory->AddItemSorted(iObject);

				iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Dragon Orchid");
				iObject->value  = 10000;
				iObject->amount = rand() % 30 + 50;
				inventory->AddItemSorted(iObject);

				iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Demon Amulet");
				iObject->value  = 50000;
				iObject->amount = 1;
				inventory->AddItemSorted(iObject);
			}
			
			// add some staffs here
			{
				for (int qual = 0; qual < 3; ++qual)
				{
					iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
					InvStaff *extra = (InvStaff *)iObject->extra;
					extra->type     = 0;
					extra->quality  = qual;

					iObject->mass = 0.0f;
					iObject->value = 500 * (qual + 1) * (qual + 1);
					iObject->amount = rand() % 10 + 1;
					UpdateStaff(iObject, 0);
					inventory->AddItemSorted(iObject);
				}
			}

			// add recall scrolls
			char tempText[1024];
			sprintf(tempText,"%s Recall", townList[townIndex].name);
			iObject = new InventoryObject(INVOBJ_POTION,0,tempText);
			InvPotion *extra = (InvPotion *)iObject->extra;
			extra->type      = POTION_TYPE_RECALL;
			extra->subType   = townIndex;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 20 + (rand() % 10);
			inventory->AddItemSorted(iObject);

			// add beads
			for (int da = 1; da < 6; ++da)
			{
				sprintf(tempText,"%s Bead", dropAdj2[da-1]);
				iObject = new InventoryObject(INVOBJ_GEOPART, 0, tempText);
				InvGeoPart *exIn = (InvGeoPart *)iObject->extra;
				exIn->type     = 0;
				exIn->power    = da;

				iObject->mass = 0.0f;
				iObject->value = 1000 * da * da;
				iObject->amount = 100/da;
				inventory->AddItemSorted(iObject);
			}

		}
	}

	if (SMOB_TRAINER == WhatAmI())
	{
		delta = now - lastReplenishTime;

		if (0 == lastReplenishTime || now < lastReplenishTime) // || now == moveStartTime)
		{
			delta = 1000 * 60 * 4 + 1;
		}

		// replenish inventory!
		if (delta > 1000 * 60 * 4)	
		{
			if (PlayerInMySquare(ss))
			{
				lastReplenishTime += 1000 * 10;
				return;
			}

			lastReplenishTime = now;

			// throw away old stuff
			InventoryObject *iObject = (InventoryObject *) inventory->objects.First();
			while (iObject)
			{
				inventory->objects.Remove(iObject);
				delete iObject;
				iObject = (InventoryObject *) inventory->objects.First();
			}


			iObject = new InventoryObject(INVOBJ_SKILL,0,"Dodging");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Sword Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Katana Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Chaos Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Mace Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Bladestaff Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Claw Mastery");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Explosives");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Swordsmith");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);
			   
			iObject = new InventoryObject(INVOBJ_SKILL,0,"Katana Expertise");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Chaos Expertise");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Mace Expertise");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Bladestaff Expertise");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Claw Expertise");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Weapon Dismantle");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Bear Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Wolf Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Eagle Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Snake Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Frog Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Sun Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Moon Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Turtle Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Evil Magic");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Geomancy");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);

			iObject = new InventoryObject(INVOBJ_SKILL,0,"Totem Shatter");
			iObject->mass = 0.0f;
			iObject->value = 1.0f;
			iObject->amount = 100;
			inventory->objects.Append(iObject);
		}
	}
}

//******************************************************************
int BBOSNpc::PlayerInMySquare(SharedSpace *ss)
{
	BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
	while (curMob)
	{
		if (curMob->cellX == cellX && curMob->cellY == cellY)
			return TRUE;

		curMob = (BBOSMob *) ss->avatars->Next();
	}

	return FALSE;
}



/* end of file */



