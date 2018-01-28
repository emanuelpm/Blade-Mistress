#ifndef INVENTORY_H
#define INVENTORY_H

#include "./helper/linklist.h"
#include "longtime.h"
#include "flockPoints.h"
#include "BBO.h"
#include "./network/server.h"
#include "./network/NetWorldMessages.h"
#include "memory_helper.h"

const int INVOBJECT_NAME_LEN = 32;

// do NOT get these out of order; the values are saved in user files!!!
enum
{
	INVOBJ_SIMPLE,
	INVOBJ_BLADE,
	INVOBJ_TOTEM,
	INVOBJ_POTION,
	INVOBJ_INGOT,
	INVOBJ_BOTTLE,

	INVOBJ_SKILL,
	INVOBJ_EFFECT,
	INVOBJ_INGREDIENT,
	INVOBJ_EGG,
	INVOBJ_MEAT,
	INVOBJ_EXPLOSIVE,
	INVOBJ_FUSE,
	INVOBJ_BOMB,

	INVOBJ_STAFF,
	INVOBJ_FAVOR,

	INVOBJ_GEOPART,
	INVOBJ_EARTHKEY,

	INVOBJ_MAX
};

enum
{
	INVSTATUS_NORMAL,
	INVSTATUS_QUEST_ITEM,
	INVSTATUS_MAX
};

enum
{
	INVTRANSFER_BUY,
	INVTRANSFER_SELL,
	INVTRANSFER_DROP,
	INVTRANSFER_USE,
	INVTRANSFER_EQUIP,
	INVTRANSFER_UNEQUIP,
	INVTRANSFER_ACTIVATE,
	INVTRANSFER_DEACTIVATE,
	INVTRANSFER_GIVE,
	INVTRANSFER_MAX
};

enum
{
	// These HAVE to remain in order
	INGR_GREEN_DUST,
	INGR_BLUE_DUST,
	INGR_BLACK_DUST,
	INGR_WHITE_DUST,
	INGR_RED_DUST,

	INGR_WHITE_SHARD,
	INGR_RED_SHARD,
	INGR_BLUE_SHARD,
	INGR_GREEN_SHARD,
	INGR_AQUA_SHARD,
	INGR_PURPLE_SHARD,
	INGR_YELLOW_SHARD,
	INGR_ORANGE_SHARD,
	INGR_PINK_SHARD,

	// New DUST types
	INGR_GOLD_DUST,
	INGR_SILVER_DUST,
	
	INGR_MAX
};

enum
{
	POTION_TYPE_RECALL,
	POTION_TYPE_DARK_RECALL,
	POTION_TYPE_TOWER_RECALL,
	POTION_TYPE_MAX
};

//******************************************************************
class InventoryRef
{
public:

	InventoryRef() {name[0] = 0; };
	virtual ~InventoryRef() {;};

	char name[INVOBJECT_NAME_LEN * 2];
	unsigned int type;
	int status;
	long ptr;
	int position;
	long value, amount;

};

//******************************************************************
struct InvBlade
{
	long damageDone;
	long toHit;
	float size;
	char isWielded;
	int r,g,b;
	int bladeGlamourType;
	int poison;
	int blind;
	int slow;
	int heal;
	int lightning;
	int numOfHits;
	int type;

	// Number of ingots used to make the blade
	int tinIngots;
	int aluminumIngots;
	int steelIngots;
	int carbonIngots;
	int zincIngots;
	int adamIngots;
	int mithIngots;
	int vizIngots;
	int elatIngots;
	int chitinIngots;
	int maligIngots;

	int GetIngotCount() {
		return tinIngots + aluminumIngots +
			steelIngots + carbonIngots +
			zincIngots + adamIngots +
			mithIngots + vizIngots +
			elatIngots + chitinIngots +
			maligIngots;
	}
};

//******************************************************************
struct InvSkill
{
	unsigned long skillPoints;
	unsigned long skillLevel;
};

//******************************************************************
struct InvIngot
{
	int r,g,b;
	float damageVal, challenge;
};

//******************************************************************
struct InvTotem
{
	int type, quality;
	char isActivated;
	LongTime timeToDie;
	float imbue[MAGIC_MAX];
	float imbueDeviation;

	int TotalImbues() {
		int total = 0;
		for( int i = 0; i < MAGIC_MAX; i++ )
			total += (int)imbue[ i ];
		return total;
	}
};

//******************************************************************
struct InvStaff
{
	int type, quality, charges;
	char isActivated;
	float imbue[MAGIC_MAX];
	float imbueDeviation;
	
	// These variables are not used, but it keeps
	// us from getting heap deletion problems... I dunno
	int randomCrap, randomCrap2;
};

//******************************************************************
struct InvIngredient
{
	int type, quality;
};

//******************************************************************
struct InvEgg
{
	int type, quality;
};

//******************************************************************
struct InvMeat
{
	int type, quality, age; // if age == -1, it's salted.  if -2, rotted salted
};

//******************************************************************
struct InvExplosive
{
	int type;
	float quality;
	float power;
};

//******************************************************************
struct InvFuse
{
	int type;
	float quality;
};

//******************************************************************
struct InvFavor
{
	char spirit;
};

//******************************************************************
struct InvBomb
{
	float power;
	float stability;
	int type;
	int fuseDelay;
	unsigned long flags;
	int r,g,b;
};

//******************************************************************
struct InvPotion
{
	int type;
	DWORD subType;
};

//******************************************************************
struct InvGeoPart
{
	int type;  // 0 == bead, 1 == Heart Gem
	float power;
};

//******************************************************************
struct InvEarthKey
{
	int width, height;
	float power;
	int monsterType[2];
};

//******************************************************************
class InventoryObject : public DataObject
{
public:

	InventoryObject(unsigned int type, int doid, char *doname);
	virtual ~InventoryObject();
	virtual void Save(FILE *fp);
	virtual void CopyTo(InventoryObject *io);

	float mass, value;
	unsigned int type; //  INVOBJ_
	int  status;
	long amount;

	void *extra; // based on type

};

extern InventoryObject *LoadInventoryObject(FILE *fp);

//******************************************************************
class Inventory
{
public:

	Inventory(int type = MESS_INVENTORY_GROUND, void *p = NULL);
	virtual ~Inventory();
	void InventoryLoad(FILE *fp, float version);
	void InventorySave(FILE *fp);

	void AddItemSorted(InventoryObject *item);

	DoublyLinkedList objects;
	long money;
	Inventory *partner; // used by the server to "lock" inventories so they can't be used
						// by more than one player at a time.
	int giving;    // FALSE if partnered with a trader
	int subType;   // MESS_INVENTORY_* from netWorldMessages.h
	void *parent;

};

extern char dustNames[INGR_MAX][64];
#endif
