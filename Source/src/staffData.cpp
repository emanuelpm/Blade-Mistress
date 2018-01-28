
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "inventory.h"
#include "StaffData.h"

char staffTypeName[STAFF_MAX][17] =
{
	{"Fire"},
	{"Blinding"},
	{"Slowing"},
	{"Poisonous"},
	{"Stunning"},
	{"Tangling"},
	{"Din Fire"},
	{"Din Blind"},
	{"Din Slow"},
	{"Din Poison"},
	{"Din Stun"},
	{"Din Tangle"}
};

// BEAR, WOLF, EAGLE, SNAKE, FROG, SUN, MOON, TURTLE, evil
int staffImbueValue[STAFF_MAX][MAGIC_MAX] =
{
	{2, 4, 3, 1,     0, 4, 2, 2, 0},
	{0, 3, 3, 1,     3, 4, 1, 0, 0},
	{1, 2, 4, 3,     2, 2, 2, 0, 0},
	{3, 0, 2, 0,     4, 1, 4, 0, 0},
	{4, 1, 1, 2,     0, 3, 0, 2, 0},
	{3, 1, 1, 3,     2, 1, 0, 4, 0},
	{2, 4, 0, 1,     2, 0, 1, 3, 0},
	{1, 2, 0, 4,     0, 3, 2, 3, 0},
	{0, 0, 2, 2,     4, 3, 1, 1, 0},
	{1, 0, 3, 2,     0, 4, 2, 3, 0},
	{0, 4, 3, 0,     2, 2, 1, 1, 0},
	{4, 1, 2, 1,     0, 3, 1, 1, 0}
};

unsigned char staffColor[STAFF_MAX][3] =
{
	{255,0,0},
	{0,255,0},
	{0,0,255},
	{255,255,0},
	{0,255,255},
	{255,0,255},

	{255,128,0},
	{0,255,128},
	{128,0,255},
	{255,255,128},
	{128,255,255},
	{255,128,255}

};


char staffQualityName[STAFF_QUALITY_MAX][12] =
{
	{"Pine"},
	{"Birch"},
	{"Spruce"},
	{"Oak"},
	{"Onyx"}
};

//******************************************************************************
void UpdateStaff(InventoryObject *staffObject, float workLevel)
{ 
	InvStaff *extra = (InvStaff *)staffObject->extra;

	// clamp bad values
	for (int j = 0; j < MAGIC_MAX; ++j)
	{
		if (extra->imbue[j] < 0 ||
			 extra->imbue[j] > 10000)
			extra->imbue[j] = 0;
	}

	// find closest magic to current imbue set
	int candidate = 0, canDev = 100, active = FALSE;

	for (int i = 0; i < STAFF_MAX; ++i)
	{
		int dev = 0;

		for (int j = 0; j < MAGIC_MAX; ++j)
		{
			dev += abs(staffImbueValue[i][j] - extra->imbue[j]);
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
		sprintf(tempText, "%s %s Staff", staffQualityName[extra->quality],
			                                          staffTypeName[extra->type]);
	else
		sprintf(tempText, "%s Staff", staffQualityName[extra->quality]);

	sprintf(staffObject->do_name, tempText);

	// if not activated, and has been imbued, activate!
	if (!extra->isActivated && active)
	{
		extra->isActivated = TRUE;
		extra->charges = workLevel * 4 / 10;
	}
	
}

//******************************************************************************
int ImbueStaff(InventoryObject *staffObject, int magicType, float workLevel)
{

	InvStaff *extra = (InvStaff *)staffObject->extra;

	// clamp bad values
	if (extra->imbue[magicType] < 0 ||
		 extra->imbue[magicType] > 10000)
		extra->imbue[magicType] = 0;

	// how hard is it to imbue?

	// try to imbue
	if (ImbueStaffChallenge(extra) <= workLevel)
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
int StaffImbueExperience(InventoryObject *staffObject, int skillLevel)
{
	InvStaff *extra = (InvStaff *)staffObject->extra;

	if (abs(ImbueStaffChallenge(extra) - skillLevel) < 9)
	{
		int retVal = (ImbueStaffChallenge(extra) - skillLevel + 1);
		if (retVal < 1)
			retVal = 1;
		return retVal;
	}
	else
		return 0;
};

//******************************************************************************
float ImbueStaffChallenge(InvStaff *extra)
{
	return 10 + extra->quality * 5;
}

//******************************************************************************
int StaffAffectsArea(InvStaff *extra)
{
	if (extra->type >= STAFF_AREA_DAMAGE)
		return 1; // up to 1 square away
	return 0;
}


/* end of file */



