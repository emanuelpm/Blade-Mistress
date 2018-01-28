
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "inventory.h"
#include ".\helper\GeneralUtils.h"


char dustNames[INGR_MAX][64] =
{
	{"Glowing Green Dust"},
	{"Glowing Blue Dust"},
	{"Glowing Black Dust"},
	{"Glowing White Dust"},
	{"Glowing Red Dust"},
	{"White Metal Shard"},
	{"Red Metal Shard"},
	{"Blue Metal Shard"},
	{"Green Metal Shard"},
	{"Aqua Metal Shard"},
	{"Purple Metal Shard"},
	{"Yellow Metal Shard"},
	{"Orange Metal Shard"},
	{"Pink Metal Shard"},
	{"Glowing Gold Dust"},
	{"Glowing Silver Dust"}
};


//******************************************************************
//******************************************************************
InventoryObject::InventoryObject(unsigned int t, int doid, char *doname)	 : DataObject(doid,doname)
{
	mass = value = 0.0f;
	type = t;
	status = INVSTATUS_NORMAL;
	amount = 1;
	extra = NULL;


	switch(type)
	{
	case INVOBJ_BLADE:
		extra = new InvBlade();
		((InvBlade *)extra)->damageDone = 1;
		((InvBlade *)extra)->toHit = 1;
		((InvBlade *)extra)->size = 1.0f;
		((InvBlade *)extra)->isWielded = FALSE;
		((InvBlade *)extra)->r = 128;
		((InvBlade *)extra)->g = 128;
		((InvBlade *)extra)->b = 128;
		((InvBlade *)extra)->bladeGlamourType = 0;  // nothing special
		((InvBlade *)extra)->poison = 0;
		((InvBlade *)extra)->blind  = 0;
		((InvBlade *)extra)->slow   = 0;
		((InvBlade *)extra)->heal   = 0;
		((InvBlade *)extra)->lightning = 0;
		((InvBlade *)extra)->numOfHits = 0;
		((InvBlade *)extra)->type = BLADE_TYPE_NORMAL;

		// Number of ingots in the blade
		((InvBlade *)extra)->tinIngots = 0;
		((InvBlade *)extra)->aluminumIngots = 0;
		((InvBlade *)extra)->steelIngots = 0;
		((InvBlade *)extra)->carbonIngots = 0;
		((InvBlade *)extra)->zincIngots = 0;
		((InvBlade *)extra)->adamIngots = 0;
		((InvBlade *)extra)->mithIngots = 0;
		((InvBlade *)extra)->vizIngots = 0;
		((InvBlade *)extra)->elatIngots = 0;
		((InvBlade *)extra)->chitinIngots = 0;
		((InvBlade *)extra)->maligIngots = 0;
		break;

	case INVOBJ_SKILL:
		extra = new InvSkill();
		((InvSkill *)extra)->skillLevel  = 1; // skills start at 1, not 0
		((InvSkill *)extra)->skillPoints = 0;
		break;

	case INVOBJ_INGREDIENT:
		extra = new InvIngredient();
		((InvIngredient *)extra)->type    = 0; 
		((InvIngredient *)extra)->quality = 0;
		break;

	case INVOBJ_MEAT:
		extra = new InvMeat();
		((InvMeat *)extra)->type    = 0; 
		((InvMeat *)extra)->quality = 0;
		((InvMeat *)extra)->age     = 0;
		break;

	case INVOBJ_EGG:
		extra = new InvEgg();
		((InvEgg *)extra)->type    = 0; 
		((InvEgg *)extra)->quality = 0;
		break;

	case INVOBJ_INGOT:
		extra = new InvIngot();
		((InvIngot *)extra)->damageVal  = 1;
		((InvIngot *)extra)->challenge  = 1;
		((InvIngot *)extra)->r = 128;
		((InvIngot *)extra)->g = 128;
		((InvIngot *)extra)->b = 128;
		break;

	case INVOBJ_TOTEM:
		extra = new InvTotem();
		((InvTotem *)extra)->isActivated = 0;
		((InvTotem *)extra)->type = 0;
		((InvTotem *)extra)->quality = 0;
		((InvTotem *)extra)->imbue[0] = ((InvTotem *)extra)->imbue[1] =
		((InvTotem *)extra)->imbue[2] = ((InvTotem *)extra)->imbue[3] =
		((InvTotem *)extra)->imbue[4] = ((InvTotem *)extra)->imbue[5] =
		((InvTotem *)extra)->imbue[6] = ((InvTotem *)extra)->imbue[7] = 
		((InvTotem *)extra)->imbue[8] = 0;
		break;

	case INVOBJ_STAFF:
		extra = new InvStaff();
		((InvStaff *)extra)->isActivated = 0;
		((InvStaff *)extra)->type = 0;
		((InvStaff *)extra)->quality = 0;
		((InvStaff *)extra)->charges = 0;
		((InvStaff *)extra)->imbue[0] = ((InvStaff *)extra)->imbue[1] =
		((InvStaff *)extra)->imbue[2] = ((InvStaff *)extra)->imbue[3] =
		((InvStaff *)extra)->imbue[4] = ((InvStaff *)extra)->imbue[5] =
		((InvStaff *)extra)->imbue[6] = ((InvStaff *)extra)->imbue[7] = 
		((InvTotem *)extra)->imbue[8] = 0;
		break;

	case INVOBJ_EXPLOSIVE:
		extra = new InvExplosive();
		((InvExplosive *)extra)->type    = 0;
		((InvExplosive *)extra)->quality = 1;
		((InvExplosive *)extra)->power   = 1;
		break;

	case INVOBJ_POTION:
		extra = new InvPotion();
		((InvPotion *)extra)->type    = 0;
		((InvPotion *)extra)->subType = 0;
		break;

	case INVOBJ_FUSE:
		extra = new InvFuse();
		((InvFuse *)extra)->type    = 0;
		((InvFuse *)extra)->quality = 1.0f;
		break;

	case INVOBJ_BOMB:
		extra = new InvBomb();
		((InvBomb *)extra)->power     = 1.0f;
		((InvBomb *)extra)->stability = 1.0f;
		((InvBomb *)extra)->fuseDelay = 0;
		((InvBomb *)extra)->type      = 1;
		((InvBomb *)extra)->flags     = 0;
		((InvBomb *)extra)->r         = 255;
		((InvBomb *)extra)->g         = 255;
		((InvBomb *)extra)->b         = 0;
		break;

	case INVOBJ_FAVOR:
		extra = new InvFavor();
		((InvFavor *)extra)->spirit = 0;
		break;

	case INVOBJ_GEOPART:
		extra = new InvGeoPart();
		((InvGeoPart *)extra)->type = 0;
		((InvGeoPart *)extra)->power = 0;
		break;

	case INVOBJ_EARTHKEY:
		extra = new InvEarthKey();
		((InvEarthKey *)extra)->monsterType[0] = 0;
		((InvEarthKey *)extra)->monsterType[1] = 0;
		((InvEarthKey *)extra)->power       = 0;
		((InvEarthKey *)extra)->height      = 5;
		((InvEarthKey *)extra)->width       = 5;
		break;

	default:
		// no extra data required
		break;
	}

}

