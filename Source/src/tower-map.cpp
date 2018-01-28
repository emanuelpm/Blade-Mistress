
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "BBO-Savatar.h"
#include "BBO-Stower.h"
#include ".\helper\UniqueNames.h"
#include ".\helper\GeneralUtils.h"
#include ".\helper\crc.h"
#include "version.h"

#include "Tower-Map.h"

char guildStyleNames[GUILDSTYLE_MAX][32] =
{
	{"NONE"},
	{"CAUCUS"},
	{"COUNCIL"},
	{"VESTRY"},
	{"MONARCHY"},
	{"TYRANNY"}
};

char guildBillNames[GUILDBILL_MAX][32] =
{
	{"INACTIVE"},
	{"PROMOTE"},
	{"KICK OUT"},
	{"CHANGE STYLE"},
	{"CHANGE NAME"},
	{"FIGHTER SPEC."},
	{"MAGE SPEC."},
	{"CRAFTER SPEC."},
	{"DEMOTE"}
};

char guildBillDesc[GUILDBILL_MAX][256] =
{
	{"This bill slot is empty, waiting for someone to use the /startvote command to fill it."},
	{"This bill is to decide whether to promote the subject by one level."},
	{"This bill is to decide whether to kick the subject out of the guild."},
	{"This bill is to decide whether to change the style of the guild."},
	{"This bill is to decide whether to change the name of the guild."},
	{"This bill is to decide whether to add one level of Fighter specialization to the guild."},
	{"This bill is to decide whether to add one level of Mage specialization to the guild."},
	{"This bill is to decide whether to add one level of Crafter specialization to the guild."},
	{"This bill is to decide whether to DEMOTE the subject by one level."}
};

/*
TYPE_KEYWORDs are:        Caucus     Council      Vestry   Monarchy      Tyranny
    promote                   50%      50%         80%      100%           100% (once)
    kickout                   50       60          75       100            1
    specialise for fighters   50       60	         75       75             1
    specialise for mages
    specialise for crafters
*/
int guildVoteThreshold[GUILDSTYLE_MAX-1][GUILDBILL_MAX-1] =
{
  /*CAUCUS"}*/	{50 ,50 ,50,50,50,50,50,50 },
  /*COUNCIL"*/	{50 ,60 ,60,60,60,60,60,60 },
  /*VESTRY"}*/	{80 ,75 ,75,75,75,75,75,75 },
  /*MONARCHY*/	{100,100,75,75,75,75,75,100},
  /*TYRANNY"*/	{100,1  ,1 ,1 ,1 ,1 ,1 ,100}
};

// ignored if EVERYONE'S a commoner
int commonVotesAllowed[GUILDSTYLE_MAX-1] = {4,2,1,1,0};

char guildLevelNames[GUILDSTYLE_MAX-1][3][32] =
{
	{ /*CAUCUS"}*/
		{"Elder"},
		{"Prefect"},
		{"Secretary"}
	},
	{ /*COUNCIL"}*/
		{"Whip"},
		{"Chancellor"},
		{"Chairwoman"}
	},
	{ /*VESTRY"}*/
		{"Acolyte"},
		{"Abbess"},
		{"Luminance"}
	},
	{ /*MONARCHY"}*/
		{"Royal"},
		{"Duchess"},
		{"Queen"}
	},
	{ /*TRYANNY"}*/
		{"Tyrant"},
		{"M"},
		{"L"}
	},
};



//***************************************************************************************
//***************************************************************************************
MemberRecord::MemberRecord(int doid, char *doname)	 : DataObject(doid,doname)
{
	value1 = value2 = 0;
}

//***************************************************************************************
MemberRecord::~MemberRecord()
{
}

//******************************************************************
//******************************************************************
TowerMap::TowerMap(int doid, char *doname, NetWorldRadio * ls) : SharedSpace(doid,doname, ls)
{

	leftWall = topWall = NULL;
	pathMap = NULL;
	enterX = enterY = -1;  // no visible tower in the world

	itemBox = new Inventory(MESS_INVENTORY_TOWER, this);

	members   = new DoublyLinkedList();
	guildStyle = GUILDSTYLE_NONE;
   specLevel[0] = specLevel[1] = specLevel[2] = 0;

	for (int i = 0; i < 4; ++i)
	{
		bills[i].type = GUILDBILL_INACTIVE;

		bills[i].voteState = bills[i].sponsorLevel = 0;

		sprintf(bills[i].sponsor,"NONE");
		sprintf(bills[i].subject,"NONE");
	}
}

