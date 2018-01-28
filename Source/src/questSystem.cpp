
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./helper/GeneralUtils.h"
#include "BBO-Savatar.h"
#include "BBO-Snpc.h"
#include "BBO-Smonster.h"
#include "BBO-Sgenerator.h"
#include "BBO-Stower.h"
#include "Dungeon-Map.h"
#include "monsterData.h"
#include "StaffData.h"
#include "inventory.h"
#include "TotemData.h"
#include ".\helper\crc.h"
#include "version.h"

#include "QuestSystem.h"

QuestManager *questMan = NULL;

QuestPartDesc questTargetText[QUEST_TARGET_MAX] =
{
	{"MOBTYPE",
	 "monster type",
	 "monster of a certain type"},
	{"LOCATION",
	 "place",
	 "a location you can stand on"},
	{"SPACE",
	 "area",
	 "a place (such as a dungeon, tower, or realm)"},
	{"PLAYER",
	 "player",
	 "another player"},
	{"WEAPON",
	 "weapon",
	 "a weapon (blade, mace, etc.)"},
	{"TOTEM",
	 "totem",
	 "a totem (merchants sell them) perfectly"},
	{"STAFF",
	 "staff",
	 "a staff (merchants sell them) perfectly"},
	{"NPC",
	 "npc",
	 "an npc (Great Trees, merchants, etc.)"},
	{"EGG",
	 "eggs",
	 "dragon eggs (kill dragons for them)"},
	{"DUST",
	 "dust",
	 "Glowing Dust (kill certain monsters for them)"}
};

QuestPartDesc questVerbText[QUEST_VERB_MAX] =
{
	{"KILL",
	 "kill",
	 "destroy"},
	{"GOTO",
	 "go to",
	 "travel to"},
	{"GIVEGOLD",
	 "give gold to",
	 "give a gift of gold to"},
	{"VISIT",
	 "visit",
	 "share a square and talk to"},
	{"CRAFT",
	 "craft",
	 "craft (using your skills)"},
	{"IMBUE",
	 "imbue",
	 "imbue (using your skills)"},
	{"RETRIEVE",
	 "get from",
	 "retrieve an item from"},
	{"COLLECT",
	 "collect",
	 "gather up"},
	{"GROUP",
	 "group to",
	 "gather together with other players at"},
	{"ESCAPE",
	 "escape from",
	 "leave (without dying or logging off)"}
};

char questPlayerTypeDesc[QUEST_PLAYER_TYPE_MAX][64] =
{
	{"warrior"},
	{"mage"},
	{"crafter"},
	{"generalist"},
	{"young"},
	{"poor"}
};

char questWeaponTypeDesc[QUEST_WEAPON_TYPE_MAX][64] =
{
	{"sword"},
	{"katana"},
	{"mace"},
	{"chaos sword"},
	{"claw"},
	{"bladestaff"}
};

char questRetrieveTypeDesc[MAGIC_MAX][64] =
{
	{"Tuft of fur"},
	{"Steel toenail"},
	{"Silver talon"},
	{"Shed snakeskin"},
	{"Slimy eggsac"},
	{"Scorched fleck"},
	{"Silver teardrop"},
	{"Shell fragment"},
	{"Foul Blob"}
};

//********************************************************************************
//********************************************************************************
QuestPart::QuestPart(int type, char *doname)	: DataObject(type,doname)
{

}

//********************************************************************************
QuestPart::~QuestPart()
{

}

//********************************************************************************
//********************************************************************************
Quest::Quest()	: DataObject(0, "A3GENERICQUEST")
{
	completeVal = -1; // empty;
	questSetVal = -1; // means no QuestSet affiliation
	questText = NULL;
}

//********************************************************************************
Quest::~Quest()
{
	EmptyOut();
	if (questText)
		delete[] questText;
}

