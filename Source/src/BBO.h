#ifndef BBO_H
#define BBO_H

#include "helper/PasswordHash.h"

struct MessBladeDesc;
class InventoryObject;
class BBOSAvatar;

// general defines and values that are common and specific for the WHOLE game,
// client and server alike.

const int NUM_OF_CHARS_PER_USER = 8;

const int NUM_OF_CHARS_FOR_USERNAME = 32;
const int NUM_OF_CHARS_FOR_PASSWORD = OUT_HASH_SIZE + 1;

const int MAP_SIZE_WIDTH  = 256;
const int MAP_SIZE_HEIGHT = 256;

const int NUM_OF_SLOTS_PER_SPACE = 28;

const int NUM_OF_TOWNS = 24;

const int NUM_OF_MONSTERS = 29;
const int NUM_OF_MONSTER_SUBTYPES = 6;

const int NUM_OF_DUNGEON_WALL_TYPES  = 11; // 0 = open, no wall
const int NUM_OF_DUNGEON_FLOOR_TYPES = 4;
const int NUM_OF_TOWER_WALL_TYPES  = 8; // 0 = open, no wall
const int NUM_OF_TOWER_FLOOR_TYPES = 7;
const int DUNGEON_PIECE_SIZE = 5;

const int NUM_OF_FACES = 6;
const int NUM_OF_TOPS = 5;
const int NUM_OF_BOTTOMS = 4;

const float FPNORMALSIZE = 0.3f;

const char TEXT_COLOR_TELL     = (char)255;
const char TEXT_COLOR_SHOUT    = (char)254;
const char TEXT_COLOR_ANNOUNCE = (char)253;
const char TEXT_COLOR_DATA     = (char)252;
const char TEXT_COLOR_EMOTE    = (char)251;
const char TEXT_COLOR_GUILD    = (char)250;
const char TEXT_COLOR_GOD      = (char)249;

enum
{
	MAGIC_BEAR,
	MAGIC_WOLF,
	MAGIC_EAGLE,
	MAGIC_SNAKE,
	MAGIC_FROG,
	MAGIC_SUN,
	MAGIC_MOON,
	MAGIC_TURTLE,
	MAGIC_EVIL,
	MAGIC_MAX
};

enum
{
	SMOB_AVATAR,
	SMOB_MONSTER,
	SMOB_TRADER,
	SMOB_TRAINER,
	SMOB_TOWNMAGE,
	SMOB_TREE,
	SMOB_ITEM_SACK,
	SMOB_TOWER,
	SMOB_CHEST,
	SMOB_MONSTER_GRAVE,
	SMOB_PARTICLE_STREAM,
	SMOB_FLOWERS,
	SMOB_BOMB,
	SMOB_TOKEN,
	SMOB_WARP_POINT,
	SMOB_BANKER,
	SMOB_WITCH,
	SMOB_GROUND_EFFECT,
	SMOB_ROPENTERANCE,
	SMOB_CASTLE,
	SMOB_MAX
};

enum
{
	TMSERVICE_TELEPORT,
	TMSERVICE_TELEPORT_BACK,
	TMSERVICE_HEAL,
	TMSERVICE_BANK,
	TMSERVICE_MAX
};

enum
{
	SPECIAL_APP_NOTHING,
	SPECIAL_APP_TELEPORT,
	SPECIAL_APP_ADMIN_TELEPORT,
	SPECIAL_APP_TELEPORT_AWAY,
	SPECIAL_APP_DEATH,
	SPECIAL_APP_RES,
	SPECIAL_APP_DUNGEON,
	SPECIAL_APP_ENTER_GAME,
	SPECIAL_APP_LEAVE_GAME,
	SPECIAL_APP_HEAL,
	SPECIAL_MAX
};

enum
{
	REALM_ID_SPIRITS,
	REALM_ID_DEAD,
	REALM_ID_DRAGONS,
	REALM_ID_LAB1,
	REALM_ID_LAB2,
	REALM_ID_MAX
};

enum
{
	CONTACT_NONE,
	CONTACT_FRIEND,
	CONTACT_IGNORE,
	CONTACT_MAX
};

enum
{
	FRIEND_ACTION_LIST,
	FRIEND_ACTION_ADD,
	FRIEND_ACTION_REMOVE,
	FRIEND_ACTION_IGNORE,
	FRIEND_ACTION_MAX
};

// cannot change the values of this list; save files would get screwed up
enum
{
	BLADE_TYPE_NORMAL = 0,
	BLADE_TYPE_KATANA = 1,
	BLADE_TYPE_MACE   = 2,
	BLADE_TYPE_CHAOS  = 3,
	BLADE_TYPE_CLAWS  = 4,
	BLADE_TYPE_DOUBLE = 5,
	BLADE_TYPE_STAFF1 = 128
};

