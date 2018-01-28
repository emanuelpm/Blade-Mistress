
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./helper/GeneralUtils.h"
#include "BBO-Savatar.h"
#include "BBO-Snpc.h"
#include "BBO-Smonster.h"
#include "BBO-Sgenerator.h"
#include "BBO-Stower.h"
#include "monsterData.h"
#include "StaffData.h"
#include "version.h"

#include "SharedSpace.h"

char dropAdj[10][32] =
{
	{"Stinking"},
	{"Grungy"},
	{"Plain"},
	{"Shiny"},
	{"Glimmering"},
	{"Exceptional"},
	{"Glorious"},
	{"Royal"},
	{"Fantastic"},
	{"Ultimate"}
};

char dropAdj2[11][32] =
{
	{"Plain"},
	{"Marbled"},
	{"Shiny"},
	{"Translucent"},
	{"Glimmering"},
	{"Fiery"},
	{"Exceptional"},
	{"Glorious"},
	{"Royal"},
	{"Fantastic"},
	{"Ultimate"}
};

char dropName[10][32] =
{
	{"Fish Head"},
	{"Stick"},
	{"Carving"},
	{"Trinket"},
	{"Symbol"},
	{"Gold Band"},
	{"Pearl"},
	{"Sapphire"},
	{"Ruby"},
	{"Diamond"}
};

//******************************************************************
SharedSpace::SharedSpace(int doid, char *doname, NetWorldRadio * ls) : DataObject(doid,doname)
{

	lserver = ls;

	mobList = NULL;
	avatars    = new DoublyLinkedList();
	generators = new DoublyLinkedList();

	groundInventory = NULL;

	sizeX =  sizeY = 0;

}

//******************************************************************
SharedSpace::~SharedSpace()
{

	delete mobList;
	delete avatars;
	delete generators;

	if (groundInventory)
		delete[] groundInventory;

}

//******************************************************************
void SharedSpace::InitNew(int w, int h, int eX, int eY)
{
	mobList = new MapList(w,h);
}

//******************************************************************
void SharedSpace::Save(void)
{
}

//******************************************************************
void SharedSpace::Load(void)
{
}

//******************************************************************
int SharedSpace::CanMove(int srcX, int srcY, int dstX, int dstY)
{

	return TRUE;
}

//*******************************************************************************
void SharedSpace::SendToEveryoneBut(int handleToExclude, int size, const void *dataPtr)
{
	std::vector<TagID> tempReceiptList;

	tempReceiptList.clear();

	BBOSMob *curMob = (BBOSMob *) avatars->First();
	while (curMob)
	{
		if (SMOB_AVATAR == curMob->WhatAmI())
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
			if (curAvatar->socketIndex != handleToExclude)
			{
				tempReceiptList.push_back(curAvatar->socketIndex);
			}
		}
		curMob = (BBOSMob *) avatars->Next();
	}

  	lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);

}

//*******************************************************************************
void SharedSpace::IgnorableSendToEveryone(BBOSAvatar *srcAvatar, int size, const void *dataPtr)
{
	std::vector<TagID> tempReceiptList;

	tempReceiptList.clear();

	BBOSMob *curMob = (BBOSMob *) avatars->First();
	while (curMob)
	{
		if (SMOB_AVATAR == curMob->WhatAmI())
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
			if (!curAvatar->IsContact(
				   srcAvatar->charInfoArray[srcAvatar->curCharacterIndex].name,
					CONTACT_IGNORE)
			   )
			{
				if (curAvatar->chatChannels & srcAvatar->chatChannels)
					tempReceiptList.push_back(curAvatar->socketIndex);
			}
		}
		curMob = (BBOSMob *) avatars->Next();
	}

  	lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);

}

