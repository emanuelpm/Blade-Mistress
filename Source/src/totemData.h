#ifndef TOTEMDATA_H
#define TOTEMDATA_H

#include "BBO.h"

class InventoryObject;

enum
{
	TOTEM_STRENGTH,
	TOTEM_QUICKNESS,
	TOTEM_TOUGHNESS,
	TOTEM_ACCURACY,
	TOTEM_HEALING,
	TOTEM_PROT_SPIRITS,
	TOTEM_PROT_WEB,
	TOTEM_PHYSICAL,
	TOTEM_MAGICAL,
	TOTEM_CREATIVE,
	TOTEM_LIFESTEAL,
	TOTEM_MAX
};

struct TotemEffects
{
	char effect[TOTEM_MAX];
};

enum
{
	IMBUE_RES_SUCCESS,
	IMBUE_RES_FAIL,
	IMBUE_RES_DISINTEGRATE,
	IMBUE_RES_MAX
};

const int TOTEM_QUALITY_MAX = 22;
const int TOTEM_SELLABLE_QUALITY_MAX = 17;

extern void UpdateTotem(InventoryObject *totemObject);
extern int  ImbueTotem(InventoryObject *totemObject, int magicType, float workLevel);
extern int  TotemImbueExperience(InventoryObject *totemObject, int skillLevel);

extern char totemTypeName[TOTEM_MAX][12];
extern char totemQualityName[TOTEM_QUALITY_MAX][12];

#endif
