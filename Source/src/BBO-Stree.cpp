
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Stree.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"


//******************************************************************
BBOSTree::BBOSTree(int xa, int ya) : BBOSMob(SMOB_TREE,"TREE")
{
	cellX = xa;
	cellY = ya;
//	isOpen = FALSE;
}

//******************************************************************
BBOSTree::~BBOSTree()
{
}

//******************************************************************
void BBOSTree::Tick(SharedSpace *ss)
{
	/*
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
			MessTreeInfo treeInfo;
			treeInfo.mobID = (unsigned long) this;
			treeInfo.type = 0;
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(treeInfo),(void *)&treeInfo);
		}
	}
	*/
}






/* end of file */