//*******************************************************************************
void SharedSpace::SendToEveryFriend(BBOSAvatar *srcAvatar, int size, const void *dataPtr)
{
	std::vector<TagID> tempReceiptList;

	tempReceiptList.clear();

	BBOSMob *curMob = (BBOSMob *) avatars->First();
	while (curMob)
	{
		if (SMOB_AVATAR == curMob->WhatAmI())
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
			if (srcAvatar->IsContact(
				   curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
					CONTACT_FRIEND)
			   )
			{
				if (!curAvatar->IsContact(
					   srcAvatar->charInfoArray[srcAvatar->curCharacterIndex].name,
						CONTACT_IGNORE)
					)
				{
					tempReceiptList.push_back(curAvatar->socketIndex);
				}
			}
		}
		curMob = (BBOSMob *) avatars->Next();
	}

  	lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);

}

//*******************************************************************************
void SharedSpace::SendToEveryoneNearBut(int handleToExclude, float x, float y, 
													 int size, const void *dataPtr, int radius, 
													 unsigned long flags)
{
	std::vector<TagID> tempReceiptList;

	tempReceiptList.clear();

	BBOSMob *curMob = (BBOSMob *) avatars->First();
	while (curMob)
	{
		if (SMOB_AVATAR == curMob->WhatAmI())
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
			if (curAvatar->socketIndex != handleToExclude && 
				 (0 == flags || flags & curAvatar->infoFlags)
				)
			{
				if (curAvatar->controlledMonster)
				{
					if (mobList->IsInList(curAvatar->controlledMonster))
					{
						if (abs(curAvatar->controlledMonster->cellX - x) < radius && 
							 abs(curAvatar->controlledMonster->cellY - y) < radius)
							tempReceiptList.push_back(curAvatar->socketIndex);
					}
				}
				else if (abs(curAvatar->cellX - x) < radius && abs(curAvatar->cellY - y) < radius)
					tempReceiptList.push_back(curAvatar->socketIndex);
			}
		}
		curMob = (BBOSMob *) avatars->Next();
	}

  	lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);
}

//*******************************************************************************
void SharedSpace::IgnorableSendToEveryoneNear(BBOSAvatar *srcAvatar, float x, float y, int size, const void *dataPtr, int radius)
{
	std::vector<TagID> tempReceiptList;

	tempReceiptList.clear();

	BBOSMob *curMob = (BBOSMob *) avatars->First();
	while (curMob)
	{
		if (SMOB_AVATAR == curMob->WhatAmI())
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
			if (!curAvatar->IsContact(
				   srcAvatar->charInfoArray[srcAvatar->curCharacterIndex].name,
					CONTACT_IGNORE)
			   )
			{
				if (curAvatar->controlledMonster)
				{
					if (mobList->IsInList(curAvatar->controlledMonster))
					{
						if (abs(curAvatar->controlledMonster->cellX - x) < radius && 
							 abs(curAvatar->controlledMonster->cellY - y) < radius)
							tempReceiptList.push_back(curAvatar->socketIndex);
					}
				}
				else if (abs(curAvatar->cellX - x) < radius && abs(curAvatar->cellY - y) < radius)
					tempReceiptList.push_back(curAvatar->socketIndex);
			}
		}
		curMob = (BBOSMob *) avatars->Next();
	}

  	lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);
}