//******************************************************************
TowerMap::~TowerMap()
{

	delete itemBox;
	delete members;

	if (leftWall)
		delete[] leftWall;
	if (topWall)
		delete[] topWall;
	if (pathMap)
		delete[] pathMap;

}

//******************************************************************
void TowerMap::InitNew(int w, int h)
{

	SharedSpace::InitNew(w, h, 0,0);

	sizeX = width  = w;
	sizeY = height = h;

	floorIndex = outerWallIndex = 0;

	leftWall = new unsigned char[width * height];
	topWall  = new unsigned char[width * height];
	pathMap  = new int[width * height];

//	srand(0);

	for (int i = 0; i < w * h; ++i)
	{
		if (rand() % 6)
			leftWall[i] = 0;
		else
			leftWall[i] = 1;
	}

	for (int i = 0; i < w * h; ++i)
	{
		if (rand() % 6)
			topWall[i] = 0;
		else
			topWall[i] = 1;
	}

	groundInventory = new Inventory[w * h];

	if (!SetPathMap())
	{
		while (!ForceDungeonContiguous())
			SetPathMap();
	}

	guildStyle = GUILDSTYLE_NONE;
   specLevel[0] = specLevel[1] = specLevel[2] = 0;

}

//******************************************************************
void TowerMap::Save(FILE *fp)
{
	// save general info
	fprintf(fp,"%s\n",WhoAmI());
	fprintf(fp,"%d %d %d %d\n", width, height, enterX, enterY);

	fprintf(fp,"%d %d %d %d\n", guildStyle, specLevel[0], specLevel[1], specLevel[2]);

	// save members
	MemberRecord *mr = (MemberRecord *) members->First();
	while (mr)
	{
		fprintf(fp,"%s\n", mr->WhoAmI());
		fprintf(fp,"%d %d\n", mr->value1, mr->value2);

		mr = (MemberRecord *) members->Next();
	}
	fprintf(fp,"ENDMEMBERLIST\n");

	fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
			lastChangedTime.value.wYear,
			lastChangedTime.value.wMonth,
			lastChangedTime.value.wDay,
			lastChangedTime.value.wDayOfWeek,
			lastChangedTime.value.wHour,
			lastChangedTime.value.wMinute,
			lastChangedTime.value.wSecond,
			lastChangedTime.value.wMilliseconds);

	// save layout
	fprintf(fp,"%d %d\n", (int) floorIndex, (int) outerWallIndex);

	int lineBreak = 0;
	for (int i = 0; i < width * height; ++i)
	{
		fprintf(fp,"%d %d ", (int) leftWall[i], (int) topWall[i]);
		if (++lineBreak > 20 || i == width * height - 1)
		{
			fprintf(fp,"\n");
			lineBreak = 0;
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		fprintf(fp,"%d %d %d\n", bills[i].voteState, bills[i].sponsorLevel, bills[i].type);

		if (0 == bills[i].sponsor[0])
			sprintf(bills[i].sponsor,"NONE");
		if (0 == bills[i].subject[0])
			sprintf(bills[i].subject,"NONE");
		fprintf(fp,"%s\n%s\n", bills[i].sponsor, bills[i].subject);

		fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
			bills[i].expTime.value.wYear,
			bills[i].expTime.value.wMonth,
			bills[i].expTime.value.wDay,
			bills[i].expTime.value.wDayOfWeek,
			bills[i].expTime.value.wHour,
			bills[i].expTime.value.wMinute,
			bills[i].expTime.value.wSecond,
			bills[i].expTime.value.wMilliseconds);

		// save votes
		mr = (MemberRecord *) bills[i].recordedVotes.First();
		while (mr)
		{
			fprintf(fp,"%d %s\n", mr->WhatAmI(), mr->WhoAmI());

			mr = (MemberRecord *) bills[i].recordedVotes.Next();
		}
		fprintf(fp,"0 ENDVOTELIST\n");

	}

	itemBox->InventorySave(fp);
}

