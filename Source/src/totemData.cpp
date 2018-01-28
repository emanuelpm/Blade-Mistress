
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "inventory.h"
#include "TotemData.h"

char totemTypeName[TOTEM_MAX][12] =
{
	{"Strength"},
	{"Quickness"},
	{"Toughness"},
	{"Accuracy"},
	{"Healing"},
	{"Spirit Prot"},
	{"Web Prot"},
	{"Phys Stat"},
	{"Mage Stat"},
	{"Crea Stat"},
	{"Lifesteal"}
};

// BEAR, WOLF, EAGLE, SNAKE, FROG, SUN, MOON, TURTLE
int totemImbueValue[TOTEM_MAX][MAGIC_MAX] =
{
	{2, 0, 0, 0,     0, 0, 0, 1, 0},
	{0, 0, 1, 1,     0, 0, 1, 0, 0},
	{1, 0, 0, 0,     0, 2, 0, 0, 0},
	{0, 0, 2, 0,     0, 1, 0, 0, 0},
	{0, 1, 0, 0,     2, 0, 0, 1, 0},
	{0, 1, 0, 1,     1, 0, 2, 0, 0},
	{1, 2, 1, 0,     1, 0, 0, 2, 0},
	{1000, 1000, 1000, 1000,     1000, 1000, 1000, 1000, 1000},
	{1000, 1000, 1000, 1000,     1000, 1000, 1000, 1000, 1000},
	{1000, 1000, 1000, 1000,     1000, 1000, 1000, 1000, 1000},
	{0, 1, 0, 2,     1, 0, 1, 0, 2}
};

char totemQualityName[TOTEM_QUALITY_MAX][12] =
{
	{"Wood"},
	{"Clay"},
	{"Leather"},
	{"Stone"},
	{"Iron"},
	{"Shale"},
	{"Bone"},
	{"Quartz"},
	{"Silver"},
	{"Amber"},
	{"Golden"},
	{"Jade"},
	{"Platinum"},
	{"Crystal"},
	{"Ruby"},
	{"Emerald"},
	{"Diamond"},
	{"Pumpkin"},
	{"Undead"},
	{"Dragon"},
	{"Chitin"},
	{"Lava"}
};

//******************************************************************************
void UpdateTotem(InventoryObject *totemObject)
{
	InvTotem *extra = (InvTotem *)totemObject->extra;
			 
	// find closest magic to current imbue set
	int candidate = 0, canDev = 100, active = FALSE;

	// clamp bad values
	for (int j = 0; j < MAGIC_MAX; ++j)
	{
		if (extra->imbue[j] < 0 ||
			 extra->imbue[j] > 10000)
			extra->imbue[j] = 0;
	}

	for (int i = 0; i < TOTEM_MAX; ++i)
	{
		int dev = 0;

		for (int j = 0; j < MAGIC_MAX; ++j)
		{
			dev += abs(totemImbueValue[i][j] - extra->imbue[j]);
			if (extra->imbue[j] > 0)
				active = TRUE;
		}

		if (dev < canDev)
		{
			candidate = i;
			canDev = dev;
		}
	}

	// set imbue deviation
	extra->imbueDeviation = canDev;
	extra->type = candidate;

	// change name
	char tempText[1024];
	if (active)
		sprintf(tempText, "%s %s Totem", totemQualityName[extra->quality],
			                                          totemTypeName[extra->type]);
	else
		sprintf(tempText, "%s Totem", totemQualityName[extra->quality]);

	sprintf(totemObject->do_name, tempText);

	// if not activated, and has been imbued, activate!
	if (!extra->isActivated && active)
	{
		extra->isActivated = TRUE;
		extra->timeToDie.SetToNow();
		if (17 == extra->quality)  // pumpkin totems have half the lifespan!
			extra->timeToDie.AddMinutes(
			      (extra->quality + 1) * (extra->quality + 1) * 18);
		else
			extra->timeToDie.AddMinutes(
			      (extra->quality + 1) * (extra->quality + 1) * 36);
	}
	
}

//******************************************************************************
int ImbueTotem(InventoryObject *totemObject, int magicType, float workLevel)
{

	InvTotem *extra = (InvTotem *)totemObject->extra;

	// clamp bad values
	if (extra->imbue[magicType] < 0 ||
		 extra->imbue[magicType] > 10000)
		extra->imbue[magicType] = 0;

	// how hard is it to imbue?

	// try to imbue
	if (extra->quality <= workLevel)
	{
		// success

		// add imbue
		extra->imbue[magicType] += 1;

		return IMBUE_RES_SUCCESS;
	}
	else
	{
		// failure

		// did it break?
		if (!(rand() % 7))
			return IMBUE_RES_DISINTEGRATE;
		else
			return IMBUE_RES_FAIL;
	}

	return IMBUE_RES_FAIL;

}

//******************************************************************************
int TotemImbueExperience(InventoryObject *totemObject, int skillLevel)
{
	InvTotem *extra = (InvTotem *)totemObject->extra;

	if (abs(extra->quality - skillLevel) < 4)
	{
		int retVal = (extra->quality - skillLevel + 1);
		if (retVal < 1)
			retVal = 1;
		return retVal;
	}
	else
		return 0;
};


/* end of file */



