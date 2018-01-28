
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOServer.h"
#include "BBO-Smonster.h"
#include "BBO-Savatar.h"
#include "BBO-Sarmy.h"
#include "BBO-SAutoQuest.h"
#include "ArmyDragonChaplain.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "dungeon-map.h"
#include "MonsterData.h"
#include "StaffData.h"
#include ".\helper\crc.h"


//******************************************************************
BBOSMonsterGrave::BBOSMonsterGrave(int t, int st, int x, int y) : 
                        BBOSMob(SMOB_MONSTER_GRAVE,"MONSTER_GRAVE")
{
	type = t;
	subType = st;
	cellX = spawnX = x;
	cellY = spawnY = y;
//	spawnTime = timeGetTime() + 60 * (t+1) * (st+1) * 20;
	spawnTime = timeGetTime() + 1000 * 60 * (t+1) * (st+1);
	uniqueName[0] = 0;

}

//******************************************************************
BBOSMonsterGrave::~BBOSMonsterGrave()
{
}

//******************************************************************
void BBOSMonsterGrave::Tick(SharedSpace *ss)
{
//	DWORD delta;
	DWORD now = timeGetTime();
	if (now > spawnTime)
	{
		// kill me and create a monster where I was
		isDead = TRUE;
	}
}


//******************************************************************
//******************************************************************
BBOSMonster::BBOSMonster( int t, int st, BBOSGenerator *myGen, bool isVag ) : BBOSMob(SMOB_MONSTER,"MONSTER")
{
	myGenerator	= myGen;

	cellX = cellY = 39;
	targetCellX = targetCellY = 39;
	moveStartTime = lastWanderlustTime = lastHealTime = lastEffectTime = 0;
	isMoving = FALSE;
	lastThiefTime = 0;
	thiefMode = THIEF_NOT_THIEF;
	healAmountPerSecond = 1;
	dontRespawn = FALSE;
	form = 0;

	inventory = new Inventory(MESS_INVENTORY_MONSTER, this);

	magicResistance = 0;
	archMageMode = FALSE;

	curTarget = FALSE;
	curMonsterTarget = NULL;
	bane = NULL;
	controllingAvatar = NULL;
	bombDamage = 0;

	uniqueName[0] = 0; // no unique name
	r = g = b = a = 255;
	sizeCoeff = 1.0f;
//	inventory = NULL;

	health = maxHealth = monsterData[t][st].maxHealth;
	damageDone         = monsterData[t][st].damageDone;
	toHit              = monsterData[t][st].toHit;
	defense            = monsterData[t][st].defense;
	dropAmount         = monsterData[t][st].dropAmount;
	dropType           = monsterData[t][st].dropType;

	// store weather it is a vagabond or not
	isVagabond = isVag;

	type = t;
	subType = st;
	isWandering = isPossessed = FALSE;

	if (21 == type) // if thief
		thiefMode = THIEF_WAITING;

	creationTime = timeGetTime();

	for (int i = 0; i < 10; ++i)
	{
		attackerPtrList[i] = NULL;
	}

	if (myGenerator)
	{
		myGenerator->count[t][st]++;
//		if (11 == type && 1 == subType && myGenerator->count[t][st] == myGenerator->max[t][st]) // dokk's centurion
//			myGenerator->specialSpawnFlag = TRUE;
//		if (16 == type && 1 == subType && myGenerator->count[t][st] == myGenerator->max[t][st]) // dokk
//			myGenerator->specialSpawnFlag = TRUE;
	}
}

//******************************************************************
BBOSMonster::~BBOSMonster()
{
	delete inventory;

	if (myGenerator)
		myGenerator->count[type][subType]--;

//	SAFE_DELETE(inventory);
}