//******************************************************************
void TowerMap::Load(FILE *fp, float version)
{
	if (leftWall)
		delete[] leftWall;
	if (topWall)
		delete[] topWall;

	// load general info
	LoadLineToString(fp, do_name);

	int tempInt, temp1, temp2; //, temp5;

	fscanf(fp,"%d %d %d %d\n", &width, &height, &enterX, &enterY);

	if (version >= 1.83f)
	{
		if (version < 2.21f)
		{
			int sType, sLevel;
			fscanf(fp,"%d %d %d\n", &guildStyle, &sType, &sLevel);
			if (sType > 0)
				specLevel[sType-1] = sLevel;
		}
		else
			fscanf(fp,"%d %d %d %d\n", &guildStyle, &specLevel[0], &specLevel[1], &specLevel[2]);

	}
	char tempText[1024];

	SharedSpace::InitNew(width, height, 0,0);

	// load members
	LoadLineToString(fp, tempText);
	while (!IsSame("ENDMEMBERLIST",tempText))
	{
		MemberRecord *mr = new MemberRecord(0,tempText);
		members->Append(mr);
		fscanf(fp,"%d %d\n", &mr->value1, &mr->value2);

		LoadLineToString(fp, tempText);
	}

//	int tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wYear         = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wMonth        = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wDay          = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wDayOfWeek    = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wHour         = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wMinute       = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wSecond       = tempInt;
	fscanf(fp,"%d", &tempInt);
	lastChangedTime.value.wMilliseconds = tempInt;

	leftWall = new unsigned char[width * height];
	topWall  = new unsigned char[width * height];
	pathMap  = new int[width * height];

	// load layout
	fscanf(fp,"%d %d", &temp1, &temp2);
	floorIndex = temp1;
	outerWallIndex = temp2;

	for (int i = 0; i < width * height; ++i)
	{
		fscanf(fp,"%d %d", &temp1, &temp2);
		leftWall[i] = temp1;
		topWall[i] = temp2;
	}

	groundInventory = new Inventory[width * height];

	// clear the line
	LoadLineToString(fp, tempText);

	if (version >= 1.83f)
	{
		for (int i = 0; i < 4; ++i)
		{
			fscanf(fp,"%d %d %d\n", &bills[i].voteState, &bills[i].sponsorLevel, &bills[i].type);

			LoadLineToString(fp, bills[i].sponsor);
			LoadLineToString(fp, bills[i].subject);

			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wYear         = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wMonth        = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wDay          = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wDayOfWeek    = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wHour         = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wMinute       = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wSecond       = tempInt;
			fscanf(fp,"%d", &tempInt);
			bills[i].expTime.value.wMilliseconds = tempInt;

			// load votes
			fscanf(fp,"%d\n", &tempInt);
			LoadLineToString(fp, tempText);
			while (!IsSame("ENDVOTELIST",tempText))
			{
				MemberRecord *mr = new MemberRecord(tempInt,tempText);
				bills[i].recordedVotes.Append(mr);

				fscanf(fp,"%d\n", &tempInt);
				LoadLineToString(fp, tempText);
			}

		}

		itemBox->InventoryLoad(fp, version);
	}
}

//******************************************************************
int TowerMap::CanMove(int srcX, int srcY, int dstX, int dstY)
{
	// can't move out of bounds
	if (dstX < 0 || dstX >= width)
		return FALSE;
	if (dstY < 0 || dstY >= height)
		return FALSE;
/*
	// can't move diagonally
	if (dstX != srcX && dstY != srcY)
		return FALSE;

	// can't move more than one square
	int dist = abs(dstX - srcX) + abs(dstY - srcY);
	if (dist > 1)
		return FALSE;
*/
	// can't move left if there's a wall in the way
	if (dstX < srcX && leftWall[srcY * width + srcX])
		return FALSE;

	// can't move right if there's a wall in the way
	if (dstX > srcX && leftWall[dstY * width + dstX])
		return FALSE;

	// can't move up if there's a wall in the way
	if (dstY < srcY && topWall[srcY * width + srcX])
		return FALSE;

	// can't move down if there's a wall in the way
	if (dstY > srcY && topWall[dstY * width + dstX])
		return FALSE;

	return TRUE;
}


//*******************************************************************************
Inventory *TowerMap::GetGroundInventory(int x, int y)
{
	Inventory *inv = &(groundInventory[width * y + x]);

	return inv;
}

//*******************************************************************************
int TowerMap::NewPathMapValue(int curValue, int neighborValue)
{
	if (neighborValue != -1)
	{
		if (-1 == curValue || curValue > neighborValue + 1)
		{
			curValue = neighborValue + 1;
		}
	}

	return curValue;
}