//******************************************************************
InventoryObject::~InventoryObject()
{
	if( extra )
		delete extra;

}

//******************************************************************
void InventoryObject::Save(FILE *fp)
{
	char *name = WhoAmI();
	if (0 == name[0])
		return;     // SHOULD be an assert...

	fprintf(fp,"%s\n",name);
	fprintf(fp,"%f %f %d %d %ld\n", mass, value, type, status, amount);

	switch(type)
	{
	case INVOBJ_BLADE:
		fprintf(fp,"%ld %ld %f %d %d %d %d %d %d %d %d %d %d %d %d\n", 
			((InvBlade *)extra)->damageDone,
			((InvBlade *)extra)->toHit     ,
			((InvBlade *)extra)->size      ,
			((InvBlade *)extra)->isWielded ,
			((InvBlade *)extra)->r ,
			((InvBlade *)extra)->g ,
			((InvBlade *)extra)->b ,
			((InvBlade *)extra)->bladeGlamourType,
			((InvBlade *)extra)->poison,
			((InvBlade *)extra)->blind,
			((InvBlade *)extra)->slow,
			((InvBlade *)extra)->heal,
			((InvBlade *)extra)->lightning,
			((InvBlade *)extra)->type,
			((InvBlade *)extra)->numOfHits
		);
		fprintf(fp, "%d %d %d %d %d %d %d %d %d %d %d\n",
			((InvBlade *)extra)->tinIngots,
			((InvBlade *)extra)->aluminumIngots,
			((InvBlade *)extra)->steelIngots,
			((InvBlade *)extra)->carbonIngots,
			((InvBlade *)extra)->zincIngots,
			((InvBlade *)extra)->adamIngots,
			((InvBlade *)extra)->mithIngots,
			((InvBlade *)extra)->vizIngots,
			((InvBlade *)extra)->elatIngots,
			((InvBlade *)extra)->chitinIngots,
			((InvBlade *)extra)->maligIngots
		);
		break;

	case INVOBJ_INGOT:
		fprintf(fp,"%f %f %d %d %d\n", 
			((InvIngot *)extra)->damageVal,
			((InvIngot *)extra)->challenge,
			((InvIngot *)extra)->r ,
			((InvIngot *)extra)->g ,
			((InvIngot *)extra)->b);
		break;

	case INVOBJ_SKILL:
		fprintf(fp,"%ld %ld\n", 
			((InvSkill *)extra)->skillLevel,
			((InvSkill *)extra)->skillPoints);
		break;

	case INVOBJ_INGREDIENT:
		fprintf(fp,"%d %d\n", 
			((InvIngredient *)extra)->quality,
			((InvIngredient *)extra)->type);

		// these numbers are suspect, for some reason, so for now they're being reset on load
//		((InvIngredient *)extra)->quality = 1;
//		((InvIngredient *)extra)->type = 1;
		break;

	case INVOBJ_MEAT:
		fprintf(fp,"%d %d %d\n", 
			((InvMeat *)extra)->quality,
			((InvMeat *)extra)->type,
			((InvMeat *)extra)->age);
		break;

	case INVOBJ_EGG:
		fprintf(fp,"%d %d\n", 
			((InvEgg *)extra)->quality,
			((InvEgg *)extra)->type);
		break;

/*	int type, quality;
	char isActivated;
	DWORD timeToDie;
	float imbue[8];
	float imbueDeviation; */

	case INVOBJ_TOTEM:
		fprintf(fp,"%d %d %d %f\n", 
			((InvTotem *)extra)->type, ((InvTotem *)extra)->quality, 
			((InvTotem *)extra)->isActivated, ((InvTotem *)extra)->imbueDeviation);
		fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
			((InvTotem *)extra)->timeToDie.value.wYear,
			((InvTotem *)extra)->timeToDie.value.wMonth,
			((InvTotem *)extra)->timeToDie.value.wDay,
			((InvTotem *)extra)->timeToDie.value.wDayOfWeek,
			((InvTotem *)extra)->timeToDie.value.wHour,
			((InvTotem *)extra)->timeToDie.value.wMinute,
			((InvTotem *)extra)->timeToDie.value.wSecond,
			((InvTotem *)extra)->timeToDie.value.wMilliseconds);
		fprintf(fp,"%f %f %f %f %f %f %f %f %f\n", 
			((InvTotem *)extra)->imbue[0], ((InvTotem *)extra)->imbue[1],
			((InvTotem *)extra)->imbue[2], ((InvTotem *)extra)->imbue[3],
			((InvTotem *)extra)->imbue[4], ((InvTotem *)extra)->imbue[5],
			((InvTotem *)extra)->imbue[6], ((InvTotem *)extra)->imbue[7], 
			((InvTotem *)extra)->imbue[8]);
		break;

	case INVOBJ_STAFF:
		fprintf(fp,"%d %d %d %d %f\n", 
			((InvStaff *)extra)->type, ((InvStaff *)extra)->quality, 
			((InvStaff *)extra)->isActivated, ((InvStaff *)extra)->charges, 
			((InvStaff *)extra)->imbueDeviation);
		fprintf(fp,"%f %f %f %f %f %f %f %f %f\n", 
			((InvStaff *)extra)->imbue[0], ((InvStaff *)extra)->imbue[1],
			((InvStaff *)extra)->imbue[2], ((InvStaff *)extra)->imbue[3],
			((InvStaff *)extra)->imbue[4], ((InvStaff *)extra)->imbue[5],
			((InvStaff *)extra)->imbue[6], ((InvStaff *)extra)->imbue[7], 
			((InvStaff *)extra)->imbue[8]);
		break;

	case INVOBJ_EXPLOSIVE:
		fprintf(fp,"%d %f %f\n", 
			((InvExplosive *)extra)->type,
			((InvExplosive *)extra)->quality,
			((InvExplosive *)extra)->power);
		break;

	case INVOBJ_POTION:
		fprintf(fp,"%d %ld\n", 
			((InvPotion *)extra)->type,
			((InvPotion *)extra)->subType);
		break;

	case INVOBJ_FUSE:
		fprintf(fp,"%d %f\n", 
			((InvFuse *)extra)->type,
			((InvFuse *)extra)->quality);
		break;

	case INVOBJ_BOMB:
		fprintf(fp,"%f %f %d %d %ld %d %d %d\n", 
			((InvBomb *)extra)->power,
			((InvBomb *)extra)->stability,
			((InvBomb *)extra)->fuseDelay,
			((InvBomb *)extra)->type,
			((InvBomb *)extra)->flags,
			((InvBomb *)extra)->r,
			((InvBomb *)extra)->g,
			((InvBomb *)extra)->b);
		break;

	case INVOBJ_FAVOR:
		fprintf(fp,"%d\n", 
			((InvFavor *)extra)->spirit);
		break;

	case INVOBJ_GEOPART:
		fprintf(fp,"%d %f\n", 
		((InvGeoPart *)extra)->type,
		((InvGeoPart *)extra)->power);
		break;

	case INVOBJ_EARTHKEY:
		fprintf(fp,"%d %d %f %d %d\n", 
		((InvEarthKey *)extra)->monsterType[0],
		((InvEarthKey *)extra)->monsterType[1],
		((InvEarthKey *)extra)->power      ,
		((InvEarthKey *)extra)->height     ,
		((InvEarthKey *)extra)->width      );
		break;

	default:
		// no extra data required
		break;
	}

}