//******************************************************************
void BBOSMonster::Tick(SharedSpace *ss)
{
	std::vector<TagID> tempReceiptList;
	char tempText[1024];
	MessInfoText infoText;

	BBOSMob *curMob = NULL;
	BBOSAvatar *curAvatar = NULL;
	DungeonMap *dm = NULL;

	if (SPACE_DUNGEON == ss->WhatAmI())
	{
		dm = (DungeonMap *) ss;
//		assert(cellX != 0 || cellY != 0);
	}
	
	DWORD delta;
	DWORD now = timeGetTime();

	// handle bomb damage
	if (bombDamage > 0 && bombOuchTime <= now)
	{
		health -= bombDamage;

		if (health <= 0)
		{
			isDead = TRUE;
			bane = bombDropper;

			if (myGenerator && 1 == myGenerator->WhatAmI())
			{
				BBOSArmy *army = (BBOSArmy *)	myGenerator;
				army->MonsterEvent(this, ARMY_EVENT_DIED);
//				curMob = (BBOSMob *) ss->mobs->Find(this); // reset list
			}
			else if (myGenerator && 2 == myGenerator->WhatAmI())
			{
				BBOSAutoQuest *quest = (BBOSAutoQuest *)	myGenerator;
				quest->MonsterEvent(this, AUTO_EVENT_DIED);
			}
		}
		else
		{
			MessMonsterHealth messMH;
			messMH.mobID = (unsigned long)this;
			messMH.health = health;
			messMH.healthMax = maxHealth;
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messMH),(void *)&messMH, 2);

			if (defense * defense <= bombDamage)
			{
				MonsterMagicEffect(MONSTER_EFFECT_STUN, 40 * 1000, 1);
			}
			if (!HasTarget())
			{
				lastAttackTime = now;
				curTarget = bombDropper;
			}

			if (myGenerator && 1 == myGenerator->WhatAmI())
			{
				BBOSArmy *army = (BBOSArmy *)	myGenerator;
				army->MonsterEvent(this, ARMY_EVENT_ATTACKED);
//				curMob = (BBOSMob *) ss->mobs->Find(this);
			}
			else if (myGenerator && 2 == myGenerator->WhatAmI())
			{
				BBOSAutoQuest *quest = (BBOSAutoQuest *)	myGenerator;
				quest->MonsterEvent(this, AUTO_EVENT_ATTACKED);
			}
		}

		bombDamage = 0;
	}

	if (isMoving)
	{
		delta = now - moveStartTime;

		if (0 == moveStartTime || now < moveStartTime) // || now == moveStartTime)
		{
			delta = 1000 * 4 + 1;
		}

		curTarget = FALSE;
		curMonsterTarget = NULL;
		// Finish the move
		if (delta > 1000 * 4)	
		{
			isMoving = FALSE;
			cellX = targetCellX;
			cellY = targetCellY;

			ss->mobList->Move(this);

//			if (ss->map.toughestMonsterPoints[cellX][cellY] < maxHealth)
//				ss->map.toughestMonsterPoints[cellX][cellY] = maxHealth;

			if (uniqueName[0])
			{
				MessMobAppearCustom mAppear;
				mAppear.mobID = (unsigned long) this;
				mAppear.x = cellX;
				mAppear.y = cellY;
				mAppear.monsterType = type;
				mAppear.subType = subType;
				mAppear.type = SMOB_MONSTER;
				CopyStringSafely(Name(), 32, mAppear.name, 32);
				mAppear.a = a;
				mAppear.r = r;
				mAppear.g = g;
				mAppear.b = b;
				mAppear.sizeCoeff = sizeCoeff;

				if(SPACE_DUNGEON == ss->WhatAmI())
				{
					mAppear.staticMonsterFlag = FALSE;
					if (!isWandering && !isPossessed)
						mAppear.staticMonsterFlag = TRUE;
				}
				mAppear.type = WhatAmI();

				// send arrival message
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mAppear), &mAppear,6);
			}
			else
			{
				MessMobAppear mAppear;
				mAppear.mobID = (unsigned long) this;
				mAppear.x = cellX;
				mAppear.y = cellY;
				mAppear.monsterType = type;
				mAppear.subType = subType;
				if(SPACE_DUNGEON == ss->WhatAmI())
				{
					mAppear.staticMonsterFlag = FALSE;
					if (!isWandering && !isPossessed)
						mAppear.staticMonsterFlag = TRUE;
				}
				mAppear.type = WhatAmI();

				// send arrival message
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mAppear), &mAppear,6);
			}

			// make players aggro on me (as I'm arriving)!
			curMob = (BBOSMob *) ss->avatars->First();
			while (curMob)
			{
				if( curMob->cellX == cellX && 
					 curMob->cellY == cellY)
				{
					curAvatar = (BBOSAvatar *) curMob;

					// make him aggro
					if (!curAvatar->curTarget && !curAvatar->isInvisible &&
						 !controllingAvatar &&
						 !(dm && dm->CanEdit(curAvatar))
						)
					{
						curAvatar->lastAttackTime = now;
						curAvatar->curTarget = this;
					}

				}
				curMob = (BBOSMob *) ss->avatars->Next();
			}

			// and if the avatar attacks me, I'm gonna attack him right back!
			if (!curTarget && curAvatar && !curAvatar->isInvisible &&
						 !controllingAvatar && !(dm && dm->CanEdit(curAvatar)))
			{
				lastAttackTime = now - (rand() % 1000);
				curTarget = curAvatar;
			}

			if (controllingAvatar)
			{
				SharedSpace *sx;
				if (bboServer->FindAvatar(controllingAvatar, &sx))
				{
					controllingAvatar->UpdateClient(sx);
				}
			}

		}
		return;
	}

	delta = now - lastAttackTime;

	if (0 == lastAttackTime || now < lastAttackTime) // || now == moveStartTime)
	{
		delta = 1000 * 2 + 1;
	}

	if (curTarget && !(ss->avatars->Find(curTarget)))
		curTarget = NULL;

	if (curMonsterTarget)
	{
		MapListState state = ss->mobList->GetState();
		int stillThere = FALSE;
		BBOSMob *tMob = NULL;
		tMob = ss->mobList->GetFirst(cellX, cellY);
		while (tMob && !stillThere)
		{
			if (SMOB_MONSTER == tMob->WhatAmI() && curMonsterTarget == tMob)
				stillThere = TRUE;
			tMob = ss->mobList->GetNext();
		}

		ss->mobList->SetState(state);

		if (!stillThere)
			curMonsterTarget = NULL;

	}

	// ******************** attack curTarget (if it's still there)
	if (curTarget && 
		 delta > 1000 * 2 + magicEffectAmount[DRAGON_TYPE_BLUE] * 300 &&
		 magicEffectAmount[MONSTER_EFFECT_STUN] <= 0 &&
		 !(dm && dm->CanEdit(curTarget))
		)	
	{
		lastAttackTime = now - (rand() % 100);
		int didAttack = FALSE;

		if (form > 0)
			return;

		if (27 == type && 3 == (rand() % 6))
		{
			VLordAttack(ss);
			didAttack = TRUE;
			return;
		}

		curMob = (BBOSMob *) ss->avatars->First();
		while (curMob && !didAttack && !isDead)
		{
			if (curMob == curTarget && curMob->cellX == cellX && curMob->cellY == cellY) 
			{
				didAttack = TRUE;
				curAvatar = (BBOSAvatar *) curMob;

				InvSkill *skillInfo = NULL;
				int dodgeMod = 0;
				int found = 0;
				InventoryObject *io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills->objects.First();
				while( io && !found )
				{
					if (!strcmp("Dodging",io->WhoAmI()))
					{
						found = 1;
						skillInfo = (InvSkill *) io->extra;
						dodgeMod = skillInfo->skillLevel - 1;
						int addAmount = toHit - dodgeMod;
						if (addAmount < 0)
							addAmount = 0;
						if (addAmount > 10)
							addAmount = 0;
						skillInfo->skillPoints += addAmount;

//						if (abs(dodgeMod - toHit) < 3)
//							skillInfo->skillPoints += 1;
					}
					io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills->objects.Next();
				}

				dodgeMod += curAvatar->totemEffects.effect[TOTEM_QUICKNESS];

            //    1. chance = 2d20 + ToHit - player.Physical
				int cVal = toHit - dodgeMod;

				if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
					cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

				int chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

				if (cVal + 40 <= 20) // if monster can't EVER hit
				{
					if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
						chance = 30;  // Hit!
				}

            //    2. if chance > 20, hit was successful
				if (chance > 20)
				{
	            //    3. damage = damage
					int dDone = damageDone - curAvatar->totemEffects.effect[TOTEM_TOUGHNESS] / 3;
					if (dDone < 0)
						dDone = 0;

					if (MONSTER_PLACE_SPIRITS & 
						 monsterData[type][subType].placementFlags)
					{
						float fDone = (float)dDone * 
							(1.0f - (curAvatar->totemEffects.effect[TOTEM_PROT_SPIRITS] / 30.0f));
						dDone = (int) fDone;
					}

					dDone += (int) rnd(0, dDone);

					if (curAvatar->totemEffects.effect[TOTEM_LIFESTEAL] > 0)
					{
						int suck = dDone * curAvatar->totemEffects.effect[TOTEM_LIFESTEAL] / 60;
						dDone += suck;
						health += suck;
					}

					if (ACCOUNT_TYPE_ADMIN != curAvatar->accountType)
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].health -= dDone;

					MessMonsterAttack messAA;
					messAA.avatarID = curAvatar->socketIndex;
					messAA.mobID    = (long) this;
					messAA.damage   = dDone;
					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA,2);

					MessAvatarHealth messHealth;
					messHealth.avatarID  = curAvatar->socketIndex;
					messHealth.health    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
					messHealth.healthMax = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messHealth), &messHealth,2);

					if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health <= 0)
					{
						curAvatar->charInfoArray[curAvatar->curCharacterIndex].health = 0;
						curAvatar->isDead = TRUE;

						tempReceiptList.clear();
						tempReceiptList.push_back(curAvatar->socketIndex);
						if (SPACE_GROUND == ss->WhatAmI())
						{
							sprintf(tempText,"The %s defeats you at %dN %dE.", Name(),
							256-curAvatar->cellY, 256-curAvatar->cellX);
						}
						else if (SPACE_REALM == ss->WhatAmI())
						{
							sprintf(tempText,"The %s defeats you in a Mystic Realm at %dN %dE.",
							Name(),
							64-curAvatar->cellY, 64-curAvatar->cellX);
						}
						else if (SPACE_LABYRINTH == ss->WhatAmI())
						{
							sprintf(tempText,"The %s defeats you in the Labyrinth at %dN %dE.",
							Name(),
							64-curAvatar->cellY, 64-curAvatar->cellX);
						}
						else
						{
							sprintf(tempText,"The %s defeats you in the %s (%dN %dE) at %dN %dE.",
							Name(),
							((DungeonMap *) ss)->name,
							256-((DungeonMap *) ss)->enterY, 
							256-((DungeonMap *) ss)->enterX,
							((DungeonMap *) ss)->height - curAvatar->cellY, 
							((DungeonMap *) ss)->width  - curAvatar->cellX);
						}
						CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
						ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

					}
					else if (!(curAvatar->curTarget))
					{
						curAvatar->lastAttackTime = now;
						curAvatar->curTarget = this;
					}

					if (24 == type) // spidren group
					{
						cVal = toHit - dodgeMod;

						if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
							cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

						cVal -= curAvatar->totemEffects.effect[TOTEM_PROT_WEB];

						chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

						if (cVal + 40 <= 20) // if monster can't EVER hit
						{
							if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
								chance = 30;  // Hit!
						}

						//    2. if chance > 20, hit was successful
						if (chance > 20)
						{
							// spin a web on her ass!
							MessMagicAttack messMA;
							messMA.damage = 30;
							messMA.mobID = -1;
							messMA.avatarID = curAvatar->socketIndex;
							messMA.type = 1;
							ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
											sizeof(messMA), &messMA,3);

							tempReceiptList.clear();
							tempReceiptList.push_back(curAvatar->socketIndex);
							sprintf(tempText,"The %s ensnares you in its web!", Name());
							CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
							ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

							int effectVal = toHit - dodgeMod/2 - curAvatar->totemEffects.effect[TOTEM_PROT_WEB] * 2;
							if (effectVal < 2)
								effectVal = 2;

							curAvatar->MagicEffect(MONSTER_EFFECT_BIND, 
									timeGetTime() + effectVal * 1000, effectVal);
							curAvatar->MagicEffect(MONSTER_EFFECT_TYPE_BLUE, 
									timeGetTime() + effectVal * 1000, effectVal);

						}
					}
				}
				else
				{
					// whish!
					MessMonsterAttack messAA;
					messAA.avatarID = curAvatar->socketIndex;
					messAA.mobID    = (long) this;
					messAA.damage   = -1; // miss!
					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA,2);

					if (!(curAvatar->curTarget))
					{
						curAvatar->lastAttackTime = now;
						curAvatar->curTarget = this;
					}

				}
			}
			curMob = (BBOSMob *) ss->avatars->Next();
		}

		if (!didAttack)
		{
			curTarget = NULL;
		}

	}

	// ******************** attack curMonsterTarget
	if (curMonsterTarget && 
		 delta > 1000 * 2 + magicEffectAmount[DRAGON_TYPE_BLUE] * 300 &&
		 magicEffectAmount[MONSTER_EFFECT_STUN] <= 0)
	{
		lastAttackTime = now;

		int cVal = toHit - curMonsterTarget->defense;

		if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
			cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

		int chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

		if (cVal + 40 <= 20) // if monster can't EVER hit
		{
			if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
				chance = 30;  // Hit!
		}

      //    2. if chance > 20, hit was successful
		if (chance > 20)
		{
	      //    3. damage = damage
			int dDone = damageDone;
			if (dDone < 0)
				dDone = 0;

			dDone += (int) rnd(0, dDone);

			MessMonsterAttack messAA;
			messAA.avatarID = (int) curMonsterTarget;
			messAA.mobID    = (long) this;
			messAA.damage   = dDone;
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA,2);

			curMonsterTarget->health -= dDone;

			MessMonsterHealth messMH;
			messMH.mobID = (unsigned long)curMonsterTarget;
			messMH.health = curMonsterTarget->health;
			messMH.healthMax = curMonsterTarget->maxHealth;
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messMH),(void *)&messMH, 2);

			// if monster dies
			if (curMonsterTarget->health <= 0)
			{
				MessMonsterAttack messAA;
				messAA.avatarID = (int) curMonsterTarget;
				messAA.mobID    = (long) this;
				messAA.damage   = -1000;
//				messAA.health   = -1000;
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA, 2);

				curMonsterTarget->isDead = TRUE;
				curMonsterTarget->bane = NULL;