//*******************************************************************************
void SharedSpace::DoMonsterDrop(Inventory *inv, BBOSMonster *monster)
{
	float val; // = monster->subType * 100 + monster->maxHealth + monster->defense * 50;
	val = monster->dropAmount - 1;

	int wordOffset = val;
	if (wordOffset < 0)
		wordOffset = 0;
	if (wordOffset > 7)
		wordOffset = 7;

	float valDiv = monster->dropAmount / 2;

	int wordOffset2 = valDiv;
	if (wordOffset2 < 0)
		wordOffset2 = 0;
	if (wordOffset2 > 7)
		wordOffset2 = 7;

	if (valDiv < 3)
		valDiv = (rand() % 3) + 2;

//	iter = 

	// first, if this monster is carrying anything, drop it.
	InventoryObject *iObject = (InventoryObject *) monster->inventory->objects.First();
	while (iObject)
	{
		monster->inventory->objects.Remove(iObject);
		DropItemToProperPlace(inv, monster, iObject, 0.4f);
		iObject = (InventoryObject *) monster->inventory->objects.First();
	}

	// drop egg
	if (7 == monster->type || 16 == monster->type)  // if dragons
	{
		int r = rand() % 81;
		if (r > 80-21)
		{
			int type = 0;
			if (r > 67)
				type = 1;
			if (r > 71)
				type = 2;
			if (r > 75)
				type = 3;
			if (r > 77)
				type = 4;
			if (r > 79)
				type = 5;

			InventoryObject *iObject = new InventoryObject(
						INVOBJ_EGG,0,dragonInfo[0][type].eggName);
			iObject->mass = 1.0f;
			iObject->value = 1000;

			InvEgg *im = (InvEgg *) iObject->extra;
			im->type   = type;
			im->quality = 0;

			DropItemToProperPlace(inv, monster, iObject, 5.0f);

			char tempText[1024];
			FILE *source = fopen("petInfo.txt","a");
			/* Display operating system-style date and time. */
			_strdate( tempText );
			fprintf(source, "%s, ", tempText );
			_strtime( tempText );
			fprintf(source, "%s, ", tempText );

			fprintf(source,"EGG DROP, %s, %s, %d, %d\n",
				dragonInfo[0][type].eggName,
				monster->Name(),
				monster->cellX, monster->cellY);

			fclose(source);


		}
	}

	// drop money-items
	for (int i = 0; i < valDiv; ++i)
	{
		int adj = wordOffset + (rand() % 3);
		if (adj < 0)
			adj = 0;
		if (adj > 9)
			adj = 9;

		int noun = wordOffset2 + (rand() % 3);
		if (noun < 0)
			noun = 0;
		if (noun > 9)
			noun = 9;

		char tempText[1024];
		sprintf(tempText,"%s %s", dropAdj[adj], dropName[noun]);
		++adj;
		++noun;
		InventoryObject *iObject = new InventoryObject(INVOBJ_SIMPLE,0,tempText);
		iObject->mass = 1.0f;
//		iObject->value = adj * adj * noun * noun;
		iObject->value = adj * adj * noun / 2;
		if (iObject->value < 2)
			iObject->value = 2;
		iObject->amount = 1;
		DropItemToProperPlace(inv, monster, iObject, 0.5f);
	}

	// drop meat
	if (2 == rand()%5)
	{
		char tempText[1024];
		sprintf(tempText,"%s Meat", monster->Name());

		InventoryObject *iObject = new InventoryObject(INVOBJ_MEAT,0,tempText);
		iObject->mass = 1.0f;
		iObject->value = 10;

		InvMeat *im = (InvMeat *) iObject->extra;
		im->type   = monster->type;
		im->quality = monster->subType;

		DropItemToProperPlace(inv, monster, iObject, 0.5f);
	}

}