//******************************************************************
InventoryObject *LoadInventoryObject(FILE *fp, float version)
{
	// make a simple object, change it as we load it
	InventoryObject *io = new InventoryObject(INVOBJ_SIMPLE, 0, "LOADED");

	LoadLineToString(fp, io->WhoAmI());

	if (!strnicmp("XYZENDXYZ",io->WhoAmI(),9))
	{
		delete io;
		return NULL;
	}

	if (!strnicmp("River Magic",io->WhoAmI(),11))
		sprintf(io->WhoAmI(),"Turtle Magic");


	if (version < 1.94f)
		fscanf(fp,"%f %f %d %ld", &io->mass, &io->value, &io->type, &io->amount);
	else
		fscanf(fp,"%f %f %d %d %ld", &io->mass, &io->value, &io->type, &io->status, &io->amount);


	int tempInt;
	float tempFloat;
	InvTotem *it;
	InvStaff *is;

	switch(io->type)
	{
	case INVOBJ_BLADE:
		io->extra = new InvBlade();

		((InvBlade *)io->extra)->poison = 0;
		((InvBlade *)io->extra)->blind  = 0;
		((InvBlade *)io->extra)->slow   = 0;
		((InvBlade *)io->extra)->heal   = 0;
		((InvBlade *)io->extra)->lightning = 0;
		((InvBlade *)io->extra)->numOfHits = 0;
		((InvBlade *)io->extra)->type = BLADE_TYPE_NORMAL;

		if (version < 1.31f)
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b);

			((InvBlade *)io->extra)->bladeGlamourType = 0;

		}
		else if (version < 1.47f)
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b,
				&((InvBlade *)io->extra)->bladeGlamourType);
		}
		else if (version <= 1.68f)
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d %d %d %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b,
				&((InvBlade *)io->extra)->bladeGlamourType,
				&((InvBlade *)io->extra)->poison,
				&((InvBlade *)io->extra)->blind, 
				&((InvBlade *)io->extra)->slow,  
				&((InvBlade *)io->extra)->heal,
				&((InvBlade *)io->extra)->numOfHits);
		}
		else if( version < 2.22f )
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d %d %d %d %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b,
				&((InvBlade *)io->extra)->bladeGlamourType,
				&((InvBlade *)io->extra)->poison,
				&((InvBlade *)io->extra)->blind, 
				&((InvBlade *)io->extra)->slow,  
				&((InvBlade *)io->extra)->heal,
				&((InvBlade *)io->extra)->type,
				&((InvBlade *)io->extra)->numOfHits);

				((InvBlade *)io->extra)->tinIngots = 0;
				((InvBlade *)io->extra)->aluminumIngots = 0;
				((InvBlade *)io->extra)->steelIngots = 0;
				((InvBlade *)io->extra)->carbonIngots = 0;
				((InvBlade *)io->extra)->zincIngots = 0;
				((InvBlade *)io->extra)->adamIngots = 0;
				((InvBlade *)io->extra)->mithIngots = 0;
				((InvBlade *)io->extra)->vizIngots = 0;
				((InvBlade *)io->extra)->elatIngots = 0;
				((InvBlade *)io->extra)->chitinIngots = 0;
				((InvBlade *)io->extra)->maligIngots = 0;
		}
		else if( version < 2.25f )
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d %d %d %d %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b,
				&((InvBlade *)io->extra)->bladeGlamourType,
				&((InvBlade *)io->extra)->poison,
				&((InvBlade *)io->extra)->blind, 
				&((InvBlade *)io->extra)->slow,  
				&((InvBlade *)io->extra)->heal,
				&((InvBlade *)io->extra)->type,
				&((InvBlade *)io->extra)->numOfHits
			);
	
			fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d\n",
				&((InvBlade *)io->extra)->tinIngots,
				&((InvBlade *)io->extra)->aluminumIngots,
				&((InvBlade *)io->extra)->steelIngots,
				&((InvBlade *)io->extra)->carbonIngots,
				&((InvBlade *)io->extra)->zincIngots,
				&((InvBlade *)io->extra)->adamIngots,
				&((InvBlade *)io->extra)->mithIngots,
				&((InvBlade *)io->extra)->vizIngots,
				&((InvBlade *)io->extra)->elatIngots,
				&((InvBlade *)io->extra)->chitinIngots,
				&((InvBlade *)io->extra)->maligIngots
			);

			((InvBlade *)io->extra)->lightning = 0;
		}
		else
		{
			fscanf(fp,"%ld %ld %f %d %d %d %d %d %d %d %d %d %d %d %d\n", 
				&((InvBlade *)io->extra)->damageDone,
				&((InvBlade *)io->extra)->toHit,
				&((InvBlade *)io->extra)->size,
				&tempInt,
				&((InvBlade *)io->extra)->r,
				&((InvBlade *)io->extra)->g,
				&((InvBlade *)io->extra)->b,
				&((InvBlade *)io->extra)->bladeGlamourType,
				&((InvBlade *)io->extra)->poison,
				&((InvBlade *)io->extra)->blind, 
				&((InvBlade *)io->extra)->slow,  
				&((InvBlade *)io->extra)->heal,
				&((InvBlade *)io->extra)->lightning,
				&((InvBlade *)io->extra)->type,
				&((InvBlade *)io->extra)->numOfHits
			);
	
			fscanf(fp, "%d %d %d %d %d %d %d %d %d %d %d\n",
				&((InvBlade *)io->extra)->tinIngots,
				&((InvBlade *)io->extra)->aluminumIngots,
				&((InvBlade *)io->extra)->steelIngots,
				&((InvBlade *)io->extra)->carbonIngots,
				&((InvBlade *)io->extra)->zincIngots,
				&((InvBlade *)io->extra)->adamIngots,
				&((InvBlade *)io->extra)->mithIngots,
				&((InvBlade *)io->extra)->vizIngots,
				&((InvBlade *)io->extra)->elatIngots,
				&((InvBlade *)io->extra)->chitinIngots,
				&((InvBlade *)io->extra)->maligIngots
			);

			if( ((InvBlade *)io->extra)->lightning < 0 )
				((InvBlade *)io->extra)->lightning = 0;
		}

		((InvBlade *)io->extra)->isWielded = tempInt;

		if (version < 1.70f && BLADE_TYPE_KATANA == ((InvBlade *)io->extra)->type)
		{
			((InvBlade *)io->extra)->damageDone /= 2;
		}

		if (((InvBlade *)io->extra)->numOfHits > 22000)
			((InvBlade *)io->extra)->numOfHits = 22000;

		break;

	case INVOBJ_INGOT:
		io->extra = new InvIngot();
		fscanf(fp,"%f %f %d %d %d\n", 
			&((InvIngot *)io->extra)->damageVal,
			&((InvIngot *)io->extra)->challenge,
			&((InvIngot *)io->extra)->r,
			&((InvIngot *)io->extra)->g,
			&((InvIngot *)io->extra)->b);
		break;

	case INVOBJ_SKILL:
		io->extra = new InvSkill();
		fscanf(fp,"%ld %ld\n", 
			&((InvSkill *)io->extra)->skillLevel,
			&((InvSkill *)io->extra)->skillPoints);
		break;

	case INVOBJ_INGREDIENT:
		io->extra = new InvIngredient();
		fscanf(fp,"%d %d\n", 
			&((InvIngredient *)io->extra)->quality,
			&((InvIngredient *)io->extra)->type);

		if (((InvIngredient *)io->extra)->quality != 0) // just one quality right now
			((InvIngredient *)io->extra)->quality = 0;
		if (((InvIngredient *)io->extra)->type < 0 ||
			 ((InvIngredient *)io->extra)->type >= INGR_MAX)
			((InvIngredient *)io->extra)->type = 0;

		break;

	case INVOBJ_MEAT:
		io->extra = new InvMeat();
		fscanf(fp,"%d %d %d\n", 
			&((InvMeat *)io->extra)->quality,
			&((InvMeat *)io->extra)->type,
			&((InvMeat *)io->extra)->age);
		break;

	case INVOBJ_EGG:
		io->value = 1000;  // fix any other price

		io->extra = new InvEgg();
		fscanf(fp,"%d %d\n", 
			&((InvEgg *)io->extra)->quality,
			&((InvEgg *)io->extra)->type);
		break;

	case INVOBJ_TOTEM:
		io->extra = new InvTotem();
		it = (InvTotem *) io->extra;

		if (version < 1.09f)
		{
			fscanf(fp,"%d %d %d %ld %f\n", 
				&((InvTotem *)io->extra)->type, &((InvTotem *)io->extra)->quality, 
				&tempInt, &((InvTotem *)io->extra)->timeToDie,
				&tempFloat);
			((InvTotem *)io->extra)->isActivated = tempInt;
			((InvTotem *)io->extra)->imbueDeviation = tempFloat;
		}
		else
		{
			fscanf(fp,"%d %d", &(it->type), &(it->quality));
			fscanf(fp,"%d %f", &tempInt, &tempFloat);
			((InvTotem *)io->extra)->isActivated = tempInt;
			((InvTotem *)io->extra)->imbueDeviation = tempFloat;

			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wYear         = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wMonth        = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wDay          = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wDayOfWeek    = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wHour         = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wMinute       = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wSecond       = tempInt;
			fscanf(fp,"%d", &tempInt);
			((InvTotem *)io->extra)->timeToDie.value.wMilliseconds = tempInt;
		}

		if( version < 2.3f ) {	// Add two weeks
			((InvTotem *)io->extra)->timeToDie.AddMinutes( 60 * 24 * 14 );
		}

		if (version < 2.21f)
			fscanf(fp,"%f %f %f %f %f %f %f %f\n", 
				&((InvTotem *)io->extra)->imbue[0], &((InvTotem *)io->extra)->imbue[1],
				&((InvTotem *)io->extra)->imbue[2], &((InvTotem *)io->extra)->imbue[3],
				&((InvTotem *)io->extra)->imbue[4], &((InvTotem *)io->extra)->imbue[5],
				&((InvTotem *)io->extra)->imbue[6], &((InvTotem *)io->extra)->imbue[7]);
		else
			fscanf(fp,"%f %f %f %f %f %f %f %f %f\n", 
				&((InvTotem *)io->extra)->imbue[0], &((InvTotem *)io->extra)->imbue[1],
				&((InvTotem *)io->extra)->imbue[2], &((InvTotem *)io->extra)->imbue[3],
				&((InvTotem *)io->extra)->imbue[4], &((InvTotem *)io->extra)->imbue[5],
				&((InvTotem *)io->extra)->imbue[6], &((InvTotem *)io->extra)->imbue[7], 
				&((InvTotem *)io->extra)->imbue[8]);

		io->value = it->quality * it->quality * 14 + 1;
		if (it->quality > 12)
			io->value = it->quality * it->quality * 14 + 1 + (it->quality-12) * 1600;
		break;

	case INVOBJ_STAFF:
		io->extra = new InvStaff();
		is = (InvStaff *) io->extra;

		fscanf(fp,"%d %d", &(is->type), &(is->quality));
		fscanf(fp,"%d %d %f", &tempInt, &(is->charges), &tempFloat);
		((InvStaff *)io->extra)->isActivated = tempInt;
		((InvStaff *)io->extra)->imbueDeviation = tempFloat;

		if (version < 2.21f)
			fscanf(fp,"%f %f %f %f %f %f %f %f\n", 
				&((InvStaff *)io->extra)->imbue[0], &((InvStaff *)io->extra)->imbue[1],
				&((InvStaff *)io->extra)->imbue[2], &((InvStaff *)io->extra)->imbue[3],
				&((InvStaff *)io->extra)->imbue[4], &((InvStaff *)io->extra)->imbue[5],
				&((InvStaff *)io->extra)->imbue[6], &((InvStaff *)io->extra)->imbue[7]);
		else
			fscanf(fp,"%f %f %f %f %f %f %f %f %f\n", 
				&((InvStaff *)io->extra)->imbue[0], &((InvStaff *)io->extra)->imbue[1],
				&((InvStaff *)io->extra)->imbue[2], &((InvStaff *)io->extra)->imbue[3],
				&((InvStaff *)io->extra)->imbue[4], &((InvStaff *)io->extra)->imbue[5],
				&((InvStaff *)io->extra)->imbue[6], &((InvStaff *)io->extra)->imbue[7], 
				&((InvStaff *)io->extra)->imbue[8]);
		break;

	case INVOBJ_EXPLOSIVE:
		io->extra = new InvExplosive();
		if (version < 1.56f)
		{
			fscanf(fp,"%d %f\n", 
				&((InvExplosive *)io->extra)->type,
				&((InvExplosive *)io->extra)->quality);
			((InvExplosive *)io->extra)->power = ((InvExplosive *)io->extra)->quality;
		}
		else
		{
			fscanf(fp,"%d %f %f\n", 
				&((InvExplosive *)io->extra)->type,
				&((InvExplosive *)io->extra)->quality,
				&((InvExplosive *)io->extra)->power);
		}
		break;

	case INVOBJ_POTION:
		io->extra = new InvPotion();
		fscanf(fp,"%d %ld\n", 
			&((InvPotion *)io->extra)->type,
			&((InvPotion *)io->extra)->subType);
		break;

	case INVOBJ_FUSE:
		io->extra = new InvFuse();
		fscanf(fp,"%d %f\n", 
			&((InvFuse *)io->extra)->type,
			&((InvFuse *)io->extra)->quality);
		break;

	case INVOBJ_BOMB:
		io->extra = new InvBomb();
		fscanf(fp,"%f %f %d %d %ld %d %d %d\n", 
			&((InvBomb *)io->extra)->power,
			&((InvBomb *)io->extra)->stability,
			&((InvBomb *)io->extra)->fuseDelay,
			&((InvBomb *)io->extra)->type,
			&((InvBomb *)io->extra)->flags,
			&((InvBomb *)io->extra)->r,
			&((InvBomb *)io->extra)->g,
			&((InvBomb *)io->extra)->b);
		break;

	case INVOBJ_FAVOR:
		io->extra = new InvFavor();
		fscanf(fp,"%d\n", &tempInt);
		((InvFavor *)io->extra)->spirit = tempInt;
		break;

	case INVOBJ_GEOPART:
		io->extra = new InvGeoPart();
		fscanf(fp,"%d %f\n", 
		&((InvGeoPart *)io->extra)->type,
		&((InvGeoPart *)io->extra)->power);
		break;

	case INVOBJ_EARTHKEY:
		io->extra = new InvEarthKey();
		fscanf(fp,"%d %d %f %d %d\n", 
		&((InvEarthKey *)io->extra)->monsterType[0],
		&((InvEarthKey *)io->extra)->monsterType[1],
		&((InvEarthKey *)io->extra)->power      ,
		&((InvEarthKey *)io->extra)->height     ,
		&((InvEarthKey *)io->extra)->width      );
		break;

	default:
		// no extra data required
		fscanf(fp,"\n");

		break;
	}


	return io;
}