//*******************************************************************************
int TowerMap::SetPathMap(void)
{
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			pathMap[width * y + x] = -1; // unconnected state
		}
	}

	pathMap[0] = 0; // start in door square

	int done = FALSE;
	while (!done)
	{
		done = TRUE;
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (-1 == pathMap[width * y + x])  // if unconnected
				{
					// see if we can connect to a connected square
/*
					if (CanMove(x,y, x, y-1))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * y + x-1] + 1)
						{
							pathMap[width * y + x] = pathMap[width * y + x-1] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x, y+1))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * y + x+1] + 1)
						{
							pathMap[width * y + x] = pathMap[width * y + x+1] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x-1, y))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * (y-1) + x] + 1)
						{
							pathMap[width * y + x] = pathMap[width * (y-1) + x] + 1;
							done = FALSE;
						}
					}
					if (CanMove(x,y, x+1, y))
					{
						if (-1 == pathMap[width * y + x] || 
							 pathMap[width * y + x] > pathMap[width * (y+1) + x] + 1)
						{
							pathMap[width * y + x] = pathMap[width * (y+1) + x] + 1;
							done = FALSE;
						}
					}
*/
					if (CanMove(x,y, x-1, y))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * y + x-1]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x+1, y))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * y + x+1]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x, y-1))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * (y-1) + x]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
					if (CanMove(x,y, x, y+1))
					{
						pathMap[width * y + x] = NewPathMapValue(pathMap[width * y + x], 
							pathMap[width * (y+1) + x]);
						if (-1 != pathMap[width * y + x])
							done = FALSE;
					}
				}
			}
		}
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (-1 == pathMap[width * y + x]) // unconnected state
				return 0;
		}
	}

	return 1;
}

//*******************************************************************************
int TowerMap::ChangeWall(int isLeft, int x, int y)
{
	if (x >= 0 && 
		 x < width &&
	    y >= 0 && 
		 y < height)
	{

		unsigned char *theWall;
		if (isLeft)
			theWall = &leftWall[y * width + x];
		else
			theWall = &topWall[y * width + x];

		*theWall += 1;
		if (*theWall >= NUM_OF_TOWER_WALL_TYPES)
			*theWall = 0;
		else if (!SetPathMap())
		{
			*theWall = 0;
			return -1;
		}

		return (*theWall) + 1;
	}

	return -2;
}
  

//*******************************************************************************
int TowerMap::ForceDungeonContiguous(void)
{

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			if (-1 == pathMap[width * y + x]) // unconnected state
			{
				leftWall[y * width + x] = 0;
				topWall [y * width + x] = 0;
				return 0;
			}
		}
	}

	return 1;
}

//*******************************************************************************
void TowerMap::DoMonsterDrop(BBOSMonster *monster)
{
}

//*******************************************************************************
void TowerMap::DoMonsterDropSpecial(BBOSMonster *monster, int type)
{
}

//******************************************************************
int TowerMap::IsMember(char *avatarName)
{
	MemberRecord *mr = (MemberRecord *) members->First();
	while (mr)
	{
		if (IsCompletelySame(avatarName, mr->WhoAmI()))
			return TRUE;

		mr = (MemberRecord *) members->Next();
	}

	return FALSE;
}