//				curMonsterTarget->HandleQuestDeath();

				if (curMonsterTarget->myGenerator && 1 == curMonsterTarget->myGenerator->WhatAmI())
				{
					MapListState oldState = ss->mobList->GetState();

					BBOSArmy *army = (BBOSArmy *)	curMonsterTarget->myGenerator;
					army->MonsterEvent(curMonsterTarget, ARMY_EVENT_DIED);

					ss->mobList->SetState(oldState);
				}
			}
			else if (!curMonsterTarget->HasTarget())
			{
				curMonsterTarget->lastAttackTime = now;
				curMonsterTarget->curMonsterTarget = this;
			}

			if (24 == type) // spidren group
			{
				cVal = toHit - curMonsterTarget->defense;

				if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
					cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

				chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

				if (cVal + 40 <= 20) // if monster can't EVER hit
				{
					if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
						chance = 30;  // Hit!
				}

				//    2. if chance > 20, hit was successful
				if (chance > 20)
				{
					// spin a web on her ass!
					MessMagicAttack messMA;
					messMA.damage = 30;
					messMA.mobID = (unsigned long)curMonsterTarget;
					messMA.avatarID = -1;
					messMA.type = 1;
					ss->SendToEveryoneNearBut(0, cellX, cellY, 
									sizeof(messMA), &messMA,3);

					int effectVal = toHit - curMonsterTarget->defense;
					if (effectVal < 2)
						effectVal = 2;

					curMonsterTarget->MagicEffect(MONSTER_EFFECT_BIND, 
							timeGetTime() + effectVal * 1000, effectVal);
					curMonsterTarget->MagicEffect(MONSTER_EFFECT_TYPE_BLUE, 
							timeGetTime() + effectVal * 1000, effectVal);

				}
			}
		}
		else
		{
			// whish!
			MessMonsterAttack messAA;
			messAA.avatarID = (int) curMonsterTarget;
			messAA.mobID    = (long) this;
			messAA.damage   = -1; // miss!
			ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA,2);

			if (!curMonsterTarget->HasTarget())
			{
				curMonsterTarget->lastAttackTime = now;
				curMonsterTarget->curMonsterTarget = this;
			}
		}
	}


	delta = now - lastHealTime;

	if (0 == lastHealTime || now < lastHealTime)
	{
		delta = 1000 * 1 + 1;
	}

	if (delta > 1000 * 1)	
	{
		lastHealTime = now;
		health += healAmountPerSecond;

		if (7 == type && 4 == subType) // Dragon Overlord
		{
			if (!strcmp(uniqueName,"Dragon Chaplain"))
			{
				//The Dragon Chapalin have a special 
				//healing power of 200 hp +(number of Dragon Followers around * 100 hp)			
				health += 200;

				ArmyChaplain *army = (ArmyChaplain *)	myGenerator;
				int totalLivingPrelates = 0;

				// for each monster atEase
				ArmyMember *curMember = (ArmyMember *) army->atEase.First();
				while (curMember)
				{
					if (!curMember->isDead && curMember->quality < 10)
						++totalLivingPrelates;
					curMember = (ArmyMember *) army->atEase.Next();
				}
				// for each focus
				for (int f = 0; f < 3; ++f)
				{
					// for each monster in the focus
					curMember = (ArmyMember *) army->focus[f].First();
					while (curMember)
					{
						if (!curMember->isDead && curMember->quality < 10)
							++totalLivingPrelates;
						curMember = (ArmyMember *) army->focus[f].Next();
					}
				}

				health += 100 * totalLivingPrelates;
			}
			else if (!strcmp(uniqueName,"Dragon ArchMage"))
			{
				health += 200;
			}
			else
				health += 600;
		}
		else if (7 == type && 3 == subType) // Dragon Archon
		{
			if (!strcmp(uniqueName,"Dragon Prelate"))
			{
				;
			}
			else if (!strcmp(uniqueName,"Dragon Acolyte"))
			{
				health += 60;
			}
			else
				health += 150;
		}
		else if (24 == type && 4 == subType) // Grey Spidren
		{
			healAmountPerSecond = 200 * sizeCoeff;
		}
		else if (27 == type) // vlords
		{
			healAmountPerSecond = 200 * (1+ subType);
		}


		if (health > maxHealth)
		{
			health = maxHealth;
			archMageMode = FALSE;
		}

//		if (ss->map.toughestMonsterPoints[cellX][cellY] < maxHealth)
//			ss->map.toughestMonsterPoints[cellX][cellY] = maxHealth;

	}
	
	// process effects
	delta = now - lastEffectTime;

	if (0 == lastEffectTime || now < lastEffectTime)
	{
		delta = 1000 * 10 + 1;
	}

	// Effect damage
	if (delta > 1000 * 10)	
	{
		lastEffectTime = now;

		if (25 == type && !uniqueName && SPACE_GROUND == ss->WhatAmI()) // if vampire
		{
			if (5 == rand() % 50)
			{
				int oneHour = 60 * 6;
				if (bboServer->dayTimeCounter < 2.0f * oneHour ||
					 bboServer->dayTimeCounter > 3.85f * oneHour)
				{
					// die with no loot for anyone!
					for (int i = 0; i < 10; ++i)
						attackerPtrList[i] = NULL; 
					isDead = TRUE;
					dropAmount = 0; // dont' drop anything!
				}
			}
		}

		if (!strncmp(uniqueName,"Revenant", 8))
		{
			LongTime ltNow;
			if (creationLongTime.MinutesDifference(&ltNow) > 60 * 24)
			{
				// die with no loot for anyone!
				for (int i = 0; i < 10; ++i)
					attackerPtrList[i] = NULL; 
				isDead = TRUE;
				dropAmount = 0; // dont' drop anything!
			}
		}

		// possessed monsters OCCASSIONALLY move
		if (!strncmp(uniqueName,"Possessed", 9) && 23 == (rand() % 250))
		{
			// what direction do I want to go?
			int tX = cellX;
			int tY = cellY;

			int randDir = rand() % 4;

			if (0 == randDir)
				--tX;
			else if (1 == randDir)
				++tX;
			else if (2 == randDir)
				--tY;
			else if (3 == randDir)
				++tY;

			int willGo = TRUE;

			// Can I go there?
			if (tX == cellX && tY == cellY)
				willGo = FALSE;
			if (!ss->CanMove(cellX, cellY, tX, tY))
				willGo = FALSE;

			if (willGo)
			{
				// okay, let's go!
				isMoving = TRUE;
				targetCellX = tX;
				targetCellY = tY;
				moveStartTime = timeGetTime();

				MessMobBeginMove bMove;
				bMove.mobID = (unsigned long) this;
				bMove.x = cellX;
				bMove.y = cellY;
				bMove.targetX = tX;
				bMove.targetY = tY;
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(bMove), &bMove);
			}
		}

		for (int i = 0; i < MONSTER_EFFECT_TYPE_NUM; ++i)
		{
			if (magicEffectAmount[i] > 0)
			{
				if (magicEffectTimer[i] < now)
					magicEffectAmount[i] = 0;
				else if (MONSTER_EFFECT_TYPE_BLACK == i)
				{
					health -= magicEffectAmount[i] / 2.0f;

					MessMonsterHealth messMH;
					messMH.mobID = (unsigned long)this;
					messMH.health = health;
					messMH.healthMax = maxHealth;
					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messMH),(void *)&messMH, 2);

					if (health <= 0)
					{
						health = 0;
						isDead = TRUE;
					}
				}

			}
		}

		// decay the attacker info here
		for (int i = 0; i < 10; ++i)
		{
			if (attackerPtrList[i])
			{
				attackerDamageList[i] = attackerDamageList[i] * 9 / 10;
				if (attackerDamageList[i] < 1)
					attackerPtrList[i] = NULL; // decayed to nothingness
			}
		}

		// for vlords, change form?
		if (27 == type)
		{
			if (health < maxHealth)
			{
				if (1 == form)
					form = 0;
				else
					form = 1;

				MessMonsterChangeForm messCF;
				messCF.mobID = (unsigned long)this;
				messCF.form = form;
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messCF),(void *)&messCF);

				if (1 == form)
				{
					// what direction do I want to go?
					int dirTry = 0;
					while (dirTry < 10)
					{
						++dirTry;
						int tX = cellX;
						int tY = cellY;

						int randDir = rand() % 4;

						if (0 == randDir)
							--tX;
						else if (1 == randDir)
							++tX;
						else if (2 == randDir)
							--tY;
						else if (3 == randDir)
							++tY;

						int willGo = TRUE;
						if (spawnX - tX > 2)
							willGo = FALSE;
						if (spawnY - tY > 2)
							willGo = FALSE;
						if (tX - spawnX > 2)
							willGo = FALSE;
						if	(tY - spawnY > 2)
							willGo = FALSE;

						// don't go too near the door
						if (tX < cellX && tX < 2)
							willGo = FALSE;
						if (tY < cellY && tY < 2)
							willGo = FALSE;

						// Can I go there?
						if (tX == cellX && tY == cellY)
							willGo = FALSE;
						if (!ss->CanMove(cellX, cellY, tX, tY))
							willGo = FALSE;

						if (willGo && magicEffectAmount[MONSTER_EFFECT_BIND] <= 0)
						{
							// okay, let's go!
							isMoving = TRUE;
							targetCellX = tX;
							targetCellY = tY;
							moveStartTime = timeGetTime();

							MessMobBeginMove bMove;
							bMove.mobID = (unsigned long) this;
							bMove.x = cellX;
							bMove.y = cellY;
							bMove.targetX = tX;
							bMove.targetY = tY;
							ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(bMove), &bMove);

							dirTry = 100;

							// try to teleport a bat
							PortalBat(ss);

						}
					}
				}

			}

		}

		// for dragons, check the ground for tradable items...
		if (7 == type)
		{
			Inventory *inv = ss->GetGroundInventory(cellX, cellY);
			int scaleCount = 0;
			int demonAmuletCount = 0;
			InventoryObject *iObject = (InventoryObject *) inv->objects.First();
			while (iObject)
			{
				if (INVOBJ_SIMPLE == iObject->type && 
					 !strncmp("Ancient Dragonscale", iObject->WhoAmI(), strlen("Ancient Dragonscale"))
					)
					scaleCount += iObject->amount;
				if (INVOBJ_SIMPLE == iObject->type && 
					 !strncmp("Demon Amulet", iObject->WhoAmI(), strlen("Demon Amulet"))
					)
					demonAmuletCount += iObject->amount;


				iObject = (InventoryObject *) inv->objects.Next();
			}
			 /*
			if (2 == subType)
			{
				scaleCount = scaleCount;
			}
				*/
			if (2 == subType && scaleCount > 1)
			{
				// okay, first, take my scales
				iObject = (InventoryObject *) inv->objects.First();
				while (iObject)
				{
					if (INVOBJ_SIMPLE == iObject->type && 
						 !strncmp("Ancient Dragonscale", iObject->WhoAmI(), strlen("Ancient Dragonscale"))
						)
					{
						inv->objects.Remove(iObject);
						delete iObject;
						iObject = (InventoryObject *) inv->objects.First();
					}
					else
						iObject = (InventoryObject *) inv->objects.Next();
				}

				// then drop the trade
				iObject = new InventoryObject(INVOBJ_INGOT,0,"Elatium Ingot");
				InvIngot *exIngot = (InvIngot *)iObject->extra;
				exIngot->damageVal = 9;
				exIngot->challenge = 9;
				exIngot->r = 0;
				exIngot->g = 50;
				exIngot->b = 128;

				iObject->mass = 1.0f;
				iObject->value = 75000;
				iObject->amount = scaleCount/2;
				if (iObject->amount < 1)
					iObject->amount = 1;

				inv->objects.Append(iObject);

				// and announce it
			  	sprintf(&(tempText[2]),"The dragon agrees to a trade.");
				tempText[0] = NWMESS_PLAYER_CHAT_LINE;
				tempText[1] = TEXT_COLOR_EMOTE;
				ss->SendToEveryoneNearBut(0, cellX, cellY, 
					       strlen(tempText) + 1,(void *)&tempText,1);
			}

			if (2 == subType && demonAmuletCount > 0)
			{
				// okay, first, take my amulets
				int objectsStillThere = FALSE;
				iObject = (InventoryObject *) inv->objects.First();
				while (iObject)
				{
					if (INVOBJ_SIMPLE == iObject->type && 
						 !strncmp("Demon Amulet", iObject->WhoAmI(), strlen("Demon Amulet"))
						)
					{
						inv->objects.Remove(iObject);
						delete iObject;
						iObject = (InventoryObject *) inv->objects.First();
					}
					else
					{
						objectsStillThere = TRUE;
						iObject = (InventoryObject *) inv->objects.Next();
					}
				}

				// then respond
				HandlePossessedQuest(demonAmuletCount, ss);

				if (!objectsStillThere)
				{
					MessMobDisappear messMobDisappear;
					messMobDisappear.mobID = (unsigned long) inv;
					messMobDisappear.x = cellX;
					messMobDisappear.y = cellY;
					ss->SendToEveryoneNearBut(0, cellX, cellY,
						sizeof(messMobDisappear),(void *)&messMobDisappear);
				}
			}
		}

		if (7 == type && 4 == subType) // Dragon Overlord
		{
			if (!strcmp(uniqueName,"Dragon ArchMage"))
			{
				if (health < 15000 && FALSE == archMageMode)
				{
					archMageMode = TRUE;
					// create Dragon Illusions
					for (int spawnIndex = 0; spawnIndex < 4; ++spawnIndex)
					{
						BBOSMonster *monster = new BBOSMonster(7,5, NULL);  // make Dragon Illusion
						int mx = cellX;
						int my = cellY;
//						int mx = cellX + spaceOffset[spawnIndex][0];
//						int my = cellY + spaceOffset[spawnIndex][1];
						monster->cellX = mx;
						monster->cellY = my;
						monster->targetCellX = mx;
						monster->targetCellY = my;
						monster->spawnX = mx;
						monster->spawnY = my;
						ss->mobList->Add(monster);

						MessMobAppear mobAppear;
						mobAppear.mobID = (unsigned long) monster;
						mobAppear.type = monster->WhatAmI();
						mobAppear.monsterType = monster->type;
						mobAppear.subType = monster->subType;
						mobAppear.staticMonsterFlag = FALSE;

						mobAppear.x = monster->cellX;
						mobAppear.y = monster->cellY;
						ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
									 sizeof(mobAppear), &mobAppear);
					}
				}
			}
		}
	}
	
	delta = now - lastWanderlustTime;

	if (0 == lastWanderlustTime || now < lastWanderlustTime)
	{
		delta = 1000 * 50 + 1;
	}

