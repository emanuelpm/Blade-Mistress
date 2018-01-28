
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-SautoQuest.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

//******************************************************************
//******************************************************************
BBOSAutoQuest::BBOSAutoQuest(SharedSpace *s) : BBOSGenerator(0,0)
{
	ss = s;

	do_id = 2; // this is an autoquest, not a generator

	totalMembersKilled = 0;
	totalMembersBorn = 0;

	lastTickTime = timeGetTime();

	questState = 0;
	questCounter = 0;

}

//******************************************************************
BBOSAutoQuest::~BBOSAutoQuest()
{

}

//******************************************************************
void BBOSAutoQuest::Tick(SharedSpace *unused)
{
	DWORD delta;
	DWORD now = timeGetTime();

	// process at intervals
	delta = now - lastTickTime;
	if (delta < 1000 * 10)  // 10 seconds	
		return;

	lastTickTime = now;

	++questCounter;
//	if (AUTOQUEST_READY == questState)
	{
	}

}

//******************************************************************
void BBOSAutoQuest::MonsterEvent(BBOSMonster *theMonster, int eventType, int x, int y)
{
	if (theMonster->controllingAvatar) // forget about controlled avatars.
		return;

	if (AUTO_EVENT_ATTACKED == eventType)
	{
	}
	else if (AUTO_EVENT_DIED == eventType)
	{
	}
}

/* end of file */