//********************************************************************************
void Quest::ShortDesc(char *buffer, int goLong)
{
	if (-1 == completeVal)
	{
		sprintf(buffer, "Free quest slot.");
		return;
	}

	// find the verb and target
	QuestPart *verb = NULL;
	QuestPart *target = NULL;

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		if (QUEST_PART_VERB == qp->WhatAmI())
			verb = qp;
		if (QUEST_PART_TARGET == qp->WhatAmI())
			target = qp;
			
		qp = (QuestPart *) parts.Next();
	}

	if (!verb)
	{
		sprintf(buffer, "INVALID: This quest has no action. Expires soon.");
		LongTime now;
		long diff = now.MinutesDifference(&timeLeft);
		if (diff > 10)
		{
			timeLeft.SetToNow();
			timeLeft.AddMinutes(10); // add 10 minutes
		}
		return;
	}
	if (!target)
	{
		sprintf(buffer, "INVALID: This quest has no point. Expires soon.");
		LongTime now;
		long diff = now.MinutesDifference(&timeLeft);
		if (diff > 10)
		{
			timeLeft.SetToNow();
			timeLeft.AddMinutes(10); // add 10 minutes
		}
		return;
	}

	if (verb->type < 0 || verb->type >= QUEST_VERB_MAX)
	{
		sprintf(buffer, "INVALID: This quest action is invalid. Expires soon.");
		LongTime now;
		long diff = now.MinutesDifference(&timeLeft);
		if (diff > 10)
		{
			timeLeft.SetToNow();
			timeLeft.AddMinutes(10); // add 10 minutes
		}
		return;
	}

	if (target->type < 0 || target->type >= QUEST_TARGET_MAX)
	{
		sprintf(buffer, "INVALID: This quest target is invalid. Expires soon.");
		LongTime now;
		long diff = now.MinutesDifference(&timeLeft);
		if (diff > 10)
		{
			timeLeft.SetToNow();
			timeLeft.AddMinutes(10); // add 10 minutes
		}
		return;
	}

	if (goLong)
	{
		if (QUEST_TARGET_LOCATION == target->type && QUEST_VERB_KILL == verb->type)
			sprintf(buffer, "%s at %s.  ", 
			     questVerbText[verb->type].longText, 
				  questTargetText[target->type].longText);
		else
			sprintf(buffer, "%s %s.  ", 
			     questVerbText[verb->type].longText, 
				  questTargetText[target->type].longText);

		if (QUEST_TARGET_LOCATION == target->type)
		{
			if (target->mapType != SPACE_GROUND)
			{
				if (SPACE_REALM == target->mapType)
				{
					if (REALM_ID_SPIRITS == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of Spirits.  ", 64-target->y, 64-target->x);
					if (REALM_ID_DEAD == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of the Dead. ", 64-target->y, 64-target->x);
					if (REALM_ID_DRAGONS == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of Dragons.  ", 64-target->y, 64-target->x);
				}
				else if (SPACE_DUNGEON == target->mapType)
				{
					SharedSpace *sd = GetDungeonOfQuest(target);
					DungeonMap *dm = (DungeonMap *)sd;

					if (sd)
						sprintf(&buffer[strlen(buffer)], 
						           "At %dN %dE in the %s (%dN %dE).  ", 
									   dm->height - target->y, dm->width - target->x,
									   dm->name, 256-dm->enterY, 256-dm->enterX);
					else
						sprintf(&buffer[strlen(buffer)], 
						           "ERROR!  No dungeon matches CRC.  ");
				}
			}
			else
				sprintf(&buffer[strlen(buffer)], "At %dN %dE.  ", 256-target->y, 256-target->x);
		}
		if (QUEST_TARGET_MONSTER_TYPE == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, a %s.  ", 
				         monsterData[target->monsterType][target->monsterSubType].name);
		}
		if (QUEST_TARGET_PLAYER == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "They need to be a %s player.  ",
         				questPlayerTypeDesc[target->playerType]);
		}
		if (QUEST_TARGET_WEAPON == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "It neads to be a %s, and the challenge of the attempt cannot be less than half the work value.  ",
         				questWeaponTypeDesc[target->playerType]);
		}
		if (QUEST_TARGET_TOTEM == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "It needs to be a %s totem, and the challenge of the last imbue cannot be less than half the work value.  ",
         				totemTypeName[target->playerType]);
		}
		if (QUEST_TARGET_STAFF == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "It needs to be a %s staff, and the challenge of the last imbue cannot be less than half the work value.  ",
         				staffTypeName[target->playerType]);
		}
		if (QUEST_TARGET_NPC == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, The Great Tree of the %s.  ",
         				magicNameList[target->monsterType]);
		}
		if (QUEST_TARGET_EGG == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "You must give up %d %s.  ",
				        target->monsterSubType - completeVal, dragonInfo[0][target->monsterType].eggName);
			if (10000 > completeVal && questSource < MAGIC_MAX)
			{
				sprintf(&buffer[strlen(buffer)], "Sacrifice them to the Great Tree of the %s.  ", magicNameList[questSource]);
			}
		}
		if (QUEST_TARGET_DUST == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "You must give up %d %s.  ",
				        target->monsterSubType - completeVal, dustNames[target->monsterType]);
			if (10000 > completeVal && questSource < MAGIC_MAX)
			{
				sprintf(&buffer[strlen(buffer)], "Sacrifice them to the Great Tree of the %s.  ", magicNameList[questSource]);
			}
		}

		if (QUEST_VERB_GIVEGOLD == verb->type)
		{
			sprintf(&buffer[strlen(buffer)], "The Great Spirit will not be impressed with a gift smaller than %d gold.  ", target->range);
		}
		if (QUEST_VERB_GROUP == verb->type)
		{
			sprintf(&buffer[strlen(buffer)], "There, three players (including you) must all simultaniously say the Word of Power, Atronach, to complete the prayer successfully.  ");
		}
	}
	else
	{
		if (QUEST_TARGET_LOCATION == target->type && QUEST_VERB_KILL == verb->type)
			sprintf(buffer, "%s at %s.  ", 
			     questVerbText[verb->type].shortText, 
				  questTargetText[target->type].shortText);
		else
			sprintf(buffer, "%s %s.  ", 
			     questVerbText[verb->type].shortText, 
				  questTargetText[target->type].shortText);

		if (QUEST_TARGET_LOCATION == target->type)
		{
			if (target->mapType != SPACE_GROUND)
			{
				if (SPACE_REALM == target->mapType)
				{
					if (REALM_ID_SPIRITS == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of Spirits.  ", 64-target->y, 64-target->x);
					if (REALM_ID_DEAD == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of the Dead. ", 64-target->y, 64-target->x);
					if (REALM_ID_DRAGONS == target->mapSubType)
						sprintf(&buffer[strlen(buffer)], "At %dN %dE in the Realm of Dragons.  ", 64-target->y, 64-target->x);
				}
				else if (SPACE_DUNGEON == target->mapType)
				{
					SharedSpace *sd = GetDungeonOfQuest(target);
					DungeonMap *dm = (DungeonMap *)sd;

					if (sd)
						sprintf(&buffer[strlen(buffer)], 
						           "At %dN %dE in the %s (%dN %dE).  ",
									   dm->height - target->y, dm->width - target->x,
									   dm->name, 256-dm->enterY, 256-dm->enterX);
					else
						sprintf(&buffer[strlen(buffer)], 
						           "ERROR!  No dungeon matches CRC.  ");
				}
			}
			else
				sprintf(&buffer[strlen(buffer)], "At %dN %dE.  ", 256-target->y, 256-target->x);
		}
		if (QUEST_TARGET_MONSTER_TYPE == target->type)
		{
			if (target->monsterSubType < 0 || target->monsterType < 0)
			{
				sprintf(&buffer[strlen(buffer)], "INVALID MONSTER.  PLEASE REPORT."); 
				LongTime now;
				long diff = now.MinutesDifference(&timeLeft);
				if (diff > 10)
				{
					timeLeft.SetToNow();
					timeLeft.AddMinutes(10); // add 10 minutes
				}
			}
			else
				sprintf(&buffer[strlen(buffer)], "Specifically, a %s.  ", 
				         monsterData[target->monsterType][target->monsterSubType].name);
		}
		if (QUEST_TARGET_PLAYER == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, a %s player.  ",
         				questPlayerTypeDesc[target->playerType]);
		}
		if (QUEST_TARGET_WEAPON == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, a %s.  ",
         				questWeaponTypeDesc[target->playerType]);
		}
		if (QUEST_TARGET_TOTEM == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, a %s totem.  ",
         				totemTypeName[target->playerType]);
		}
		if (QUEST_TARGET_STAFF == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, a %s staff.  ",
         				staffTypeName[target->playerType]);
		}
		if (QUEST_TARGET_NPC == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, The %s Tree.  ",
         				magicNameList[target->monsterType]);
		}
		if (QUEST_TARGET_EGG == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, %d %s.  ",
				        target->monsterSubType - completeVal, dragonInfo[0][target->monsterType].eggName);
			if (10000 > completeVal && questSource < MAGIC_MAX)
			{
				sprintf(&buffer[strlen(buffer)], "Sacrifice them to the Great Tree of the %s.  ", magicNameList[questSource]);
			}
		}
		if (QUEST_TARGET_DUST == target->type)
		{
			sprintf(&buffer[strlen(buffer)], "Specifically, %d %s.  ",
				        target->monsterSubType - completeVal, dustNames[target->monsterType]);
			if (10000 > completeVal && questSource < MAGIC_MAX)
			{
				sprintf(&buffer[strlen(buffer)], "Sacrifice them to the Great Tree of the %s.  ", magicNameList[questSource]);
			}
		}

		if (QUEST_VERB_GIVEGOLD == verb->type)
		{
			sprintf(&buffer[strlen(buffer)], "At least %d gold.  ", target->range);
		}
		if (QUEST_VERB_GROUP == verb->type)
		{
			sprintf(&buffer[strlen(buffer)], "There, 2 other players must say Atronach together with you.  ");
		}

	}


	if (10000 == completeVal)
	{
		sprintf(&buffer[strlen(buffer)], "This quest is complete!  ");
		if (questSource < MAGIC_MAX)
		{
			sprintf(&buffer[strlen(buffer)], "Return to the Great Tree of the %s.  ", magicNameList[questSource]);
		}
	}
	else
	{
		LongTime now;
		long diff = now.MinutesDifference(&timeLeft);
		if (diff > 0)
		{
			if (diff > 60)
				sprintf(&buffer[strlen(buffer)], "You have %ld more hours to complete it.  ", diff/60);
			else
				sprintf(&buffer[strlen(buffer)], "You have %ld more minutes to complete it.  ", diff);
		}
		else
			sprintf(&buffer[strlen(buffer)], "It is about to expire.  ");
	}
}

