//**********************************************************************************
// a file of Thom's new game, "Blade Mistress", destined to catapult
// Aggressive Game Designs into the big time!
// incept Feb, 2002
//**********************************************************************************
#ifndef CLIENT_ONLY_H
#define CLIENT_ONLY_H

#include ".\puma\pumaSound.h"
#include "bbo.h"
// defined in firstOptionMode.cpp
extern PumaSound *button1Sound;
extern PumaSound *button2Sound;

const float MAX_SOUND_DIST = 50.0f;

enum
{
	SLOT_EMPTY,
	SLOT_MOB,
	SLOT_DESERT_TREE1,
	SLOT_DESERT_TREE2,
	SLOT_FOREST_TREE1,
	SLOT_FOREST_TREE2,
	SLOT_FOREST_TREE3,
	SLOT_FOREST_TREE4,
	SLOT_TUNDRA_TREE1,
	SLOT_TUNDRA_TREE2,
	SLOT_SWAMP_TREE1,
	SLOT_SWAMP_TREE2,
	SLOT_BEACH_TREE1,
	SLOT_GIANT_MUSHROOM,
	SLOT_COLUMN,
	SLOT_CABIN,
	SLOT_CABIN2,
	SLOT_PETHEADSTONE1,
	SLOT_PETHEADSTONE2,
	SLOT_MAX
};


struct normalRecord
{
	float x, y;
//	int numOfNeighbors;
};

struct LocationSlots
{
	unsigned char used[NUM_OF_SLOTS_PER_SPACE];
//	long toughestMonsterPoints;
};

const unsigned long LOCAL_FLAGS_FOG_ON       = 0x0001;
const unsigned long LOCAL_FLAGS_MIST_ON      = 0x0002;
const unsigned long LOCAL_FLAGS_CLICKMOVE_ON = 0x0004;
const unsigned long LOCAL_FLAGS_FILTER_ON    = 0x0008;
const unsigned long LOCAL_FLAGS_HOTKEY_CTRL  = 0x0010;
const unsigned long LOCAL_FLAGS_WEATHER      = 0x0020;
const unsigned long LOCAL_FLAGS_GLAMOUR      = 0x0040;
const unsigned long LOCAL_FLAGS_WASD		 = 0x0080;

// found in firstOptionMode.cpp
extern void DrawPleaseWaitScreen(void);

#endif