//	if (isWandering && delta > 1000 * 50 && !isMoving && !curTarget)	
	if (delta > 1000 * 50 && !isMoving && !curTarget)	
	{
		lastWanderlustTime = now - (rand() % 10000);

		int shouldGo = isWandering;
		if (!shouldGo)
		{
			if (SPACE_GROUND == ss->WhatAmI())
			{
				BBOSMob *tMob = NULL;
				MapListState state = ss->mobList->GetState();
				tMob = ss->mobList->GetFirst(cellX, cellY);
				while (tMob)
				{
					if (SMOB_TOWER == tMob->WhatAmI())
						shouldGo = TRUE;
					tMob = ss->mobList->GetNext();
				}
				ss->mobList->SetState(state);

			}
		}

		if (26 == type && !shouldGo)
		{
			// find out how many bats are in this square
			int numBats = 0;
			MapListState state = ss->mobList->GetState();
			BBOSMob *tMob = ss->mobList->GetFirst(cellX, cellY);
			while (tMob)
			{
				if (SMOB_MONSTER == tMob->WhatAmI() && 26 == ((BBOSMonster *) tMob)->type)
					++numBats;
				tMob = ss->mobList->GetNext();
			}
			ss->mobList->SetState(state);

			if (numBats < 2 || numBats > 6)
				shouldGo = TRUE;
		}

		if (shouldGo)
		{
			// what direction do I want to go?
			int tX = cellX;
			int tY = cellY;

			int randDir = rand() % 4;

			if (0 == randDir)
				--tX;
			else if (1 == randDir)
				++tX;
			else if (2 == randDir)
				--tY;
			else if (3 == randDir)
				++tY;

			int willGo = TRUE;
			// don't go too near the door
			if (tX < cellX && tX < 2)
				willGo = FALSE;
			if (tY < cellY && tY < 2)
				willGo = FALSE;

			// Can I go there?
			if (tX == cellX && tY == cellY)
				willGo = FALSE;
			if (!ss->CanMove(cellX, cellY, tX, tY))
				willGo = FALSE;

			if (willGo && magicEffectAmount[MONSTER_EFFECT_BIND] <= 0)
			{
				// okay, let's go!
				isMoving = TRUE;
				targetCellX = tX;
				targetCellY = tY;
				moveStartTime = timeGetTime();

				MessMobBeginMove bMove;
				bMove.mobID = (unsigned long) this;
				bMove.x = cellX;
				bMove.y = cellY;
				bMove.targetX = tX;
				bMove.targetY = tY;
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(bMove), &bMove);
			}
		}
	}

	if (thiefMode != THIEF_NOT_THIEF)
	{
		// process effects
		delta = now - lastThiefTime;

		if (now < lastThiefTime)
		{
			delta = 1000 * 600 + 1;
		}

		if (0 == lastThiefTime)
		{
			lastThiefTime = now;
			delta = 0;
		}

		if (THIEF_WAITING == thiefMode)
		{
			if (delta > 1000 * 600)	
			{
				lastThiefTime = now;

				// find some poor sucker
				BBOSAvatar *candAv = NULL;
				curMob = (BBOSMob *) ss->avatars->First();
				while (curMob && !candAv)
				{
					if (3 == rand() % 10)
					{
						candAv = (BBOSAvatar *) curMob;
					}

					curMob = (BBOSMob *) ss->avatars->Next();
					if (!curMob)
						curMob = (BBOSMob *) ss->avatars->First();
				}

				if (candAv && magicEffectAmount[MONSTER_EFFECT_BIND] <= 0)
				{
					// teleport next to her
					MessMobDisappear messMobDisappear;
					messMobDisappear.mobID = (unsigned long) this;
					messMobDisappear.x = cellX;
					messMobDisappear.y = cellY;
					ss->SendToEveryoneNearBut(0, cellX, cellY,
						sizeof(messMobDisappear),(void *)&messMobDisappear,6);

					cellX = candAv->cellX;
					cellY = candAv->cellY;
					ss->mobList->Move(this);

					// send arrival message
					MessMobAppear mAppear;
					mAppear.mobID = (unsigned long) this;
					mAppear.x = cellX;
					mAppear.y = cellY;
					mAppear.monsterType = type;
					mAppear.subType = subType;
					mAppear.type = WhatAmI();

					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mAppear), &mAppear,6);

					// laugh
					MessGenericEffect messGE;
					messGE.avatarID = -1;
					messGE.mobID    = (long) this;
					messGE.x        = cellX;
					messGE.y        = cellY;
					messGE.r        = 255;
					messGE.g        = 0;
					messGE.b        = 0;
					messGE.type     = 1;  // type of particles
					messGE.timeLen  = 1; // in seconds
					ss->SendToEveryoneNearBut(0, cellX, cellY,
										 sizeof(messGE),(void *)&messGE);

					// announce the stealing
			  		sprintf(&(tempText[2]),"The Thieving Spirit has arrived!");
					tempText[0] = NWMESS_PLAYER_CHAT_LINE;
					tempText[1] = TEXT_COLOR_SHOUT;
					ss->SendToEveryoneNearBut(0, cellX, cellY, 
						       strlen(tempText) + 1,(void *)&tempText);
					// set to running
					thiefMode = THIEF_RUNNING;
				}
			}
		}
		else if (THIEF_RUNNING == thiefMode)
		{
			if (delta > 1000 * 30)	
			{
				lastThiefTime = now;

				// find some poor sucker
				BBOSAvatar *candAv = NULL;
				curMob = (BBOSMob *) ss->avatars->First();
				int avTries = 0;
				while (curMob && !candAv && avTries < 200)
				{
					if (curMob->cellX == cellX && curMob->cellY == cellY)
					{
						candAv = (BBOSAvatar *) curMob;
					}

					++avTries;

					curMob = (BBOSMob *) ss->avatars->Next();
					if (!curMob)
						curMob = (BBOSMob *) ss->avatars->First();
				}

				if (candAv)
				{
					// steal some stuff
					Inventory *inv = (candAv->charInfoArray[candAv->curCharacterIndex].wield);

					int tries = 0;
					InventoryObject * goodie = NULL;

					// first, try to steal totems
					InventoryObject *iObject = (InventoryObject *) inv->objects.First();
					int totemCount = 0;
					while (iObject && !goodie)
					{
						if (INVOBJ_TOTEM == iObject->type)
						{
							if (3 == rand() % 10)
							{
								goodie = iObject;
							}
							++totemCount;
						}

						iObject = (InventoryObject *) inv->objects.Next();
						if (!iObject && totemCount > 0)
							iObject = (InventoryObject *) inv->objects.First();
					}

					// no?  Then just steal any old thing
					tries = 0;
					iObject = (InventoryObject *) inv->objects.First();
					while (iObject && !goodie && tries < 100)
					{
						if (INVOBJ_BLADE != iObject->type && 3 == rand() % 5)
						{
							goodie = iObject;
						}

						++tries;

						iObject = (InventoryObject *) inv->objects.Next();
						if (!iObject)
							iObject = (InventoryObject *) inv->objects.First();
					}


					if (!goodie)
					{
						// no?  Then steal from general inventory!
						inv = (candAv->charInfoArray[candAv->curCharacterIndex].inventory);
				
						tries = 0;
						iObject = (InventoryObject *) inv->objects.First();
						while (iObject && !goodie && tries < 100)
						{
							if (INVOBJ_BLADE != iObject->type && 3 == rand() % 35)
							{
								goodie = iObject;
							}

							++tries;
					
							iObject = (InventoryObject *) inv->objects.Next();
							if (!iObject)
								iObject = (InventoryObject *) inv->objects.First();
						}
					}

					if (goodie && !candAv->accountType)
					{
						// get the goodie
						inv->objects.Remove(goodie);
						inventory->objects.Append(goodie);

						// announce the stealing
				  		sprintf(&(tempText[2]), "The Thieving Spirit has stolen %s's %s!",
							candAv->charInfoArray[candAv->curCharacterIndex].name,
							goodie->WhoAmI());
						tempText[0] = NWMESS_PLAYER_CHAT_LINE;
						tempText[1] = TEXT_COLOR_SHOUT;
						ss->SendToEveryoneNearBut(0, cellX, cellY, 
							       strlen(tempText) + 1,(void *)&tempText);
					}

					// laugh
					MessGenericEffect messGE;
					messGE.avatarID = -1;
					messGE.mobID    = (long) this;
					messGE.x        = cellX;
					messGE.y        = cellY;
					messGE.r        = 255;
					messGE.g        = 0;
					messGE.b        = 0;
					messGE.type     = 1;  // type of particles
					messGE.timeLen  = 1; // in seconds
					ss->SendToEveryoneNearBut(0, cellX, cellY,
										 sizeof(messGE),(void *)&messGE);
				}

				// move 1 squares away (to a legal spot)
				int tempX, tempY;
				do
				{
					tempX = cellX + (rand() % 3) - 1;
					tempY = cellY + (rand() % 3) - 1;
				} while (!ss->CanMove(tempX,tempY,tempX,tempY) || 
					      (tempX == cellX && tempY == cellY)
						  );


				if (magicEffectAmount[MONSTER_EFFECT_BIND] > 0)
				{
					tempX = cellX;
					tempY = cellY;
				}

				MessMobDisappear messMobDisappear;
				messMobDisappear.mobID = (unsigned long) this;
				messMobDisappear.x = cellX;
				messMobDisappear.y = cellY;
				ss->SendToEveryoneNearBut(0, cellX, cellY,
					sizeof(messMobDisappear),(void *)&messMobDisappear,6);

				cellX = tempX;
				cellY = tempY;
				ss->mobList->Move(this);

				// send arrival message
				MessMobAppear mAppear;
				mAppear.mobID = (unsigned long) this;
				mAppear.x = cellX;
				mAppear.y = cellY;
				mAppear.monsterType = type;
				mAppear.subType = subType;
				mAppear.type = WhatAmI();

				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mAppear), &mAppear,6);


				// set to waiting
				thiefMode = THIEF_WAITING;

			}
		}
	}

}