//********************************************************************************
void Quest::LongDesc (char *buffer)
{
	ShortDesc(buffer, TRUE);
}

//********************************************************************************
void Quest::Load(FILE *fp, float version)
{
	if (version < 2.10f)
		fscanf(fp,"%d\n", &completeVal);
	else if (version < 2.11f)
	{
		fscanf(fp,"%d %d\n", &completeVal, &questSetVal, do_name);
		LoadLineToString(fp, do_name);		
	}
	else
	{
		fscanf(fp,"%ld %d %d\n", &do_id, &completeVal, &questSetVal, do_name);
		LoadLineToString(fp, do_name);		
	}


	if (-1 == completeVal)
		return;

	int tempInt;

	fscanf(fp,"%ld\n", &questSource);
	if (version < 2.21f && MAGIC_MAX-1 == questSource)
		questSource = MAGIC_MAX;

	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wYear         = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wMonth        = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wDay          = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wDayOfWeek    = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wHour         = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wMinute       = tempInt;
	fscanf(fp,"%d", &tempInt);
	timeLeft.value.wSecond = timeLeft.value.wMilliseconds = 0;

	char buff[128], buff2[1024];

//	fscanf(fp,"%s", buff);
	int done = FALSE;
	while (!done)
	{
		fscanf(fp,"%s", buff);
		if (IsSame(buff, "PART"))
		{
			fscanf(fp,"%d", &tempInt);

			QuestPart *qp = new QuestPart(tempInt, "QUESTPART");
			parts.Append(qp);

			fscanf(fp,"%d %d %d %d %d %d %d %d %d\n", 
				&qp->type, &qp->monsterType, &qp->monsterSubType,
				&qp->mapType, &qp->mapSubType, &qp->x, &qp->y, &qp->range,
				&qp->playerType);

			if (version >= 2.20f)
			{
				LoadLineToString(fp, buff2);
				CopyStringSafely(buff2, 1024, qp->otherName, 64);
			}
			else
				qp->otherName[0] = 0;
		}
		else if (IsSame(buff, "END"))
			done = TRUE;
	}

}

