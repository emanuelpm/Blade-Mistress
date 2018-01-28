
//***************************************************************
// base class for a game mode which likes to belong to a stack.
//***************************************************************
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./network/NetWorldMessages.h"
#include "robot.h"
#include "version.h"
#include "./helper/GeneralUtils.h"

DoublyLinkedList robotList;

enum
{
	ROBOT_STATE_UNCONNECTED,
	ROBOT_STATE_STARTING,
	ROBOT_STATE_PLAYING,
	ROBOT_STATE_MAKING_NEW,
	ROBOT_STATE_MAX
};

//******************************************************************
Robot::Robot(int doid, char *doname) : DataObject(doid,doname)
{
	lclient = new Client();

	state = ROBOT_STATE_UNCONNECTED;
	stateTimer = rand() % 200;

#ifdef _TEST_SERVER
	if (!lclient->connect("127.0.0.1", 9178))
#else
	if (!lclient->connect("127.0.0.1", 3678))
#endif
	{
		return;
	}

	state = ROBOT_STATE_STARTING;
	stateTimer = 0;

	MessPlayerReturning messPlayerReturning;

	sprintf(messPlayerReturning.name,"robot%d",doid);
	sprintf(messPlayerReturning.pass,"robot");
	messPlayerReturning.version = VERSION_NUMBER;
	messPlayerReturning.uniqueId = GetUniqueComputerId();
	lclient->SendMsg(sizeof(messPlayerReturning),(void *)&messPlayerReturning);

}

//******************************************************************
Robot::~Robot()
{
	delete lclient;
}

//******************************************************************
void Robot::Tick(void)
{
	++stateTimer;

	MessPlayerReturning messPlayerReturning;

	// **************** state handling
	switch(state)
	{
	case ROBOT_STATE_UNCONNECTED:
		{
			if (stateTimer > 1000)
			{
				lclient = new Client();

#ifdef _TEST_SERVER
				if (lclient->connect("127.0.0.1", 9178))
#else
				if (lclient->connect("127.0.0.1", 3678))
#endif
				{
					state = ROBOT_STATE_STARTING;
					stateTimer = 0;
					sprintf(messPlayerReturning.name,"robot%d",WhatAmI());
					sprintf(messPlayerReturning.pass,"robot");
					messPlayerReturning.version = VERSION_NUMBER;
					messPlayerReturning.uniqueId = GetUniqueComputerId();
					lclient->SendMsg(sizeof(messPlayerReturning),(void *)&messPlayerReturning);
				}
			}
		}
		break;

	case ROBOT_STATE_STARTING :
		break;

	case ROBOT_STATE_PLAYING:
		break;

	}


	// **************** message handling
	char messData[4000];
	int  dataSize;
	FILE *source = NULL;

	MessPlayerChatLine chatMess;
	MessAvatarRequestStats request;
	MessAvatarStats stats;
	MessEnterGame enterGame;
	MessPlayerNew messPlayerNew;
	MessAvatarMoveRequest mRequest;

	MessGeneralNo  *messNoPtr;
	MessGeneralYes *messYesPtr;


	DoublyLinkedList *list = NULL;

	std::vector<TagID> tempReceiptList;
	int					fromSocket = 0;

	if (lclient)
		lclient->GetNextMsg(NULL, dataSize);
	
	while (lclient && dataSize > 0)
	{
		if (dataSize > 4000)
			dataSize = 4000;
		
		lclient->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);

		switch(messData[0])
		{
		case NWMESS_GENERAL_NO:
			messNoPtr = (MessGeneralNo *) messData;
			if ((messNoPtr->subType >= 1 && messNoPtr->subType <= 3) ||
			    (messNoPtr->subType >= 5 && messNoPtr->subType <= 6))
			{
				delete lclient;
				lclient = 0;
				state = ROBOT_STATE_UNCONNECTED;
				stateTimer = rand() % 200;
			}
			if (4 == messNoPtr->subType) // new char
			{
				sprintf(messPlayerNew.name,"robot%d",WhatAmI());
				sprintf(messPlayerNew.pass,"robot");
//				messPlayerNew.version = VERSION_NUMBER;
				messPlayerNew.uniqueId = GetUniqueComputerId();
				lclient->SendMsg(sizeof(messPlayerNew),(void *)&messPlayerNew);

				stats.avatarID = 0;
				sprintf(stats.name,"robot%d",WhatAmI());
				stats.bottomR = stats.bottomG = stats.bottomB =
				stats.hairR = stats.hairG = stats.hairB =
				stats.topR = stats.topG = stats.topB = 50;
				stats.bottomIndex = stats.faceIndex = stats.topIndex = 0;
				stats.physical = stats.magical = stats.creative = 0;
				stats.imageFlags = 0;
				lclient->SendMsg(sizeof(stats), &stats);
				state = ROBOT_STATE_MAKING_NEW;
				stateTimer = 0;
			}
		   break;

		case NWMESS_GENERAL_YES:
			messYesPtr = (MessGeneralYes *) messData;

			avatarID = messYesPtr->subType;

			enterGame.characterIndex = 0;
			lclient->SendMsg(sizeof(enterGame),(void *)&enterGame);

		   break;

		case NWMESS_AVATAR_APPEAR:
		case NWMESS_AVATAR_APPEAR_SPECIAL:

			if (ROBOT_STATE_PLAYING != state)
			{
				state = ROBOT_STATE_PLAYING;
				stateTimer = rand() % 200;
			}

			if (stateTimer > 2000)
			{
				delete lclient;
				lclient = 0;
				state = ROBOT_STATE_UNCONNECTED;
				stateTimer = rand() % 200;
			}
			else
			{
				mRequest.avatarID = avatarID;
				mRequest.x = rand() % 4;
				lclient->SendMsg(sizeof(mRequest),(void *)&mRequest);
			}

		   break;
		}
		
		if (lclient)
			lclient->GetNextMsg(NULL, dataSize);
	}

}


/* end of file */