//******************************************************************
void BBOSMonster::ReactToAdjacentPlayer(BBOSAvatar *curAvatar, SharedSpace *ss)
{
	char tempText[1024];

	// mosnter comes to attack!
	if ((MONSTER_PLACE_SPIRITS & monsterData[type][subType].placementFlags ||
		  MONSTER_PLACE_DRAGONS & monsterData[type][subType].placementFlags ||
		  26 == type // bats
		  ) 
		 && magicEffectAmount[MONSTER_EFFECT_BIND] <= 0)
	{

		int range = 2;
		if (SPACE_DUNGEON == ss->WhatAmI() && 
			 (((DungeonMap *) ss)->specialFlags & SPECIAL_DUNGEON_TEMPORARY))
			range = 1;
			
		if(curTarget == curAvatar || NULL == curTarget)
		{
			int go = TRUE;
			if (type == 16 && subType == 1)
				go = FALSE;
			if (7 == type && 5 != subType)
				go = FALSE;
				            // exclude Dokk and normal dragons, accept for Dragon Illusions
			if (go)
			{
				int tcX = curAvatar->cellX;
				int tcY = curAvatar->cellY;

				if (spawnX - tcX > range)
					tcX = cellX+1;
				if (spawnY - tcY > range)
					tcY = cellY+1;
				if (tcX - spawnX > range)
					tcX = cellX-1;
				if	(tcY - spawnY > range)
					tcY = cellY-1;

				int fail = FALSE;
				if (SPACE_DUNGEON == ss->WhatAmI() && 
					 !(MONSTER_PLACE_SPIRITS & monsterData[type][subType].placementFlags))
				{
					if (!ss->CanMove(cellX, cellY, tcX, tcY))
						fail = TRUE;
				}

				if (!fail)
				{
					targetCellX = tcX;
					targetCellY = tcY;

					isMoving = TRUE;
					moveStartTime = timeGetTime() - 500;

					MessMobBeginMove bMove;
					bMove.mobID = (unsigned long) this;
					bMove.x = cellX;
					bMove.y = cellY;
					bMove.targetX = targetCellX;
					bMove.targetY = targetCellY;
					ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(bMove), &bMove);
				}
			}
		}
	}

	if (16 == type && 1 == subType) // Dokk
	{
		std::vector<TagID> targetReceiptList;
		targetReceiptList.clear();
		targetReceiptList.push_back(curAvatar->socketIndex);

		sprintf(&(tempText[1]),"Dokk tells you, Another worthless female comes to hinder my plans...  Nothing can defeat me, Human!  I will crush you!");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

	}

	if (7 == type && 4 == subType) // Dragon Overlord
	{
		std::vector<TagID> targetReceiptList;
		targetReceiptList.clear();
		targetReceiptList.push_back(curAvatar->socketIndex);

		
		if (!strcmp(uniqueName,"Dragon Chaplain"))
			sprintf(&(tempText[1]),"The Chaplain tells you, All of us are children of the Great Spirits, but still humans disgust me.");
		else if (!strcmp(uniqueName,"Dragon ArchMage"))
			sprintf(&(tempText[1]),"The ArchMage tells you, We have no time for foolish, mundane humans.");
		else
			sprintf(&(tempText[1]),"The Overlord tells you, Why do you approach, Human?  What are your intentions?");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

	}

}