//********************************************************************************
void Quest::Save(FILE *fp)
{
	fprintf(fp,"%ld %d %d\n%s\n", do_id, completeVal, questSetVal, do_name);

	if (-1 == completeVal)
		return;

	fprintf(fp,"%ld\n", questSource);
	fprintf(fp,"%d %d %d %d %d %d\n", 
				timeLeft.value.wYear,
				timeLeft.value.wMonth,
				timeLeft.value.wDay,
				timeLeft.value.wDayOfWeek,
				timeLeft.value.wHour,
				timeLeft.value.wMinute);

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{

		fprintf(fp,"PART %d %d %d %d %d %d %d %d %d %d\n", 
			qp->WhatAmI(),
			qp->type, qp->monsterType, qp->monsterSubType,
			qp->mapType, qp->mapSubType, qp->x, qp->y, qp->range,
			qp->playerType);
		if (qp->otherName[0])
			fprintf(fp,"%s\n", qp->otherName);
		else
			fprintf(fp,"quest\n");

		qp = (QuestPart *) parts.Next();
	}
	fprintf(fp,"END\n");
}

//********************************************************************************
void Quest::CopyTo(Quest *dest)
{
	dest->EmptyOut();

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		QuestPart *qp2 = new QuestPart(qp->WhatAmI(), qp->WhoAmI());
		qp2->type				= qp->type;          
		qp2->monsterType		= qp->monsterType;    
		qp2->monsterSubType	= qp->monsterSubType;
		qp2->x					= qp->x;              
		qp2->y					= qp->y;              
		qp2->range				= qp->range;          
		qp2->mapType			= qp->mapType;        
		qp2->mapSubType		= qp->mapSubType;    
		qp2->playerType		= qp->playerType;    
		qp2->object = NULL;

		CopyStringSafely(qp->otherName, 64, qp2->otherName, 64);

		dest->parts.Append(qp2);

		qp = (QuestPart *) parts.Next();
	}

	dest->do_id = do_id;
	sprintf(dest->do_name, do_name);

}

//********************************************************************************
QuestPart *Quest::GetVerb  (void)
{
	// find the verb and target
	QuestPart *verb = NULL;

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		if (QUEST_PART_VERB == qp->WhatAmI())
			return qp;
		qp = (QuestPart *) parts.Next();
	}

	return NULL;
}

//********************************************************************************
QuestPart *Quest::GetTarget(void)
{
	// find the verb and target
	QuestPart *target = NULL;

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		if (QUEST_PART_TARGET == qp->WhatAmI())
			return qp;
		qp = (QuestPart *) parts.Next();
	}

	return NULL;
}


//********************************************************************************
int Quest::CheckValidity(void)
{
	int hasVerb   = FALSE;
	int hasTarget = FALSE;

	QuestPart *target = NULL;

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		if (QUEST_PART_VERB == qp->WhatAmI())
			hasVerb = TRUE;
		if (QUEST_PART_TARGET == qp->WhatAmI())
			hasTarget = TRUE;
		qp = (QuestPart *) parts.Next();
	}

	if (hasTarget && hasVerb)
		return TRUE;
	return FALSE;
}


//********************************************************************************
void Quest::EmptyOut(void)
{
	sprintf(do_name, "A3GENERICQUEST");
	completeVal = -1;
	questSource = 0;
	timeLeft.SetToNow();

	QuestPart *qp = (QuestPart *) parts.First();
	while(qp)
	{
		parts.Remove(qp);
		delete qp;
		qp = (QuestPart *) parts.First();
	}
}

//********************************************************************************
SharedSpace *Quest::GetDungeonOfQuest(QuestPart *qt)
{
	SharedSpace *tempss = (SharedSpace *) bboServer->spaceList->First();
	while (tempss)
	{
		if (SPACE_DUNGEON == tempss->WhatAmI() && 0 == ((DungeonMap *) tempss)->specialFlags )
		{
			DungeonMap *dm = (DungeonMap *) tempss;
			if (qt->mapSubType == GetCRCForString(dm->name))
				return tempss;
		}
		tempss = (SharedSpace *) bboServer->spaceList->Next();
	}

	return NULL;
}


//********************************************************************************
//********************************************************************************
QuestScriptSet::QuestScriptSet(int type, char *doname)	: DataObject(type,doname)
{
}

//********************************************************************************
QuestScriptSet::~QuestScriptSet()
{

}

//********************************************************************************
//********************************************************************************
QuestWitch::QuestWitch(int type, char *doname)	: DataObject(type,doname)
{
	witchID = 0;
	sprintf(statement, "Must you bother this old woman?");
	isOpen = FALSE;
}

//********************************************************************************
QuestWitch::~QuestWitch()
{

}

//********************************************************************************
//********************************************************************************
QuestManager::QuestManager(void)
{

}