//******************************************************************
void InventoryObject::CopyTo(InventoryObject *io)
{
	io->mass   = mass;
	io->value  = value;
	io->type   = type;
	io->status = status;
	io->amount = amount;
	io->do_id  = do_id;
	sprintf(io->do_name, do_name);

	delete io->extra;

	switch(type)
	{
	case INVOBJ_BLADE:
		io->extra = new InvBlade();
		((InvBlade *)io->extra)->damageDone = ((InvBlade *)extra)->damageDone ;
		((InvBlade *)io->extra)->toHit      = ((InvBlade *)extra)->toHit      ;
		((InvBlade *)io->extra)->size       = ((InvBlade *)extra)->size       ;
		((InvBlade *)io->extra)->isWielded  = ((InvBlade *)extra)->isWielded  ;
		((InvBlade *)io->extra)->r          = ((InvBlade *)extra)->r  ;
		((InvBlade *)io->extra)->g          = ((InvBlade *)extra)->g  ;
		((InvBlade *)io->extra)->b          = ((InvBlade *)extra)->b  ;
		((InvBlade *)io->extra)->bladeGlamourType = ((InvBlade *)extra)->bladeGlamourType  ;
		((InvBlade *)io->extra)->poison     = ((InvBlade *)extra)->poison  ;
		((InvBlade *)io->extra)->heal       = ((InvBlade *)extra)->heal  ;
		((InvBlade *)io->extra)->slow       = ((InvBlade *)extra)->slow  ;
		((InvBlade *)io->extra)->blind      = ((InvBlade *)extra)->blind  ;
		((InvBlade *)io->extra)->lightning      = ((InvBlade *)extra)->lightning  ;
		((InvBlade *)io->extra)->numOfHits  = ((InvBlade *)extra)->numOfHits ;
		((InvBlade *)io->extra)->type       = ((InvBlade *)extra)->type ;

		((InvBlade *)io->extra)->tinIngots		= ((InvBlade *)extra)->tinIngots;
		((InvBlade *)io->extra)->aluminumIngots = ((InvBlade *)extra)->aluminumIngots;
		((InvBlade *)io->extra)->steelIngots	= ((InvBlade *)extra)->steelIngots;
		((InvBlade *)io->extra)->carbonIngots	= ((InvBlade *)extra)->carbonIngots;
		((InvBlade *)io->extra)->zincIngots		= ((InvBlade *)extra)->zincIngots;
		((InvBlade *)io->extra)->adamIngots		= ((InvBlade *)extra)->adamIngots;
		((InvBlade *)io->extra)->mithIngots		= ((InvBlade *)extra)->mithIngots;
		((InvBlade *)io->extra)->vizIngots		= ((InvBlade *)extra)->vizIngots;
		((InvBlade *)io->extra)->elatIngots		= ((InvBlade *)extra)->elatIngots;
		((InvBlade *)io->extra)->chitinIngots	= ((InvBlade *)extra)->chitinIngots;
		((InvBlade *)io->extra)->maligIngots	= ((InvBlade *)extra)->maligIngots;
		break;

	case INVOBJ_INGOT:
		io->extra = new InvIngot();
		((InvIngot *)io->extra)->damageVal  = ((InvIngot *)extra)->damageVal ;
		((InvIngot *)io->extra)->challenge  = ((InvIngot *)extra)->challenge ;
		((InvIngot *)io->extra)->r          = ((InvIngot *)extra)->r  ;
		((InvIngot *)io->extra)->g          = ((InvIngot *)extra)->g  ;
		((InvIngot *)io->extra)->b          = ((InvIngot *)extra)->b  ;
		break;

	case INVOBJ_SKILL:
		io->extra = new InvSkill();
		((InvSkill *)io->extra)->skillLevel   = ((InvSkill *)extra)->skillLevel ;
		((InvSkill *)io->extra)->skillPoints  = ((InvSkill *)extra)->skillPoints ;
		break;

	case INVOBJ_INGREDIENT:
		io->extra = new InvIngredient();
		((InvIngredient *)io->extra)->quality = ((InvIngredient *)extra)->quality;
		((InvIngredient *)io->extra)->type    = ((InvIngredient *)extra)->type;
		break;

	case INVOBJ_MEAT:
		io->extra = new InvMeat();
		((InvMeat *)io->extra)->quality = ((InvMeat *)extra)->quality;
		((InvMeat *)io->extra)->type    = ((InvMeat *)extra)->type;
		((InvMeat *)io->extra)->age     = ((InvMeat *)extra)->age;
		break;

	case INVOBJ_EGG:
		io->extra = new InvEgg();
		((InvEgg *)io->extra)->quality = ((InvEgg *)extra)->quality;
		((InvEgg *)io->extra)->type    = ((InvEgg *)extra)->type;
		break;

	case INVOBJ_TOTEM:
		io->extra = new InvTotem();
		((InvTotem *)io->extra)->imbueDeviation = ((InvTotem *)extra)->imbueDeviation ;
		((InvTotem *)io->extra)->isActivated    = ((InvTotem *)extra)->isActivated ;
		((InvTotem *)io->extra)->quality        = ((InvTotem *)extra)->quality ;
		((InvTotem *)io->extra)->timeToDie      = ((InvTotem *)extra)->timeToDie ;
		((InvTotem *)io->extra)->type           = ((InvTotem *)extra)->type ;
		((InvTotem *)io->extra)->imbue[0]       = ((InvTotem *)extra)->imbue[0] ;
		((InvTotem *)io->extra)->imbue[1]       = ((InvTotem *)extra)->imbue[1] ;
		((InvTotem *)io->extra)->imbue[2]       = ((InvTotem *)extra)->imbue[2] ;
		((InvTotem *)io->extra)->imbue[3]       = ((InvTotem *)extra)->imbue[3] ;
		((InvTotem *)io->extra)->imbue[4]       = ((InvTotem *)extra)->imbue[4] ;
		((InvTotem *)io->extra)->imbue[5]       = ((InvTotem *)extra)->imbue[5] ;
		((InvTotem *)io->extra)->imbue[6]       = ((InvTotem *)extra)->imbue[6] ;
		((InvTotem *)io->extra)->imbue[7]       = ((InvTotem *)extra)->imbue[7] ;
		((InvTotem *)io->extra)->imbue[8]       = ((InvTotem *)extra)->imbue[8] ;
		break;

	case INVOBJ_STAFF:
		io->extra = new InvStaff();
		((InvStaff *)io->extra)->imbueDeviation = ((InvStaff *)extra)->imbueDeviation ;
		((InvStaff *)io->extra)->isActivated    = ((InvStaff *)extra)->isActivated ;
		((InvStaff *)io->extra)->quality        = ((InvStaff *)extra)->quality ;
		((InvStaff *)io->extra)->charges        = ((InvStaff *)extra)->charges ;
		((InvStaff *)io->extra)->type           = ((InvStaff *)extra)->type ;
		((InvStaff *)io->extra)->imbue[0]       = ((InvStaff *)extra)->imbue[0] ;
		((InvStaff *)io->extra)->imbue[1]       = ((InvStaff *)extra)->imbue[1] ;
		((InvStaff *)io->extra)->imbue[2]       = ((InvStaff *)extra)->imbue[2] ;
		((InvStaff *)io->extra)->imbue[3]       = ((InvStaff *)extra)->imbue[3] ;
		((InvStaff *)io->extra)->imbue[4]       = ((InvStaff *)extra)->imbue[4] ;
		((InvStaff *)io->extra)->imbue[5]       = ((InvStaff *)extra)->imbue[5] ;
		((InvStaff *)io->extra)->imbue[6]       = ((InvStaff *)extra)->imbue[6] ;
		((InvStaff *)io->extra)->imbue[7]       = ((InvStaff *)extra)->imbue[7] ;
		((InvStaff *)io->extra)->imbue[8]       = ((InvStaff *)extra)->imbue[8] ;
		break;

	case INVOBJ_EXPLOSIVE:
		io->extra = new InvExplosive();
		((InvExplosive *)io->extra)->quality = ((InvExplosive *)extra)->quality;
		((InvExplosive *)io->extra)->type    = ((InvExplosive *)extra)->type;
		((InvExplosive *)io->extra)->power   = ((InvExplosive *)extra)->power;
		break;

	case INVOBJ_POTION:
		io->extra = new InvPotion();
		((InvPotion *)io->extra)->subType = ((InvPotion *)extra)->subType;
		((InvPotion *)io->extra)->type    = ((InvPotion *)extra)->type;
		break;

	case INVOBJ_FUSE:
		io->extra = new InvFuse();
		((InvFuse *)io->extra)->quality = ((InvFuse *)extra)->quality;
		((InvFuse *)io->extra)->type    = ((InvFuse *)extra)->type;
		break;

	case INVOBJ_BOMB:
		io->extra = new InvBomb();
		((InvBomb *)io->extra)->power     = ((InvBomb *)extra)->power;
		((InvBomb *)io->extra)->stability = ((InvBomb *)extra)->stability;
		((InvBomb *)io->extra)->fuseDelay = ((InvBomb *)extra)->fuseDelay;
		((InvBomb *)io->extra)->flags     = ((InvBomb *)extra)->flags;
		((InvBomb *)io->extra)->type      = ((InvBomb *)extra)->type;
		((InvBomb *)io->extra)->r         = ((InvBomb *)extra)->r;
		((InvBomb *)io->extra)->g         = ((InvBomb *)extra)->g;
		((InvBomb *)io->extra)->b         = ((InvBomb *)extra)->b;
		break;

	case INVOBJ_FAVOR:
		io->extra = new InvFavor();
		((InvFavor *)io->extra)->spirit = ((InvFavor *)extra)->spirit;
		break;

	case INVOBJ_GEOPART:
		io->extra = new InvGeoPart();
		((InvGeoPart *)io->extra)->type = ((InvGeoPart *)extra)->type;
		((InvGeoPart *)io->extra)->power = ((InvGeoPart *)extra)->power;
		break;

	case INVOBJ_EARTHKEY:
		io->extra = new InvEarthKey();
		((InvEarthKey *)io->extra)->monsterType[0] = ((InvEarthKey *)extra)->monsterType[0];
		((InvEarthKey *)io->extra)->monsterType[1] = ((InvEarthKey *)extra)->monsterType[1];
		((InvEarthKey *)io->extra)->power       = ((InvEarthKey *)extra)->power;
		((InvEarthKey *)io->extra)->width       = ((InvEarthKey *)extra)->width;
		((InvEarthKey *)io->extra)->height      = ((InvEarthKey *)extra)->height;
		break;

	default:
		// no extra data required
		break;
	}

}

