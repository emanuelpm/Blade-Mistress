
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "tokenManager.h"
#include ".\helper\GeneralUtils.h"
#include "BBO-Stower.h"

TokenManager tokenMan;

//******************************************************************
//******************************************************************
TokenManager::TokenManager()
{
}

//******************************************************************
TokenManager::~TokenManager()
{
}

//******************************************************************
void TokenManager::Init(void)
{
	SharedSpace *ss = (SharedSpace *) bboServer->spaceList->Find(SPACE_GROUND);

	// initial positions
	for (int i = 0; i < MAGIC_MAX; ++i)
	{
		towerName[i][0] = 0;  // no name means out on the main map
		int done;
		do
		{
			done = TRUE;
			x[i] = (rand() % 124) + 2;
			y[i] = (rand() % 124) + 2;

			for (int j = 0; j < i; ++j)
			{
				if (x[i] == x[j] && y[i] == y[j]) // is on same square
					done = FALSE;
			}

			if (!ss->CanMove(x[i],y[i],x[i],y[i]))
				done = FALSE;

		} while(!done);
	}
}

//******************************************************************
void TokenManager::Tick(void)
{
	LongTime now;
	for (int i = 0; i < MAGIC_MAX; ++i)
	{
		if (towerName[i][0] && now.MinutesDifference(&tokenTimeLeft[i]) <= 0)
		{
			SharedSpace *ground = NULL;
			SharedSpace *tower  = NULL;
			SharedSpace *tempss;
			tempss = (SharedSpace *) bboServer->spaceList->First();
			while (tempss)
			{
				if (SPACE_GUILD == tempss->WhatAmI())
				{
					if (IsCompletelySame(towerName[i], tempss->WhoAmI()))
					{
						tower = tempss;
					}
				}
				else if (SPACE_GROUND == tempss->WhatAmI())
				{
					ground = tempss;
				}
				tempss = (SharedSpace *) bboServer->spaceList->Next();
			}

			if (tower)
			{
				// remove token from tower
				MessTokenDisappear messMD;
				messMD.mobID = (unsigned long) i;
				messMD.type = SMOB_TOKEN;
				messMD.x = x[i];
				messMD.y = y[i];
				tower->SendToEveryoneNearBut(0, messMD.x, messMD.y,
				       sizeof(messMD),(void *)&messMD);
			}

			if (ground)
			{
				// randomly place token
				int done;
				do
				{
					done = TRUE;
					x[i] = (rand() % 248) + 3;
					y[i] = (rand() % 248) + 3;

					for (int j = 0; j < MAGIC_MAX; ++j)
					{
						if ( i != j && x[i] == x[j] && y[i] == y[j]) // is on same square
							done = FALSE;
					}

					if (!ground->CanMove(x[i],y[i],x[i],y[i]))
						done = FALSE;

				} while(!done);


				// add token to map
				MessMobAppear messMA;
				messMA.mobID = (unsigned long) i;
				messMA.type = SMOB_TOKEN;
				messMA.x = x[i];
				messMA.y = y[i];
				ground->SendToEveryoneNearBut(0, messMA.x, messMA.y,
				       sizeof(messMA),(void *)&messMA);

				// announce the release to all
				MessInfoText infoText;
				char tempText[1024];
				sprintf(tempText,"** The %s token has passed from the favor", 
					     magicNameList[i]);
				CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
		  		bboServer->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, NULL);

				if (tower)
					sprintf(tempText,"** of the guild, %s, and awaits new mistresses.", 
						     tower->WhoAmI());
				else
					sprintf(tempText,"** of a defunct guild, and awaits new mistresses.");

				CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
		  		bboServer->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, NULL);

				// update the data
				towerName[i][0] = 0;
			}
		}
		else if (!towerName[i][0]) // if token is outside
		{
			// run through the mob list, to find a tower on the same spot.
			SharedSpace *ground = NULL;
			SharedSpace *tempss;
			tempss = (SharedSpace *) bboServer->spaceList->First();
			while (tempss)
			{
				if (SPACE_GROUND == tempss->WhatAmI())
				{
					ground = tempss;
				}
				tempss = (SharedSpace *) bboServer->spaceList->Next();
			}

			if (ground)
			{
				BBOSMob *curMob = NULL;
				curMob = ground->mobList->GetFirst(x[i], y[i]);
				while (curMob)
				{
//					if (SMOB_TOWER == curMob->WhatAmI() && ((BBOSTower *)curMob)->isGuildTower)
					if (SMOB_TOWER == curMob->WhatAmI())
					{
						// randomly place token
						int done;
						do
						{
							done = TRUE;
							x[i] = (rand() % 248) + 3;
							y[i] = (rand() % 248) + 3;

							for (int j = 0; j < MAGIC_MAX; ++j)
							{
								if ( i != j && x[i] == x[j] && y[i] == y[j]) // is on same square
									done = FALSE;
							}

							if (!ground->CanMove(x[i],y[i],x[i],y[i]))
								done = FALSE;

						} while(!done);
					}
					curMob = ground->mobList->GetNext();
				}
			}
		}
	}
}