//******************************************************************
char *BBOSMonster::Name(void)
{
	if (uniqueName[0])
	{
		return uniqueName;
	}
	else
	{
		return monsterData[type][subType].name;
	}
}

//******************************************************************
void BBOSMonster::AnnounceMyselfCustom(SharedSpace *ss)
{
	MessMobAppearCustom mAppear;
	mAppear.mobID = (unsigned long) this;
	mAppear.x = cellX;
	mAppear.y = cellY;
	mAppear.monsterType = type;
	mAppear.subType = subType;
	CopyStringSafely(Name(), 32, mAppear.name, 32);
	mAppear.a = a;
	mAppear.r = r;
	mAppear.g = g;
	mAppear.b = b;
	mAppear.sizeCoeff = sizeCoeff;

	if(SPACE_DUNGEON == ss->WhatAmI())
	{
		mAppear.staticMonsterFlag = FALSE;
		if (!isWandering && !isPossessed)
			mAppear.staticMonsterFlag = TRUE;
	}
	mAppear.type = WhatAmI();

	// send arrival message
	ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mAppear), &mAppear);
}


//******************************************************************
void BBOSMonster::RecordDamageForLootDist(int damageDone, BBOSAvatar *curAvatar)
{
	int recIndex = -1;
	// find the existing record for this attacker
	for (int i = 0; i < 10; ++i)
	{
		if (attackerPtrList[i] == curAvatar && 
			 attackerLifeList[i] <= 
			      curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime)
			recIndex = i;
	}
	// if found
	if (recIndex > -1)
	{
		// add damage to record, and we're done
		attackerLifeList[recIndex] =
			      curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime;
		attackerDamageList[recIndex] += damageDone;
		return;
	}
	else
	{
		// find an empty record to put this attacker into
		for (int i = 0; i < 10; ++i)
		{
			if (NULL == attackerPtrList[i])
				recIndex = i;
		}
		// if found
		if (recIndex > -1)
		{
			// change record, set initial damage
			attackerPtrList[recIndex] = curAvatar;
			attackerLifeList[recIndex] =
				      curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime;
			attackerDamageList[recIndex] = damageDone;
			return;
		}
		else
		{
			// find a record with less damage than damageDone
			for (int i = 0; i < 10; ++i)
			{
				if (attackerDamageList[i] < damageDone)
					recIndex = i;
			}
			// if found
			if (recIndex > -1)
			{
				// take over the record
				attackerPtrList[recIndex] = curAvatar;
				attackerLifeList[recIndex] =
					      curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime;
				attackerDamageList[recIndex] = damageDone;
			}
		}
	}

}


//******************************************************************
void BBOSMonster::ClearDamageForLootDist(BBOSAvatar *curAvatar)
{
	int recIndex = -1;
	// find the existing record for this attacker
	for (int i = 0; i < 10; ++i)
	{
		if (attackerPtrList[i] == curAvatar && 
			 attackerLifeList[i] <= 
			      curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime)
			recIndex = i;
	}
	// if found
	if (recIndex > -1)
	{
		attackerDamageList[recIndex] = 0;
	}

}


//******************************************************************
BBOSAvatar *BBOSMonster::MostDamagingPlayer(float degradeCoeff)
{
	int recIndex = -1;
	int mostDam = 0;
	// find the valid record with the most damage
	for (int i = 0; i < 10; ++i)
	{
		if (attackerPtrList[i] && 
			 attackerDamageList[i] > mostDam)
		{
			SharedSpace *sp;
			BBOSAvatar * foundAvatar = 
				bboServer->FindAvatar(attackerPtrList[i], &sp);
			if (foundAvatar && 
				 foundAvatar->charInfoArray[foundAvatar->curCharacterIndex].lifeTime >=
				      attackerLifeList[i])
			{
				recIndex = i;
				mostDam = attackerDamageList[i];
			}
		}
	}
	// if found
	if (recIndex > -1)
	{
		float f = (float) attackerDamageList[recIndex];
		f *= degradeCoeff;
		if (f < 1)
			f = 1;
		attackerDamageList[recIndex] = (long) f;

		return attackerPtrList[recIndex];
	}

	return NULL;
}


//******************************************************************
void BBOSMonster::HandleQuestDeath(void)
{
	for (int i = 0; i < 10; ++i)
	{
		if (attackerPtrList[i])
		{
			SharedSpace *sp;
			BBOSAvatar * foundAvatar = 
				bboServer->FindAvatar(attackerPtrList[i], &sp);
			if (foundAvatar)
				foundAvatar->QuestMonsterKill(sp, this);
		}
	}
}


//******************************************************************
void BBOSMonster::MonsterMagicEffect(int type, float timeDelta, float amount)
{
	MagicEffect(type,	
		         timeGetTime() + timeDelta * (1 - magicResistance), 
					amount * (1 - magicResistance));
}


