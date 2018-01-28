
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "ArmySpiders.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

//******************************************************************
//******************************************************************
ArmySpiders::ArmySpiders(SharedSpace *s, int centerX, int centerY, int sX, int sY, int numSpiders) : BBOSArmy(s, centerX, centerY)
{
	// create an army of spiders!

	spawnX = sX;
	spawnY = sY;

	// start with a general
	ArmyMember *am = new ArmyMember();
	am->quality = 10;
	am->type = 8;
	am->subType = 5; // demon spiders
	am->targetX = centerX;
	am->targetY = centerY;
	atEase.Append(am);

	// add leutenants
	for (int i = 0; i < (numSpiders-1)/3; ++i)
	{
		am = new ArmyMember();
		am->quality = 6;
		am->type = 8;
		am->subType = 4; // venomous spiders
		am->targetX = centerX + spaceOffset[i % 8][0];
		am->targetY = centerY + spaceOffset[i % 8][1];
		atEase.Append(am);
	}

	// add privates
	for (int i = 0; i < (numSpiders-1)*2/3; ++i)
	{
		am = new ArmyMember();
		am->quality = 3;
		am->type = 8;
		am->subType = 2; // hunting spiders
		am->targetX = centerX + spaceOffset[i % 8][0] * 3 + (rand() % 3) - 1;
		am->targetY = centerY + spaceOffset[i % 8][1] * 3 + (rand() % 3) - 1;
		atEase.Append(am);
	}

}

//******************************************************************
ArmySpiders::~ArmySpiders()
{

}


/* end of file */