//******************************************************************
void TokenManager::Save(void)
{
	FILE *fp = fopen("serverdata\\tokenData.dat","w");
	if (fp)
	{
		for (int i = 0; i < MAGIC_MAX; ++i)
		{
			fprintf(fp,"%d %d %d %d %d %d\n", 
			tokenTimeLeft[i].value.wYear,
			tokenTimeLeft[i].value.wMonth,
			tokenTimeLeft[i].value.wDay,
			tokenTimeLeft[i].value.wDayOfWeek,
			tokenTimeLeft[i].value.wHour,
			tokenTimeLeft[i].value.wMinute);

			fprintf(fp,"%d %d\n", x[i], y[i]);

			if (towerName[i][0])
				fprintf(fp,"%s\n", towerName[i]);
			else
				fprintf(fp,"OUTDOORS\n");
		}

		fclose(fp);
	}

}

//******************************************************************
void TokenManager::Load(void)
{
	int tempInt;

	FILE *fp = fopen("tokenData.dat","r");
	if (fp)
	{
		for (int i = 0; i < MAGIC_MAX; ++i)
		{
			fscanf(fp,"%d", &tempInt);
			tokenTimeLeft[i].value.wYear         = tempInt;
			fscanf(fp,"%d", &tempInt);
			tokenTimeLeft[i].value.wMonth        = tempInt;
			fscanf(fp,"%d", &tempInt);
			tokenTimeLeft[i].value.wDay          = tempInt;
			fscanf(fp,"%d", &tempInt);
			tokenTimeLeft[i].value.wDayOfWeek    = tempInt;
			fscanf(fp,"%d", &tempInt);
			tokenTimeLeft[i].value.wHour         = tempInt;
			fscanf(fp,"%d\n", &tempInt);
			tokenTimeLeft[i].value.wMinute       = tempInt;

			tokenTimeLeft[i].value.wSecond       = 0;

			tokenTimeLeft[i].value.wMilliseconds = 0;

			fscanf(fp,"%d %d\n", &x[i], &y[i]);

			LoadLineToString(fp,towerName[i]);
			if (IsSame("", towerName[i]))
			{
				towerName[i][0] = 0;
				x[i] = y[i] = 205;
			}
			else if (IsCompletelySame(towerName[i],"OUTDOORS"))
				towerName[i][0] = 0;
		}

		fclose(fp);
	}

}

//******************************************************************
int TokenManager::TokenTypeInSquare(SharedSpace *ss, int tx, int ty)
{

	for (int i = 0; i < MAGIC_MAX; ++i)
	{
		if (SPACE_GROUND == ss->WhatAmI() && !towerName[i][0])
		{
			if (tx == x[i] && ty == y[i])
				return i;
		}
		else if (IsCompletelySame(towerName[i], ss->WhoAmI()))
		{
			if (tx == x[i] && ty == y[i])
				return i;
		}
	}
	return -1; // means no token in square
}

//******************************************************************
void TokenManager::PlayerEntersSquare(char *playerName, SharedSpace *ss, int tx, int ty)
{
	for (int i = 0; i < MAGIC_MAX; ++i)
	{
		if (SPACE_GROUND == ss->WhatAmI())
		{
			if (tx == x[i] && ty == y[i] && !towerName[i][0])
			{
				// find out which guild the player belongs to
				SharedSpace *sx;
				if (bboServer->FindAvatarInGuild(playerName, &sx) && ((TowerMap *)sx)->enterX > -1)
				{
					// remove token from map
					MessTokenDisappear messMD;
					messMD.mobID = (unsigned long) i;
					messMD.type = SMOB_TOKEN;
					messMD.x = tx;
					messMD.y = ty;
					ss->SendToEveryoneNearBut(0, messMD.x, messMD.y,
				          sizeof(messMD),(void *)&messMD);


					// add token to tower
					MessMobAppear messMA;
					messMA.mobID = (unsigned long) i;
					messMA.type = SMOB_TOKEN;
					messMA.x = 4 - (i % 3);
					messMA.y = 0 + i/3;
					sx->SendToEveryoneNearBut(0, messMA.x, messMA.y,
				          sizeof(messMA),(void *)&messMA);

					// announce the capture to all
					MessInfoText infoText;
					char tempText[1024];
				  	sprintf(tempText,"** %s has captured the %s token", 
						     playerName, magicNameList[i]);
					CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
		  			bboServer->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, NULL);
				  	sprintf(tempText,"** for her guild, %s!!", 
						     sx->WhoAmI());
					CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
		  			bboServer->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, NULL);

					// update the data
					tokenTimeLeft[i].SetToNow();
					tokenTimeLeft[i].AddMinutes(60 * 30);
					x[i] = messMA.x;
					y[i] = messMA.y;
					sprintf(towerName[i],sx->WhoAmI());

					Save();
				}
			}
		}
	}
}


//******************************************************************
int TokenManager::TokenIsInHere(int index, SharedSpace *ss)
{
	if (towerName[index][0])
	{
		if (IsCompletelySame(towerName[index], ss->WhoAmI()))
		{
			return TRUE;
		}
	}
	return FALSE;
}


/* end of file */