//******************************************************************
void BBOSMonster::HandlePossessedQuest(int amuletsGiven, SharedSpace *ss)
{

	char tempText[1024];

	float totalPlayerPower = 0;
	//   finds every player near
	BBOSAvatar *chosenAvatar = NULL;
	BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
	while (curMob)
	{
		if (abs(curMob->cellX - cellX) < 3 && abs(curMob->cellY - cellY) < 3) 
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;

			// remember the name of someone in the same square as me
			if (curMob->cellX == cellX && curMob->cellY == cellY && !chosenAvatar)
				chosenAvatar = curAvatar;

			//	totals up their power
			//		power = dodge skill + age + sword damage + sword to-hit + sword magic
			totalPlayerPower += 
				     curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime / 20;

			totalPlayerPower += 
				curAvatar->GetDodgeLevel();

			totalPlayerPower += curAvatar->BestSwordRating();
		}

		curMob = (BBOSMob *) ss->avatars->Next();
	}

	if (!chosenAvatar)
		return;

	int freeSlot = -1;
	for (int i = 0; i < QUEST_SLOTS; ++i)
	{
		if (-1 == chosenAvatar->charInfoArray[chosenAvatar->curCharacterIndex].
			              quests[i].completeVal)
		{
			freeSlot = i;
			i = QUEST_SLOTS;
		}
	}

	if (-1 == freeSlot)
	{
		sprintf(&(tempText[1]),"The Dragon takes the amulets and says,");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

		sprintf(&(tempText[1]),"%s, you are too burdened with tasks already to",
			       chosenAvatar->charInfoArray[chosenAvatar->curCharacterIndex].name);
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

		sprintf(&(tempText[1]),"accept what I have to tell.  I'll keep your amulets.");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

		sprintf(&(tempText[1]),"Let this be a lesson to you, puny Human.");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);
		return;
	}

	//	randomly picks a dungeon
	SharedSpace *tempss = (SharedSpace *) bboServer->spaceList->First();
	SharedSpace *dungeonPicked = NULL;
	while (!dungeonPicked)
	{
		if (SPACE_DUNGEON == tempss->WhatAmI() && 0 == ((DungeonMap *) tempss)->specialFlags )
		{
			if (36 == rand() % 100)
			{
				DungeonMap *dm = (DungeonMap *) tempss;
//				if (!IsCompletelySame(dm->masterName, closePlayerName))
				dungeonPicked = tempss;
			}
		}
		tempss = (SharedSpace *) bboServer->spaceList->Next();
		if (!tempss)
			tempss = (SharedSpace *) bboServer->spaceList->First();
	}

	//	creates a Possessed monster in the back of the dungeon
	int monsterPosType = rand() % 5;
	int mPosX = rand() % (((DungeonMap *)dungeonPicked)->width/2);
	int mPosY = rand() % (((DungeonMap *)dungeonPicked)->height/2);

	Quest *quest = &chosenAvatar->charInfoArray[chosenAvatar->curCharacterIndex].
		                  quests[freeSlot];
	quest->EmptyOut();
	quest->completeVal = 0;  // active, but not complete
	quest->timeLeft.SetToNow();
	quest->timeLeft.AddMinutes(60*24); // add one day

	quest->questSource = MAGIC_MAX;

	// now add questParts
	QuestPart *qp = new QuestPart(QUEST_PART_VERB, "VERB");
	quest->parts.Append(qp);
	qp->type = QUEST_VERB_KILL;

	QuestPart *qt = new QuestPart(QUEST_PART_TARGET, "TARGET");
	quest->parts.Append(qt);

	qt->type = QUEST_TARGET_LOCATION;
	
	qt->monsterType    = 1 + rand() % 5; // means a possessed
	qt->monsterSubType = totalPlayerPower;
	qt->mapType = SPACE_DUNGEON;
	qt->x = mPosX;
	qt->y = mPosY;
	qt->range = amuletsGiven;

	DungeonMap *dm = (DungeonMap *)dungeonPicked;
	qt->mapSubType = GetCRCForString(dm->name);
//	sprintf(tempText2, dm->name);

	//tell the assembled about it
	if (amuletsGiven > 1)
	{
		sprintf(&(tempText[1]),"The Dragon takes the amulets and tells you,");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

		sprintf(&(tempText[1]),"These amulets bring to me a vision of a foul Possessed");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);
	}
	else
	{
		sprintf(&(tempText[1]),"The Dragon takes the amulet and tells you,");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

		sprintf(&(tempText[1]),"This amulet brings to me a vision of a foul Possessed");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);
	}

//	if (rand() % 2)
	if (TRUE)
	{
		sprintf(&(tempText[1]),"creature in the %s.", ((DungeonMap *)dungeonPicked)->name);
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);
	}
	else
	{
		int rX = ((DungeonMap *)dungeonPicked)->enterX + (rand() % 13) - 6;
		if (rX < 0)
			rX = 0;
		if (rX > 255)
			rX = 255;
		int rY = ((DungeonMap *)dungeonPicked)->enterY + (rand() % 13) - 6;
		if (rY < 0)
			rY = 0;
		if (rY > 255)
			rY = 255;

		sprintf(&(tempText[1]),"creature in a dungeon near %dN %dE.", 256-rY, 256-rX);
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);
	}

	sprintf(&(tempText[1]),"Be careful.  It is a terrible minion of the Demon King.");
	tempText[0] = NWMESS_PLAYER_CHAT_LINE;
	ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

	sprintf(&(tempText[1]),"But it must be destroyed, if you want your world to continue.");
	tempText[0] = NWMESS_PLAYER_CHAT_LINE;
	ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

	sprintf(&(tempText[1]),"Good luck, %s!", 
		  chosenAvatar->charInfoArray[chosenAvatar->curCharacterIndex].name);
	tempText[0] = NWMESS_PLAYER_CHAT_LINE;
	ss->SendToEveryoneNearBut(0, cellX, cellY, strlen(tempText) + 1,(void *)&tempText, 3);

}

//******************************************************************
void BBOSMonster::AddPossessedLoot(int count)
{

//	char tempText[1024];

	InventoryObject *iObject;
	InvStaff *staffExtra;
	InvIngredient *exIn;
	InvTotem *totemExtra;

	//	gives it random stuff according to its power
	for (int i = 0; i < count; ++i)
	{
		int goodie = rand() % 10;
		switch(goodie)
		{
		case 0:
		case 6:
		case 7:
			iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
			staffExtra = (InvStaff *)iObject->extra;
			staffExtra->type     = 0;
			staffExtra->quality  = 3;

			iObject->mass = 0.0f;
			iObject->value = 500 * (3 + 1) * (3 + 1);
			iObject->amount = 1;
			UpdateStaff(iObject, 0);
			inventory->objects.Append(iObject);
			break;

		case 8:
		case 9:
			iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
			totemExtra = (InvTotem *)iObject->extra;
			totemExtra->type     = 0;
			totemExtra->quality  = 18; // undead

			iObject->mass = 0.0f;
			iObject->value = totemExtra->quality * totemExtra->quality * 14 + 1;
			if (totemExtra->quality > 12)
				iObject->value = totemExtra->quality * totemExtra->quality * 14 + 1 + (totemExtra->quality-12) * 1600;
			iObject->amount = 1;
			UpdateTotem(iObject);
			inventory->objects.Append(iObject);
			break;
/*
		case 1:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_GREEN_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			inventory->objects.Append(iObject);
			break;

		case 2:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_BLACK_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			inventory->objects.Append(iObject);
			break;
  */
		case 3:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_RED_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			inventory->objects.Append(iObject);
			break;

		case 4:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_WHITE_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			inventory->objects.Append(iObject);
			break;

		case 1:
		case 2:
		case 5:
			iObject = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
			exIn = (InvIngredient *)iObject->extra;
			exIn->type     = INGR_BLUE_DUST;
			exIn->quality  = 1;

			iObject->mass = 0.0f;
			iObject->value = 1000;
			iObject->amount = 1;
			inventory->objects.Append(iObject);
			break;

		}
	}

}


