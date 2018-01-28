
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//#include "dxutil.h"
#include "BBO.h"
#include "BBO-Savatar.h"
#include "inventory.h"
#include ".\network\NetWorldMessages.h"



/*
struct TownRecord
{
	char name[64];
	int x,y;
	int size, level;
}
*/

TownRecord townList[NUM_OF_TOWNS] =
{
	{"Mangel",   15,12, 2,3},
	{"Vargras",  73,27, 1,2},
	{"Fingle",  100,46, 4,1},
	{"Binu",     63,55, 3,2},
	{"Jereston", 41,68, 1,2},
	{"Bree",    151,58, 2,3},
	{"Felloan", 148,86, 3,2},
	{"Magarnas",200,65, 4,5},
	{"Mostool",106,120, 3,4},
	{"Rac",     238,46, 1,3},
	{"Gentanos",242,12, 2,5},
	{"Aldebar", 173,29, 1,6},

	{"Mirial"    ,228,128, 2,3},
	{"Van Togo"  ,172,179, 1,2},
	{"Floan"     ,223,174, 4,1},
	{"Bora Mist" ,115,151, 3,2},
	{"Jinn"      , 46,152, 1,2},
	{"Bucansa"   , 66,178, 2,3},
	{"Quixonta"  , 27,195, 3,2},
	{"Naral"     ,140,200, 4,5},
	{"Omion"     , 29,232, 3,4},
	{"Sintil"    ,110,219, 1,3},
	{"Hoomarch"  , 72,216, 2,5},
	{"Zandall"   , 90,190, 1,6}
};

BladeRecord bladeList[6] =
{
	{"Dirk",			  2, 1},
	{"Knife",	     4, 2},	
	{"Short Sword",  8, 3},	
	{"Sword",	    16, 4},	
	{"Long Sword",	 32, 5},	
	{"Great Sword", 64, 6}	
};

BladeRecord katanaList[3] =
{
	{"Tanto" ,	  8, 4},	
	{"Katana",	 16, 5},	
	{"Tachi" ,   32, 6}	
};

BladeRecord clawList[3] =
{
	{"Talon"      ,	 8, 4},	
	{"Claw"       ,	16, 5},	
	{"Heavy Claw" ,   32, 6}	
};

BladeRecord maceList[3] =
{
	{"Small Mace" ,	 8, 4},	
	{"Mace"       ,	16, 5},	
	{"War Mace"   ,   32, 6}	
};

BladeRecord bladestaffList[3] =
{
	{"Edgestaff"  ,	 8, 4},	
	{"Bladestaff" ,   16, 5},	
	{"Swordstaff" ,   32, 6}	
};

BladeRecord chaosList[3] =
{
	{"Chaos Knife" ,	  8, 4},	
	{"Chaos Blade" ,	 16, 5},	
	{"Chaos Sword" ,   32, 6}	
};

char magicNameList[MAGIC_MAX][12] =
{
	{"BEAR"},
	{"WOLF"},
	{"EAGLE"},
	{"SNAKE"},
	{"FROG"},
	{"SUN"},
	{"MOON"},
	{"TURTLE"},
	{"EVIL"}
};

int greatTreePos[MAGIC_MAX][2] =
{
	{180, 34},
	{216, 90},
	{111,216},
	{33 ,187},

	{45 ,130},
	{50 , 82},
	{79 , 37},
	{132, 36},
	{250, 245},
};


char ingotNameList[11][19] =
{
	{"Tin"},
	{"Aluminum"},
	{"Steel"},
	{"Carbon"},
	{"Zinc"},
	{"Adamantium"},
	{"Mithril"},
	{"Vizorium"},
	{"Elatium"},
	{"Chitin"},
	{"Malignant"}
};