enum
{
	BLADE_GLAMOUR_NONE,
	BLADE_GLAMOUR_TOHIT1,
	BLADE_GLAMOUR_TOHIT2,
	BLADE_GLAMOUR_TOHIT3,
	BLADE_GLAMOUR_TOHIT4,
	BLADE_GLAMOUR_SPIRIT1,
	BLADE_GLAMOUR_SPIRIT2,
	BLADE_GLAMOUR_SPIRIT3,
	BLADE_GLAMOUR_SPIRIT4,
	BLADE_GLAMOUR_BLIND1,
	BLADE_GLAMOUR_BLIND2,
	BLADE_GLAMOUR_BLIND3,
	BLADE_GLAMOUR_BLIND4,
	BLADE_GLAMOUR_SLOW1,
	BLADE_GLAMOUR_SLOW2,
	BLADE_GLAMOUR_SLOW3,
	BLADE_GLAMOUR_SLOW4,
	BLADE_GLAMOUR_HEAL1,
	BLADE_GLAMOUR_HEAL2,
	BLADE_GLAMOUR_HEAL3,
	BLADE_GLAMOUR_HEAL4,
	BLADE_GLAMOUR_POISON1,
	BLADE_GLAMOUR_POISON2,
	BLADE_GLAMOUR_POISON3,
	BLADE_GLAMOUR_POISON4,
	BLADE_GLAMOUR_TRAILWHITE,
	BLADE_GLAMOUR_TRAILRED,
	BLADE_GLAMOUR_TRAILBLUE,
	BLADE_GLAMOUR_TRAILGREEN,
	BLADE_GLAMOUR_TRAILAQUA,
	BLADE_GLAMOUR_TRAILPURPLE,
	BLADE_GLAMOUR_TRAILYELLOW,
	BLADE_GLAMOUR_TRAILORANGE,
	BLADE_GLAMOUR_TRAILPINK,
	BLADE_GLAMOUR_MAX
};

enum
{
	CHANT_REALM_SPIRITS,
	CHANT_REALM_DEAD,
	CHANT_LEAVE_REALM,
	CHANT_CREATE_GUILD,
	CHANT_PLANT_TOWER,
	CHANT_INDUCT_MEMBER,
	CHANT_EDIT_TOWER,
	CHANT_GROUP_PRAYER,
	CHANT_ROP,
	CHANT_ROP_RESPONSE,
	CHANT_MAX
};

const unsigned long SPECIAL_LOOK_HELPER          = 0x0001;
const unsigned long SPECIAL_LOOK_DOKK_KILLER     = 0x0002;
const unsigned long SPECIAL_LOOK_ANUBIS_KILLER   = 0x0004;
const unsigned long SPECIAL_LOOK_OVERLORD_KILLER = 0x0008;

// used by server and client to keep the four different lists straight
enum 
{
	GTM_BUTTON_LIST_INV = 200,
	GTM_BUTTON_LIST_WRK = 201,
	GTM_BUTTON_LIST_SKL = 202,
	GTM_BUTTON_LIST_WLD = 203
};

struct TownRecord
{
	char name[64];
	int x,y;
	int size, level;
};

struct BladeRecord
{
	char name[64];
	int size, damage;
};

struct DragonRecord
{
	char eggName[64];
	int goodMeatType[3];	      // what type of meat makes cool transform at each stage
	int goodMeatTypeResult[3];	// what type to transform to for good meat
	int okayMeatType[3];			// what type of meat makes okay transform at each stage
	int okayMeatTypeResult[3];	// what type to transform to for okay meat             
	int breedMeatType[3];		// what type of meat makes dragon drop egg at each stage
	int powerMeatType[3];		// what type of meat makes dragon stronger for each stage
	float attackDamageBase;		// damage is attackDamageBase * (1+lifeStage)
	int powerBirthMagicType;   // this type of magic makes a better pet dragon
};

enum
{
	DRAGON_TYPE_RED,
	DRAGON_TYPE_GREEN,
	DRAGON_TYPE_BLUE,
	DRAGON_TYPE_WHITE,
	DRAGON_TYPE_BLACK,
	DRAGON_TYPE_GOLD,
	DRAGON_TYPE_WONDER,
	DRAGON_TYPE_NUM
};

enum
{
	MONSTER_EFFECT_TYPE_RED,
	MONSTER_EFFECT_TYPE_GREEN,
	MONSTER_EFFECT_TYPE_BLUE,
	MONSTER_EFFECT_TYPE_WHITE,
	MONSTER_EFFECT_TYPE_BLACK,
	MONSTER_EFFECT_TYPE_GOLD,
	MONSTER_EFFECT_TYPE_WONDER,
	MONSTER_EFFECT_STUN,
	MONSTER_EFFECT_BIND,
	MONSTER_EFFECT_TYPE_NUM
};

enum
{
	DRAGON_QUALITY_PLAIN,
	DRAGON_QUALITY_STRIPED,
	DRAGON_QUALITY_SPOTTED,
	DRAGON_QUALITY_PATTERN,
	DRAGON_QUALITY_NUM
};

enum
{
	DRAGON_HEALTH_GREAT,
	DRAGON_HEALTH_NORMAL,
	DRAGON_HEALTH_SICK,
	DRAGON_HEALTH_NUM
};

const unsigned long INFO_FLAGS_HITS       = 0x0001;
const unsigned long INFO_FLAGS_MISSES     = 0x0002;
const unsigned long INFO_FLAGS_LOOT_TAKEN = 0x0004;
const unsigned long INFO_FLAGS_ANNOUNCE   = 0x0008;


extern TownRecord townList[NUM_OF_TOWNS];
extern BladeRecord bladeList[6];
extern BladeRecord katanaList[3], clawList[3], bladestaffList[3], chaosList[3], maceList[3];
extern char magicNameList[MAGIC_MAX][12];
extern int greatTreePos[MAGIC_MAX][2];

extern char  ingotNameList[11][19];
extern float ingotPowerList[11];
extern long  ingotValueList[11];
extern int   ingotRGBList[11][3];

extern DragonRecord dragonInfo[DRAGON_QUALITY_NUM][DRAGON_TYPE_NUM];

extern void LoadPetData(void);

extern void DebugOutput(char *text);
extern void LogOutput(char *logFileName, char *text);

extern float terrainDrawValue;
extern int   grassDensity;
extern void  SaveOptions(void);
extern void  LoadOptions(void);

extern char ageTextArray[6][15];
extern int CanHaveROP(int age, int clevel);
extern void FillBladeDescMessage(MessBladeDesc *mess, InventoryObject *weapon, BBOSAvatar *av);

#endif