//******************************************************************
//******************************************************************
Inventory::Inventory(int type, void *p)
{
	partner = NULL;
	parent = p;
	subType = type;
	money = 0;

}

//******************************************************************
Inventory::~Inventory()
{

}

//******************************************************************
void Inventory::InventorySave(FILE *fp)
{
	assert(money >= 0);
	fprintf(fp,"%ld\n", money);

	InventoryObject *io = (InventoryObject *) objects.First();
	while (io)
	{
		io->Save(fp);
		io = (InventoryObject *) objects.Next();
	}

	fprintf(fp,"XYZENDXYZ\n");
}

//******************************************************************
void Inventory::InventoryLoad(FILE *fp, float version)
{
	// destroy old inventory
	InventoryObject *io = (InventoryObject *) objects.First();
	while (io)
	{
		objects.Remove(io);
		delete io;
		io = (InventoryObject *) objects.First();
	}

	// load new
	fscanf(fp,"%ld\n", &money);
	assert(money >= 0);
//	moneyDelta = 0;

	bool bKeepReading = true;
	do 
	{
		io = LoadInventoryObject(fp, version);
		if (io)
		{
			if (io->amount == 0)
			{
				// The item technically doens't exist, so DIE
				delete io;
				io = NULL;
			}
			else
				AddItemSorted(io);
		}
		else
		{
			bKeepReading = false;
		}
	} while (bKeepReading);
}


