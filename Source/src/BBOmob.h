#ifndef BBOMOB_H
#define BBOMOB_H

#include ".\helper\linklist.h"
#include ".\puma\puma.h"
#include "flockPoints.h"
#include "BBO.h"
#include "BBOpet.h"
#include "avatarTexture.h"
#include ".\puma\pumaanim.h"
#include ".\puma\pumamesh.h"

enum
{
	MONSTER_STATE_STAND,
	MONSTER_STATE_RUN,
	MONSTER_STATE_ATTACK,
	MONSTER_STATE_MAX
};

struct MonsterInfo
{
	char meshName[3][128];
	char skinName[NUM_OF_MONSTER_SUBTYPES][128];
	float size[NUM_OF_MONSTER_SUBTYPES];
};

class BBOMob : public DataObject
{
public:

	BBOMob(int doid, char *doname);
	virtual ~BBOMob();

	void InitMonsterInfo(long mType, long mSubType);
	void InitCustomMonster(char *n, 
				unsigned char a, unsigned char r, 
				unsigned char g, unsigned char b, 
				float size);


	SpacePoint spacePoint;

	int type;
	int cellX, cellY, cellSlot;
	unsigned long flockPointIndex;
	float damage;

	int avatarID; // -1 = not an avatar
	unsigned long mobID;
//	int mobType;
//	int isMonster;
	long monsterType, monsterSubType, staticMonsterFlag;
	unsigned char towerType, towerHasMistress, chestType;
	int animCounter;
	AvatarTexture *avTexture;
	int moving, attacking, attackingSpecial, dying, beyondRange, inSecondForm;
	char name[32];
	char guildName[64];
	long screenHealth, screenMaxHealth, screenHealthTimer;
	unsigned short imageFlags;
	int lastStepIndex;

	D3DXCOLOR particleStreamColor;
	int particleStreamType;
	DWORD particleStreamAge;

	int chantType;
	D3DXCOLOR chantColor;
	DWORD chantAge;

	int magicAttackType, magicAttackCounter;
	D3DXCOLOR magicAttackColor;

	PumaMesh *blade;
	int bladeType;
	int bladeParticleAmount[5];  // 0 means no particles
	int totalBPA;
//	int bladeIsStaff;
	D3DXCOLOR bladeParticleColor[5];
	D3DXCOLOR bladeTrailColor;
	int hasTrail;
	float bladeLength;
	D3DXVECTOR3 bladeTrailVec[4];

//	static LPDIRECT3DTEXTURE8 skins[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
	static PumaAnim *monsterAnims[3][NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
	static int refCount[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];
	static float shadowSize[NUM_OF_MONSTERS][NUM_OF_MONSTER_SUBTYPES];

	PumaAnim *customMonsterAnims[3];
	unsigned char lasta, lastr, lastg, lastb;
	float lastSize;

	BBOPet pet[2];
};

#endif