//********************************************************************************
QuestManager::~QuestManager()
{

}

//********************************************************************************
void QuestManager::WitchTalk(BBOSNpc *witch, SharedSpace *sp)
{
	char tempText[1024];

	QuestWitch *qWitch = (QuestWitch *) sets.First();
	while (qWitch)
	{
		if (qWitch->witchID == (unsigned long) witch)
		{
			sprintf(&(tempText[2]),"The witch says, %s", qWitch->statement);
			tempText[0] = NWMESS_PLAYER_CHAT_LINE;
			tempText[1] = TEXT_COLOR_TELL;
			sp->SendToEveryoneNearBut(0, witch->cellX, witch->cellY,
	  					strlen(tempText) + 1,(void *)&tempText,1);
			return;
		}

		qWitch = (QuestWitch *) sets.Next();
	}

}

//********************************************************************************
void QuestManager::WitchGiveQuest(BBOSNpc *witch, BBOSAvatar *curAvatar, SharedSpace *sp)
{
	MessInfoText infoText;

	std::vector<TagID> tempReceiptList;
	tempReceiptList.clear();
	tempReceiptList.push_back(curAvatar->socketIndex);


	QuestWitch *qWitch = (QuestWitch *) sets.First();
	while (qWitch)
	{
		if (qWitch->witchID == (unsigned long) witch)
		{
			/*
			sprintf(&(tempText[2]),"The witch says, %s", qWitch->statement);
			tempText[0] = NWMESS_PLAYER_CHAT_LINE;
			tempText[1] = TEXT_COLOR_TELL;
			sp->SendToEveryoneNearBut(0, witch->cellX, witch->cellY,
	  					strlen(tempText) + 1,(void *)&tempText);
						*/
			int freeSlot = -1;
			int gotOneFromMe = FALSE;
			int gotOneFromWitch = FALSE;
			for (int i = 0; i < QUEST_SLOTS; ++i)
			{
				if (-1 == curAvatar->charInfoArray[curAvatar->curCharacterIndex].
					              quests[i].completeVal)
				{
					freeSlot = i;
					i = QUEST_SLOTS;
				}

				if (i < QUEST_SLOTS && IsSame(qWitch->WhoAmI(), 
					        curAvatar->charInfoArray[curAvatar->curCharacterIndex].
							      quests[i].WhoAmI()
							 )
					)
				{
					gotOneFromMe = TRUE;
				}
				if (i < QUEST_SLOTS && !IsSame("A3GENERICQUEST", 
					        curAvatar->charInfoArray[curAvatar->curCharacterIndex].
							      quests[i].WhoAmI()
							 )
					)
				{
					gotOneFromWitch = TRUE;
				}
			}

			if (-1 == freeSlot)
			{
				sprintf(infoText.text,"You are already burdened with enough tasks.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
			}
			else if (gotOneFromMe)
			{
				sprintf(infoText.text,"I have already given you a task.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
			}
			else if (gotOneFromWitch)
			{
				sprintf(infoText.text,"You are already burdened with a witch's task.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
			}
			else if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType || 
				      ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || 
						qWitch->isOpen)
			{
				// find the proper QuestScriptSet
				int qssIndex = 0;
				if (IsSame(qWitch->WhoAmI(), 
					        curAvatar->charInfoArray[curAvatar->curCharacterIndex].witchQuestName)
				   )
				{
					qssIndex = curAvatar->charInfoArray
					               [curAvatar->curCharacterIndex].witchQuestIndex;
				}

				QuestScriptSet *qss = (QuestScriptSet *) qWitch->questSets.Find(qssIndex);
				if (!qss)
				{
					qssIndex = 0;
					qss = (QuestScriptSet *) qWitch->questSets.Find((unsigned long)0);
				}
				if (qss)
				{
					// find the starting quest
					Quest *q = (Quest *) qss->quests.Find((unsigned long)0);
					if (q)
					{
						q->CopyTo(& curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  quests[freeSlot]);
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  quests[freeSlot].completeVal = 0;
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  quests[freeSlot].timeLeft.SetToNow();
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  quests[freeSlot].timeLeft.AddMinutes(60*24); // add one day

						curAvatar->charInfoArray[curAvatar->curCharacterIndex].
										  quests[freeSlot].questSource = (unsigned long) witch;

				      curAvatar->charInfoArray[curAvatar->curCharacterIndex].witchQuestIndex = qssIndex;
						CopyStringSafely(qWitch->WhoAmI(), 64,
				        curAvatar->charInfoArray[curAvatar->curCharacterIndex].witchQuestName, 64);
						if (!q->questText)
						{
							sprintf(infoText.text,"You have recieved a quest.");
							sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
						}
						else
							sp->lserver->SendMsg(strlen(q->questText) + 1,(void *)q->questText, 0, &tempReceiptList);
					}
					else
					{
						sprintf(infoText.text,"ERROR: no starting quest for this witch's adventure.");
						sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
					}
				}
				else
				{
					sprintf(infoText.text,"ERROR: no starting adventure for this witch.");
					sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
				}

			}
			else
			{
				sprintf(infoText.text,"The witch says, I'm not ready to give you a task yet.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
			}

			return;
		}

		qWitch = (QuestWitch *) sets.Next();
	}
}

//********************************************************************************
void QuestManager::ProcessWitchQuest(BBOSAvatar *curAvatar, SharedSpace *sp, Quest *destQ)
{
	MessInfoText infoText;

	std::vector<TagID> tempReceiptList;
	tempReceiptList.clear();
	tempReceiptList.push_back(curAvatar->socketIndex);


	QuestWitch *qWitch = (QuestWitch *) sets.First();
	while (qWitch)
	{
		if (IsSame(qWitch->WhoAmI(), destQ->WhoAmI()))
		{
			QuestScriptSet *qss = (QuestScriptSet *) 
				    qWitch->questSets.Find(
					              curAvatar->charInfoArray[
									                 curAvatar->curCharacterIndex].witchQuestIndex);
			if (!qss)
			{
				sprintf(infoText.text,"ERROR: unable to find adventure to retreive next task from.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
				sprintf(infoText.text,"ERROR: Aborting adventure.");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
				destQ->EmptyOut();
				return;
			}

			Quest *q = (Quest *) qss->quests.Find(destQ->WhatAmI()+1);
			if (q)
			{
				q->CopyTo(destQ);
				destQ->completeVal = 0;
				destQ->timeLeft.SetToNow();
				destQ->timeLeft.AddMinutes(60*24); // add one day
				destQ->questSource = qWitch->witchID;

				if (!q->questText)
				{
					sprintf(infoText.text,"Your quest continues.");
					sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
				}
				else
					sp->lserver->SendMsg(strlen(q->questText) + 1,(void *)q->questText, 0, &tempReceiptList);
			}
			else
			{
				sprintf(infoText.text,"You have completed your quest!");
				sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
				destQ->EmptyOut();
				curAvatar->charInfoArray[curAvatar->curCharacterIndex].witchQuestIndex++;
			}
		

			return;
		}

		qWitch = (QuestWitch *) sets.Next();
	}
}

//********************************************************************************
void QuestManager::Load(void)
{
	char tempText[1024];
	char tempText2[1024];
	int linePoint, argPoint;
	int questIndex;
	int adventureIndex;

	Quest *q = NULL;
	SharedSpace *gm = (SharedSpace *) bboServer->spaceList->Find(SPACE_GROUND);

	QuestScriptSet *qss = NULL;

	// remove all witches from world
	// clean out the sets list
	QuestWitch *qWitch = (QuestWitch *) sets.First();
	while (qWitch)
	{
		if (qWitch->witchID)
		{
			BBOSNpc *npc = (BBOSNpc *) gm->mobList->IsInList((BBOSMob *)(qWitch->witchID), TRUE);
			if (npc)
			{
				gm->mobList->Remove(npc);
				MessMobDisappear messMobDisappear;
				messMobDisappear.mobID = qWitch->witchID;
				messMobDisappear.x = npc->cellX;
				messMobDisappear.y = npc->cellY;
				gm->SendToEveryoneNearBut(0, npc->cellX, npc->cellY,
					sizeof(messMobDisappear),(void *)&messMobDisappear,6);
				delete npc;
			}
		}
		sets.Remove(qWitch);
		delete qWitch;

		qWitch = (QuestWitch *) sets.First();
	}

	qWitch = NULL;

	// open the questscripts.dat file
	FILE *fp = fopen("serverdata\\questscripts.dat","r");

	// if opened
	if (fp)
	{
		LogOutput("questLoadLog.txt", "\n-------------------------------------- loading questscripts.dat");

		int done = FALSE;
		while (!done)
		{
			linePoint = argPoint = 0;
			LoadLineToString(fp, tempText);

			argPoint = SkipSpaces(tempText,&linePoint);
			argPoint = NextWord(tempText,&linePoint);
			if (argPoint != linePoint)
			{
				if ( IsSame(&(tempText[argPoint]) , "ENDOFFILE"))
				{
					done = TRUE;
				}
				else if ( IsSame(&(tempText[argPoint]) , "WITCH"))
				{
					qWitch = new QuestWitch();
					sets.Append(qWitch);
					adventureIndex = 0;

					BBOSNpc *npc = new BBOSNpc(SMOB_WITCH);
					qWitch->witchID = (unsigned long) npc;

					LogOutput("questLoadLog.txt", "\nWITCH created");

					argPoint = NextWord(tempText,&linePoint);
					npc->cellY = 256-atoi(&(tempText[argPoint]));
					argPoint = NextWord(tempText,&linePoint);
					npc->cellX = 256-atoi(&(tempText[argPoint]));

					if (npc->cellX <= 0 || npc->cellX > 254)
						LogOutput("questLoadLog.txt", "\n***** ERROR: BAD X position");
					else if (npc->cellY <= 0 || npc->cellY > 254)
						LogOutput("questLoadLog.txt", "\n***** ERROR: BAD Y position");
					else
					{
						sprintf(tempText2,"\nPosition: %dN %dE", 256-npc->cellY, 256-npc->cellX);
						LogOutput("questLoadLog.txt", tempText2);
					}

					argPoint = NextWord(tempText,&linePoint);
					if (argPoint == linePoint)
						LogOutput("questLoadLog.txt", "\n***** ERROR: BAD witch name");
					else
						CopyStringSafely(&(tempText[argPoint]), 64, qWitch->do_name, 64);

					gm->mobList->Add(npc);

					MessMobAppear mobAppear;
					mobAppear.mobID = (unsigned long) npc;
					mobAppear.type = npc->WhatAmI();
					mobAppear.x = npc->cellX;
					mobAppear.y = npc->cellY;
					gm->SendToEveryoneNearBut(0, npc->cellX, npc->cellY, 
								 sizeof(mobAppear), &mobAppear);

					MessGenericEffect messGE;
					messGE.avatarID = -1;
					messGE.mobID    = (unsigned long) npc;
					messGE.x        = npc->cellX;
					messGE.y        = npc->cellY;
					messGE.r        = 0;
					messGE.g        = 255;
					messGE.b        = 0;
					messGE.type     = 0;  // type of particles
					messGE.timeLen  = 2; // in seconds
					gm->SendToEveryoneNearBut(0, npc->cellX, npc->cellY,
										 sizeof(messGE),(void *)&messGE);
				}
				else if ( IsSame(&(tempText[argPoint]) , "STATEMENT"))
				{
					argPoint = NextWord(tempText,&linePoint);
					sprintf(qWitch->statement,&(tempText[argPoint]));
				}
				else if ( IsSame(&(tempText[argPoint]) , "ADVENTURE"))
				{
					questIndex = 0;
					qss = new QuestScriptSet(adventureIndex++);
					qWitch->questSets.Append(qss);
				}
				else if ( IsSame(&(tempText[argPoint]) , "READY"))
				{
					qWitch->isOpen = TRUE;
				}
				else if ( IsSame(&(tempText[argPoint]) , "KILL"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}
					q = new Quest();
					q->questSetVal = q->do_id = questIndex;
					++questIndex;
					qss->quests.Append(q);
					sprintf(q->do_name, qWitch->do_name);

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					qp->type = QUEST_VERB_KILL;
					q->parts.Append(qp);

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);
				}
				else if ( IsSame(&(tempText[argPoint]) , "ASSAULT"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					q->parts.Append(qp);
					qp->type = QUEST_VERB_KILL;

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);

				}
				else if ( IsSame(&(tempText[argPoint]) , "GOTO"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}
					q = new Quest();
					q->questSetVal = q->do_id = questIndex;
					++questIndex;
					qss->quests.Append(q);
					sprintf(q->do_name, qWitch->do_name);

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					qp->type = QUEST_VERB_GOTO;
					q->parts.Append(qp);

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);
				}
				else if ( IsSame(&(tempText[argPoint]) , "REWARD"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}
					q = new Quest();
					q->questSource = (unsigned long) qWitch;
					q->questSetVal = q->do_id = questIndex;
					++questIndex;
					qss->quests.Append(q);
					sprintf(q->do_name, qWitch->do_name);

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					qp->type = QUEST_VERB_REWARD;
					q->parts.Append(qp);

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);
				}
				else if ( IsSame(&(tempText[argPoint]) , "VISIT"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}
					q = new Quest();
					q->questSource = (unsigned long) qWitch;
					q->questSetVal = q->do_id = questIndex;
					++questIndex;
					qss->quests.Append(q);
					sprintf(q->do_name, qWitch->do_name);

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					qp->type = QUEST_VERB_VISIT;
					q->parts.Append(qp);

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);
				}
				else if ( IsSame(&(tempText[argPoint]) , "GIVEGOLD"))
				{
					if (q)
					{
						if (!q->CheckValidity())
						{
							LogOutput("questLoadLog.txt", "\n***** ERROR: quest is missing verb AND/OR target parts");
						}
					}
					q = new Quest();
					q->questSource = (unsigned long) qWitch;
					q->questSetVal = q->do_id = questIndex;
					++questIndex;
					qss->quests.Append(q);
					sprintf(q->do_name, qWitch->do_name);

					QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
					qp->type = QUEST_VERB_GIVEGOLD;
					q->parts.Append(qp);

					argPoint = NextWord(tempText,&linePoint);
					HandleQuestLine(&(tempText[argPoint]), q);
				}
				else if ( IsSame(&(tempText[argPoint]) , "DESC"))
				{
					argPoint = NextWord(tempText,&linePoint);

					if (!q)
						LogOutput("questLoadLog.txt", "\n***** ERROR: DESC has no associated quest");
					else
					{
						q->questText = new char[strlen(&(tempText[argPoint])) + 4];
						sprintf(&(q->questText[2]), &(tempText[argPoint]));
						q->questText[0] = NWMESS_PLAYER_CHAT_LINE;
						q->questText[1] = TEXT_COLOR_TELL;
					}
				}
			}
		}

		fclose(fp);
	}
}

