
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-SMob.h"

//******************************************************************
BBOSMob::BBOSMob(int doid, char *doname) : DataObject(doid,doname)
{
	cellX = cellY = 40;
	targetCellX = targetCellY = 40;
	moveStartTime = 0;
	isMoving = FALSE;
	isDead = FALSE;

	for (int i = 0; i < MONSTER_EFFECT_TYPE_NUM; ++i)
		magicEffectAmount[i] = 0.0f;
}

//******************************************************************
BBOSMob::~BBOSMob()
{

}

//******************************************************************
void BBOSMob::Tick(SharedSpace *ss)
{

}

//******************************************************************
void BBOSMob::MagicEffect(int type, DWORD time, float amount)
{
	// Only take this if it's as powerful as the current
	if (magicEffectAmount[type] < amount)
	{
		magicEffectAmount[type] = amount;
		magicEffectTimer[type]  = time;
	}
}






/* end of file */