//******************************************************************
void BBOSMonster::VLordAttack(SharedSpace *ss)
{
	DWORD now = timeGetTime();
	std::vector<TagID> tempReceiptList;
	char tempText[1024];
	MessInfoText infoText;

	MessMonsterSpecialAttack messAA;
	messAA.mobID    = (long) this;
	messAA.type   = 1;
	ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messAA), &messAA,2);

	BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
	while (curMob)
	{
		if (curMob->cellX == cellX && curMob->cellY == cellY) 
		{
			BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;

			InvSkill *skillInfo = NULL;
			int dodgeMod = 0;
			InventoryObject *io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills->objects.First();
			while (io)
			{
				if (!strcmp("Dodging",io->WhoAmI()))
				{
					skillInfo = (InvSkill *) io->extra;
					dodgeMod = skillInfo->skillLevel - 1;
					int addAmount = toHit - dodgeMod;
					if (addAmount < 0)
						addAmount = 0;
					if (addAmount > 10)
						addAmount = 0;
					skillInfo->skillPoints += addAmount;

//					if (abs(dodgeMod - toHit) < 3)
//						skillInfo->skillPoints += 1;
				}
				io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills->objects.Next();
			}

			dodgeMod += curAvatar->totemEffects.effect[TOTEM_QUICKNESS];

         //    1. chance = 2d20 + ToHit - player.Physical
			int cVal = toHit - dodgeMod;

			if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
				cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

			int chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

			if (cVal + 40 <= 20) // if monster can't EVER hit
			{
				if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
					chance = 30;  // Hit!
			}

         //    2. if chance > 20, hit was successful
			if (chance > 20)
			{
	         //    3. damage = damage
				int dDone = damageDone - curAvatar->totemEffects.effect[TOTEM_TOUGHNESS] / 3;
				if (dDone < 0)
					dDone = 0;

				if (MONSTER_PLACE_SPIRITS & 
					 monsterData[type][subType].placementFlags)
				{
					float fDone = (float)dDone * 
						(1.0f - (curAvatar->totemEffects.effect[TOTEM_PROT_SPIRITS] / 30.0f));
					dDone = (int) fDone;
				}

				dDone += (int) rnd(0, dDone);

				if (curAvatar->totemEffects.effect[TOTEM_LIFESTEAL] > 0)
				{
					int suck = dDone * curAvatar->totemEffects.effect[TOTEM_LIFESTEAL] / 60;
					dDone += suck;
					health += suck;
				}

				if (ACCOUNT_TYPE_ADMIN != curAvatar->accountType)
					curAvatar->charInfoArray[curAvatar->curCharacterIndex].health -= dDone;

				MessAvatarHealth messHealth;
				messHealth.avatarID  = curAvatar->socketIndex;
				messHealth.health    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
				messHealth.healthMax = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
				ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messHealth), &messHealth,2);

				if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health <= 0)
				{
					curAvatar->charInfoArray[curAvatar->curCharacterIndex].health = 0;
					curAvatar->isDead = TRUE;

					tempReceiptList.clear();
					tempReceiptList.push_back(curAvatar->socketIndex);
					if (SPACE_GROUND == ss->WhatAmI())
					{
						sprintf(tempText,"The %s defeats you at %dN %dE.", Name(),
						256-curAvatar->cellY, 256-curAvatar->cellX);
					}
					else if (SPACE_REALM == ss->WhatAmI())
					{
						sprintf(tempText,"The %s defeats you in a Mystic Realm at %dN %dE.",
						Name(),
						64-curAvatar->cellY, 64-curAvatar->cellX);
					}
					else if (SPACE_LABYRINTH == ss->WhatAmI())
					{
						sprintf(tempText,"The %s defeats you in the Labyrinth at %dN %dE.",
						Name(),
						64-curAvatar->cellY, 64-curAvatar->cellX);
					}
					else
					{
						sprintf(tempText,"The %s defeats you in the %s (%dN %dE) at %dN %dE.",
						Name(),
						((DungeonMap *) ss)->name,
						256-((DungeonMap *) ss)->enterY, 
						256-((DungeonMap *) ss)->enterX,
						((DungeonMap *) ss)->height - curAvatar->cellY, 
						((DungeonMap *) ss)->width  - curAvatar->cellX);
					}
					CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
					ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

				}
				else if (!(curAvatar->curTarget))
				{
					curAvatar->lastAttackTime = now;
					curAvatar->curTarget = this;
				}

				if (24 == type) // spidren group
				{
					cVal = toHit - dodgeMod;

					if (magicEffectAmount[DRAGON_TYPE_WHITE] > 0)
						cVal -= magicEffectAmount[DRAGON_TYPE_WHITE] * 1.0f;

					cVal -= curAvatar->totemEffects.effect[TOTEM_PROT_WEB];

					chance = 2 + (rand() % 20) + (rand() % 20) + cVal;

					if (cVal + 40 <= 20) // if monster can't EVER hit
					{
						if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
							chance = 30;  // Hit!
					}

					//    2. if chance > 20, hit was successful
					if (chance > 20)
					{
						// spin a web on her ass!
						MessMagicAttack messMA;
						messMA.damage = 30;
						messMA.mobID = -1;
						messMA.avatarID = curAvatar->socketIndex;
						messMA.type = 1;
						ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
										sizeof(messMA), &messMA,3);

						tempReceiptList.clear();
						tempReceiptList.push_back(curAvatar->socketIndex);
						sprintf(tempText,"The %s ensnares you in its web!", Name());
						CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
						ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

						int effectVal = toHit - dodgeMod/2 - curAvatar->totemEffects.effect[TOTEM_PROT_WEB] * 2;
						if (effectVal < 2)
							effectVal = 2;

						curAvatar->MagicEffect(MONSTER_EFFECT_BIND, 
								timeGetTime() + effectVal * 1000, effectVal);
						curAvatar->MagicEffect(MONSTER_EFFECT_TYPE_BLUE, 
								timeGetTime() + effectVal * 1000, effectVal);

					}
				}
			}
			else
			{
				// whish!
				if (!(curAvatar->curTarget))
				{
					curAvatar->lastAttackTime = now;
					curAvatar->curTarget = this;
				}

			}
		}
		ss->avatars->Find(curMob);
		curMob = (BBOSMob *) ss->avatars->Next();
	}

}

//******************************************************************
void BBOSMonster::PortalBat(SharedSpace *ss)
{
	char tempText[1024];

	MapListState state = ss->mobList->GetState();
	int foundBat = FALSE;
	BBOSMob *tMob = NULL;
	tMob = ss->mobList->GetFirst(cellX, cellY, 1000);
	while (tMob && !foundBat)
	{
		if (SMOB_MONSTER == tMob->WhatAmI() && 
			 26 == ((BBOSMonster *) tMob)->type && 
			 4 == (rand() % 10))
		{
			foundBat = TRUE;
		}
		if (!foundBat)
			tMob = ss->mobList->GetNext();
	}

	ss->mobList->SetState(state);

	if (tMob && foundBat)
	{
		// teleport next to me
		MessMobDisappear messMobDisappear;
		messMobDisappear.mobID = (unsigned long) tMob;
		messMobDisappear.x = tMob->cellX;
		messMobDisappear.y = tMob->cellY;
		ss->SendToEveryoneNearBut(0, tMob->cellX, tMob->cellY,
			sizeof(messMobDisappear),(void *)&messMobDisappear,6);

		tMob->cellX = tMob->targetCellX = targetCellX;
		tMob->cellY = tMob->targetCellY = targetCellY;
		ss->mobList->Move(tMob);

		// send arrival message
		MessMobAppear mAppear;
		mAppear.mobID = (unsigned long) tMob;
		mAppear.x = tMob->cellX;
		mAppear.y = tMob->cellY;
		mAppear.monsterType = ((BBOSMonster *) tMob)->type;
		mAppear.subType = ((BBOSMonster *) tMob)->subType;
		mAppear.type = tMob->WhatAmI();

		ss->SendToEveryoneNearBut(0, tMob->cellX, tMob->cellY, sizeof(mAppear), &mAppear,6);

		// laugh
		MessGenericEffect messGE;
		messGE.avatarID = -1;
		messGE.mobID    = (long) tMob;
		messGE.x        = tMob->cellX;
		messGE.y        = tMob->cellY;
		messGE.r        = 255;
		messGE.g        = 0;
		messGE.b        = 255;
		messGE.type     = 0;  // type of particles
		messGE.timeLen  = 1; // in seconds
		ss->SendToEveryoneNearBut(0, tMob->cellX, tMob->cellY,
							 sizeof(messGE),(void *)&messGE);

		// announce the summoning
		sprintf(&(tempText[2]),"A bat has been summoned!");
		tempText[0] = NWMESS_PLAYER_CHAT_LINE;
		tempText[1] = TEXT_COLOR_SHOUT;
		ss->SendToEveryoneNearBut(0, tMob->cellX, tMob->cellY, 
			       strlen(tempText) + 1,(void *)&tempText);

	}

}

/* end of file */