//********************************************************************************
void QuestManager::HandleQuestLine(char *tempText, Quest *q)
{
	int done = FALSE;
	int linePoint = 0;
	int argPoint = 0;
	QuestPart *qp;

	while (!done)
	{
		argPoint = NextWord(tempText,&linePoint);
		if (argPoint == linePoint)
			done = TRUE;
		else if ( IsSame(&(tempText[argPoint]) , "MONSTER_TYPE"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_TARGET_MONSTER_TYPE;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->monsterType = atoi(&(tempText[argPoint]));

			if (qp->monsterType < 0 || qp->monsterType >= NUM_OF_MONSTERS)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster type");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->monsterSubType = atoi(&(tempText[argPoint]));

			if (qp->monsterSubType < 0 || qp->monsterSubType >= NUM_OF_MONSTER_SUBTYPES)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster subType");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "QUEST_MONSTER"))
		{
			QuestPart *qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_TARGET_LOCATION;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->monsterType = atoi(&(tempText[argPoint]));

			if (qp->monsterType < 0 || qp->monsterType >= NUM_OF_MONSTERS)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster type");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->monsterSubType = atoi(&(tempText[argPoint]));

			if (qp->monsterSubType < 0 || qp->monsterSubType >= NUM_OF_MONSTER_SUBTYPES)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster subType");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->y = 256-atoi(&(tempText[argPoint]));

			if (qp->y < 1 || qp->y >= 255)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD location N");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->x = 256-atoi(&(tempText[argPoint]));

			if (qp->x < 1 || qp->x >= 255)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD location E");
				return;
			}

			qp->mapType = 0;
			qp->mapSubType = 0; // needs to be correct

			argPoint = NextWord(tempText,&linePoint);
			qp->range = atof(&(tempText[argPoint])) * 100;

			if (qp->range < 1 || qp->range >= 1000000)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster power coefficient");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			if (linePoint == argPoint)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD monster name");
				return;
			}

			CopyStringSafely(&(tempText[argPoint]), 100, qp->otherName, 64);

		}
		else if ( IsSame(&(tempText[argPoint]) , "LOCATION"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_TARGET_LOCATION;
			qp->mapType = 0;
			qp->mapSubType = 0; // needs to be correct
			qp->range = 0;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->y = 256-atoi(&(tempText[argPoint]));

			if (qp->y < 1 || qp->y >= 255)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD location N");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->x = 256-atoi(&(tempText[argPoint]));

			if (qp->x < 1 || qp->x >= 255)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD location E");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "GOLD"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_GOLD;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 1 || qp->x >= 25000)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD gold amount");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "STAFF"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_STAFF;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 0 || qp->x >= STAFF_QUALITY_MAX)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD staff quality");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "TOTEM"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_TOTEM;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 0 || qp->x >= TOTEM_QUALITY_MAX)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD totem quality");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "DUST"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_DUST;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 0 || qp->x >= INGR_MAX)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD dust type");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "FAVOR"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_FAVOR;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 0 || qp->x >= MAGIC_MAX)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD favor type");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "EGG"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_EGG;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));

			if (qp->x < 0 || qp->x >= DRAGON_TYPE_NUM)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD dragon egg type");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->y = atoi(&(tempText[argPoint]));

			if (qp->y < 0 || qp->y >= DRAGON_QUALITY_NUM)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD dragon egg quality");
				return;
			}
		}
		else if ( IsSame(&(tempText[argPoint]) , "INGOT"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_REWARD_TYPE_INGOT;
			
			argPoint = NextWord(tempText,&linePoint);
			qp->monsterType = atoi(&(tempText[argPoint]));
			if (qp->monsterType < 1 || qp->monsterType >= 50)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD ingot power (chitin is 10)");
				return;
			}
			argPoint = NextWord(tempText,&linePoint);
			qp->monsterSubType = atoi(&(tempText[argPoint]));
			if (qp->monsterSubType < 1)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD ingot value (chitin is 80000)");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->x = atoi(&(tempText[argPoint]));
			if (qp->x < 0 || qp->x >= 256)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD ingot color (0-255)");
				return;
			}
			argPoint = NextWord(tempText,&linePoint);
			qp->y = atoi(&(tempText[argPoint]));
			if (qp->y < 0 || qp->y >= 256)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD ingot color (0-255)");
				return;
			}
			argPoint = NextWord(tempText,&linePoint);
			qp->range = atoi(&(tempText[argPoint]));
			if (qp->range < 0 || qp->range >= 256)
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: BAD ingot color (0-255)");
				return;
			}

			argPoint = NextWord(tempText,&linePoint);
			if (argPoint == linePoint)
				LogOutput("questLoadLog.txt", "\n***** ERROR: No ingot name");
			else
				CopyStringSafely(&(tempText[argPoint]), 128, qp->do_name, 128);

			while (argPoint != linePoint)
				argPoint = NextWord(tempText,&linePoint);

		}
		else if ( IsSame(&(tempText[argPoint]) , "PLAYERTYPE"))
		{
			qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
			q->parts.Append(qp);

			qp->type = QUEST_TARGET_PLAYER;
			qp->playerType = rand() % QUEST_PLAYER_TYPE_MAX;

			argPoint = NextWord(tempText,&linePoint);

			if ( IsSame(&(tempText[argPoint]) , "FIGHTER"))
				qp->playerType = QUEST_PLAYER_TYPE_FIGHTER;
			else if ( IsSame(&(tempText[argPoint]) , "MAGE"))
				qp->playerType = QUEST_PLAYER_TYPE_MAGE;
			else if ( IsSame(&(tempText[argPoint]) , "CRAFTER"))
				qp->playerType = QUEST_PLAYER_TYPE_CRAFTER;
			else if ( IsSame(&(tempText[argPoint]) , "BALANCED"))
				qp->playerType = QUEST_PLAYER_TYPE_BALANCED;
			else if ( IsSame(&(tempText[argPoint]) , "YOUNG"))
				qp->playerType = QUEST_PLAYER_TYPE_YOUNG;
			else if ( IsSame(&(tempText[argPoint]) , "POOR"))
				qp->playerType = QUEST_PLAYER_TYPE_POOR;
			else
			{
				LogOutput("questLoadLog.txt", "\n***** ERROR: No PLAYERTYPE specified.  Will be random.");
			}

			argPoint = NextWord(tempText,&linePoint);
			qp->range = atoi(&(tempText[argPoint]));
			if (qp->range < 1)
				qp->range = 1;
		}
	}
}


/* end of file */
