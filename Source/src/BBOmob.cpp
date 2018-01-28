
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOMob.h"
#include "monsterData.h"

//LPDIRECT3DTEXTURE8 BBOMob::skins[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
PumaAnim * BBOMob::monsterAnims[3][NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
int BBOMob::refCount[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
float BBOMob::shadowSize[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
int monsterAnimsInited = 0;

MonsterInfo monsterInfo[NUM_OF_MONSTERS] =
{
	//******** normal monsters
	{"dat\\ghost-idle", "dat\\ghost-fly", "dat\\ghost-attack", 
		"dat\\ghost.png", "dat\\whight.png", "dat\\spectre.png", 
		"dat\\ghostBlueLady.png", "", "", 
		 0.1f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\golem-stand", "dat\\golem-run", "dat\\golem-attack", 
		"dat\\golemWood.PNG", "dat\\golemDrek.PNG", "dat\\golemDirt.PNG", 
		"dat\\golemMetal.PNG", "dat\\golemStoun.PNG", "dat\\golem_granite.PNG", 
		 0.4f, 0.5f, 0.45f, 0.6f, 0.65f, 0.7f},

	{"dat\\minotaur-stand", "dat\\minotaur-run", "dat\\minotaur-attack", 
		"dat\\MinotaurSnakeGreen.png", "dat\\minotaur.png", "dat\\minotaurLion.png", 
		"dat\\MinotaurMarble.png", "dat\\minotaurArmor.png", "dat\\MinotaurFire1.png", 
		 0.5f, 0.8f, 1.0f, 1.05f, 1.1f, 1.2f},

	{"dat\\tiger-stand", "dat\\tiger-run", "dat\\tiger-attack", 
		"dat\\tiger-regular.png", "dat\\tiger-ice.png", "dat\\TigerJaguar.png", 
		"", "", "", 
		 1.0f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f},
	{"dat\\skeleton-stand", "dat\\skeleton-walk", "dat\\skeleton-attack", 
		"dat\\skeleton.png", "", "", 
		"", "", "", 
		 0.3f, 1.2f, 1.0f, 1.0f, 1.0f, 1.0f},
	{"dat\\plant-stand", "dat\\plant-walk", "dat\\plant-attack", 
		"dat\\plant.png", "dat\\DiamondBackPlant.png", "", 
		"", "", "", 
		 0.15f, 0.25f, 1.0f, 1.0f, 1.0f, 1.0f},
	{"dat\\werewolf-stand", "dat\\werewolf-run", "dat\\werewolf-attack", 
		"dat\\werewolf.png", "dat\\RedWerewolf.png", "dat\\ArcticWerewolf.png",
		"dat\\WerewolfMalacite.png",  "dat\\werewolfStone.png",  "", 
		 10.0f, 10.0f, 10.0f, 10.0f, 10.0f, 10.0f},
	{"dat\\dragon-stand", "dat\\dragon-fly", "dat\\dragon-attack", 
		"dat\\dragon.png", "dat\\QueenDragon.png", "dat\\GuardianDragon.png", 
		"dat\\IrredescentDragon.png", "dat\\RegalDragon.png", "dat\\blackness.bmp", 
		 1.5f, 2.2f, 2.0f, 2.2f, 2.6f, 1.3f},
	{"dat\\spider-idle", "dat\\spider-walk", "dat\\spider-attack", 
		"dat\\spider.png", "dat\\NightSpider.png", "dat\\StickySpider.png", 
		"dat\\HairySpider.png", "dat\\PoisonSpider.png", "dat\\RedSpider.png", 
		 1.0f, 1.2f, 1.3f, 1.4f, 1.5f, 2.0f},



	//******** spirit monsters
	{"dat\\ghost-idle", "dat\\ghost-fly", "dat\\ghost-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 0.1f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\golem-stand", "dat\\golem-run", "dat\\golem-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 0.5f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\minotaur-stand", "dat\\minotaur-run", "dat\\minotaur-attack", 
		"dat\\realm-spirits-sky.png", "dat\\MinotaurMarble.png", "", "", "", "", 
		 1.0f, 1.5f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\tiger-stand", "dat\\tiger-run", "dat\\tiger-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 1.0f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\skeleton-stand", "dat\\skeleton-walk", "dat\\skeleton-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 0.3f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\plant-stand", "dat\\plant-walk", "dat\\plant-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 0.15f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\werewolf-stand", "dat\\werewolf-run", "dat\\werewolf-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 10.1f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\dragon-stand", "dat\\dragon-fly", "dat\\dragon-attack", 
		"dat\\realm-spirits-sky.png", "dat\\GarnetDragon.png", "", "", "", "", 
		 1.3f, 2.2f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\spider-idle", "dat\\spider-walk", "dat\\spider-attack", 
		"dat\\realm-spirits-sky.png", "", "", "", "", "", 
		 1.2f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	//******** dead monsters
	{"dat\\ghost-idle", "dat\\ghost-fly", "dat\\ghost-attack", 
		"dat\\shade.png", "dat\\haunt.png", "dat\\wraith.png", "dat\\wraith.png", "", "", 
		 0.11f, 0.13f, 0.15f, 0.18f, 1.0f, 1.0f},

	{"dat\\skeleton-stand", "dat\\skeleton-walk", "dat\\skeleton-attack", 
		"dat\\bone-warrior.png", "dat\\bone-sargent.png", "dat\\bone-lt.png", 
		"dat\\bone-general.png", "dat\\bone-general.png", "dat\\bone-general.png",
		 0.4f, 0.5f, 0.6f, 0.8f, 0.9f, 1.0f},

	{"dat\\werewolf-stand", "dat\\werewolf-run", "dat\\werewolf-attack", 
		"dat\\anubis.png", "", "", "", "", "", 
		 30.0f, 0.105f, 0.11f, 0.08f, 1.0f, 1.0f},

	{"dat\\ghost-idle", "dat\\ghost-fly", "dat\\ghost-attack", 
		"dat\\thief.png", "", "", "", "", "", 
		 0.08f, 0.13f, 0.15f, 0.18f, 1.0f, 1.0f},

	//******** dragon realm monsters
	{"dat\\orc-idle", "dat\\orc-walk", "dat\\orc-attack", 
		"dat\\orc.png", "dat\\orc2.png", "dat\\orc3.png", 
		                "dat\\orc4.png", "dat\\orc5.png", "dat\\blackness.bmp", 
		 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.6f},

	{"dat\\wurm-stand", "dat\\wurm-fly", "dat\\wurm-attack", 
		"dat\\wurm.png", "dat\\ZebraWurm.png", "dat\\blackness.bmp", "", "", "",
		 1.0f, 1.1f, 1.2f, 0.8f, 0.9f, 1.0f},

	{"dat\\spider-idle", "dat\\spider-walk", "dat\\spider-attack", 
		"dat\\HornetSpider.png", "dat\\WorkerSpider.png", "dat\\DroneSpider2.png", 
		"dat\\QueenSpider.png", "dat\\GreySpider.png", "", 
		 1.5f, 1.2f, 1.3f, 1.6f, 1.5f, 2.0f},

	{"dat\\vamp-stand", "dat\\vamp-walk", "dat\\vamp-attack", 
		"dat\\zombie.png", "dat\\ghoul.png", "dat\\vamp.png", 
		"dat\\ashVamp.png", "dat\\pitVamp.png", "", 
		 0.03f, 0.035f, 0.04f, 0.045f, 0.05f, 0.055f},

	{"dat\\bat-fly", "dat\\bat-fly", "dat\\bat-attack", 
		"dat\\bat.png", "dat\\bat2.png", "dat\\bat3.png", 
		"dat\\bat4.png", "dat\\bat5.png", "dat\\bat6.png", 
		 0.05f, 0.06f, 0.07f, 0.08f, 0.09f, 0.10f},

	{"dat\\vlord-idle", "dat\\vlord-attack2", "dat\\vlord-attack", 
		"dat\\v_lord.png", "dat\\v_lord2.png", "dat\\v_lord3.png", 
		"dat\\v_lord4.png", "dat\\v_lord5.png", "dat\\v_lord6.png", 
		 0.3f, 0.35f, 0.4f, 0.45f, 0.5f, 0.55f},

	// Lizard Man
	{"dat\\lizard-idle", "dat\\lizard-walk", "dat\\lizard-attack", 
		"dat\\lizard.png", "dat\\lizard.png", "dat\\lizard.png", 
		"dat\\lizard.png", "dat\\lizard.png", "dat\\lizard-tiger.png", 
		 0.7f, 0.8f, 0.9f, 1.2f, 1.4f, 1.7f}

};

//******************************************************************
BBOMob::BBOMob(int doid, char *doname) : DataObject(doid,doname)
{
	type = SMOB_AVATAR;
	avatarID = -1;
	avTexture = NULL;
	cellSlot = -1;
//	isMonster = FALSE;
	staticMonsterFlag = FALSE;
	name[0] = 0;
	guildName[0] = 0;
	blade = NULL;
	bladeType = 0;
	chestType = 0;
	attacking = attackingSpecial = moving = dying = beyondRange = inSecondForm = FALSE;
	animCounter = 0;
	lastStepIndex = -1;
	chantType = -1;
	magicAttackType = -1;

	screenHealth = screenMaxHealth = screenHealthTimer = 0;

	customMonsterAnims[0] = customMonsterAnims[1] = customMonsterAnims[2] = NULL;

	lasta = lastr = lastg = lastb = 255;
	lastSize = 1.0f;

	for (int i = 0; i < 2; i++)
	{
		pet[i].Init(NULL);
	}

	for (int i = 0; i < 5; i++)
		bladeParticleAmount[i] = 0;  // means no particles
	hasTrail = FALSE;

	if (!monsterAnimsInited)
	{
		for (int i = 0; i < NUM_OF_MONSTERS; ++i)
		{
			for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
			{
				refCount[i][j] = 0;
			}
		}
		monsterAnimsInited = 1;
	}
}

//******************************************************************
void BBOMob::InitMonsterInfo(long mType, long mSubType)
{

	monsterType    = mType;
	monsterSubType = mSubType;

	sprintf(name, monsterData[mType][mSubType].name);

	DWORD color = 0xffffffff;
	if (MONSTER_PLACE_SPIRITS & monsterData[mType][mSubType].placementFlags)
	{
		color = 0x80ffffff;
//		if (11 == mType && 1 == mSubType) // dokk's centurion
	}
	if (refCount[mType][mSubType] < 1)
	{
		FILE *fp;
		char tempText [1024];
		char tempText2[1024];
		int isASE;

		int useTransparency = 0;
		if (23 == mType)	// wurm
			useTransparency = 1;


		for (int i = 0; i < 3; i++)
		{
			monsterAnims[i][mType][mSubType] = new PumaAnim(i,"MONSTER_ANIM");
			sprintf(tempText,"%s.ASE", monsterInfo[mType].meshName[i]);
			sprintf(tempText2,"%s.anc", monsterInfo[mType].meshName[i]);

			fp = fopen(tempText,"rb");
			if (!fp)
				isASE = FALSE;
			else
			{
				isASE = TRUE;
				fclose(fp);
			}

			if (isASE)
			{
				monsterAnims[i][mType][mSubType]->
					      LoadFromASC(puma->m_pd3dDevice, tempText);
				monsterAnims[i][mType][mSubType]->SaveCompressed(puma->m_pd3dDevice, tempText2);
				monsterAnims[i][mType][mSubType]->
					      LoadTexture(puma->m_pd3dDevice,	monsterInfo[mType].skinName[mSubType],useTransparency);
				float scaleVal = monsterInfo[mType].size[mSubType] * 0.02f;
				monsterAnims[i][mType][mSubType]->
							Scale(puma->m_pd3dDevice, scaleVal, scaleVal, scaleVal);
			}
			else
			{
				monsterAnims[i][mType][mSubType]->LoadCompressed(puma->m_pd3dDevice, tempText2, color);
				monsterAnims[i][mType][mSubType]->
					      LoadTexture(puma->m_pd3dDevice,	monsterInfo[mType].skinName[mSubType],useTransparency);
				float scaleVal = monsterInfo[mType].size[mSubType] * 0.02f;
				monsterAnims[i][mType][mSubType]->
							Scale(puma->m_pd3dDevice, scaleVal, scaleVal, scaleVal);
			}
		}
	}
	++refCount[mType][mSubType];

}


//******************************************************************
BBOMob::~BBOMob()
{
	if (SMOB_MONSTER == type)
	{
		--refCount[monsterType][monsterSubType];
		if (refCount[monsterType][monsterSubType] < 1)
		{
			for (int i = 0; i < 3; i++)
			{
				delete monsterAnims[i][monsterType][monsterSubType];
			}
		}
	}

	SAFE_DELETE(customMonsterAnims[2]);
	SAFE_DELETE(customMonsterAnims[1]);
	SAFE_DELETE(customMonsterAnims[0]);

	if (blade)
		delete blade;
	SAFE_DELETE(avTexture);

	for (int i = 0; i < 2; i++)
	{
		pet[i].Clear();
	}
}


//******************************************************************
void BBOMob::InitCustomMonster(char *n, 
				unsigned char a, unsigned char r, 
				unsigned char g, unsigned char b, 
				float size)
{

	sprintf(name,n);
	if (a == lasta && r == lastr && g == lastg && b == lastb && lastSize == size)
		return;  // nothing more to do.

	lasta = a;
	lastr = r;
	lastg = g;
	lastb = b;
	lastSize = size;

	SAFE_DELETE(customMonsterAnims[2]);
	SAFE_DELETE(customMonsterAnims[1]);
	SAFE_DELETE(customMonsterAnims[0]);

	DWORD color = D3DCOLOR_ARGB(a,r,g,b);

	FILE *fp;
	char tempText [1024];
	char tempText2[1024];
	int isASE;

	for (int i = 0; i < 3; i++)
	{
		customMonsterAnims[i] = new PumaAnim(i,"MONSTER_ANIM");
		sprintf(tempText,"%s.ASE", monsterInfo[monsterType].meshName[i]);
		sprintf(tempText2,"%s.anc", monsterInfo[monsterType].meshName[i]);

		fp = fopen(tempText,"rb");
		if (!fp)
			isASE = FALSE;
		else
		{
			isASE = TRUE;
			fclose(fp);
		}

		if (isASE)
		{
			customMonsterAnims[i]->
				      LoadFromASC(puma->m_pd3dDevice, tempText);
			customMonsterAnims[i]->SaveCompressed(puma->m_pd3dDevice, tempText2);
			customMonsterAnims[i]->
				      LoadTexture(puma->m_pd3dDevice,	monsterInfo[monsterType].skinName[monsterSubType]);
			float scaleVal = monsterInfo[monsterType].size[monsterSubType] * 0.02f * size;
			customMonsterAnims[i]->
						Scale(puma->m_pd3dDevice, scaleVal, scaleVal, scaleVal);
		}
		else
		{
			customMonsterAnims[i]->LoadCompressed(puma->m_pd3dDevice, tempText2, color);
			customMonsterAnims[i]->
				      LoadTexture(puma->m_pd3dDevice,	monsterInfo[monsterType].skinName[monsterSubType]);
			float scaleVal = monsterInfo[monsterType].size[monsterSubType] * 0.02f * size;
			customMonsterAnims[i]->
						Scale(puma->m_pd3dDevice, scaleVal, scaleVal, scaleVal);
		}
	}

}



/* end of file */



