#ifndef BBO_SMOB_H
#define BBO_SMOB_H

#include ".\helper\linklist.h"
//#include "puma.h"
#include "flockPoints.h"
#include "BBO.h"
#include "BBOServer.h"
#include "sharedSpace.h"

class BBOSMob : public DataObject
{
public:

	BBOSMob(int doid, char *doname);
	virtual ~BBOSMob();
	virtual void Tick(SharedSpace *ss);
	void MagicEffect(int type, DWORD time, float amount);

	int cellX, cellY;
	int targetCellX, targetCellY;
	DWORD moveStartTime, lastAttackTime;
	int isMoving, isDead;

	DWORD magicEffectTimer [MONSTER_EFFECT_TYPE_NUM];
	float magicEffectAmount[MONSTER_EFFECT_TYPE_NUM];

};

#endif
