#ifndef BBO_SMONSTER_H
#define BBO_SMONSTER_H

//#include "pumamesh.h"
#include "BBO-Smob.h"
#include ".\network\NetWorldMessages.h"
#include "inventory.h"
#include "sharedSpace.h"

enum
{
	THIEF_NOT_THIEF,
	THIEF_RUNNING,
	THIEF_WAITING,
	THIEF_MAX
};

class BBOSAvatar;
class BBOSGenerator;

class BBOSMonsterGrave : public BBOSMob
{
public:

	BBOSMonsterGrave(int t, int st, int x, int y);
	virtual ~BBOSMonsterGrave();
	void Tick(SharedSpace *ss);

	int type, subType;
	int spawnX, spawnY;
	char isWandering;
	DWORD spawnTime;

	char uniqueName[32];
	unsigned char r,g,b,a;
	float sizeCoeff;
	long health, maxHealth, damageDone, toHit, defense;
	long dropAmount;
	float magicResistance;

};

class BBOSMonster : public BBOSMob
{
public:

	BBOSMonster(int t, int st, BBOSGenerator *myGenerator, bool isVag = false);
	virtual ~BBOSMonster();
	void Tick(SharedSpace *ss);

	void ReactToAdjacentPlayer(BBOSAvatar *curAvatar, SharedSpace *ss);
	char *Name(void);
	void AnnounceMyselfCustom(SharedSpace *ss);
	void RecordDamageForLootDist(int damageDone, BBOSAvatar *curAvatar);
	void ClearDamageForLootDist (BBOSAvatar *curAvatar);
	BBOSAvatar *MostDamagingPlayer(float degradeCoeff);
	void MonsterMagicEffect(int type, float timeDelta, float amount);
	void HandlePossessedQuest(int amuletsGiven, SharedSpace *ss);
	void HandleQuestDeath(void);
	void AddPossessedLoot(int count);
	void VLordAttack(SharedSpace *ss);
	void PortalBat(SharedSpace *ss);

	int HasTarget(void) {if (curTarget || curMonsterTarget) return TRUE; return FALSE;};

	Inventory *inventory;

	char uniqueName[32];
	unsigned char r,g,b,a;
	float sizeCoeff;

	long health, maxHealth, damageDone, toHit, defense;
	long dropAmount, dropType, lastHealTime, lastWanderlustTime, lastEffectTime;
	long lastThiefTime, thiefMode;
	char isWandering, dontRespawn, isPossessed, form;
	bool isVagabond;
	DWORD creationTime, bombOuchTime;
	int bombDamage;
	float magicResistance;

	BBOSAvatar *curTarget, *bane, *bombDropper;
	BBOSMonster *curMonsterTarget;
	BBOSGenerator *myGenerator;

	BBOSAvatar *controllingAvatar;

	int type, subType;
	int spawnX, spawnY;
	int archMageMode;
	int healAmountPerSecond;

	BBOSAvatar *attackerPtrList[10];
	long attackerDamageList[10];
	long attackerLifeList[10];

	LongTime creationLongTime;

};

#endif
