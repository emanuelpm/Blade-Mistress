
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-SwarpPoint.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"


//******************************************************************
BBOSWarpPoint::BBOSWarpPoint(int xa, int ya) : BBOSMob(SMOB_WARP_POINT,"WARPPOINT")
{
	cellX = xa;
	cellY = ya;
	allCanUse = TRUE;
//	isGuildWarpPoint = FALSE;
}

BBOSWarpPoint::BBOSWarpPoint(int xa, int ya, int type) : BBOSMob(type,"WARPPOINT")
{
	cellX = xa;
	cellY = ya;
	allCanUse = TRUE;
//	isGuildWarpPoint = FALSE;
}


//******************************************************************
BBOSWarpPoint::~BBOSWarpPoint()
{
}

//******************************************************************
void BBOSWarpPoint::Tick(SharedSpace *ss)
{
}






/* end of file */



