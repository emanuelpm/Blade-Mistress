
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "BBOServer.h"
#include "BBO-SAvatar.h"
#include "BBO-SMonster.h"
#include "BBO-Stower.h"
#include "BBO-Schest.h"
#include "BBO-Stree.h"
#include "BBO-Sarmy.h"
#include "BBO-SAutoQuest.h"
#include "BBO-SwarpPoint.h"
#include "BBO-SgroundEffect.h"
#include "BBO.h"
#include ".\helper\GeneralUtils.h"
#include ".\network\NetWorldMessages.h"
#include "TotemData.h"
#include "StaffData.h"
#include "monsterData.h"
#include "dungeon-map.h"
#include "realm-map.h"
#include "tower-map.h"
#include "tokenManager.h"
#include "QuestSystem.h"
#include ".\helper\crc.h"
#include "version.h"
#include ".\helper\uniquenames.h"

#include <Shlobj.h>
#include <direct.h>

extern int lastAvatarCount;

int skillHotKeyArray[19] = {88,79, 75,72,78,66,67, 49,50,51,52, 53,54,55,56, 71, 68, 57, 48};
//                                                                            g   d	 

char skillNameArray[19][32] =
{
    {"Explosives"},
    {"Swordsmith"},
    {"Katana Expertise"},
    {"Chaos Expertise"},
    {"Mace Expertise"},
    {"Bladestaff Expertise"},
    {"Claw Expertise"},
    {"Bear Magic"},
    {"Wolf Magic"},
    {"Eagle Magic"},
    {"Snake Magic"},
    {"Frog Magic"},
    {"Sun Magic"},
    {"Moon Magic"},
    {"Turtle Magic"},
    {"Geomancy"},
    {"Weapon Dismantle"},
    {"Evil Magic"},
    {"Totem Shatter"}
};

char earthKeyArray[10][32] =
{
    {"Karaol"},
    {"Vil"},
    {"Char"},
    {"Rin"},
    {"Bool"},
    {"Teth"},
    {"Gar"},
    {"Sver"},
    {"Mite"},
    {"Haow"}
};

struct PosMonsterTypes
{
    int type, subType;
    char name[32];
};

PosMonsterTypes pmTypes[5] =
{
    {1,3,"Possessed Golem"},
    {2,4,"Possessed Minotaur"},
    {3,1,"Possessed Tiger"},
    {8,2,"Possessed Spider"},
    {7,0,"Possessed Dragon"}
} ;

//******************************************************************
//******************************************************************
SkillObject::SkillObject(int doid, char *doname)	 : DataObject(doid,doname)
{
    exp = expUsed = 0;
}

//******************************************************************
SkillObject::~SkillObject()
{
    
}


//******************************************************************
//******************************************************************
BBOSCharacterInfo::BBOSCharacterInfo(void)
{
    inventory = new Inventory(MESS_INVENTORY_PLAYER);
    workbench = new Inventory(MESS_WORKBENCH_PLAYER);
    skills	 = new Inventory(MESS_SKILLS_PLAYER);
    wield		 = new Inventory(MESS_WIELD_PLAYER);

    cLevel = oldCLevel = 0;
    sprintf(witchQuestName,"NO WITCH");

    age = 1;
}

//******************************************************************
BBOSCharacterInfo::~BBOSCharacterInfo()
{
    delete inventory;
    delete workbench;
    delete skills	 ;
    delete wield	 ;	
}


//******************************************************************
//******************************************************************
BBOSAvatar::BBOSAvatar() : BBOSMob(SMOB_AVATAR,"AVATAR")
{
    cellX = targetCellX = townList[2].x;
    cellY = targetCellY = townList[2].y;
    moveStartTime = 0;
    lastSaveTime = lastHealTime = combineStartTime = lastTenTime = lastMinuteTime = 0;
    isMoving = TRUE;
    moveTimeCost = 0;
    isCombining = FALSE;
    curTarget = FALSE;
    activeCounter = 0;
    chantType = -1; // means no chant
    infoFlags = 0xffffffff; // all on
    kickOff = FALSE;

    trade = new Inventory(MESS_INVENTORY_YOUR_SECURE, this);
    bank  = new Inventory(MESS_INVENTORY_BANK       , this);

    agreeToTrade = FALSE;

    isInvisible = FALSE;
    controlledMonster = NULL;

    for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
    {
        charInfoArray[i].inventory->parent = this;
        charInfoArray[i].workbench->parent = this;
        charInfoArray[i].skills->parent    = this;
        charInfoArray[i].wield->parent     = this;

        charInfoArray[i].petDragonInfo[0].lastAttackTime = 0;
        charInfoArray[i].petDragonInfo[1].lastAttackTime = 0;

        for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
        {
            for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
            {
                charInfoArray[i].karmaGiven[j][k] = 0;
                charInfoArray[i].karmaReceived[j][k] = 0;
            }
        }

    }

    for (int i = 0; i < MAP_SIZE_WIDTH*MAP_SIZE_HEIGHT; ++i)
        updateMap[i] = FALSE;

    lastPlayerInvSent = MESS_INVENTORY_PLAYER;
    for (int i = 0; i < MESS_INVENTORY_MAX; ++i)
        indexList[i] = 0;

    sprintf(name,"UNINITALIZED");

    accountExperationTime.AddMinutes(60*24*14);

    contacts = new DoublyLinkedList();

    tradingPartner = NULL;

    timeOnCounter = kickMeOffNow = 0;

    lastTellName[0] = 0;

    specLevel[0] = specLevel[1] = specLevel[2] = 0;

    restrictionType = 0;

    status = AVATAR_STATUS_AVAILABLE;
    sprintf( status_text, "" );

    for (int i = 0; i < TOTEM_MAX; ++i)
        totemEffects.effect[i] = 0;

    isReferralDone = patronCount = 0;

    bHasLoaded = false;

}

//******************************************************************
BBOSAvatar::~BBOSAvatar()
{
    delete bank;
    delete trade;
    delete contacts;
}

//******************************************************************
void BBOSAvatar::Tick(SharedSpace *ss)
{
    if (!bHasLoaded)
    {
        kickOff = true;
    }

    BBOSMob *curMob = NULL;
    BBOSMonster *curMonster = NULL;
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);
    char tempText[1024];
    MessInfoText infoText;

    if (curCharacterIndex >= 0 && curCharacterIndex < NUM_OF_CHARS_PER_USER)
    {
        assert(charInfoArray[curCharacterIndex].inventory->money >= 0);
    }

    DungeonMap *dm = NULL;
    if (SPACE_DUNGEON == ss->WhatAmI())
        dm = (DungeonMap *) ss;

    DWORD delta;
    DWORD now = timeGetTime();
    if (isMoving)
    {
        agreeToTrade = FALSE;  // never trade while running

        delta = now - moveStartTime;

        if (0 == moveStartTime || now < moveStartTime) // || now == moveStartTime)
        {
            delta = moveTimeCost + 1;
        }

        curTarget = FALSE;
        // Finish the move
        if (delta > moveTimeCost)	
        {
            isMoving = FALSE;

            if (SPACE_GROUND == ss->WhatAmI())
            {
                // entering a town?
                for (int i = 0; i < NUM_OF_TOWNS; ++i)
                {
                    if (!(abs(townList[i].x - cellX) <= 3 &&	abs(townList[i].y - cellY) <= 3) &&
                         (abs(townList[i].x - targetCellX) <= 3 && abs(townList[i].y - targetCellY) <= 3))
                    {
                        sprintf(tempText,"You are entering %s.",townList[i].name);
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                        
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        charInfoArray[curCharacterIndex].spawnX = townList[i].x;
                        charInfoArray[curCharacterIndex].spawnY = townList[i].y;\
                    }
                }

                // leaving a town?
                for (int i = 0; i < NUM_OF_TOWNS; ++i)
                {
                    if ((abs(townList[i].x - cellX) <= 3 &&	abs(townList[i].y - cellY) <= 3) &&
                         !(abs(townList[i].x - targetCellX) <= 3 && abs(townList[i].y - targetCellY) <= 3))
                    {
                        sprintf(tempText,"You are leaving %s.",townList[i].name);
                        memcpy(infoText.text, tempText, 63);
                        
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        charInfoArray[curCharacterIndex].spawnX = townList[i].x;
                        charInfoArray[curCharacterIndex].spawnY = townList[i].y;
                    }
                }
            }
            cellX = targetCellX;
            cellY = targetCellY;

            // send arrival message
            MessAvatarAppear mAppear;
            mAppear.avatarID = socketIndex;
            mAppear.x = cellX;
            mAppear.y = cellY;
            if (isInvisible)
                ss->lserver->SendMsg(sizeof(mAppear), &mAppear, 0, &tempReceiptList);
            else
                ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(mAppear), &mAppear);

            UpdateClient(ss);

            QuestMovement(ss);

            // make monsters aggro on me (as I'm arriving)!
            curMob = ss->mobList->GetFirst(cellX, cellY, 1);
            while (curMob)
            {
                if (SMOB_MONSTER  == curMob->WhatAmI())
                {
                    if (curMob->cellX == cellX && 
                         curMob->cellY == cellY)
                    {
                        curMonster = (BBOSMonster *) curMob;

                        // make him aggro
                        if (!curMonster->curTarget && !isInvisible &&
                             !curMonster->controllingAvatar &&
                             curMonster->type != 7 &&  // not normal dragon
                             !(dm && dm->CanEdit(this))
                            )
                        {
                            curMonster->lastAttackTime = now - (rand() % 1000);
                            curMonster->curTarget = this;
                        }
                        else
                            curMonster = NULL;
                    }
                    else if (1 == abs(curMob->cellX - cellX) + abs(curMob->cellY - cellY) && 
                              !isInvisible && !(((BBOSMonster *) curMob)->controllingAvatar))
                    {
                        ((BBOSMonster *) curMob)->ReactToAdjacentPlayer(this, ss);
                    }
                }
                curMob = ss->mobList->GetNext();
            }

            // and if the monster attacks me, I'm gonna attack him right back!
            if (!curTarget && curMonster && !isInvisible && !curMonster->controllingAvatar)
            {
                lastAttackTime = now;
                charInfoArray[curCharacterIndex].petDragonInfo[0].lastAttackTime = now;
                charInfoArray[curCharacterIndex].petDragonInfo[1].lastAttackTime = now;

                curTarget = curMonster;
            }

            tokenMan.PlayerEntersSquare(charInfoArray[curCharacterIndex].name, ss, cellX, cellY);

        }
        return;
    }

    delta = now - lastAttackTime;

    if (0 == lastAttackTime || now < lastAttackTime) // || now == moveStartTime)
    {
        delta = 1000 * 2 + 1;
    }

    // attack curTarget (if it's still there)
    if (delta > 1000 * 2 + magicEffectAmount[DRAGON_TYPE_BLUE] * 300 && curTarget &&
         !(dm && dm->CanEdit(this) && 0 == curTarget->form)
        )	
    {
        lastAttackTime = now;
        int didAttack = FALSE;

        InvStaff usedStaff;
        int staffRange;
        usedStaff.charges = -3; // not used;

        curMob = ss->mobList->GetFirst(cellX, cellY);
        while (curMob && !didAttack)
        {
            if (curMob == curTarget && SMOB_MONSTER == curMob->WhatAmI() && !curMob->isDead &&  
                  curMob->cellX == cellX && curMob->cellY == cellY) 
            {
                didAttack = TRUE;
                curMonster = (BBOSMonster *) curMob;

                // find the weapon the avatar is using
                long bladeToHit = -1, bladeDamage;
                unsigned short bladePoison, bladeHeal, bladeSlow, bladeBlind, bladeShock;
                bladePoison = bladeHeal = bladeSlow = bladeBlind = bladeShock = 0;
    
                InvBlade *iBlade = NULL;
                InvStaff *iStaff = NULL;

                InventoryObject *io = (InventoryObject *) 
                    charInfoArray[curCharacterIndex].wield->objects.First();

                while (io && -1 == bladeToHit && !iStaff)
                {
                    if (INVOBJ_BLADE == io->type)
                    {
                        // if there are multiple swords
                        if( io->amount > 1 ) {
                            // create a copy of the sword being used
                            InventoryObject *tmp = new InventoryObject( INVOBJ_BLADE, 0, "tmp" );
                            io->CopyTo( tmp );
                            io->amount -= 1;  // decrement the amount

                            // tell the new sword it is one object
                            tmp->amount = 1;

                            // add it to the beginning of the list
                            (InventoryObject *) 
                                charInfoArray[curCharacterIndex].wield->objects.Prepend( tmp );

                            // set it as the attacking item
                            io = tmp;

                        }

                        iBlade = (InvBlade *)io->extra;

                        bladeToHit  = ((InvBlade *)io->extra)->toHit;
                        bladeDamage = ((InvBlade *)io->extra)->damageDone;

                        bladePoison = ((InvBlade *)io->extra)->poison;
                        bladeHeal   = ((InvBlade *)io->extra)->heal;
                        bladeSlow   = ((InvBlade *)io->extra)->slow;
                        bladeBlind  = ((InvBlade *)io->extra)->blind;
                        bladeShock  = ((InvBlade *)io->extra)->lightning;
                    }

                    if (INVOBJ_STAFF == io->type)
                    {
                        iStaff = (InvStaff *) io->extra;
                    }
                    io = (InventoryObject *) 
                        charInfoArray[curCharacterIndex].wield->objects.Next();
                }

                // nerfing green and black dust effects on weapons
                bladePoison = 0; //bladePoison/4;
                bladeHeal   = 0; //bladeHeal/4;

                if (iStaff)
                {
                    if (iStaff->isActivated)
                    {
                        // swing!
                        MessAvatarAttack messAA;
                        messAA.avatarID = socketIndex;
                        messAA.mobID    = (long) curMonster;
                        messAA.damage   = -2;
                        messAA.health   = -1000;
                        ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messAA), &messAA, 2);

                        if (!(curMonster->curTarget) || 
                                 (24 == curMonster->type && 3 == (rand() % 10))
                            )
                        {
                            curMonster->lastAttackTime = now;
                            curMonster->curTarget = this;
                        }

                        // do the effect!
                        staffRange = StaffAffectsArea(iStaff);
                        if (staffRange > 0)
                            usedStaff = *iStaff;
                        else
                            UseStaffOnMonster(ss, iStaff, curMonster);

                        // reduce the charge!
                        --(iStaff->charges);
                        if (iStaff->charges <= 0)
                        {
                            // it's gone!

                            InventoryObject *io = (InventoryObject *) 
                                charInfoArray[curCharacterIndex].wield->objects.First();
                            while (io)
                            {
                                if (INVOBJ_STAFF == io->type && iStaff == (InvStaff *) io->extra)
                                {
                                    charInfoArray[curCharacterIndex].wield->objects.Remove(io);
                                    delete io;
                                    iStaff = NULL;

                                    MessUnWield messUnWield;
                                    messUnWield.bladeID = (long)socketIndex;
                                    ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY,
                                        sizeof(messUnWield),(void *)&messUnWield);
                                }
                                io = (InventoryObject *) 
                                    charInfoArray[curCharacterIndex].wield->objects.Next();
                            }

                            // wield the next wielded weapon (if you have one)
                            MessBladeDesc messBladeDesc;

                            io = (InventoryObject *) 
                                charInfoArray[curCharacterIndex].wield->objects.First();
                            while (io)
                            {
                                if (INVOBJ_BLADE == io->type)
                                {

                                    FillBladeDescMessage(&messBladeDesc, io, this);
                                    if (isInvisible)
                                        ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                                    else
                                        ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY,
                                                sizeof(messBladeDesc),(void *)&messBladeDesc);

                                    charInfoArray[curCharacterIndex].wield->objects.Last();
                                }
                                else if (INVOBJ_STAFF == io->type)
                                {
                                    InvStaff *iStaff = (InvStaff *) io->extra;
                                    
                                    messBladeDesc.bladeID = (long)io;
                                    messBladeDesc.size    = 4;
                                    messBladeDesc.r       = staffColor[iStaff->type][0];
                                    messBladeDesc.g       = staffColor[iStaff->type][1];
                                    messBladeDesc.b       = staffColor[iStaff->type][2]; 
                                    messBladeDesc.avatarID= socketIndex;
                                    messBladeDesc.trailType  = 0;
                                    messBladeDesc.meshType = BLADE_TYPE_STAFF1;
                                    if (isInvisible)
                                        ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                                    else
                                        ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY,
                                                sizeof(messBladeDesc),(void *)&messBladeDesc);
                                    
                                    charInfoArray[curCharacterIndex].wield->objects.Last();
                                }

                                io = (InventoryObject *) 
                                    charInfoArray[curCharacterIndex].wield->objects.Next();
                            }

                        }
                    }
                    
                }
                else
                {
                    if (-1 == bladeToHit)
                    {
                        bladeToHit = 1;  // fist is minimal;
                        bladeDamage = 1;
                    }

                    // 1. chance = 2d20 + Blade * Physical + weapon.ToHit - monster.Defense
                    int cVal = bladeToHit - 
                                     curMonster->defense + PhysicalStat() +
                                     totemEffects.effect[TOTEM_ACCURACY];
                    int chance = (rand() % 20) + 2 + (rand() % 20) + cVal;

                    if (cVal + 40 <= 20) // if you can't EVER hit
                    {
                        if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
                            chance = 30;  // Hit!
                    }

                    // 2. if chance > 20, hit was successful
                    if (chance > 20)
                    {
                        // 3. damage = Physical * weapon.Damage
                        long damValue = (long)(bladeDamage * 
                                            (1 + PhysicalStat() * 0.15f) + 
                                             totemEffects.effect[TOTEM_STRENGTH]);

                        AddMastery( ss );	// Add to mastery level

                        long additional_stun = 0;


                        if (specLevel[0] > 0)
                            damValue = (long)(damValue * (1 + 0.05f * specLevel[0]));

                        if (iBlade && BLADE_TYPE_CHAOS == iBlade->type)
                        {
                            if (3 == rand() % 20)
                            {
                                damValue *= 3;

                                if (infoFlags & INFO_FLAGS_HITS) {
                                    sprintf( tempText,"With a burst of strength, you do THREE TIMES as much damage!.");
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                            else if (2 == rand() % 5)
                            {
                                damValue *= 2;

                                if (infoFlags & INFO_FLAGS_HITS) {
                                    sprintf( tempText,"With a burst of strength, you do TWICE as much damage!.");
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                        }

                        if (iBlade && BLADE_TYPE_CLAWS == iBlade->type)
                        {
                            if (8 == rand() % 10)
                            {
                                curMonster->MonsterMagicEffect(MONSTER_EFFECT_STUN, damValue * 50.0f, 1);
                                additional_stun = damValue * 50;

                                if (infoFlags & INFO_FLAGS_HITS) {
                                    sprintf( tempText,"The %s gets stunned by %s's Claw!", curMonster->Name(), charInfoArray[curCharacterIndex].name);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                        }

                        if (iBlade && BLADE_TYPE_DOUBLE == iBlade->type)
                        {
                            if (1 == rand() % 5)
                            {
                                DoBladestaffExtra(ss, iBlade, damValue, curMonster);
                            }
                        }

                        if (totemEffects.effect[TOTEM_LIFESTEAL] > 0)
                        {
                            int suck = damValue * totemEffects.effect[TOTEM_LIFESTEAL] / 40;
                            if (suck > charInfoArray[curCharacterIndex].healthMax/4)
                                suck = charInfoArray[curCharacterIndex].healthMax/4;
                            damValue += suck;

                            charInfoArray[curCharacterIndex].health += suck;

                            if (charInfoArray[curCharacterIndex].health >
                                        charInfoArray[curCharacterIndex].healthMax)
                                charInfoArray[curCharacterIndex].health =
                                        charInfoArray[curCharacterIndex].healthMax;
                        }

                        // Add mastery damage
                        if( iBlade )
                            damValue += ( ( GetMasteryForType( iBlade->type ) / 2 ) * charInfoArray[curCharacterIndex].physical );

                        curMonster->health -= damValue;
                        curMonster->RecordDamageForLootDist(damValue, this);

                        // age the blade
                        if (iBlade)
                        {
                            if (iBlade->numOfHits < 22000)
                                ++(iBlade->numOfHits);
                        }

                        // give experience for monster damage!

                        if (bladeHeal > 0)
                        {
                            // green dragons heal their owner.
                            charInfoArray[curCharacterIndex].health += bladeHeal;

                            if (charInfoArray[curCharacterIndex].health >
                                        charInfoArray[curCharacterIndex].healthMax)
                            {
                                charInfoArray[curCharacterIndex].health =
                                        charInfoArray[curCharacterIndex].healthMax;
                            }
                        }

                        if( bladeShock > 0 ) {
                            int cnt = iBlade->GetIngotCount();

                            if( cnt > 96 )
                                cnt = 96;

                            if( rand() % 144 < cnt && ( bladeShock / 2 ) ) {
                                curMonster->MonsterMagicEffect( MONSTER_EFFECT_STUN, bladeShock * 500.0f + additional_stun, 1.0f );

                                curMonster->health -= ( bladeShock / 2 );

                                if (infoFlags & INFO_FLAGS_HITS) {
                                    sprintf( tempText,"The %s gets shocked by %d lightning damage and is stunned.", 
                                        curMonster->Name(), ( bladeShock / 2 ) );
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                        }

                        if (curMonster->health <= 0)
                        {
                            MessAvatarAttack messAA;
                            messAA.avatarID = socketIndex;
                            messAA.mobID    = (long) curMonster;
                            messAA.damage   = damValue;
                            messAA.health   = -1000;
                            ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messAA), &messAA, 2);

                            curMonster->isDead = TRUE;
                            curMonster->bane = this;

                            curMonster->HandleQuestDeath();

                            if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                            {
                                MapListState oldState = ss->mobList->GetState();

                                BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                                army->MonsterEvent(curMonster, ARMY_EVENT_DIED);

                                ss->mobList->SetState(oldState);
                            }
                            else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                            {
                                MapListState oldState = ss->mobList->GetState();

                                BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                                quest->MonsterEvent(curMonster, AUTO_EVENT_DIED);

                                ss->mobList->SetState(oldState);
                            }
                        }
                        else
                        {
                            if (!(curMonster->curTarget) || 
                                 (24 == curMonster->type && 3 == (rand() % 10))
                                )
                            {
                                curMonster->lastAttackTime = now;
                                curMonster->curTarget = this;
                            }

                            if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                            {
                                MapListState oldState = ss->mobList->GetState();

                                BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                                army->MonsterEvent(curMonster, ARMY_EVENT_ATTACKED);

                                ss->mobList->SetState(oldState);
                            }
                            else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                            {
                                MapListState oldState = ss->mobList->GetState();

                                BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                                quest->MonsterEvent(curMonster, AUTO_EVENT_ATTACKED);

                                ss->mobList->SetState(oldState);
                            }

                            MessAvatarAttack messAA;
                            messAA.avatarID = socketIndex;
                            messAA.mobID    = (long) curMonster;
                            messAA.damage   = damValue;
                            messAA.health   = charInfoArray[curCharacterIndex].health;
                            messAA.healthMax= charInfoArray[curCharacterIndex].healthMax;
                            ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messAA), &messAA, 2);

                            MessMonsterHealth messMH;
                            messMH.mobID = (unsigned long)curMonster;
                            messMH.health = curMonster->health;
                            messMH.healthMax = curMonster->maxHealth;
                            ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messMH),(void *)&messMH, 2);

                            if (bladeSlow > 0)
                            {
                                // blue dragons slow the target.
                                int chance = bladeSlow;

                                if (chance + (rand() % 20) - curMonster->defense > 10)
                                {
                                    curMonster->MonsterMagicEffect(DRAGON_TYPE_BLUE, 
                                        bladeSlow * 1000.0f, bladeSlow);

                                    if (infoFlags & INFO_FLAGS_HITS)
                                    {
                                        sprintf(tempText,"The %s is Slowed.",
                                            curMonster->Name());
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                }
                            }

                            
                            if (bladePoison > 0)
                            {
                                int chance = bladePoison;

                                if (chance + (rand() % 20) - curMonster->defense > 10)
                                {
                                    curMonster->MonsterMagicEffect(DRAGON_TYPE_BLACK, 
                                        bladePoison * 1000.0f, bladePoison);

                                    curMonster->RecordDamageForLootDist(bladePoison * bladePoison / 40, this);

                                    if (infoFlags & INFO_FLAGS_HITS)
                                    {
                                        sprintf(tempText,"The %s is Poisoned.",
                                            curMonster->Name());
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                }
                            }
                            if (bladeBlind > 0)
                            {
                                int chance = bladeBlind;

                                if (chance + (rand() % 20) - curMonster->defense > 10)
                                {
                                    curMonster->MonsterMagicEffect(DRAGON_TYPE_WHITE, 
                                        bladeBlind * 1000.0f, bladeBlind);

                                    if (infoFlags & INFO_FLAGS_HITS)
                                    {
                                        sprintf(tempText,"The %s is Blinded.",
                                            curMonster->Name());
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                }
                            }
                        }

                    }
                    else
                    {
                        // whish!
                        MessAvatarAttack messAA;
                        messAA.avatarID = socketIndex;
                        messAA.mobID    = (long) curMonster;
                        messAA.damage   = -1; // miss!
                        messAA.health   = charInfoArray[curCharacterIndex].health;
                        messAA.healthMax= charInfoArray[curCharacterIndex].healthMax;
                        ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messAA), &messAA, 2);
                    }
                }
            }
            curMob = ss->mobList->GetNext();
        }

        if (-3 != usedStaff.charges)
        {
            curMob = ss->mobList->GetFirst(cellX, cellY, staffRange);
            while (curMob)
            {
                curMonster = (BBOSMonster *) curMob;

                if (SMOB_MONSTER == curMob->WhatAmI() &&
                     abs(cellX - curMob->cellX) <= staffRange &&
                     abs(cellY - curMob->cellY) <= staffRange)
                   UseStaffOnMonster(ss, &usedStaff, curMonster);

                curMob = ss->mobList->GetNext();
            }
        }

        if (!didAttack)
        {
            curTarget = NULL;
        }

    }

    for (int index = 0; index < 2; ++index)
    {
        PetDragonInfo *dInfo = &charInfoArray[curCharacterIndex].petDragonInfo[index];

        delta = now - dInfo->lastAttackTime;

        if (0 == dInfo->lastAttackTime || now < dInfo->lastAttackTime) // || now == moveStartTime)
        {
            delta = 1000 * 3 + 1;
        }

        // attack curTarget (if it's still there)
        if (255 != dInfo->type && delta > 1000 * 3 && curTarget &&
             !(dm && dm->CanEdit(this) && 0 == curTarget->form)
            )	
        {
            dInfo->lastAttackTime = now - rand() % 200;
            int didAttack = FALSE;

            curMob = ss->mobList->GetFirst(cellX, cellY);
            while (curMob && !didAttack)
            {
                if (!curMob->isDead && curMob == curTarget && 
                      curMob->cellX == cellX && curMob->cellY == cellY && 
                      SMOB_MONSTER == curMob->WhatAmI()) 
                {
                    didAttack = TRUE;

                    curMonster = (BBOSMonster *) curMob;

                    long damValue = (dInfo->lifeStage+1) * (dInfo->quality+1) * 
                        dragonInfo[dInfo->quality][dInfo->type].attackDamageBase / 5 +
                        dInfo->healthModifier * (dInfo->lifeStage+1); 

                    curMonster->health -= damValue;
                    curMonster->RecordDamageForLootDist(damValue, this);

                    if (!(curMonster->curTarget) || 
                                 (24 == curMonster->type && 3 == (rand() % 10))
                         )
                    {
                        curMonster->lastAttackTime = now;
                        curMonster->curTarget = this;
                    }

                    // give experience for monster damage!
                    int breathType = dInfo->type;
                    if (6 == breathType)
                        breathType = rand() % 6;

                    MessPetAttack messAA;
                    messAA.avatarID = socketIndex;
                    messAA.mobID    = (long) curMonster;
                    messAA.damage   = damValue;
                    messAA.which    = index;
                    messAA.type     = breathType;
                    messAA.effect   = 0;
                    ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messAA), &messAA, 2);

                    MessMonsterHealth messMH;
                    messMH.mobID = (unsigned long)curMonster;
                    messMH.health = curMonster->health;
                    messMH.healthMax = curMonster->maxHealth;
                    ss->SendToEveryoneNearBut(0, (float)cellX, (float)cellY, sizeof(messMH),(void *)&messMH, 2);

                    if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                    {
                        MapListState oldState = ss->mobList->GetState();

                        BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                        army->MonsterEvent(curMonster, ARMY_EVENT_ATTACKED);

                        ss->mobList->SetState(oldState);
                    }
                    else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                    {
                        MapListState oldState = ss->mobList->GetState();

                        BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                        quest->MonsterEvent(curMonster, AUTO_EVENT_ATTACKED);

                        ss->mobList->SetState(oldState);
                    }

                    if (curMonster->health <= 0)
                    {
                        curMonster->isDead = TRUE;
                        curMonster->bane = this;

                        curMonster->HandleQuestDeath();

                        // give experience for monster death!
                        if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                        {
                            MapListState oldState = ss->mobList->GetState();

                            BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                            army->MonsterEvent(curMonster, ARMY_EVENT_DIED);

                            ss->mobList->SetState(oldState);
                        }
                        else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                        {
                            MapListState oldState = ss->mobList->GetState();

                            BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                            quest->MonsterEvent(curMonster, AUTO_EVENT_DIED);

                            ss->mobList->SetState(oldState);
                        }
                    }
                    else if (DRAGON_TYPE_GREEN == breathType)
                    {
                        // green dragons heal their owner.
                        charInfoArray[curCharacterIndex].health += 
                            (dInfo->lifeStage+1) * (dInfo->quality+1) / 2 +
                        dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (charInfoArray[curCharacterIndex].health >
                                    charInfoArray[curCharacterIndex].healthMax)
                        {
                            charInfoArray[curCharacterIndex].health =
                                    charInfoArray[curCharacterIndex].healthMax;
                        }
                    }
                    else if (DRAGON_TYPE_BLUE == breathType)
                    {
                        // blue dragons slow the target.
                        int chance = (dInfo->lifeStage+1) * (dInfo->quality+1) +
                            dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (chance + (rand() % 20) - curMonster->defense > 10)
                        {
                            curMonster->MonsterMagicEffect(DRAGON_TYPE_BLUE, 
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 2000.0f,
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 1.0f);

                            if (infoFlags & INFO_FLAGS_HITS)
                            {
                                sprintf(tempText,"%s slows the %s.",
                                    dInfo->name, curMonster->Name());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                    else if (DRAGON_TYPE_BLACK == breathType)
                    {
                        // black dragons poison the target.
                        int chance = (dInfo->lifeStage+1) * (dInfo->quality+1) +
                            dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (chance + (rand() % 20) - curMonster->defense > 10)
                        {
                            curMonster->MonsterMagicEffect(DRAGON_TYPE_BLACK, 
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 2000.0f,
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 1.0f);

                            curMonster->RecordDamageForLootDist(
                                          (dInfo->lifeStage+1) * (dInfo->quality+1) / 20, this);

                            if (infoFlags & INFO_FLAGS_HITS)
                            {
                                sprintf(tempText,"%s poisons the %s.",
                                    dInfo->name, curMonster->Name());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                    else if (DRAGON_TYPE_WHITE == breathType)
                    {
                        // white dragons blind the target.
                        int chance = (dInfo->lifeStage+1) * (dInfo->quality+1) +
                            dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (chance + (rand() % 20) - curMonster->defense > 10)
                        {
                            curMonster->MonsterMagicEffect(DRAGON_TYPE_WHITE, 
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 2000.0f,
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 1.0f);

                            if (infoFlags & INFO_FLAGS_HITS)
                            {
                                sprintf(tempText,"%s blinds the %s.",
                                    dInfo->name, curMonster->Name());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                    else if (DRAGON_TYPE_GOLD == breathType)
                    {
                        // heal their owner.
                        charInfoArray[curCharacterIndex].health += 
                            (dInfo->lifeStage+1) * (dInfo->quality+1) / 2 +
                        dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (charInfoArray[curCharacterIndex].health >
                                    charInfoArray[curCharacterIndex].healthMax)
                        {
                            charInfoArray[curCharacterIndex].health =
                                    charInfoArray[curCharacterIndex].healthMax;
                        }

                        // blind the target.
                        int chance = (dInfo->lifeStage+1) * (dInfo->quality+1) / 1.0f +
                            dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (chance + (rand() % 20) - curMonster->defense > 10)
                        {
                            curMonster->MonsterMagicEffect(DRAGON_TYPE_WHITE, 
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 1000 * 2,
                                (dInfo->lifeStage+1) * (dInfo->quality+1) );

                            if (infoFlags & INFO_FLAGS_HITS)
                            {
                                sprintf(tempText,"%s blinds the %s.",
                                    dInfo->name, curMonster->Name());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }

                        // slow the target.
                        chance = (dInfo->lifeStage+1) * (dInfo->quality+1) / 1.0f +
                            dInfo->healthModifier * (dInfo->lifeStage+1); 

                        if (chance + (rand() % 20) - curMonster->defense > 10)
                        {
                            curMonster->MonsterMagicEffect(DRAGON_TYPE_BLUE, 
                                (dInfo->lifeStage+1) * (dInfo->quality+1) * 1000 * 2,
                                (dInfo->lifeStage+1) * (dInfo->quality+1) );

                            if (infoFlags & INFO_FLAGS_HITS)
                            {
                                sprintf(tempText,"%s slows the %s.",
                                    dInfo->name, curMonster->Name());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                }
                curMob = ss->mobList->GetNext();
            }

        }
    }

    delta = now - lastSaveTime;

    if (0 == lastSaveTime || now < lastSaveTime)
    {
        lastSaveTime = timeGetTime();
        delta = 1;
    }

    // save character
    if (delta > 1000 * 60 * 5)	
    {
        lastSaveTime = now;
        if (SPACE_GROUND != ss->WhatAmI())
        {
            charInfoArray[curCharacterIndex].lastX = charInfoArray[curCharacterIndex].spawnX;
            charInfoArray[curCharacterIndex].lastY = charInfoArray[curCharacterIndex].spawnY;
        }
        else
        {
            charInfoArray[curCharacterIndex].lastX = cellX;
            charInfoArray[curCharacterIndex].lastY = cellY;
        }

        ++(charInfoArray[curCharacterIndex].lifeTime);
        ++activeCounter;

        if (!tradingPartner && bHasLoaded) // postpone saving until the transaction is finished
            SaveAccount();

        HandleMeatRot(ss);
        PetAging(ss);

        QuestTime(ss);

        ++timeOnCounter;


        /*
        NO LONGER REQUIRING ACCOUNT PAYMENTS

        LongTime rightNow;
        long timeDiff = rightNow.MinutesDifference(&accountExperationTime);
        if (timeDiff <= 0 && timeOnCounter > 1)
            kickMeOffNow = 1;
        else if (timeDiff <= 60 * 24 && 1 == timeOnCounter % 6)
        {
            sprintf(tempText,"Your account will expire in %ld minutes.",timeDiff);
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            sprintf(tempText,"Go to www.blademistress.com to purchase a code for more time.");
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        }
        */
    }
    
    if (chantType > -1)
    {
        delta = now - chantTime;

        // stop chanting
        if (delta > 1000 * 10) // 10 seconds	
        {
            chantType = -1;

            MessChant messChant;
            messChant.avatarID = socketIndex;
            messChant.r = messChant.b = messChant.g = 0;	 // black means stop
            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messChant), &messChant);
        }
    }

    // ********healing
    delta = now - lastHealTime;

    if (0 == lastHealTime || now < lastHealTime)
    {
        delta = 1000 * 5 + 1;
    }


    // heal damage
    if (delta > 1000 * 5)	
    {
        float tHeal = 0.0f;
            
        if (curTarget)
        {
            tHeal = 0.3f * totemEffects.effect[TOTEM_HEALING];

            if (tHeal > charInfoArray[curCharacterIndex].healthMax / 4.0f)
                tHeal = charInfoArray[curCharacterIndex].healthMax / 4.0f;
        }
        else
            tHeal = charInfoArray[curCharacterIndex].healthMax / 6.0f;

        lastHealTime = now;
        charInfoArray[curCharacterIndex].health += 0.7f + tHeal;
        if (charInfoArray[curCharacterIndex].health >
             charInfoArray[curCharacterIndex].healthMax)
            charInfoArray[curCharacterIndex].health = charInfoArray[curCharacterIndex].healthMax;

        MessAvatarHealth messHealth;
        messHealth.health    = charInfoArray[curCharacterIndex].health;
        messHealth.healthMax = charInfoArray[curCharacterIndex].healthMax;
        messHealth.avatarID  = socketIndex;
        ss->lserver->SendMsg(sizeof(messHealth),(void *)&messHealth, 0, &tempReceiptList);

        while (QuestReward(ss))
            ;

    }

    //********** 10 second tasks
    delta = now - lastTenTime;

    if (0 == lastTenTime || now < lastTenTime)
    {
        delta = 1000 * 10 + 1;
    }

    // heal damage
    if (delta > 1000 * 10)	
    {
        lastTenTime = now;

        for (int i = 0; i < MONSTER_EFFECT_TYPE_NUM; ++i)
        {
            if (magicEffectAmount[i] > 0)
            {
                if (magicEffectTimer[i] < now)
                    magicEffectAmount[i] = 0;
                else if (MONSTER_EFFECT_TYPE_BLACK == i)
                {
                    /*
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
                    */
                }

            }
        }

        // find out if there's another player on with my exact name
        BBOSAvatar *other = (BBOSAvatar *)ss->avatars->First();
        {
            if (other != this)
            {
                if (IsCompletelyVisiblySame(other->name, name))
                {
                    kickOff = TRUE;
                    other->kickOff = TRUE;
                }
            }
            other = (BBOSAvatar *)ss->avatars->Next();
        }

        ss->avatars->Find(this);

        // piggy-back the admin info message on healing
        if (ACCOUNT_TYPE_ADMIN == accountType)
        {
            MessAdminInfo aInfo;
            DWORD timeSinceConnect = (timeGetTime() - bboServer->lastConnectTime) / 1000 / 60;
            aInfo.numPlayers = lastAvatarCount;
            aInfo.lastConnectTime = timeSinceConnect;
            ss->lserver->SendMsg(sizeof(aInfo),(void *)&aInfo, 0, &tempReceiptList);
        }
/*
        InvSkill *skillInfo = NULL;
        InventoryObject *io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
        while (io)
        {
            if (!strcmp("Dodging",io->WhoAmI()))
            {
                skillInfo = (InvSkill *) io->extra;

                if (skillInfo->skillLevel * skillInfo->skillLevel * 100 <= skillInfo->skillPoints)
                {
                    // made a skill level!!!
                    skillInfo->skillLevel++;
                    sprintf(tempText,"You gained Dodging skill!!");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

            }
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
        }
*/
    }
    
    if (isCombining)
    {
        delta = now - combineStartTime;

        if (0 == combineStartTime || now < combineStartTime) // || now == combineStartTime)
        {
            delta = 1000 * 5 + 1;
        }

//		curTarget = FALSE;
        // Finish the combine
        if (delta > 1000 * 5)	
        {
            isCombining = FALSE;

            Combine(ss);
        }
    }

    delta = now - lastMinuteTime;

    if (0 == lastMinuteTime || now < lastMinuteTime)
    {
        delta = 1000 * 60 + 1;
    }

    // general checks each minute
    if (delta > 1000 * 60)	
    {
        lastMinuteTime = now;

        specLevel[0] = specLevel[1] = specLevel[2] = 0;

        // find guild
        SharedSpace *sx;
        if(bboServer->FindAvatarInGuild(charInfoArray[curCharacterIndex].name, &sx))
        {
            specLevel[0] = ((TowerMap *) sx)->specLevel[0];
            specLevel[1] = ((TowerMap *) sx)->specLevel[1];
            specLevel[2] = ((TowerMap *) sx)->specLevel[2];
        }

        for (int i = 0; i < TOTEM_MAX; ++i)
            totemEffects.effect[i] = 0;


        // see if any activated totems have expired
        InventoryObject *io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].inventory->objects.First();
        while (io)
        {
            if (INVOBJ_TOTEM == io->type)
            {
                InvTotem *it = (InvTotem *) io->extra;
                LongTime ltNow;
                if (it->isActivated && it->timeToDie.MinutesDifference(&ltNow) >= 0)
                {
                    sprintf(tempText,"%s disintegrates.",io->WhoAmI());
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].inventory->objects.Remove(io);
                    delete io;
                }
/*				else if (it->isActivated)
                {
                    int value = it->quality + 1 - it->imbueDeviation;
                    if (value < 0)
                        value = 0;
                    if (value > totemEffects.effect[it->type])
                        totemEffects.effect[it->type] = value;
                }
                */
            }
            io = (InventoryObject *) 
                charInfoArray[curCharacterIndex].inventory->objects.Next();
        }

        // check workbench for any activated totems that have expired, too
        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_TOTEM == io->type)
            {
                InvTotem *it = (InvTotem *) io->extra;
                LongTime ltNow;
                if (it->isActivated && it->timeToDie.MinutesDifference(&ltNow) >= 0)
                {
                    sprintf(tempText,"%s disintegrates.",io->WhoAmI());
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                    delete io;
                }
/*				else if (it->isActivated)
                {
                    int value = it->quality - it->imbueDeviation;
                    if (value < 0)
                        value = 0;
                    if (value > totemEffects.effect[it->type])
                        totemEffects.effect[it->type] = value;
                }
                */
            }
            io = (InventoryObject *) 
                charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        // check wield for any activated totems that have expired, too
        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.First();
        while (io)
        {
            if (INVOBJ_TOTEM == io->type)
            {
                InvTotem *it = (InvTotem *) io->extra;
                LongTime ltNow;
                if (it->isActivated && it->timeToDie.MinutesDifference(&ltNow) >= 0)
                {
                    sprintf(tempText,"%s disintegrates.",io->WhoAmI());
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].wield->objects.Remove(io);
                    delete io;
                }
                else if (it->isActivated)
                {
                    int value = it->quality - it->imbueDeviation;
                    if (value < 0)
                        value = 0;

                    if (it->type >= TOTEM_PHYSICAL && it->type <= TOTEM_CREATIVE)
                        value = it->imbueDeviation;

                    if (value > totemEffects.effect[it->type])
                        totemEffects.effect[it->type] = value;
                }
            }
            io = (InventoryObject *) 
                charInfoArray[curCharacterIndex].wield->objects.Next();
        }

        // see if any skills have gone up
        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].skills->objects.First();
        while (io)
        {
            if (!strcmp("Dodging",io->WhoAmI()))
            {
                InvSkill *skillInfo = (InvSkill *) io->extra;

                if (skillInfo->skillLevel * skillInfo->skillLevel * 100 <= skillInfo->skillPoints)
                {
                    // made a skill level!!!
                    skillInfo->skillLevel++;
                    sprintf(tempText,"You gained Dodging skill!!");
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_DODGE;
                }

            }
            io = (InventoryObject *) 
                charInfoArray[curCharacterIndex].skills->objects.Next();
        }

        charInfoArray[curCharacterIndex].healthMax = 20 + PhysicalStat() * 6 + charInfoArray[curCharacterIndex].cLevel; 

        if( charInfoArray[curCharacterIndex].age < 6 )
            charInfoArray[curCharacterIndex].healthMax *= charInfoArray[curCharacterIndex].age;
        else
            charInfoArray[curCharacterIndex].healthMax *= 5;

        if ((long)charInfoArray[curCharacterIndex].oldCLevel != 
             (long)charInfoArray[curCharacterIndex].cLevel)
        {

            MessAvatarStats mStats;
            BuildStatsMessage(&mStats);
            ss->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);

            sprintf(tempText,"You gained a Level!!  You are now Level %ld.", 
                        (long) charInfoArray[curCharacterIndex].cLevel);
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            MessGenericEffect messGE;
            messGE.avatarID = socketIndex;
            messGE.mobID    = -1;
            messGE.x        = cellX;
            messGE.y        = cellY;
            messGE.r        = 40;
            messGE.g        = 255;
            messGE.b        = 40;
            messGE.type     = 0;  // type of particles
            messGE.timeLen  = 5; // in seconds
            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messGE),(void *)&messGE);

            charInfoArray[curCharacterIndex].oldCLevel = 
                 charInfoArray[curCharacterIndex].cLevel;
        }


    }

}

