
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Savatar.h"
#include "BBO-SgroundEffect.h"
#include "BBO-Sarmy.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "dungeon-map.h"
#include "MonsterData.h"


//******************************************************************
BBOSGroundEffect::BBOSGroundEffect(void) : 
                        BBOSMob(SMOB_GROUND_EFFECT,"GROUNDEFFECT")
{
	type = amount = 0;
	r = g = b = 255;
}

//******************************************************************
BBOSGroundEffect::~BBOSGroundEffect()
{
}

//******************************************************************
void BBOSGroundEffect::Tick(SharedSpace *ss)
{
}


/* end of file */



