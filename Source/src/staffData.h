#ifndef STAFFDATA_H
#define STAFFDATA_H

#include "BBO.h"
#include "totemData.h"

class InventoryObject;
struct InvStaff;

enum
{
	STAFF_DAMAGE,
	STAFF_BLIND,
	STAFF_SLOW,
	STAFF_POISON,
	STAFF_STUN,
	STAFF_BIND,
	STAFF_AREA_DAMAGE,
	STAFF_AREA_BLIND,
	STAFF_AREA_SLOW,
	STAFF_AREA_POISON,
	STAFF_AREA_STUN,
	STAFF_AREA_BIND,
	STAFF_MAX
};

struct StaffEffects
{
	char effect[STAFF_MAX];
};

const int STAFF_QUALITY_MAX = 5;
const int STAFF_SELLABLE_QUALITY_MAX = 1;

extern void UpdateStaff(InventoryObject *staffObject, float workLevel);
extern int  ImbueStaff(InventoryObject *staffObject, int magicType, float workLevel);
extern int  StaffImbueExperience(InventoryObject *staffObject, int skillLevel);
extern float ImbueStaffChallenge(InvStaff *extra);
extern int StaffAffectsArea(InvStaff *extra);

extern unsigned char staffColor[STAFF_MAX][3];

extern char staffTypeName[STAFF_MAX][17];

#endif