//******************************************************************
// returns 0 if success, 1 if already made, 3 if password is wrong, -1 if bad string, 2 otherwise
int BBOSAvatar::LoadAccount(char *n, char *p, int isNew, int justLoad)
{
    assert(this);
    assert(n);
    if (!justLoad)
        assert(p);

    if (NULL == p || NULL == n)
        return -1;

	const size_t nLen = strlen(n);
	const size_t pLen = strlen(p);
    if (	nLen <= 0 ||
			nLen >= NUM_OF_CHARS_FOR_USERNAME ||
			pLen >= NUM_OF_CHARS_FOR_PASSWORD ||
			pLen <= 0 )
        return -1;

    int i,r,g,b;
    char fileName[64], tempText[1024];

    // remove leading whitespace
    int startChar = 0;
    while (' ' == n[startChar])
        ++startChar;

    int nameStart = startChar;

    if (	!(n[startChar] >= 'a' && n[startChar] <= 'z') &&
            !(n[startChar] >= 'A' && n[startChar] <= 'Z') &&
            !(n[startChar] >= '0' && n[startChar] <= '9')	)
        return -1;

    char dir[2] = {n[startChar], '\0'};

    if (justLoad)
        sprintf(fileName, n);
    else
        sprintf(fileName, "users\\%s\\%s.use", &dir, &(n[startChar]));

    LogOutput("loadInfo.dat", "Attempting a load of ");
    LogOutput("loadInfo.dat", fileName);
    LogOutput("loadInfo.dat", "\n");

    startChar = 0;
    while (0 != fileName[startChar])
    {
        if (' ' == fileName[startChar])
            fileName[startChar] = '-';

        ++startChar;
    }

    FILE *fp;
    if (justLoad)
    {
        fp = fopen(fileName,"r");
        if (!fp)
            return 10;
    }
    else
    {
        fp = fopen(fileName,"r");
        if (fp && isNew)
        {
            fclose(fp);
            return 1;
        }
        if (!fp && !isNew)
            return 4;

        if (!fp)
        {
            // creating a new user
            startChar = 0;
            while (' ' == p[startChar])
                ++startChar;

            int passStart = startChar;

            // Clean this up
            char cCurrentPath[FILENAME_MAX];
            _getcwd(cCurrentPath, sizeof(cCurrentPath));

            // Create the directory hierarchy
            char dirHierarchy[FILENAME_MAX];
            sprintf_s(dirHierarchy, "%s\\users\\%s\\", cCurrentPath, &dir);
            SHCreateDirectoryEx(NULL, dirHierarchy, NULL);

            fp = fopen(fileName,"w");
            if (!fp)
                return 2;

            fprintf(fp,"%f\n",VERSION_NUMBER);

            // Save hashed password out
            unsigned char hash[OUT_HASH_SIZE];
            if (!PasswordHash::CreateSerializableHash((const unsigned char*) &p[passStart], hash))
            {
                fclose(fp);
                return 2;
            }

            fprintf(fp,"%s\n%s\n",&n[nameStart], &hash[0]);
            fprintf(fp,"0 -1 0 0 0\n"); // admin flag, info flags, chat chans, invuln, hasPaid

            fprintf(fp,"0 0\n"); // isReferralDone, patronCount;

            fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
                accountExperationTime.value.wYear,
                accountExperationTime.value.wMonth,
                accountExperationTime.value.wDay,
                accountExperationTime.value.wDayOfWeek,
                accountExperationTime.value.wHour,
                accountExperationTime.value.wMinute,
                accountExperationTime.value.wSecond,
                accountExperationTime.value.wMilliseconds);

            for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
            {
                fprintf(fp,"EMPTY\n-1 0 0 0\n");
                fprintf(fp,"-1 0 0 0\n");
                fprintf(fp,"-1 0 0 0\n");
                fprintf(fp,"0.0\n");	 // cLevel
                fprintf(fp,"50 50\n");
                fprintf(fp,"4 4 4\n");
                fprintf(fp,"100 50 100 50\n"); // last x,y, spawn x,y
                fprintf(fp,"0 0 1\n"); // special drawing flags, lifeTime, age

                fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
                    accountExperationTime.value.wYear,
                    accountExperationTime.value.wMonth,
                    accountExperationTime.value.wDay,
                    accountExperationTime.value.wDayOfWeek,
                    accountExperationTime.value.wHour,
                    accountExperationTime.value.wMinute,
                    accountExperationTime.value.wSecond,
                    accountExperationTime.value.wMilliseconds);

                for (int dIndex = 0; dIndex < 2; ++dIndex)
                {
                    fprintf(fp,"NO PET\n");
                    fprintf(fp,"255 0 0 0 0 0 0 0.0\n");
                }

                for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
                {
                    for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
                    {
                        fprintf(fp," 0 0");
                    }
                }
                fprintf(fp,"\n");

                // quests
                fprintf(fp,"0 -1 -1\nEMPTY\n0 -1 -1\nEMPTY\n0 -1 -1\nEMPTY\n");
                fprintf(fp,"0 -1 -1\nEMPTY\n0 -1 -1\nEMPTY\n0 -1 -1\nEMPTY\n");
                fprintf(fp,"0\nNOWITCH\n");  // which quest for which witch

                fprintf(fp,"100\nXYZENDXYZ\n");  // end of objects list
                fprintf(fp," 00\nXYZENDXYZ\n");  // end of workbench list
                fprintf(fp," 00\nXYZENDXYZ\n");  // end of skills list
                fprintf(fp," 00\nXYZENDXYZ\n");  // end of wield list
            }

            fprintf(fp," 00\nXYZENDXYZ\n");  // end of bank list
            fprintf(fp," 00\nXYZENDXYZ\n");  // end of contacts list

            fclose(fp);
            fp = fopen(fileName,"r");
            if (!fp)
                return 2;
        }
    }
    // loading an existing user
    float fileVersionNumber;
    fscanf(fp,"%f\n",&fileVersionNumber);

    // check here to make sure this is the correct version!

    // Use the longer out hash size as that is the string that was written out in new versions
    unsigned char tempName[NUM_OF_CHARS_FOR_USERNAME], tempPass[OUT_HASH_SIZE];
    LoadLineToString(fp, tempText);
    CopyStringSafely(tempText, 1024, (char*)&tempName[0], NUM_OF_CHARS_FOR_USERNAME);
    RemoveStringTrailingSpaces((char*)&tempName[0]);

	LoadLineToString(fp, tempText);

	if (fileVersionNumber < 2.6f)
	{
		// We need to hash
		unsigned char salt[256];
		sprintf_s((char*)&salt[0], 256, "%s-%s", "BladeMistress", tempName);

		// Hash the password
		// Need to add 1 to null terminate for the CreateSerializableHash below
		unsigned char hashPass[HASH_BYTE_SIZE + 1] = { 0 };
		if (!PasswordHash::CreateStandaloneHash((const unsigned char*)tempText, salt, 6969, hashPass))
		{
			fclose(fp);
			return 2;
		}

		if (!PasswordHash::CreateSerializableHash(hashPass, (unsigned char*)&tempPass[0]))
		{
			fclose(fp);
			return 2;
		}
	}
	else
	{
		// Password is already the serialized hash
		CopyStringSafely(tempText, 1024, (char*)&tempPass[0], OUT_HASH_SIZE);
	}
    
//	if (!justLoad)
//	{
//		sprintf(pass,p);
//		sprintf(name,n);
//	}
//	else
    {
		// Validate hashed password
		if (!PasswordHash::ValidateSerializablePassword((unsigned char*)p, tempPass))
		{
			fclose(fp);
			return 3;
		}

		sprintf_s(pass, NUM_OF_CHARS_FOR_PASSWORD, "%s", tempPass);
        sprintf_s(name, NUM_OF_CHARS_FOR_USERNAME, "%s", n);

        passLen = strlen((char*)&tempPass[0]);
        assert(0 < passLen && passLen <= OUT_HASH_SIZE);
    }

    isInvulnerable = hasPaid = 0;

    chatChannels = 0;

    if (fileVersionNumber >= 1.82f)
        fscanf(fp,"%d %ld %ld %d %d\n",&accountType, &infoFlags, &chatChannels, &isInvulnerable, &hasPaid);
    else if (fileVersionNumber >= 1.78f)
        fscanf(fp,"%d %ld %d %d\n",&accountType, &infoFlags, &isInvulnerable, &hasPaid);
    else if (fileVersionNumber >= 1.41f)
        fscanf(fp,"%d %ld\n",&accountType, &infoFlags);
    else if (fileVersionNumber >= 1.15f)
        fscanf(fp,"%d\n",&accountType);
    else
    {
        accountType = ACCOUNT_TYPE_PLAYER;
    }

    if (ACCOUNT_TYPE_BANNED == accountType)
    {
        fclose(fp);
        return 10;  // banned
    }

    if (fileVersionNumber >= 2.13f)
    {
        fscanf(fp,"%d %d\n", &isReferralDone, &patronCount);
    }
    else if (hasPaid)
    {
        isReferralDone = TRUE; // existing paid accounts don't get to refer
    }

    if (fileVersionNumber >= 1.77f)
    {
        int tempInt;

        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wYear         = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wMonth        = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wDay          = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wDayOfWeek    = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wHour         = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wMinute       = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountExperationTime.value.wSecond       = tempInt;
        fscanf(fp,"%d\n", &tempInt);
        accountExperationTime.value.wMilliseconds = tempInt;
    }

    accountRestrictionTime.SetToNow();
    restrictionType = 0;
    if (fileVersionNumber >= 1.89f)
    {
        int tempInt;

        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wYear         = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wMonth        = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wDay          = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wDayOfWeek    = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wHour         = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wMinute       = tempInt;
        fscanf(fp,"%d", &tempInt);
        accountRestrictionTime.value.wSecond       = tempInt;
        fscanf(fp,"%d\n", &tempInt);
        accountRestrictionTime.value.wMilliseconds = tempInt;

        fscanf(fp,"%d\n", &restrictionType);
    }

#ifdef _TEST_SERVER
    // Game is free now!
    /*
    LongTime now;
    if( !hasPaid )
    {
        fclose(fp);
        return 11;
    }
    */