//*******************************************************************************
void SharedSpace::DoMonsterDropSpecial(Inventory *inv, BBOSMonster *monster, int type)
{
	InventoryObject *iObject;
	InvIngredient   *exIn;
	InvIngot        *exIngot;
	InvFuse         *exFuse;
	InvStaff        *exStaff;
	InvTotem        *extra;
	InvExplosive    *exPlosive;
	InvEgg          *im;
	InvPotion       *ip;

	char tempText[1024];


	FILE *source = fopen("specialDrop.txt","a");
	
	/* Display operating system-style date and time. */
	_strdate( tempText );
	fprintf(source, "%s, ", tempText );
	_strtime( tempText );
	fprintf(source, "%s, ", tempText );

	iObject = NULL;

	switch(type)
	{
	case 0:
		{
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_BLUE_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;

		fprintf(source,"BLUE DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		}
		break;

	case 1:
		{
		iObject = new InventoryObject(INVOBJ_INGOT,0,"Vizorium Ingot");
		exIngot = (InvIngot *)iObject->extra;
		exIngot->damageVal = 8;
		exIngot->challenge = 8;
		exIngot->r = 128;
		exIngot->g = 0;
		exIngot->b = 0;

		iObject->mass = 1.0f;
		iObject->value = 50000;
		iObject->amount = 1;

		fprintf(source,"VIZORIUM INGOT, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		}
		break;

	case 2:
		iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 18; // undead

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14 + 1;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
		iObject->amount = 1;
		UpdateTotem(iObject);

		fprintf(source,"UNDEAD TOTEM, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 3:
	case 4:
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_RED_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;
		fprintf(source,"RED DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 5:
	case 6:
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_WHITE_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;
		fprintf(source,"WHITE DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;
/*
	case 5:
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_GREEN_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;
		fprintf(source,"GREEN DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 6:
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_BLACK_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;
		fprintf(source,"BLACK DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;
  */
	case 7:
		iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Sulphur Brick");
		exPlosive = (InvExplosive *)iObject->extra;
		exPlosive->type     = 0;
		exPlosive->quality  = 1.0f;
		exPlosive->power    = 1.5f; // explosive power of this one item
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"SULPHUR BRICK, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 8:
		iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Fulminate Brick");
		exPlosive = (InvExplosive *)iObject->extra;
		exPlosive->type     = 0;
		exPlosive->quality  = 1.0f;
		exPlosive->power    = 3; // explosive power of this one item
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"FULMINATE BRICK, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 9:
		iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Selarim Brick");
		exPlosive = (InvExplosive *)iObject->extra;
		exPlosive->type     = 0;
		exPlosive->quality  = 1.0f;
		exPlosive->power    = 6; // explosive power of this one item
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"SELARIM BRICK, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 10:
		iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
		exStaff = (InvStaff *)iObject->extra;
		exStaff->type     = 0;
		exStaff->quality  = 3;

		iObject->mass = 0.0f;
		iObject->value = 500 * (3 + 1) * (3 + 1);
		iObject->amount = 1;
		UpdateStaff(iObject, 0);

		fprintf(source,"OAK STAFF, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 11:
		iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 19; // dragon

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14 + 1;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
		iObject->amount = 1;
		UpdateTotem(iObject);

		fprintf(source,"DRAGON TOTEM, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 12:
		iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Azides Brick");
		exPlosive = (InvExplosive *)iObject->extra;
		exPlosive->type     = 0;
		exPlosive->quality  = 1.0f;
		exPlosive->power    = 8; // explosive power of this one item
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"Azides BRICK, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 13:
		iObject = new InventoryObject(INVOBJ_EXPLOSIVE,0,"Nitrate Brick");
		exPlosive = (InvExplosive *)iObject->extra;
		exPlosive->type     = 0;
		exPlosive->quality  = 1.0f;
		exPlosive->power    = 10; // explosive power of this one item
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"Nitrate BRICK, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 14:
		iObject = new InventoryObject(INVOBJ_FUSE,0,"Cotton Rope");
		exFuse = (InvFuse *)iObject->extra;
		exFuse->type     = 0;
		exFuse->quality  = 0.4f;
		iObject->value  = 20;
		iObject->amount = 1;
		fprintf(source,"COTTON ROPE, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 15:
		iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 20; // chitin

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14 + 1;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
		iObject->amount = 1;
		UpdateTotem(iObject);

		fprintf(source,"CHITIN TOTEM, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 16:
		iObject = new InventoryObject(INVOBJ_EGG,0,dragonInfo[0][6].eggName);
		iObject->mass = 1.0f;
		iObject->value = 1000;

		im = (InvEgg *) iObject->extra;
		im->type   = 6;
		im->quality = 0;

		fprintf(source,"RAINBOW EGG, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 17:
		iObject = new InventoryObject(INVOBJ_INGOT,0,"Chitin Ingot");
		exIngot = (InvIngot *)iObject->extra;
		exIngot->damageVal = 10;
		exIngot->challenge = 10;
		exIngot->r = 0;
		exIngot->g = 128;
		exIngot->b = 128;

		iObject->mass = 1.0f;
		iObject->value = 80000;
		iObject->amount = 1;

		fprintf(source,"CHITIN INGOT, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);

		break;

	case 18:
		iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
		extra = (InvTotem *)iObject->extra;
		extra->type     = 0;
		extra->quality  = 21; // lava

		iObject->mass = 0.0f;
		iObject->value = extra->quality * extra->quality * 14 + 1;
		if (extra->quality > 12)
			iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
		iObject->amount = 1;
		UpdateTotem(iObject);

		fprintf(source,"LAVA TOTEM, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 19:
		iObject = new InventoryObject(INVOBJ_POTION,0, "Waste Recall");
		iObject->mass = 1.0f;
		iObject->value = 1000;

		ip = (InvPotion *) iObject->extra;
		ip->type   = POTION_TYPE_DARK_RECALL;
		ip->subType = 0; // waste

		fprintf(source,"WASTE RECALL, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 20:
		iObject = new InventoryObject(INVOBJ_POTION,0, "Graveyard Recall");
		iObject->mass = 1.0f;
		iObject->value = 1000;

		ip = (InvPotion *) iObject->extra;
		ip->type   = POTION_TYPE_DARK_RECALL;
		ip->subType = 1; // graveyard

		fprintf(source,"GRAVEYARD RECALL, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 21:
		iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
		exStaff = (InvStaff *)iObject->extra;
		exStaff->type     = 0;
		exStaff->quality  = 4;

		iObject->mass = 0.0f;
		iObject->value = 500 * (3 + 1) * (3 + 1);
		iObject->amount = 1;
		UpdateStaff(iObject, 0);

		fprintf(source,"ONYX STAFF, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;

	case 22:
		iObject = new InventoryObject(INVOBJ_INGOT,0,"Malignant Ingot");
		exIngot = (InvIngot *)iObject->extra;
		exIngot->damageVal = 11;
		exIngot->challenge = 11;
		exIngot->r = 0;
		exIngot->g = 128;
		exIngot->b = 48;

		iObject->mass = 1.0f;
		iObject->value = 90000;
		iObject->amount = 1;

		fprintf(source,"MALIGNANT INGOT, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);

		break;

	case 23:	// Drop Silver
		iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Silver Dust");
		exIn = (InvIngredient *)iObject->extra;
		exIn->type     = INGR_SILVER_DUST;
		exIn->quality  = 1;

		iObject->mass = 0.0f;
		iObject->value = 1000;
		iObject->amount = 1;
		fprintf(source,"SILVER DUST, %s, %d, %d\n",
			monster->Name(),
			monster->cellX, monster->cellY);
		break;
	}

	DropItemToProperPlace(inv, monster, iObject, 0.4f);

	fclose(source);

}

//*******************************************************************************
void SharedSpace::DropItemToProperPlace(Inventory *inv, 
													 BBOSMonster *monster, 
													 InventoryObject *iObject,
													 float coeff)
{
	std::vector<TagID> tempReceiptList;
	tempReceiptList.clear();

	MessInfoText infoText;
	char tempText[1024];

	if (iObject)
	{
		BBOSAvatar *av = monster->MostDamagingPlayer(coeff);
		if (av)
		{
			sprintf(tempText,"%s gets %s from the %s corpse.",
						av->charInfoArray[av->curCharacterIndex].name, 
						iObject->WhoAmI(),
						monster->Name());
			CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
			SendToEveryoneNearBut(av->socketIndex, monster->cellX, monster->cellY,
							sizeof(infoText),(void *)&infoText,2, INFO_FLAGS_LOOT_TAKEN);

			if (av->infoFlags & INFO_FLAGS_LOOT_TAKEN)
			{
				tempReceiptList.push_back(av->socketIndex);
			  	lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
			}

			av->charInfoArray[av->curCharacterIndex].inventory->AddItemSorted(iObject);
		}
		else
		{
			inv->AddItemSorted(iObject);

			// tell client about an item sack
			MessMobAppear messMA;
			messMA.mobID = (unsigned long) inv;
			messMA.type = SMOB_ITEM_SACK;
			messMA.x = monster->cellX;
			messMA.y = monster->cellY;
			SendToEveryoneNearBut(0, monster->cellX, monster->cellY,
							sizeof(messMA),(void *)&messMA);
		}
	}

}


//*******************************************************************************
int SharedSpace::SX(int realX)
{
	assert(sizeX);
	return sizeX - realX;
}

//*******************************************************************************
int SharedSpace::SY(int realY)
{
	assert(sizeY);
	return sizeY - realY;
}


/* end of file */



