
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Stower.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"


//******************************************************************
BBOSTower::BBOSTower(int xa, int ya) : BBOSMob(SMOB_TOWER,"TOWER")
{
	cellX = xa;
	cellY = ya;
	isGuildTower = FALSE;
}

//******************************************************************
BBOSTower::~BBOSTower()
{
}

//******************************************************************
void BBOSTower::Tick(SharedSpace *ss)
{
}






/* end of file */