int ingotRGBList[11][3] =
{
	{128,128,128},
	{168,168,168},
	{228,128,128},
	{128,128,228},
	{128,228,128},
	{ 28, 28, 48},
	{228,228,228},
	{128,  0,  0},
	{  0, 50,128},
	{  0,128,128},
	{  0,128, 48}
};

float ingotPowerList[11] = {1,2,3,4,5,6,7,8,9,10,11};
long  ingotValueList[11] = {1,5,15,50,250,1000,10000,50000,75000,80000,90000};


DragonRecord dragonInfo[DRAGON_QUALITY_NUM][DRAGON_TYPE_NUM] =
{
	{
		{"Plain Red Egg"  ,   1,7,4, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,0},
		{"Plain Green Egg",   2,8,5, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,1},
		{"Plain Blue Egg" ,   3,0,6, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,2},
		{"Plain White Egg",   4,1,7, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,3},
		{"Plain Black Egg",   5,2,8, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,4},
		{"Plain Gold Egg" ,   6,3,0, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,5},
		{"Plain Rainbow Egg" ,   6,3,0, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,5}
	},													 					 					 
	{													 					 					 
		{"Striped Red Egg"  , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,6},
		{"Striped Green Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,7},
		{"Striped Blue Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,0},
		{"Striped White Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,1},
		{"Striped Black Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,2},
		{"Striped Gold Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,3},
		{"Striped Rainbow Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,3}
	},													 					 					 
	{													 					 					 
		{"Spotted Red Egg"  , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,4},
		{"Spotted Green Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,5},
		{"Spotted Blue Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,6},
		{"Spotted White Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,7},
		{"Spotted Black Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,0},
		{"Spotted Gold Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,1},
		{"Spotted Rainbow Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,1}
	},													 					 					 
	{													 					 					 
		{"Strange Red Egg"  , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,2},
		{"Strange Green Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,3},
		{"Strange Blue Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,4},
		{"Strange White Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,5},
		{"Strange Black Egg", 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,6},
		{"Strange Gold Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,7},
		{"Strange Rainbow Egg" , 1,1,1, 1,1,1,  0,0,0, 0,0,0,  2,2,2, 2,2,2,  25.0f,7}
	}													 					 					 

};

/*
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
*/


//*******************************************************************************
void LogOutput(char *logFileName, char *text)
{
	char logstr[80] = ".\\logs\\\0";
	strcat( logstr, logFileName );

	FILE *fp = fopen(logstr,"a");
	fprintf(fp,text);
	fclose(fp);
}


//*******************************************************************************
void LoadPetData(void)
{
	FILE *fp = fopen("serverdata\\petDragonData.txt","r");
	assert(fp);

	char tempText[1028]; // to hold a string we discard

	for (int qual = 0; qual < DRAGON_QUALITY_NUM; ++qual)
	{
		for (int type = 0; type < DRAGON_TYPE_NUM; ++type)
		{
			DragonRecord *dr = &dragonInfo[qual][type];

	  		fscanf(fp,"%s", tempText); // scan over the name tag 

	  		fscanf(fp,"%d %d %d %d %d %d", 
				 &dr->goodMeatType[0], &dr->goodMeatType[1], &dr->goodMeatType[2], 
				 &dr->goodMeatTypeResult[0], &dr->goodMeatTypeResult[1], 
				 &dr->goodMeatTypeResult[2]);

	  		fscanf(fp,"%d %d %d %d %d %d", 
				 &dr->okayMeatType[0], &dr->okayMeatType[1], &dr->okayMeatType[2], 
				 &dr->okayMeatTypeResult[0], &dr->okayMeatTypeResult[1], 
				 &dr->okayMeatTypeResult[2]);

	  		fscanf(fp,"%d %d %d", 
				 &dr->breedMeatType[0], &dr->breedMeatType[1], &dr->breedMeatType[2]);

	  		fscanf(fp,"%d %d %d", 
				 &dr->powerMeatType[0], &dr->powerMeatType[1], &dr->powerMeatType[2]);

	  		fscanf(fp,"%f %d\n", 
				 &dr->attackDamageBase, &dr->powerBirthMagicType);

			/*
			for (int i = 0; i < 3; ++i)
			{
				if (dr->okayMeatType[i] == dr->goodMeatType[i] ||
				    dr->okayMeatType[i] == dr->breedMeatType[i] ||
				    dr->okayMeatType[i] == dr->powerMeatType[i])
				{
					sprintf(tempText,"dup: okayMeatType %d   qual:%d type:%d",i, qual, type);
					DEBUG_MSG(tempText);
				}
				if (dr->goodMeatType[i] == dr->okayMeatType[i] ||
				    dr->goodMeatType[i] == dr->breedMeatType[i] ||
				    dr->goodMeatType[i] == dr->powerMeatType[i])
				{
					sprintf(tempText,"dup: goodMeatType %d   qual:%d type:%d",i, qual, type);
					DEBUG_MSG(tempText);
				}
				if (dr->breedMeatType[i] == dr->goodMeatType[i] ||
				    dr->breedMeatType[i] == dr->okayMeatType[i] ||
				    dr->breedMeatType[i] == dr->powerMeatType[i])
				{
					sprintf(tempText,"dup: breedMeatType %d   qual:%d type:%d",i, qual, type);
					DEBUG_MSG(tempText);
				}
				if (dr->powerMeatType[i] == dr->goodMeatType[i] ||
				    dr->powerMeatType[i] == dr->breedMeatType[i] ||
				    dr->powerMeatType[i] == dr->okayMeatType[i])
				{
					sprintf(tempText,"dup: powerMeatType %d   qual:%d type:%d",i, qual, type);
					DEBUG_MSG(tempText);
				}
			}
			*/
		}
	}

	fclose(fp);
}

float terrainDrawValue = 60.0f;
int grassDensity = 3;

//*******************************************************************************
void SaveOptions(void)
{
	FILE *fp = fopen("options.dat","w");
	fprintf(fp,"%f %d\n", terrainDrawValue, grassDensity);
	fclose(fp);
}

//*******************************************************************************
void LoadOptions(void)
{
	FILE *fp = fopen("options.dat","r");
	if (fp)
	{
		fscanf(fp,"%f %d\n", &terrainDrawValue, &grassDensity);
		fclose(fp);
	}
}


char ageTextArray[6][15] =
{
	{"Young"},
	{"Young Adult"},
	{"Adult"},
	{"Mature"},
	{"Elder"},
	{"Counselor"}
};

//*******************************************************************************
int CanHaveROP(int age, int clevel)
{
	
	if (6 == age)
		return FALSE;
	if (5 == age && clevel >= 310 )
		return TRUE;
	if (4 == age && clevel >= 150)
		return TRUE;
	if (3 == age && clevel >= 70)
		return TRUE;
	if (2 == age && clevel >= 30)
		return TRUE;
	if (1 == age && clevel >= 10)
		return TRUE;

	return FALSE;
}

//*******************************************************************************
void FillBladeDescMessage(MessBladeDesc *mess, InventoryObject *weapon, BBOSAvatar *av)
{
	InvBlade *iBlade = (InvBlade *) weapon->extra;

	mess->bladeID      = (long)weapon;
	mess->size         = iBlade->size;
	mess->r            = iBlade->r;
	mess->g            = iBlade->g;
	mess->b            = iBlade->b;
	mess->avatarID     = av->socketIndex;
	mess->meshType     = iBlade->type;

	mess->trailType    = iBlade->bladeGlamourType;
	mess->black        = iBlade->poison;
	mess->green        = iBlade->heal;
	mess->red          = iBlade->slow;
	mess->white        = iBlade->blind;
	mess->blue         = iBlade->toHit-1;
	if (BLADE_TYPE_KATANA == iBlade->type)
		mess->blue -= 20;
}


/* end of file */