//*******************************************************************************
void TowerMap::ProcessVotes(void)
{

	int messLen = 0;
	char tempText[1024];
	char logText[1024];
//	char fullName[300];
//	char fullName2[300];
	int startingLine = TRUE;

	MessPlayerChatLine chatMess;
	MessInfoText       infoText;

	tempText[0] = NWMESS_PLAYER_CHAT_LINE;
	tempText[1] = TEXT_COLOR_DATA;

	for (int i = 0; i < 4; ++i)
	{

		if (GUILDBILL_INACTIVE != bills[i].type)
		{
			LongTime now;
			long diff = now.MinutesDifference(&(bills[i].expTime));
			if (diff <= 0 && VOTESTATE_VOTING == bills[i].voteState)
			{
				int yesVotes, allVotes;
				yesVotes = allVotes = 0;

				MemberRecord *mr = (MemberRecord *) bills[i].recordedVotes.First();
				while(mr)
				{
					++allVotes;
					if (mr->WhatAmI())
						++yesVotes;
					mr = (MemberRecord *) bills[i].recordedVotes.Next();
				}

				MemberRecord *mr3 = NULL;
				MemberRecord *mr2 = (MemberRecord *) members->First();
				while(mr2)
				{
					if (IsCompletelySame(mr2->WhoAmI(), bills[i].subject))
						mr3 = mr2;
					mr2 = (MemberRecord *) members->Next();
				}

				int percent = 0;
				if (allVotes > 0)
					percent = yesVotes * 100 / allVotes;

				int newSpecVal = 0, newSpecType = 0;

				BBOSAvatar *subjAvatar = NULL;
				SharedSpace *sp2 = NULL;



				LongTime lt;
				lt.value.wHour += 19;
				if (lt.value.wHour < 24)
				{
//					lt.value.wHour += 24;
					lt.value.wDay -= 1;
				}
				else
					lt.value.wHour -= 24;
				sprintf(logText,"%d/%02d, %d:%02d, ", (int)lt.value.wMonth, (int)lt.value.wDay, 
						  (int)lt.value.wHour, (int)lt.value.wMinute);
				LogOutput("votelog.txt", logText);

				sprintf(logText," vote on %d:%s, by %s, %d, about %s, ", i,
					     guildBillNames[bills[i].type], bills[i].sponsor, bills[i].sponsorLevel, 
						  bills[i].subject);
				LogOutput("votelog.txt", logText);

				sprintf(logText,"all votes, %d, yes votes, %d, percent, %d, must be at least, %d, ",
					     allVotes, yesVotes, percent, guildVoteThreshold[guildStyle-1][bills[i].type-1]);
				LogOutput("votelog.txt", logText);

				
				// Make guild name changes unique
				if( GUILDBILL_CHANGESTYLE == bills[i].type && !UN_IsNameUnique(&(bills[i].subject[0])) ) {
					sprintf(logText,"FAILED, ");
					LogOutput("votelog.txt", logText);

					bills[i].voteState = VOTESTATE_FAILED;
					sprintf(&(tempText[2]),"*** Bill %d, %s regarding %s, has failed (name taken)!", i, 
						guildBillNames[bills[i].type], bills[i].subject);

					SharedSpace *sp = (SharedSpace *) bboServer->spaceList->First();
					while (sp)
					{
						bboServer->SendToEveryGuildMate("NILLNILL",
								sp, this, strlen(tempText) + 1,(void *)&tempText);
						sp = (SharedSpace *) bboServer->spaceList->Next();
					}
				}
				else if ((mr3 || (GUILDBILL_CHANGESTYLE == bills[i].type ||
                   		 GUILDBILL_CHANGENAME  == bills[i].type)
					 ) && percent >= guildVoteThreshold[guildStyle-1][bills[i].type-1])
				{
					sprintf(logText,"PASSED, ");
					LogOutput("votelog.txt", logText);

					bills[i].voteState = VOTESTATE_PASSED;
					sprintf(&(tempText[2]),"*** Bill %d, %s regarding %s, has passed!", i, 
						guildBillNames[bills[i].type], bills[i].subject);

					SharedSpace *sp = (SharedSpace *) bboServer->spaceList->First();
					while (sp)
					{
						bboServer->SendToEveryGuildMate("NILLNILL",
								sp, this, strlen(tempText) + 1,(void *)&tempText);
						sp = (SharedSpace *) bboServer->spaceList->Next();
					}

					int index;

					switch(bills[i].type)
					{
					case GUILDBILL_PROMOTE:
						mr3->value1 += 1;
						if (mr3->value1 > 3)
							mr3->value1 = 3;
						if (GUILDSTYLE_TYRANNY == guildStyle)
							mr3->value1 = 1;

						sprintf(logText,"Promoted to %d, ", mr3->value1);
						LogOutput("votelog.txt", logText);

						break;

					case GUILDBILL_DEMOTE:
						mr3->value1 -= 1;
						if (mr3->value1 < 0)
							mr3->value1 = 0;

						sprintf(logText,"Demoted to %d, ", mr3->value1);
						LogOutput("votelog.txt", logText);

						break;

					case GUILDBILL_KICKOUT:
						members->Remove(mr3);
						delete mr3;
						sprintf(logText,"Kicked out, ");
						LogOutput("votelog.txt", logText);

						break;

					case GUILDBILL_CHANGESTYLE:
						sprintf(logText,"Guild style change attempted, ");
						LogOutput("votelog.txt", logText);
						for (index = 1; index < GUILDSTYLE_MAX; ++index)
						{
							if (!stricmp(bills[i].subject, guildStyleNames[index]))
							{
								guildStyle = index;
								sprintf(logText,"SUCCESSFUL, ");
								LogOutput("votelog.txt", logText);
							}
						}

						break;

					case GUILDBILL_CHANGENAME:
						UN_RemoveName( do_name );
						sprintf(do_name, bills[i].subject);
						UN_AddName( do_name );

						sprintf(logText,"Guild name changed, ");
						LogOutput("votelog.txt", logText);

						break;

					case GUILDBILL_FIGHTER_SPEC:
					case GUILDBILL_MAGE_SPEC:
					case GUILDBILL_CRAFTER_SPEC:
						index = bills[i].type - GUILDBILL_FIGHTER_SPEC;
						newSpecVal = specLevel[index]+1;

						long cost = newSpecVal * newSpecVal * 10000000;

						// find the subject online
						subjAvatar = bboServer->FindAvatarByAvatarName(bills[i].subject, &sp2);
						if (subjAvatar)
						{
							// remove the requisite cash from her
							if (subjAvatar->charInfoArray[subjAvatar->curCharacterIndex].
								               inventory->money >= cost)
							{
								subjAvatar->charInfoArray[subjAvatar->curCharacterIndex].
									               inventory->money -= cost;
								subjAvatar->SaveAccount();

								specLevel[index] = newSpecVal;
							}
							else
							{
								sprintf(logText,"LACK OF FUNDS, ");
								LogOutput("votelog.txt", logText);

								sprintf(&(tempText[2]),"*** But FAILED because %s didn't have enough money!",
									bills[i].subject);
								bills[i].voteState = VOTESTATE_FAILED;

								SharedSpace *sp = (SharedSpace *) bboServer->spaceList->First();
								while (sp)
								{
									bboServer->SendToEveryGuildMate("NILLNILL",
											sp, this, strlen(tempText) + 1,(void *)&tempText);
									sp = (SharedSpace *) bboServer->spaceList->Next();
								}
							}
						}
						else
						{
							sprintf(logText,"SUBJECT OFF LINE, ");
							LogOutput("votelog.txt", logText);

							sprintf(&(tempText[2]),"*** But FAILED because %s is not online!",
								bills[i].subject);
							bills[i].voteState = VOTESTATE_FAILED;

							SharedSpace *sp = (SharedSpace *) bboServer->spaceList->First();
							while (sp)
							{
								bboServer->SendToEveryGuildMate("NILLNILL",
										sp, this, strlen(tempText) + 1,(void *)&tempText);
								sp = (SharedSpace *) bboServer->spaceList->Next();
							}
						}

						break;

					}
				}
				else
				{
					sprintf(logText,"DEFEATED, ");
					LogOutput("votelog.txt", logText);

					bills[i].voteState = VOTESTATE_FAILED;
					sprintf(&(tempText[2]),"*** Bill %d, %s regarding %s, is defeated.", i, 
						guildBillNames[bills[i].type], bills[i].subject);

					SharedSpace *sp = (SharedSpace *) bboServer->spaceList->First();
					while (sp)
					{
						bboServer->SendToEveryGuildMate("NILLNILL",
								sp, this, strlen(tempText) + 1,(void *)&tempText);
						sp = (SharedSpace *) bboServer->spaceList->Next();
					}
				}

				sprintf(logText,"END\n");
				LogOutput("votelog.txt", logText);
			}
			else if (diff < -60 * 24)
			{
				bills[i].type = GUILDBILL_INACTIVE;
			}

		}
	}		

	bboServer->spaceList->Find(this);

	DWORD nameCRC = 0;
	
	if (strlen(WhoAmI()) > 0)
		nameCRC = GetCRCForString(WhoAmI());

	// process guild tower recall scrolls
	int found = FALSE;
	InventoryObject *io = (InventoryObject *) itemBox->objects.First();
	while (io && !found)
	{
		if (INVOBJ_POTION == io->type)
		{
			InvPotion *ip = (InvPotion *) io->extra;
			if (POTION_TYPE_TOWER_RECALL == ip->type &&
				 nameCRC == ip->subType)
			{
				found = TRUE;
				io->amount = 30;
				io->value = 1;
			}
		}
		io = (InventoryObject *) itemBox->objects.Next();
	}

	if (!found)
	{
		CopyStringSafely(WhoAmI(), 256, logText, 35);
		sprintf(tempText,"%s Recall", logText);
		InventoryObject *iObject = new InventoryObject(INVOBJ_POTION,0,tempText);
		InvPotion *extra = (InvPotion *)iObject->extra;
		extra->type      = POTION_TYPE_TOWER_RECALL;
		extra->subType   = nameCRC;

		iObject->mass = 0.0f;
		iObject->value = 1;
		iObject->amount = 30;
		itemBox->AddItemSorted(iObject);
	}

}

/* end of file */



