
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Savatar.h"
#include "BBO-Sbomb.h"
#include "BBO-Sarmy.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "dungeon-map.h"
#include "MonsterData.h"


//******************************************************************
BBOSBomb::BBOSBomb(BBOSAvatar *d) : 
                        BBOSMob(SMOB_BOMB,"BOMB")
{
	dropper = d;

	power = 1;
	type = 0;
	flags = 0;
	r = g = b = 255;
	detonateTime = timeGetTime();
}

//******************************************************************
BBOSBomb::~BBOSBomb()
{
}

//******************************************************************
void BBOSBomb::Tick(SharedSpace *ss)
{
	std::vector<TagID> tempReceiptList;
	char tempText[1024];
	MessInfoText infoText;

//	DWORD delta;
	DWORD now = timeGetTime();
	if (now > detonateTime)
	{
		int lootAllowed = TRUE;

		// damage every monster in range and the avatar who dropped me
		if (dropper)
		{
			BBOSMob *curMob = (BBOSMob *) ss->avatars->Find(dropper);
			if (curMob)
			{
				BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;

				long damValue = power * 1.0f / 
						  (1 + Distance(cellX, cellY, curMob->cellX, curMob->cellY)); 

				if (damValue > 0)
				{
					if (!curAvatar->accountType)
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].health -= damValue;

					MessAvatarHealth messHealth;
					messHealth.avatarID  = curAvatar->socketIndex;
					messHealth.health    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
					messHealth.healthMax = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
					ss->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY, sizeof(messHealth), &messHealth);

					if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health <= 0)
					{
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].health = 0;
						curAvatar->isDead = TRUE;
						lootAllowed = FALSE;

						tempReceiptList.clear();
						tempReceiptList.push_back(curAvatar->socketIndex);
						sprintf(tempText,"You were blown up at %dN %dE.", 
							256-curAvatar->cellY, 256-curAvatar->cellX);

						if (SPACE_REALM == ss->WhatAmI())
						{
							sprintf(tempText,"You were blown up in the %d at %dN %dE.",
							ss->WhoAmI(),
							64-curAvatar->cellY, 64-curAvatar->cellX);
						}
						else if (SPACE_GUILD == ss->WhatAmI())
						{
							sprintf(tempText,"You were blown up in the %d tower at %dN %dE.",
							ss->WhoAmI(),
							5-curAvatar->cellY, 5-curAvatar->cellX);
						}
						else if (SPACE_LABYRINTH == ss->WhatAmI())
						{
							sprintf(tempText,"You were blown up in the Labyrinth at %dN %dE.",
							64-curAvatar->cellY, 64-curAvatar->cellX);
						}
						else if (SPACE_DUNGEON == ss->WhatAmI())
						{
							sprintf(tempText,"You were blown up in the %s (%dN %dE) at %dN %dE.",
							((DungeonMap *) ss)->name,
							256-((DungeonMap *) ss)->enterY,
							256-((DungeonMap *) ss)->enterX, 
							((DungeonMap *) ss)->height - curAvatar->cellY, 
							((DungeonMap *) ss)->width  - curAvatar->cellX);
						}
						CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
						ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
					}
				}

				curMob = (BBOSMob *) ss->mobList->GetFirst(cellX, cellY, power);
				while (curMob)
				{
					if (SMOB_MONSTER  == curMob->WhatAmI())
					{
						if (TRUE)
						{
							BBOSMonster *curMonster = (BBOSMonster *) curMob;

							float dist =1 + Distance(cellX, cellY, curMob->cellX, curMob->cellY); 

							long damValue = power * 1.0f / dist;

							if (dist < 6 && damValue > 0)
							{
								curMonster->bombDamage = damValue;
								curMonster->bombDropper = dropper;
								curMonster->bombOuchTime = timeGetTime() + 200 * dist;

								MapListState oldState = ss->mobList->GetState();

								if (lootAllowed)
									curMonster->RecordDamageForLootDist(damValue, dropper);
								else
									curMonster->ClearDamageForLootDist(dropper);

								ss->mobList->SetState(oldState);
								/*
								curMonster->health -= damValue;
								if (curMonster->health <= 0)
								{
									curMonster->isDead = TRUE;
									curMonster->bane = dropper;

									if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
									{
										BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
										army->MonsterEvent(curMonster, ARMY_EVENT_DIED);
										curMob = (BBOSMob *) ss->mobs->Find(curMonster); // reset list
									}
								}
								else
								{
									MessMonsterHealth messMH;
									messMH.mobID = (unsigned long)curMonster;
									messMH.health = curMonster->health;
									messMH.healthMax = curMonster->maxHealth;
									ss->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY, sizeof(messMH),(void *)&messMH, 2);

									if (curMonster->defense * curMonster->defense <= damValue)
									{
										curMonster->MagicEffect(MONSTER_EFFECT_STUN, 
											timeGetTime() + 40 * 1000, 1);
									}
									if (!(curMonster->curTarget))
									{
										curMonster->lastAttackTime = now;
										curMonster->curTarget = dropper;
									}

									if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
									{
										BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
										army->MonsterEvent(curMonster, ARMY_EVENT_ATTACKED);
										curMob = (BBOSMob *) ss->mobs->Find(curMonster);
									}
								}
								*/
							}
						}
					}
					curMob = (BBOSMob *) ss->mobList->GetNext();
				}
			}
		}


		// send a message about me to every avatar that is appropriate
		MessExplosion explo;
		explo.avatarID = 0;
//		if (dropper)
//			explo.avatarID = dropper->socketIndex;
		explo.r = r;
		explo.g = g;
		explo.b = b;
		explo.type = type;
		explo.flags = flags;
		explo.size = power;
		explo.x = cellX;
		explo.y = cellY;

		ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(explo), &explo, 5 + power/10);

		// kill myself
		isDead = TRUE;
	}
}


/* end of file */