//******************************************************************
void Inventory::AddItemSorted(InventoryObject *item)
{
	// if there's already an object by that name
	InventoryObject *io2 = (InventoryObject *) objects.Find(item->WhoAmI());

	int same = TRUE;

	if (io2 && INVOBJ_TOTEM == item->type) {
		InvTotem *t = (InvTotem *) item->extra;
		if (t->isActivated)
			same = FALSE;
		t = (InvTotem *) io2->extra;
		if (t->isActivated)
			same = FALSE;
	}
	else if (io2 && INVOBJ_STAFF == item->type) {
		InvStaff *t = (InvStaff *) item->extra;
		if (t->isActivated)
			same = FALSE;
		t = (InvStaff *) io2->extra;
		if (t->isActivated)
			same = FALSE;
	}
	else if (io2 && INVOBJ_BLADE == item->type) {
		if( ((InvBlade *)item->extra)->damageDone != ((InvBlade *)io2->extra)->damageDone )
			same = FALSE;

		if( ((InvBlade *)item->extra)->toHit != ((InvBlade *)io2->extra)->toHit )
			same = FALSE;
		
		if( ((InvBlade *)item->extra)->size != ((InvBlade *)io2->extra)->size )
			same = FALSE;

		if( ((InvBlade *)item->extra)->r != ((InvBlade *)io2->extra)->r )
			same = FALSE;
		
		if( ((InvBlade *)item->extra)->g != ((InvBlade *)io2->extra)->g )
			same = FALSE;

		if( ((InvBlade *)item->extra)->b != ((InvBlade *)io2->extra)->b )
			same = FALSE;

		if( ((InvBlade *)item->extra)->bladeGlamourType != ((InvBlade *)io2->extra)->bladeGlamourType )
			same = FALSE;

		if( ((InvBlade *)item->extra)->poison != ((InvBlade *)io2->extra)->poison )
			same = FALSE;
		if( ((InvBlade *)item->extra)->blind != ((InvBlade *)io2->extra)->blind )
			same = FALSE;
		if( ((InvBlade *)item->extra)->slow != ((InvBlade *)io2->extra)->slow )
			same = FALSE;
		if( ((InvBlade *)item->extra)->heal != ((InvBlade *)io2->extra)->heal )
			same = FALSE;

		if( ((InvBlade *)item->extra)->lightning != ((InvBlade *)io2->extra)->lightning )
			same = FALSE;

		if( ((InvBlade *)item->extra)->numOfHits != ((InvBlade *)io2->extra)->numOfHits )
			same = FALSE;

		if( ((InvBlade *)item->extra)->type != ((InvBlade *)io2->extra)->type )
			same = FALSE;

		if( ((InvBlade *)item->extra)->tinIngots != ((InvBlade *)io2->extra)->tinIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->aluminumIngots != ((InvBlade *)io2->extra)->aluminumIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->steelIngots != ((InvBlade *)io2->extra)->steelIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->carbonIngots != ((InvBlade *)io2->extra)->carbonIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->zincIngots != ((InvBlade *)io2->extra)->zincIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->adamIngots != ((InvBlade *)io2->extra)->adamIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->mithIngots != ((InvBlade *)io2->extra)->mithIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->vizIngots != ((InvBlade *)io2->extra)->vizIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->elatIngots != ((InvBlade *)io2->extra)->elatIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->chitinIngots != ((InvBlade *)io2->extra)->chitinIngots )
			same = FALSE;
		if( ((InvBlade *)item->extra)->maligIngots != ((InvBlade *)io2->extra)->maligIngots )
			same = FALSE;
	}
	else if (io2 && INVOBJ_MEAT == item->type) {
		InvMeat *t = (InvMeat *) item->extra;
		if (t->age >= 0)
			same = FALSE;
	}
	else if (io2 && INVOBJ_EGG == item->type) {
		io2->value = item->value = 1000;
	}
	else if (io2 && INVOBJ_BOMB == item->type) {
		same = FALSE;
	}
	else if (INVOBJ_EARTHKEY == item->type) {
		io2 = (InventoryObject *) objects.First();
		same = FALSE;
		while (io2 && !same)
		{
			if (IsSame(item->WhoAmI(), io2->WhoAmI()))
			{
				same = TRUE;
				InvEarthKey *ek  = (InvEarthKey *) item->extra;
				InvEarthKey *ek2 = (InvEarthKey *) io2->extra;
				
				if (ek->height != ek2->height)
					same = FALSE;
				if (ek->monsterType[0] != ek2->monsterType[0])
					same = FALSE;
				if (ek->monsterType[1] != ek2->monsterType[1])
					same = FALSE;
				if (ek->power != ek2->power)
					same = FALSE;
				if (ek->width != ek2->width)
					same = FALSE;
			}
			if (!same)
				io2 = (InventoryObject *) objects.Next();
		}
	}
	/*
	if (io2 && INVOBJ_GEOPART == item->type)
	{
		InvGeoPart *t = (InvGeoPart *) item->extra;
		if (t->age >= 0)
			same = FALSE;
	}
	*/

	if (io2 && same)
	{
		// just move an amount over
		io2->amount += item->amount;
		delete item;
		return;
	}

	int theType = item->type;
	if (INVOBJ_MEAT == theType)
		theType = -1;

	InventoryObject *io = (InventoryObject *) objects.First();
	while ( io )
	{
		int ioType = io->type;
		if (INVOBJ_MEAT == ioType)
			ioType = -1;

		if (ioType < theType)
		{
			objects.AddBefore(item,io);
			return;
		}
		else if (ioType == theType)
		{
			if (io->value < item->value)
			{
				objects.AddBefore(item,io);
				return;
			}
		}
		io = (InventoryObject *) objects.Next();
	}

	objects.Append(item);

}





/* end of file */