#endif

    for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
    {
        LoadLineToString(fp, tempText);
        CopyStringSafely(tempText,1024,charInfoArray[i].name,32);
        CleanString(charInfoArray[i].name);
        RemoveStringTrailingSpaces(charInfoArray[i].name);

//		fscanf(fp,"%d\n", &(charInfoArray[i].artIndex));

        if (fileVersionNumber < 1.10f)
        {
            fscanf(fp,"%d %d %d %d\n", 
                         &(charInfoArray[i].topIndex), &r, &g, &b);
            charInfoArray[i].topR = charInfoArray[i].hairR = charInfoArray[i].bottomR = r;
            charInfoArray[i].topG = charInfoArray[i].hairG = charInfoArray[i].bottomG = g;
            charInfoArray[i].topB = charInfoArray[i].hairB = charInfoArray[i].bottomB = b;

            if (charInfoArray[i].topIndex >= NUM_OF_TOPS)
            charInfoArray[i].topIndex = NUM_OF_TOPS - 1;
            charInfoArray[i].faceIndex = 0;
            charInfoArray[i].bottomIndex = 0;
        }
        else
        {
            fscanf(fp,"%d %d %d %d\n", 
                         &(charInfoArray[i].topIndex), &r, &g, &b);
            charInfoArray[i].topR = r;
            charInfoArray[i].topG = g;
            charInfoArray[i].topB = b;

            if (!strcmp(charInfoArray[i].name, "EMPTY"))
                charInfoArray[i].topIndex = -1;

            fscanf(fp,"%d %d %d %d\n", 
                         &(charInfoArray[i].bottomIndex), &r, &g, &b);
            charInfoArray[i].bottomR = r;
            charInfoArray[i].bottomG = g;
            charInfoArray[i].bottomB = b;
            fscanf(fp,"%d %d %d %d\n", 
                         &(charInfoArray[i].faceIndex), &r, &g, &b);
            charInfoArray[i].hairR = r;
            charInfoArray[i].hairG = g;
            charInfoArray[i].hairB = b;
        }

        if (fileVersionNumber >= 2.00f)
            fscanf(fp,"%f\n",&(charInfoArray[i].cLevel));

        if (fileVersionNumber >= 2.51f)
            fscanf(fp,"%ld %ld\n",&(charInfoArray[i].health), &(charInfoArray[i].healthMax));
        else
            fscanf(fp,"%f %ld\n",&(charInfoArray[i].health), &(charInfoArray[i].healthMax));

        fscanf(fp,"%ld %ld %ld\n",&(charInfoArray[i].physical), 
                                   &(charInfoArray[i].magical), 
                                          &(charInfoArray[i].creative));

        if (fileVersionNumber >= 1.15f)
            fscanf(fp,"%d %d %d %d\n",&(charInfoArray[i].lastX), &(charInfoArray[i].lastY),
                   &(charInfoArray[i].spawnX), &(charInfoArray[i].spawnY));
        else
        {
            charInfoArray[i].lastX = 100;
            charInfoArray[i].lastY = 50;
            charInfoArray[i].spawnX = 100;
            charInfoArray[i].spawnY = 50;
        }

        if (fileVersionNumber >= 2.13f)
        {
            long tempLong;
            fscanf(fp,"%ld %ld %d\n",&tempLong, &(charInfoArray[i].lifeTime),
                                      &(charInfoArray[i].age));
            charInfoArray[i].imageFlags = (unsigned short) tempLong;
        }
        else if (fileVersionNumber >= 1.24f)
        {
            long tempLong;
            fscanf(fp,"%ld %ld\n",&tempLong, &(charInfoArray[i].lifeTime));
            charInfoArray[i].imageFlags = (unsigned short) tempLong;
        }
        else if (fileVersionNumber >= 1.23f)
        {
            long tempLong;
            fscanf(fp,"%ld\n",&tempLong);
            charInfoArray[i].imageFlags = (unsigned short) tempLong;
            charInfoArray[i].lifeTime = 0;
        }
        else
        {
            charInfoArray[i].imageFlags = 0;
            charInfoArray[i].lifeTime = 0;
        }

        charInfoArray[i].lastSavedTime.SetToNow();
        if (fileVersionNumber >= 1.89f)
        {
            int tempInt;

            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wYear         = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wMonth        = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wDay          = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wDayOfWeek    = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wHour         = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wMinute       = tempInt;
            fscanf(fp,"%d", &tempInt);
            charInfoArray[i].lastSavedTime.value.wSecond       = tempInt;
            fscanf(fp,"%d\n", &tempInt);
            charInfoArray[i].lastSavedTime.value.wMilliseconds = tempInt;
        }

        if (fileVersionNumber >= 1.33f)
        {

            for (int dIndex = 0; dIndex < 2; ++dIndex)
            {
                LoadLineToString(fp, tempText);
                CopyStringSafely(tempText, 1024, charInfoArray[i].petDragonInfo[dIndex].name, 16);

                int temp1, temp2, temp3, temp4;

                fscanf(fp,"%d %d %d %d",&temp1, &temp2, &temp3, &temp4);
                charInfoArray[i].petDragonInfo[dIndex].type      = temp1;
                charInfoArray[i].petDragonInfo[dIndex].quality   = temp2;
                charInfoArray[i].petDragonInfo[dIndex].state     = temp3;
                charInfoArray[i].petDragonInfo[dIndex].lifeStage = temp4;

                fscanf(fp,"%ld %d %d %f\n",
                         &charInfoArray[i].petDragonInfo[dIndex].age,
                          &temp2, 
                          &temp3, 
                         &charInfoArray[i].petDragonInfo[dIndex].healthModifier);

              charInfoArray[i].petDragonInfo[dIndex].lastEatenType    = temp2;
                charInfoArray[i].petDragonInfo[dIndex].lastEatenSubType = temp3;

                if (!strncmp(".0", charInfoArray[i].petDragonInfo[dIndex].name, 2))
                    charInfoArray[i].petDragonInfo[dIndex].type = 255;
                if (!strncmp("0", charInfoArray[i].petDragonInfo[dIndex].name, 1))
                    charInfoArray[i].petDragonInfo[dIndex].type = 255;
            }
        }
        else
        {
            charInfoArray[i].petDragonInfo[0].type      = 255; // no dragon
            charInfoArray[i].petDragonInfo[1].type      = 255; // no dragon
            sprintf(charInfoArray[i].petDragonInfo[0].name,"NO PET");
            sprintf(charInfoArray[i].petDragonInfo[1].name,"NO PET");
        }

        charInfoArray[i].petDragonInfo[0].lastEatenTime.SetToNow();
        charInfoArray[i].petDragonInfo[1].lastEatenTime.SetToNow();


        if (fileVersionNumber >= 1.79f)
        {
            for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
            {
                for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
                {
                    fscanf(fp,"%d %d", &charInfoArray[i].karmaGiven[j][k], &charInfoArray[i].karmaReceived[j][k]);
                }
            }
        }

        if (fileVersionNumber >= 1.93f)
        {
            if (fileVersionNumber < 2.06f)
            {
                for (int j = 0; j < 3; ++j)
                {
                    charInfoArray[i].quests[j].Load(fp, fileVersionNumber);
                }
            }
            else
            {
                for (int j = 0; j < QUEST_SLOTS; ++j)
                {
                    charInfoArray[i].quests[j].Load(fp, fileVersionNumber);
                }
                if (fileVersionNumber >= 2.11f)
                {
                    fscanf(fp,"%d\n", &charInfoArray[i].witchQuestIndex);
                    LoadLineToString(fp, charInfoArray[i].witchQuestName);
                }
            }
        }
        charInfoArray[i].inventory->InventoryLoad(fp, fileVersionNumber);
        charInfoArray[i].workbench->InventoryLoad(fp, fileVersionNumber);
        charInfoArray[i].skills->InventoryLoad(fp, fileVersionNumber);
        charInfoArray[i].wield->InventoryLoad(fp, fileVersionNumber);

//		if (fileVersionNumber < 2.00f)
        {
            // recalculation of cLevel allows us to change the CLEVEL_VAL_ constants
            // and fix balancing issues, including taking levels away from people.

            charInfoArray[i].cLevel = 0;

            InventoryObject *io = (InventoryObject *) 
                charInfoArray[i].skills->objects.First();
            while (io)
            {
                InvSkill *skillInfo = (InvSkill *) io->extra;

                if (!strcmp("Dodging",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_DODGE;
                if (!strcmp("Explosives",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_BOMB;
                if (!strcmp("Swordsmith",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH;
                if (!strcmp("Weapon Dismantle",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH;

                if (!strcmp("Katana Expertise",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH_EXPERT;
                if (!strcmp("Chaos Expertise",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH_EXPERT;
                if (!strcmp("Mace Expertise",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH_EXPERT;
                if (!strcmp("Bladestaff Expertise",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH_EXPERT;
                if (!strcmp("Claw Expertise",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SMITH_EXPERT;

                if (!strcmp("Bear Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Wolf Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Eagle Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Snake Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Frog Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Sun Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Moon Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Turtle Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Evil Magic",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC;
                if (!strcmp("Geomancy",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_MAGIC * 2;
                if (!strcmp("Totem Shatter",io->WhoAmI()))
                    charInfoArray[i].cLevel += skillInfo->skillLevel * CLEVEL_VAL_SHATTER;
                
                io = (InventoryObject *) 
                    charInfoArray[i].skills->objects.Next();
            }
        }

        if (fileVersionNumber < 1.55f)
        {
            charInfoArray[i].inventory->money = 0;

            InventoryObject *io = (InventoryObject *) 
                charInfoArray[i].skills->objects.First();
            while (io)
            {
                if (!strcmp("Dodging",io->WhoAmI()))
                {
                    InvSkill *skillInfo = (InvSkill *) io->extra;
                    charInfoArray[i].inventory->money = 
                             skillInfo->skillLevel * skillInfo->skillLevel * 500;
                }
                io = (InventoryObject *) 
                    charInfoArray[i].skills->objects.Next();
            }

        }


        charInfoArray[i].healthMax = 20 + charInfoArray[i].physical * 6 + charInfoArray[i].cLevel;
        
        if( charInfoArray[i].age < 6 )
            charInfoArray[i].healthMax *= charInfoArray[i].age;
        else
            charInfoArray[i].healthMax *= 5;

        charInfoArray[i].oldCLevel = charInfoArray[i].cLevel;

        MakeCharacterValid(i);

    }

    if (fileVersionNumber >= 1.99f)
        bank->InventoryLoad(fp, fileVersionNumber);

    if (fileVersionNumber >= 1.30f)
        LoadContacts(fp, fileVersionNumber);

    if( fileVersionNumber < 2.22f ) // this version gave a month of free time!
        accountExperationTime.AddMinutes(60*24*31);

    if( fileVersionNumber < 2.3f ) // this version gave 6 weeks of free time!
        accountExperationTime.AddMinutes(60*24*42);

    fclose(fp);

    fp = fopen("logs\\nameripperoutput.txt","a");

    // save the names into the unique name system (if unique)
    for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
    {
        if (charInfoArray[i].topIndex > -1)
        {
            fprintf(fp,"Checking %s..... ", charInfoArray[i].name );
            if( UN_IsNameUnique( charInfoArray[i].name ) )
            {
                fprintf(fp, "%s\n", "IS UNIQUE!! ADDING!" );
                UN_AddName( charInfoArray[i].name );
            }
            else
            {
                fprintf(fp, "%s\n", "Not Unique" );
            }
        }
    }

    fclose( fp );

    bHasLoaded = true;
    return 0;
}


//******************************************************************
void BBOSAvatar::SaveAccount(void)
{
    int i;
    char fileName[64];//, temp[1024];

    assert(bHasLoaded);
    assert(passLen == strlen(pass));
    pass[passLen] = 0;

    // don't delete anything left in the trade inventory
    if (curCharacterIndex > -1)
    {
        int sendStopMessage = FALSE;
        InventoryObject *io = (InventoryObject *) trade->objects.First();
        while (io)
        {
            sendStopMessage = TRUE;
            trade->objects.Remove(io);

            if (io->amount == 0)
            {
                delete io;
            }
            else
            {
                charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
            }

            io = (InventoryObject *) trade->objects.First();
        }

        if (sendStopMessage)
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessSecureTrade mess;
            mess.type = MESS_SECURE_STOP; 
            bboServer->lserver->SendMsg(sizeof(mess),(void *)&mess, 0, &tempReceiptList);
        }

    }

    // remove leading whitespace
    int startChar = 0;
    while (' ' == name[startChar])
        ++startChar;

    int nameStart = startChar;
    char dir[2] = {name[startChar], '\0'};

    sprintf(fileName, "users\\%s\\%s.use", &dir, &(name[startChar]));

    startChar = 0;
    while (0 != fileName[startChar])
    {
        if (' ' == fileName[startChar])
            fileName[startChar] = '-';

        ++startChar;
    }

    FILE *fp = fopen(fileName,"w");

    if (fp)
    {
        // writing out
        startChar = 0;
        while (' ' == pass[startChar])
            ++startChar;

        int passStart = startChar;

        fprintf(fp,"%f\n",VERSION_NUMBER);
        fprintf(fp,"%s\n%s\n",&name[nameStart], &pass[passStart]);

        fprintf(fp,"%d %ld %ld %d %d\n", accountType, infoFlags, chatChannels, isInvulnerable, hasPaid);

        fprintf(fp,"%d %d\n", isReferralDone, patronCount);

        fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
                accountExperationTime.value.wYear,
                accountExperationTime.value.wMonth,
                accountExperationTime.value.wDay,
                accountExperationTime.value.wDayOfWeek,
                accountExperationTime.value.wHour,
                accountExperationTime.value.wMinute,
                accountExperationTime.value.wSecond,
                accountExperationTime.value.wMilliseconds);

        fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
                accountRestrictionTime.value.wYear,
                accountRestrictionTime.value.wMonth,
                accountRestrictionTime.value.wDay,
                accountRestrictionTime.value.wDayOfWeek,
                accountRestrictionTime.value.wHour,
                accountRestrictionTime.value.wMinute,
                accountRestrictionTime.value.wSecond,
                accountRestrictionTime.value.wMilliseconds);

        fprintf(fp,"%d\n", restrictionType);

        for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
        {
            fprintf(fp,"%s\n%d %d %d %d\n",charInfoArray[i].name, 
                     charInfoArray[i].topIndex, charInfoArray[i].topR,
                      charInfoArray[i].topG,    charInfoArray[i].topB);

            fprintf(fp,"%d %d %d %d\n", 
                     charInfoArray[i].bottomIndex, charInfoArray[i].bottomR,
                      charInfoArray[i].bottomG,    charInfoArray[i].bottomB);

            fprintf(fp,"%d %d %d %d\n", 
                     charInfoArray[i].faceIndex, charInfoArray[i].hairR,
                      charInfoArray[i].hairG,    charInfoArray[i].hairB);

            fprintf(fp,"%f\n",charInfoArray[i].cLevel);

            fprintf(fp,"%ld %ld\n",charInfoArray[i].health, charInfoArray[i].healthMax);
            fprintf(fp,"%ld %ld %ld\n",charInfoArray[i].physical, 
                                        charInfoArray[i].magical, 
                                               charInfoArray[i].creative);

            fprintf(fp,"%d %d %d %d\n",charInfoArray[i].lastX, charInfoArray[i].lastY,
                   charInfoArray[i].spawnX, charInfoArray[i].spawnY);

            fprintf(fp,"%ld %ld %d\n", (long)charInfoArray[i].imageFlags, 
                                         charInfoArray[i].lifeTime, charInfoArray[i].age);

            if (i == curCharacterIndex)
                charInfoArray[i].lastSavedTime.SetToNow();
            
            fprintf(fp,"%d %d %d %d %d %d %d %d\n", 
                    charInfoArray[i].lastSavedTime.value.wYear,
                    charInfoArray[i].lastSavedTime.value.wMonth,
                    charInfoArray[i].lastSavedTime.value.wDay,
                    charInfoArray[i].lastSavedTime.value.wDayOfWeek,
                    charInfoArray[i].lastSavedTime.value.wHour,
                    charInfoArray[i].lastSavedTime.value.wMinute,
                    charInfoArray[i].lastSavedTime.value.wSecond,
                    charInfoArray[i].lastSavedTime.value.wMilliseconds);

            for (int dIndex = 0; dIndex < 2; ++dIndex)
            {
                fprintf(fp,"%s\n",charInfoArray[i].petDragonInfo[dIndex].name);
                fprintf(fp,"%d %d %d %d %ld %d %d %f\n",
                         charInfoArray[i].petDragonInfo[dIndex].type,
                         charInfoArray[i].petDragonInfo[dIndex].quality,
                         charInfoArray[i].petDragonInfo[dIndex].state,
                         charInfoArray[i].petDragonInfo[dIndex].lifeStage,
                         charInfoArray[i].petDragonInfo[dIndex].age,
                         charInfoArray[i].petDragonInfo[dIndex].lastEatenType,
                         charInfoArray[i].petDragonInfo[dIndex].lastEatenSubType,
                         charInfoArray[i].petDragonInfo[dIndex].healthModifier
                         );
            }

            for (int j = 0; j < SAMARITAN_REL_MAX; ++j)
            {
                for (int k = 0; k < SAMARITAN_TYPE_MAX; ++k)
                {
                    fprintf(fp," %d %d", charInfoArray[i].karmaGiven[j][k], charInfoArray[i].karmaReceived[j][k]);
                }
            }
            fprintf(fp,"\n");

            for (int j = 0; j < QUEST_SLOTS; ++j)
            {
                charInfoArray[i].quests[j].Save(fp);
            }
            fprintf(fp,"%d\n%s\n", 
                     charInfoArray[i].witchQuestIndex, 
                      charInfoArray[i].witchQuestName);

            charInfoArray[i].inventory->InventorySave(fp);
            charInfoArray[i].workbench->InventorySave(fp);
            charInfoArray[i].skills->InventorySave(fp);
            charInfoArray[i].wield->InventorySave(fp);
        }

        bank->InventorySave(fp);
        SaveContacts(fp);

        fclose(fp);
    }


    return;
}


//******************************************************************
void BBOSAvatar::BuildStatsMessage(MessAvatarStats *mStats)
{
    mStats->faceIndex = charInfoArray[curCharacterIndex].faceIndex;
    mStats->hairR    = charInfoArray[curCharacterIndex].hairR;
    mStats->hairG    = charInfoArray[curCharacterIndex].hairG;
    mStats->hairB    = charInfoArray[curCharacterIndex].hairB;

    mStats->topIndex = charInfoArray[curCharacterIndex].topIndex;
    mStats->topR    = charInfoArray[curCharacterIndex].topR;
    mStats->topG    = charInfoArray[curCharacterIndex].topG;
    mStats->topB    = charInfoArray[curCharacterIndex].topB;

    mStats->bottomIndex = charInfoArray[curCharacterIndex].bottomIndex;
    mStats->bottomR    = charInfoArray[curCharacterIndex].bottomR;
    mStats->bottomG    = charInfoArray[curCharacterIndex].bottomG;
    mStats->bottomB    = charInfoArray[curCharacterIndex].bottomB;

    mStats->imageFlags = charInfoArray[curCharacterIndex].imageFlags;

    mStats->avatarID = socketIndex;
    memcpy(mStats->name, charInfoArray[curCharacterIndex].name,31);
    mStats->name[31] = 0;

    mStats->cash = charInfoArray[curCharacterIndex].inventory->money;
    mStats->cLevel = charInfoArray[curCharacterIndex].cLevel;
    mStats->age = charInfoArray[curCharacterIndex].age;

}

//******************************************************************
void BBOSAvatar::GiveInfoFor(int x, int y, SharedSpace *ss)
{
    BBOSAvatar *curAvatar;
    MessAvatarAppear messAvAppear;
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);
    MessBladeDesc messBladeDesc;

    int tokenVal = tokenMan.TokenTypeInSquare(ss, x,y);
    if (tokenVal > -1)
    {
        // tell client about a token
        MessMobAppear messMA;
        messMA.mobID = (unsigned long) tokenVal;
        messMA.type = SMOB_TOKEN;
        messMA.x = x;
        messMA.y = y;
        ss->lserver->SendMsg(sizeof(messMA),(void *)&messMA, 0, &tempReceiptList);
    }

    BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI() && curMob != this && 
             curMob->cellX == x               && curMob->cellY == y &&
             FALSE == ((BBOSAvatar *) curMob)->isInvisible)
        {
            curAvatar = (BBOSAvatar *) curMob;
            messAvAppear.avatarID = curAvatar->socketIndex;
            messAvAppear.x = curAvatar->cellX;
            messAvAppear.y = curAvatar->cellY;
            ss->lserver->SendMsg(sizeof(messAvAppear),(void *)&messAvAppear, 0, &tempReceiptList);
            MessAvatarStats mStats;
            curAvatar->BuildStatsMessage(&mStats);
            ss->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);

            // tell people about my cool dragons!
            for (int index = 0; index < 2; ++index)
            {
                if (255 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].type)
                {
                    // tell everyone about it!
                    MessPet mPet;
                    mPet.avatarID = curAvatar->socketIndex;
                    CopyStringSafely(curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].name,16, 
                                          mPet.name,16);
                    mPet.quality = curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].quality;
                    mPet.type    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].type;
                    mPet.state   = curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].state;
                    mPet.size    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].lifeStage +
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[index].healthModifier / 10.0f;
                    mPet.which   = index;

                    ss->lserver->SendMsg(sizeof(mPet),(void *)&mPet, 0, &tempReceiptList);
                }
            }

            curAvatar->AssertGuildStatus(ss, FALSE, socketIndex);

            InventoryObject *iObject = (InventoryObject *) 
                curAvatar->charInfoArray[curAvatar->curCharacterIndex].wield->objects.First();
            while (iObject)
            {
                if (INVOBJ_BLADE == iObject->type)
                {
                    FillBladeDescMessage(&messBladeDesc, iObject, curAvatar);
                    ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                    iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].wield->objects.Last();
                    
                }
                if (INVOBJ_STAFF == iObject->type)
                {
                    InvStaff *iStaff = (InvStaff *) iObject->extra;
                    
                    messBladeDesc.bladeID = (long)iObject;
                    messBladeDesc.size    = 4;
                    messBladeDesc.r       = staffColor[iStaff->type][0];
                    messBladeDesc.g       = staffColor[iStaff->type][1];
                    messBladeDesc.b       = staffColor[iStaff->type][2]; 
                    messBladeDesc.avatarID= curAvatar->socketIndex;
                    messBladeDesc.trailType  = 0;
                    messBladeDesc.meshType = BLADE_TYPE_STAFF1;
                    ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                    iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].wield->objects.Last();
                    
                }
                iObject = (InventoryObject *) 
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].wield->objects.Next();
            }

        }

        curMob = (BBOSMob *) ss->avatars->Next();
    }

    curMob = ss->mobList->GetFirst(x,y);
    while (curMob)
    {
        if (curMob->cellX == x && curMob->cellY == y)
        {
            /*
            curAvatar = (BBOSAvatar *) curMob;
            messAvAppear.avatarID = curAvatar->socketIndex;
            messAvAppear.x = curAvatar->cellX;
            messAvAppear.y = curAvatar->cellY;
            lserver->SendMsg(sizeof(messAvAppear),(void *)&messAvAppear, 0, &tempReceiptList);
            MessAvatarStats mStats;
            curAvatar->BuildStatsMessage(&mStats);
            lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);
            */

            if (curMob->cellX == 0 && curMob->cellY == 3)
            {
                if (SMOB_MONSTER == curMob->WhatAmI() && ((BBOSMonster *) curMob)->subType > 5)
                {
                    ((BBOSMonster *) curMob)->subType = 0;
                    ((BBOSMonster *) curMob)->type = 2;
                    CopyStringSafely("ILLEGAL CREATURE", 32, ((BBOSMonster *) curMob)->uniqueName, 32);
                }
            }

            int sent = FALSE;

            if (SMOB_MONSTER == curMob->WhatAmI() && ((BBOSMonster *) curMob)->uniqueName[0])
            {
                BBOSMonster *curMonster = (BBOSMonster *) curMob;

                MessMobAppearCustom mAppear;
                mAppear.type = SMOB_MONSTER;
                mAppear.mobID = (unsigned long) curMob;
                mAppear.x = curMonster->cellX;
                mAppear.y = curMonster->cellY;
                mAppear.monsterType = curMonster->type;
                mAppear.subType = curMonster->subType;
                CopyStringSafely(curMonster->Name(), 32, mAppear.name, 32);
                mAppear.a = curMonster->a;
                mAppear.r = curMonster->r;
                mAppear.g = curMonster->g;
                mAppear.b = curMonster->b;
                mAppear.sizeCoeff = curMonster->sizeCoeff;

                if(SPACE_DUNGEON == ss->WhatAmI())
                {
                    mAppear.staticMonsterFlag = FALSE;
                    if (!curMonster->isWandering && !curMonster->isPossessed)
                        mAppear.staticMonsterFlag = TRUE;
                }

                ss->lserver->SendMsg(sizeof(mAppear),(void *)&mAppear, 0, &tempReceiptList);
                sent = TRUE;
            }

            if (!sent && SMOB_MONSTER_GRAVE != curMob->WhatAmI() && 
                          SMOB_BOMB != curMob->WhatAmI() &&
                          SMOB_GROUND_EFFECT != curMob->WhatAmI()
                )
            {
                MessMobAppear mobAppear;
                mobAppear.mobID = (unsigned long) curMob;
                mobAppear.type = curMob->WhatAmI();

                if (SMOB_MONSTER == mobAppear.type)
                {
                    BBOSMonster *curMonster = (BBOSMonster *) curMob;
                    mobAppear.monsterType = curMonster->type;
                    mobAppear.subType = curMonster->subType;
                    if(SPACE_DUNGEON == ss->WhatAmI())
                    {
                        mobAppear.staticMonsterFlag = FALSE;
                        if (!curMonster->isWandering && !curMonster->isPossessed)
                            mobAppear.staticMonsterFlag = TRUE;
                    }
                }
                else if (SMOB_TREE == mobAppear.type)
                {
                    BBOSTree *curTree = (BBOSTree *) curMob;
                    mobAppear.monsterType = curTree->index;
                }
                else if (SMOB_WARP_POINT == mobAppear.type)
                {
                    mobAppear.subType = 0;

                    if ((((BBOSWarpPoint *)curMob)->allCanUse ||
                          ACCOUNT_TYPE_ADMIN == accountType) &&
                         ((BBOSWarpPoint *)curMob)->spaceType < 100)
                    mobAppear.subType = 1;
                }
                else
                    mobAppear.subType = 0;

                mobAppear.x = curMob->cellX;
                mobAppear.y = curMob->cellY;
                ss->lserver->SendMsg(sizeof(mobAppear),(void *)&mobAppear, 0, &tempReceiptList);

                if (SMOB_TOWER == mobAppear.type)
                {
                    MessCaveInfo cInfo;
                    cInfo.mobID       = (long) curMob;

                    if (((BBOSTower *)curMob)->isGuildTower)
                    {
                        TowerMap *dm = (TowerMap *) ((BBOSTower *)curMob)->ss;
                        cInfo.hasMistress = FALSE;
                        cInfo.type        = -1;
                    }
                    else
                    {
                        DungeonMap *dm = (DungeonMap *) ((BBOSTower *)curMob)->ss;
                        cInfo.hasMistress = dm->masterName[0];
                        cInfo.type        = dm->dungeonRating;
                    }
                    ss->lserver->SendMsg(sizeof(cInfo),(void *)&cInfo, 0, &tempReceiptList);
                }
                else if (SMOB_CHEST == mobAppear.type)
                {
                    MessChestInfo cInfo;
                    cInfo.mobID       = (long) curMob;
                    cInfo.type = ((BBOSChest *)curMob)->isOpen;
                    ss->lserver->SendMsg(sizeof(cInfo),(void *)&cInfo, 0, &tempReceiptList);
                }
            }
            else if (!sent && SMOB_GROUND_EFFECT == curMob->WhatAmI())
            {
                BBOSGroundEffect *bboGE = (BBOSGroundEffect *) curMob;
                MessGroundEffect messGE;
                messGE.mobID  = (unsigned long) bboGE;
                messGE.type   = bboGE->type;
                messGE.amount = bboGE->amount;
                messGE.x      = bboGE->cellX;
                messGE.y      = bboGE->cellY;
                messGE.r      = bboGE->r;
                messGE.g      = bboGE->g;
                messGE.b      = bboGE->b;

                ss->lserver->SendMsg(sizeof(messGE),(void *)&messGE, 0, &tempReceiptList);
            }
        }
        curMob = ss->mobList->GetNext();
    }

}

char bombSizeNames[15][20] =
{
    {"Tiny"},
    {"Small"},
    {"Plain"},
    {"Big"},
    {"Hefty"},
    {"Huge"},
    {"Massive"},
    {"Colossal"},
    {"Enormous"},
    {"Gigantic"},
    {"Immense"},
    {"Titanic"},
    {"Nuclear"},
    {"Thermonuclear"},
    {"Doomsday"}
};


//******************************************************************
void BBOSAvatar::Combine(SharedSpace *ss)
{

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    char tempText[1024];
    MessInfoText infoText;


    //         ***********************************
    if (!strcmp(combineSkillName,"Swordsmith"))
    {
        //1)	determine the size the sword will be from the total number of ingots in 
        // the workbench-> Extra ingots will be used in the construction of the weapon, 
        // and the actual size of the weapon will reflect how many ingots got used.  
        // This test is only to choose part of the resulting name string and base damage.
        InventoryObject *io;
        int numIngots = 0;
        int allR = 0;
        int allG = 0;
        int allB = 0;
        float allDamage = 0, allCost = 0, challenge = 0;
        int dustAmount[INGR_MAX];
        int totalDust = 0;
        bool shardIncluded = false;			// this is to make sure we don't use more than one shard

        for (int i = 0; i < INGR_MAX; ++i)
            dustAmount[i] = 0;

        char ingotNames[20][64]; // assumes no more than 20 ingot types, ever!
        int ingotAmount[20], ingotAmount2[20];;

        for (int i = 0; i < 20; ++i) {
            ingotAmount[i] = 0;
            ingotAmount2[i] = 0;
        }

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_INGREDIENT == io->type)
            {
                InvIngredient * ingre = (InvIngredient *)io->extra;

                if (ingre->type < 0 || ingre->type > INGR_MAX)
                {
                    sprintf(tempText,"Ingredient type out of bounds!  Please report this bug.");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return;
                }

                // make sure we only include one shard
                if( ingre->type >= INGR_WHITE_SHARD && ingre->type <= INGR_PINK_SHARD ) {
                    if( !shardIncluded && io->amount == 1 ) {
                        dustAmount[ingre->type] += io->amount;
                        totalDust += io->amount;
                        shardIncluded = true;
                    }
                    else {
                        sprintf(tempText,"You can only have one shard in your workbench when you craft.");
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
                        
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        return;
                    }
                }
                else {
                    dustAmount[ingre->type] += io->amount;
                    totalDust += io->amount;
                }
                    
            }

            if (INVOBJ_INGOT == io->type)
            {
                int usableAmount = io->amount;
                if (usableAmount > 64)
                    usableAmount = 64;

                numIngots += 1 * usableAmount;
                InvIngot *ingotInfo = (InvIngot *) io->extra;

                //4)	 Get the sum total challenge level of all the ingots, as the 
                // task challenge level
                challenge += ingotInfo->challenge * usableAmount;
                allR += ingotInfo->r * usableAmount;
                allG += ingotInfo->g * usableAmount;
                allB += ingotInfo->b * usableAmount;
                allDamage +=  ingotInfo->damageVal * usableAmount;
                allCost += io->value  * usableAmount;

                for (int i = 0; i < 20; ++i)
                {
                    if (ingotAmount[i] > 0 && !strcmp(ingotNames[i],io->WhoAmI()))
                    {
                        ingotAmount[i] = 1  * usableAmount;
                        ingotAmount2[ (int)ingotInfo->challenge - 1 ] = ingotAmount[i];
                    }
                    else if (0 == ingotAmount[i])
                    {
                        ingotAmount[i] = 1 * usableAmount;
                        sprintf(ingotNames[i],io->WhoAmI());
                        ingotAmount2[ (int)ingotInfo->challenge - 1] = ingotAmount[i];
                        i = 21;
                    }
                }

            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        if (numIngots > 64)
        {
            challenge += challenge * (1.0f + (numIngots-64) / 32.0f);
        }

//		for (i = 0; i < INGR_MAX; ++i)
        challenge += (challenge / 5) * totalDust; // +20% for each ingredient

        //2)	Find out the first most abundant type of ingot
        int candidate = -1, most = 0;
        for (int i = 0; i < 20; ++i)
        {
            if (most < ingotAmount[i])
            {
                most = ingotAmount[i];
                candidate = i;
            }
        }

        if (candidate < 0)
        {
            sprintf(tempText,"Use ingots to make blades.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            //3)	Find out the second most abundant type of ingot
            ingotAmount[candidate] = 0;
            int candidate2 = -1, most2 = 0;
            for (int i = 0; i < 20; ++i)
            {
                if (most2 < ingotAmount[i])
                {
                    most2 = ingotAmount[i];
                    candidate2 = i;
                }
            }

            //5)	IF the task is successful,
            // B)	work = skill level * creativity * rnd(0.8,1.2)
            // C)	if Challenge <= work, success!

            InvSkill *skillInfo = NULL;
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
            while (io)
            {
                if (!strcmp(combineSkillName,io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
            }

            if (!skillInfo)
            {
                sprintf(tempText,"BUG: Cannot find skill for level.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                float work = skillInfo->skillLevel * 
                              CreativeStat() *
                                 rnd(0.3f,1.7f);

                if (tokenMan.TokenIsInHere(MAGIC_WOLF, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_BEAR, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_EAGLE, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }

                if (specLevel[2] > 0)
                    work *= (1.0f + 0.05f * specLevel[2]);

                sprintf(tempText,"Task difficulty is %4.2f.", challenge);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"Your work value was %4.2f.", work);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                if (challenge <= work) // success!
                {
                    bool shardDestroyed = false;

                    // destroy everything from workbench
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {
                        if (INVOBJ_INGOT == io->type)
                        {
                            if (io->amount > 64)
                            {
                                io->amount -= 64;
                            }
                            else
                            {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }
                        else if (INVOBJ_INGREDIENT == io->type)
                        {
                            if( io->amount > 1 && ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD && !shardDestroyed ) {
                                --(io->amount);
                                shardDestroyed = true;
                            }
                            else if( !( ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD ) || !shardDestroyed )
                            {
                                if( ( ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD ) )
                                    shardDestroyed = true;

                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }
                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                    //6)	The name of the sword is a fusion of the 
                    //    first and second ingot types, 
                    //    and the size (Carbon-Zinc Short Sword),
                    if (candidate2 > -1)
                    {
                        sscanf (ingotNames[candidate],"%s", &(tempText[0]));
                        sprintf(&(tempText[strlen(tempText)]), "-");
                        sscanf (ingotNames[candidate2],"%s", &(tempText[strlen(tempText)]));
                    }
                    else
                    {
                        sscanf (ingotNames[candidate],"%s", &(tempText[0]));
                    }
                    sprintf(&(tempText[strlen(tempText)]), " ");

                    int candidate3 = 6, most3 = 0;
                    for (int j = 0; j < 6; ++j)
                    {
                        if (bladeList[j].size > numIngots)
                        {
                            candidate3 = j-1;
                            j = 7;
                        }
                    }

                    if (candidate3 < 0)
                        candidate3 = 0;
                    if (candidate3 > 5)
                        candidate3 = 5;

                    sprintf(&(tempText[strlen(tempText)]), bladeList[candidate3].name);

                    // make the new blade!
                    io = new InventoryObject(INVOBJ_BLADE,0,tempText);
                    InvBlade *ib = (InvBlade *) io->extra; 

                    //7)	The damage of the sword is the average damage value of all ingots * the size base damage
                    ib->damageDone = allDamage * bladeList[candidate3].damage / numIngots;

                    //8)	The value of the sword is the sum of the costs of all the ingots * 1.4
                    io->value = allCost * 1.4f;

                    //9)	The color of the sword blade is the average of the color of all the ingots
                    ib->r = allR / numIngots;
                    ib->g = allG / numIngots;
                    ib->b = allB / numIngots;

                    ib->toHit = 1 + dustAmount[INGR_BLUE_DUST];
                    ib->size = 1.0 + numIngots / 16.0f;

                    ib->poison = dustAmount[INGR_BLACK_DUST];
                    ib->heal   = dustAmount[INGR_GREEN_DUST];
                    ib->slow   = dustAmount[INGR_RED_DUST];
                    ib->blind  = dustAmount[INGR_WHITE_DUST];
                    ib->lightning  = dustAmount[INGR_SILVER_DUST];


                    ib->tinIngots = ingotAmount2[0];
                    ib->aluminumIngots = ingotAmount2[1];
                    ib->steelIngots = ingotAmount2[2];
                    ib->carbonIngots = ingotAmount2[3];
                    ib->zincIngots = ingotAmount2[4];
                    ib->adamIngots = ingotAmount2[5];
                    ib->mithIngots = ingotAmount2[6];
                    ib->vizIngots = ingotAmount2[7];
                    ib->elatIngots = ingotAmount2[8];
                    ib->chitinIngots = ingotAmount2[9];
                    ib->maligIngots = ingotAmount2[10];

                    int mostIngr = -1, ingrCount = 0;
                    for (int i = 0; i < INGR_WHITE_SHARD; ++i)
                    {
                        if (dustAmount[i] > ingrCount)
                        {
                            ingrCount = dustAmount[i];
                            mostIngr = i;
                        }
                    }

                    if (mostIngr > -1)
                    {
                        switch(mostIngr)
                        {
                        case INGR_BLUE_DUST:
                        default:
                            ib->bladeGlamourType = BLADE_GLAMOUR_TOHIT1;
                            break;
                        case INGR_GREEN_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_HEAL1;
                            break;
                        case INGR_BLACK_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_POISON1;
                            break;
                        case INGR_WHITE_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_BLIND1;
                            break;
                        case INGR_RED_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_SLOW1;
                            break;
                        }

                        if (ingrCount > 1)
                            ++(ib->bladeGlamourType);
                        if (ingrCount > 4)
                            ++(ib->bladeGlamourType);
                        if (ingrCount > 9)
                            ++(ib->bladeGlamourType);
                    }

                    int shardIndex = -1;
                    for (int i = INGR_WHITE_SHARD; i <= INGR_PINK_SHARD; ++i)
                    {
                        if (dustAmount[i] > 0)
                        {
                            shardIndex = i-INGR_WHITE_SHARD;
                            i = INGR_MAX;
                        }
                    }

                    if (shardIndex > -1)
                    {
                        ib->bladeGlamourType = BLADE_GLAMOUR_TRAILWHITE + shardIndex;

                        ib->damageDone += ib->damageDone/10;
                    }

                    sprintf(tempText,"You have created a");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    sprintf(tempText,"%s!",io->WhoAmI());
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    QuestCraftWeapon(ss, io, challenge, work, combineSkillName);

                    charInfoArray[curCharacterIndex].inventory->AddItemSorted( io );
                }
                else
                {
                    // D)	ELSE, for each item in the workbench, 
                    //    the chance of being destroyed = 1.0 -(work/challenge) (0.0 to 1.0)
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {
                        int randomVal = rnd(0,5);

                    //    the chance of being destroyed = 1 - (work/challenge) (0.0 to 1.0)
                        if (
                               work/challenge < rnd(0,1) &&
                             ( INVOBJ_INGOT == io->type || ( 
                               INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type < INGR_WHITE_SHARD )  ||
                             ( INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD &&
                               INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type < INGR_MAX && randomVal == 2 )
                             )
                             )
                        {
                            if (io->amount > 1)
                                io->amount--;
                            else
                            {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                }

                // In any case, you get skill sub-points for that skill.
                if( skillInfo->skillLevel < 10000 && challenge > work / 20 )
                {
                    skillInfo->skillPoints += work;

                    if (skillInfo->skillLevel * skillInfo->skillLevel * 30 <= skillInfo->skillPoints)
                    {
                        // made a skill level!!!
                        skillInfo->skillLevel++;
                        sprintf(tempText,"You gained a skill level!!");
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                        
                        charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_SMITH;

                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }
            }
        }
        return;
    }


    //         ***********************************
    if (!strcmp(combineSkillName,"Katana Expertise")     ||
         !strcmp(combineSkillName,"Claw Expertise")       ||
         !strcmp(combineSkillName,"Chaos Expertise")      ||
         !strcmp(combineSkillName,"Bladestaff Expertise") ||
         !strcmp(combineSkillName,"Mace Expertise")           )
    {
        //1)	determine the size the sword will be from the total number of ingots in 
        // the workbench-> Extra ingots will be used in the construction of the weapon, 
        // and the actual size of the weapon will reflect how many ingots got used.  
        // This test is only to choose part of the resulting name string and base damage.
        InventoryObject *io;
        int numIngots = 0;
        InventoryObject *oldBlade = NULL;
        int numOldBlades = 0;
        int allR = 0;
        int allG = 0;
        int allB = 0;
        float allDamage = 0, allCost = 0, challenge = 0;
        int dustAmount[INGR_MAX];
        int totalDust = 0;
        bool shardIncluded = false;			// this is to make sure we don't use more than one shard

        for (int i = 0; i < INGR_MAX; ++i)
            dustAmount[i] = 0;

        char ingotNames[20][64]; // assumes no more than 20 ingot types, ever!
        int ingotAmount[20], ingotAmount2[20];

        for (int i = 0; i < 20; ++i) {
            ingotAmount[i] = 0;
            ingotAmount2[i] = 0;
        }

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_INGREDIENT == io->type)
            {
                InvIngredient * ingre = (InvIngredient *)io->extra;

                if (ingre->type < 0 || ingre->type > INGR_MAX)
                {
                    sprintf(tempText,"Ingredient type out of bounds!  Please report this bug.");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return;
                }

                // make sure we only include one shard
                if( ingre->type >= INGR_WHITE_SHARD && ingre->type <= INGR_PINK_SHARD ) {
                    if( !shardIncluded && io->amount == 1 ) {
                        dustAmount[ingre->type] += io->amount;
                        totalDust += io->amount;
                        shardIncluded = true;
                    }
                    else {
                        sprintf(tempText,"You can only have one shard in your workbench when you craft.");
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
                        
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        return;
                    }
                }
                else {
                    dustAmount[ingre->type] += io->amount;
                    totalDust += io->amount;
                }
            }

            if (INVOBJ_BLADE == io->type)
            {
                numOldBlades += io->amount;
                oldBlade = io;

            }

            if (INVOBJ_INGOT == io->type)
            {
                int usableAmount = io->amount;
                if (usableAmount > 32)
                    usableAmount = 32;

                numIngots += 1 * usableAmount;
                InvIngot *ingotInfo = (InvIngot *) io->extra;

                //4)	 Get the sum total challenge level of all the ingots, as the 
                // task challenge level
                challenge += ingotInfo->challenge * usableAmount;
                allR += ingotInfo->r * usableAmount;
                allG += ingotInfo->g * usableAmount;
                allB += ingotInfo->b * usableAmount;
                allDamage +=  ingotInfo->damageVal * usableAmount;
                allCost += io->value  * usableAmount;

                for (int i = 0; i < 20; ++i)
                {
                    if (ingotAmount[i] > 0 && !strcmp(ingotNames[i],io->WhoAmI()))
                    {
                        ingotAmount[i] = 1  * usableAmount;
                        ingotAmount2[ (int)ingotInfo->challenge - 1 ] = ingotAmount[i];
                    }
                    else if (0 == ingotAmount[i])
                    {
                        ingotAmount[i] = 1 * usableAmount;
                        sprintf(ingotNames[i],io->WhoAmI());
                        ingotAmount2[ (int)ingotInfo->challenge - 1 ] = ingotAmount[i];
                        i = 21;
                    }
                }

            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        // find and check the Swordsmith skill!
        InvSkill *skillInfo = NULL;
        io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
        while (io)
        {
            if (!strcmp("Swordsmith",io->WhoAmI()))
            {
                skillInfo = (InvSkill *) io->extra;
            }
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
        }

        if (!skillInfo)
        {
            sprintf(tempText,"You must first learn to create swords.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (skillInfo->skillLevel < 20)
        {
            sprintf(tempText,"You must first reach level 20 in your Swordsmith skill.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        // do you have to required sacrifical blade?
        if (!oldBlade || numOldBlades < 1 || ((InvBlade *)oldBlade->extra)->type != BLADE_TYPE_NORMAL)
        {
            sprintf(tempText,"You must sacrifice one blade to make this.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        float oldBladeSize = ((InvBlade *)oldBlade->extra)->size;
        if (oldBladeSize < 1.0 + 32 / 16.0f)
        {
            sprintf(tempText,"The sacrifical blade is not big enough.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (numIngots > 32)
        {
            challenge += challenge * (1.0f + (numIngots-32) / 32.0f);
        }

//		for (i = 0; i < INGR_MAX; ++i)
        challenge += (challenge / 5) * totalDust; // +20% for each ingredient

        //2)	Find out the first most abundant type of ingot
        int candidate = -1, most = 0;
        for (int i = 0; i < 20; ++i)
        {
            if (most < ingotAmount[i])
            {
                most = ingotAmount[i];
                candidate = i;
            }
        }

        if (candidate < 0)
        {
            sprintf(tempText,"Use ingots to make weapons.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            //3)	Find out the second most abundant type of ingot
            ingotAmount[candidate] = 0;
            int candidate2 = -1, most2 = 0;
            for (int i = 0; i < 20; ++i)
            {
                if (most2 < ingotAmount[i])
                {
                    most2 = ingotAmount[i];
                    candidate2 = i;
                }
            }

            //5)	IF the task is successful,
            // B)	work = skill level * creativity * rnd(0.8,1.2)
            // C)	if Challenge <= work, success!

            skillInfo = NULL;
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
            while (io)
            {
                if (!strcmp(combineSkillName,io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
            }

            if (!skillInfo)
            {
                sprintf(tempText,"BUG: Cannot find skill for level.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                float work = (20 * skillInfo->skillLevel) * 
                              CreativeStat() *
                                 rnd(0.3f,1.7f);

                if (tokenMan.TokenIsInHere(MAGIC_WOLF, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_BEAR, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_EAGLE, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }

                if (specLevel[2] > 0)
                    work *= (1.0f + 0.05f * specLevel[2]);

                sprintf(tempText,"Task difficulty is %4.2f.", challenge);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"Your work value was %4.2f.", work);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                if (challenge <= work) // success!
                {
                    // destroy everything from workbench
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {
                        bool shardDestroyed = false;

                        if (INVOBJ_INGOT == io->type)
                        {
                            if (io->amount > 32)
                            {
                                io->amount -= 32;
                            }
                            else
                            {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }
                        else if (INVOBJ_INGREDIENT == io->type)
                        {
                            if( io->amount > 1 && ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD && !shardDestroyed ) {
                                --(io->amount);
                                shardDestroyed = true;
                            }
                            else if( !( ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD ) || !shardDestroyed )
                            {
                                if( ( ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD && ((InvIngredient *)io->extra)->type <= INGR_PINK_SHARD ) )
                                    shardDestroyed = true;

                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }
                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                    //6)	The name of the sword is a fusion of the 
                    //    first and second ingot types, 
                    //    and the size (Carbon-Zinc Short Sword),
                    if (candidate2 > -1)
                    {
                        sscanf (ingotNames[candidate],"%s", &(tempText[0]));
                        sprintf(&(tempText[strlen(tempText)]), "-");
                        sscanf (ingotNames[candidate2],"%s", &(tempText[strlen(tempText)]));
                    }
                    else
                    {
                        sscanf (ingotNames[candidate],"%s", &(tempText[0]));
                    }
                    sprintf(&(tempText[strlen(tempText)]), " ");

                    int candidate3 = 3, most3 = 0;
                    for (int j = 0; j < 3; ++j)
                    {
                        if (katanaList[j].size > numIngots)
                        {
                            candidate3 = j-1;
                            j = 4;
                        }
                    }

                    if (candidate3 < 0)
                        candidate3 = 0;
                    if (candidate3 > 2)
                        candidate3 = 2;

                    if (!strcmp(combineSkillName,"Katana Expertise")     )
                        sprintf(&(tempText[strlen(tempText)]), katanaList[candidate3].name);
                    else if (!strcmp(combineSkillName,"Claw Expertise")       )
                        sprintf(&(tempText[strlen(tempText)]), clawList[candidate3].name);
                    else if (!strcmp(combineSkillName,"Chaos Expertise")      )
                        sprintf(&(tempText[strlen(tempText)]), chaosList[candidate3].name);
                    else if (!strcmp(combineSkillName,"Bladestaff Expertise") )
                        sprintf(&(tempText[strlen(tempText)]), bladestaffList[candidate3].name);
                    else if (!strcmp(combineSkillName,"Mace Expertise")       )
                        sprintf(&(tempText[strlen(tempText)]), maceList[candidate3].name);

                    // make the new blade!
                    io = new InventoryObject(INVOBJ_BLADE,0,tempText);
                    InvBlade *ib = (InvBlade *) io->extra; 

                    //7)	The damage of the sword is the average damage value of all ingots * the size base damage
                    ib->damageDone = allDamage * katanaList[candidate3].damage / numIngots / 2;
                    ib->damageDone += ((InvBlade *)oldBlade->extra)->damageDone/2;

                    // the 20 is the katana's special enhancement
                    ib->toHit = dustAmount[INGR_BLUE_DUST] + ((InvBlade *)oldBlade->extra)->toHit;
                    ib->size = 1.0 + (numIngots + 32) / 16.0f;
                    ib->size *= 0.7f;
                    

                    ib->tinIngots = ingotAmount2[0] + ((InvBlade *)oldBlade->extra)->tinIngots;
                    ib->aluminumIngots = ingotAmount2[1] + ((InvBlade *)oldBlade->extra)->aluminumIngots;
                    ib->steelIngots = ingotAmount2[2] + ((InvBlade *)oldBlade->extra)->steelIngots;
                    ib->carbonIngots = ingotAmount2[3] + ((InvBlade *)oldBlade->extra)->carbonIngots;
                    ib->zincIngots = ingotAmount2[4] + ((InvBlade *)oldBlade->extra)->zincIngots;
                    ib->adamIngots = ingotAmount2[5] + ((InvBlade *)oldBlade->extra)->adamIngots;
                    ib->mithIngots = ingotAmount2[6] + ((InvBlade *)oldBlade->extra)->mithIngots;
                    ib->vizIngots = ingotAmount2[7] + ((InvBlade *)oldBlade->extra)->vizIngots;
                    ib->elatIngots = ingotAmount2[8] + ((InvBlade *)oldBlade->extra)->elatIngots;
                    ib->chitinIngots = ingotAmount2[9] + ((InvBlade *)oldBlade->extra)->chitinIngots;
                    ib->maligIngots = ingotAmount2[10] + ((InvBlade *)oldBlade->extra)->maligIngots;


                    // Okay, here's the REAL reason to make a specialized weapon
                    ib->damageDone = ib->damageDone * (1.0f + 
                        (((InvBlade *)oldBlade->extra)->numOfHits / 22000.0 * 1.0f));

                    // weapon specializiations...
                    if (!strcmp(combineSkillName,"Katana Expertise")     )
                    {
                        ib->type = BLADE_TYPE_KATANA;
                        ib->toHit += 20;
                    }
                    else if (!strcmp(combineSkillName,"Claw Expertise")       )
                    {
                        ib->type = BLADE_TYPE_CLAWS;
                    }
                    else if (!strcmp(combineSkillName,"Chaos Expertise")      )
                    {
                        ib->type = BLADE_TYPE_CHAOS;
                    }
                    else if (!strcmp(combineSkillName,"Bladestaff Expertise") )
                    {
                        ib->type = BLADE_TYPE_DOUBLE;
                    }
                    else if (!strcmp(combineSkillName,"Mace Expertise")       )
                    {
                        ib->type = BLADE_TYPE_MACE;
                        ib->damageDone += 30;
                    }

                    //8)	The value of the sword is the sum of the costs of all the ingots * 1.4
                    io->value = allCost * 1.4f;
                    io->value += oldBlade->value;

                    //9)	The color of the sword blade is the average of the color of all the ingots
                    ib->r = allR / numIngots;
                    ib->g = allG / numIngots;
                    ib->b = allB / numIngots;

                    ib->poison = dustAmount[INGR_BLACK_DUST] + ((InvBlade *)oldBlade->extra)->poison;
                    ib->heal   = dustAmount[INGR_GREEN_DUST] + ((InvBlade *)oldBlade->extra)->heal;
                    ib->slow   = dustAmount[INGR_RED_DUST]   + ((InvBlade *)oldBlade->extra)->slow;
                    ib->blind  = dustAmount[INGR_WHITE_DUST] + ((InvBlade *)oldBlade->extra)->blind;
                    ib->lightning  = dustAmount[INGR_SILVER_DUST] + ((InvBlade *)oldBlade->extra)->lightning;

                    int mostIngr = -1, ingrCount = 0;
                    if (ib->poison > ingrCount)
                    {
                        ingrCount = ib->poison;
                        mostIngr = INGR_BLACK_DUST;
                    }
                    if (ib->heal > ingrCount)
                    {
                        ingrCount = ib->heal;
                        mostIngr = INGR_GREEN_DUST;
                    }
                    if (ib->slow > ingrCount)
                    {
                        ingrCount = ib->slow;
                        mostIngr = INGR_RED_DUST;
                    }
                    if (ib->blind > ingrCount)
                    {
                        ingrCount = ib->blind;
                        mostIngr = INGR_WHITE_DUST;
                    }
                    if (dustAmount[INGR_BLUE_DUST] + ((InvBlade *)oldBlade->extra)->toHit > ingrCount)
                    {
                        ingrCount = dustAmount[INGR_BLUE_DUST] + ((InvBlade *)oldBlade->extra)->toHit;
                        mostIngr = INGR_BLUE_DUST;
                    }

                    if (mostIngr > -1)
                    {
                        switch(mostIngr)
                        {
                        case INGR_BLUE_DUST:
                        default:
                            ib->bladeGlamourType = BLADE_GLAMOUR_TOHIT1;
                            break;
                        case INGR_GREEN_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_HEAL1;
                            break;
                        case INGR_BLACK_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_POISON1;
                            break;
                        case INGR_WHITE_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_BLIND1;
                            break;
                        case INGR_RED_DUST:
                            ib->bladeGlamourType = BLADE_GLAMOUR_SLOW1;
                            break;
                        }

                        if (ingrCount > 1)
                            ++(ib->bladeGlamourType);
                        if (ingrCount > 4)
                            ++(ib->bladeGlamourType);
                        if (ingrCount > 9)
                            ++(ib->bladeGlamourType);
                    }

                    int shardIndex = -1;
                    for (int i = INGR_WHITE_SHARD; i <= INGR_PINK_SHARD; ++i)
                    {
                        if (dustAmount[i] > 0)
                        {
                            shardIndex = i-INGR_WHITE_SHARD;
                            i = INGR_MAX;
                        }
                    }

                    if (shardIndex > -1)
                    {
                        ib->bladeGlamourType = BLADE_GLAMOUR_TRAILWHITE + shardIndex;
                        ib->damageDone += ib->damageDone/10;
                    }

                    sprintf(tempText,"You have created a");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    sprintf(tempText,"%s!",io->WhoAmI());
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    QuestCraftWeapon(ss, io, challenge, work, combineSkillName);

                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);

                    // destroy blade from workbench
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {
                        if (INVOBJ_BLADE == io->type && oldBlade == io)
                        {
                            if( io->amount == 1 ) {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                            else {
                                io->amount -= 1;
                            }
                        }
                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                }
                else
                {
                    // D)	ELSE, for each item in the workbench, 
                    //    the chance of being destroyed = 1.0 -(work/challenge) (0.0 to 1.0)
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {

                    //    the chance of being destroyed = 1 - (work/challenge) (0.0 to 1.0)
                        if (
                               work/challenge < rnd(0,1) &&
                             ( INVOBJ_INGOT == io->type || ( 
                               INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type < INGR_WHITE_SHARD )  ||
                             ( INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type >= INGR_WHITE_SHARD &&
                               INVOBJ_INGREDIENT == io->type && ((InvIngredient *)io->extra)->type < INGR_MAX && rnd(0,5) == 2 )
                             )
                             && io->type != INVOBJ_BLADE
                        )
                        {
                            if (io->amount > 1)
                                io->amount--;
                            else
                            {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                }

                // In any case, you get skill sub-points for that skill.
                if( skillInfo->skillLevel < 1000 && challenge > work / 50 )
                {
                    skillInfo->skillPoints += work;

                    if (skillInfo->skillLevel * skillInfo->skillLevel * 3000 <= skillInfo->skillPoints)
                    {
                        // made a skill level!!!
                        skillInfo->skillLevel++;
                        sprintf(tempText,"You gained a skill level!!");
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                        
                        charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_SMITH_EXPERT;

                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }
            }
        }
        return;
    }


    /*
    float r,g,b;

    for (int i = 0; i < 3; ++i)
    {
        if (enemyPen[i])
            DeleteObject(enemyPen[i]);
        if (enemyPen2[i])
            DeleteObject(enemyPen2[i]);

        int gl = gameLevel-(2-i);
        if (gl < 0)
            gl = 0;
        gl *= 111;
        while (gl > 355)
            gl -= 360;

        HSVtoRGB(gl, 1, 1, r,g,b);
        enemyPen[i] = CreatePen(PS_SOLID, 1, RGB(255*r, 255*g, 255*b));

    */


    //         ***********************************
    if (!strcmp(combineSkillName,"Explosives"))
    {
        // determine the total power and color of the bomb

        InventoryObject *io;
        int numParts = 0;
        int numFuses = 0;
        int allR = 0;
        int allG = 0;
        int allB = 0;
        float bombR, bombG, bombB;

        bombR = bombG = bombB = 1;

        float allPower = 0, allCost = 0, challenge = 0;
        float fuseQuality = 0;

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_EXPLOSIVE == io->type)
            {
                InvExplosive * ingre = (InvExplosive *)io->extra;

                allPower  += ingre->power   * io->amount;
                challenge += ingre->quality * io->amount;
                numParts  += io->amount;
            }
            else if (INVOBJ_MEAT == io->type)
            {
                InvMeat * ingre = (InvMeat *)io->extra;

                int gl = ingre->type;
                if (gl < 0)
                    gl = 0;
                gl *= 360/25;
                while (gl > 355)
                    gl -= 360;

                HSVtoRGB(gl, 1, 1, bombR, bombG, bombB);
            }


            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_FUSE == io->type)
            {
                InvFuse * ingre = (InvFuse *)io->extra;
                numFuses += io->amount;
                fuseQuality += ingre->quality * io->amount;
//				challenge -= ingre->quality;
                charInfoArray[curCharacterIndex].workbench->objects.Last();
            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        challenge *= 1.0f + fuseQuality;

        if (numParts <= 0)
        {
            sprintf(tempText,"Use explosive material to make a bomb.");
            CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        InvSkill *skillInfo = NULL;
        io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
        while (io)
        {
            if (!strcmp(combineSkillName,io->WhoAmI()))
            {
                skillInfo = (InvSkill *) io->extra;
            }
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
        }

        if (!skillInfo)
        {
            sprintf(tempText,"BUG: Cannot find skill for level.");
            CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            float work = skillInfo->skillLevel * 
                          CreativeStat() *
                             rnd(0.7f,1.3f);

            if (tokenMan.TokenIsInHere(MAGIC_SNAKE, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                work *= 1.1f;
            if (tokenMan.TokenIsInHere(MAGIC_FROG, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                work *= 1.1f;
            if (tokenMan.TokenIsInHere(MAGIC_SUN, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                work *= 1.1f;

            if (specLevel[2] > 0)
                work *= (1.0f + 0.05f * specLevel[2]);

            sprintf(tempText,"Task difficulty is %4.2f.", challenge);
            CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            sprintf(tempText,"Your work value was %4.2f.", work);
            CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            if (challenge <= work) // success!
            {
                // destroy everything from workbench
                io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                while (io)
                {
                    if (INVOBJ_EXPLOSIVE == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        delete io;
                    }
                    else if (INVOBJ_FUSE == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        delete io;
                    }
                    else if (INVOBJ_MEAT == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        delete io;
                    }
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                }

                // make the new bomb!

                int tempPower, size;
                size = 0;
                tempPower = allPower/10;
                do 
                {
                    ++size;
                    tempPower /= 10;
                } while (tempPower > 0);

                if (size > 14)
                    size = 14;

                sprintf(tempText,"%s Bomb", bombSizeNames[size]);

                io = new InventoryObject(INVOBJ_BOMB,0,tempText);
                charInfoArray[curCharacterIndex].inventory->objects.Prepend(io);
                InvBomb *ib = (InvBomb *) io->extra; 

                ib->power = allPower;
                ib->fuseDelay = numFuses * 3;

                ib->stability = work/(challenge*2);
                int getsEXP = TRUE;
                if (ib->stability < 0)
                    ib->stability = 0;
                if (ib->stability > 1)
                {
                    if (ib->stability > 5)
                        getsEXP = FALSE;
                    ib->stability = 1;
                }
                io->value = allPower * 2.0f;

                // color
                ib->r = bombR * 255;
                ib->g = bombG * 255;
                ib->b = bombB * 255;

                sprintf(tempText,"You have created a");
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"%s!",io->WhoAmI());
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                // If successful, you get skill sub-points for that skill.
//				if (getsEXP)
//					skillInfo->skillPoints += 10 * (1.0f-ib->stability) + 5;
                skillInfo->skillPoints += challenge*2/3;

                if (skillInfo->skillLevel * skillInfo->skillLevel * 100 <= skillInfo->skillPoints)
                {
                    // made a skill level!!!
                    skillInfo->skillLevel++;
                    sprintf(tempText,"You gained a skill level!!");
                    CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_BOMB;
                }
            }
            else
            {
                // D)	ELSE, for each item in the workbench, 
                //    the chance of being destroyed = 1.0 -(work/challenge) (0.0 to 1.0)
                int puffPower = 0;
                io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                while (io)
                {
                    if (INVOBJ_EXPLOSIVE == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        puffPower += io->amount;
                        delete io;
                    }
                    else if (INVOBJ_FUSE == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        puffPower += io->amount;
                        delete io;
                    }
                    else if (INVOBJ_FUSE == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        delete io;
                    }
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                }

                MessExplosion explo;
                explo.avatarID = socketIndex;
                explo.r = 0;
                explo.g = 0;
                explo.b = 0;
                explo.type = 0;
                explo.flags = 0;
                explo.size = 5 + puffPower/10;
                explo.x = cellX;
                explo.y = cellY;
                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(explo), &explo, explo.size);

            }

        }
        
        return;
    }

    //         ***********************************
    if (!strcmp(combineSkillName,"Weapon Dismantle"))
    {
        InventoryObject *io;
//		int dustAmount[INGR_MAX];

        InventoryObject *weapon = NULL;
        int numWeapons = 0;

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_BLADE == io->type)
            {
                numWeapons += io->amount;
                weapon = io;
                io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Last();
            }
            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        if (numWeapons < 1)
        {
            sprintf(tempText,"Put a weapon in the workbench to dismantle it.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            InvBlade *ib = (InvBlade *) weapon->extra;

            // If they are stacked, remove the current one so it doesn't mess up
            if( weapon->amount > 1 ) {
                InventoryObject *tmp = new InventoryObject( INVOBJ_SIMPLE, 0, "UNNAMED" );
                weapon->CopyTo( tmp );
                tmp->amount = weapon->amount - 1;
                weapon->amount = 1;

                charInfoArray[curCharacterIndex].workbench->objects.Prepend( tmp );
            }

            int type1, type2;
            DetectIngotTypes(ib, type1, type2);

            float weapSize = ib->size;

            if (BLADE_TYPE_NORMAL != ib->type)
                    weapSize /= 0.7f;

            float ingotCount = ib->GetIngotCount();
            float originalIngotCount = ib->GetIngotCount();
              
            float damDone = ib->damageDone;

            if (ib->bladeGlamourType >= BLADE_GLAMOUR_TRAILWHITE)
                 damDone = damDone * 10 / 11;

            if (BLADE_TYPE_MACE == ib->type)
                damDone -= 30;

            float challenge = (1+ingotCount) * (2+type1) * (1+ib->poison/5) * 
                               (1+ib->slow/5) * (1+ib->blind/5) * (1+ib->heal/5);


            //5)	IF the task is successful,
            // B)	work = skill level * creativity * rnd(0.8,1.2)
            // C)	if Challenge <= work, success!

            InvSkill *skillInfo = NULL;
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
            while (io)
            {
                if (!strcmp(combineSkillName,io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
            }

            if (!skillInfo)
            {
                sprintf(tempText,"BUG: Cannot find skill for level.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                float work = skillInfo->skillLevel * 
                              CreativeStat() *
                                 rnd(0.9f,1.1f) * 3;

//				float tempChall = challenge;

                if (tokenMan.TokenIsInHere(MAGIC_WOLF, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_BEAR, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }
                if (tokenMan.TokenIsInHere(MAGIC_EAGLE, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                {
                    work *= 1.1f;
                }

                if (specLevel[2] > 0)
                    work *= (1.0f + 0.05f * specLevel[2]);

                sprintf(tempText,"This weapon's complexity is %4.2f.", challenge);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"Your work value was %4.2f.", work);
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                // generate some ingots
                int ingotsReturned = work/5/challenge * ingotCount;
                ingotsReturned = Bracket(ingotsReturned,
                                          (int)(ingotCount * 0.5f), (int)(ingotCount * 0.9f));

                int ingotsReturnedArray[25];
                for (int i = 0; i < 25; ++i)
                    ingotsReturnedArray[i] = 0;

                ib->maligIngots = ib->maligIngots * 0.8;
                ib->chitinIngots = ib->chitinIngots * 0.8;
                ib->elatIngots = ib->elatIngots * 0.8;
                ib->vizIngots = ib->vizIngots * 0.8;
                ib->mithIngots = ib->mithIngots * 0.8;
                ib->adamIngots = ib->adamIngots * 0.8;
                ib->zincIngots = ib->zincIngots * 0.8;
                ib->carbonIngots = ib->carbonIngots * 0.8;
                ib->steelIngots = ib->steelIngots * 0.8;
                ib->aluminumIngots = ib->aluminumIngots * 0.8;
                ib->tinIngots = ib->tinIngots * 0.8;

                for (int i = 0; i < ingotsReturned; ++i)
                {
                    int ingotVal;

                    if( ib->maligIngots )
                    {
                        ingotVal = 11;
                        --(ib->maligIngots);
                    }
                    else if( ib->chitinIngots )
                    {
                        ingotVal = 10;
                        --(ib->chitinIngots);
                    }
                    else if( ib->elatIngots )
                    {
                        ingotVal = 9;
                        --(ib->elatIngots);
                    }
                    else if( ib->vizIngots )
                    {
                        ingotVal = 8;
                        --(ib->vizIngots);
                    }
                    else if( ib->mithIngots )
                    {
                        ingotVal = 7;
                        --(ib->mithIngots);
                    }
                    else if( ib->adamIngots )
                    {
                        ingotVal = 6;
                        --(ib->adamIngots);
                    }
                    else if( ib->zincIngots )
                    {
                        ingotVal = 5;
                        --(ib->zincIngots);
                    }
                    else if( ib->carbonIngots )
                    {
                        ingotVal = 4;
                        --(ib->carbonIngots);
                    }
                    else if( ib->steelIngots )
                    {
                        ingotVal = 3;
                        --(ib->steelIngots);
                    }
                    else if( ib->aluminumIngots )
                    {
                        ingotVal = 2;
                        --(ib->aluminumIngots);
                    }
                    else if( ib->tinIngots )
                    {
                        ingotVal = 1;
                        --(ib->tinIngots);
                    }
                    else
                        break;

                    sprintf(tempText,"%s Ingot", ingotNameList[ingotVal-1]);
                    io = new InventoryObject(INVOBJ_INGOT,0, tempText);
                    InvIngot *exIngot = (InvIngot *)io->extra;
                    exIngot->damageVal = ingotVal;
                    exIngot->challenge = ingotVal;
                    exIngot->r = ingotRGBList[ingotVal-1][0];
                    exIngot->g = ingotRGBList[ingotVal-1][1];
                    exIngot->b = ingotRGBList[ingotVal-1][2];

                    io->mass = 1.0f;
                    io->value = ingotValueList[ingotVal-1];
                    io->amount = 1;

                    ++ingotsReturnedArray[ingotVal-1];

                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                }

                for (int i = 0; i < 25; ++i)
                {
                    if (ingotsReturnedArray[i] > 0)
                    {
                        sprintf(tempText,"You recover %d %s Ingots.", 
                                 ingotsReturnedArray[i], ingotNameList[i]);
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }

                // generate some dust
                int gotDust = FALSE;
                float dustReturned = Bracket( (int)(work/10/challenge * ib->blind),
                                            (int)(ib->blind*0.3f), (int)(ib->blind*0.9f));
                if (dustReturned >= 1)
                {
                    io = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing White Dust");
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = INGR_WHITE_DUST;
                    exIn->quality  = 1;
                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = dustReturned;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                    gotDust = TRUE;

                    sprintf(tempText,"You recover %d Glowing White Dust.", (int)dustReturned);
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

                dustReturned = Bracket( (int)(work/10/challenge * ib->slow),
                                            (int)(ib->slow*0.3f), (int)(ib->slow*0.9f));
                if (dustReturned >= 1)
                {
                    io = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Red Dust");
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = INGR_RED_DUST;
                    exIn->quality  = 1;
                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = dustReturned;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                    gotDust = TRUE;

                    sprintf(tempText,"You recover %d Glowing Red Dust.", (int)dustReturned);
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

                dustReturned = Bracket( (int)(work/10/challenge * ib->heal),
                                            (int)(ib->heal*0.3f), (int)(ib->heal*0.9f));
                if (dustReturned >= 1)
                {
                    io = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Green Dust");
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = INGR_GREEN_DUST;
                    exIn->quality  = 1;
                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = dustReturned;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                    gotDust = TRUE;

                    sprintf(tempText,"You recover %d Glowing Green Dust.", (int)dustReturned);
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

                dustReturned = Bracket( (int)(work/10/challenge * ib->poison),
                                            (int)(ib->poison*0.3f), (int)(ib->poison*0.9f));
                if (dustReturned >= 1)
                {
                    io = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Black Dust");
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = INGR_BLACK_DUST;
                    exIn->quality  = 1;
                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = dustReturned;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                    gotDust = TRUE;

                    sprintf(tempText,"You recover %d Glowing Black Dust.", (int)dustReturned);
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

                dustReturned = Bracket( (int)(work/10/challenge * (ib->toHit-20)),
                                            (int)((ib->toHit-20)*0.2f), (int)((ib->toHit-20)*0.7f));
                if (dustReturned >= 1)
                {
                    io = new InventoryObject(INVOBJ_INGREDIENT,0,"Glowing Blue Dust");
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = INGR_BLUE_DUST;
                    exIn->quality  = 1;
                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = dustReturned;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                    gotDust = TRUE;

                    sprintf(tempText,"You recover %d Glowing Blue Dust.", (int)dustReturned);
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }


                // generate something special

                dustReturned = Bracket( (int)(work/15/challenge), 0, 1);

                int chance = (rand() % 10);
                
                if( dustReturned > 0 && ib->numOfHits >= 22000 && gotDust && chance == 1 && originalIngotCount >= 96 )
                {
                    int type = INGR_WHITE_SHARD + (rand() % 8);
                    io = new InventoryObject(
                                INVOBJ_INGREDIENT,0,dustNames[type]);
                    InvIngredient *exIn = (InvIngredient *)io->extra;
                    exIn->type     = type;
                    exIn->quality  = 1;

                    io->mass = 0.0f;
                    io->value = 1000;
                    io->amount = 1;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);

                    sprintf(tempText,"You find something special inside the weapon.");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);


                    sprintf( tempText, "%s got a shard\n", charInfoArray[curCharacterIndex].name );
                    LogOutput("ShardCreationLog.txt", tempText);


                }
                
                // destroy weapons from workbench
                io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                while (io)
                {
                    if (weapon == io)
                    {
                        if (io->amount > 1)
                            --io->amount;
                        else
                        {
                            charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                            delete io;
                        }
                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Last();
                    }
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                }


                sprintf(tempText,"You dismantle the weapon.",io->WhoAmI());
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);


                // In any case, you get skill sub-points for that skill.
                if( skillInfo->skillLevel < 10000 && challenge > work / 10 ) 
                {
                    skillInfo->skillPoints += work;

                    if( skillInfo->skillLevel * skillInfo->skillLevel * 30 <= skillInfo->skillPoints )
                    {
                        // made a skill level!!!
                        skillInfo->skillLevel++;
                        sprintf(tempText,"You gained a skill level!!");
                        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                        
                        charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_SMITH;

                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }
            }
        }
        return;
    }

    //         ***********************************
    if (!strcmp(combineSkillName,"Geomancy"))
    {
        InventoryObject *io;
        int meatType[2];
        float gemPower, beadPower;
        int beadCount;

        beadCount = 0;
        meatType[0] = meatType[1] = -1;
        gemPower = beadPower = 1;

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_MEAT == io->type)
            {
                InvMeat * ingre = (InvMeat *)io->extra;
                if (-1 != meatType[0])
                    meatType[1] = ingre->type;
                else
                    meatType[0] = ingre->type;
            }
            else if (INVOBJ_GEOPART == io->type)
            {
                InvGeoPart * ingre = (InvGeoPart *)io->extra;
                if (0 == ingre->type) // bead
                {
                    beadCount += io->amount;
                    beadPower += ingre->power * io->amount;
                }
                else if (1 == ingre->type) // Heart Gem
                {
                    gemPower += ingre->power * io->amount;
                }
            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        if (-1 == meatType[0])
        {
            sprintf(tempText,"An EarthKey requires the flesh of a monster.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else if (beadCount < 1)
        {
            sprintf(tempText,"An EarthKey requires at least one bead to power it.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            InvSkill *skillInfo = NULL;
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
            while (io)
            {
                if (!strcmp(combineSkillName,io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
            }

            if (!skillInfo)
            {
                sprintf(tempText,"BUG: Cannot find skill for level.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                float work = skillInfo->skillLevel * 
                    MagicalStat() / 4.0f * rnd(0.7f,1.3f);

                if (tokenMan.TokenIsInHere(MAGIC_MOON, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                    work *= 1.2f;
                if (tokenMan.TokenIsInHere(MAGIC_TURTLE, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                    work *= 1.3f;

                if (specLevel[1] > 0)
                    work *= (1.0f + 0.05f * specLevel[1]);

                float challenge = beadCount * beadCount * gemPower * 1.5f;

                sprintf(tempText,"Task difficulty is %4.2f.", challenge);
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"Your work value was %4.2f.", work);
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                if (challenge <= work) // success!
                {
                    // destroy everything from workbench
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {
                        if (INVOBJ_MEAT == io->type || INVOBJ_GEOPART == io->type)
                        {
                            charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                            delete io;
                        }
                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                    // make the new item, an EarthKey.

                    int ekName1 = Bracket((int)(beadPower * gemPower) % 4, 0, 4);
                    int ekName2 = 5 + Bracket((int)(beadPower * gemPower/10) % 4, 0, 4);

                    sprintf(tempText, "%s %s EarthKey", 
                             earthKeyArray[ekName1], earthKeyArray[ekName2]);

                    io = new InventoryObject(INVOBJ_EARTHKEY,0,tempText);
                    io->mass = 0.0f;
                    io->value = 200 + beadPower * gemPower * 3;
                    io->amount = 1;

                    InvEarthKey *exIn = (InvEarthKey *)io->extra;
                    exIn->power = beadPower * gemPower;

                    // exclude a few certain types of monster
                    if (11 == meatType[0] || 16 == meatType[0] || 
                         23 == meatType[0] || 21 == meatType[0] || 
                         20 == meatType[0] || 27 == meatType[0]
                         || 28 == meatType[0])
                        meatType[0] = 0;

                    if (11 == meatType[1] || 16 == meatType[1] || 
                         23 == meatType[1] || 21 == meatType[1] || 
                         20 == meatType[1] || 27 == meatType[1]
                         || 28 == meatType[1])
                        meatType[1] = 0;

                    exIn->monsterType[0] = meatType[0];
                    exIn->monsterType[1] = meatType[1];
                    exIn->width = sqrt(exIn->power)/5;
                    exIn->width = exIn->width * 5 + 10;
                    if (exIn->width < 10)
                        exIn->width = 10;

                    exIn->height = sqrt((exIn->power*0.8f))/5;
                    exIn->height = exIn->height * 5 + 10;
                    if (exIn->height < 10)
                        exIn->height = 10;

                    sprintf(tempText,"You have created a");
                    CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    sprintf(tempText,"%s!",io->WhoAmI());
                    CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);

                    // IF successful, you get skill sub-points for that skill.
                    if ( skillInfo->skillLevel < 10000 && challenge > work / 20 )
                    {
                        skillInfo->skillPoints += work;

                        if (skillInfo->skillLevel * skillInfo->skillLevel * 30 <= skillInfo->skillPoints)
                        {
                            // made a skill level!!!
                            skillInfo->skillLevel++;
                            sprintf(tempText,"You gained a skill level!!");
                            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                            
                            charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_SMITH;

                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }

                }
                else
                {
                    sprintf(tempText,"Your Eye of the Earth fails to see.");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    // D)	ELSE, for each item in the workbench, 
                    //    the chance of being destroyed = 1.0 -(work/challenge) (0.0 to 1.0)
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                    while (io)
                    {

                    //    the chance of being destroyed = 1 - (work/challenge) (0.0 to 1.0)
                        if (work/challenge < rnd(0,1) &&
                             (INVOBJ_MEAT == io->type || INVOBJ_GEOPART == io->type))
                        {
                            if (io->amount > 1)
                                io->amount--;
                            else
                            {
                                charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                                delete io;
                            }
                        }

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                    }

                }

            }

        }
        
        return;
    }



    // *******************************************
    // TOTEM SHATTER
    // *******************************************
    if (!strcmp(combineSkillName,"Totem Shatter"))
    {
        InventoryObject *io;
        InventoryObject *tot = NULL;
        

        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_TOTEM == io->type)
            {
                tot = io;
            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        if ( tot == NULL )
        {
            sprintf(tempText,"You must have a totem in your workbench to shatter.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            InvSkill *skillInfo = NULL;
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
            while (io)
            {
                if (!strcmp(combineSkillName,io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
            }

            if (!skillInfo)
            {
                sprintf(tempText,"BUG: Cannot find skill for level.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                // Calculate work value
                float work = skillInfo->skillLevel * MagicalStat() / 4.0f * rnd( 0.8f,1.5f );

                if (tokenMan.TokenIsInHere(MAGIC_EVIL, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
                    work *= 1.01f;

                if (specLevel[1] > 0)
                    work *= (1.0f + 0.05f * specLevel[1]);


                // Calculate challenge of shattering all magic from totem ( Quality + 1 ) * imbues
                float challenge = ( ((InvTotem *)tot->extra)->quality * 2 ) * ( ((InvTotem *)tot->extra)->quality * 2 ) + 
                    ((InvTotem *)tot->extra)->TotalImbues() * 10;

                if( challenge < 1 )
                    challenge = 1;

                sprintf(tempText,"Task difficulty is %4.2f.", challenge);
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"Your work value was %4.2f.", work);
                CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                if (challenge <= work) // success!
                {
                    sprintf(tempText,"You successfully shatter the totem with magic!  Its power engulfs the items in your workbench.");
                    CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);


                    //****
                    // BEGIN SHATTER ITEM CODE
                    //****

                    float * imbues;
                    imbues = ((InvTotem *)tot->extra)->imbue;

                    if( imbues[ MAGIC_FROG ] == 2 && imbues[ MAGIC_BEAR ] == 2 ) { // green dust into white
                        sprintf(tempText,"The released power wants to turn Green Dust into White Dust.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                        while (io) {
                            if ( INVOBJ_INGREDIENT == io->type && ((InvIngredient*)io->extra)->type == INGR_GREEN_DUST && imbues[ MAGIC_EVIL ] > 0 ) {
                                if( io->amount <= imbues[ MAGIC_EVIL ] ) {
                                    charInfoArray[curCharacterIndex].workbench->objects.Remove( io );
                                    sprintf( io->do_name, "Glowing White Dust" );
                                    ((InvIngredient*)io->extra)->type = INGR_WHITE_DUST;
                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                                else {
                                    io->amount -= imbues[ MAGIC_EVIL ];

                                    io = new InventoryObject( INVOBJ_INGREDIENT, 0, "Glowing White Dust" );
                                    io->extra = new InvIngredient();
                                    io->amount = imbues[ MAGIC_EVIL ];
                                    ((InvIngredient*)io->extra)->type = INGR_WHITE_DUST;
                                    ((InvIngredient*)io->extra)->quality = 1;
                                    io->mass = 0.0f;
                                    io->value = 1000;
                                    
                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                            }

                            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                        }
                    }
                    else if( imbues[ MAGIC_FROG ] == 2 && imbues[ MAGIC_TURTLE ] == 2 ) { // black dust into red
                        sprintf(tempText,"The released power wants to turn Black Dust into Red Dust.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                        while (io) {
                            if ( INVOBJ_INGREDIENT == io->type && ((InvIngredient*)io->extra)->type == INGR_BLACK_DUST && imbues[ MAGIC_EVIL ] > 0 ) {
                                if( io->amount <= imbues[ MAGIC_EVIL ] ) {
                                    charInfoArray[curCharacterIndex].workbench->objects.Remove( io );

                                    sprintf( io->do_name, "Glowing Red Dust" );
                                    ((InvIngredient*)io->extra)->type = INGR_RED_DUST;

                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                                else {
                                    io->amount -= imbues[ MAGIC_EVIL ];

                                    io = new InventoryObject( INVOBJ_INGREDIENT, 0, "Glowing Red Dust" );
                                    io->extra = new InvIngredient();
                                    io->amount = imbues[ MAGIC_EVIL ];
                                    ((InvIngredient*)io->extra)->type = INGR_RED_DUST;
                                    ((InvIngredient*)io->extra)->quality = 1;
                                    io->mass = 0.0f;
                                    io->value = 1000;
                                    
                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                            }

                            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                        }
                    }
                    else if( imbues[ MAGIC_BEAR ] == 5 && imbues[ MAGIC_TURTLE ] == 5 ) { // green dust into blue
                        sprintf(tempText,"The released power wants to turn Green Dust into Blue Dust.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                        while (io) {
                            if ( INVOBJ_INGREDIENT == io->type && ((InvIngredient*)io->extra)->type == INGR_GREEN_DUST && imbues[ MAGIC_EVIL ] > 0 ) {
                                if( io->amount <= imbues[ MAGIC_EVIL ] ) {
                                    charInfoArray[curCharacterIndex].workbench->objects.Remove( io );

                                    sprintf( io->do_name, "Glowing Blue Dust" );
                                    ((InvIngredient*)io->extra)->type = INGR_BLUE_DUST;

                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                                else {
                                    io->amount -= imbues[ MAGIC_EVIL ];

                                    io = new InventoryObject( INVOBJ_INGREDIENT, 0, "Glowing Blue Dust" );
                                    io->extra = new InvIngredient();
                                    io->amount = imbues[ MAGIC_EVIL ];
                                    ((InvIngredient*)io->extra)->type = INGR_BLUE_DUST;
                                    ((InvIngredient*)io->extra)->quality = 1;
                                    io->mass = 0.0f;
                                    io->value = 1000;
                                    
                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                            }

                            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                        }
                    }
                    else if( imbues[ MAGIC_BEAR ] == 7 && imbues[ MAGIC_TURTLE ] == 7 ) { // black dust into blue
                        sprintf(tempText,"The released power wants to turn Black Dust into Blue Dust.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                        while (io) {
                            if ( INVOBJ_INGREDIENT == io->type && ((InvIngredient*)io->extra)->type == INGR_BLACK_DUST && imbues[ MAGIC_EVIL ] > 0 ) {
                                if( io->amount <= imbues[ MAGIC_EVIL ] ) {
                                    charInfoArray[curCharacterIndex].workbench->objects.Remove( io );

                                    sprintf( io->do_name, "Glowing Blue Dust" );
                                    ((InvIngredient*)io->extra)->type = INGR_BLUE_DUST;

                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                                else {
                                    io->amount -= imbues[ MAGIC_EVIL ];

                                    io = new InventoryObject( INVOBJ_INGREDIENT, 0, "Glowing Blue Dust" );
                                    io->extra = new InvIngredient();
                                    io->amount = imbues[ MAGIC_EVIL ];
                                    ((InvIngredient*)io->extra)->type = INGR_BLUE_DUST;
                                    ((InvIngredient*)io->extra)->quality = 1;
                                    io->mass = 0.0f;
                                    io->value = 1000;
                                    
                                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
                                }
                            }

                            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                        }
                    }
                    else if( imbues[ MAGIC_WOLF ] == 6 && imbues[ MAGIC_SNAKE ] == 6 && imbues[ MAGIC_TURTLE ] == 6 ) { // rotate shard color + 1
                        sprintf(tempText,"The released power wants to change a shard's color.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();

                        while (io) {
                            if ( INVOBJ_INGREDIENT == io->type && 
                                    ( ((InvIngredient*)io->extra)->type >= INGR_WHITE_SHARD &&
                                        ((InvIngredient*)io->extra)->type <= INGR_PINK_SHARD ) ) {
                                ((InvIngredient*)io->extra)->type += 1;

                                if( ((InvIngredient*)io->extra)->type > INGR_PINK_SHARD )
                                    ((InvIngredient*)io->extra)->type = INGR_WHITE_SHARD;

                                strcpy( io->do_name, dustNames[ ((InvIngredient*)io->extra)->type ] );

                                break;
                            }

                            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                        }
                    }
                    else if( imbues[ MAGIC_WOLF ] == 1 && imbues[ MAGIC_BEAR ] == 1 && imbues[ MAGIC_MOON ] == 1 ) { // Free both drakes
                        sprintf(tempText,"The released power wants to release your pets.");
                        CopyStringSafely(tempText,1024, infoText.text, MESSINFOTEXTLEN);
                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);		

                        PetDragonInfo *dInfo1 = &( charInfoArray[curCharacterIndex].petDragonInfo[0] );
                        PetDragonInfo *dInfo2 = &( charInfoArray[curCharacterIndex].petDragonInfo[1] );

                        MessGenericEffect messGE;
                        MessPet mPet;
                        
                        if( dInfo1->type != 255 ) {
                            messGE.avatarID = socketIndex;
                            messGE.mobID    = -1;
                            messGE.x        = cellX;
                            messGE.y        = cellY;
                            messGE.r        = 40;
                            messGE.g        = 40;
                            messGE.b        = 50;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 5; // in seconds
                            ss->SendToEveryoneNearBut(0, cellX, cellY,
                                    sizeof(messGE),(void *)&messGE);

                            sprintf(tempText,"%s is freed into the wild.", dInfo1->name);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->SendToEveryoneNearBut(0, cellX, cellY,
                                sizeof(infoText),(void *)&infoText);

                            mPet.avatarID = socketIndex;
                            CopyStringSafely(dInfo1->name,16, 
                                        mPet.name,16);
                            mPet.quality = dInfo1->quality;
                            mPet.type    = 255;
                            mPet.state   = dInfo1->state;
                            mPet.size    = dInfo1->lifeStage + dInfo1->healthModifier/10.0f;
                            mPet.which   = 0;

                            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

                            dInfo1->type = 255;
                        }

                        if( dInfo2->type != 255 ) {
                            messGE.avatarID = socketIndex;
                            messGE.mobID    = -1;
                            messGE.x        = cellX;
                            messGE.y        = cellY;
                            messGE.r        = 40;
                            messGE.g        = 40;
                            messGE.b        = 50;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 5; // in seconds
                            ss->SendToEveryoneNearBut(0, cellX, cellY,
                                    sizeof(messGE),(void *)&messGE);

                            sprintf(tempText,"%s is freed into the wild.", dInfo2->name);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->SendToEveryoneNearBut(0, cellX, cellY,
                                sizeof(infoText),(void *)&infoText);

                            mPet.avatarID = socketIndex;
                            CopyStringSafely(dInfo2->name,16, 
                                        mPet.name,16);
                            mPet.quality = dInfo2->quality;
                            mPet.type    = 255;
                            mPet.state   = dInfo2->state;
                            mPet.size    = dInfo2->lifeStage + dInfo2->healthModifier/10.0f;
                            mPet.which   = 1;

                            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

                            dInfo2->type = 255;
                        }
                    }
                    
                    //****
                    // END SHATTER ITEM CODE
                    //****


                    // delete the totem
                    if (tot->amount > 1)
                        tot->amount--;
                    else
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(tot);
                        delete tot;
                    }
                    
                    // IF successful, you get skill sub-points for that skill.
                    if ( skillInfo->skillLevel < 10000 && challenge > work / 20 )
                    {
                        skillInfo->skillPoints += challenge;

                        if( skillInfo->skillLevel * skillInfo->skillLevel * 20 <= skillInfo->skillPoints )
                        {
                            // made a skill level!!!
                            skillInfo->skillLevel++;
                            sprintf(tempText,"You gained a skill level!!");
                            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                            
                            charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_SHATTER;

                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }

                }
                else
                {
                    sprintf(tempText, "You attempt to magically break the shatter and fail. It crumbles to dust.");
                    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    if (tot->amount > 1)
                        tot->amount--;
                    else
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(tot);
                        delete tot;
                    }
                }

            }

        }
        
        return;
    }





    //         ***********************************
    int usedMagic = -1;
    for (int i = 0; i < MAGIC_MAX; ++i)
    {
        if (!strnicmp(magicNameList[i],combineSkillName, strlen(magicNameList[i])))
            usedMagic = i;
    }

    if (usedMagic > -1)
    {
        int num = 0, eggNum = 0, staffNum = 0;
        int favorType = -1, favorAmount = 0;
        InventoryObject *totem;
        InventoryObject *egg;
        InventoryObject *staff;

        InventoryObject *io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
        while (io)
        {
            if (INVOBJ_TOTEM == io->type)
            {
                num += io->amount;
                totem = io;
            }
            else if (INVOBJ_STAFF == io->type)
            {
                staffNum += io->amount;
                staff = io;
            }
            else if (INVOBJ_EGG == io->type)
            {
                eggNum += io->amount;
                egg = io;
            }
            else if (INVOBJ_FAVOR == io->type)
            {
                favorAmount += io->amount;
                if (-1 == favorType)
                    favorType = ((InvFavor *) io->extra)->spirit;
                else
                    favorType = -2;
            }
            else
            {
                sprintf(tempText,"Use magic on a totem, staff or egg.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
        }

        if (num > 1)
        {
            sprintf(tempText,"Use magic on one totem at a time.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (eggNum > 1)
        {
            sprintf(tempText,"Use magic on one egg at a time.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (staffNum > 1)
        {
            sprintf(tempText,"Use magic on one staff at a time.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (num < 1 && eggNum < 1 && staffNum < 1)
        {
            sprintf(tempText,"Put a totem in your workbench.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        InvSkill *skillInfo = NULL;
        io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
        while (io)
        {
            if (!strcmp(combineSkillName,io->WhoAmI()))
            {
                skillInfo = (InvSkill *) io->extra;
            }
            io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
        }

        if (!skillInfo)
        {
            sprintf(tempText,"BUG: Cannot find skill for magic.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (eggNum > 0)
        {
            InvEgg *iEgg = (InvEgg *) egg->extra;
            int minimum = 4 + iEgg->quality + iEgg->type;
            if (minimum > 10)
                minimum = 10;

            if (MagicalStat() < minimum)
            {
                sprintf(tempText,"Your magic is too weak to hatch this egg.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            int index;
            if (255 == charInfoArray[curCharacterIndex].petDragonInfo[0].type)
                index = 0;
            else if (255 == charInfoArray[curCharacterIndex].petDragonInfo[1].type)
                index = 1;
            else
            {
                sprintf(tempText,"You already have two pet drakes.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            if (0 == index)
                CopyStringSafely("Pet A", 1024,
                                 charInfoArray[curCharacterIndex].petDragonInfo[index].name,
                                      16);
            else
                CopyStringSafely("Pet B", 1024,
                                 charInfoArray[curCharacterIndex].petDragonInfo[index].name,
                                      16);

            charInfoArray[curCharacterIndex].petDragonInfo[index].age = 0;
            charInfoArray[curCharacterIndex].petDragonInfo[index].lastEatenTime.SetToNow();
            charInfoArray[curCharacterIndex].petDragonInfo[index].lifeStage = 0;
            charInfoArray[curCharacterIndex].petDragonInfo[index].type = iEgg->type;
            charInfoArray[curCharacterIndex].petDragonInfo[index].quality = iEgg->quality;
            charInfoArray[curCharacterIndex].petDragonInfo[index].state = DRAGON_HEALTH_GREAT;  // normal
            charInfoArray[curCharacterIndex].petDragonInfo[index].lastEatenType = 0;
            charInfoArray[curCharacterIndex].petDragonInfo[index].lastEatenSubType = 0;

            // if you used the right type of magic, it's born a little stronger.
            charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier = 0;
            if (usedMagic == dragonInfo[iEgg->quality][iEgg->type].powerBirthMagicType)
                charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier = 1.0;

            // that type of token is also important
            if (tokenMan.TokenIsInHere(dragonInfo[iEgg->quality][iEgg->type].powerBirthMagicType, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
            {
                charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier += 1.0;
            }

            if (specLevel[1] > 0)
                charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier += 
                          2 * specLevel[1];

            // tell everyone about it!
            MessPet mPet;
            mPet.avatarID = socketIndex;
            mPet.name[0] = 0;
            mPet.quality = charInfoArray[curCharacterIndex].petDragonInfo[index].quality;
            mPet.type    = charInfoArray[curCharacterIndex].petDragonInfo[index].type;
            mPet.state   = charInfoArray[curCharacterIndex].petDragonInfo[index].state;
            mPet.size    = charInfoArray[curCharacterIndex].petDragonInfo[index].lifeStage +
                    charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier / 10.0f;
            mPet.which   = index;

            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

            sprintf(tempText,"You have hatched a pet drake!");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            MessGenericEffect messGE;
            messGE.avatarID = socketIndex;
            messGE.mobID    = -1;
            messGE.x        = cellX;
            messGE.y        = cellY;
            messGE.r        = 255;
            messGE.g        = 255;
            messGE.b        = 0;
            messGE.type     = 0;  // type of particles
            messGE.timeLen  = 5; // in seconds
            ss->SendToEveryoneNearBut(0, cellX, cellY,
                              sizeof(messGE),(void *)&messGE);

            FILE *source = fopen("petInfo.txt","a");
            /* Display operating system-style date and time. */
            _strdate( tempText );
            fprintf(source, "%s, ", tempText );
            _strtime( tempText );
            fprintf(source, "%s, ", tempText );

            fprintf(source,"BIRTH, %s, %s, %d, %d\n",
                dragonInfo[mPet.quality][mPet.type].eggName,
                charInfoArray[curCharacterIndex].name,
                cellX, cellY);

            fclose(source);

            // destroy the egg
            charInfoArray[curCharacterIndex].workbench->objects.Remove(egg);
            delete egg;

            return;
        }


        float work = skillInfo->skillLevel * 
            MagicalStat() / 4.0f * rnd(0.7f,1.3f);

        if (tokenMan.TokenIsInHere(usedMagic, ss) && ( (TowerMap*) ss)->IsMember( charInfoArray[curCharacterIndex].name ) )
            work *= 1.3f;

        if (specLevel[1] > 0)
            work *= (1.0f + 0.05f * specLevel[1]);

        // ********** imbue staff
        if (staffNum > 0)
        {
            int result = ImbueStaff(staff, usedMagic, work);

            InvStaff *extra = (InvStaff *)staff->extra;

            float chall = ImbueStaffChallenge(extra);
            sprintf(tempText,"Task difficulty is %4.2f.", chall);
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            sprintf(tempText,"Your work value was %4.2f.", work);
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            unsigned long exp = StaffImbueExperience(staff, skillInfo->skillLevel);
            skillInfo->skillPoints += exp;

            switch (result)
            {
            case IMBUE_RES_SUCCESS:
                sprintf(tempText,"The magic infuses the staff!");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                UpdateStaff(staff, work);

                QuestImbue(ss, staff, chall, work, combineSkillName);
                break;

            case IMBUE_RES_FAIL:
                sprintf(tempText,"The magic fails to imbue the staff.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                break;

            case IMBUE_RES_DISINTEGRATE:
                sprintf(tempText,"The attempt destroys the staff.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                charInfoArray[curCharacterIndex].workbench->objects.Remove(staff);
                delete staff;
                staff = NULL;

                break;
            }

            if (staff)
            {
                int totalImbues = 0;
                for (int j = 0; j < MAGIC_MAX; ++j)
                {
                    totalImbues += extra->imbue[j];
                }
                if (totalImbues > extra->quality * 5 + 20 && staff)
                {
                    CopyStringSafely("The staff can't hold that much magic.  It shatters!",1024,infoText.text,MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    charInfoArray[curCharacterIndex].workbench->objects.Remove(staff);
                    delete staff;
                }
            }

            if ( skillInfo->skillLevel < 100 && skillInfo->skillLevel * 30 <= skillInfo->skillPoints )
            {
                // made a skill level!!!
                skillInfo->skillLevel++;
                sprintf(tempText,"You gained a skill level!!");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_MAGIC;

                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }


            return;
        }


        // ********** imbue totem
        InvTotem *extra = (InvTotem *)totem->extra;

        if (extra->isActivated && 
             extra->type >= TOTEM_PHYSICAL &&
             extra->type <= TOTEM_CREATIVE)
        {
            CopyStringSafely("This type of totem cannot be further imbued.",1024,infoText.text,MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (!extra->isActivated && favorType >= 0)
        {
            // special favor-imbue!
            if (usedMagic != favorType)
            {
                CopyStringSafely("The attempt feels painful and improper; it fizzles.",1024,infoText.text,MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            sprintf(tempText,"Task difficulty is %4.2f.", (float) favorAmount + extra->quality);
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            sprintf(tempText,"Your work value was %4.2f.", work);
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            if (favorAmount + extra->quality <= work) // success!
            {
                // destroy favors from workbench
                io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.First();
                while (io)
                {
                    if (INVOBJ_FAVOR == io->type)
                    {
                        charInfoArray[curCharacterIndex].workbench->objects.Remove(io);
                        delete io;
                    }
                    io = (InventoryObject *) charInfoArray[curCharacterIndex].workbench->objects.Next();
                }

                extra->isActivated = TRUE;
                extra->imbueDeviation = sqrt((float)favorAmount);
                extra->timeToDie.SetToNow();
                extra->timeToDie.AddMinutes(
                  (extra->quality + 1) * (extra->quality + 1) * 10);

                switch(favorType)
                {
                case MAGIC_BEAR:
                case MAGIC_EAGLE:
                case MAGIC_SUN:
                default:
                    extra->type = TOTEM_PHYSICAL;
                    break;
                
                case MAGIC_SNAKE:
                case MAGIC_MOON:
                case MAGIC_TURTLE:
                    extra->type = TOTEM_MAGICAL;
                    break;

                case MAGIC_WOLF:
                case MAGIC_FROG:
                    extra->type = TOTEM_CREATIVE;
                    break;
                }

                sprintf(totem->do_name, "%s %s Totem", totemQualityName[extra->quality],
                                                      totemTypeName[extra->type]);

                if (abs((long double)(favorAmount - skillInfo->skillLevel)) < 8)
                {
                    int add = (favorAmount - skillInfo->skillLevel + 1);
                    if (add < 1)
                        add = 1;
                    skillInfo->skillPoints += add;
                }
            }
            else
            {
                sprintf(tempText,"The attempt fails.");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }

            if (skillInfo->skillLevel * 30 <= skillInfo->skillPoints)
            {
                // made a skill level!!!
                skillInfo->skillLevel++;
                sprintf(tempText,"You gained a skill level!!");
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_MAGIC;

                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }

            return;
        }


        int result = ImbueTotem(totem, usedMagic, work);

        sprintf(tempText,"Task difficulty is %4.2f.", (float)extra->quality);
        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
        
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        sprintf(tempText,"Your work value was %4.2f.", work);
        CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
        
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        unsigned long exp = TotemImbueExperience(totem, skillInfo->skillLevel);
        skillInfo->skillPoints += exp;

        switch (result)
        {
        case IMBUE_RES_SUCCESS:
            sprintf(tempText,"The magic infuses the totem!");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            UpdateTotem(totem);

            QuestImbue(ss, totem, (float) extra->quality, work, combineSkillName);
            break;

        case IMBUE_RES_FAIL:
            sprintf(tempText,"The magic fails to imbue the totem.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            break;

        case IMBUE_RES_DISINTEGRATE:
            sprintf(tempText,"The attempt destroys the totem.");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            charInfoArray[curCharacterIndex].workbench->objects.Remove(totem);
            delete totem;
            totem = NULL;

            break;
        }

        int totalImbues = 0;
        for (int j = 0; j < MAGIC_MAX && totem; ++j)
        {
            totalImbues += extra->imbue[j];
        }
        if (totalImbues > extra->quality + 10 && totem)
        {
            CopyStringSafely("The totem can't hold that much magic.  It shatters!",1024,infoText.text,MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            charInfoArray[curCharacterIndex].workbench->objects.Remove(totem);
            delete totem;
        }
        

        if ( skillInfo->skillLevel < 100 && skillInfo->skillLevel * 30 <= skillInfo->skillPoints)
        {
            // made a skill level!!!
            skillInfo->skillLevel++;
            sprintf(tempText,"You gained a skill level!!");
            CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
            
            charInfoArray[curCharacterIndex].cLevel += CLEVEL_VAL_MAGIC;

            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }

        return;
    }

    sprintf(tempText,"No combining happened.");
    CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
    
    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    
}

//******************************************************************
void BBOSAvatar::UpdateClient(SharedSpace *ss, int clearAll)
{
    int cX = cellX;
    int cY = cellY;

    if (controlledMonster)
    {
        cX = controlledMonster->cellX;
        cY = controlledMonster->cellY;
    }

    if (clearAll)
    {
        for (int y = 0; y < MAP_SIZE_HEIGHT; ++y)
        {
            for (int x = 0; x < MAP_SIZE_WIDTH; ++x)
            {
                updateMap[y*MAP_SIZE_WIDTH+x] = FALSE;
            }
        }
    }

    // tell my client about other mobs and avatars
    for (int y = 0; y < MAP_SIZE_HEIGHT; ++y)
    {
        for (int x = 0; x < MAP_SIZE_WIDTH; ++x)
        {
            if (updateMap[y*MAP_SIZE_WIDTH+x])
            {
                if (abs(y - cY) > 4 || abs(x - cX) > 4)
                {
                    // get rid of existing static objects
                    updateMap[y*MAP_SIZE_WIDTH+x] = FALSE;
                }
            }
            else
            {
                if (abs(y - cY) <= 4 && abs(x - cX) <= 4)
                {
                    // tell my client about mobs in this square
                    updateMap[y*MAP_SIZE_WIDTH+x] = TRUE;
                    GiveInfoFor(x,y, ss);
                }
            }
        }
    }
}


//******************************************************************
int BBOSAvatar::GetDodgeLevel(void)
{

    InventoryObject *io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].skills->objects.First();
    while (io)
    {
        if (!strcmp("Dodging",io->WhoAmI()))
        {
            InvSkill *skillInfo = (InvSkill *) io->extra;
            return skillInfo->skillLevel;
        }
        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].skills->objects.Next();
    }

    return 0;
}


//******************************************************************
void BBOSAvatar::IntroduceMyself(SharedSpace *ss, unsigned short special)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

//	if (isInvisible)
//		return;

    MessBladeDesc messBladeDesc;

    // tell everyone about my arrival
    if (0 == special)
    {
        MessAvatarAppear messAvAppear;
        messAvAppear.avatarID = socketIndex;
        messAvAppear.x = cellX;
        messAvAppear.y = cellY;
        if (isInvisible)
            ss->lserver->SendMsg(sizeof(messAvAppear),(void *)&messAvAppear, 0, &tempReceiptList);
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY,
                        sizeof(messAvAppear),(void *)&messAvAppear);
    }
    else
    {
        MessAvatarAppearSpecial messAvAppearSpecial;
        messAvAppearSpecial.avatarID = socketIndex;
        messAvAppearSpecial.x = cellX;
        messAvAppearSpecial.y = cellY;
        messAvAppearSpecial.typeOfAppearance = special;
        if (isInvisible)
            ss->lserver->SendMsg(sizeof(messAvAppearSpecial),(void *)&messAvAppearSpecial, 0, &tempReceiptList);
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY,
                        sizeof(messAvAppearSpecial),(void *)&messAvAppearSpecial);
    }

    MessAvatarStats mStats;
    BuildStatsMessage(&mStats);
    if (isInvisible)
        ss->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);
    else
        ss->SendToEveryoneNearBut(0, cellX, cellY,
                    sizeof(mStats),(void *)&mStats);

    // tell people about my cool dragons!
    for (int index = 0; index < 2; ++index)
    {
        if (255 != charInfoArray[curCharacterIndex].petDragonInfo[index].type)
        {
            // tell everyone about it!
            MessPet mPet;
            mPet.avatarID = socketIndex;
            CopyStringSafely(charInfoArray[curCharacterIndex].petDragonInfo[index].name,16, 
                              mPet.name,16);
            mPet.quality = charInfoArray[curCharacterIndex].petDragonInfo[index].quality;
            mPet.type    = charInfoArray[curCharacterIndex].petDragonInfo[index].type;
            mPet.state   = charInfoArray[curCharacterIndex].petDragonInfo[index].state;
            mPet.size    = charInfoArray[curCharacterIndex].petDragonInfo[index].lifeStage +
                    charInfoArray[curCharacterIndex].petDragonInfo[index].healthModifier / 10.0f;
            mPet.which   = index;

            if (isInvisible)
                ss->lserver->SendMsg(sizeof(mPet),(void *)&mPet, 0, &tempReceiptList);
            else
                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);
        }
    }

    // send the time to myself
    MessTimeOfDay mTime;
    mTime.value = bboServer->dayTimeCounter;
    ss->lserver->SendMsg(sizeof(mTime),(void *)&mTime, 0, &tempReceiptList);


    AssertGuildStatus(ss);

    // wield the wielded weapon (if you have one)
    InventoryObject *iObject = (InventoryObject *) 
        charInfoArray[curCharacterIndex].wield->objects.First();
    while (iObject)
    {
        if (INVOBJ_BLADE == iObject->type)
        {
            FillBladeDescMessage(&messBladeDesc, iObject, this);
            if (isInvisible)
                ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
            else
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                        sizeof(messBladeDesc),(void *)&messBladeDesc);

            return; // just the first wielded weapon
        }

        if (INVOBJ_STAFF == iObject->type)
        {
            InvStaff *iStaff = (InvStaff *) iObject->extra;
            
            messBladeDesc.bladeID = (long)iObject;
            messBladeDesc.size    = 4;
            messBladeDesc.r       = staffColor[iStaff->type][0];
            messBladeDesc.g       = staffColor[iStaff->type][1];
            messBladeDesc.b       = staffColor[iStaff->type][2]; 
            messBladeDesc.avatarID= socketIndex;
            messBladeDesc.trailType  = 0;
            messBladeDesc.meshType = BLADE_TYPE_STAFF1;
            if (isInvisible)
                ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
            else
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                        sizeof(messBladeDesc),(void *)&messBladeDesc);
            return;
            
        }

        iObject = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.Next();
    }



}

//******************************************************************
void BBOSAvatar::AssertGuildStatus(SharedSpace *ss, int full, int socketTarget)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    // if I'm an admin
    if(ACCOUNT_TYPE_ADMIN == accountType)
    {
        Chronos::BStream *	stream		= NULL;
        stream	= new Chronos::BStream(sizeof(AvatarGuildName) + 65);

        *stream << (unsigned char) NWMESS_AVATAR_GUILD_NAME; 

        *stream << (int) socketIndex; 

        stream->write("Administrator", strlen("Administrator"));

        *stream << (unsigned char) 0; 

        if (isInvisible)
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        else if (socketTarget)
        {
            tempReceiptList.clear();
            tempReceiptList.push_back(socketTarget);
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        }
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY, stream->used(), stream->buffer());

        delete stream;

        return;
    }

    // if I'm a moderator
    if( ACCOUNT_TYPE_MODERATOR == accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == accountType )
    {
        Chronos::BStream *	stream		= NULL;
        stream	= new Chronos::BStream(sizeof(AvatarGuildName) + 65);

        *stream << (unsigned char) NWMESS_AVATAR_GUILD_NAME; 

        *stream << (int) socketIndex; 

        if( accountType == ACCOUNT_TYPE_MODERATOR )
            stream->write("Moderator", strlen("Moderator"));
        else
            stream->write("Trial Moderator", strlen("Trial Moderator"));

        *stream << (unsigned char) 0; 

        if (isInvisible)
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        else if (socketTarget)
        {
            tempReceiptList.clear();
            tempReceiptList.push_back(socketTarget);
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        }
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY, stream->used(), stream->buffer());

        delete stream;

        return;
    }

    // if I belong to a guild, mention that
    SharedSpace *sx;
    if(bboServer->FindAvatarInGuild(charInfoArray[curCharacterIndex].name, &sx))
    {
        Chronos::BStream *	stream		= NULL;
        stream	= new Chronos::BStream(sizeof(AvatarGuildName) + 65);

        *stream << (unsigned char) NWMESS_AVATAR_GUILD_NAME; 

        *stream << (int) socketIndex; 

        stream->write(sx->WhoAmI(), strlen(sx->WhoAmI()));
        *stream << (unsigned char) 0; 

        if (isInvisible)
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        else if (socketTarget)
        {
            tempReceiptList.clear();
            tempReceiptList.push_back(socketTarget);
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        }
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY, stream->used(), stream->buffer());

        delete stream;
    }
    else if (full)
    {
        Chronos::BStream *	stream		= NULL;
        stream	= new Chronos::BStream(sizeof(AvatarGuildName) + 65);

        *stream << (unsigned char) NWMESS_AVATAR_GUILD_NAME; 

        *stream << (int) socketIndex; 

        *stream << (unsigned char) 0; 

        if (isInvisible)
            ss->lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);
        else
            ss->SendToEveryoneNearBut(0, cellX, cellY, stream->used(), stream->buffer());

        delete stream;
    }

}

//******************************************************************
void BBOSAvatar::MakeCharacterValid(int i)
{
    int total_stats = 0;

    if (charInfoArray[i].bottomIndex < 0)
        charInfoArray[i].bottomIndex = 0;
    
    if (charInfoArray[i].bottomIndex >= NUM_OF_BOTTOMS)
        charInfoArray[i].bottomIndex = NUM_OF_BOTTOMS-1;

    if (charInfoArray[i].topIndex < -1)	  // -1 means empty slot
        charInfoArray[i].topIndex = -1;
    
    if (charInfoArray[i].topIndex >= NUM_OF_TOPS)
        charInfoArray[i].topIndex = NUM_OF_TOPS-1;

    if (charInfoArray[i].faceIndex < 0)
        charInfoArray[i].faceIndex = 0;
    
    if (charInfoArray[i].faceIndex >= NUM_OF_FACES)
        charInfoArray[i].faceIndex = NUM_OF_FACES-1;

    if (accountType)
        return;

    total_stats = charInfoArray[i].creative + charInfoArray[i].physical + charInfoArray[i].magical;

    if ( ( total_stats > 12 && charInfoArray[i].age < 4 ) || charInfoArray[i].creative < 1 || charInfoArray[i].physical < 1 || charInfoArray[i].magical  < 1 ) {
        charInfoArray[i].magical = charInfoArray[i].physical = charInfoArray[i].creative = 4;
    }
}

//******************************************************************
void BBOSAvatar::LoadContacts(FILE *fp, float version)
{
    char tempText[1024];
    int linePoint, argPoint;

    // delete old list
    delete contacts;
    contacts = new DoublyLinkedList();


    LoadLineToString(fp, tempText);

    while (strnicmp("XYZENDXYZ",tempText,9))
    {
        linePoint = 0;
        argPoint = NextWord(tempText,&linePoint);

        int val = atoi(&(tempText[argPoint]));
        argPoint = NextWord(tempText,&linePoint);

        DataObject *dat = new DataObject(val, &(tempText[argPoint]));
        contacts->Append(dat);

        LoadLineToString(fp, tempText);
    }


}

//******************************************************************
void BBOSAvatar::SaveContacts(FILE *fp)
{
//	char tempText[1024];
//	int linePoint, argPoint;

    DataObject *dat = contacts->First();
    while (dat)
    {
        fprintf(fp,"%d %s\n",dat->WhatAmI(), dat->WhoAmI());
        dat = contacts->Next();
    }

    fprintf(fp,"XYZENDXYZ\n");
}

//******************************************************************
int BBOSAvatar::IsContact(char *name, int type)
{
    DataObject *dat = contacts->First();
    while (dat)
    {
        if (dat->WhatAmI() == type && IsCompletelySame(dat->WhoAmI(), name))
            return TRUE;

        dat = contacts->Next();
    }

    return FALSE;
}


//******************************************************************
void BBOSAvatar::HandleMeatRot(SharedSpace *ss)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    char tempText[1024];
    MessInfoText infoText;

    // handle for wield inventory
    InventoryObject *iObject = (InventoryObject *) 
        charInfoArray[curCharacterIndex].wield->objects.First();
    while (iObject)
    {
        if (INVOBJ_MEAT == iObject->type)
        {
            if (!HandleOneMeatRot(iObject))
            {
                sprintf(tempText,"Some %s rots away.",iObject->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                charInfoArray[curCharacterIndex].wield->objects.Remove(iObject);
                delete iObject;
            }
        }
        iObject = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.Next();
    }


    // handle for normal inventory
    iObject = (InventoryObject *) 
        charInfoArray[curCharacterIndex].inventory->objects.First();
    while (iObject)
    {
        if (INVOBJ_MEAT == iObject->type)
        {
            if (!HandleOneMeatRot(iObject))
            {
                sprintf(tempText,"Some %s rots away.",iObject->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                charInfoArray[curCharacterIndex].inventory->objects.Remove(iObject);
                delete iObject;
            }
        }
        iObject = (InventoryObject *) 
            charInfoArray[curCharacterIndex].inventory->objects.Next();
    }

    // handle for workbench inventory
    iObject = (InventoryObject *) 
        charInfoArray[curCharacterIndex].workbench->objects.First();
    while (iObject)
    {
        if (INVOBJ_MEAT == iObject->type)
        {
            if (!HandleOneMeatRot(iObject))
            {
                sprintf(tempText,"Some %s rots away.",iObject->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                charInfoArray[curCharacterIndex].workbench->objects.Remove(iObject);
                delete iObject;
            }
        }
        iObject = (InventoryObject *) 
            charInfoArray[curCharacterIndex].workbench->objects.Next();
    }

}

//******************************************************************
int BBOSAvatar::HandleOneMeatRot(InventoryObject *iObject)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    char tempText[1024];
    MessInfoText infoText;

    InvMeat *im = (InvMeat *) iObject->extra;

    if (im->age < 0)
        return 1;

    ++(im->age);
    if (24 == im->age) // if 2 hours have passed
    {
        sprintf(tempText,"Rotted %s",iObject->WhoAmI());
        CopyStringSafely(tempText, 1024, iObject->do_name, DO_NAME_LENGTH);
    }
    else if (48 <= im->age) // if 4 hours have passed
    {
        return 0;
    }

    return 1;
    
}


//******************************************************************
void BBOSAvatar::PetAging(SharedSpace *ss)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    char tempText[1024];
    MessInfoText infoText;

    LongTime ltNow;

    int stageTime = 48; // in 5 minute increments, so 1 hour = 12

    for (int i = 0; i < 2; ++i)
    {
        PetDragonInfo *dInfo = &(charInfoArray[curCharacterIndex].petDragonInfo[i]);

        if (255 != dInfo->type) // if a valid dragon
        {
            int oldState   = dInfo->state;

            if (dInfo->lastEatenTime.MinutesDifference(&ltNow) > 120)
            {
                // death by starvation!
                MessGenericEffect messGE;
                messGE.avatarID = socketIndex;
                messGE.mobID    = -1;
                messGE.x        = cellX;
                messGE.y        = cellY;
                messGE.r        = 40;
                messGE.g        = 40;
                messGE.b        = 50;
                messGE.type     = 0;  // type of particles
                messGE.timeLen  = 5; // in seconds
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(messGE),(void *)&messGE);

                sprintf(tempText,"%s dies of starvation.", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(infoText),(void *)&infoText);

                MessPet mPet;
                mPet.avatarID = socketIndex;
                CopyStringSafely(dInfo->name,16, 
                              mPet.name,16);
                mPet.quality = dInfo->quality;
                mPet.type    = 255;
                mPet.state   = dInfo->state;
                mPet.size    = dInfo->lifeStage + dInfo->healthModifier/10.0f;
                mPet.which   = i;

                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

                FILE *source = fopen("petInfo.txt","a");
                /* Display operating system-style date and time. */
                _strdate( tempText );
                fprintf(source, "%s, ", tempText );
                _strtime( tempText );
                fprintf(source, "%s, ", tempText );

                fprintf(source,"STARVATION, %s, %s, %d, %d\n",
                    dragonInfo[dInfo->quality][dInfo->type].eggName,
                    charInfoArray[curCharacterIndex].name,
                    cellX, cellY);

                fclose(source);

                dInfo->type = 255;

            }
            else if (dInfo->lastEatenTime.MinutesDifference(&ltNow) > 60)
            {
                sprintf(tempText,"%s is famished!", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                dInfo->healthModifier -= 0.1f;
                dInfo->state = DRAGON_HEALTH_SICK;
            }
            else if (dInfo->lastEatenTime.MinutesDifference(&ltNow) > 30)
            {
                sprintf(tempText,"%s is hungry.", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                dInfo->state = DRAGON_HEALTH_NORMAL;
            }
            else if (dInfo->lastEatenTime.MinutesDifference(&ltNow) > 20)
            {
                sprintf(tempText,"%s seems to be hungry.", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }

            ++(dInfo->age);
            if (dInfo->age >= 4 * stageTime)
            {
                // time... to die.
                MessGenericEffect messGE;
                messGE.avatarID = socketIndex;
                messGE.mobID    = -1;
                messGE.x        = cellX;
                messGE.y        = cellY;
                messGE.r        = 40;
                messGE.g        = 40;
                messGE.b        = 50;
                messGE.type     = 0;  // type of particles
                messGE.timeLen  = 5; // in seconds
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(messGE),(void *)&messGE);

                sprintf(tempText,"%s passes away.", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(infoText),(void *)&infoText);

                MessPet mPet;
                mPet.avatarID = socketIndex;
                CopyStringSafely(dInfo->name,16, 
                              mPet.name,16);
                mPet.quality = dInfo->quality;
                mPet.type    = 255;
                mPet.state   = dInfo->state;
                mPet.size    = dInfo->lifeStage + dInfo->healthModifier/10.0f;
                mPet.which   = i;

                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

                // give orchid if in graveyard
                // pet graveyard 90,53 and 71 206
                if (
                     (cellX >= 90 && cellX <= 91 && cellY >= 53 && cellY <= 54) ||
                     (cellX >= 71 && cellX <= 72 && cellY >= 206 && cellY <= 207)
                    )
                {
                    sprintf(tempText,"%s gives you a final gift for your kindness.", dInfo->name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    InventoryObject *iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Dragon Orchid");
                    iObject->value  = 10000;
                    iObject->amount = 1;
                    charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);
                }


                FILE *source = fopen("petInfo.txt","a");
                /* Display operating system-style date and time. */
                _strdate( tempText );
                fprintf(source, "%s, ", tempText );
                _strtime( tempText );
                fprintf(source, "%s, ", tempText );

                fprintf(source,"DEATH, %s, %s, %d, %d\n",
                    dragonInfo[dInfo->quality][dInfo->type].eggName,
                    charInfoArray[curCharacterIndex].name,
                    cellX, cellY);

                fclose(source);

                // turn off the dragon
                dInfo->type = 255;

            }
            else if (dInfo->age == (dInfo->lifeStage+1) * stageTime)
            {
                // does she change?
                if (dragonInfo[dInfo->quality][dInfo->type].goodMeatType[dInfo->lifeStage] ==
                           dInfo->lastEatenType)
                {
                    dInfo->type = 
                        dragonInfo[dInfo->quality][dInfo->type].goodMeatTypeResult[dInfo->lifeStage];
                }
/*
                if (dragonInfo[dInfo->quality][dInfo->type].okayMeatType[dInfo->lifeStage] ==
                           dInfo->lastEatenType)
                {
                    dInfo->type = 
                        dragonInfo[dInfo->quality][dInfo->type].okayMeatTypeResult[dInfo->lifeStage];
                }
*/
                // does she drop an egg?
                if (dragonInfo[dInfo->quality][dInfo->type].breedMeatType[dInfo->lifeStage] ==
                           dInfo->lastEatenType ||
                    dragonInfo[dInfo->quality][dInfo->type].okayMeatType[dInfo->lifeStage] ==
                           dInfo->lastEatenType)
                {
                    sprintf(tempText,"%s lays an egg!", dInfo->name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    Inventory *inv = charInfoArray[curCharacterIndex].inventory;

                    int qual = dInfo->quality + 1;
                    if (qual > 3)
                        qual = 3;

                    InventoryObject *iObject = new InventoryObject(
                                INVOBJ_EGG,0,dragonInfo[qual][dInfo->type].eggName);
                    iObject->mass = 1.0f;
                    iObject->value = 1000;

                    InvEgg *im = (InvEgg *) iObject->extra;
                    im->type   = dInfo->type;
                    im->quality = qual;

                    inv->AddItemSorted(iObject);

                    FILE *source = fopen("petInfo.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText );
                    fprintf(source, "%s, ", tempText );
                    _strtime( tempText );
                    fprintf(source, "%s, ", tempText );

                    fprintf(source,"EGG LAY, %s, %s, %d, %d\n",
                        dragonInfo[qual][dInfo->type].eggName,
                        charInfoArray[curCharacterIndex].name,
                        cellX, cellY);

                    fclose(source);

                }

                // time for a change!
                dInfo->lifeStage += 1;

                MessGenericEffect messGE;
                messGE.avatarID = socketIndex;
                messGE.mobID    = -1;
                messGE.x        = cellX;
                messGE.y        = cellY;
                messGE.r        = 255;
                messGE.g        = 255;
                messGE.b        = 0;
                messGE.type     = 0;  // type of particles
                messGE.timeLen  = 5; // in seconds
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(messGE),(void *)&messGE);

                MessPet mPet;
                mPet.avatarID = socketIndex;
                CopyStringSafely(dInfo->name,16, 
                              mPet.name,16);
                mPet.quality = dInfo->quality;
                mPet.type    = dInfo->type;
                mPet.state   = dInfo->state;
                mPet.size    = dInfo->lifeStage + dInfo->healthModifier/10.0f;
                mPet.which   = i;

                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);

                sprintf(tempText,"%s has changed!", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            }
            else if (dInfo->age == (dInfo->lifeStage+1) * stageTime - 1 ||
                        dInfo->age == (dInfo->lifeStage+1) * stageTime - 2)
            {
                sprintf(tempText,"%s is acting strangely...", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            }
            else if (dInfo->age == (dInfo->lifeStage+1) * stageTime - 3)
            {
                sprintf(tempText,"%s just made a wierd noise...", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            }

            if (dInfo->age == 4 * stageTime - 2)
            {
                sprintf(tempText,"%s looks very old and sickly...", dInfo->name);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->SendToEveryoneNearBut(0, cellX, cellY,
                          sizeof(infoText),(void *)&infoText);

                dInfo->state = DRAGON_HEALTH_SICK;
            }

            if (oldState != dInfo->state)
            {
                MessPet mPet;
                mPet.avatarID = socketIndex;
                CopyStringSafely(dInfo->name,16, 
                              mPet.name,16);
                mPet.quality = dInfo->quality;
                mPet.type    = dInfo->type;
                mPet.state   = dInfo->state;
                mPet.size    = dInfo->lifeStage + dInfo->healthModifier/10.0f;
                mPet.which   = i;

                ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(mPet),(void *)&mPet);
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::AnnounceDisappearing(SharedSpace *sp, int type)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    // tell everyone I'm dissappearing

    if (SPECIAL_APP_NOTHING != type)
    {
        MessAvatarAppearSpecial messAvAppearSpecial;
        messAvAppearSpecial.avatarID = socketIndex;
        messAvAppearSpecial.x = cellX;
        messAvAppearSpecial.y = cellY;
        messAvAppearSpecial.typeOfAppearance = type;

        if (isInvisible)
            sp->lserver->SendMsg(sizeof(messAvAppearSpecial),
                                  (void *)&messAvAppearSpecial, 
                                       0, &tempReceiptList);
        else
            sp->SendToEveryoneNearBut(0, cellX, cellY,
                    sizeof(messAvAppearSpecial),(void *)&messAvAppearSpecial);
    }

    MessAvatarDisappear aDisappear;
    aDisappear.avatarID = socketIndex;
    aDisappear.x = cellX;
    aDisappear.y = cellY;

    if (isInvisible)
        sp->lserver->SendMsg(sizeof(aDisappear),
                              (void *)&aDisappear, 
                                   0, &tempReceiptList);
    else
        sp->SendToEveryoneNearBut(0, cellX, cellY, 
                               sizeof(aDisappear), &aDisappear);

    // stop any trading!
    charInfoArray[curCharacterIndex].inventory->partner = NULL;

}

//******************************************************************
void BBOSAvatar::AnnounceSpecial(SharedSpace *sp, int type)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    MessAvatarAppearSpecial messAvAppearSpecial;
    messAvAppearSpecial.avatarID = socketIndex;
    messAvAppearSpecial.x = cellX;
    messAvAppearSpecial.y = cellY;
    messAvAppearSpecial.typeOfAppearance = type;

    if (isInvisible)
        sp->lserver->SendMsg(sizeof(messAvAppearSpecial),
                              (void *)&messAvAppearSpecial, 
                                   0, &tempReceiptList);
    else
        sp->SendToEveryoneNearBut(0, cellX, cellY,
                sizeof(messAvAppearSpecial),(void *)&messAvAppearSpecial);
}

//******************************************************************
void BBOSAvatar::MoveControlledMonster(SharedSpace *ss, int deltaX, int deltaY)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    if (!controlledMonster->isMoving && ss->CanMove(controlledMonster->cellX, controlledMonster->cellY, 
                controlledMonster->cellX + deltaX, controlledMonster->cellY + deltaY))
    {
        controlledMonster->inventory->partner = NULL;

        // okay, let's go!
        controlledMonster->isMoving = TRUE;
        controlledMonster->targetCellX = controlledMonster->cellX + deltaX;
        controlledMonster->targetCellY = controlledMonster->cellY + deltaY;
        controlledMonster->moveStartTime = timeGetTime();

        MessMobBeginMove bMove;
        bMove.mobID = (unsigned long) controlledMonster;
        bMove.x = controlledMonster->cellX;
        bMove.y = controlledMonster->cellY;
        bMove.targetX = controlledMonster->cellX + deltaX;
        bMove.targetY = controlledMonster->cellY + deltaY;
        ss->SendToEveryoneNearBut(socketIndex, controlledMonster->cellX, 
                       controlledMonster->cellY, sizeof(bMove), &bMove);

        MessMobBeginMoveSpecial bMoveSpecial;
        bMoveSpecial.mobID = (unsigned long) controlledMonster;
        bMoveSpecial.x = controlledMonster->cellX;
        bMoveSpecial.y = controlledMonster->cellY;
        bMoveSpecial.targetX = controlledMonster->cellX + deltaX;
        bMoveSpecial.targetY = controlledMonster->cellY + deltaY;
        bMoveSpecial.specialType = 1;  // controlled monster
        ss->lserver->SendMsg(sizeof(bMoveSpecial),
                              (void *)&bMoveSpecial, 
                                   0, &tempReceiptList);
    }

}


//******************************************************************
void BBOSAvatar::AbortSecureTrading(SharedSpace *ss)
{
    // if the partner is valid
    if (tradingPartner)
    {
        SharedSpace *sx;
        BBOSAvatar *partnerAvatar = NULL;
        partnerAvatar = bboServer->FindAvatar(tradingPartner, &sx);

        if (partnerAvatar)
        {
            // make the partner's partner invalid
            partnerAvatar->tradingPartner = NULL;

            // call this function for the partner
            partnerAvatar->AbortSecureTrading(sx);
        }
    }

    // don't delete anything left in the trade inventory
    if (curCharacterIndex > -1)
    {
        InventoryObject *io = (InventoryObject *) trade->objects.First();
        while (io)
        {
            trade->objects.Remove(io);

            if (io->amount == 0)
            {
                delete io;
            }
            else
            {
                charInfoArray[curCharacterIndex].inventory->AddItemSorted(io);
            }

            io = (InventoryObject *) trade->objects.First();
        }

        assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

        charInfoArray[curCharacterIndex].inventory->money += trade->money;

        assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

        trade->money = 0;

        std::vector<TagID> tempReceiptList;
        tempReceiptList.clear();
        tempReceiptList.push_back(socketIndex);

        MessSecureTrade mess;
        mess.type = MESS_SECURE_STOP; 
        ss->lserver->SendMsg(sizeof(mess),(void *)&mess, 0, &tempReceiptList);
    }

    tradingPartner = NULL;

    assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

    bboServer->UpdateInventory(this);

    assert(charInfoArray[curCharacterIndex].inventory->money >= 0);
}

//******************************************************************
void BBOSAvatar::CompleteSecureTrading(SharedSpace *ss)
{
    assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

    // if the partner is valid
    if (tradingPartner)
    {
        SharedSpace *sx;
        BBOSAvatar *partnerAvatar = NULL;
        partnerAvatar = bboServer->FindAvatar(tradingPartner, &sx);

        if (partnerAvatar)
        {
            char tempText[1024];
            char dateString[128], timeString[128];

            _strdate( dateString );
            _strtime( timeString );

            FILE *source = fopen("logs\\tradeLog.txt","a");

            int iGave = FALSE;
            // give her my stuff
            InventoryObject *io = (InventoryObject *) trade->objects.First();
            while (io)
            {
                /* Display operating system-style date and time. */
                _strdate( tempText );
                fprintf(source, "%s, ", tempText );
                _strtime( tempText );
                fprintf(source, "%s, ", tempText );

                fprintf(source,"%d, %s, FROM %s, %s, %ld TO %s, %s, %ld\n",
                    io->amount, io->WhoAmI(),
                     name,
                     charInfoArray[curCharacterIndex].name,
                     charInfoArray[curCharacterIndex].inventory->money,
                     partnerAvatar->name,
                     partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                     partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].inventory->money);

                if (INVOBJ_BLADE == io->type)
                {
                    sprintf(tempText,"SECURE-TRADE, %d, %s, FROM, %s, %s, %s, %s, %s, %d, %d, TO, %s, %s\n", 
                                io->amount, io->WhoAmI(),
                                 charInfoArray[curCharacterIndex].name,
                                 name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), cellX, cellY,
                                 partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                                 partnerAvatar->name
                                  );
                        LogOutput("swordTransferLog.txt", tempText);
                }
                else if (INVOBJ_INGREDIENT == io->type)
                {
                    sprintf(tempText,"SECURE-TRADE, %d, %s, FROM, %s, %s, %s, %s, %s, %d, %d, TO, %s, %s\n", 
                                io->amount, io->WhoAmI(),
                                 charInfoArray[curCharacterIndex].name,
                                 name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), cellX, cellY,
                                 partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                                 partnerAvatar->name
                                  );
                        LogOutput("dustTransferLog.txt", tempText);
                }


                trade->objects.Remove(io);

                if (io->amount == 0)
                {
                    delete io;
                }
                else
                {
                    partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
                                         inventory->AddItemSorted(io);
                }

                iGave = TRUE;			
                io = (InventoryObject *) trade->objects.First();
            }


            QuestGiveGold(ss, partnerAvatar, trade->money);

            partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].inventory->money += 
                               trade->money;
            assert(
                partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].inventory->money >= 0);

            if (trade->money > 0)
                iGave = TRUE;
            trade->money = 0;

            int sheGave = FALSE;

            // give me her stuff
            io = (InventoryObject *) partnerAvatar->trade->objects.First();
            while (io)
            {
                /* Display operating system-style date and time. */
                _strdate( tempText );
                fprintf(source, "%s, ", tempText );
                _strtime( tempText );
                fprintf(source, "%s, ", tempText );

                fprintf(source,"%d, %s, FROM %s, %s, %ld TO %s, %s, %ld\n",
                    io->amount, io->WhoAmI(),
                     name,
                     charInfoArray[curCharacterIndex].name,
                     charInfoArray[curCharacterIndex].inventory->money,
                     partnerAvatar->name,
                     partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                     partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].inventory->money);

                if (INVOBJ_BLADE == io->type)
                {
                    sprintf(tempText,"SECURE-TRADE, %d, %s, FROM, %s, %s, %s, %s, %s, %d, %d, TO, %s, %s\n", 
                                io->amount, io->WhoAmI(),
                                 partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                                 partnerAvatar->name,
                                  dateString, timeString,
                                  ss->WhoAmI(), cellX, cellY,
                                 charInfoArray[curCharacterIndex].name,
                                 name
                                  );
                        LogOutput("swordTransferLog.txt", tempText);
                }
                else if (INVOBJ_INGREDIENT == io->type)
                {
                    sprintf(tempText,"SECURE-TRADE, %d, %s, FROM, %s, %s, %s, %s, %s, %d, %d, TO, %s, %s\n", 
                                io->amount, io->WhoAmI(),
                                 partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name,
                                 partnerAvatar->name,
                                  dateString, timeString,
                                  ss->WhoAmI(), cellX, cellY,
                                 charInfoArray[curCharacterIndex].name,
                                 name 
                                  );
                        LogOutput("dustTransferLog.txt", tempText);
                }


                partnerAvatar->trade->objects.Remove(io);

                if (io->amount == 0)
                {
                    delete io;
                }
                else
                {
                    charInfoArray[curCharacterIndex].
                                         inventory->AddItemSorted(io);
                }

                sheGave = TRUE;
                io = (InventoryObject *) partnerAvatar->trade->objects.First();
            }

            fclose(source);

            assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

            partnerAvatar->QuestGiveGold(ss, this, partnerAvatar->trade->money);

            charInfoArray[curCharacterIndex].inventory->money += 
                               partnerAvatar->trade->money;
            assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

            if (partnerAvatar->trade->money > 0)
                sheGave = TRUE;
         partnerAvatar->trade->money = 0;

            assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

            // KARMA
            int myRelationship, herRelationship;

            CompareWith(partnerAvatar, myRelationship, herRelationship);

            if (sheGave && !iGave)
            {
                partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
                      karmaGiven[herRelationship][SAMARITAN_TYPE_GIFT] += 1;
                charInfoArray[curCharacterIndex].
                      karmaReceived[myRelationship][SAMARITAN_TYPE_GIFT] += 1;

                partnerAvatar->LogKarmaExchange(
                           this, herRelationship, myRelationship, SAMARITAN_TYPE_GIFT);

                if (IsAGuildMate(partnerAvatar))
                {
                    partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
                          karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;
                    charInfoArray[curCharacterIndex].
                          karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;

                    partnerAvatar->LogKarmaExchange(
                         this, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_GIFT);
                }
            }
            if (!sheGave && iGave)
            {
                partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
                      karmaReceived[herRelationship][SAMARITAN_TYPE_GIFT] += 1;
                charInfoArray[curCharacterIndex].
                      karmaGiven[myRelationship][SAMARITAN_TYPE_GIFT] += 1;

                LogKarmaExchange(partnerAvatar,
                           myRelationship, herRelationship, SAMARITAN_TYPE_GIFT);

                if (IsAGuildMate(partnerAvatar))
                {
                    partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].
                          karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;
                    charInfoArray[curCharacterIndex].
                          karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_GIFT] += 1;

                    LogKarmaExchange(partnerAvatar,
                         SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, SAMARITAN_TYPE_GIFT);
                }
            }
            // END KARMA

            if (bHasLoaded)
            {
                SaveAccount();
                partnerAvatar->SaveAccount();
            }

            // make the partner's partner invalid
            partnerAvatar->tradingPartner = NULL;

            // call the abort function for the partner
            partnerAvatar->AbortSecureTrading(sx);

            assert(charInfoArray[curCharacterIndex].inventory->money >= 0);
        }
    }

    AbortSecureTrading(ss);

}


//******************************************************************
void BBOSAvatar::StateNoAgreement(SharedSpace *ss)
{
    agreeToTrade = FALSE;
    // if the partner is valid
    if (tradingPartner)
    {
        SharedSpace *sx;
        BBOSAvatar *partnerAvatar = NULL;
        partnerAvatar = bboServer->FindAvatar(tradingPartner, &sx);

        if (partnerAvatar)
        {
            partnerAvatar->agreeToTrade = FALSE;

            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessSecureTrade mess;
            mess.type = MESS_SECURE_NO_AGREEMENT; 
            ss->lserver->SendMsg(sizeof(mess),(void *)&mess, 0, &tempReceiptList);

            tempReceiptList.clear();
            tempReceiptList.push_back(partnerAvatar->socketIndex);
            sx->lserver->SendMsg(sizeof(mess),(void *)&mess, 0, &tempReceiptList);

            assert(charInfoArray[curCharacterIndex].inventory->money >= 0);

        }
    }
}

//******************************************************************
long BBOSAvatar::InventoryValue(void)
{
    float totalVal = 0;

    InventoryObject *io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].inventory->objects.First();
    while (io)
    {
        totalVal += io->amount * io->value;

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].inventory->objects.Next();
    }

    io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].wield->objects.First();
    while (io)
    {
        totalVal += io->amount * io->value;

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.Next();
    }

    io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].workbench->objects.First();
    while (io)
    {
        totalVal += io->amount * io->value;

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].workbench->objects.Next();
    }

    return (long) (totalVal/1000);
}


//******************************************************************
void BBOSAvatar::UseStaffOnMonster(SharedSpace *ss, InvStaff *iStaff, BBOSMonster *curMonster)
{
    MessMonsterHealth messMH;

    // 1. chance = 2d20 + Blade * Physical + weapon.ToHit - monster.Defense
    int cVal = iStaff->quality * 5 - 
                     curMonster->defense + MagicalStat() +
                     totemEffects.effect[TOTEM_ACCURACY] - iStaff->imbueDeviation;
    int chance = (rand() % 20) + 2 + (rand() % 20) + cVal;

    if (StaffAffectsArea(iStaff))
        chance -= 4;

    if (cVal + 40 <= 20) // if you can't EVER hit
    {
        if (6 == (rand() % 20)) // 1-in-20 chance of hitting anyway
            chance = 30;  // Hit!
    }

    // 2. if chance > 20, hit was successful
//	if (chance > 20)



    // always hit!
    {
        int effectVal = 3 + iStaff->quality * 5 - iStaff->imbueDeviation;
        if (effectVal < 0)
            effectVal = 0;
        long damValue = 0;

        if (specLevel[1] > 0)
            effectVal += specLevel[1]*2;

        switch (iStaff->type)
        {
        case STAFF_DAMAGE:
        case STAFF_AREA_DAMAGE:

            // 3. damage = Physical * weapon.Damage
            damValue = (iStaff->quality + 1) * (10 - iStaff->imbueDeviation) * 
                                (1.0f + MagicalStat() * 0.5f);

            if (StaffAffectsArea(iStaff))
                damValue = (long)(damValue * 0.6f);

            if (specLevel[1] > 0)
                damValue = damValue * (1.0f + 0.05f * specLevel[1]);

            curMonster->health -= damValue;
            curMonster->RecordDamageForLootDist(damValue, this);

            messMH.mobID = (unsigned long)curMonster;
            messMH.health = curMonster->health;
            messMH.healthMax = curMonster->maxHealth;
            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messMH),(void *)&messMH, 2);

            break;

        case STAFF_BLIND:
        case STAFF_AREA_BLIND:
            curMonster->MonsterMagicEffect(DRAGON_TYPE_WHITE, 
                effectVal * 1000, effectVal);

            break;

        case STAFF_SLOW:
        case STAFF_AREA_SLOW:
            curMonster->MonsterMagicEffect(DRAGON_TYPE_BLUE, 
                effectVal * 1000, effectVal);

            break;

        case STAFF_POISON:
        case STAFF_AREA_POISON:
            curMonster->MonsterMagicEffect(DRAGON_TYPE_BLACK, 
                effectVal * 1000, effectVal);

            curMonster->RecordDamageForLootDist(effectVal * effectVal / 20, this);

            break;

        case STAFF_STUN:
        case STAFF_AREA_STUN:
            curMonster->MonsterMagicEffect(MONSTER_EFFECT_STUN, 
                effectVal * 1000, effectVal);

            break;

        case STAFF_BIND:
        case STAFF_AREA_BIND:
            curMonster->MonsterMagicEffect(MONSTER_EFFECT_BIND, 
                effectVal * 1000, effectVal);

            break;
        }

        MessMagicAttack messMA;
        messMA.damage = damValue;
        messMA.mobID = (unsigned long) curMonster;
        messMA.avatarID = -1;
        messMA.type = iStaff->type;
        ss->SendToEveryoneNearBut(0, curMonster->cellX, curMonster->cellY, 
                     sizeof(messMA), &messMA,3);

        if (curMonster->health <= 0)
        {
            curMonster->isDead = TRUE;
            curMonster->bane = this;

            curMonster->HandleQuestDeath();

            if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
            {
                MapListState oldState = ss->mobList->GetState();

                BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                army->MonsterEvent(curMonster, ARMY_EVENT_DIED, cellX, cellY);

                ss->mobList->SetState(oldState);
            }
            else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
            {
                MapListState oldState = ss->mobList->GetState();

                BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                quest->MonsterEvent(curMonster, AUTO_EVENT_DIED, cellX, cellY);

                ss->mobList->SetState(oldState);
            }
        }
        else
        {
            if (!(curMonster->curTarget) || 
                                 (24 == curMonster->type && 3 == (rand() % 10))
             )
            {
                curMonster->lastAttackTime = timeGetTime();
                curMonster->curTarget = this;
            }

            if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
            {
                MapListState oldState = ss->mobList->GetState();

                BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                army->MonsterEvent(curMonster, ARMY_EVENT_ATTACKED, cellX, cellY);

                ss->mobList->SetState(oldState);
            }
            else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
            {
                MapListState oldState = ss->mobList->GetState();

                BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                quest->MonsterEvent(curMonster, AUTO_EVENT_ATTACKED, cellX, cellY);

                ss->mobList->SetState(oldState);
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::DoBladestaffExtra(SharedSpace *ss, InvBlade *ib, 
                                              long damValue, BBOSMonster *targetMonster)
{
    char tempText[1024];
    MessInfoText infoText;
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(socketIndex);

    MessMonsterHealth messMH;
    BBOSMob *curMob = NULL;

    MapListState oldState2 = ss->mobList->GetState();

    unsigned short bladePoison, bladeSlow, bladeBlind;
    bladePoison = ib->poison;
    bladeSlow   = ib->slow;
    bladeBlind  = ib->blind;

    curMob = ss->mobList->GetFirst(cellX, cellY);
    while (curMob)
    {
        BBOSMonster *curMonster = (BBOSMonster *) curMob;

        if (SMOB_MONSTER == curMob->WhatAmI() &&	targetMonster != curMonster)
        {

            curMonster->health -= damValue;
            curMonster->RecordDamageForLootDist(damValue, this);

            // give experience for monster damage!

            MessMonsterHealth messMH;
            messMH.mobID = (unsigned long)curMonster;
            messMH.health = curMonster->health;
            messMH.healthMax = curMonster->maxHealth;
            ss->SendToEveryoneNearBut(0, cellX, cellY, sizeof(messMH),(void *)&messMH, 2);

            if (curMonster->health <= 0)
            {
                curMonster->isDead = TRUE;
                curMonster->bane = this;

                curMonster->HandleQuestDeath();

                if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                {
                    MapListState oldState = ss->mobList->GetState();

                    BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                    army->MonsterEvent(curMonster, ARMY_EVENT_DIED);

                    ss->mobList->SetState(oldState);
                }
                else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                {
                    MapListState oldState = ss->mobList->GetState();

                    BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                    quest->MonsterEvent(curMonster, AUTO_EVENT_DIED);

                    ss->mobList->SetState(oldState);
                }
                // give experience for monster death!
            }
            else
            {
                if (bladeSlow > 0)
                {
                    int chance = bladeSlow;

                    if (chance + (rand() % 20) - curMonster->defense > 10)
                    {
                        curMonster->MonsterMagicEffect(DRAGON_TYPE_BLUE, 
                            bladeSlow * 1000, bladeSlow);

                        if (infoFlags & INFO_FLAGS_HITS)
                        {
                            sprintf(tempText,"The %s is Slowed.",
                                curMonster->Name());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
                if (bladePoison > 0)
                {
                    int chance = bladePoison;

                    if (chance + (rand() % 20) - curMonster->defense > 10)
                    {
                        curMonster->MonsterMagicEffect(DRAGON_TYPE_BLACK, 
                            bladePoison * 1000, bladePoison);

                        curMonster->RecordDamageForLootDist(bladePoison * bladePoison / 40, this);

                        if (infoFlags & INFO_FLAGS_HITS)
                        {
                            sprintf(tempText,"The %s is Poisoned.",
                                curMonster->Name());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
                if (bladeBlind > 0)
                {
                    int chance = bladeBlind;

                    if (chance + (rand() % 20) - curMonster->defense > 10)
                    {
                        curMonster->MonsterMagicEffect(DRAGON_TYPE_WHITE, 
                            bladeBlind * 1000, bladeBlind);

                        if (infoFlags & INFO_FLAGS_HITS)
                        {
                            sprintf(tempText,"The %s is Blinded.",
                                curMonster->Name());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }

                if (!(curMonster->curTarget) || 
                     (24 == curMonster->type && 3 == (rand() % 10))
                    )
                {
                    curMonster->lastAttackTime = timeGetTime();
                    curMonster->curTarget = this;
                }

                if (curMonster->myGenerator && 1 == curMonster->myGenerator->WhatAmI())
                {
                    MapListState oldState = ss->mobList->GetState();

                    BBOSArmy *army = (BBOSArmy *)	curMonster->myGenerator;
                    army->MonsterEvent(curMonster, ARMY_EVENT_ATTACKED);

                    ss->mobList->SetState(oldState);
                }
                else if (curMonster->myGenerator && 2 == curMonster->myGenerator->WhatAmI())
                {
                    MapListState oldState = ss->mobList->GetState();

                    BBOSAutoQuest *quest = (BBOSAutoQuest *) curMonster->myGenerator;
                    quest->MonsterEvent(curMonster, AUTO_EVENT_ATTACKED);

                    ss->mobList->SetState(oldState);
                }
            }
        }

        curMob = ss->mobList->GetNext();
    }

    ss->mobList->SetState(oldState2);
}

//******************************************************************
void BBOSAvatar::CompareWith(BBOSAvatar *other, int &myRelationship, int &otherRelationship)
{

    int meVal = 0;
    int otherVal = 0;

    // lifeTime is in 5 minute increments

    if (charInfoArray[curCharacterIndex].lifeTime > 20)
        ++meVal;
    if (charInfoArray[curCharacterIndex].lifeTime > 100)
        ++meVal;
    if (charInfoArray[curCharacterIndex].lifeTime > 200)
        ++meVal;
    if (charInfoArray[curCharacterIndex].lifeTime > 1000)
        ++meVal;
    if (charInfoArray[curCharacterIndex].lifeTime > 3000)
        ++meVal;

    if (other->charInfoArray[other->curCharacterIndex].lifeTime > 20)
        ++otherVal;
    if (other->charInfoArray[other->curCharacterIndex].lifeTime > 100)
        ++otherVal;
    if (other->charInfoArray[other->curCharacterIndex].lifeTime > 200)
        ++otherVal;
    if (other->charInfoArray[other->curCharacterIndex].lifeTime > 1000)
        ++otherVal;
    if (other->charInfoArray[other->curCharacterIndex].lifeTime > 3000)
        ++otherVal;

    if (meVal == otherVal)
    {
        myRelationship    = SAMARITAN_REL_PEER;
        otherRelationship = SAMARITAN_REL_PEER;
        return;
    }
    else if (meVal > otherVal)
    {
        myRelationship    = SAMARITAN_REL_OLDER;
        otherRelationship = SAMARITAN_REL_YOUNGER;
        return;
    }

    myRelationship    = SAMARITAN_REL_YOUNGER;
    otherRelationship = SAMARITAN_REL_OLDER;

}

//******************************************************************
int BBOSAvatar::IsAGuildMate(BBOSAvatar *other)
{

    SharedSpace *sx = NULL, *sx2 = NULL;

    bboServer->FindAvatarInGuild(charInfoArray[curCharacterIndex].name, &sx);

    bboServer->FindAvatarInGuild(other->charInfoArray[other->curCharacterIndex].name, &sx2);

    if (sx != NULL && sx2 == sx)
        return TRUE;
    return FALSE;
    
    
}
    
//******************************************************************
void BBOSAvatar::LogKarmaExchange(BBOSAvatar *receiver, 
                                             int myRel, int receiverRel, int exchangeType,
                                             char *originalText)
{
    char tempText[1024];
    LongTime lt;

    sprintf(tempText,"%d/%02d, %d:%02d,    ", (int)lt.value.wMonth, (int)lt.value.wDay, 
              (int)lt.value.wHour, (int)lt.value.wMinute);
    LogOutput("karma.txt", tempText);

    sprintf(tempText,"%s, ", charInfoArray[curCharacterIndex].name);
    LogOutput("karma.txt", tempText);

    switch(myRel)
    {
    case SAMARITAN_REL_OLDER:
        LogOutput("karma.txt", "ELDER, ");
        break;

    case SAMARITAN_REL_PEER:
        LogOutput("karma.txt", "PEER, ");
        break;

    case SAMARITAN_REL_YOUNGER:
        LogOutput("karma.txt", "YOUNGER, ");
        break;

    case SAMARITAN_REL_GUILD:
        LogOutput("karma.txt", "GUILDMATE, ");
        break;

    default:
        LogOutput("karma.txt", "NO_RELATIONSHIP, ");
        break;
    }

    switch(exchangeType)
    {
    case SAMARITAN_TYPE_THANKS:
        LogOutput("karma.txt", "SAYS_THANKS_TO, ");
        break;

    case SAMARITAN_TYPE_PLEASE:
        LogOutput("karma.txt", "SAYS_PLEASE_TO, ");
        break;

    case SAMARITAN_TYPE_WELCOME:
        LogOutput("karma.txt", "SAYS_WELCOME_TO, ");
        break;

    case SAMARITAN_TYPE_GIFT:
        LogOutput("karma.txt", "GIVES_SECURE_GIFT_TO, ");
        break;

    case SAMARITAN_TYPE_CASH:
        LogOutput("karma.txt", "GIVES_CASH_TO, ");
        break;

    default:
        LogOutput("karma.txt", "NO_ACTION, ");
        break;
    }


    sprintf(tempText,"%s, ", receiver->charInfoArray[receiver->curCharacterIndex].name);
    LogOutput("karma.txt", tempText);

    switch(receiverRel)
    {
    case SAMARITAN_REL_OLDER:
        LogOutput("karma.txt", "ELDER, ");
        break;

    case SAMARITAN_REL_PEER:
        LogOutput("karma.txt", "PEER, ");
        break;

    case SAMARITAN_REL_YOUNGER:
        LogOutput("karma.txt", "YOUNGER, ");
        break;

    case SAMARITAN_REL_GUILD:
        LogOutput("karma.txt", "GUILDMATE, ");
        break;

    default:
        LogOutput("karma.txt", "NO_RELATIONSHIP, ");
        break;
    }

    if (originalText)
    {
        sprintf(tempText, originalText);
        RemoveCommasAndReturnsFromString(tempText);
        LogOutput("karma.txt", tempText);
    }
    else
        LogOutput("karma.txt", "NO_TEXT");

    LogOutput("karma.txt", "\n");
}
    
//******************************************************************
float BBOSAvatar::BestSwordRating(void)
{
    float bestVal = 0, curVal;
    InvBlade *iBlade;

    InventoryObject *io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].inventory->objects.First();
    while (io)
    {

        if (INVOBJ_BLADE == io->type)
        {
            curVal = 0;
            iBlade = (InvBlade *)io->extra;
            curVal += ((InvBlade *)io->extra)->toHit;
            curVal += ((InvBlade *)io->extra)->damageDone;

            curVal += ((InvBlade *)io->extra)->poison;
            curVal += ((InvBlade *)io->extra)->heal;
            curVal += ((InvBlade *)io->extra)->slow;
            curVal += ((InvBlade *)io->extra)->blind;
            if (curVal > bestVal)
                bestVal = curVal;
        }

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].inventory->objects.Next();
    }

    io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].wield->objects.First();
    while (io)
    {

        if (INVOBJ_BLADE == io->type)
        {
            curVal = 0;
            iBlade = (InvBlade *)io->extra;
            curVal += ((InvBlade *)io->extra)->toHit;
            curVal += ((InvBlade *)io->extra)->damageDone;

            curVal += ((InvBlade *)io->extra)->poison;
            curVal += ((InvBlade *)io->extra)->heal;
            curVal += ((InvBlade *)io->extra)->slow;
            curVal += ((InvBlade *)io->extra)->blind;
            if (curVal > bestVal)
                bestVal = curVal;
        }

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.Next();
    }

    io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].workbench->objects.First();
    while (io)
    {

        if (INVOBJ_BLADE == io->type)
        {
            curVal = 0;
            iBlade = (InvBlade *)io->extra;
            curVal += ((InvBlade *)io->extra)->toHit;
            curVal += ((InvBlade *)io->extra)->damageDone;

            curVal += ((InvBlade *)io->extra)->poison;
            curVal += ((InvBlade *)io->extra)->heal;
            curVal += ((InvBlade *)io->extra)->slow;
            curVal += ((InvBlade *)io->extra)->blind;
            if (curVal > bestVal)
                bestVal = curVal;
        }

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].workbench->objects.Next();
    }

    return bestVal;
}

//******************************************************************
void BBOSAvatar::QuestMovement(SharedSpace *ss)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_GOTO == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_LOCATION == qt->type)
                        {
                            if (cellX == qt->x && cellY == qt->y && 
                                 SPACE_GROUND == ss->WhatAmI())
                            {
                                q->completeVal = 10000; // finished!

                                if (q->questSource < MAGIC_MAX)
                                {
                                    sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                    sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                                else
                                {
                                    questMan->ProcessWitchQuest(this, ss, q);
                                }
                            }
                        }
                    }
                }
                else if (QUEST_VERB_KILL == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_LOCATION == qt->type)
                        {
                            DungeonMap *dm = (DungeonMap *) ss;
                            if (abs(cellX - qt->x) < 4 && abs(cellY - qt->y) < 4 && 
                                 0 == q->completeVal && SPACE_DUNGEON == ss->WhatAmI() &&
                                 qt->mapSubType == GetCRCForString(dm->name))
                            {
                                q->completeVal = 1; // made monster; don't make it again.

                                BBOSMonster *monster;
                                if (0 == qt->monsterType)
                                {
                                    sprintf(tempText,"You feel the presence of the hellspawn in the distance.");
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                    monster = new BBOSMonster(2,5,NULL);
                                    monster->cellX = monster->targetCellX = monster->spawnX = qt->x;
                                    monster->cellY = monster->targetCellY = monster->spawnY = qt->y;
                                    ss->mobList->Add(monster);
                                    sprintf(monster->uniqueName, "Demon Prince");

                                    //	adjusts its power to match player power
                                    monster->r               = 255;
                                    monster->g               = 0;
                                    monster->b               = 0;
                                    monster->a               = 255;
                                    monster->sizeCoeff       = 1.5f + 1.0f * qt->monsterSubType /800;
                                    monster->health          = 40 * qt->monsterSubType;
                                    monster->maxHealth       = 40 * qt->monsterSubType;
                                    monster->damageDone      = qt->monsterSubType/20;
                                    if (monster->damageDone > 45)
                                        monster->damageDone = 45;

                                    monster->toHit           = qt->monsterSubType/14;
                                    monster->defense         = qt->monsterSubType/14;
                                    monster->dropAmount      = qt->monsterSubType/100;
                                    monster->magicResistance = qt->monsterSubType/20/100;
                                    if (monster->magicResistance > 1)
                                        monster->magicResistance = 1;

                                    monster->healAmountPerSecond = qt->monsterSubType/3;

                                }
                                else
                                {
                                    sprintf(tempText,"You smell the stench of the possessed beast in the distance.");
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                    monster = new BBOSMonster(
                                               pmTypes[qt->monsterType-1].type, 
                                         pmTypes[qt->monsterType-1].subType,NULL);
                                    monster->cellX = monster->targetCellX = monster->spawnX = qt->x;
                                    monster->cellY = monster->targetCellY = monster->spawnY = qt->y;
                                    ss->mobList->Add(monster);
                                    sprintf(tempText,"%s - %s", pmTypes[qt->monsterType-1].name, 
                                                charInfoArray[curCharacterIndex].name);
                                    CopyStringSafely(tempText, 1024, 
                                                      monster->uniqueName, 31);

                                    //	adjusts its power to match player power
                                    monster->r               = 255;
                                    monster->g               = 0;
                                    monster->b               = 0;
                                    monster->a               = 255;
                                    monster->sizeCoeff       = 1.5f + 1.0f * qt->monsterSubType /800;
                                    monster->health          = 40 * qt->monsterSubType;
                                    monster->maxHealth       = 40 * qt->monsterSubType;
                                    monster->damageDone      = qt->monsterSubType/20;
                                    if (monster->damageDone > 55)
                                        monster->damageDone = 55;

                                    monster->toHit           = qt->monsterSubType/14;
                                    monster->defense         = qt->monsterSubType/14;
                                    monster->dropAmount      = qt->monsterSubType/100;
                                    monster->magicResistance = qt->monsterSubType/20/100;
                                    if (monster->magicResistance > 1)
                                        monster->magicResistance = 1;

                                    monster->healAmountPerSecond = qt->monsterSubType/3;

                                    monster->isPossessed = TRUE;

                                    monster->AddPossessedLoot(qt->monsterSubType/150+qt->range);
                                }

                                // tell everyone about me
                                MessMobAppearCustom mAppear;
                                mAppear.type = SMOB_MONSTER;
                                mAppear.mobID = (unsigned long) monster;
                                mAppear.x = monster->cellX;
                                mAppear.y = monster->cellY;
                                mAppear.monsterType = monster->type;
                                mAppear.subType = monster->subType;
                                CopyStringSafely(monster->Name(), 32, mAppear.name, 32);
                                mAppear.a = monster->a;
                                mAppear.r = monster->r;
                                mAppear.g = monster->g;
                                mAppear.b = monster->b;
                                mAppear.sizeCoeff = monster->sizeCoeff;

                                mAppear.staticMonsterFlag = FALSE;

                                ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY,
                                         sizeof(mAppear),(void *)&mAppear);
                            }
                            else if (abs(cellX - qt->x) < 4 && abs(cellY - qt->y) < 4 && 
                                 0 == q->completeVal && SPACE_GROUND == ss->WhatAmI())
                            {
                                q->completeVal = 1; // made monster; don't make it again.

                                qt->monsterType    = Bracket(qt->monsterType   , 0, NUM_OF_MONSTERS-1);
                                qt->monsterSubType = Bracket(qt->monsterSubType, 0, NUM_OF_MONSTER_SUBTYPES-1);
                                qt->range = Bracket(qt->range,1,100000);

                                BBOSMonster *monster;
                                sprintf(tempText,"You see a creature in the distance.");
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                monster = new BBOSMonster(qt->monsterType,qt->monsterSubType,NULL);
                                monster->cellX = monster->targetCellX = monster->spawnX = qt->x;
                                monster->cellY = monster->targetCellY = monster->spawnY = qt->y;
                                ss->mobList->Add(monster);
                                sprintf(monster->uniqueName, qt->otherName);

                                
                                //	adjust its power
                                monster->sizeCoeff       = 1.5f + 1.0f * qt->range/100.0f /800;
                                monster->health          = 40 * qt->range/100.0f;
                                monster->maxHealth       = 40 * qt->range/100.0f;
                                monster->damageDone      = qt->range/100.0f/20;

                                monster->toHit           = qt->range/100.0f/14;
                                monster->defense         = qt->range/100.0f/14;
                                monster->dropAmount      = qt->range/100.0f/100;
                                monster->magicResistance = qt->range/100.0f/20/100;
                                if (monster->magicResistance > 1)
                                    monster->magicResistance = 1;

                                monster->healAmountPerSecond = qt->range/100.0f/3;
                                
                                // tell everyone about me
                                MessMobAppearCustom mAppear;
                                mAppear.type = SMOB_MONSTER;
                                mAppear.mobID = (unsigned long) monster;
                                mAppear.x = monster->cellX;
                                mAppear.y = monster->cellY;
                                mAppear.monsterType = monster->type;
                                mAppear.subType = monster->subType;
                                CopyStringSafely(monster->Name(), 32, mAppear.name, 32);
                                mAppear.a = monster->a;
                                mAppear.r = monster->r;
                                mAppear.g = monster->g;
                                mAppear.b = monster->b;
                                mAppear.sizeCoeff = monster->sizeCoeff;

                                mAppear.staticMonsterFlag = FALSE;

                                ss->SendToEveryoneNearBut(0, monster->cellX, monster->cellY,
                                         sizeof(mAppear),(void *)&mAppear);
                            }
                        }
                    }
                }
                else if (QUEST_VERB_RETRIEVE == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_LOCATION == qt->type)
                        {
                            if (cellX == qt->x && cellY == qt->y && 
                                 SPACE_REALM == ss->WhatAmI() && 
                                 qt->mapSubType == ((RealmMap *)ss)->type &&
                                 q->questSource < MAGIC_MAX &&
                                 0 == q->completeVal)
                            {
                                sprintf(tempText,"There's something unusual on the ground here...");
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                InventoryObject *iObject = 
                                        new InventoryObject(INVOBJ_SIMPLE,0,
                                                             questRetrieveTypeDesc[q->questSource]);
                                iObject->mass = 1.0f;
                                iObject->value = 10;
                                iObject->amount = 1;
                                iObject->status = INVSTATUS_QUEST_ITEM;

                                RealmMap *rp = (RealmMap *) ss;

                                Inventory *inv = rp->GetGroundInventory(qt->x, qt->y);
                                inv->objects.Append(iObject);

                                MessMobAppear messMA;
                                messMA.mobID = (unsigned long) inv;
                                messMA.type = SMOB_ITEM_SACK;
                                messMA.x = qt->x;
                                messMA.y = qt->y;
                                ss->SendToEveryoneNearBut(0, qt->x, qt->y,
                                                sizeof(messMA),(void *)&messMA);

                                q->completeVal += 1;

                            }
                        }
                        else if (QUEST_TARGET_NPC == qt->type)
                        {
                            if (qt->monsterType < MAGIC_MAX && SPACE_GROUND == ss->WhatAmI())
                            {
                                if (cellX == greatTreePos[qt->monsterType][0] && 
                                     cellY == greatTreePos[qt->monsterType][1])
                                {

                                    sprintf(tempText,"The Great Tree gives you something.");
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                    InventoryObject *iObject = 
                                             new InventoryObject(INVOBJ_SIMPLE,0,
                                                                        questRetrieveTypeDesc[q->questSource]);
                                    iObject->mass = 1.0f;
                                    iObject->value = 10;
                                    iObject->amount = 1;
                                    iObject->status = INVSTATUS_QUEST_ITEM;

                                    charInfoArray[curCharacterIndex].inventory->objects.Prepend(iObject);

                                    q->completeVal = 10000; // finished!

                                    sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    if (q->questSource < MAGIC_MAX)
                                    {
                                        sprintf(tempText,"***** Return to the Great Tree of the %s to give the item to it.  ", magicNameList[q->questSource]);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestTime(SharedSpace *ss)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal)// &&
//			 10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            LongTime now;
            if (now.MinutesDifference(&q->timeLeft) <= 0)
            {
                q->EmptyOut();
                sprintf(tempText,"***** Quest %d has expired.", i + 1);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            else
            {
                /*
                QuestPart *qp = q->GetVerb();
                if (qp)
                {
                    if (QUEST_VERB_GOTO == qp->type)
                    {
                        QuestPart *qt = q->GetTarget();
                        if (qt)
                        {
                            if (QUEST_TARGET_LOCATION == qp->type)
                            {
                                if (cellX == qp->x && cellY == qp->y)
                                {
                                    q->completeVal = 10000; // finished!

                                    CopyStringSafely("*****", 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    sprintf(tempText,"Quest %d has been completed!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    CopyStringSafely("*****", 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                        }
                        
                    }
                }
                */
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestMonsterKill(SharedSpace *ss, BBOSMonster *deadMonster)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_KILL == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_MONSTER_TYPE == qt->type)
                        {
                            if (qt->monsterType == deadMonster->type &&
                                 (qt->monsterSubType == deadMonster->subType ||
                                  -1 == qt->monsterSubType)
                                )
                            {
                                q->completeVal = 10000; // finished!

                                if (q->questSource < MAGIC_MAX)
                                {
                                    sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                    sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                                else
                                {
                                    questMan->ProcessWitchQuest(this, ss, q);
                                }
                            }
                        }
                        else if (QUEST_TARGET_LOCATION == qt->type)
                        {
                            DungeonMap *dm = (DungeonMap *) ss;

                            if (2 == deadMonster->type &&
                                 5 == deadMonster->subType && 0 == qt->monsterType &&
                                 cellX == qt->x && cellY == qt->y
                                )
                            {
                                q->completeVal = 10000; // finished!

                                sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                if (q->questSource < MAGIC_MAX)
                                {
                                    sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                                else
                                {
                                    questMan->ProcessWitchQuest(this, ss, q);
                                }
                            }
                            else if (!strncmp(deadMonster->uniqueName,"Possessed", 9) && 
                                 qt->monsterType > 0 &&
                                 qt->mapSubType == GetCRCForString(dm->name))
                            {
                                q->EmptyOut(); // finished!

                                sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else if (IsSame(deadMonster->uniqueName,qt->otherName) && 
                                 qt->monsterType == deadMonster->type &&
                                 qt->monsterSubType == deadMonster->subType)
                            {
                                questMan->ProcessWitchQuest(this, ss, q);
                            }
                        }
                    }
                }
            }
        }
    }
}


//Test possessed quest monster dies, gives loot, and finishes quest entry
//Add generator for nihgttime vampires near towns


//******************************************************************
void BBOSAvatar::QuestTalk(SharedSpace *ss)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_VISIT == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_PLAYER == qt->type)
                        {
                            BBOSAvatar *other = (BBOSAvatar *)ss->avatars->First();
                            while (other)
                            {
                                if (other != this && other->cellX == cellX && other->cellY == cellY)
                                {
                                    int found = FALSE;

                                    if (QUEST_PLAYER_TYPE_FIGHTER == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].physical >
                                          other->charInfoArray[other->curCharacterIndex].magical &&
                                          other->charInfoArray[other->curCharacterIndex].physical >
                                          other->charInfoArray[other->curCharacterIndex].creative)
                                        found = TRUE;
                                    if (QUEST_PLAYER_TYPE_MAGE == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].magical >
                                          other->charInfoArray[other->curCharacterIndex].physical &&
                                          other->charInfoArray[other->curCharacterIndex].magical >
                                          other->charInfoArray[other->curCharacterIndex].creative)
                                        found = TRUE;
                                    if (QUEST_PLAYER_TYPE_CRAFTER == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].creative >
                                          other->charInfoArray[other->curCharacterIndex].magical &&
                                          other->charInfoArray[other->curCharacterIndex].creative >
                                          other->charInfoArray[other->curCharacterIndex].physical)
                                        found = TRUE;
                                    if (QUEST_PLAYER_TYPE_BALANCED == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].creative ==
                                          other->charInfoArray[other->curCharacterIndex].magical &&
                                          other->charInfoArray[other->curCharacterIndex].creative ==
                                          other->charInfoArray[other->curCharacterIndex].physical)
                                        found = TRUE;
                                    if (QUEST_PLAYER_TYPE_YOUNG == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].lifeTime < 12)
                                        found = TRUE;
                                    if (QUEST_PLAYER_TYPE_POOR == qt->playerType &&
                                          other->charInfoArray[other->curCharacterIndex].
                                                     inventory->money < 100)
                                        found = TRUE;

                                    if (found)
                                    {
                                        q->completeVal = 10000; // finished!

                                        sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                        if (q->questSource < MAGIC_MAX)
                                        {
                                            sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                        }
                                        else
                                        {
                                            questMan->ProcessWitchQuest(this, ss, q);
                                        }

                                        return;
                                    }
                                }
                                other = (BBOSAvatar *)ss->avatars->Next();
                            }
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestCraftWeapon(SharedSpace *ss, InventoryObject *io, 
                                             float challenge, float work, char *skillName)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_CRAFT == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_WEAPON == qt->type)
                        {
                            int correctSkill = FALSE;

                            if (!strcmp(skillName,"Swordsmith") && 
                                  QUEST_WEAPON_TYPE_SWORD == qt->playerType)
                                correctSkill = TRUE;

                            if (!strcmp(skillName,"Katana Expertise") && 
                                  QUEST_WEAPON_TYPE_KATANA == qt->playerType)
                                correctSkill = TRUE;
     
                            if (!strcmp(skillName,"Claw Expertise") && 
                                  QUEST_WEAPON_TYPE_CLAWS == qt->playerType)
                                correctSkill = TRUE;
       
                            if (!strcmp(skillName,"Chaos Expertise") && 
                                  QUEST_WEAPON_TYPE_CHAOS == qt->playerType)
                                correctSkill = TRUE;
      
                            if (!strcmp(skillName,"Bladestaff Expertise") && 
                                  QUEST_WEAPON_TYPE_DOUBLE == qt->playerType)
                                correctSkill = TRUE;
 
                            if (!strcmp(skillName,"Mace Expertise") && 
                                  QUEST_WEAPON_TYPE_MACE == qt->playerType)
                                correctSkill = TRUE;

                            if (correctSkill)
                            {
                                if (challenge > work / 2)
                                {
                                    q->completeVal = 10000; // finished!
                
                                    io->status = INVSTATUS_QUEST_ITEM;

                                    sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    if (q->questSource < MAGIC_MAX)
                                    {
                                        sprintf(tempText,"***** Return to the Great Tree of the %s to give the item to it.  ", magicNameList[q->questSource]);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                    else
                                    {
                                        questMan->ProcessWitchQuest(this, ss, q);
                                    }
            
                                    return;
                                }
                                else
                                {
                                    sprintf(tempText,"That crafting attempt was too easy to complete Quest %d.", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            
//									return;
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestImbue(SharedSpace *ss, InventoryObject *io, 
                                      float challenge, float work, char *skillName)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_IMBUE == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_TOTEM == qt->type && INVOBJ_TOTEM == io->type)
                        {
                            InvTotem *extra = (InvTotem *)io->extra;

                            if (0 == extra->imbueDeviation && extra->type == qt->playerType)
                            {
                                if (challenge > work / 2)
                                {
                                    q->completeVal = 10000; // finished!
                
                                    io->status = INVSTATUS_QUEST_ITEM;

                                    sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    if (q->questSource < MAGIC_MAX)
                                    {
                                        sprintf(tempText,"***** Return to the Great Tree of the %s to give the item to it.  ", magicNameList[q->questSource]);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                    else
                                    {
                                        questMan->ProcessWitchQuest(this, ss, q);
                                    }

                                    return;
                                }
                                else
                                {
                                    sprintf(tempText,"That imbuing task was too easy to complete Quest %d.", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            
                                    return;
                                }
                            }
                        }
                        else if (QUEST_TARGET_STAFF == qt->type && INVOBJ_STAFF == io->type)
                        {
                            InvStaff *extra = (InvStaff *)io->extra;

                            if (0 == extra->imbueDeviation && extra->type == qt->playerType)
                            {
                                if (challenge > work / 2)
                                {
                                    q->completeVal = 10000; // finished!

                                    io->status = INVSTATUS_QUEST_ITEM;
                
                                    sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    if (q->questSource < MAGIC_MAX)
                                    {
                                        sprintf(tempText,"***** Return to the Great Tree of the %s to give the item to it.  ", magicNameList[q->questSource]);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                    else
                                    {
                                        questMan->ProcessWitchQuest(this, ss, q);
                                    }
            
                                    return;
                                }
                                else
                                {
                                    sprintf(tempText,"That imbuing task was too easy to complete Quest %d.", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            
                                    return;
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
int BBOSAvatar::SacrificeQuestItem(int type, int subType)
{
    int retVal;
    retVal = SacrificeQuestItemFrom(type, subType, charInfoArray[curCharacterIndex].inventory);
    if (retVal)
        return retVal;
    retVal = SacrificeQuestItemFrom(type, subType, charInfoArray[curCharacterIndex].wield);
    if (retVal)
        return retVal;
    retVal = SacrificeQuestItemFrom(type, subType, charInfoArray[curCharacterIndex].workbench);
    if (retVal)
        return retVal;

    return FALSE;
}

//******************************************************************
int BBOSAvatar::SacrificeQuestItemFrom(int type, int subType, Inventory *inv)
{
    InventoryObject *io = (InventoryObject *) inv->objects.First();
    while (io)
    {
        if (INVSTATUS_QUEST_ITEM == io->status)
        {
            if (INVOBJ_BLADE == io->type && QUEST_TARGET_WEAPON == type)
            {
                InvBlade *ib = (InvBlade *) io->extra;
                if (ib->type == subType)
                {
                    inv->objects.Remove(io);
                    delete io;
                    return TRUE;
                }
            }
            else if (INVOBJ_TOTEM == io->type && QUEST_TARGET_TOTEM == type)
            {
                InvTotem *ib = (InvTotem *) io->extra;
                if (ib->type == subType)
                {
                    inv->objects.Remove(io);
                    delete io;
                    return TRUE;
                }
            }
            else if (INVOBJ_STAFF == io->type && QUEST_TARGET_STAFF == type)
            {
                InvStaff *ib = (InvStaff *) io->extra;
                if (ib->type == subType)
                {
                    inv->objects.Remove(io);
                    delete io;
                    return TRUE;
                }
            }
            else if (INVOBJ_SIMPLE == io->type && (
                      QUEST_TARGET_LOCATION == type || QUEST_TARGET_NPC == type))
            {
                io->amount -= 1;
                if (io->amount < 1)
                {
                    inv->objects.Remove(io);
                    delete io;
                }
                return TRUE;
            }
        }
        else // items not specifically marked as quest items
        {
            if (INVOBJ_EGG == io->type && QUEST_TARGET_EGG == type)
            {
                InvEgg *ib = (InvEgg *) io->extra;
                if (ib->type == subType && 0 == ib->quality)
                {
                    io->amount -= 1;
                    if (io->amount < 1)
                    {
                        inv->objects.Remove(io);
                        delete io;
                    }
                    return 1;
                }
            }
            else if (INVOBJ_INGREDIENT == io->type && QUEST_TARGET_DUST == type)
            {
                InvIngredient *ib = (InvIngredient *) io->extra;
                if (ib->type == subType)
                {
                    io->amount -= 1;
                    if (io->amount < 1)
                    {
                        inv->objects.Remove(io);
                        delete io;
                    }
                    return 1;
                }
            }
        }

        io = (InventoryObject *) inv->objects.Next();
    }


    return FALSE;
}

//******************************************************************
void BBOSAvatar::QuestGiveGold(SharedSpace *ss, BBOSAvatar *other, long amount)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_GIVEGOLD == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_PLAYER == qt->type)
                        {
                            if (other != this && other->cellX == cellX && 
                                 other->cellY == cellY && amount >= qt->range)
                            {
                                int found = FALSE;

                                if (QUEST_PLAYER_TYPE_FIGHTER == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].physical >
                                      other->charInfoArray[other->curCharacterIndex].magical &&
                                      other->charInfoArray[other->curCharacterIndex].physical >
                                      other->charInfoArray[other->curCharacterIndex].creative)
                                    found = TRUE;
                                if (QUEST_PLAYER_TYPE_MAGE == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].magical >
                                      other->charInfoArray[other->curCharacterIndex].physical &&
                                      other->charInfoArray[other->curCharacterIndex].magical >
                                      other->charInfoArray[other->curCharacterIndex].creative)
                                    found = TRUE;
                                if (QUEST_PLAYER_TYPE_CRAFTER == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].creative >
                                      other->charInfoArray[other->curCharacterIndex].magical &&
                                      other->charInfoArray[other->curCharacterIndex].creative >
                                      other->charInfoArray[other->curCharacterIndex].physical)
                                    found = TRUE;
                                if (QUEST_PLAYER_TYPE_BALANCED == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].creative ==
                                      other->charInfoArray[other->curCharacterIndex].magical &&
                                      other->charInfoArray[other->curCharacterIndex].creative ==
                                      other->charInfoArray[other->curCharacterIndex].physical)
                                    found = TRUE;
                                if (QUEST_PLAYER_TYPE_YOUNG == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].lifeTime < 12)
                                    found = TRUE;
                                if (QUEST_PLAYER_TYPE_POOR == qt->playerType &&
                                      other->charInfoArray[other->curCharacterIndex].
                                                 inventory->money < 100)
                                    found = TRUE;

                                if (found)
                                {
                                    q->completeVal = 10000; // finished!

                                    sprintf(tempText,"***** Quest %d has been completed!", i + 1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    if (q->questSource < MAGIC_MAX)
                                    {
                                        sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    }
                                    else
                                    {
                                        questMan->ProcessWitchQuest(this, ss, q);
                                    }

                                    return;
                                }
                            }
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestPickupItem(SharedSpace *ss, InventoryObject *io)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_RETRIEVE == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_LOCATION == qt->type && INVOBJ_SIMPLE == io->type &&
                             INVSTATUS_QUEST_ITEM == io->status)
                        {
                            q->completeVal = 10000; // finished!
                
                            sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            if (q->questSource < MAGIC_MAX)
                            {
                                sprintf(tempText,"***** Return to the Great Tree of the %s to give the item to it.  ", magicNameList[q->questSource]);
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else
                            {
                                questMan->ProcessWitchQuest(this, ss, q);
                            }
            
                            return;
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
void BBOSAvatar::QuestPrayer(SharedSpace *ss)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_GROUP == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_LOCATION == qt->type &&
                             qt->x == cellX && qt->y == cellY)
                        {
                            q->completeVal = 10000; // finished!
                
                            sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            if (q->questSource < MAGIC_MAX)
                            {
                                sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else
                            {
                                questMan->ProcessWitchQuest(this, ss, q);
                            }
            
                            return;
                        }
                    }
                    
                }
            }
        }
    }
}

//******************************************************************
int BBOSAvatar::QuestReward(SharedSpace *ss)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_REWARD == qp->type)
                {
                    InventoryObject *iObject;
                    InvIngredient   *exIn;
                    InvIngot        *exIngot;
//					InvFuse         *exFuse;
                    InvStaff        *exStaff;
                    InvTotem        *extra;
//					InvExplosive    *exPlosive;
                    InvEgg          *im;
//					InvPotion       *ip;
                    InvFavor        *exF;

                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_REWARD_TYPE_GOLD == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            sprintf(tempText,"***** You recieve %d gold!", qt->x);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            charInfoArray[curCharacterIndex].inventory->money += qt->x;

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_INGOT == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            iObject = new InventoryObject(INVOBJ_INGOT,0, qt->WhoAmI());
                            exIngot = (InvIngot *)iObject->extra;
                            exIngot->damageVal = exIngot->challenge = qt->monsterType;
                            exIngot->r = qt->x;
                            exIngot->g = qt->y;
                            exIngot->b = qt->range;

                            iObject->mass = 1.0f;
                            iObject->value = qt->monsterSubType;
                            iObject->amount = 1;

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            sprintf(tempText,"***** You recieve %s!", qt->WhoAmI());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_FAVOR == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            sprintf(tempText,"%s Favor", magicNameList[qt->x]);
                            iObject = new InventoryObject(INVOBJ_FAVOR,0, tempText);
                            exF = (InvFavor *)iObject->extra;
                            exF->spirit = qt->x;

                            iObject->mass = 1.0f;
                            iObject->value = 10;
                            iObject->amount = 1;

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            sprintf(tempText,"***** You recieve a %s Favor!", magicNameList[qt->x]);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_EGG == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            iObject = new InventoryObject(INVOBJ_EGG,0, 
                                                           dragonInfo[qt->y][qt->x].eggName);
                            im = (InvEgg *)iObject->extra;
                            im->type = qt->x;
                            im->quality = qt->y;

                            iObject->mass = 1.0f;
                            iObject->value = 1000;
                            iObject->amount = 1;

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            sprintf(tempText,"***** You recieve a %s!", dragonInfo[qt->y][qt->x].eggName);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_DUST == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            iObject = new InventoryObject(INVOBJ_INGREDIENT,0, 
                                                           dustNames[qt->x]);
                            exIn = (InvIngredient *)iObject->extra;
                            exIn->type     = qt->x;
                            exIn->quality  = 1;

                            iObject->mass = 1.0f;
                            iObject->value = 1000;
                            iObject->amount = 1;

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            sprintf(tempText,"***** You recieve a %s!", dustNames[qt->x]);
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_STAFF == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            iObject = new InventoryObject(INVOBJ_STAFF,0,"Unnamed Staff");
                            exStaff = (InvStaff *)iObject->extra;
                            exStaff->type     = 0;
                            exStaff->quality  = qt->x;

                            iObject->mass = 0.0f;
                            iObject->value = 500 * (3 + 1) * (3 + 1);
                            iObject->amount = 1;
                            UpdateStaff(iObject, 0);

                            sprintf(tempText,"***** You recieve a %s!", iObject->WhoAmI());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                        else if (QUEST_REWARD_TYPE_TOTEM == qt->type)
                        {
                            q->completeVal = 10000; // finished!
                
                            iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
                            extra = (InvTotem *)iObject->extra;
                            extra->type     = 0;
                            extra->quality  = qt->x;

                            iObject->mass = 0.0f;

                            iObject->value = extra->quality * extra->quality * 14 + 1;
                            if (extra->quality > 12)
                                iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
                            iObject->amount = 1;
                            
                            UpdateTotem(iObject);

                            sprintf(tempText,"***** You recieve a %s!", iObject->WhoAmI());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            charInfoArray[curCharacterIndex].inventory->AddItemSorted(iObject);

                            questMan->ProcessWitchQuest(this, ss, q);
            
                            return TRUE;
                        }
                    }
                    
                }
            }
        }
    }

    return FALSE;
}

//******************************************************************
// call this BEFORE actually changing the space
void BBOSAvatar::QuestSpaceChange(SharedSpace *spaceFrom, SharedSpace *spaceTo)
{
    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *q = &charInfoArray[curCharacterIndex].quests[i];
        if (-1    != q->completeVal &&
             10000 != q->completeVal) // if active quest
        {
            std::vector<TagID> tempReceiptList;
            tempReceiptList.clear();
            tempReceiptList.push_back(socketIndex);

            MessInfoText infoText;
            char tempText[1024];

            QuestPart *qp = q->GetVerb();
            if (qp)
            {
                if (QUEST_VERB_ESCAPE == qp->type)
                {
                    QuestPart *qt = q->GetTarget();
                    if (qt)
                    {
                        if (QUEST_TARGET_SPACE == qt->type)
                        {
                            if (spaceFrom && spaceTo &&
                                 qt->mapType    == spaceFrom->WhatAmI() && 
                                 qt->mapSubType == spaceTo->WhatAmI())
                            {
                                q->completeVal = 10000; // finished!
                    
                                sprintf(tempText,"***** Quest %d has been accomplished!", i + 1);
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                spaceFrom->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                if (q->questSource < MAGIC_MAX)
                                {
                                    sprintf(tempText,"***** Return to the Great Tree of the %s for a reward.  ", magicNameList[q->questSource]);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    spaceFrom->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                                else
                                {
                                    questMan->ProcessWitchQuest(this, spaceFrom, q);
                                }
                
                                return;
                            }
                            else
                            {
                                if (spaceFrom)
                                {
                                    sprintf(tempText,"***** Quest %d is a failure.", i+1);
                                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                    spaceFrom->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                                q->EmptyOut();
                            }
                        }
                    }
                }
            }
        }
    }
}

//******************************************************************
int BBOSAvatar::PhysicalStat(void)
{
    if (charInfoArray[curCharacterIndex].physical < totemEffects.effect[TOTEM_PHYSICAL])
        return totemEffects.effect[TOTEM_PHYSICAL];
    return charInfoArray[curCharacterIndex].physical;
}

//******************************************************************
int BBOSAvatar::MagicalStat(void)
{
    if (charInfoArray[curCharacterIndex].magical < totemEffects.effect[TOTEM_MAGICAL])
        return totemEffects.effect[TOTEM_MAGICAL];
    return charInfoArray[curCharacterIndex].magical;
}

//******************************************************************
int BBOSAvatar::CreativeStat(void)
{
    if (charInfoArray[curCharacterIndex].creative < totemEffects.effect[TOTEM_CREATIVE])
        return totemEffects.effect[TOTEM_CREATIVE];
    return charInfoArray[curCharacterIndex].creative;
}

//*************************************************************************
int BBOSAvatar::DetectSize(char *string, int type)
{
    char tempText[1024];

    int size = 5;

    sprintf(tempText, string);

    // find a match
    int stringSize = 0;
    while (tempText[stringSize] != 0)
    {
        for (int i = 0; i < 6 && BLADE_TYPE_NORMAL == type; ++i)
        {
            if (IsSame(&tempText[stringSize],bladeList[i].name))
            {
                size = i;
                return size;
            }
        }

        for (int i = 0; i < 3; ++i)
        {
            if (IsSame(&tempText[stringSize],katanaList[i].name) && BLADE_TYPE_KATANA == type)
                return i+3;
            if (IsSame(&tempText[stringSize],clawList[i].name) && BLADE_TYPE_CLAWS == type)
                return i+3;
            if (IsSame(&tempText[stringSize],maceList[i].name) && BLADE_TYPE_MACE == type)
                return i+3;
            if (IsSame(&tempText[stringSize],bladestaffList[i].name) && BLADE_TYPE_DOUBLE == type)
                return i+3;
            if (IsSame(&tempText[stringSize],chaosList[i].name) && BLADE_TYPE_CHAOS == type)
                return i+3;
        }

        ++stringSize;
    }

    return size;
}


//*************************************************************************
void BBOSAvatar::DetectIngotTypes(InvBlade *ib, int &type1, int &type2)
{
    type1 = -1;
    type2 = -1;

    if( ib->maligIngots )
    {
        type1 = 10;
    }
    
    if( ib->chitinIngots )
    {
        if( type1 == -1 )
            type1 = 9;
        else if( type2 == -1 )
            type2 = 9;
    }
    
    if( ib->elatIngots )
    {
        if( type1 == -1 )
            type1 = 8;
        else if( type2 == -1 )
            type2 = 8;
    }
    
    if( ib->vizIngots )
    {
        if( type1 == -1 )
            type1 = 7;
        else if( type2 == -1 )
            type2 = 7;
    }
    
    if( ib->mithIngots )
    {
        if( type1 == -1 )
            type1 = 6;
        else if( type2 == -1 )
            type2 = 6;
    }
    
    if( ib->adamIngots )
    {
        if( type1 == -1 )
            type1 = 5;
        else if( type2 == -1 )
            type2 = 5;
    }
    
    if( ib->zincIngots )
    {
        if( type1 == -1 )
            type1 = 4;
        else if( type2 == -1 )
            type2 = 4;
    }
    
    if( ib->carbonIngots )
    {
        if( type1 == -1 )
            type1 = 3;
        else if( type2 == -1 )
            type2 = 3;
    }
    
    if( ib->steelIngots )
    {
        if( type1 == -1 )
            type1 = 2;
        else if( type2 == -1 )
            type2 = 2;
    }
    
    if( ib->aluminumIngots )
    {
        if( type1 == -1 )
            type1 = 1;
        else if( type2 == -1 )
            type2 = 1;
    }
    
    if( ib->tinIngots )
    {
        if( type1 == -1 )
            type1 = 0;
        else if( type2 == -1 )
            type2 = 0;
    }
}


void BBOSAvatar::AddMastery( SharedSpace *ss ) {
    InventoryObject *io = (InventoryObject *) 
        charInfoArray[curCharacterIndex].wield->objects.First();
    int found = 0;
    char tmp[80];
    char tempText[80];
    MessInfoText infoText;
    
    sprintf( tmp, "" );

    while( io && !found ) {
        if( INVOBJ_BLADE == io->type ) {
            found = 1;

            switch( ((InvBlade *) io->extra )->type ) {
                case BLADE_TYPE_CHAOS:
                    sprintf( tmp, "Chaos Mastery" );
                    break;
                case BLADE_TYPE_NORMAL:
                    sprintf( tmp, "Sword Mastery" );
                    break;
                case BLADE_TYPE_KATANA:
                    sprintf( tmp, "Katana Mastery" );
                    break;
                case BLADE_TYPE_MACE:
                    sprintf( tmp, "Mace Mastery" );
                    break;
                case BLADE_TYPE_CLAWS:
                    sprintf( tmp, "Claw Mastery" );
                    break;
                case BLADE_TYPE_DOUBLE:
                    sprintf( tmp, "Bladestaff Mastery" );
                    break;
            }
                
        }

        io = (InventoryObject *) 
            charInfoArray[curCharacterIndex].wield->objects.Next();
    }
    
    found = 0;
    io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
    while( io && !found ) {
        if( !strcmp( tmp, io->WhoAmI() ) ) {
            InvSkill *iSkill = (InvSkill *)io->extra;
            found = 1;
            
            if( iSkill->skillLevel < 100 && iSkill->skillLevel * 100000 <= iSkill->skillPoints ) {
                std::vector<TagID> tempReceiptList;
    
                tempReceiptList.clear();
                tempReceiptList.push_back(socketIndex);
                
                // made a skill level!!!
                iSkill->skillLevel += 1;
                
                sprintf( tempText,"You gained %s skill!! You feel more in tune with your weapon.", tmp );
                CopyStringSafely(tempText,1024,infoText.text,MESSINFOTEXTLEN);;
                
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }

            iSkill->skillPoints += 1;
        }

        io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
    }
}


int BBOSAvatar::GetMasteryForType( int type ) {
    char tmp[80];
    
    sprintf( tmp, "" );

    switch( type ) {
        case BLADE_TYPE_CHAOS:
            sprintf( tmp, "Chaos Mastery" );
            break;
        case BLADE_TYPE_NORMAL:
            sprintf( tmp, "Sword Mastery" );
            break;
        case BLADE_TYPE_KATANA:
            sprintf( tmp, "Katana Mastery" );
            break;
        case BLADE_TYPE_MACE:
            sprintf( tmp, "Mace Mastery" );
            break;
        case BLADE_TYPE_CLAWS:
            sprintf( tmp, "Claw Mastery" );
            break;
        case BLADE_TYPE_DOUBLE:
            sprintf( tmp, "Bladestaff Mastery" );
            break;
    }
        
    InventoryObject *io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.First();
    while( io ) {
        if( !strcmp( tmp, io->WhoAmI() ) ) {
            return ((InvSkill *)io->extra)->skillLevel;
        }

        io = (InventoryObject *) charInfoArray[curCharacterIndex].skills->objects.Next();
    }

    return 0;
}


/* end of file */



