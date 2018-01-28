
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Schest.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"


//******************************************************************
BBOSChest::BBOSChest(int xa, int ya) : BBOSMob(SMOB_CHEST,"CHEST")
{
	cellX = xa;
	cellY = ya;
	isOpen = FALSE;
}

//******************************************************************
BBOSChest::~BBOSChest()
{
}

//******************************************************************
void BBOSChest::Tick(SharedSpace *ss)
{
	DWORD delta;
	DWORD now = timeGetTime();

	delta = now - openTime;

	if (0 == openTime || now < openTime)
	{
		openTime = now - rand() % 300;
	}

	// spawn
	if (delta > 1000 * 60 * 60 * 3) // 3 hours	
	{
		openTime = now;
		if (isOpen)
		{
			isOpen = FALSE;
			MessChestInfo chestInfo;
			chestInfo.mobID = (unsigned long) this;
			chestInfo.type = 0;
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(chestInfo),(void *)&chestInfo);
		}
	}

}






/* end of file */



