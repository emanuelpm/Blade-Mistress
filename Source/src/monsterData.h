#ifndef MONSTERDATA_H
#define MONSTERDATA_H

#include "BBO.h"

struct MonsterData
{
	char name[64];
	long maxHealth, damageDone, toHit, defense;
	long dropAmount, dropType, townDist;
	int  dungeonType;	// 0-4, 4 is the toughest dungeon
	unsigned long placementFlags;
};

extern MonsterData monsterData[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];


const unsigned long MONSTER_PLACE_DUNGEON	= 0x0001;
const unsigned long MONSTER_PLACE_GRASS		= 0x0002;
const unsigned long MONSTER_PLACE_SNOW		= 0x0004;
const unsigned long MONSTER_PLACE_DESERT	= 0x0008;
const unsigned long MONSTER_PLACE_SWAMP		= 0x0010;
const unsigned long MONSTER_PLACE_SPIRITS	= 0x0020;
const unsigned long MONSTER_PLACE_DEAD		= 0x0040;
const unsigned long MONSTER_PLACE_DRAGONS	= 0x0080;
const unsigned long MONSTER_PLACE_LABYRINTH	= 0x0100;
const unsigned long MONSTER_PLACE_WASTE		= 0x0200;
const unsigned long MONSTER_PLACE_LABY2		= 0x0400;
const unsigned long MONSTER_PLACE_LABY3		= 0x0800;
const unsigned long MONSTER_PLACE_ROP		= 0x1000;


const unsigned long MONSTER_PLACE_ALL_LAND	=	MONSTER_PLACE_GRASS + 
												MONSTER_PLACE_SNOW + 
												MONSTER_PLACE_DESERT +   
												MONSTER_PLACE_SWAMP;                          

const unsigned long MONSTER_PLACE_ALL		=	MONSTER_PLACE_DUNGEON + 
												MONSTER_PLACE_GRASS + 
												MONSTER_PLACE_SNOW + 
												MONSTER_PLACE_DESERT +
												MONSTER_PLACE_SWAMP;
#endif
