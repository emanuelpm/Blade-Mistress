
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "BBOServer.h"
#include "BBO-Savatar.h"
#include "BBO-Snpc.h"
#include "BBO-Stree.h"
#include "BBO-Smonster.h"
#include "BBO-Sgenerator.h"
#include "BBO-Sgen-spirits.h"
#include "BBO-Sgen-dragons.h"
#include "BBO-Sgen-orcClan.h"
#include "BBO-Sgen-vamps.h"
#include "BBO-Sgen-nearvamps.h"
#include "BBO-SNewbGenerator.h"
#include "BBO-Stower.h"
#include "BBO-Schest.h"
#include "BBO-Sbomb.h"
#include "BBO-SwarpPoint.h"
#include "BBO-SgroundEffect.h"
#include "monsterData.h"
#include "Ground-Map.h"
#include "Dungeon-Map.h"
#include "Realm-Map.h"
#include "Tower-Map.h"
#include "Labyrinth-Map.h"
#include "tokenManager.h"
#include ".\helper\GeneralUtils.h"
#include ".\helper\UniqueNames.h"
#include "longtime.h"
#include ".\helper\sendMail.h"
#include "ArmySpiders.h"
#include "ArmyHalloween.h"
#include "ArmyDead.h"
#include "ArmyDragons.h"
#include "ArmyDuskWurm.h"
#include "ArmyDragonChaplain.h"
#include "ArmyDragonArchMage.h"
#include "ArmyDragonOverlord.h"
#include "staffData.h"
#include "ipBanList.h"
#include "uidbanlist.h"
#include ".\helper\crypto.h"
#include ".\helper\crc.h"
#include "QuestSystem.h"
#include "BBO-SchainQuest.h"
#include "BBO-SinvadeQuest.h"

#include "version.h"

BBOServer *bboServer = NULL; // better be only one instance of this server at a time!

IPBanList *banList;
UIDBanList *uidBanList;

extern char skillNameArray[19][32];
extern int skillHotKeyArray[19];

char *cryptoText1 = "Connecting...";
char *cryptoText2 = "Sending hello...";
char *cryptoText3 = "Simple avatars OFF";


//******************************************************************
UserMessage::UserMessage(unsigned long connectionID, char *name) : DataObject(connectionID,name)
{
    password[0] = 0;
    avatar  [0] = 0;
    message [0] = 0;

    color = 0xffffffff;
    age = timeGetTime() + 1000 * 60 * 12;
}

//******************************************************************
UserMessage::~UserMessage()
{
}

//******************************************************************
//******************************************************************
BBOServer::BBOServer(int useIOCP)
{

    bboServer = this;

    pleaseKillMe = 10000; // kill when this gets to 0

    IOCPFlag = useIOCP;
    if (useIOCP)
    {
        IOCPSocket::StartIOCP();
        lserver = new IOCPServer();
#ifdef _TEST_SERVER
        ((IOCPServer *)lserver)->startServer(9178, 100, NULL);
#else
        ((IOCPServer *)lserver)->startServer(3678, 100, NULL);
#endif
    }
    else
    {
        lserver = new Server();
#ifdef _TEST_SERVER
        ((Server *)lserver)->startServer(9178, 100, NULL, NULL);
#else
        ((Server *)lserver)->startServer(3678, 100, NULL, NULL);
#endif
    }

	// Create Logs directory
	CreateDirectory("Logs", NULL);

    char tempText[1024];
    LongTime lt;
    lt.value.wHour += 19;
    if (lt.value.wHour < 24)
        lt.value.wDay -= 1;
    else
        lt.value.wHour -= 24;

    sprintf(tempText,"--------Server Starting at %d/%02d, %d:%02d\n", lt.value.wMonth, lt.value.wDay, 
             lt.value.wHour, lt.value.wMinute);
    LogOutput("gamelog.txt", tempText);

//	lserver = new Server();
//	lserver->startServer(3678, 32, NULL, NULL);
     
//	mobs       = new DoublyLinkedList();
//	avatars    = new DoublyLinkedList();
    incoming   = new DoublyLinkedList();
    spaceList  = new DoublyLinkedList();

    userMessageList = new DoublyLinkedList();

    LoadPetData();

    GroundMap *gm = new GroundMap(SPACE_GROUND,"Ground",lserver);
    gm->InitNew(0,0,0,0);
    spaceList->Append(gm);

//	dungeonList= new DoublyLinkedList();

    // add in starting mobs
    /*
    BBOSMonster *monster = new BBOSMonster(0,0);
    mobList->Add(monster);
    monster = new BBOSMonster(0,1);
    monster->cellX--;
    mobList->Add(monster);
    monster = new BBOSMonster(0,2);
    monster->cellX++;
    mobList->Add(monster);
    */

    // add town merchants
    for (int t = 0; t < NUM_OF_TOWNS; ++t)
    {
        for (int t2 = 0; t2 < townList[t].size + 1; ++t2)
        {
            BBOSNpc *npc = new BBOSNpc(SMOB_TRADER);
            npc->level = townList[t].level;
            npc->townIndex = t;
            do
            {
                npc->cellX = townList[t].x + (rand() % (1 + townList[t].size)) - townList[t].size/2;
                npc->cellY = townList[t].y + (rand() % (1 + townList[t].size)) - townList[t].size/2;
            } while (npc->cellX == townList[t].x && npc->cellY == townList[t].y);
            gm->mobList->Add(npc);
        }
    }

    // add town trainers
    for (int t = 0; t < NUM_OF_TOWNS; ++t)
    {
        for (int t2 = 0; t2 < townList[t].size / 2 + 1; ++t2)
        {
            BBOSNpc *npc = new BBOSNpc(SMOB_TRAINER);
            do
            {
                npc->cellX = townList[t].x + (rand() % (1 + townList[t].size)) - townList[t].size/2;
                npc->cellY = townList[t].y + (rand() % (1 + townList[t].size)) - townList[t].size/2;
            } while (npc->cellX == townList[t].x && npc->cellY == townList[t].y);
            gm->mobList->Add(npc);
        }
    }

    // add town mages
    for (int t = 0; t < NUM_OF_TOWNS; ++t)
    {
        BBOSNpc *npc = new BBOSNpc(SMOB_TOWNMAGE);
        npc->cellX = townList[t].x;
        npc->cellY = townList[t].y + 1;
        gm->mobList->Add(npc);
    }

    
    // add the new enterance to the Realm of Pain
    BBOSWarpPoint *wpRop = new BBOSWarpPoint( 161, 121, SMOB_ROPENTERANCE );
    wpRop->allCanUse = false;
    gm->mobList->Add( wpRop );

    /*
    // add the new TEMP CASTLE MESH
    BBOSWarpPoint *wpCastle = new BBOSWarpPoint( 24, 24, SMOB_CASTLE );
    wpCastle->allCanUse = false;
    gm->mobList->Add( wpCastle );
    */

    // add dungeons
    
    FILE *fp = fopen("serverdata\\dungeons.dat","r");

    if (!fp)
    {
        for (int t = 0; t < MAP_SIZE_WIDTH; ++t)
        {
            for (int t2 = 0; t2 < MAP_SIZE_HEIGHT; ++t2)
            {
                int col = gm->Color(t,t2) - 250;
                if (col >= 0 && col < 5)
                {
                    BBOSTower *tower = new BBOSTower(t,t2);
                    gm->mobList->Add(tower);

                    DungeonMap *dm = new DungeonMap(SPACE_DUNGEON,"Test Dungeon",lserver);
                    dm->InitNew(20 + col * 5,20,t,t2, col);
                    spaceList->Append(dm);

                    tower->ss = dm;

                    // add static monsters
                    for (int m = 0; m < (dm->height * dm->width) / 12;)
                    {
                        int t  = rand() % NUM_OF_MONSTERS;
                        int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

                        if (monsterData[t][t2].name[0] && monsterData[t][t2].dungeonType >= col - 3 &&
                             monsterData[t][t2].dungeonType <= col)
                        {
                            int mx, my;
                            do
                            {
                                mx = rand() % (dm->width);
                                my = rand() % (dm->height);
                            } while (mx < 4 && my < 4);

                            BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
                            monster->cellX = mx;
                            monster->cellY = my;
                            monster->targetCellX = mx;
                            monster->targetCellY = my;
                            monster->spawnX = mx;
                            monster->spawnY = my;
                            dm->mobList->Add(monster);
                            ++m;
                        }
                    }
                    // add wandering monsters
                    if (0 == col)
                        col = 1;

                    for (int m = 0; m < (dm->height * dm->width) / 12;)
                    {
                        int t, t2;
                        int done = FALSE;
                        while (!done)
                        {
                            t  = rand() % NUM_OF_MONSTERS;
                            t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

                            if (MONSTER_PLACE_DUNGEON & monsterData[t][t2].placementFlags)
                                done = TRUE;
                        }

                        if (monsterData[t][t2].name[0] && monsterData[t][t2].dungeonType >= col - 3 &&
                             monsterData[t][t2].dungeonType <= col - 1)
                        {
                            int mx, my;
                            do
                            {
                                mx = rand() % (dm->width);
                                my = rand() % (dm->height);
                            } while (mx < 4 && my < 4);

                            BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
                            monster->isWandering = TRUE;
                            monster->cellX = mx;
                            monster->cellY = my;
                            monster->targetCellX = mx;
                            monster->targetCellY = my;
                            monster->spawnX = mx;
                            monster->spawnY = my;
                            dm->mobList->Add(monster);
                            ++m;
                        }
                    }
                }
            }
        }
    }
    else
    {
        srand(25);
//		char tempText[128];
        float dunVers;

        fscanf(fp,"%f\n",&dunVers);

        LoadLineToString(fp,tempText);
        while (!strcmp(tempText,"DUNGEON"))
        {
            DungeonMap *dm = new DungeonMap(SPACE_DUNGEON,"Test Dungeon",lserver);
            if (dunVers < 2.04f)
            {
                dm->Load(fp,dunVers);
                sprintf(tempText, dm->name);
                int oldX = dm->enterX;
                int oldY = dm->enterY;
                delete dm;

                dm = new DungeonMap(SPACE_DUNGEON,"Test Dungeon",lserver);
                dm->InitNew(5,10,-1,-1, rand() % 5);
                sprintf(dm->name, tempText);
                dm->enterX = oldX;
                dm->enterY = oldY;
            }
            else
                dm->Load(fp,dunVers);
            AddDungeonSorted(dm);

            if (dunVers < 2.02f)
            {
                dm->enterX *= 2;
                dm->enterY *= 2;
            }

            if (dunVers < 2.04f)
            {
                // add static monsters
                for (int m = 0; m < (dm->height * dm->width) / 12;)
                {
                    int t  = rand() % NUM_OF_MONSTERS;
                    int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

                    if (monsterData[t][t2].name[0])
                    {
                        int mx, my;
                        do
                        {
                            mx = rand() % (dm->width);
                            my = rand() % (dm->height);
                            if (!(MONSTER_PLACE_DUNGEON & monsterData[t][t2].placementFlags))
                            {
                                mx = my = 1000; // try again
                                t  = rand() % NUM_OF_MONSTERS;
                                t2 = rand() % NUM_OF_MONSTER_SUBTYPES;
                            }
                        } while (mx > dm->width-2 && my > dm->height-2);

                        BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
                        monster->cellX = mx;
                        monster->cellY = my;
                        monster->targetCellX = mx;
                        monster->targetCellY = my;
                        monster->spawnX = mx;
                        monster->spawnY = my;
                        dm->mobList->Add(monster);
                        ++m;
                    }
                }
            }

            int badPlace = TRUE;
            while (badPlace)
            {
                badPlace = FALSE;

                if (!gm->CanMove(dm->enterX, dm->enterY, dm->enterX, dm->enterY) || 
                    !gm->CanMove(dm->enterX, dm->enterY-1, dm->enterX, dm->enterY-1))
                    badPlace = TRUE;

                if (dm->enterX < 2 || dm->enterX >= MAP_SIZE_WIDTH-2)
                    badPlace = TRUE;

                if (dm->enterY < 2 || dm->enterY >= MAP_SIZE_HEIGHT-2)
                    badPlace = TRUE;

                for (int i = 0; i < NUM_OF_TOWNS; ++i)
                {
                    if (abs(dm->enterX - townList[i].x) < townList[i].size &&
                         abs(dm->enterY - townList[i].y) < townList[i].size)
                        badPlace = TRUE;
                }

                if (badPlace)
                {
                    dm->enterX = rand() % 256;
                    dm->enterY = rand() % 256;
                }

            }
            BBOSTower *tower = new BBOSTower(dm->enterX, dm->enterY);
            gm->mobList->Add(tower);
            tower->ss = dm;

            // add the treasure chests
            BBOSChest *chest = new BBOSChest(rand() % dm->width, rand() % dm->height);
            dm->mobList->Add(chest);

            chest = new BBOSChest(rand() % dm->width, rand() % dm->height);
            dm->mobList->Add(chest);

            chest = new BBOSChest(rand() % dm->width, rand() % dm->height);
            dm->mobList->Add(chest);

            BBOSWarpPoint *wp = new BBOSWarpPoint(dm->width-1,dm->height-1);
            wp->targetX      = dm->enterX;
            wp->targetY      = dm->enterY-1;
            wp->spaceType    = SPACE_GROUND;
            wp->spaceSubType = 0;
            dm->mobList->Add(wp);

            LoadLineToString(fp,tempText);
        }

        fclose(fp);
    }

    // add mod-controlled dungeons
    BBOSTower *tower = new BBOSTower(62,62);
    gm->mobList->Add(tower);

    DungeonMap *dm = new DungeonMap(SPACE_DUNGEON,"Mysterious Cave",lserver);
    dm->InitNew(20,20, 62,62, 0);
    spaceList->Append(dm);
    tower->ss = dm;
    dm->specialFlags = SPECIAL_DUNGEON_MODERATED;
    sprintf(dm->name, "Mysterious Cave");

    BBOSWarpPoint *wp2 = new BBOSWarpPoint(dm->width-1,dm->height-1);
    wp2->targetX      = dm->enterX;
    wp2->targetY      = dm->enterY-1;
    wp2->spaceType    = SPACE_GROUND;
    wp2->spaceSubType = 0;
    dm->mobList->Add(wp2);


    // add monster generators
    BBOSGenerator *curGen;
    curGen = new BBOSGenerator(0,0);
    gm->generators->Append(curGen);
    for (int i = 0; i < NUM_OF_MONSTERS; ++i)
    {
        for (int j = 0; j < NUM_OF_MONSTER_SUBTYPES; ++j)
        {
           curGen->max[i][j]   = 200;
        }
    }

    BBOSGenVamps *genVamps = new BBOSGenVamps(0,0);
    gm->generators->Append(genVamps);

    BBOSGenNearVamps *genNearVamps = new BBOSGenNearVamps(0,0);
    gm->generators->Append(genNearVamps);
/*
    BBOSMonster *monster = new BBOSMonster(15,0, NULL);
    monster->cellX = 4;
    monster->cellY = 4;
    monster->targetCellX = 4;
    monster->targetCellY = 4;
    monster->spawnX = 4;
    monster->spawnY = 4;
    gm->mobList->Add(monster);



*/
    
    BBOSChainQuest *curCQ;
    curCQ = new BBOSChainQuest(gm);
    gm->generators->Append(curCQ);
      
    BBOSInvadeQuest *curIQ;
    curIQ = new BBOSInvadeQuest(gm);
    gm->generators->Append(curIQ);
      
    BBOSNewbGenerator *curNewbGen;
    curNewbGen = new BBOSNewbGenerator(0,0);
    gm->generators->Append(curNewbGen);
  
    // halloween is OVAH!
//	ArmyHalloween *ah = new ArmyHalloween(gm, 104,53, 104, 43, 25);
//	gm->generators->Append(ah);

    ArmySpiders *as;

    as = new ArmySpiders(gm, 120,37, 120, 27, 35);
    gm->generators->Append(as);

    as = new ArmySpiders(gm, 96,152, 90, 140, 45);
    gm->generators->Append(as);

    /*
    for (t = 0; t < 50;)
    {
        // pick a point
        int tempX = rand() % 128;
        int tempY = rand() % 128;

        // is it valid?
        if (map.CanMoveTo(tempX, tempY))
        {
            // is it too close to town?
            int toClose = FALSE;
            for (int t2 = 0; t2 < NUM_OF_TOWNS; ++t2)
            {
                if (abs(townList[t2].x - tempX) < MONSTER_SPAWN_RANGE/3 ||
                    abs(townList[t2].y - tempY) < MONSTER_SPAWN_RANGE/3)
                         toClose = TRUE;
            }

            if (!toClose)
            {
                // make a spawn point there
                curGen = new BBOSGenerator(tempX, tempY);
                generators->Append(curGen);
                ++t;
            }
        }
    }
    */

    // add the Great Trees
    for (int t = 0; t < 9; ++t)
    {
        BBOSTree *gt = new BBOSTree(0,0);
        gt->cellX = greatTreePos[t][0];
        gt->cellY = greatTreePos[t][1];
        gt->index = t;
        gm->mobList->Add(gt);
    }

    // pet graveyard 58 88 and 70 30

    RealmMap *rm = new RealmMap(SPACE_REALM,"Realm of Spirits",lserver);
    rm->InitNew("dat\\realm-spirits-server.raw",64,64,0,0);
    rm->type = REALM_ID_SPIRITS;
    spaceList->Append(rm);

//	rm->CreateSpiritStaticPositions();

    curGen = new BBOSGenerator(0,0);
    rm->generators->Append(curGen);


    // add Dokk
    BBOSMonster *monster = new BBOSMonster(16,1, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 62;
    monster->cellY = monster->targetCellY = monster->spawnY = 3;
    rm->mobList->Add(monster);

    // add Dokk's centurions
    monster = new BBOSMonster(11,1, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 62;
    monster->cellY = monster->targetCellY = monster->spawnY = 4;
    rm->mobList->Add(monster);

    monster = new BBOSMonster(11,1, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 61;
    monster->cellY = monster->targetCellY = monster->spawnY = 5;
    rm->mobList->Add(monster);

    monster = new BBOSMonster(11,1, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 62;
    monster->cellY = monster->targetCellY = monster->spawnY = 6;
    rm->mobList->Add(monster);

    
    rm = new RealmMap(SPACE_REALM,"Realm of the Dead",lserver);
    rm->InitNew("dat\\realm-dead-server.raw",64,64,0,0);
    rm->type = REALM_ID_DEAD;
    spaceList->Append(rm);

    curGen = new BBOSGenerator(0,0);
    rm->generators->Append(curGen);

    // anubis
    ArmyDead *ad = new ArmyDead(rm, 41,5, 50, 2, 1);
    rm->generators->Append(ad);

    // other armies
    ad = new ArmyDead(rm, 32, 35, 21, 32, 2);
    rm->generators->Append(ad);
    ad = new ArmyDead(rm, 14,54, 24, 54, 3);
    rm->generators->Append(ad);
    ad = new ArmyDead(rm, 50,40, 43,25, 4);
    rm->generators->Append(ad);


    rm = new RealmMap(SPACE_REALM,"Realm of Dragons",lserver);
    rm->InitNew("dat\\realm-dragons-server.raw",64,64,0,0);
    rm->type = REALM_ID_DRAGONS;
    spaceList->Append(rm);

    curGen = new BBOSGenDragons(0,0);
    rm->generators->Append(curGen);

    curGen = new BBOSGenOrcClan(21,7);
    rm->generators->Append(curGen);

    curGen = new BBOSGenOrcClan(12,58);
    rm->generators->Append(curGen);

    curGen = new BBOSGenOrcClan(24,19);
    rm->generators->Append(curGen);

    ArmyOverlord *adr;

    adr = new ArmyOverlord(rm, 60, 9, 60, 2, 85);
    rm->generators->Append(adr);

    ArmyArchMage *aAM;

    aAM = new ArmyArchMage(rm, 61,53, 62, 62, 86);
    rm->generators->Append(aAM);

    ArmyChaplain *aChap;

    aChap = new ArmyChaplain(rm, 41,54, 41,54, 87);
    rm->generators->Append(aChap);

    ArmyDuskWurm *aDuskWurm;

    aDuskWurm = new ArmyDuskWurm(rm, 5, 42, 16, 42, 85);
    rm->generators->Append(aDuskWurm);

    aDuskWurm = new ArmyDuskWurm(rm, 36, 3, 36, 11, 86);
    rm->generators->Append(aDuskWurm);




    // add gateway skeletons
    
    monster = new BBOSMonster(19,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 67;
    monster->cellY = monster->targetCellY = monster->spawnY = 76;
    gm->mobList->Add(monster);

    monster = new BBOSMonster(19,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 239;
    monster->cellY = monster->targetCellY = monster->spawnY = 145;
    gm->mobList->Add(monster);

    monster = new BBOSMonster(19,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 223;
    monster->cellY = monster->targetCellY = monster->spawnY = 188;
    gm->mobList->Add(monster);

    // add gateway spirits
    
    monster = new BBOSMonster(9,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 223;
    monster->cellY = monster->targetCellY = monster->spawnY = 8;
    gm->mobList->Add(monster);

    monster = new BBOSMonster(9,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 45;
    monster->cellY = monster->targetCellY = monster->spawnY = 130;
    gm->mobList->Add(monster);

    monster = new BBOSMonster(9,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 87;
    monster->cellY = monster->targetCellY = monster->spawnY = 230;
    gm->mobList->Add(monster);

    // ********* add tricksters
    monster = new BBOSMonster(2,2, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 112;
    monster->cellY = monster->targetCellY = monster->spawnY = 9;
    gm->mobList->Add(monster);
    sprintf(monster->uniqueName,"Trickster Minotaur");

    int deathCount = 1;
    fp = fopen("serverdata\\tricksterMino.dat","r");
    if (fp)
    {
        fscanf(fp,"%d",&deathCount);
        fclose(fp);
    }

    if (deathCount < 1)
        deathCount = 1;
    if (deathCount > 3000)
        deathCount = 3000;

    monster->r               = 255;
    monster->g               = 0;
    monster->b               = 0;
    monster->a               = 255;
    monster->sizeCoeff       = 1.0f + 1.0f * deathCount/50;
    monster->health          = 100 * deathCount;
    monster->maxHealth       = 100 * deathCount;
    monster->damageDone      = deathCount/2;
    monster->toHit           = deathCount;
    monster->defense         = deathCount;
    monster->dropAmount      = deathCount;
    monster->magicResistance = 1.0f - 1.0f/deathCount;


    // ********* end of add tricksters

    // add towers/guilds
    fp = fopen("serverdata\\guilds.dat","r");

    if (fp)
    {
        srand(25);
        float dunVers;

        fscanf(fp,"%f\n",&dunVers);
                          
        LoadLineToString(fp,tempText);
        while (!strcmp(tempText,"TOWER"))
        {
            TowerMap *tm = new TowerMap(SPACE_GUILD,"Test Tower",lserver);
            tm->Load(fp,dunVers);
            if (tm->members->ItemsInList() < 2)
                delete tm;
            else
            {
                spaceList->Append(tm);
                // TEST
                if (-1 != tm->enterX && -1 != tm->enterY)
                {
                    int badPlace = TRUE;
                    while (badPlace)
                    {
                        badPlace = FALSE;

                        if (!gm->CanMove(tm->enterX, tm->enterY, tm->enterX, tm->enterY) || 
                             !gm->CanMove(tm->enterX, tm->enterY-1, tm->enterX, tm->enterY-1))
                            badPlace = TRUE;

                        if (tm->enterX < 2 || tm->enterX >= MAP_SIZE_WIDTH-2)
                            badPlace = TRUE;

                        if (tm->enterY < 2 || tm->enterY >= MAP_SIZE_HEIGHT-2)
                            badPlace = TRUE;

                        for (int i = 0; i < NUM_OF_TOWNS; ++i)
                        {
                            if (abs(tm->enterX - townList[i].x) < townList[i].size &&
                                 abs(tm->enterY - townList[i].y) < townList[i].size)
                                badPlace = TRUE;
                        }

                        if (badPlace)
                        {
//							int offsetX = (rand() % 3) - 1;
//							int offsetX = (rand() % 3) - 1;
                            int foundValidPoint = FALSE;
                            while (!foundValidPoint)
                            {
                                tm->enterX += (rand() % 3) - 1;
                                tm->enterY += (rand() % 3) - 1;

                                while (tm->enterX < 2)
                                    tm->enterX += 3;
                                while (tm->enterY < 2)
                                    tm->enterY += 3;
                                while (tm->enterX >= MAP_SIZE_WIDTH -2)
                                    tm->enterX -= 3;
                                while (tm->enterY >= MAP_SIZE_HEIGHT-2)
                                    tm->enterY -= 3;

                                if (gm->CanMove(tm->enterX, tm->enterY, tm->enterX, tm->enterY))
                                    foundValidPoint = TRUE;
                            }
                        }
                    }

                    BBOSTower *tower = new BBOSTower(tm->enterX, tm->enterY);
                    gm->mobList->Add(tower);
                    tower->ss = tm;
                    tower->isGuildTower = TRUE;

                    BBOSChest *chest = new BBOSChest(2,2);
                    tm->mobList->Add(chest);

                    BBOSWarpPoint *wp = new BBOSWarpPoint(4,4);
                    wp->targetX      = tm->enterX;
                    wp->targetY      = tm->enterY-1;
                    wp->spaceType    = SPACE_GROUND;
                    wp->spaceSubType = 0;
                    tm->mobList->Add(wp);

                }
            }
            LoadLineToString(fp,tempText);
        }

        fclose(fp);
    }

    LabyrinthMap *lm = new LabyrinthMap(SPACE_LABYRINTH,"Labyrinth",lserver);
    lm->InitNew("dat\\labyrinth-server.raw",64,64,1,1);
    lm->type = REALM_ID_LAB1;
    spaceList->Append(lm);

    LabyrinthMap *lm2 = new LabyrinthMap(SPACE_LABYRINTH,"Deep Labyrinth",lserver);
    lm2->InitNew("dat\\labyrinth2-server.raw",64,64,1,1);
    lm2->type = REALM_ID_LAB2;
    spaceList->Append(lm2);

    curGen = new BBOSGenerator(0,0);
    lm->generators->Append(curGen);

    // ******** create Larath (Grey Spidren)
    monster = new BBOSMonster(24,4, NULL);  // make boss spidren
    sprintf(monster->uniqueName,"Larath");
    monster->sizeCoeff = 1.7f;

    monster->health = monster->maxHealth = 80000;
    monster->damageDone         = 40;
    monster->toHit              = 90;
    monster->defense            = 135;
    monster->dropAmount         = 20;

    monster->magicResistance = 0.8f;

    monster->r = 255;
    monster->g = 128;
    monster->b = 00;

    monster->targetCellX = monster->spawnX = monster->cellX = 25;
    monster->targetCellY = monster->spawnY = monster->cellY = 4;

    lm->mobList->Add(monster);

    // ******** create Moultz (Grey Spidren)
    monster = new BBOSMonster(24,4, NULL);  // make boss spidren
    sprintf(monster->uniqueName,"Moultz");
    monster->sizeCoeff = 1.8f;

    monster->health = monster->maxHealth = 80000;
    monster->damageDone         = 40;
    monster->toHit              = 100;
    monster->defense            = 145;
    monster->dropAmount         = 20;

    monster->magicResistance = 0.9f;

    monster->r = 00;
    monster->g = 255;
    monster->b = 128;

    monster->targetCellX = monster->spawnX = monster->cellX = 48;
    monster->targetCellY = monster->spawnY = monster->cellY = 28;

    lm->mobList->Add(monster);

    // ******** create Reorth (Grey Spidren)
    monster = new BBOSMonster(24,4, NULL);  // make boss spidren
    sprintf(monster->uniqueName,"Reorth");
    monster->sizeCoeff = 1.9f;

    monster->health = monster->maxHealth = 80000;
    monster->damageDone         = 40;
    monster->toHit              = 110;
    monster->defense            = 155;
    monster->dropAmount         = 20;

    monster->magicResistance = 0.99f;

    monster->r = 128;
    monster->g = 00;
    monster->b = 255;

    monster->targetCellX = monster->spawnX = monster->cellX = 24;
    monster->targetCellY = monster->spawnY = monster->cellY = 56;

    lm->mobList->Add(monster);

/*
    BBOSWarpPoint *wp = new BBOSWarpPoint(28,28);
    wp->targetX      = 0;
    wp->targetY      = 0;
    wp->spaceType    = 10000;
    wp->spaceSubType = 0;
    wp->allCanUse    = FALSE;
    lm->mobList->Add(wp);
    */
    AddWarpPair(lm, 28,28,
                    lm2, 1,1, 
                    TRUE);

    AddWarpPair(gm, 19, 5,
                    lm, 1,1, 
                    TRUE);

    AddWarpPair(gm, 141, 86,
                    lm, 51,4, 
                    TRUE);

    AddWarpPair(gm, 183, 26,
                    lm, 51,59, 
                    TRUE);

    AddWarpPair(gm, 32, 237,
                    lm, 4, 55, 
                    TRUE);

    AddWarpPair(gm, 215, 222,
                    lm, 22, 31, 
                    TRUE);

    AddWarpPair(gm, 123, 150,
                    lm, 46, 32, 
                    TRUE);
  



    curGen = new BBOSGenerator(0,0);
    lm2->generators->Append(curGen);

    // add level-1 vlord
    monster = new BBOSMonster(27,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 5;
    monster->cellY = monster->targetCellY = monster->spawnY = 6;
    monster->magicResistance = 0.99f;
    lm2->mobList->Add(monster);

    monster = new BBOSMonster(27,0, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 52;
    monster->cellY = monster->targetCellY = monster->spawnY = 59;
    monster->magicResistance = 0.99f;
    lm2->mobList->Add(monster);

    monster = new BBOSMonster(27,1, NULL);
    monster->cellX = monster->targetCellX = monster->spawnX = 7;
    monster->cellY = monster->targetCellY = monster->spawnY = 58;
    monster->magicResistance = 0.99f;
    lm2->mobList->Add(monster);


    // **** done with labyrinth

    tokenMan.Init();
    tokenMan.Load();

    testTime          = timeGetTime();
//	dungeonUpdateTime = timeGetTime();
    lastConnectTime   = timeGetTime();
    lastGraveyardTime = 0;
    errorContactVal   = 0;
    dungeonUpdateTime = tenSecondTime = labyMapTime = 0;
    smasherCounter = 0;

    dayTimeCounter    = 0;
//	dayTimeCounter    = 60 * 6 * 3; // start in darkness

    banList = new IPBanList();
    uidBanList = new UIDBanList();

    weatherState = 0; // normal, no weather

    questMan = new QuestManager();
    questMan->Load();

}

//******************************************************************
BBOServer::~BBOServer()
{
    delete questMan;

    delete banList;
    delete uidBanList;

//	delete[] groundInventory;

//	delete mobs;
    delete userMessageList;
    delete incoming;
    delete spaceList;
//	delete dungeonList;

    delete lserver;
    if (IOCPFlag)
        IOCPSocket::StopIOCP();
}


//BBOSAvatar *ServerRandomAvatar;
//int currentAvatarCount = 0;
int lastAvatarCount = 0;

          
//******************************************************************
void BBOServer::Tick(void)
{
    std::vector<TagID> tempReceiptList;
    char tempText[1024];
    HandleMessages();

    DWORD now = timeGetTime();

    DWORD delta;

    if (pleaseKillMe > 0 && pleaseKillMe < 10000)
    {
        LongTime rightNow;

        DWORD age = countDownTime.MinutesDifference(&rightNow);
        if (age != 0)
        {
            --pleaseKillMe;
            countDownTime.SetToNow();

            int ageByFives = pleaseKillMe/5*5;

            if (pleaseKillMe < 10 || pleaseKillMe == ageByFives)
            {
                MessPlayerChatLine chatMess;
                sprintf(&chatMess.text[1],"AUTOMATED SERVER SHUTDOWN IN %d MINUTES.",pleaseKillMe);
                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess);

                sprintf(&chatMess.text[1],"After maintainence, the server will restart soon.");
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess);
            }
        }
    }

    // Server is going down, time to do things!
    if (0 == pleaseKillMe)
    {
        // save everyone!
        SharedSpace *sp = (SharedSpace *) spaceList->First();
        while (sp)
        {
            BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
            while (curMob)
            {
                if (SMOB_AVATAR == curMob->WhatAmI())
                {
                    BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
                    curAvatar->SaveAccount();
                }
                curMob = (BBOSMob *) sp->avatars->Next();
            }


            sp = (SharedSpace *) spaceList->Next();
        }

        // Save tokens
        tokenMan.Tick();
        tokenMan.Save();

        // Save ban list
        banList->Save();
        uidBanList->Save();

        // Save out guild towers
        FILE *fp = fopen("serverdata\\guilds.dat","w");
        if (fp)
        {
            fprintf(fp,"%f\n",VERSION_NUMBER);

            SharedSpace *sp = (SharedSpace *) spaceList->First();
            while (sp)
            {
                if (SPACE_GUILD == sp->WhatAmI())
                {
                    TowerMap *dm = (TowerMap *)sp;
                    fprintf(fp,"TOWER\n");

                    dm->ProcessVotes();

                    dm->Save(fp);
                }
                sp = (SharedSpace *) spaceList->Next();
            }

            fprintf(fp,"END\n");
            fclose(fp);
        }
    }

    delta = now - lastConnectTime;

    /*  NO MORE TRYING TO EMAIL FOR ERRORS
    if (delta > 1000 * 60 * 45 && 0 == errorContactVal)	
    {
        errorContactVal = 1;
        SendMailMessage("bigthom@vtext.com","No connections for 45 minutes.");
    }
    if (delta > 1000 * 60 * 55 && 1 == errorContactVal)	
    {
        errorContactVal = 2;
        SendMailMessage("bigthom@vtext.com","No connections for 55 minutes.");
    }
    */

    if (IOCPFlag)
    {
        // ***** bytes transfered
        delta = now - ((IOCPServer *)lserver)->lastTimeUpdate;

        if (0 == ((IOCPServer *)lserver)->lastTimeUpdate  || 
             now < ((IOCPServer *)lserver)->lastTimeUpdate)
        {
            delta = 1000 * 10  + 1;
        }

        // write out every 10 seconds
        if (delta > 1000 * 10)	
        {
            MaintainIncomingAvatars();  // do this every 10 seconds

            ((IOCPServer *)lserver)->lastTimeUpdate = now;

            FILE *source = fopen("byteflow.txt","a");
            
            /* Display operating system-style date and time. */
            _strdate( tempText );
            fprintf(source, "%s, ", tempText );
            _strtime( tempText );
            fprintf(source, "%s, ", tempText );

            fprintf(source,"%d, %d, %d, %d\n", 
                      (int) ((IOCPServer *)lserver)->bytesIn, 
                      (int) ((IOCPServer *)lserver)->bytesOut,
                      (int) ((IOCPServer *)lserver)->bytesIn + 
                      (int) ((IOCPServer *)lserver)->bytesOut,
                      (int) ((IOCPServer *)lserver)->curConnections()); 


            fclose(source);

            ((IOCPServer *)lserver)->bytesIn  = 0;
            ((IOCPServer *)lserver)->bytesOut = 0;

        }


        // ***** messages transfered
        delta = now - ((IOCPServer *)lserver)->lastMessUpdate;

        if (0 == ((IOCPServer *)lserver)->lastMessUpdate  || 
             now < ((IOCPServer *)lserver)->lastMessUpdate || 
             now == ((IOCPServer *)lserver)->lastMessUpdate)
        {
            delta = 1000 * 60 * 10 + 1;
        }

        // write out every 10 minutes
        if (delta > 1000 * 60 * 10)	
        {
            ((IOCPServer *)lserver)->lastMessUpdate = now;

            FILE *source = fopen("messageAmount.txt","a");
            
            /* Display operating system-style date and time. */
            _strdate( tempText );
            fprintf(source, "%s, ", tempText );
            _strtime( tempText );
            fprintf(source, "%s, ", tempText );

            for (int i = 0; i < 256; ++i)
            {
                fprintf(source,"%d, ", 
                      (int) ((IOCPServer *)lserver)->messSent[i]);
               ((IOCPServer *)lserver)->messSent[i] = 0; 
            }

            fprintf(source,"\n");
            fclose(source);

            source = fopen("messageBytes.txt","a");
            
            /* Display operating system-style date and time. */
            _strdate( tempText );
            fprintf(source, "%s, ", tempText );
            _strtime( tempText );
            fprintf(source, "%s, ", tempText );

            for (int i = 0; i < 256; ++i)
            {
                fprintf(source,"%d, ", 
                      (int) ((IOCPServer *)lserver)->messBytes[i]);
                ((IOCPServer *)lserver)->messBytes[i] = 0;
            }

            fprintf(source,"\n");
            fclose(source);



        }

    }
    // tick all of the spaces
    BBOSMob *curMob; //, *tempMob;

    lastAvatarCount = 0;
//	int randomAvatarIndex = rand() % (currentAvatarCount+1);
//	currentAvatarCount = 0;

    SharedSpace *sp = (SharedSpace *) spaceList->First();
    while (sp)
    {

        if (SPACE_GROUND == sp->WhatAmI())
        {
            delta = now - lastGraveyardTime;

            if (0 == lastGraveyardTime || now < lastGraveyardTime)
            {
                delta = 1000 * 10 + 1;
            }

            if (delta > 1000 * 10)	
            {
                lastGraveyardTime = now;

                // pet graveyard 90,53 and 71 206
                CheckGraveyard(sp, 90,53);
                CheckGraveyard(sp, 71,206);
            }
        }


        // tick all of this space's Avatars
    //	BBOSMob *curMob;

        int cont;
        cont = FALSE;

        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
//			if (randomAvatarIndex == currentAvatarCount)
//				ServerRandomAvatar = (BBOSAvatar *)curMob;

            ++lastAvatarCount;
            cont = TRUE;
            curMob->Tick(sp);
            if (curMob->isDead)
            {
                MessAvatarDeath messAD;
                int fromSocket = ((BBOSAvatar *)curMob)->socketIndex;
                messAD.avatarID = fromSocket;
                sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                sizeof(messAD),(void *)&messAD);

                ((BBOSAvatar *)curMob)->QuestSpaceChange(NULL, NULL);
                TransferAvatar(FALSE, fromSocket);

                tempReceiptList.clear();
                tempReceiptList.push_back(fromSocket);
                MessChangeMap changeMap;
                changeMap.oldType = sp->WhatAmI(); 
                changeMap.newType = SPACE_GROUND; 
                changeMap.sizeX   = MAP_SIZE_WIDTH;
                changeMap.sizeY   = MAP_SIZE_HEIGHT;
                lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                curMob->cellX = curMob->targetCellX = 
                    ((BBOSAvatar *)curMob)->charInfoArray[((BBOSAvatar *)curMob)->
                             curCharacterIndex].spawnX;
                curMob->cellY = curMob->targetCellY = 
                    ((BBOSAvatar *)curMob)->charInfoArray[((BBOSAvatar *)curMob)->
                             curCharacterIndex].spawnY;
                curMob->isDead = FALSE;

                TransferAvatar(TRUE, fromSocket);

                for (int i = 0; i < MONSTER_EFFECT_TYPE_NUM; ++i)
                    curMob->magicEffectAmount[i] = 0;

            }

            BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;

            // handle inter-system messages ment for a particular user
            tempReceiptList.clear();
            tempReceiptList.push_back(curAvatar->socketIndex);
            MessInfoText infoText;

            UserMessage *forMe = (UserMessage *) userMessageList->First();
            while (forMe)
            {
                if (!strcmp(forMe->WhoAmI(),curAvatar->name) && 
                     forMe->password[0] && 
                     !strcmp(forMe->password, curAvatar->pass))
                {
                    if (!strcmp(forMe->message, "PAYOUT"))
                    {
                        sprintf(infoText.text, "There are other players in me, Thank you!");
                        sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        sprintf(infoText.text, "There are %ld unopened chests.  You get %ld gold.", forMe->value2, forMe->value3);
                        sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        assert(
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money +=
                            forMe->value3;

                        assert(
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                        if (forMe->value3 > 0)
                        {
                            FILE *source = fopen("chests.txt","a");
                            
                            /* Display operating system-style date and time. */
                            _strdate( tempText );
                            fprintf(source, "%s, ", tempText );
                            _strtime( tempText );
                            fprintf(source, "%s, ", tempText );

                            fprintf(source,"PAYOUT, %s, %d, %d, %d\n",
                                     curAvatar->name,
                                     forMe->value1, forMe->value2, forMe->value3);

                            fclose(source);
                        }
                        userMessageList->Remove(forMe);
                        delete forMe;
                        forMe = (UserMessage *) userMessageList->First();
                    }
                    else
                    {
                        memcpy(infoText.text, forMe->message, MESSINFOTEXTLEN-1);
                        infoText.text[MESSINFOTEXTLEN-1] = 0;
                        sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        userMessageList->Remove(forMe);
                        delete forMe;
                        forMe = (UserMessage *) userMessageList->First();
                    }
                }
                else if (forMe->age < timeGetTime())
                {
                    userMessageList->Remove(forMe);
                    delete forMe;

                    forMe = (UserMessage *) userMessageList->First();
                }
                else
                    forMe = (UserMessage *) userMessageList->Next();
            }

            if (curAvatar->activeCounter >= 4 && ACCOUNT_TYPE_ADMIN != curAvatar->accountType)
            {
                // boot this sloth!
                sprintf(infoText.text,"%s is inactive; logged off.",
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                sp->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                    sizeof(infoText),(void *)&infoText);

                HandleKickoff(curAvatar, sp);
            }
            else if (curAvatar->kickOff)
            {
                // boot this duper!
                sprintf(infoText.text,"%s is logged-on twice; forced off.",
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                sp->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                    sizeof(infoText),(void *)&infoText);

                HandleKickoff(curAvatar, sp);
            }
            else if (false && curAvatar->kickMeOffNow) // no more kicking off expired peeps
            {
                // boot this expired account!
                HandleKickoff(curAvatar, sp);
            }

            curMob = (BBOSMob *) sp->avatars->Next();
        }

        if (cont)
        {
            // tick all of this space's Mobs
            int mobTickCount = 0;

            curMob = (BBOSMob *) sp->mobList->GetFirst(0,0,1000);
            while (curMob)
            {
                if (SMOB_MAX <= curMob->WhatAmI() || curMob->WhatAmI() < 0)
                    cont = TRUE;

                curMob->Tick(sp);
                ++mobTickCount;

                int lastBranch = 0;

                if (curMob->isDead && SMOB_MONSTER == curMob->WhatAmI())
                {
                    lastBranch = 1;

                    if (((BBOSMonster *)curMob)->uniqueName[0] ||
                         ((BBOSMonster *)curMob)->type == 7     || // if a dragon
                        ((BBOSMonster *)curMob)->type == 16    || // if a spirit dragon
                        ((BBOSMonster *)curMob)->type == 27    || // if a vlord
                       (((BBOSMonster *)curMob)->type == 11 && 
                        ((BBOSMonster *)curMob)->subType == 1) // if a Dokk's Centurion
                      )
                    {
                        sprintf(tempText,"-----------------------------  %s killed! ",
                            ((BBOSMonster *)curMob)->Name());
                        LogOutput("gamelog.txt", tempText);

                        LongTime lt;
                        lt.value.wHour += 19;
                        if (lt.value.wHour < 24)
                        {
//							lt.value.wHour += 24;
                            lt.value.wDay -= 1;
                        }
                        else
                            lt.value.wHour -= 24;

                        sprintf(tempText,"%d/%02d, %d:%02d\n", (int)lt.value.wMonth, (int)lt.value.wDay, 
                                  (int)lt.value.wHour, (int)lt.value.wMinute);
                        LogOutput("gamelog.txt", tempText);


                        // who was there?
                        sprintf(tempText,"----- attending: ");
                        LogOutput("gamelog.txt", tempText);

                        BBOSMob *curMob2 = (BBOSMob *) sp->avatars->First();
                        while (curMob2)
                        {
                            if (abs(curMob->cellX - curMob2->cellX) < 2 &&
                                abs(curMob->cellY - curMob2->cellY) < 2 )
                            {
                                BBOSAvatar *curAvatar2 = (BBOSAvatar *) curMob2;

                                sprintf(tempText,"%s, ", 
                                    curAvatar2->charInfoArray[curAvatar2->curCharacterIndex].name);
                                LogOutput("gamelog.txt", tempText);

                                // reward for killing Dokk!
                                if (((BBOSMonster *)curMob)->type == 16 && 
                                ((BBOSMonster *)curMob)->subType == 1) // if Dokk
                                {
                                    curAvatar2->charInfoArray[curAvatar2->curCharacterIndex].imageFlags |= 
                                      SPECIAL_LOOK_DOKK_KILLER;
                                    MessAvatarStats mStats;
                                    curAvatar2->BuildStatsMessage(&mStats);
                                    sp->SendToEveryoneNearBut(0, curAvatar2->cellX, curAvatar2->cellY,
                                              sizeof(mStats),(void *)&mStats);
                                    sp->avatars->Find(curAvatar2);
                                }
                                // reward for killing anubis!
                                if (((BBOSMonster *)curMob)->type == 20 && 
                                ((BBOSMonster *)curMob)->subType == 0) // if anubis
                                {
                                    curAvatar2->charInfoArray[curAvatar2->curCharacterIndex].imageFlags |= 
                                      SPECIAL_LOOK_ANUBIS_KILLER;
                                    MessAvatarStats mStats;
                                    curAvatar2->BuildStatsMessage(&mStats);
                                    sp->SendToEveryoneNearBut(0, curAvatar2->cellX, curAvatar2->cellY,
                                              sizeof(mStats),(void *)&mStats);
                                    sp->avatars->Find(curAvatar2);
                                }
                                // reward for killing overlord!
                                if (((BBOSMonster *)curMob)->type == 7 && 
                                ((BBOSMonster *)curMob)->subType == 4) // if Dragon Overlord
                                {
                                    if (!strcmp(((BBOSMonster *)curMob)->uniqueName,"Dragon Overlord"))
                                    {
                                        curAvatar2->charInfoArray[curAvatar2->curCharacterIndex].imageFlags |= 
                                          SPECIAL_LOOK_OVERLORD_KILLER;
                                        MessAvatarStats mStats;
                                        curAvatar2->BuildStatsMessage(&mStats);
                                        sp->SendToEveryoneNearBut(0, curAvatar2->cellX, curAvatar2->cellY,
                                                     sizeof(mStats),(void *)&mStats);
                                        sp->avatars->Find(curAvatar2);
                                    }
                                }


                            }

                            curMob2 = (BBOSMob *) sp->avatars->Next();
                        }

                        sprintf(tempText,"-----\n");
                        LogOutput("gamelog.txt", tempText);

                        // special effect for killing Dokk!
                        if (((BBOSMonster *)curMob)->type == 16 && 
                        ((BBOSMonster *)curMob)->subType == 1) // if Dokk
                        {
                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = (long)curMob;
                            messGE.x        = curMob->cellX;
                            messGE.y        = curMob->cellY;
                            messGE.r        = 255;
                            messGE.g        = 0;
                            messGE.b        = 255;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 20; // in seconds
                            sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                              sizeof(messGE),(void *)&messGE);

                            // 3 viz ingots for Dokk
                            sp->DoMonsterDropSpecial((BBOSMonster *)curMob,1);
                            sp->DoMonsterDropSpecial((BBOSMonster *)curMob,1);
                            sp->DoMonsterDropSpecial((BBOSMonster *)curMob,1);
                        }

                        // special effect for killing Anubis!
                        if (((BBOSMonster *)curMob)->type == 20 && 
                        ((BBOSMonster *)curMob)->subType == 0) 
                        {
                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = (long)curMob;
                            messGE.x        = curMob->cellX;
                            messGE.y        = curMob->cellY;
                            messGE.r        = 255;
                            messGE.g        = 210;
                            messGE.b        = 0;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 20; // in seconds
                            sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                              sizeof(messGE),(void *)&messGE);
                        }

                        // special effect for killing overlord!
                        if (((BBOSMonster *)curMob)->type == 7 && 
                        ((BBOSMonster *)curMob)->subType == 4) 
                        {
                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = (long)curMob;
                            messGE.x        = curMob->cellX;
                            messGE.y        = curMob->cellY;
                            messGE.r        = 200;
                            messGE.g        = 200;
                            messGE.b        = 255;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 20; // in seconds
                            sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                              sizeof(messGE),(void *)&messGE);
                        }

                        // special effect for killing spidren bosses!
                        if (((BBOSMonster *)curMob)->type == 24 && 
                        ((BBOSMonster *)curMob)->subType == 4) 
                        {
                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = (long)curMob;
                            messGE.x        = curMob->cellX;
                            messGE.y        = curMob->cellY;
                            messGE.r        = 100;
                            messGE.g        = 255;
                            messGE.b        = 100;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 20; // in seconds
                            sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                              sizeof(messGE),(void *)&messGE);
                        }

                        // special effect for killing vlords
                        if (((BBOSMonster *)curMob)->type == 27)
                        {
                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = (long)curMob;
                            messGE.x        = curMob->cellX;
                            messGE.y        = curMob->cellY;
                            messGE.r        = 255;
                            messGE.g        = 100;
                            messGE.b        = 200;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 20; // in seconds
                            sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                              sizeof(messGE),(void *)&messGE);
                        }

                        if (((BBOSMonster *)curMob)->type == 11 && 
                        ((BBOSMonster *)curMob)->subType == 1) // if centurion
                        {
                            sp->DoMonsterDropSpecial((BBOSMonster *)curMob,1);
                        }

                        if (((BBOSMonster *)curMob)->type == 16) // if spirit dragon
                        {
                            // small chance of dropping viz ingot
                            if (!(rand() % 20))
                                sp->DoMonsterDropSpecial((BBOSMonster *)curMob,1);
                        }
                    }

                    if (((BBOSMonster *)curMob)->type == 23 && 
                    ((BBOSMonster *)curMob)->subType == 2) // if Dusk Wurm
                    {
                        sp->DoMonsterDropSpecial((BBOSMonster *)curMob,6); // 1 black dust
                    }

                    MessMonsterDeath messMD;
                    messMD.mobID = (unsigned long) curMob;
                    sp->SendToEveryoneNearBut(0, curMob->cellX, curMob->cellY,
                                    sizeof(messMD),(void *)&messMD);

                    sp->mobList->Remove(curMob);

    //				Inventory *inv = &(groundInventory[128*curMob->cellY + curMob->cellX]);

                    // drop goodies!
                    BBOSMonster *monster = (BBOSMonster *) curMob;

                    if (monster->controllingAvatar)
                    {
                        SharedSpace *sx;
                        if (FindAvatar(monster->controllingAvatar, &sx))
                        {
                            monster->controllingAvatar->controlledMonster = NULL;
                        }
                        monster->controllingAvatar = NULL;
                    }


                    if (monster->type == 25 && monster->dropAmount <= 0) // vanished vamp
                        ;
                    else
                        sp->DoMonsterDrop(monster);

                    if (monster->bane)
                    {
                        SharedSpace *tempss;
                        BBOSAvatar *curAvatar = FindAvatar(monster->bane,&tempss);
                        if (curAvatar)
                        {
                            int bestSkill = 0;
                            Inventory *inv = (curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills);
                            InventoryObject *io = (InventoryObject *) inv->objects.First();
                            while(io)
                            {
                                if (!strcmp("Dodging",io->WhoAmI()))
                                {
                                    InvSkill *skillInfo = (InvSkill *) io->extra;
                                    if (bestSkill < skillInfo->skillLevel)
                                        bestSkill = skillInfo->skillLevel;
                                }
                                io = (InventoryObject *) inv->objects.Next();
                            }

                            if (bestSkill < monster->toHit + 8 && 44 == (rand() % 140))
                            {
                                sp->DoMonsterDropSpecial(monster,0);
                            }
                        }
                    }

                    // if in the realm of the dead, small chance of dropping stuff
                    if (SPACE_REALM == sp->WhatAmI() && REALM_ID_DEAD == ((RealmMap *)sp)->type)
                    {
                        if (44 == (rand() % 100))
                        {
                            int dropType = (rand() % 3) +2; 
                            sp->DoMonsterDropSpecial(monster,dropType);
                        }

                        // if wraith or wraith mistress
                        if (18 == monster->type && monster->subType >= 2 && 4 == (rand() % 30))
                        {
                            sp->DoMonsterDropSpecial(monster,2);
                        }

                    }

                    // if in the realm of dragons, small chance of dropping stuff
                    if (SPACE_REALM == sp->WhatAmI() && REALM_ID_DRAGONS == ((RealmMap *)sp)->type)
                    {
                        if (((BBOSMonster *)curMob)->type == 7) // if dragon
                        {
                            if (3 == (rand() % 40))
                            {
                                int dropType = (rand() % 4);
                                switch(dropType)
                                {
                                case 0:
                                    sp->DoMonsterDropSpecial(monster, 0); // blue dust
                                    break;
                                case 1:
                                    sp->DoMonsterDropSpecial(monster, 5); // green dust
                                    break;
                                case 2:
                                    sp->DoMonsterDropSpecial(monster, 10); // oak staff
                                    break;
                                case 3:
                                    sp->DoMonsterDropSpecial(monster, 11); // dragon totem
                                    break;
                                }
                            }
                        }
                        else if (((BBOSMonster *)curMob)->type == 22) // if orc
                        {
                            if (((BBOSMonster *)curMob)->subType == 4) // if orc	champion
                            {
                                sp->DoMonsterDropSpecial(monster, 12); // azides brick
                                sp->DoMonsterDropSpecial(monster, 13); // nitrate brick
                                sp->DoMonsterDropSpecial(monster, 14); // cotton rope
                            }
                            else if (3 == (rand() % 60))
                            {
                                int dropType = (rand() % 2);
                                switch(dropType)
                                {
                                case 0:
                                    sp->DoMonsterDropSpecial(monster, 12); // azides brick
                                    break;
                                case 1:
                                    sp->DoMonsterDropSpecial(monster, 13); // nitrate brick
                                    break;
                                }
                            }
                        }
                        else if (((BBOSMonster *)curMob)->type == 23) // if wurm
                        {
                            if (3 == (rand() % 60))
                            {
                                sp->DoMonsterDropSpecial(monster, 6); // black dust
                            }
                        }
                    }

                    // if in the realm of the dead, small chance of dropping stuff
                    if (SPACE_LABYRINTH == sp->WhatAmI())
                    {
                        if (48 == (rand() % 150))
                        {
                            sp->DoMonsterDropSpecial(monster,15);
                        }
                        if (68 == (rand() % 150))
                        {
                            sp->DoMonsterDropSpecial(monster,16);
                        }

                        if (REALM_ID_LAB2 == ((LabyrinthMap *)sp)->type)
                        {
                            if (48 == (rand() % 450))
                            {
                                sp->DoMonsterDropSpecial(monster,22);
                            }
                        }

                    }

                    // if in a dungeon, place a grave
                    if (SPACE_DUNGEON == sp->WhatAmI() && !monster->dontRespawn &&
                         !(((DungeonMap *) sp)->specialFlags & SPECIAL_DUNGEON_TEMPORARY))
                    {
                        if (monster->type == 11) // if Dokk's centurion
                            ;
                        else
                        {
                            BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                     monster->type, monster->subType,
                                     monster->cellX, monster->cellY);
                            mg->isWandering = monster->isWandering;
                            sp->mobList->Add(mg);

                            // if in a dungeon, possibly drop explosives
                            if (5 == (rand() % 7))
                            {
                                int dropType = monster->toHit/4 + 7;
                                if (dropType < 7)
                                    dropType = 7;
                                if (dropType > 9)
                                    dropType = 9;
                                sp->DoMonsterDropSpecial(monster,dropType);
                            }
                        }
                    }
                    else if (((BBOSMonster *)curMob)->type == 25 &&
                              ((BBOSMonster *)curMob)->dropAmount > 0) // killed vamp
                    {
                        int baseVal = ((BBOSMonster *)curMob)->subType;

                        for (int i = 0; i < baseVal + 1; ++i)
                        {
                            // chance of dropping a scroll
                            if ((rand() % 30) > 30 - (2 + baseVal*2))
                            {
                                int dropType = 19 + (rand() % 2);
                                sp->DoMonsterDropSpecial(monster,dropType);
                            }

                            // chance of dropping a lava totem
                            if ((rand() % 100) > 100 - (1 + baseVal*2))
                            {
                                sp->DoMonsterDropSpecial(monster,18);
                            }
                        }
                    }
                    else if (((BBOSMonster *)curMob)->type == 16 && 
                        ((BBOSMonster *)curMob)->subType == 1 && !monster->dontRespawn) // if Dokk
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);
                    }
                    else if (((BBOSMonster *)curMob)->type == 11 && 
                        ((BBOSMonster *)curMob)->subType == 1 && !monster->dontRespawn) // if Dokk's centurion
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);
                    }
                    else if (!strcmp(((BBOSMonster *)curMob)->uniqueName,"Trickster Minotaur"))
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);

                        int deathCount = monster->toHit + 1;
                        if (deathCount < 1)
                            deathCount = 1;
                        if (deathCount > 3000)
                            deathCount = 3000;

                        strncpy(mg->uniqueName, monster->uniqueName, 32);
                        mg->r              = 255;                  
                        mg->g              = 0;                    
                        mg->b              = 0;                    
                        mg->a              = 255;                  
                        mg->sizeCoeff      = 1.0f + 1.0f * deathCount/50; 
                        mg->health         = 100 * deathCount;     
                        mg->maxHealth      = 100 * deathCount;     
                        mg->damageDone     = deathCount/2;         
                        mg->toHit          = deathCount;           
                        mg->defense        = deathCount;           
                        mg->dropAmount     = deathCount;           
                        mg->magicResistance= 1.0f - 1.0f/deathCount;

                        mg->spawnTime = timeGetTime() + 1000 * deathCount;


                        FILE *fp = fopen("serverdata\\tricksterMino.dat","w");
                        if (fp)
                        {
                            fprintf(fp,"%d",monster->toHit + 1);
                            fclose(fp);
                        }

                    }
                    else if ( ((BBOSMonster *)curMob)->type == 28 && !monster->dontRespawn &&
                            !(((DungeonMap *) sp)->specialFlags & SPECIAL_DUNGEON_TEMPORARY)) {	// lizard men
                        if( ((BBOSMonster *)curMob)->subType < 3 && rand() % 400 == 58 ) {
                            sp->DoMonsterDropSpecial( monster, 23 );
                        }
                        else if( rand() % 200 == 53 ) {
                            sp->DoMonsterDropSpecial( monster, 23 );
                        }
                    }
                    else if (((BBOSMonster *)curMob)->type == 24 && 
                        ((BBOSMonster *)curMob)->subType == 4 && !monster->dontRespawn) // if spidren boss
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);

                        strncpy(mg->uniqueName, monster->uniqueName, 32);
                        mg->r              = monster->r;                  
                        mg->g              = monster->g;                    
                        mg->b              = monster->b;                    
                        mg->a              = monster->a;                  
                        mg->sizeCoeff      = monster->sizeCoeff; 
                        mg->health         = monster->maxHealth;     
                        mg->maxHealth      = monster->maxHealth;     
                        mg->damageDone     = monster->damageDone;         
                        mg->toHit          = monster->toHit;           
                        mg->defense        = monster->defense;           
                        mg->dropAmount     = monster->dropAmount;           
                        mg->magicResistance= monster->magicResistance;

                        mg->spawnTime = timeGetTime() + 1000 * 60 * 60 * 4;
//						mg->spawnTime = timeGetTime() + 1000 * 60;	// TEST!!!!

                        if (!strcmp(((BBOSMonster *)curMob)->uniqueName,"Larath"))
                            sp->DoMonsterDropSpecial(monster,17);
                        else if (!strcmp(((BBOSMonster *)curMob)->uniqueName,"Moultz"))
                        {
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                        }
                        else if (!strcmp(((BBOSMonster *)curMob)->uniqueName,"Reorth"))
                        {
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                            sp->DoMonsterDropSpecial(monster,17);
                        }
                    }
                    else if (((BBOSMonster *)curMob)->type == 27 && !monster->dontRespawn ) // if vlord
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);

                        mg->magicResistance= monster->magicResistance;

                        mg->spawnTime = timeGetTime() + 1000 * 60 * 60 * 4;
//						mg->spawnTime = timeGetTime() + 1000 * 60;	// TEST!!!!

                        if( monster->isVagabond ) {
                            if( monster->toHit > 250 )
                                sp->DoMonsterDropSpecial(monster,22);
                        }
                        else {
                            for (int i = 0; i < 1 + ((BBOSMonster *)curMob)->subType * 2; ++i)
                                sp->DoMonsterDropSpecial(monster,22);
                        }
                    }

                    if (SPACE_GROUND == sp->WhatAmI() && 
                         ((BBOSMonster *)curMob)->type == 19 && 
                        ((BBOSMonster *)curMob)->subType == 0) // if bone warrior in normal world
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);

                        HandleDeadGate(curMob->cellX, curMob->cellY, sp);

                    }

                    if (SPACE_GROUND == sp->WhatAmI() && 
                         ((BBOSMonster *)curMob)->type == 9 && 
                        ((BBOSMonster *)curMob)->subType == 0 && !monster->dontRespawn) // if spirit vision in normal world
                    {
                        BBOSMonsterGrave *mg = new BBOSMonsterGrave(
                                monster->type, monster->subType,
                                 monster->spawnX, monster->spawnY);
                        mg->isWandering = FALSE;
                        sp->mobList->Add(mg);

                        HandleSpiritGate(curMob->cellX, curMob->cellY, sp);

                    }

                    delete curMob;
                }
                else if (curMob->isDead && SMOB_MONSTER_GRAVE == curMob->WhatAmI())
                {
                    lastBranch = 2;

                    sp->mobList->Remove(curMob);

                    BBOSMonsterGrave *monsterGrave = (BBOSMonsterGrave *) curMob;

                    BBOSMonster *monster = new BBOSMonster(
                                      monsterGrave->type, monsterGrave->subType, NULL);
                    monster->cellX = monsterGrave->spawnX;
                    monster->cellY = monsterGrave->spawnY;
                    monster->targetCellX = monsterGrave->spawnX;
                    monster->targetCellY = monsterGrave->spawnY;
                    monster->spawnX = monsterGrave->spawnX;
                    monster->spawnY = monsterGrave->spawnY;
                    monster->isWandering = monsterGrave->isWandering;
                    sp->mobList->Add(monster);

                    if (!monsterGrave->uniqueName[0])
                    {
                        MessMobAppear mobAppear;
                        mobAppear.mobID = (unsigned long) monster;
                        mobAppear.type = monster->WhatAmI();
                        mobAppear.monsterType = monster->type;
                        mobAppear.subType = monster->subType;

                        if(SPACE_DUNGEON == sp->WhatAmI())
                        {
                            mobAppear.staticMonsterFlag = FALSE;
                            if (!monster->isWandering)
                                mobAppear.staticMonsterFlag = TRUE;
                        }

                        mobAppear.x = monster->cellX;
                        mobAppear.y = monster->cellY;
                        sp->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
                                     sizeof(mobAppear), &mobAppear);
                    }
                    else
                    {
                        strncpy(monster->uniqueName, monsterGrave->uniqueName,32);
                        monster->r              = monsterGrave->r;
                        monster->g              = monsterGrave->g;
                        monster->b              = monsterGrave->b;
                        monster->a              = monsterGrave->a;
                        monster->sizeCoeff      = monsterGrave->sizeCoeff;
                        monster->health         = monsterGrave->health;
                        monster->maxHealth      = monsterGrave->maxHealth;
                        monster->damageDone     = monsterGrave->damageDone;
                        monster->toHit          = monsterGrave->toHit;
                        monster->defense        = monsterGrave->defense;
                        monster->dropAmount     = monsterGrave->dropAmount;
                        monster->magicResistance= monsterGrave->magicResistance;

                        MessMobAppearCustom mAppearCustom;
                        mAppearCustom.type = SMOB_MONSTER;
                        mAppearCustom.mobID = (unsigned long) monster;
                        mAppearCustom.x = monster->cellX;
                        mAppearCustom.y = monster->cellY;
                        mAppearCustom.monsterType = monster->type;
                        mAppearCustom.subType = monster->subType;
                        CopyStringSafely(monster->Name(), 32, mAppearCustom.name, 32);
                        mAppearCustom.a = monster->a;
                        mAppearCustom.r = monster->r;
                        mAppearCustom.g = monster->g;
                        mAppearCustom.b = monster->b;
                        mAppearCustom.sizeCoeff = monster->sizeCoeff;

                        sp->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
                                   sizeof(mAppearCustom), &mAppearCustom);
                    }


                    delete curMob;
                }
                else if (curMob->isDead && SMOB_BOMB == curMob->WhatAmI())
                {
                    lastBranch = 3;

                    sp->mobList->Remove(curMob);
                    delete curMob;
                }

                curMob = (BBOSMob *) sp->mobList->GetNext();
            }
        }

//		if (cont)
        if (cont || SPACE_REALM == sp->WhatAmI())
        {
            // tick all of this space's Generators

            BBOSGenerator *curGen = (BBOSGenerator *) sp->generators->First();
            while (curGen)
            {
                curGen->Tick(sp);
                curGen = (BBOSGenerator *) sp->generators->Next();
            }
        }
/*
        if (SPACE_LABYRINTH == sp->WhatAmI() && REALM_ID_LAB2 == ((LabyrinthMap *)sp)->type)
        {

            delta = now - labyMapTime;

            if (0 == labyMapTime || now < labyMapTime || now == labyMapTime)
            {
                delta = 1000 * 60 + 1;
            }

            if (delta > 1000 * 60)	
            {
                labyMapTime = now;

                ((LabyrinthMap *)sp)->UpdateMonsterMap();

            }
        }
        */

        sp = (SharedSpace *) spaceList->Next();
    }


    // process 10 second items
    delta = now - tenSecondTime;

    if (0 == tenSecondTime || now < tenSecondTime || now == tenSecondTime)
    {
        delta = 1000 * 10 + 1;
    }

    if (delta > 1000 * 10)	
    {
        tenSecondTime = now;

        int oldWeatherState = weatherState;

        int oneHour = 60 * 6;
        if (dayTimeCounter > 2.75f * oneHour &&
             dayTimeCounter < 3.75f * oneHour)
        {
            if (1 == weatherState) // if storming
            {
//				if (7 == rand() % 10) // TEST
                if (7 == rand() % 15)
                    weatherState = 0; // clears up
            }
            else if (0 == weatherState) // if normal
            {
//				if (7 == rand() % 10) // TEST
                if (7 == rand() % 50)
                    weatherState = 1; // lightning storm
            }
        }
        else
            weatherState = 0; // clears up

        if (oldWeatherState != weatherState)
        {
            MessWeatherState state;
            state.value = weatherState;
            lserver->SendMsg(sizeof(state),(void *)&state, 0, NULL);
        }

        ++dayTimeCounter;
        if (dayTimeCounter >= 4 * 60 * 6)
        {
            dayTimeCounter = 0;

            ++smasherCounter;
            // find a tower to destroy

            TowerMap *candidate = NULL;
            GroundMap *gm = NULL;
            SharedSpace *sp = (SharedSpace *) spaceList->First();
            while (sp)
            {
                if (SPACE_GROUND == sp->WhatAmI())
                    gm = (GroundMap *) sp;
                else if (SPACE_GUILD == sp->WhatAmI())
                {
                    TowerMap *dm = (TowerMap *)sp;
                    LongTime rightNow;

                    DWORD age = dm->lastChangedTime.MinutesDifference(&rightNow);

                    DWORD ageOut = 60 * 24 * 21; // three weeks for normal guild
                    if (!dm->itemBox->objects.IsListEmpty() ||
                         dm->specLevel[0] > 0 || dm->specLevel[1] > 0 || dm->specLevel[2] > 0)
                        ageOut = 60 * 24 * 7 * 8; // eight weeks for big guild

                    if (age > ageOut)
                    {
                        candidate = dm;
                    }
                }
                sp = (SharedSpace *) spaceList->Next();
            }

            if (candidate && gm && 2 == (smasherCounter % 5))
            {
                // find and remove the outside tower
                BBOSTower *towerMob = NULL;
                curMob = (BBOSMob *) gm->mobList->GetFirst(0,0,1000);
                while (curMob)
                {
                    if (SMOB_TOWER == curMob->WhatAmI() && ((BBOSTower *)curMob)->isGuildTower &&
                         ((BBOSTower *)curMob)->ss == candidate)
                    {
                        towerMob = (BBOSTower *)curMob;
                        gm->mobList->SetToLast();
                    }
                    curMob = (BBOSMob *) gm->mobList->GetNext();
                }

                if (towerMob)
                {
                    // announce it's removal
                    MessMobDisappear messMobDisappear;
                    messMobDisappear.mobID = (unsigned long) towerMob;
                    messMobDisappear.x = towerMob->cellX;
                    messMobDisappear.y = towerMob->cellY;
                    gm->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY,
                        sizeof(messMobDisappear),(void *)&messMobDisappear);

                    // add a generic effect on that spot
                    MessGenericEffect messGE;
                    messGE.avatarID = -1;
                    messGE.mobID    = (long)towerMob;
                    messGE.x        = towerMob->cellX;
                    messGE.y        = towerMob->cellY;
                    messGE.r        = 255;
                    messGE.g        = 255;
                    messGE.b        = 255;
                    messGE.type     = 0;  // type of particles
                    messGE.timeLen  = 20; // in seconds
                    gm->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY,
                                         sizeof(messGE),(void *)&messGE);

                    MessExplosion explo;
                    explo.avatarID = 0;
                    explo.r = 255;
                    explo.g = 255;
                    explo.b = 000;
                    explo.type = 0;
                    explo.flags = 0;
                    explo.size = 1000;
                    explo.x = towerMob->cellX;
                    explo.y = towerMob->cellY;

                    gm->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY, 
                        sizeof(explo), &explo, 5 + 1000/10);

                    // create a big nasty monster on that spot

                    BBOSMonster *monster = new BBOSMonster(1,5, NULL);  // granite golem
                    monster->cellX = monster->targetCellX = monster->spawnX = towerMob->cellX;
                    monster->cellY = monster->targetCellY = monster->spawnY = towerMob->cellY;
                    gm->mobList->Add(monster);
                    sprintf(monster->uniqueName, "Tower Smasher");

                    //	adjusts its power
                    monster->r               = 0;
                    monster->g               = 255;
                    monster->b               = 255;
                    monster->a               = 255;
                    monster->sizeCoeff       = 3;
                    monster->health          *= 10;
                    monster->maxHealth       *= 10;
                    monster->damageDone       = 45;

                    monster->toHit           = 20 + (rand() % 80);
                    monster->defense         = 20 + (rand() % 80);
                    monster->dropAmount      = 30;
                    monster->magicResistance = 0.6f;
                    monster->healAmountPerSecond = 200 + (rand() % 300);

                    // announce it
                    MessMobAppear mobAppear;
                    mobAppear.mobID = (unsigned long) monster;
                    mobAppear.type = monster->WhatAmI();
                    mobAppear.monsterType = monster->type;
                    mobAppear.subType = monster->subType;
                    mobAppear.x = monster->cellX;
                    mobAppear.y = monster->cellY;
                    gm->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
                                 sizeof(mobAppear), &mobAppear);

                    // tell everyone what's happening
                    sprintf(&(tempText[1]),"You hear a tremendous crash, and a low, rumbling laugh!");
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    bboServer->lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

                    // delete the outside tower
                    gm->mobList->Remove(towerMob);
                    delete towerMob;

                }
                else
                    dayTimeCounter = 3000;  // instantly try to do it all again

                spaceList->Remove(candidate);
                delete candidate;
            }
        }
    }

    // process dungeons
    delta = now - dungeonUpdateTime;

    if (0 == dungeonUpdateTime || now < dungeonUpdateTime || now == dungeonUpdateTime)
    {
        delta = 1000 * 60 * 10 + 1;
    }

    if (delta > 1000 * 60 * 10)	
    {
        tokenMan.Tick();
        tokenMan.Save();

        banList->Save();
        uidBanList->Save();

        // save out guild towers *************
        FILE *fp = fopen("serverdata\\guilds.dat","w");
        if (fp)
        {
            fprintf(fp,"%f\n",VERSION_NUMBER);

            SharedSpace *sp = (SharedSpace *) spaceList->First();
            while (sp)
            {
                if (SPACE_GUILD == sp->WhatAmI())
                {
                    TowerMap *dm = (TowerMap *)sp;
                    fprintf(fp,"TOWER\n");

                    dm->ProcessVotes();

                    dm->Save(fp);
                }
                sp = (SharedSpace *) spaceList->Next();
            }

            fprintf(fp,"END\n");
            fclose(fp);
        }


        // save out dungeons *************
        dungeonUpdateTime = now;
        fp = fopen("serverdata\\dungeons.dat","w");
        if (fp)
        {
            fprintf(fp,"%f\n",VERSION_NUMBER);

            DungeonMap *needyDungeon = NULL;

            // make absolutely sure ServerRandomAvatar is valid.
            /*
            if (ServerRandomAvatar)
            {
                SharedSpace *tempss;
                if (!FindAvatar(ServerRandomAvatar,&tempss))
                    ServerRandomAvatar = NULL;
            }
              */
            SharedSpace *sp = (SharedSpace *) spaceList->First();
            while (sp)
            {
                if (SPACE_DUNGEON == sp->WhatAmI())
                {
                    DungeonMap *dm = (DungeonMap *)sp;
                    if (0 == dm->specialFlags)
                    {
                        fprintf(fp,"DUNGEON\n");
                        dm->Save(fp);
                    }
                    else if (dm->specialFlags & SPECIAL_DUNGEON_TEMPORARY)
                    {
                        if (dm->avatars->IsListEmpty())
                        {
                            spaceList->Remove(dm);
                            delete dm;
                        }
                    }
                    /*
                    if (dm->masterName[0])
                    {
                        // if ServerRandomAvatar is already the master here, invalidate it.
                        if (ServerRandomAvatar &&
                             !strcmp(ServerRandomAvatar->name,dm->masterName) &&
                             !strcmp(ServerRandomAvatar->pass,dm->masterPass))
                             ServerRandomAvatar = NULL;
                    }
                    */
                }
                sp = (SharedSpace *) spaceList->Next();
            }

            fprintf(fp,"END\n");
            fclose(fp);

            LongTime ltNow;
            /*
            sp = (SharedSpace *) spaceList->First();
            while (sp)
            {
                if (SPACE_DUNGEON == sp->WhatAmI())
                {
                    DungeonMap *dm = (DungeonMap *)sp;

                    if (dm->masterName[0])
                    {
                        if (ltNow.MinutesDifference(&(dm->masterTimeout)) <= 0)
                        {
                            // time to say goodbye to this master!
                            UserMessage *forMe = new UserMessage(0, dm->masterName);
                            userMessageList->Append(forMe);
                            sprintf(forMe->password, dm->masterPass);
                            sprintf(forMe->message, "You are no longer the mistress of a dungeon.");
                            dm->masterName[0] = 0;
                            dm->masterPass[0] = 0;
                        }
                        else
                        {
                            // tell the master how much time is left
                            UserMessage *forMe = new UserMessage(0, dm->masterName);
                            userMessageList->Append(forMe);
                            sprintf(forMe->password, dm->masterPass);
                            sprintf(forMe->message, "You are the mistress of the %s", dm->name);

                            forMe = new UserMessage(0, dm->masterName);
                            userMessageList->Append(forMe);
                            sprintf(forMe->password, dm->masterPass);
                            if (ltNow.MinutesDifference(&(dm->masterTimeout)) < 60)
                                sprintf(forMe->message, "at %dN %dE for %d more minutes.", 
                                         256-dm->enterY, 256-dm->enterX, 
                                          ltNow.MinutesDifference(&(dm->masterTimeout)));
                            else if (ltNow.MinutesDifference(&(dm->masterTimeout)) < 60 * 24)
                                sprintf(forMe->message, "at %dN %dE for %d more hours.", 
                                         256-dm->enterY, 256-dm->enterX, 
                                          ltNow.MinutesDifference(&(dm->masterTimeout)) / 60);
                            else 
                                sprintf(forMe->message, "at %dN %dE for %d more days.", 
                                         256-dm->enterY, 256-dm->enterX, 
                                          ltNow.MinutesDifference(&(dm->masterTimeout)) / 60 / 24);

                            // now, handle payout
                            forMe = new UserMessage(0, dm->masterName);
                            sprintf(forMe->password, dm->masterPass);
                            sprintf(forMe->message, "PAYOUT");

                            forMe->value1 = forMe->value2 = forMe->value3 = 0;

                            curMob = (BBOSMob *) sp->avatars->First();
                            while (curMob)
                            {
                                if (!strcmp(((BBOSAvatar *)curMob)->name,dm->masterName) &&
                                   !strcmp(((BBOSAvatar *)curMob)->pass,dm->masterPass))
                                    ;
                                else
                                    ++(forMe->value1);

                                curMob = (BBOSMob *) sp->avatars->Next();
                            }

                            curMob = (BBOSMob *) sp->mobList->GetFirst(0,0,1000);
                            while (curMob)
                            {
                                if (SMOB_CHEST == curMob->WhatAmI())
                                {
                                    if (((BBOSChest *)curMob)->isOpen)
                                        ;
                                    else
                                        ++(forMe->value2);
                                }

                                curMob = (BBOSMob *) sp->mobList->GetNext();
                            }

                            if (forMe->value1 > 0)
                            {
                                forMe->value3 = forMe->value2 * 10;
                                userMessageList->Append(forMe);
                            }
                            else
                                delete forMe;
                        }
                    }
                    else if (ServerRandomAvatar)
                    {
                        // Say hello to a new master!
                        UserMessage *forMe = new UserMessage(0, ServerRandomAvatar->name);
                        userMessageList->Append(forMe);
                        sprintf(forMe->password, ServerRandomAvatar->pass);
                        sprintf(forMe->message, "I am the spirit of the %s.", dm->name);

                        forMe = new UserMessage(0, ServerRandomAvatar->name);
                        userMessageList->Append(forMe);
                        sprintf(forMe->password, ServerRandomAvatar->pass);
                        sprintf(forMe->message, "You are my mistress now.");

                        forMe = new UserMessage(0, ServerRandomAvatar->name);
                        userMessageList->Append(forMe);
                        sprintf(forMe->password, ServerRandomAvatar->pass);
                        sprintf(forMe->message, "Come to %dN %dE and shape my halls.", 
                                 256-dm->enterY, 256-dm->enterX);

                        sprintf(dm->masterName, ServerRandomAvatar->name);
                        sprintf(dm->masterPass, ServerRandomAvatar->pass);
                        dm->masterTimeout.SetToNow();
                        dm->masterTimeout.AddMinutes(60*24*14);

                        ServerRandomAvatar = NULL;
                    }
            
                }
                sp = (SharedSpace *) spaceList->Next();
            }
            */

        }
    }

}


//***************************************************************
void BBOServer::HandleMessages(void)
{
    char messData[4000], tempText[1024];
    int  dataSize;
    FILE *source = NULL;
    int i,j, tempX, tempY;

    InventoryObject *iObject;
    InvBlade *iBlade;
    InvTotem *iTotem;
    InvIngot *iIngot;
    Inventory *curInventory;

    MessPlayerChatLine chatMess;
    MessInfoText infoText;
    MessGeneralYes messYes;
    MessGeneralNo	messNo;
    MessAvatarMove AvMove;
    MessAccept *messAcceptPtr;

//	MessAvatarDisappear aDisappear;
    MessAvatarStats messAvatarStats;
    MessAvatarNames messAvatarNames;

    MessInventoryInfo inventoryInfo;
    MessWield messWield;
    MessUnWield messUnWield;
    MessBladeDesc messBladeDesc;
    MessMobDisappear messMobDisappear;
    MessDungeonInfo messDInfo;
    MessDungeonChange messDungeonChange;
    MessTestPing messTP;
    MessExtendedInfoRequest infoReq;

    MessEnterGame *enterGamePtr;
    MessPlayerNew *mpNewPtr;
    MessPlayerReturning *mpReturningPtr;
    MessAvatarMoveRequest *AvMoveReqPtr;
    MessAvatarRequestStats *requestStatsPtr;
    MessAvatarStats *messAvatarStatsPtr;
    MessBugReport *bugReportPtr;
    MessInventoryRequestInfo *invRequestInfoPtr;
    MessInventoryTransferRequest *transferRequestPtr;
    MessWield *messWieldPtr;
    MessUnWield *messUnWieldPtr;
    MessAvatarAttack *messAAPtr;
    MessAvatarDelete *messDeletePtr;
    MessInventoryChange *inventoryChangePtr;
    MessTryCombine *messTryCombinePtr;
    MessRequestDungeonInfo *messRDInfoPtr;
    MessDungeonChange *messDungeonChangePtr;
    MessRequestAvatarInfo *messRequestAvatarInfo;
    MessRequestTownMageService *messTMSPtr;
    MessAvatarSendMoney *messMoney;
    MessChestInfo *messChestPtr;
    MessTalkToTree *messTreePtr;
    MessFeedPetRequest *messFPRPtr;
    MessPetName *messPNPtr;
    MessAdminMessage *adminMessPtr;
    MessInfoFlags *infoFlagsPtr;
    MessSellAll *sellAllPtr;
    MessSecureTrade *secureTradePtr;
    MessExtendedInfoRequest *requestExInfoPtr;
    MessSetBomb *setBombPtr;
    MessKeyCode *keyCodePtr;
    MessAvatarNewClothes *clothesPtr;
    MessChatChannel *chatChannelPtr;

    SharedSpace *ss;
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    BBOSNpc *curNpc;

    DoublyLinkedList *list = NULL;

    std::vector<TagID> tempReceiptList;
    int					fromSocket = 0;

    lserver->GetNextMsg(NULL, dataSize);
    
    while (dataSize > 0)
    {
        if (dataSize > 4000)
            dataSize = 4000;
        
        lserver->GetNextMsg(messData, dataSize, &fromSocket, &tempReceiptList);
        
        MessEmpty *empty = (MessEmpty *)messData;
        switch (messData[0])
        {
        case NWMESS_ACCEPT:
            messAcceptPtr = (MessAccept *) messData;

            if (banList->IsBanned(messAcceptPtr->IP))
            {
                messNo.subType = 10;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
            }
            else
            {
                // get rid of any other avatar with this id
                curAvatar = FindAvatar(fromSocket, &ss);
                while (curAvatar)
                {
                    ss->avatars->Remove(curAvatar);
                    delete curAvatar;
                    curAvatar = FindAvatar(fromSocket, &ss);
                }

                curAvatar = FindIncomingAvatar(fromSocket);
                while (curAvatar)
                {
                    incoming->Remove(curAvatar);
                    delete curAvatar;
                    curAvatar = FindIncomingAvatar(fromSocket);
                }

                curAvatar = new BBOSAvatar();
                curAvatar->socketIndex = fromSocket;
                curAvatar->curCharacterIndex = -1;
                curAvatar->IP[0] = messAcceptPtr->IP[0];
                curAvatar->IP[1] = messAcceptPtr->IP[1];
                curAvatar->IP[2] = messAcceptPtr->IP[2];
                curAvatar->IP[3] = messAcceptPtr->IP[3];

                incoming->Append(curAvatar);

                tempReceiptList.clear();
                tempReceiptList.push_back(fromSocket);
                infoReq.listType = 0;
                lserver->SendMsg(sizeof(infoReq),(void *)&infoReq, 0, &tempReceiptList);
            }

           break;

        case NWMESS_CLOSE:
            // remove any avatars that are associated with his message
            j = 0;
            curAvatar = FindAvatar(fromSocket, &ss);
            while (curAvatar)
            {
                j = 1;
                sprintf(tempText,"** closing connection: %s, %s ",curAvatar->name,
                          curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                LogOutput("gamelog.txt", tempText);

                LongTime lt;
                lt.value.wHour += 19;
                if (lt.value.wHour < 24)
                {
                    lt.value.wDay -= 1;
                }
                else
                    lt.value.wHour -= 24;


                sprintf(tempText,"%d/%02d, %d:%02d\n", (int)lt.value.wMonth, (int)lt.value.wDay, 
                          (int)lt.value.wHour, (int)lt.value.wMinute);
                LogOutput("gamelog.txt", tempText);

                if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                {
                    LogOutput("moderatorLog.txt", tempText);
                }

                curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_NOTHING);

                sprintf(infoText.text,"%s left the game.",
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                    sizeof(infoText),(void *)&infoText);

                curAvatar->QuestSpaceChange(ss,NULL);

                ss->avatars->Remove(curAvatar);
                if (SPACE_GROUND != ss->WhatAmI())
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX = 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnX;
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY = 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnY;
                }
                else
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX = 
                        curAvatar->cellX;
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY = 
                        curAvatar->cellY;
                }
                curAvatar->SaveAccount();
                delete curAvatar;
                curAvatar = FindAvatar(fromSocket, &ss);
            }

            curAvatar = FindIncomingAvatar(fromSocket);
            while (curAvatar)
            {
                j = 1;
                incoming->Remove(curAvatar);
                delete curAvatar;
                curAvatar = FindIncomingAvatar(fromSocket);
            }

            if (!j)
            {
                sprintf(tempText,"****** orphan close message, ");
                LogOutput("gamelog.txt", tempText);

                LongTime lt;
                lt.value.wHour += 19;
                if (lt.value.wHour < 24)
                {
                    lt.value.wDay -= 1;
                }
                else
                    lt.value.wHour -= 24;

                sprintf(tempText,"%d/%02d, %d:%02d\n", (int)lt.value.wMonth, (int)lt.value.wDay, 
                          (int)lt.value.wHour, (int)lt.value.wMinute);
                LogOutput("gamelog.txt", tempText);
            }

           break;

        case NWMESS_PLAYER_CHAT_LINE:
            GuaranteeTermination((char *) &(messData[1]), dataSize-1);
//			CleanString((char *) &(messData[1]), 0);
            HandleChatLine(fromSocket, &(messData[1]));
           break;

        case NWMESS_PLAYER_NEW:
            mpNewPtr = (MessPlayerNew *) messData;
            
            codePad = cryptoText1;
            UnCryptoString(mpNewPtr->name);

            GuaranteeTermination(mpNewPtr->name, NUM_OF_CHARS_FOR_USERNAME);
            CorrectString(mpNewPtr->name);
            GuaranteeTermination(mpNewPtr->pass, NUM_OF_CHARS_FOR_PASSWORD);
            CorrectString(mpNewPtr->pass);

            if (uidBanList->IsBanned(mpNewPtr->uniqueId))
            {
                messNo.subType = 12;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                break;
            }

            curAvatar = FindAvatar(mpNewPtr->name, mpNewPtr->pass, &ss);
            if (curAvatar)
            {
                messNo.subType = 6;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                break;
            }

            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                j = curAvatar->LoadAccount(mpNewPtr->name, mpNewPtr->pass, TRUE);
                curAvatar->SetUniqueId(mpNewPtr->uniqueId);

                tempReceiptList.clear();
                tempReceiptList.push_back(fromSocket);
                if (0 == j)
                {
                    messYes.subType = fromSocket;
                    lserver->SendMsg(sizeof(messYes),(void *)&messYes, 0, &tempReceiptList);
//					TransferAvatar(TRUE,fromSocket);
                }
                if (1 == j)
                {
                    messNo.subType = 1;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }
                if (2 == j)
                {
                    messNo.subType = 2;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }
                if (3 == j)
                {
                    messNo.subType = 3;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }
                if (-1 == j)
                {
                    messNo.subType = -1;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }
            }
           break;

        case NWMESS_PLAYER_RETURNING:
            mpReturningPtr = (MessPlayerReturning *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);
            codePad = cryptoText1;
            UnCryptoString(mpReturningPtr->name);
            
            GuaranteeTermination(mpReturningPtr->name, NUM_OF_CHARS_FOR_USERNAME);
            CorrectString(mpReturningPtr->name);
            GuaranteeTermination(mpReturningPtr->pass, NUM_OF_CHARS_FOR_PASSWORD);
            CorrectString(mpReturningPtr->pass);

            if (uidBanList->IsBanned(mpReturningPtr->uniqueId))
            {
                messNo.subType = 12;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                break;
            }

            if (mpReturningPtr->version != VERSION_NUMBER)
            {
                messNo.subType = 5;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                break;
            }
            curAvatar = FindAvatar(mpReturningPtr->name, mpReturningPtr->pass, &ss);
            if (curAvatar)
            {
                messNo.subType = 6;
                lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                break;
            }
            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                int cont = TRUE;
                j = curAvatar->LoadAccount(mpReturningPtr->name, mpReturningPtr->pass, FALSE);
                curAvatar->SetUniqueId(mpReturningPtr->uniqueId);

                if (1 == curAvatar->restrictionType) // if tempBanned
                {
                    LongTime now;
                    if (now.MinutesDifference(&curAvatar->accountRestrictionTime) <= 0)
                    {		
                        curAvatar->restrictionType = 0;  // you're free!
                    }
                    else
                    {
                        messNo.subType = 7;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                        cont = FALSE;
                    }
                }
                if (cont)
                {
                    if (0 == j)
                    {
                        messYes.subType = fromSocket;
                        lserver->SendMsg(sizeof(messYes),(void *)&messYes, 0, &tempReceiptList);
    //					TransferAvatar(TRUE,fromSocket);
                    }
                    if (1 == j)
                    {
                        messNo.subType = 1;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if (2 == j)
                    {
                        messNo.subType = 2;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if (3 == j)
                    {
                        messNo.subType = 3;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if (4 == j)
                    {
                        messNo.subType = 4;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if (-1 == j)
                    {
                        messNo.subType = -1;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if (10 == j)
                    {
                        messNo.subType = 10;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    if ( 11 == j )
                    {
                        messNo.subType = 11;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                }
            }
           break;

        case NWMESS_AVATAR_MOVE_REQUEST:
            AvMoveReqPtr = (MessAvatarMoveRequest *) messData;
            if (AvMoveReqPtr->avatarID != fromSocket)
                break;

            TryToMoveAvatar(fromSocket, AvMoveReqPtr);
                                 /*

            switch(AvMoveReqPtr->x)
            {
            case 0:
                tempX = 0;
                tempY = -1;
                break;
            case 1:
                tempX = 1;
                tempY = 0;
                break;
            case 2:
                tempX = 0;
                tempY = 1;
                break;
            case 3:
                tempX = -1;
                tempY = 0;
                break;
            }
            
            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                // if not going out of bounds...
                if (tempX + curAvatar->cellX < MAP_SIZE_WIDTH &&
                    tempX + curAvatar->cellX >= 0 &&
                    tempY + curAvatar->cellY < MAP_SIZE_HEIGHT &&
                    tempY + curAvatar->cellY >= 0)
                {
                    if (!curAvatar->isMoving && 
                          ss->CanMove(curAvatar->cellX,         curAvatar->cellY, 
                                      tempX + curAvatar->cellX, tempY + curAvatar->cellY))
                    {
                        // stepping into a dungeon?
                        SharedSpace *sp = (SharedSpace *) spaceList->First();
                        while (sp)
                        {
                            if (sp->wha
                            curMob = (BBOSMob *) sp->avatars->First();
                            while (curMob)
                            {
                                if (SMOB_AVATAR == curMob->WhatAmI())
                                {
                                    curAvatar = (BBOSAvatar *) curMob;
                                    if (curAvatar->socketIndex == id)
                                    {
                                        return curAvatar;
                                    }
                                }
                                curMob = (BBOSMob *) sp->avatars->Next();
                            }

                            sp = (SharedSpace *) spaceList->Next();
                        }


                        curAvatar->isMoving = TRUE;
                        curAvatar->targetCellX = tempX + curAvatar->cellX;
                        curAvatar->targetCellY = tempY + curAvatar->cellY;
                        curAvatar->moveStartTime = timeGetTime();
                        AvMove.avatarID = fromSocket;
                        AvMove.x = curAvatar->cellX;
                        AvMove.y = curAvatar->cellY;
                        AvMove.targetX = curAvatar->targetCellX;
                        AvMove.targetY = curAvatar->targetCellY;
                        lserver->SendMsg(sizeof(AvMove),(void *)&AvMove);
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;
                    }

                }
            }
                                    */
            break;

        case NWMESS_AVATAR_REQUEST_STATS:
            requestStatsPtr = (MessAvatarRequestStats *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                requestStatsPtr->characterIndex = requestStatsPtr->characterIndex % NUM_OF_CHARS_PER_USER;
                if (curAvatar->charInfoArray[requestStatsPtr->characterIndex].topIndex > -1)
                {
                    messAvatarStats.avatarID = requestStatsPtr->characterIndex;

                    messAvatarStats.faceIndex = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].faceIndex;
                    messAvatarStats.hairR = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].hairR;
                    messAvatarStats.hairG = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].hairG;
                    messAvatarStats.hairB = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].hairB;

                    messAvatarStats.topIndex = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].topIndex;
                    messAvatarStats.topR = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].topR;
                    messAvatarStats.topG = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].topG;
                    messAvatarStats.topB = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].topB;

                    messAvatarStats.bottomIndex = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].bottomIndex;
                    messAvatarStats.bottomR = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].bottomR;
                    messAvatarStats.bottomG = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].bottomG;
                    messAvatarStats.bottomB = 
                          curAvatar->charInfoArray[requestStatsPtr->characterIndex].bottomB;

                    memcpy(messAvatarStats.name,
                             curAvatar->charInfoArray[requestStatsPtr->characterIndex].name,31);
                    messAvatarStats.name[31] = 0;

                    messAvatarStats.physical =
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].physical;
                    messAvatarStats.magical =		
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].magical;
                    messAvatarStats.creative =		
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].creative;

                    messAvatarStats.imageFlags =		
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].imageFlags;

                    messAvatarStats.cLevel =		
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].cLevel;
                    messAvatarStats.cash =		
                        curAvatar->charInfoArray[requestStatsPtr->characterIndex].inventory->money;

                    lserver->SendMsg(sizeof(messAvatarStats),
                                      (void *)&messAvatarStats, 0, &tempReceiptList);
                }
                else
                {
                    // there's no valid avatar in that slot
                    messNo.subType = 1;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }

                // also send the names of the eight avatars
                for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
                {
                    if (curAvatar->charInfoArray[i].topIndex > -1)
                    {
                        memcpy(messAvatarNames.name[i],
                                 curAvatar->charInfoArray[i].name,31);
                        messAvatarNames.name[i][31] = 0;
                    }
                    else
                        messAvatarNames.name[i][0] = 0;
                }
                lserver->SendMsg(sizeof(messAvatarNames),(void *)&messAvatarNames, 0, &tempReceiptList);

                // also send the account time left
                MessAccountTimeInfo timeInfo;
                timeInfo.wYear      = curAvatar->accountExperationTime.value.wYear     ;
                timeInfo.wMonth     = curAvatar->accountExperationTime.value.wMonth    ;
                timeInfo.wDay       = curAvatar->accountExperationTime.value.wDay      ;
                timeInfo.wDayOfWeek = curAvatar->accountExperationTime.value.wDayOfWeek;
                timeInfo.wHour      = curAvatar->accountExperationTime.value.wHour     ;
                timeInfo.wMinute    = curAvatar->accountExperationTime.value.wMinute   ;

                lserver->SendMsg(sizeof(timeInfo),(void *)&timeInfo, 0, &tempReceiptList);

            }
           break;

        case NWMESS_AVATAR_STATS:
            messAvatarStatsPtr = (MessAvatarStats *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            messAvatarStatsPtr->avatarID = messAvatarStatsPtr->avatarID % NUM_OF_CHARS_PER_USER;
            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                if (messAvatarStatsPtr->avatarID < 0 || messAvatarStatsPtr->avatarID > 7)
                    messAvatarStatsPtr->avatarID = 0;

                GuaranteeTermination(messAvatarStatsPtr->name,32);
                CorrectString(messAvatarStatsPtr->name);
                CleanString(messAvatarStatsPtr->name);
                RemoveStringTrailingSpaces(messAvatarStatsPtr->name);

                if (-1 == curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].topIndex)
                {
                    if (!UN_IsNameUnique(messAvatarStatsPtr->name))
                    {
                        // there's already an avatar of that name
                        messNo.subType = 7;
                        lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                    }
                    else
                    {
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].faceIndex =
                                messAvatarStatsPtr->faceIndex;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].hairR =
                                messAvatarStatsPtr->hairR;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].hairG =
                                messAvatarStatsPtr->hairG;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].hairB =
                                messAvatarStatsPtr->hairB;		

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].topIndex =
                                messAvatarStatsPtr->topIndex;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].topR =
                                messAvatarStatsPtr->topR;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].topG =
                                messAvatarStatsPtr->topG;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].topB =
                                messAvatarStatsPtr->topB;		

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].bottomIndex =
                                messAvatarStatsPtr->bottomIndex;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].bottomR =
                                messAvatarStatsPtr->bottomR;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].bottomG =
                                messAvatarStatsPtr->bottomG;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].bottomB =
                                messAvatarStatsPtr->bottomB;		

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].physical =
                                messAvatarStatsPtr->physical;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].magical =
                                messAvatarStatsPtr->magical;		
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].creative =
                                messAvatarStatsPtr->creative;		

                        // major reality check!
                        curAvatar->MakeCharacterValid(messAvatarStatsPtr->avatarID);

                        messAvatarStatsPtr->physical =
                            curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].physical;
                        messAvatarStatsPtr->magical =		
                            curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].magical;
                        messAvatarStatsPtr->creative =		
                            curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].creative;

                        memcpy(curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].name,
                                 messAvatarStatsPtr->name,31);
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].name[31] = 0;

                        UN_AddName(messAvatarStatsPtr->name);

                        messAvatarStatsPtr->imageFlags =	0;
                        messAvatarStatsPtr->cash       = 0;
                        messAvatarStatsPtr->cLevel     = 0;

                        lserver->SendMsg(sizeof(MessAvatarStats),
                                              (void *)messAvatarStatsPtr, 0, &tempReceiptList);

                        curInventory = curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].inventory;
                        iObject = (InventoryObject *)curInventory->objects.First();
                        while (iObject)
                        {
                            curInventory->objects.Remove(iObject);
                            delete iObject;
                            iObject = (InventoryObject *)curInventory->objects.First();
                        }

                        curInventory = curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].workbench;
                        iObject = (InventoryObject *)curInventory->objects.First();
                        while (iObject)
                        {
                            curInventory->objects.Remove(iObject);
                            delete iObject;
                            iObject = (InventoryObject *)curInventory->objects.First();
                        }

                        curInventory = curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].skills;
                        iObject = (InventoryObject *)curInventory->objects.First();
                        while (iObject)
                        {
                            curInventory->objects.Remove(iObject);
                            delete iObject;
                            iObject = (InventoryObject *)curInventory->objects.First();
                        }

                        curInventory = curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].wield;
                        iObject = (InventoryObject *)curInventory->objects.First();
                        while (iObject)
                        {
                            curInventory->objects.Remove(iObject);
                            delete iObject;
                            iObject = (InventoryObject *)curInventory->objects.First();
                        }

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].petDragonInfo[0].type      = 255; // no dragon
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].petDragonInfo[1].type      = 255; // no dragon
                        sprintf(curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].petDragonInfo[0].name,"NO PET");
                        sprintf(curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].petDragonInfo[1].name,"NO PET");

                        InventoryObject *iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
                        iTotem = (InvTotem *)iObject->extra;
                        iTotem->type     = 0;
                        iTotem->quality  = 0;

                        iObject->mass = 1.0f;
                        iObject->value = 1;
                        iObject->amount = 2;
                        UpdateTotem(iObject);
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].inventory->objects.Append(iObject);

                        iObject = new InventoryObject(INVOBJ_INGOT,0,"Tin Ingot");
                        iIngot = (InvIngot *)iObject->extra;
                        iIngot->damageVal = 1;
                        iIngot->challenge = 1;
                        iIngot->r = iIngot->b = iIngot->g = 128;

                        iObject->mass = 0.0f;
                        iObject->value = 1;
                        iObject->amount = 4;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].inventory->objects.Append(iObject);


                        iObject = new InventoryObject(INVOBJ_BLADE,0,"Beginner Blade");
                        iBlade = (InvBlade *)iObject->extra;
                        iBlade->damageDone = 3;
                        iBlade->size = 1.3f;
                        iBlade->toHit = 1;
                        iBlade->r = iBlade->b = iBlade->g = 128;

                        iObject->mass = 2.0f;
                        iObject->value = 1;
                        iObject->amount = 1;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].wield->objects.Append(iObject);

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].inventory->money = 100;
//						curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].inventory->moneyDelta = 0;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].health     = 30;
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].healthMax  = 30;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].lifeTime   = 0;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].imageFlags = 0;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].age = messAvatarStatsPtr->age = 1;
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].cLevel = messAvatarStatsPtr->cLevel = 0;
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].oldCLevel = 0;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].lastX = 
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].spawnX = 
                             townList[2].x;

                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].lastY = 
                        curAvatar->charInfoArray[messAvatarStatsPtr->avatarID].spawnY = 
                              townList[2].y;

    //					curAvatar->lastX = curAvatar->cellX;
    //					curAvatar->lastY = curAvatar->cellY;
                        curAvatar->SaveAccount();

                        // also send the names of the eight avatars
                        for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
                        {
                            if (curAvatar->charInfoArray[i].topIndex > -1)
                            {
                                memcpy(messAvatarNames.name[i],
                                         curAvatar->charInfoArray[i].name,31);
                                messAvatarNames.name[i][31] = 0;
                            }
                            else
                                messAvatarNames.name[i][0] = 0;
                        }
                        lserver->SendMsg(sizeof(messAvatarNames),(void *)&messAvatarNames, 0, &tempReceiptList);
                    }
                }
                else
                {
                    // there's already an avatar in that slot
                    messNo.subType = 2;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }
            }
           break;

        case NWMESS_AVATAR_DELETE:
            messDeletePtr = (MessAvatarDelete *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                messDeletePtr->characterIndex = messDeletePtr->characterIndex % NUM_OF_CHARS_PER_USER;
                if (curAvatar->charInfoArray[messDeletePtr->characterIndex].topIndex > -1)
                {
                    UN_RemoveName(curAvatar->charInfoArray[messDeletePtr->characterIndex].name);

                    SharedSpace *guildSpace;
                    if (FindAvatarInGuild(
                                  curAvatar->charInfoArray[messDeletePtr->characterIndex].name, 
                                  &guildSpace))
                    {
                        DeleteNameFromGuild(
                              curAvatar->charInfoArray[messDeletePtr->characterIndex].name, 
                              &guildSpace);
                    }

                    curAvatar->charInfoArray[messDeletePtr->characterIndex].topIndex = -1;
                    /*
                    */
                    curAvatar->SaveAccount();
                    // there's no valid avatar in that slot
                    messNo.subType = 1;
                    lserver->SendMsg(sizeof(messNo),(void *)&messNo, 0, &tempReceiptList);
                }

                // also send the names of the eight avatars
                for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
                {
                    if (curAvatar->charInfoArray[i].topIndex > -1)
                    {
                        memcpy(messAvatarNames.name[i],
                                 curAvatar->charInfoArray[i].name,31);
                        messAvatarNames.name[i][31] = 0;
                    }
                    else
                        messAvatarNames.name[i][0] = 0;
                }
                lserver->SendMsg(sizeof(messAvatarNames),(void *)&messAvatarNames, 0, &tempReceiptList);
            }
           break;

        case NWMESS_AVATAR_NEW_CLOTHES:
            clothesPtr = (MessAvatarNewClothes *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            clothesPtr->avatarID = clothesPtr->avatarID % NUM_OF_CHARS_PER_USER;
            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                if (clothesPtr->avatarID < 0 || clothesPtr->avatarID > 7)
                    clothesPtr->avatarID = 0;

                if (-1 != curAvatar->charInfoArray[clothesPtr->avatarID].topIndex)
                {
                    curAvatar->charInfoArray[clothesPtr->avatarID].topIndex =
                            clothesPtr->topIndex;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].topR =
                            clothesPtr->topR;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].topG =
                            clothesPtr->topG;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].topB =
                            clothesPtr->topB;		

                    curAvatar->charInfoArray[clothesPtr->avatarID].bottomIndex =
                            clothesPtr->bottomIndex;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].bottomR =
                            clothesPtr->bottomR;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].bottomG =
                            clothesPtr->bottomG;		
                    curAvatar->charInfoArray[clothesPtr->avatarID].bottomB =
                            clothesPtr->bottomB;		



                    messAvatarStats.avatarID = clothesPtr->avatarID;

                    messAvatarStats.faceIndex = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].faceIndex;
                    messAvatarStats.hairR = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].hairR;
                    messAvatarStats.hairG = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].hairG;
                    messAvatarStats.hairB = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].hairB;

                    messAvatarStats.topIndex = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].topIndex;
                    messAvatarStats.topR = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].topR;
                    messAvatarStats.topG = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].topG;
                    messAvatarStats.topB = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].topB;

                    messAvatarStats.bottomIndex = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].bottomIndex;
                    messAvatarStats.bottomR = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].bottomR;
                    messAvatarStats.bottomG = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].bottomG;
                    messAvatarStats.bottomB = 
                          curAvatar->charInfoArray[clothesPtr->avatarID].bottomB;

                    memcpy(messAvatarStats.name,
                             curAvatar->charInfoArray[clothesPtr->avatarID].name,31);
                    messAvatarStats.name[31] = 0;

                    messAvatarStats.physical =
                        curAvatar->charInfoArray[clothesPtr->avatarID].physical;
                    messAvatarStats.magical =		
                        curAvatar->charInfoArray[clothesPtr->avatarID].magical;
                    messAvatarStats.creative =		
                        curAvatar->charInfoArray[clothesPtr->avatarID].creative;

                    messAvatarStats.imageFlags =		
                        curAvatar->charInfoArray[clothesPtr->avatarID].imageFlags;

                    lserver->SendMsg(sizeof(messAvatarStats),
                                      (void *)&messAvatarStats, 0, &tempReceiptList);

                    curAvatar->SaveAccount();

                    // also send the names of the eight avatars
                    for (i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
                    {
                        if (curAvatar->charInfoArray[i].topIndex > -1)
                        {
                            memcpy(messAvatarNames.name[i],
                                     curAvatar->charInfoArray[i].name,31);
                            messAvatarNames.name[i][31] = 0;
                        }
                        else
                            messAvatarNames.name[i][0] = 0;
                    }
                    lserver->SendMsg(sizeof(messAvatarNames),(void *)&messAvatarNames, 0, &tempReceiptList);
                }
            }
           break;

        case NWMESS_BUG_REPORT:
            bugReportPtr = (MessBugReport *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            source = fopen("bugs.txt","a");

            
           /* Display operating system-style date and time. */
           _strdate( tempText );
           fprintf(source, "%s, ", tempText );
           _strtime( tempText );
           fprintf(source, "%s, ", tempText );

            if (bugReportPtr->art)
                fprintf(source,"A, ");
            else
                fprintf(source,"-, ");
            if (bugReportPtr->crash)
                fprintf(source,"C, ");
            else
                fprintf(source,"-, ");
            if (bugReportPtr->gameplay)
                fprintf(source,"G, ");
            else
                fprintf(source,"-, ");
            if (bugReportPtr->hang)
                fprintf(source,"H, ");
            else
                fprintf(source,"-, ");
            if (bugReportPtr->other)
                fprintf(source,"O, ");
            else
                fprintf(source,"-, ");

            fprintf(source,"%s, ", bugReportPtr->doing);
            fprintf(source,"%s, ", bugReportPtr->playLength);
            fprintf(source,"%s, ", bugReportPtr->repeatable);
            fprintf(source,"%s,\n", bugReportPtr->info);

            fclose(source);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                sprintf(&chatMess.text[1],"Thanks for your bug report, %s!",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                curAvatar->activeCounter = 0;
            }
           break;

        case NWMESS_ENTER_GAME:
            enterGamePtr = (MessEnterGame *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            enterGamePtr->characterIndex = enterGamePtr->characterIndex % NUM_OF_CHARS_PER_USER;
            curAvatar = FindIncomingAvatar(fromSocket);
            if (curAvatar)
            {
                if (-1 != curAvatar->charInfoArray[enterGamePtr->characterIndex].topIndex)
                {
                    messYes.subType = fromSocket;
                    lserver->SendMsg(sizeof(messYes),(void *)&messYes, 0, &tempReceiptList);
                    curAvatar->curCharacterIndex = enterGamePtr->characterIndex;
                    curAvatar->cellX = curAvatar->targetCellX = 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX;
                    curAvatar->cellY = curAvatar->targetCellY = 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY;

                    curAvatar->QuestSpaceChange(NULL, NULL);
                    TransferAvatar(TRUE, fromSocket);
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                                 petDragonInfo[0].lastEatenTime.SetToNow();
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                                 petDragonInfo[1].lastEatenTime.SetToNow();

                    // OFF-LINE HEAL
                    LongTime now;
                    DWORD timeElapsed = 
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                              lastSavedTime.MinutesDifference(&now);
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].health += 
                        timeElapsed * 6;

                    if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health >
                           curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax)
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].health =
                               curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;

                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;
                    sprintf(tempText,"**** new connection from IP %d.%d.%d.%d: %s, %s ",
                        curAvatar->IP[0],		
                        curAvatar->IP[1],
                        curAvatar->IP[2],
                        curAvatar->IP[3],
                        curAvatar->name,
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                    LogOutput("gamelog.txt", tempText);

                    if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                    {
                        LogOutput("moderatorLog.txt", tempText);
                    }

                    LongTime lt;
                    lt.value.wHour += 19;
                    if (lt.value.wHour < 24)
                        lt.value.wDay -= 1;
                    else
                        lt.value.wHour -= 24;

                    sprintf(tempText,"%d/%02d, %d:%02d\n", lt.value.wMonth, lt.value.wDay, 
                              lt.value.wHour, lt.value.wMinute);
                    LogOutput("gamelog.txt", tempText);
                    if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                    {
                        LogOutput("moderatorLog.txt", tempText);
                    }

                    lastConnectTime   = timeGetTime();
                    errorContactVal = 0;

//				  	sprintf(infoText.text,"New avatar arriving.\n");
                    ss = (SharedSpace *) spaceList->Find(SPACE_GROUND); // 'cause avatars appear
                                                                        // on land.

                    if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime > 1)
                    {
                        sprintf(infoText.text,"%s entered the game.",
                             curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(infoText),(void *)&infoText);
                    }
                    else
                    {
                        sprintf(infoText.text,"***  %s begins life!  ***",
                             curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(infoText),(void *)&infoText);
                    }

                    curAvatar->activeCounter = 0;

                    TellBuddiesImHere(curAvatar);

                    MessInfoFlags infoFlags;
                    infoFlags.flags = curAvatar->infoFlags;
                    lserver->SendMsg(sizeof(infoFlags),(void *)&infoFlags, 0, &tempReceiptList);

                    if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                    {
                        MessAdminMessage adminMess;
                        adminMess.messageType = MESS_ADMIN_ACTIVATE;
                        lserver->SendMsg(sizeof(adminMess),(void *)&adminMess, 0, &tempReceiptList);
                    }

                    MessChatChannel returnCChan;
                    returnCChan.value = curAvatar->chatChannels;
                    lserver->SendMsg(sizeof(returnCChan),(void *)&returnCChan, 0, &tempReceiptList);

                    MessWeatherState state;
                    state.value = weatherState;
                    lserver->SendMsg(sizeof(state),(void *)&state, 0, &tempReceiptList);

                    if (!curAvatar->isReferralDone && curAvatar->hasPaid)
                    {
                        sprintf(infoText.text,"Thanks for paying! Remember to use the /referredby <name>");
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        sprintf(infoText.text,"command to reward the player who introduced you to Blade Mistress.");
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }

                }

            }
           break;

        case NWMESS_EXIT_GAME:
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                TransferAvatar(FALSE, fromSocket);

                sprintf(tempText,"** log-off character: %s, %s ",curAvatar->name,
                          curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                LogOutput("gamelog.txt", tempText);
                if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                {
                    LogOutput("moderatorLog.txt", tempText);
                }


                LongTime lt;
                lt.value.wHour += 19;
                if (lt.value.wHour < 24)
                {
                    lt.value.wDay -= 1;
                }
                else
                    lt.value.wHour -= 24;


                sprintf(tempText,"%d/%02d, %d:%02d\n", (int)lt.value.wMonth, (int)lt.value.wDay, 
                          (int)lt.value.wHour, (int)lt.value.wMinute);
                LogOutput("gamelog.txt", tempText);
                if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                {
                    LogOutput("moderatorLog.txt", tempText);
                }

                curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_NOTHING);

                sprintf(infoText.text,"%s left the game.",
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                    sizeof(infoText),(void *)&infoText);

                curAvatar->QuestSpaceChange(ss,NULL);

                if (SPACE_GROUND != ss->WhatAmI())
                {
                    if (SPACE_GUILD == ss->WhatAmI())
                    {
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX = 
                            ((TowerMap *)ss)->enterX;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY = 
                            ((TowerMap *)ss)->enterY-1;
                    }
                    else
                    {
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX = 
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnX;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY = 
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnY;
                    }
                }
                else
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastX = 
                        curAvatar->cellX;
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].lastY = 
                        curAvatar->cellY;
                }
                curAvatar->SaveAccount();

            }
           break;

        case NWMESS_INVENTORY_REQUEST_INFO:
            invRequestInfoPtr = (MessInventoryRequestInfo *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex)
                {
                    // tell the client what it wants to know about an inventory
                    if (MESS_INVENTORY_SAME == invRequestInfoPtr->type &&
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner &&
                         MESS_INVENTORY_TRADER == invRequestInfoPtr->which)
                    {
                        curAvatar->indexList[MESS_INVENTORY_TRADER] = invRequestInfoPtr->offset;
                        TellClientAboutInventory(curAvatar, 
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                             inventory->partner->subType);
                    }
                    else if (MESS_INVENTORY_SAME == invRequestInfoPtr->type)
                    {
                        curAvatar->indexList[curAvatar->lastPlayerInvSent] = invRequestInfoPtr->offset;
                        TellClientAboutInventory(curAvatar, invRequestInfoPtr->which);
                    }
                    else if (MESS_INVENTORY_PLAYER == invRequestInfoPtr->type)
                    {
                        TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                    }
                    else if (MESS_WORKBENCH_PLAYER == invRequestInfoPtr->type)
                    {
                        TellClientAboutInventory(curAvatar, MESS_WORKBENCH_PLAYER);
                    }
                    else if (MESS_SKILLS_PLAYER == invRequestInfoPtr->type)
                    {
                        TellClientAboutInventory(curAvatar, MESS_SKILLS_PLAYER);
                    }
                    else if (MESS_WIELD_PLAYER == invRequestInfoPtr->type)
                    {
                        TellClientAboutInventory(curAvatar, MESS_WIELD_PLAYER);
                    }
                    else if (MESS_INVENTORY_TOWER == invRequestInfoPtr->type)
                    {
                        SharedSpace *sx;

                        // Players now have to be in the guild AND paying to use the chest.
                        if( ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType )
                        {
                            sprintf(infoText.text,"Trial moderators cannot open any guild chests.");
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                        else if (FindAvatarInGuild(curAvatar->charInfoArray[
                                                    curAvatar->curCharacterIndex].name, &sx) &&
                                                        sx == ss)
                        {

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner =
                                (((TowerMap *) sx)->itemBox);
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving = TRUE;
                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_TOWER);
                        }
                        else
                        {
                            sprintf(infoText.text,"You are not allowed to open this chest. You must be in the guild and a paying player.");
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                    else if (MESS_INVENTORY_YOUR_SECURE == invRequestInfoPtr->type)
                    {
                        BBOSAvatar *partnerAvatar;
                        partnerAvatar = FindAvatar((int)invRequestInfoPtr->which, &ss);
                        if (MESS_INVENTORY_TRADER == invRequestInfoPtr->which)
                            partnerAvatar = curAvatar->tradingPartner;

                        // anti-duping check
                        if (partnerAvatar &&
                            ( strlen(partnerAvatar->name) < 1 ||
                             IsCompletelyVisiblySame(partnerAvatar->name, curAvatar->name) ))
                            partnerAvatar = NULL;

                        if (partnerAvatar)
                        {
                            LongTime now;

                            if (false && now.MinutesDifference(&curAvatar->accountExperationTime) <= 0) // no more trade expire restrictions
                            {
                                sprintf(infoText.text,"I'm sorry, but your account is expired.  You cannot trade.");
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else if (false && now.MinutesDifference(&partnerAvatar->accountExperationTime) <= 0) // no more trade expire restrictions
                            {
                                sprintf(infoText.text,"That person's account is expired.  They cannot trade.");
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else if (!curAvatar->tradingPartner && !partnerAvatar->tradingPartner)
                            {
                                curAvatar->tradingPartner     = partnerAvatar;
                                partnerAvatar->tradingPartner = curAvatar;
                                curAvatar->agreeToTrade = FALSE;
                                partnerAvatar->agreeToTrade = FALSE;

                                curAvatar->indexList[MESS_INVENTORY_YOUR_SECURE] = 0;
                                curAvatar->indexList[MESS_INVENTORY_HER_SECURE] = 0;
                                partnerAvatar->trade->money = 0;
                                curAvatar->trade->money = 0;

                                MessSecurePartnerName partnerNameMess;

                                sprintf(tempText,"You begin trading with %s.",
                                     partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name);
                                CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                sprintf(partnerNameMess.name,
                                    partnerAvatar->charInfoArray[partnerAvatar->curCharacterIndex].name); 
                                partnerNameMess.name[10] = 0;	 // truncate
                                ss->lserver->SendMsg(sizeof(partnerNameMess),(void *)&partnerNameMess, 0, &tempReceiptList);

                                tempReceiptList.clear();
                                tempReceiptList.push_back(partnerAvatar->socketIndex);
                                sprintf(tempText,"%s begins trading with you.",
                                     curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                                CopyStringSafely(tempText,1024,infoText.text, MESSINFOTEXTLEN);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                sprintf(partnerNameMess.name,
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name); 
                                partnerNameMess.name[10] = 0;	 // truncate
                                ss->lserver->SendMsg(sizeof(partnerNameMess),(void *)&partnerNameMess, 0, &tempReceiptList);

                                TellClientAboutInventory(curAvatar, MESS_INVENTORY_YOUR_SECURE);
                                TellClientAboutInventory(curAvatar, MESS_INVENTORY_HER_SECURE);

                                TellClientAboutInventory(partnerAvatar, MESS_INVENTORY_YOUR_SECURE);
                                TellClientAboutInventory(partnerAvatar, MESS_INVENTORY_HER_SECURE);


                            }
                            else
                            {
                                if (curAvatar->tradingPartner == partnerAvatar && 
                                     partnerAvatar->tradingPartner == curAvatar)
                                {
                                    curAvatar->indexList[MESS_INVENTORY_YOUR_SECURE] = 
                                        invRequestInfoPtr->offset;
                                    TellClientAboutInventory(curAvatar, MESS_INVENTORY_YOUR_SECURE);
                                }
                                else if (partnerAvatar->tradingPartner)
                                {
                                    sprintf(infoText.text,"That person is busy trading with someone else.");
                                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                }
                            }
                        }
                    }
                    else if (MESS_INVENTORY_HER_SECURE == invRequestInfoPtr->type)
                    {
                        BBOSAvatar *partnerAvatar;
                        partnerAvatar = FindAvatar((int)invRequestInfoPtr->which, &ss);
                        if (MESS_INVENTORY_TRADER == invRequestInfoPtr->which)
                            partnerAvatar = curAvatar->tradingPartner;
                        if (partnerAvatar)
                        {
                            if (curAvatar->tradingPartner == partnerAvatar && 
                                 partnerAvatar->tradingPartner == curAvatar)
                            {
                                curAvatar->indexList[MESS_INVENTORY_HER_SECURE] = 
                                        invRequestInfoPtr->offset;
                                    TellClientAboutInventory(curAvatar, MESS_INVENTORY_HER_SECURE);
                            }
                            else if (partnerAvatar->tradingPartner)
                            {
                                sprintf(infoText.text,"That person is busy trading with someone else.");
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                    else if (MESS_INVENTORY_TRADER == invRequestInfoPtr->type && 
                              invRequestInfoPtr->which)
                    {
                        curNpc = (BBOSNpc *) ss->mobList->IsInList((BBOSMob *)invRequestInfoPtr->which, TRUE);
                        if (curNpc &&  
                             curNpc->cellX == curAvatar->cellX &&
                             curNpc->cellY == curAvatar->cellY &&
                             (SMOB_TRADER == curNpc->WhatAmI() || SMOB_TRAINER == curNpc->WhatAmI())
                            )
                        {
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner =
                                (curNpc->inventory);
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving = FALSE;
//							curNpc->inventory.subType = curNpc->WhatAmI();

                            if (SMOB_TRADER == curNpc->WhatAmI())
                                TellClientAboutInventory(curAvatar, MESS_INVENTORY_TRADER);
                            else if (SMOB_TRAINER == curNpc->WhatAmI())
                                TellClientAboutInventory(curAvatar, MESS_INVENTORY_TRAINER);
                            /*
                            if (curNpc->isTrader)
                                sprintf(inventoryInfo.traderName, "Merchant");
                            else
                                sprintf(inventoryInfo.traderName, "Teacher");
                                */
                        }
                    }
                    else if (MESS_INVENTORY_GROUND == invRequestInfoPtr->type)
                    {
                        tempX = curAvatar->cellX;
                        tempY = curAvatar->cellY;
                        if (curAvatar->controlledMonster)
                        {
                            SharedSpace *sx;

                            BBOSMonster * theMonster = FindMonster(
                                      curAvatar->controlledMonster, &sx);
                            if (theMonster)
                            {
//								inv = &(theMonster->inventory);
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner =
                                    ss->GetGroundInventory(theMonster->cellX,theMonster->cellY);
                            }
                        }
                        else
                        {
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner =
                                ss->GetGroundInventory(tempX,tempY);
                        }

                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving = TRUE;
//						curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner->isGround = TRUE;

                        TellClientAboutInventory(curAvatar, MESS_INVENTORY_GROUND);
                    }
                }
            }
           break;

        case NWMESS_INVENTORY_TRANSFER_REQUEST:
            transferRequestPtr = (MessInventoryTransferRequest *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;

                if (curAvatar->tradingPartner == NULL &&
                    transferRequestPtr->isPlayerInfo &&
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner &&
                    (MESS_INVENTORY_GROUND == curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner->subType) &&
                    strlen(((InventoryObject*)transferRequestPtr->ptr)->do_name) > 1 &&
                    strncmp("Demon Amulet", ((InventoryObject*)transferRequestPtr->ptr)->do_name, strlen(((InventoryObject*)transferRequestPtr->ptr)->do_name)) &&
                    strncmp("Dragon Orchid", ((InventoryObject*)transferRequestPtr->ptr)->do_name, strlen(((InventoryObject*)transferRequestPtr->ptr)->do_name)) &&
                    strncmp("Ancient Dragonscale", ((InventoryObject*)transferRequestPtr->ptr)->do_name, strlen(((InventoryObject*)transferRequestPtr->ptr)->do_name))
                    )
                {
                    sprintf(infoText.text,"You can only put Amulets, Dragonscales and Orchids onto the ground.");
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex)
                {

                    int retVal = TransferItem(curAvatar, transferRequestPtr, transferRequestPtr->amount,
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving);
                    if (1 == retVal)
                    {
                        UpdateInventory(curAvatar);
                        TellClientAboutInventory(curAvatar, curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner->subType);

                        if (MESS_INVENTORY_GROUND == curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner->subType)
                        {
                            tempX = curAvatar->cellX;
                            tempY = curAvatar->cellY;
                            SharedSpace *sx = NULL;

                            if (curAvatar->controlledMonster)
                            {
                                BBOSMonster * theMonster = FindMonster(
                                          curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                {
                                    tempX = theMonster->cellX;
                                    tempY = theMonster->cellY;
                                }
                            }

                            if (sx)
                                ss = sx;

                            if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                      inventory->partner->objects.IsListEmpty())
                            {
                                messMobDisappear.mobID = (unsigned long)
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                              inventory->partner;
                                messMobDisappear.x = tempX;
                                messMobDisappear.y = tempY;
                                ss->SendToEveryoneNearBut(0, tempX, tempY,
                                    sizeof(messMobDisappear),(void *)&messMobDisappear);
                            }
                            else
                            {
                                // tell client about an item sack
                                MessMobAppear messMA;
                                messMA.mobID = (unsigned long) 
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                              inventory->partner;
                                messMA.type = SMOB_ITEM_SACK;
                                messMA.x = tempX;
                                messMA.y = tempY;
                                ss->SendToEveryoneNearBut(0, tempX, tempY,
                                                sizeof(messMA),(void *)&messMA);
                            }
                        }
                    }
                    else if (2 == retVal)
                    {
                        UpdateInventory(curAvatar);
                    }

                }
            }
           break;

        case NWMESS_INVENTORY_CHANGE:
            inventoryChangePtr = (MessInventoryChange *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex)
                {

                    if (ShiftItem(curAvatar, inventoryChangePtr))
                    {
                        switch(inventoryChangePtr->srcListType)
                        {
                        case GTM_BUTTON_LIST_INV:
                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                            break;
                        case GTM_BUTTON_LIST_WRK:
                            TellClientAboutInventory(curAvatar, MESS_WORKBENCH_PLAYER);
                            break;
                        case GTM_BUTTON_LIST_SKL:
                            TellClientAboutInventory(curAvatar, MESS_SKILLS_PLAYER);
                            break;
                        case GTM_BUTTON_LIST_WLD:
                            TellClientAboutInventory(curAvatar, MESS_WIELD_PLAYER);
                            break;
                        }
                    }
                }
            }
           break;

        case NWMESS_WIELD:
           break;

            messWieldPtr = (MessWield *) messData;
//			tempReceiptList.clear();
//			tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
                     messWieldPtr->bladeID)
                {
                    iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->objects.Find(
                                    (InventoryObject *) messWieldPtr->bladeID);
                    if (iObject && INVOBJ_BLADE == iObject->type) // if found the object
                    {
                        SetWield(TRUE, iObject,
                            (curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory));

                        FillBladeDescMessage(&messBladeDesc, iObject, curAvatar);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                            sizeof(messBladeDesc),(void *)&messBladeDesc);
                    }
                }
            }

           break;

        case NWMESS_UNWIELD:
           break;

            messUnWieldPtr = (MessUnWield *) messData;
//			tempReceiptList.clear();
//			tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
                     messUnWieldPtr->bladeID)
                {
                    iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->objects.Find(
                                    (InventoryObject *) messUnWieldPtr->bladeID);
                    if (iObject && INVOBJ_BLADE == iObject->type) // if found the object
                    {
                        SetWield(FALSE, iObject,
                            (curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory));

                        messUnWield.bladeID = (long)curAvatar->socketIndex;
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                            sizeof(messUnWield),(void *)&messUnWield);
                    }
                }
            }

           break;


        case NWMESS_AVATAR_ATTACK:
            messAAPtr = (MessAvatarAttack *) messData;
            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                if (curAvatar->controlledMonster)
                {
                    SharedSpace *sx, *sz;

                    BBOSMonster * theMonster = FindMonster(curAvatar->controlledMonster, &sx);
                    if	(theMonster)
                    {
                        // theMonster needs to attack a target avatar
                        curAvatar = FindAvatar(messAAPtr->mobID, &sx);
                        if (curAvatar)
                        {
                            theMonster->lastAttackTime = 0;
                            theMonster->curTarget = curAvatar;
                        }
                        else
                        {
                            BBOSMonster * targetMonster = 
                                FindMonster((BBOSMob *) messAAPtr->mobID, &sz);
                            if (targetMonster)
                            {
                                theMonster->lastAttackTime = 0;
                                theMonster->curMonsterTarget = targetMonster;
                            }
                        }
                    }
                }
                else if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
                     messAAPtr->mobID)
                {
                    curMob = (BBOSMob *) ss->mobList->IsInList((BBOSMob *)messAAPtr->mobID, TRUE);
                    if (curMob && 
                         curMob->cellX == curAvatar->cellX &&
                         curMob->cellY == curAvatar->cellY && SMOB_MONSTER == curMob->WhatAmI())
                    {
                        if (!curAvatar->curTarget)
                            curAvatar->lastAttackTime = 0;
                        curAvatar->curTarget = (BBOSMonster *)curMob;
                    }

                }
            }
           break;


        case NWMESS_TRY_COMBINE:
            messTryCombinePtr = (MessTryCombine *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                curAvatar->activeCounter = 0;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
                     messTryCombinePtr->skillID)
                {
                    iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].skills->objects.Find(
                                    (InventoryObject *) messTryCombinePtr->skillID);
                    if (iObject && INVOBJ_SKILL == iObject->type) // if found the object
                    {
                        HandleCombine(curAvatar, iObject->WhoAmI());
                    }
                    else
                    {
                        char *skillNamePtr = NULL;
                        // try to treat the value as a hotkey
                        for (i = 0; i < 19; ++i)
                        {
                            if (skillHotKeyArray[i] == messTryCombinePtr->skillID)
                            {
                                skillNamePtr = skillNameArray[i];
                            }
                        }

                        if (skillNamePtr)
                        {
                            iObject = (InventoryObject *) 
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                           skills->objects.Find(skillNamePtr);
                            if (iObject && INVOBJ_SKILL == iObject->type) // if found the object
                            {
                                HandleCombine(curAvatar, iObject->WhoAmI());
                            }
                        }
                    }
                }
            }
            break;

        case NWMESS_REQUEST_DUNGEON_INFO:
            messRDInfoPtr = (MessRequestDungeonInfo *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar && SPACE_DUNGEON == ss->WhatAmI())
            {
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex)
                {
                    messDInfo.floor = 0;
                    messDInfo.sizeX = ((DungeonMap *)ss)->width;
                    messDInfo.sizeY = ((DungeonMap *)ss)->height;
                    messDInfo.x     = messRDInfoPtr->x;
                    messDInfo.y     = messRDInfoPtr->y;

                    messDInfo.floor     = ((DungeonMap *)ss)->floorIndex;
                    messDInfo.outerWall = ((DungeonMap *)ss)->outerWallIndex;

                    for (i = 0; i < DUNGEON_PIECE_SIZE; ++i)
                    {
                        for (j = 0; j < DUNGEON_PIECE_SIZE; ++j)
                        {
                            messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            if (i+ messDInfo.y * DUNGEON_PIECE_SIZE < 0 ||
                               i + messDInfo.y * DUNGEON_PIECE_SIZE >= messDInfo.sizeY ||
                                j + messDInfo.x * DUNGEON_PIECE_SIZE < 0 ||
                               j + messDInfo.x * DUNGEON_PIECE_SIZE >= messDInfo.sizeX)
                            {
                                messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            }
                            else
                            {
                                messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 
                                    ((DungeonMap *)ss)->leftWall[
                                       (i + messDInfo.y * DUNGEON_PIECE_SIZE) * ((DungeonMap *)ss)->width +
                                        j + messDInfo.x * DUNGEON_PIECE_SIZE
                                                                    ];
                            }

                            messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            if (i+ messDInfo.y * DUNGEON_PIECE_SIZE < 0 ||
                               i + messDInfo.y * DUNGEON_PIECE_SIZE >= messDInfo.sizeY ||
                                j + messDInfo.x * DUNGEON_PIECE_SIZE < 0 ||
                               j + messDInfo.x * DUNGEON_PIECE_SIZE >= messDInfo.sizeX)
                            {
                                messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            }
                            else
                            {
                                messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 
                                    ((DungeonMap *)ss)->topWall[
                                       (i + messDInfo.y * DUNGEON_PIECE_SIZE) * ((DungeonMap *)ss)->width +
                                        j + messDInfo.x * DUNGEON_PIECE_SIZE
                                                                    ];
                            }
                        }
                    }

                    lserver->SendMsg(sizeof(messDInfo),
                                      (void *)&messDInfo, 0, &tempReceiptList);

                }
            }
            else if (curAvatar && SPACE_GUILD == ss->WhatAmI())
            {
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex)
                {
                    messDInfo.floor = 0;
                    messDInfo.sizeX = ((TowerMap *)ss)->width;
                    messDInfo.sizeY = ((TowerMap *)ss)->height;
                    messDInfo.x     = messRDInfoPtr->x;
                    messDInfo.y     = messRDInfoPtr->y;

                    messDInfo.floor     = ((TowerMap *)ss)->floorIndex;
                    messDInfo.outerWall = ((TowerMap *)ss)->outerWallIndex;

                    for (i = 0; i < DUNGEON_PIECE_SIZE; ++i)
                    {
                        for (j = 0; j < DUNGEON_PIECE_SIZE; ++j)
                        {
                            messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            if (i+ messDInfo.y * DUNGEON_PIECE_SIZE < 0 ||
                               i + messDInfo.y * DUNGEON_PIECE_SIZE >= messDInfo.sizeY ||
                                j + messDInfo.x * DUNGEON_PIECE_SIZE < 0 ||
                               j + messDInfo.x * DUNGEON_PIECE_SIZE >= messDInfo.sizeX)
                            {
                                messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            }
                            else
                            {
                                messDInfo.leftWall[i * DUNGEON_PIECE_SIZE + j] = 
                                    ((TowerMap *)ss)->leftWall[
                                       (i + messDInfo.y * DUNGEON_PIECE_SIZE) * ((TowerMap *)ss)->width +
                                        j + messDInfo.x * DUNGEON_PIECE_SIZE
                                                                    ];
                            }

                            messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            if (i+ messDInfo.y * DUNGEON_PIECE_SIZE < 0 ||
                               i + messDInfo.y * DUNGEON_PIECE_SIZE >= messDInfo.sizeY ||
                                j + messDInfo.x * DUNGEON_PIECE_SIZE < 0 ||
                               j + messDInfo.x * DUNGEON_PIECE_SIZE >= messDInfo.sizeX)
                            {
                                messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 0;
                            }
                            else
                            {
                                messDInfo.topWall[i * DUNGEON_PIECE_SIZE + j] = 
                                    ((TowerMap *)ss)->topWall[
                                       (i + messDInfo.y * DUNGEON_PIECE_SIZE) * ((TowerMap *)ss)->width +
                                        j + messDInfo.x * DUNGEON_PIECE_SIZE
                                                                    ];
                            }
                        }
                    }

                    lserver->SendMsg(sizeof(messDInfo),
                                      (void *)&messDInfo, 0, &tempReceiptList);

                }
            }

            break;

        case NWMESS_DUNGEON_CHANGE:
            messDungeonChangePtr = (MessDungeonChange *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar && SPACE_DUNGEON == ss->WhatAmI())
            {
                int go = FALSE;
                if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
                     ((DungeonMap *) ss)->CanEdit(curAvatar) )
                    go = TRUE;

                if (go)
                {
                    messDungeonChange.floor = messDungeonChange.left = 
                        messDungeonChange.outer = messDungeonChange.top = 
                        messDungeonChange.reset = 0;
                    messDungeonChange.x = messDungeonChangePtr->x;
                    messDungeonChange.y = messDungeonChangePtr->y;

                    if (messDungeonChangePtr->floor)
                    {
                        ((DungeonMap *)ss)->floorIndex += 1;
                        if (((DungeonMap *)ss)->floorIndex >= NUM_OF_DUNGEON_FLOOR_TYPES)
                            ((DungeonMap *)ss)->floorIndex = 0;

                        messDungeonChange.floor = ((DungeonMap *)ss)->floorIndex + 1; 
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                    }
                    else if (messDungeonChangePtr->outer)
                    {
                        ((DungeonMap *)ss)->outerWallIndex += 1;
                        if (((DungeonMap *)ss)->outerWallIndex >= NUM_OF_DUNGEON_WALL_TYPES)
                            ((DungeonMap *)ss)->outerWallIndex = 0;

                        messDungeonChange.outer = ((DungeonMap *)ss)->outerWallIndex + 1; 
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                    }
                    else if (messDungeonChangePtr->left)
                    {
                        int result = ((DungeonMap *)ss)->ChangeWall(TRUE,
                                          messDungeonChangePtr->x,
                                              messDungeonChangePtr->y);

                        if (result >= 0)
                        {
                            messDungeonChange.left = result; 
                            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                        }
                        else if (-2 == result)
                        {
                            sprintf(infoText.text,"Edit outside bounds denied.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                        else if (-1 == result)
                        {
                            sprintf(infoText.text,"That would close off parts of the dungeon.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                    else if (messDungeonChangePtr->top)
                    {
                        int result = ((DungeonMap *)ss)->ChangeWall(FALSE,
                                          messDungeonChangePtr->x,
                                              messDungeonChangePtr->y);

                        if (result >= 0)
                        {
                            messDungeonChange.top = result; 
                            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                        }
                        else if (-2 == result)
                        {
                            sprintf(infoText.text,"Edit outside bounds denied.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                        else if (-1 == result)
                        {
                            sprintf(infoText.text,"That would close off parts of the dungeon.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
            }
            else if (curAvatar && SPACE_GUILD == ss->WhatAmI())
            {
//				if (-1 != curAvatar->charInfoArray[curAvatar->curCharacterIndex].topIndex &&
//					 !strcmp(((TowerMap *) ss)->masterName, curAvatar->name) &&
//					 !strcmp(((TowerMap *) ss)->masterPass, curAvatar->pass)
//					)
                if (TRUE)
                {
                    ((TowerMap *)ss)->lastChangedTime.SetToNow();

                    messDungeonChange.floor = messDungeonChange.left = 
                        messDungeonChange.outer = messDungeonChange.top = 0;
                    messDungeonChange.x = messDungeonChangePtr->x;
                    messDungeonChange.y = messDungeonChangePtr->y;

                    if (messDungeonChangePtr->floor)
                    {
                        ((TowerMap *)ss)->floorIndex += 1;
                        if (((TowerMap *)ss)->floorIndex >= NUM_OF_TOWER_FLOOR_TYPES)
                            ((TowerMap *)ss)->floorIndex = 0;

                        messDungeonChange.floor = ((TowerMap *)ss)->floorIndex + 1; 
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                    }
                    else if (messDungeonChangePtr->outer)
                    {
                        ((TowerMap *)ss)->outerWallIndex += 1;
                        if (((TowerMap *)ss)->outerWallIndex >= NUM_OF_TOWER_WALL_TYPES)
                            ((TowerMap *)ss)->outerWallIndex = 0;

                        messDungeonChange.outer = ((TowerMap *)ss)->outerWallIndex + 1; 
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                    }
                    else if (messDungeonChangePtr->left)
                    {
                        int result = ((TowerMap *)ss)->ChangeWall(TRUE,
                                          messDungeonChangePtr->x,
                                              messDungeonChangePtr->y);

                        if (result >= 0)
                        {
                            messDungeonChange.left = result; 
                            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                        }
                        else if (-2 == result)
                        {
                            sprintf(infoText.text,"Edit outside bounds denied.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                        else if (-1 == result)
                        {
                            sprintf(infoText.text,"That would close off parts of the tower.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                    else if (messDungeonChangePtr->top)
                    {
                        int result = ((TowerMap *)ss)->ChangeWall(FALSE,
                                          messDungeonChangePtr->x,
                                              messDungeonChangePtr->y);

                        if (result >= 0)
                        {
                            messDungeonChange.top = result; 
                            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                        sizeof(messDungeonChange),(void *)&messDungeonChange);
                        }
                        else if (-2 == result)
                        {
                            sprintf(infoText.text,"Edit outside bounds denied.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                        else if (-1 == result)
                        {
                            sprintf(infoText.text,"That would close off parts of the tower.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
            }
            break;
                                            
        case NWMESS_REQUEST_AVATAR_INFO:
            messRequestAvatarInfo = (MessRequestAvatarInfo *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);

            if (!curAvatar)
                break;

            curMob = (BBOSMob *) ss->avatars->First();
            while (curMob)
            {
                BBOSAvatar *av = (BBOSAvatar *) curMob;
                if (SMOB_AVATAR == curMob->WhatAmI() &&
                     messRequestAvatarInfo->avatarID == av->socketIndex)
                {
                    MessAvatarStats mStats;
                    av->BuildStatsMessage(&mStats);
                    ss->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);

                    // tell people about my cool dragons!
                    for (int index = 0; index < 2; ++index)
                    {
                        if (255 != av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].type)
                        {
                            // tell everyone about it!
                            MessPet mPet;
                            mPet.avatarID = av->socketIndex;
                            CopyStringSafely(av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].name,16, 
                                                  mPet.name,16);
                            mPet.quality = av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].quality;
                            mPet.type    = av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].type;
                            mPet.state   = av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].state;
                            mPet.size    = av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].lifeStage +
                                    av->charInfoArray[av->curCharacterIndex].petDragonInfo[index].healthModifier / 10.0f;
                            mPet.which   = index;

                            ss->lserver->SendMsg(sizeof(mPet),(void *)&mPet, 0, &tempReceiptList);
                        }
                    }

                    av->AssertGuildStatus(ss, FALSE, fromSocket);

                    InventoryObject *iObject = (InventoryObject *) 
                        av->charInfoArray[av->curCharacterIndex].wield->objects.First();
                    while (iObject)
                    {
                        if (INVOBJ_BLADE == iObject->type)
                        {
                            FillBladeDescMessage(&messBladeDesc, iObject, av);
                            ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                            iObject = (InventoryObject *) 
                                av->charInfoArray[av->curCharacterIndex].wield->objects.Last();
                        }
                        else if (INVOBJ_STAFF == iObject->type)
                        {
                            InvStaff *iStaff = (InvStaff *) iObject->extra;
                            
                            MessBladeDesc messBladeDesc;
                            messBladeDesc.bladeID = (long)iObject;
                            messBladeDesc.size    = 4;
                            messBladeDesc.r       = staffColor[iStaff->type][0];
                            messBladeDesc.g       = staffColor[iStaff->type][1];
                            messBladeDesc.b       = staffColor[iStaff->type][2];
                            messBladeDesc.avatarID= av->socketIndex;
                            messBladeDesc.trailType  = 0;
                            messBladeDesc.meshType = BLADE_TYPE_STAFF1;
                            ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                            iObject = (InventoryObject *) 
                                av->charInfoArray[av->curCharacterIndex].wield->objects.Last();
                        }

                        iObject = (InventoryObject *) 
                            av->charInfoArray[av->curCharacterIndex].wield->objects.Next();
                    }

                    curMob = NULL;
                }
                else
                    curMob = (BBOSMob *) ss->avatars->Next();
            }

            break;

        case NWMESS_REQUEST_TOWNMAGE_SERVICE:
            messTMSPtr = (MessRequestTownMageService *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);

            if (!curAvatar)
                break;

            curMob = (BBOSMob *) ss->mobList->GetFirst(curAvatar->cellX, curAvatar->cellY);
            while (curMob)
            {
                if (SMOB_TOWNMAGE == curMob->WhatAmI())
                {
                    if (curMob->cellX == curAvatar->cellX && curMob->cellY == curAvatar->cellY)
                    {
                        switch(messTMSPtr->which)
                        {
                        case TMSERVICE_TELEPORT:
                            HandleTeleport(curAvatar, curMob, ss, TRUE); 
                            break;
                        case TMSERVICE_TELEPORT_BACK:
                            HandleTeleport(curAvatar, curMob, ss, FALSE); 
                            break;
                        case TMSERVICE_HEAL:
                            {
                                if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money < 180)
                                {
                                    sprintf(infoText.text,"You don't have 180 gold for healing.");
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    break;
                                }

                                if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health >=
                                     curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax)
                                {
                                    sprintf(infoText.text,"You are already fully healed.");
                                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                    break;
                                }
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money 
                                               -= 180;

                                assert(
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                                long amount = 50;
                                amount -= curAvatar->GetDodgeLevel() * 3;
                                if (amount < 3)
                                    amount = 3;
                                long realAmount = amount;
                                if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax -
                                     curAvatar->charInfoArray[curAvatar->curCharacterIndex].health	<
                                     realAmount)
                                    realAmount =
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax -
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;

                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].health += amount;
                                if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].health >
                                     curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax)
                                {
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].health =
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
                                }

                                MessAvatarHealth messHealth;
                                messHealth.health    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
                                messHealth.healthMax = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
                                messHealth.avatarID  = curAvatar->socketIndex;
                                ss->lserver->SendMsg(sizeof(messHealth),(void *)&messHealth, 0, &tempReceiptList);

                                sprintf(infoText.text,"You are healed for %d points.",amount);
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                curAvatar->AnnounceSpecial(ss, SPECIAL_APP_HEAL);

                                source = fopen("healing.txt","a");
                                
                                /* Display operating system-style date and time. */
                                _strdate( tempText );
                                fprintf(source, "%s, ", tempText );
                                _strtime( tempText );
                                fprintf(source, "%s, ", tempText );

                                fprintf(source,"%s, %ld, %ld\n",
                                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                                         amount, realAmount);

                                fclose(source);
                            }
                            break;
                        case TMSERVICE_BANK:
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = curAvatar->bank;
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving = TRUE;
                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_BANK);
                            break;
                        }
                    }
                }
                curMob = (BBOSMob *) ss->mobList->GetNext();
            }

            break;

        case NWMESS_AVATAR_SEND_MONEY:
            messMoney = (MessAvatarSendMoney *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);
            if (curAvatar)
            {
                if (-1 == messMoney->avatarID && -2 == messMoney->targetAvatarID)
                {
                    // change money in secure trading
                    if (curAvatar->tradingPartner)
                    {
                        SharedSpace *sx;
                        BBOSAvatar *partnerAvatar = NULL;
                        partnerAvatar = FindAvatar(curAvatar->tradingPartner, &sx);

                        if (partnerAvatar)
                        {
                            assert(
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                            curAvatar->StateNoAgreement(ss);

                            assert(
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                            // copy back previous amount
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money +=
                                curAvatar->trade->money;

                            assert(
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                            long change = messMoney->amount;
                            if (messMoney->amount <= 0)
                                change = 0;
                            else if (messMoney->amount > 
                                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                            inventory->money)
                            {
                                assert(change >= 0);
                                change = curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                            inventory->money;
                                assert(change >= 0);
                            }

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                             inventory->money -= change;
                            assert(curAvatar->trade->money >= 0);
                            curAvatar->trade->money = change;
                            assert(curAvatar->trade->money >= 0);

                            assert(
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_YOUR_SECURE);
                            TellClientAboutInventory(partnerAvatar, MESS_INVENTORY_HER_SECURE);

                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                        }
                    }
                }
                else if (-1 == messMoney->avatarID && -3 == messMoney->targetAvatarID)
                {
                    // change money in bank
                    // copy back previous amount
//					curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money +=
//						curAvatar->bank->money;

                    long change = messMoney->amount;

                    if (change > 10000000)
                        change = 10000000;
                    if (change < -10000000)
                        change = -10000000;

                    if (change >= 0)
                    {
                        if (change > curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                    inventory->money)
                            change = curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                    inventory->money;
                    }
                    else
                    {
                        if (-change > curAvatar->bank->money)
                             change = -curAvatar->bank->money;
                    }

                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                     inventory->money -= change;
                    curAvatar->bank->money += change;

                    assert(
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money >= 0);

                    TellClientAboutInventory(curAvatar, MESS_INVENTORY_BANK);
                    TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                
                }
            }

            
            break;

        case NWMESS_CHEST_INFO:
            messChestPtr = (MessChestInfo *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);
            if	(curAvatar)
            {
                if	(SPACE_DUNGEON == ss->WhatAmI())
                {
                    curMob = (BBOSMob *) ss->mobList->GetFirst(0,0,1000);
                    while (curMob)
                    {
                        if (SMOB_CHEST == curMob->WhatAmI() && messChestPtr->mobID == (long) curMob)
                        {
                            BBOSChest *c = (BBOSChest *) curMob;
                            if (!c->isOpen)
                            {
                                // open it
                                c->isOpen = TRUE;
                                c->openTime = timeGetTime();

                                // tell everyone near that it's open
                                MessChestInfo chestInfo;
                                chestInfo.mobID = (unsigned long) curMob;
                                chestInfo.type = 1;
                                ss->SendToEveryoneNearBut(0, c->cellX, c->cellY, sizeof(chestInfo),(void *)&chestInfo);

                                // spill the treasure
                                ((DungeonMap *)ss)->DoChestDrop(c);
                            }
                            else
                            {
                                sprintf(infoText.text,"The chest is open and empty.");
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }

                            ss->mobList->SetToLast();
                        }
                        curMob = (BBOSMob *) ss->mobList->GetNext();
                    }
                }
                else
                {
                    sprintf(infoText.text,"You can't open it.");
                    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
            }
            break;

        case NWMESS_TALK_TO_TREE:
            messTreePtr = (MessTalkToTree *) messData;
//			tempReceiptList.clear();
//			tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);

            if	(curAvatar)
            {
                HandleTreeTalk(curAvatar, ss, messTreePtr);
            }
            break;

        case NWMESS_TEST_PING:
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            lserver->SendMsg(sizeof(messTP),(void *)&messTP, 0, &tempReceiptList);
            break;

        case NWMESS_PET_NAME:
            messPNPtr = (MessPetName *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);

            GuaranteeTermination(messPNPtr->text, 16);
            CorrectString(messPNPtr->text);
            CleanString(messPNPtr->text);

            if	(curAvatar && messPNPtr->text[0] != 0)
            {
                BBOSCharacterInfo *charac = 
                      &curAvatar->charInfoArray[curAvatar->curCharacterIndex];

                if (charac->petDragonInfo[0].type != 255 && 
                     (0 == charac->petDragonInfo[0].name[0] ||
                      !strcmp(charac->petDragonInfo[0].name,"Pet A")))
                {
                    CopyStringSafely(messPNPtr->text,16, charac->petDragonInfo[0].name,16);

                    MessPet mPet;
                    mPet.avatarID = curAvatar->socketIndex;
                    CopyStringSafely(charac->petDragonInfo[0].name,16, mPet.name,16);
                    mPet.quality = charac->petDragonInfo[0].quality;
                    mPet.type    = charac->petDragonInfo[0].type;
                    mPet.state   = charac->petDragonInfo[0].state;
                    mPet.size    = charac->petDragonInfo[0].lifeStage +
                            charac->petDragonInfo[0].healthModifier / 10.0f;
                    mPet.which   = 0;

                    ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                        sizeof(mPet),(void *)&mPet);
                }
                else if (charac->petDragonInfo[1].type != 255 && 
                     (0 == charac->petDragonInfo[1].name[0] ||
                      !strcmp(charac->petDragonInfo[1].name,"Pet B")))
                {
                    CopyStringSafely(messPNPtr->text,16, charac->petDragonInfo[1].name,16);

                    MessPet mPet;
                    mPet.avatarID = curAvatar->socketIndex;
                    CopyStringSafely(charac->petDragonInfo[1].name,16, mPet.name,16);
                    mPet.quality = charac->petDragonInfo[1].quality;
                    mPet.type    = charac->petDragonInfo[1].type;
                    mPet.state   = charac->petDragonInfo[1].state;
                    mPet.size    = charac->petDragonInfo[1].lifeStage +
                            charac->petDragonInfo[1].healthModifier / 10.0f;
                    mPet.which   = 1;

                    ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                        sizeof(mPet),(void *)&mPet);
                }
            }
            break;

        case NWMESS_FEED_PET_REQUEST:
            messFPRPtr = (MessFeedPetRequest *) messData;
            tempReceiptList.clear();
            tempReceiptList.push_back(fromSocket);

            curAvatar = FindAvatar(fromSocket, &ss);

            if	(curAvatar)
            {
                HandlePetFeeding(messFPRPtr, curAvatar, ss);
            }
            break;

        case NWMESS_ADMIN_MESSAGE:
            adminMessPtr = (MessAdminMessage *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);

            if	(curAvatar && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                HandleAdminMessage(adminMessPtr, curAvatar, ss);
            }
            break;

        case NWMESS_INFO_FLAGS:
            infoFlagsPtr = (MessInfoFlags *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);

            if	(curAvatar)
            {
                curAvatar->infoFlags = infoFlagsPtr->flags;
                tempReceiptList.clear();
                tempReceiptList.push_back(fromSocket);
                lserver->SendMsg(sizeof(MessInfoFlags),(void *)infoFlagsPtr, 0, &tempReceiptList);
            }
            break;

        case NWMESS_SELL_ALL:
            sellAllPtr = (MessSellAll *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);

            if	(curAvatar)
            {
                curNpc = (BBOSNpc *) ss->mobList->IsInList((BBOSMob *)sellAllPtr->which, TRUE);
                if (curNpc &&  
                     curNpc->cellX == curAvatar->cellX &&
                     curNpc->cellY == curAvatar->cellY &&
                     SMOB_TRADER == curNpc->WhatAmI()
                    )
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner =
                        (curNpc->inventory);
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->giving = FALSE;
//					curNpc->inventory.subType = curNpc->WhatAmI();

//					TellClientAboutInventory(curAvatar, MESS_INVENTORY_TRADER);

                    HandleSellingAll(curAvatar, curNpc, sellAllPtr->type);
                }
            }
            break;

        case NWMESS_SECURE_TRADE:
            secureTradePtr = (MessSecureTrade *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);
            if	(curAvatar)
            {
                switch (secureTradePtr->type)
                {
                case MESS_SECURE_STOP:
                default:
                    curAvatar->AbortSecureTrading(ss);
                    break;

                case MESS_SECURE_ACCEPT:
                    curAvatar->agreeToTrade = TRUE;
                    if (curAvatar->tradingPartner)
                    {
                        SharedSpace *sx;
                        BBOSAvatar *partnerAvatar = NULL;
                        partnerAvatar = FindAvatar(curAvatar->tradingPartner, &sx);

                        if (partnerAvatar && partnerAvatar->agreeToTrade)
                            curAvatar->CompleteSecureTrading(ss);
                    }
                    break;
                }
            }
            break;

        case NWMESS_EXTENDED_INFO_REQUEST:
            requestExInfoPtr = (MessExtendedInfoRequest *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);
            if	(curAvatar)
            {
                HandleExtendedInfo(curAvatar, requestExInfoPtr);
            }
            break;

        case NWMESS_KEYCODE:
            keyCodePtr = (MessKeyCode *) messData;
            codePad = cryptoText3;
            UnCryptoString(keyCodePtr->string);

            curAvatar = FindIncomingAvatar(fromSocket);
            if	(curAvatar)
            {
                HandleKeyCode(curAvatar, keyCodePtr);
            }
            break;

        case NWMESS_SET_BOMB:
            setBombPtr = (MessSetBomb *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);
            if	(curAvatar)
            {
                InventoryObject *detonatedBomb = NULL;

                curInventory = 
                       curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;
                iObject = (InventoryObject *)curInventory->objects.First();
                while (iObject)
                {
                    if (setBombPtr->ptr == (long)iObject && INVOBJ_BOMB == iObject->type)
                    {
                        InvBomb *ib = (InvBomb *) iObject->extra;

                        BBOSBomb *bomb = new BBOSBomb(curAvatar);
                        bomb->cellX = curAvatar->cellX;
                        bomb->cellY = curAvatar->cellY;
                        bomb->type  = ib->type;
                        bomb->flags = ib->flags;
                        bomb->r     = ib->r;
                        bomb->g     = ib->g;
                        bomb->b     = ib->b;
                        bomb->power = ib->power;
                        bomb->detonateTime = timeGetTime() + ib->fuseDelay * 1000;
                        if (ib->stability < rnd(0,1))
                            bomb->detonateTime = timeGetTime();

                        ss->mobList->Add(bomb);

                        sprintf(tempText,"%s sets a bomb!",
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                            sizeof(infoText),(void *)&infoText);

                        detonatedBomb = iObject;
                        iObject = (InventoryObject *)curInventory->objects.Last();

                    }
                    else if (setBombPtr->ptr == (long)iObject && INVOBJ_POTION == iObject->type)
                    {
                        InvPotion *ip = (InvPotion *) iObject->extra;

                        if (POTION_TYPE_RECALL      == ip->type || 
                             POTION_TYPE_DARK_RECALL == ip->type)
                        {
                            curAvatar->QuestSpaceChange(NULL, NULL);
                            TransferAvatar(FALSE, fromSocket);

                            tempReceiptList.clear();
                            tempReceiptList.push_back(fromSocket);
                            MessChangeMap changeMap;

                            changeMap.oldType = ss->WhatAmI(); 
                            changeMap.newType = SPACE_GROUND; 
                            changeMap.sizeX   = MAP_SIZE_WIDTH;
                            changeMap.sizeY   = MAP_SIZE_HEIGHT;
                            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                            if (POTION_TYPE_RECALL == ip->type)
                            {
                                curAvatar->cellX = curAvatar->targetCellX = 
                                    townList[ip->subType].x;
                                curAvatar->cellY = curAvatar->targetCellY = 
                                    townList[ip->subType].y;

                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnX = 
                                    townList[ip->subType].x;
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].spawnY = 
                                    townList[ip->subType].y;
                            }
                            else if (POTION_TYPE_DARK_RECALL == ip->type)
                            {
                                int theX = 220;
                                int theY = 220;
                                if (1 == ip->subType)
                                {
                                    theX = 90;
                                    theY = 53;
                                }
                                curAvatar->cellX = curAvatar->targetCellX = theX;
                                curAvatar->cellY = curAvatar->targetCellY = theY;
                            }

                            TransferAvatar(TRUE, fromSocket);

                        }
                        else if (POTION_TYPE_TOWER_RECALL == ip->type)
                        {
                            // find the tower
                            SharedSpace *sp;
                            if (FindGuild(ip->subType, &sp))
                            {
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;

                                // tell everyone I'm dissappearing
                                curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                                tempReceiptList.clear();
                                tempReceiptList.push_back(fromSocket);
                                MessChangeMap changeMap;
                                changeMap.dungeonID = (long)sp;
                                changeMap.oldType = ss->WhatAmI(); 
                                changeMap.newType = sp->WhatAmI(); 
                                changeMap.sizeX   = 5;
                                changeMap.sizeY   = 5;
                                lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                                MessInfoText infoText;
                                sprintf(tempText,"You enter the guild tower of %s.", ((TowerMap *) sp)->WhoAmI());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                curAvatar->QuestSpaceChange(ss,sp);

                                // move me to my new SharedSpace
                                ss->avatars->Remove(curAvatar);
                                sp->avatars->Append(curAvatar);

                                ((TowerMap *)sp)->lastChangedTime.SetToNow();

                                curAvatar->cellX = curAvatar->targetCellX = 4;
                                curAvatar->cellY = curAvatar->targetCellY = 4;

                                // tell everyone about my arrival
                                curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                                // tell this player about everyone else around
                                curAvatar->UpdateClient(sp, TRUE);
                            }
                        }

                        detonatedBomb = iObject;
                        iObject = (InventoryObject *)curInventory->objects.Last();
                    }
                    else if (setBombPtr->ptr == (long)iObject && INVOBJ_EARTHKEY == iObject->type)
                    {
                        InvEarthKey *iek = (InvEarthKey *) iObject->extra;
                        tempReceiptList.clear();
                        tempReceiptList.push_back(fromSocket);

                        if (SPACE_GROUND == ss->WhatAmI())
                        {
                            // inside a town?
                            int isInTown = FALSE;
                            for (int i = 0; i < NUM_OF_TOWNS; ++i)
                            {
                                if (abs(townList[i].x - curAvatar->cellX) <= 3 && 
                                     abs(townList[i].y - curAvatar->cellY) <= 3)
                                {
                                    isInTown = TRUE;
                                }
                            }

                            if (!isInTown)
                            {
                                HandleEarthKeyUse(curAvatar, iek, ss);

                                detonatedBomb = iObject;
                                iObject = (InventoryObject *)curInventory->objects.Last();
                            }
                            else
                            {
                                sprintf(infoText.text,"The EarthKey doesn't work in a town.");
                                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                        else
                        {
                            sprintf(infoText.text,"The EarthKey doesn't work here.");
                            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }

                    iObject = (InventoryObject *)curInventory->objects.Next();
                }

                if (detonatedBomb)
                {
                    if (detonatedBomb->amount > 1)
                        detonatedBomb->amount -= 1;
                    else
                    {
                        curInventory->objects.Remove(detonatedBomb);
                        delete detonatedBomb;
                    }
                    UpdateInventory(curAvatar);
                }


            }
            break;

        case NWMESS_CHAT_CHANNEL:
            chatChannelPtr = (MessChatChannel *) messData;

            curAvatar = FindAvatar(fromSocket, &ss);
//			curAvatar = FindIncomingAvatar(fromSocket);
            if	(curAvatar)
            {
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType || 
                    ACCOUNT_TYPE_MODERATOR == curAvatar->accountType || 
                    ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType || 
                          curAvatar->charInfoArray[curAvatar->curCharacterIndex].imageFlags &
                         SPECIAL_LOOK_HELPER)
                {
                    if (chatChannelPtr->value & curAvatar->chatChannels)
                        curAvatar->chatChannels &= ~(chatChannelPtr->value);
                    else
                        curAvatar->chatChannels |= (chatChannelPtr->value);
                }
                else
                {
                    if (chatChannelPtr->value & curAvatar->chatChannels)
                        curAvatar->chatChannels &= ~(chatChannelPtr->value);
                    else
                        curAvatar->chatChannels = (chatChannelPtr->value);
                }

                MessChatChannel returnCChan;
                returnCChan.value = curAvatar->chatChannels;

                tempReceiptList.clear();
                tempReceiptList.push_back(fromSocket);
                lserver->SendMsg(sizeof(returnCChan),(void *)&returnCChan, 0, &tempReceiptList);
            }
            break;

        }
        lserver->GetNextMsg(NULL, dataSize);
    }

}

    


//*******************************************************************************
void BBOServer::TransferAvatar(int intoWorld, int handle)
{
//	BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    MessAvatarAppear messAvAppear;
    MessAvatarDisappear messAvDisappear;
    MessBladeDesc messBladeDesc;
//	char tempText[1024];

    SharedSpace *ss;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(handle);

    if (intoWorld)
    {
        curAvatar = FindIncomingAvatar(handle);
        if (curAvatar)
        {
            ss = (SharedSpace *) spaceList->Find(SPACE_GROUND);
            incoming->Remove(curAvatar);
            ss->avatars->Prepend(curAvatar);

            curAvatar->IntroduceMyself(ss, SPECIAL_APP_ENTER_GAME);

            // tell this player about everyone else around
            curAvatar->UpdateClient(ss, TRUE);
            /*
            curMob = (BBOSMob *) ss->avatars->First();
            while (curMob)
            {
                if (SMOB_AVATAR == curMob->WhatAmI())
                {
                    curAvatar = (BBOSAvatar *) curMob;
                    messAvAppear.avatarID = curAvatar->socketIndex;
                    messAvAppear.x = curAvatar->cellX;
                    messAvAppear.y = curAvatar->cellY;
                    ss->lserver->SendMsg(sizeof(messAvAppear),(void *)&messAvAppear, 0, &tempReceiptList);
                    MessAvatarStats mStats;
                    curAvatar->BuildStatsMessage(&mStats);
                    ss->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);

                    InventoryObject *iObject = (InventoryObject *) 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->objects.First();
                    while (iObject)
                    {
                        if (INVOBJ_BLADE == iObject->type)
                        {
                                InvBlade *iBlade = (InvBlade *) iObject->extra;
                                messBladeDesc.bladeID = (long)iObject;
                                messBladeDesc.size    = iBlade->size;
                                messBladeDesc.r       = iBlade->r;
                                messBladeDesc.g       = iBlade->g;
                                messBladeDesc.b       = iBlade->b;
                                messBladeDesc.avatarID= curAvatar->socketIndex;
                                ss->lserver->SendMsg(sizeof(messBladeDesc),(void *)&messBladeDesc, 0, &tempReceiptList);
                        }
                        iObject = (InventoryObject *) 
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->objects.Next();
                    }

                }
                else
                    ; // tell the player about other things, like monsters!!

                curMob = (BBOSMob *) ss->avatars->Next();
            }
            */


        }

    }
    else
    {
        curAvatar = FindAvatar(handle, &ss);
        if (curAvatar)
        {
            curAvatar->AnnounceSpecial(ss, SPECIAL_APP_LEAVE_GAME);

            ss->avatars->Remove(curAvatar);
            incoming->Append(curAvatar);
            messAvDisappear.avatarID = handle;
            ss->lserver->SendMsg(sizeof(messAvDisappear),(void *)&messAvDisappear);
        }
    }

}


//*******************************************************************************
BBOSAvatar * BBOServer::FindAvatar(int id, SharedSpace **sp)
{
   BBOSMob *curMob;
    BBOSAvatar *curAvatar;


    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;
                if (curAvatar->socketIndex == id)
                {
                    return curAvatar;
                }
            }
            curMob = (BBOSMob *) (*sp)->avatars->Next();
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }
    return NULL;
}

//*******************************************************************************
BBOSAvatar * BBOServer::FindAvatar(BBOSMob *mobPtr, SharedSpace **sp)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;


    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;
                if (curAvatar == mobPtr)
                {
                    return curAvatar;
                }
            }
            curMob = (BBOSMob *) (*sp)->avatars->Next();
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }
    return NULL;
}

//*******************************************************************************
BBOSAvatar * BBOServer::FindAvatar(char *name, char *password, SharedSpace **sp)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    char tempText[1024];

    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;
                if (IsCompletelyVisiblySame(curAvatar->name, name))
                {
                    return curAvatar;
                }
                if (IsCompletelyVisiblySame(curAvatar->name, name))
                {
                    LongTime lt;
                    lt.value.wHour += 19;
                    if (lt.value.wHour < 24)
                    {
                        lt.value.wDay -= 1;
                    }
                    else
                        lt.value.wHour -= 24;

                    sprintf(tempText,"%d/%02d, %d:%02d,    ", (int)lt.value.wMonth, (int)lt.value.wDay, 
                              (int)lt.value.wHour, (int)lt.value.wMinute);
                    LogOutput("dup-possibles.txt", tempText);

                    sprintf(tempText,">%s< (%s)    ",curAvatar->name, curAvatar->pass);
                    LogOutput("dup-possibles.txt", tempText);
                    sprintf(tempText,">%s< (%s)\n",name, password);
                    LogOutput("dup-possibles.txt", tempText);
                }
            }
            curMob = (BBOSMob *) (*sp)->avatars->Next();
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }

    DoublyLinkedList *list = incoming;

    *sp = NULL; 

    curMob = (BBOSMob *) list->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            curAvatar = (BBOSAvatar *) curMob;
            if (IsCompletelyVisiblySame(curAvatar->name, name))
            {
                return curAvatar;
            }

            if (IsCompletelyVisiblySame(curAvatar->name, name))
            {
                LongTime lt;
                lt.value.wHour += 19;
                if (lt.value.wHour < 24)
                {
                    lt.value.wDay -= 1;
                }
                else
                    lt.value.wHour -= 24;

                sprintf(tempText,"%d/%02d, %d:%02d,    ", (int)lt.value.wMonth, (int)lt.value.wDay, 
                          (int)lt.value.wHour, (int)lt.value.wMinute);
                LogOutput("dup-possibles.txt", tempText);

                sprintf(tempText,">%s< (%s)    ",curAvatar->name, curAvatar->pass);
                LogOutput("dup-possibles.txt", tempText);
                sprintf(tempText,">%s< (%s)\n",name, password);
                LogOutput("dup-possibles.txt", tempText);
            }
        }
        curMob = (BBOSMob *) list->Next();
    }

    return NULL;
}

//*******************************************************************************
BBOSAvatar * BBOServer::FindAvatarByAvatarName(char *avatarName, SharedSpace **sp)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;


    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;
                char *name = curAvatar->charInfoArray[curAvatar->curCharacterIndex].name;
                if (!(stricmp(avatarName, name)))
                {
                    return curAvatar;
                }
            }
            curMob = (BBOSMob *) (*sp)->avatars->Next();
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }
    return NULL;
}

//*******************************************************************************
BBOSAvatar * BBOServer::FindAvatarByPartialName(char *avatarName, SharedSpace **sp)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;


    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;
                char *name = curAvatar->charInfoArray[curAvatar->curCharacterIndex].name;
                if (!(strnicmp(avatarName, name, strlen(avatarName))))
                {
                    return curAvatar;
                }
            }
            curMob = (BBOSMob *) (*sp)->avatars->Next();
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }
    return NULL;
}

//*******************************************************************************
BBOSAvatar * BBOServer::FindIncomingAvatar(int id)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;

    DoublyLinkedList *list = incoming;

    curMob = (BBOSMob *) list->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            curAvatar = (BBOSAvatar *) curMob;
            if (curAvatar->socketIndex == id)
            {
                return curAvatar;
            }
        }
        curMob = (BBOSMob *) list->Next();
    }

    return NULL;
}

//*******************************************************************************
BBOSMonster * BBOServer::FindMonster(BBOSMob *mobPtr, SharedSpace **sp)
{
    BBOSMob *curMob;
    BBOSMonster *curMonster;


    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        curMob = (BBOSMob *) (*sp)->mobList->IsInList(mobPtr);
        if (curMob && SMOB_MONSTER == curMob->WhatAmI())
        {
            curMonster = (BBOSMonster *) curMob;
            return curMonster;
        }

        (*sp) = (SharedSpace *) spaceList->Next();
    }
    return NULL;
}

//*******************************************************************************
void BBOServer::BuildInventoryInfoStruct(MessInventoryInfo *info, 
                                                      Inventory *inv, int index, 
                                                      int isPlayerData)
{
    /*
   char text[10][32];
   char status[10], type[10];
   int offset;
   char isPlayerInfo;
   long money, moneyDelta;
      */

    int i;

    InventoryObject *io = (InventoryObject *) inv->objects.First();
    for (i = 0; io; ++i)
        io = (InventoryObject *) inv->objects.Next();

    int numOfItems = i;
    if (index > numOfItems - 9)
        index = numOfItems - 9;
    if (index < 0)
        index = 0;

    io = (InventoryObject *) inv->objects.First();
    for (i = 0; i < index; ++i)
        io = (InventoryObject *) inv->objects.Next();

    for (int j = 0; j < 10; ++j)
    {
        if (io)
        {
            memcpy(info->text[j],io->WhoAmI(),31);
            info->text[j][31] = 0; // blank means no object in this slot.
//			info->status[j] = io->status;
            info->type[j] = io->type;
            info->ptr[j] = (long)io;
            info->amount[j] = io->amount;
            info->value[j] = io->value;
            switch(io->type)
            {
            case INVOBJ_BLADE:
                info->f1[j] = ((InvBlade *)io->extra)->toHit;
                info->f2[j] = ((InvBlade *)io->extra)->damageDone;
                break;
            case INVOBJ_SKILL:
                info->f1[j] = ((InvSkill *)io->extra)->skillLevel;
                info->f2[j] = ((InvSkill *)io->extra)->skillPoints;
                break;
            }
        }
        else
        {
            info->text[j][0] = 0; // blank means no object in this slot.
        }
        if (io)
            io = (InventoryObject *) inv->objects.Next();
    }

    info->isPlayerInfo = isPlayerData;
    info->invPtr = (long)inv;
    info->money = inv->money;
//	info->moneyDelta = inv->moneyDelta;
    info->offset = index;
}

//*******************************************************************************
void BBOServer::UpdateInventory(BBOSAvatar *avatar)
{
    TellClientAboutInventory(avatar, avatar->lastPlayerInvSent);
}

//*******************************************************************************
void BBOServer::TellClientAboutInventory(BBOSAvatar *avatar, int type)
{
//	char smlText[128];
    MessInventoryInfo infoBase;
    MessInventoryInfo *info = &infoBase;
    Inventory *inv;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);

    Chronos::BStream *	stream		= NULL;
    stream	= new Chronos::BStream(sizeof(MessInventoryInfo));

    *stream << (unsigned char) NWMESS_INVENTORY_INFO; 

/*
                *stream << (unsigned char) NWMESS_PLAYER_CHAT_LINE; 
                stream->write(tEdit->text, strlen(tEdit->text));
                *stream << (unsigned char) 0; 
                lclient->SendMsg(stream->used(), stream->buffer());
*/
    int *indexPtr;

    long avatarCash = 0;
    if (avatar)
        avatarCash = avatar->charInfoArray[avatar->curCharacterIndex].inventory->money;

    SharedSpace *sx;
    BBOSAvatar *partnerAvatar = NULL;
    if (avatar->tradingPartner)
        partnerAvatar = FindAvatar(avatar->tradingPartner, &sx);

    int listSize = 9, listStartOffset = 0;

    switch(type)
    {
    case MESS_INVENTORY_PLAYER:
        listSize = 9;
        inv = (avatar->charInfoArray[avatar->curCharacterIndex].inventory);
        *stream << (unsigned char) TRUE; 
//		info->isPlayerInfo = TRUE;
        *stream << (unsigned char) MESS_INVENTORY_PLAYER; 
//		info->traderType = MESS_INVENTORY_PLAYER;
        indexPtr = &avatar->indexList[MESS_INVENTORY_PLAYER];
        avatar->lastPlayerInvSent = type;

        if (avatar->controlledMonster)
        {
//			SharedSpace *sx;

            BBOSMonster * theMonster = FindMonster(
                      avatar->controlledMonster, &sx);
            if (theMonster)
            {
                inv = (theMonster->inventory);
            }
        }
        break;

    case MESS_WORKBENCH_PLAYER:
        listSize = 11;
        inv = (avatar->charInfoArray[avatar->curCharacterIndex].workbench);
        *stream << (unsigned char) TRUE; 
//		info->isPlayerInfo = TRUE;
        *stream << (unsigned char) MESS_WORKBENCH_PLAYER; 
//		info->traderType = MESS_WORKBENCH_PLAYER;
        indexPtr = &avatar->indexList[MESS_WORKBENCH_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    case MESS_SKILLS_PLAYER:
        listSize = 11;
        inv = (avatar->charInfoArray[avatar->curCharacterIndex].skills);
        *stream << (unsigned char) TRUE; 
//		info->isPlayerInfo = TRUE;
        *stream << (unsigned char) MESS_SKILLS_PLAYER; 
//		info->traderType = MESS_SKILLS_PLAYER;
        indexPtr = &avatar->indexList[MESS_SKILLS_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    case MESS_WIELD_PLAYER:
        listSize = 11;
        inv = (avatar->charInfoArray[avatar->curCharacterIndex].wield);
        *stream << (unsigned char) TRUE; 
//		info->isPlayerInfo = TRUE;
        *stream << (unsigned char) MESS_WIELD_PLAYER; 
//		info->traderType = MESS_WIELD_PLAYER;
        indexPtr = &avatar->indexList[MESS_WIELD_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    default:
    case MESS_INVENTORY_TRADER:
        inv = avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_TRADER; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        break;

    case MESS_INVENTORY_TRAINER:
        inv = avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_TRAINER; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        break;

    case MESS_INVENTORY_BANK:
        inv = avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_BANK; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        avatarCash = avatar->bank->money;
        break;

    case MESS_INVENTORY_GROUND:
        inv = avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_GROUND; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        break;

    case MESS_INVENTORY_TOWER:
        if (FindAvatarInGuild(avatar->charInfoArray[avatar->curCharacterIndex].name, &sx))
        {
            inv = (((TowerMap *) sx)->itemBox);
            *stream << (unsigned char) FALSE; 
            *stream << (unsigned char) MESS_INVENTORY_TOWER; 
            indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        }
        break;

    case MESS_INVENTORY_HER_SECURE:
        if (!partnerAvatar)
            return;
        inv = partnerAvatar->trade;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_HER_SECURE; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_HER_SECURE];
        avatarCash = partnerAvatar->trade->money;
        break;

    case MESS_INVENTORY_YOUR_SECURE:
        inv = avatar->trade;
        *stream << (unsigned char) FALSE; 
        *stream << (unsigned char) MESS_INVENTORY_YOUR_SECURE; 
        indexPtr = &avatar->indexList[MESS_INVENTORY_YOUR_SECURE];
        avatarCash = avatar->trade->money;
        break;

    }

    if (!inv)
        inv = (avatar->charInfoArray[avatar->curCharacterIndex].inventory);

    int i;

    InventoryObject *io = (InventoryObject *) inv->objects.First();
    for (i = 0; io; ++i)
        io = (InventoryObject *) inv->objects.Next();

    int numOfItems = i;
    if (*(indexPtr) > numOfItems - listSize+1)
        *(indexPtr) = numOfItems - listSize+1;
    if (*(indexPtr) < 0)
        *(indexPtr) = 0;

    io = (InventoryObject *) inv->objects.First();
    for (i = 0; i < *(indexPtr); ++i)
        io = (InventoryObject *) inv->objects.Next();

    for (int j = 0; j < listSize; ++j)
    {
        if (io)
        {
            *stream << (char) io->type; 

            stream->write(io->WhoAmI(), strlen(io->WhoAmI()));
            *stream << (unsigned char) 0; 
//			memcpy(info->text[j],io->WhoAmI(),31);
//			info->text[j][31] = 0; // blank means no object in this slot.
//			info->type[j] = io->type;
            *stream << (long) io; 
//			info->ptr[j] = (long)io;
            *stream << (long) io->amount; 
//			info->amount[j] = io->amount;
            *stream << (long) io->value; 
//			info->value[j] = io->value;
            switch(io->type)
            {
            case INVOBJ_BLADE:
                *stream << (long) ((InvBlade *)io->extra)->toHit; 
//				info->f1[j] = ((InvBlade *)io->extra)->toHit;
                *stream << (long) ((InvBlade *)io->extra)->damageDone; 
//				info->f2[j] = ((InvBlade *)io->extra)->damageDone;

//				sprintf(smlText,"%d\n",((InvBlade *)io->extra)->numOfHits);
//				LogOutput("bladeAge.txt", smlText);


                break;
            case INVOBJ_SKILL:
                *stream << (unsigned long) ((InvSkill *)io->extra)->skillLevel; 
//				info->f1[j] = ((InvSkill *)io->extra)->skillLevel;
                *stream << (unsigned long) ((InvSkill *)io->extra)->skillPoints; 
//				info->f2[j] = ((InvSkill *)io->extra)->skillPoints;
                break;
            case INVOBJ_POTION:
                *stream << ((InvPotion *)io->extra)->type; 
                *stream << ((InvPotion *)io->extra)->subType; 
                break;
            case INVOBJ_TOTEM:
                if ( ((InvTotem *)io->extra)->type >= TOTEM_PHYSICAL &&
                      ((InvTotem *)io->extra)->type <= TOTEM_CREATIVE)
                    *stream << (float) (((InvTotem *)io->extra)->imbueDeviation * -1); 
                else
                    *stream << (float) ((InvTotem *)io->extra)->imbueDeviation; 
//				info->f1[j] = ((InvTotem *)io->extra)->imbueDeviation;
                if (((InvTotem *)io->extra)->isActivated)
                {
                    LongTime ltNow;
                    *stream << (long) ((InvTotem *)io->extra)->timeToDie.MinutesDifference(&ltNow);
//					info->f2[j] = ((InvTotem *)io->extra)->timeToDie.MinutesDifference(&ltNow);
                }
                else
                    *stream << (long) 0;
//					info->f2[j] = 0;
                break;
            case INVOBJ_STAFF:
                *stream << (float) ((InvStaff *)io->extra)->imbueDeviation; 
                if (((InvStaff *)io->extra)->isActivated)
                {
                    *stream << (int) ((InvStaff *)io->extra)->charges;
                }
                else
                    *stream << (int) -1000;
                break;
            }
        }
        else
        {
            *stream << (char) -1; 
//			info->text[j][0] = 0; // blank means no object in this slot.
        }
        if (io)
            io = (InventoryObject *) inv->objects.Next();
    }

    *stream << (long) inv; 
//	info->invPtr = (long)inv;
    *stream << (long) avatarCash; 
//	info->money = avatarCash;
    *stream << (int) *(indexPtr); 
//	info->offset = *(indexPtr);

    lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);

    delete stream;

}
/*
//*******************************************************************************
void BBOServer::TellClientAboutInventory(BBOSAvatar *avatar, int type)
{

    MessInventoryInfo infoBase;
    MessInventoryInfo *info = &infoBase;
    Inventory *inv;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);

    int *indexPtr;

    long avatarCash = 0;
    if (avatar)
        avatarCash = avatar->charInfoArray[avatar->curCharacterIndex].inventory->money;

    switch(type)
    {
    default:
    case MESS_INVENTORY_PLAYER:
        inv = &(avatar->charInfoArray[avatar->curCharacterIndex].inventory);
        info->isPlayerInfo = TRUE;
        info->traderType = MESS_INVENTORY_PLAYER;
        indexPtr = &avatar->indexList[MESS_INVENTORY_PLAYER];
        avatar->lastPlayerInvSent = type;

        if (avatar->controlledMonster)
        {
            SharedSpace *sx;

            BBOSMonster * theMonster = FindMonster(
                      avatar->controlledMonster, &sx);
            if (theMonster)
            {
                inv = &(theMonster->inventory);
            }
        }


        break;

    case MESS_WORKBENCH_PLAYER:
        inv = &(avatar->charInfoArray[avatar->curCharacterIndex].workbench);
        info->isPlayerInfo = TRUE;
        info->traderType = MESS_WORKBENCH_PLAYER;
        indexPtr = &avatar->indexList[MESS_WORKBENCH_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    case MESS_SKILLS_PLAYER:
        inv = &(avatar->charInfoArray[avatar->curCharacterIndex].skills);
        info->isPlayerInfo = TRUE;
        info->traderType = MESS_SKILLS_PLAYER;
        indexPtr = &avatar->indexList[MESS_SKILLS_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    case MESS_WIELD_PLAYER:
        inv = &(avatar->charInfoArray[avatar->curCharacterIndex].wield);
        info->isPlayerInfo = TRUE;
        info->traderType = MESS_WIELD_PLAYER;
        indexPtr = &avatar->indexList[MESS_WIELD_PLAYER];
        avatar->lastPlayerInvSent = type;
        break;

    case MESS_INVENTORY_TRADER:
        inv = avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;
        info->isPlayerInfo = FALSE;
        info->traderType = MESS_INVENTORY_TRADER;
        indexPtr = &avatar->indexList[MESS_INVENTORY_TRADER];
        break;

    }

    if (!inv)
        inv = &(avatar->charInfoArray[avatar->curCharacterIndex].inventory);

    int i;

    InventoryObject *io = (InventoryObject *) inv->objects.First();
    for (i = 0; io; ++i)
        io = (InventoryObject *) inv->objects.Next();

    int numOfItems = i;
    if (*(indexPtr) > numOfItems - 9)
        *(indexPtr) = numOfItems - 9;
    if (*(indexPtr) < 0)
        *(indexPtr) = 0;

    io = (InventoryObject *) inv->objects.First();
    for (i = 0; i < *(indexPtr); ++i)
        io = (InventoryObject *) inv->objects.Next();

    for (int j = 0; j < 10; ++j)
    {
        if (io)
        {
            memcpy(info->text[j],io->WhoAmI(),31);
            info->text[j][31] = 0; // blank means no object in this slot.
//			info->status[j] = io->status;
            info->type[j] = io->type;
            info->ptr[j] = (long)io;
            info->amount[j] = io->amount;
            info->value[j] = io->value;
            switch(io->type)
            {
            case INVOBJ_BLADE:
                info->f1[j] = ((InvBlade *)io->extra)->toHit;
                info->f2[j] = ((InvBlade *)io->extra)->damageDone;
                break;
            case INVOBJ_SKILL:
                info->f1[j] = ((InvSkill *)io->extra)->skillLevel;
                info->f2[j] = ((InvSkill *)io->extra)->skillPoints;
                break;
            case INVOBJ_TOTEM:
                info->f1[j] = ((InvTotem *)io->extra)->imbueDeviation;
                if (((InvTotem *)io->extra)->isActivated)
                {
                    LongTime ltNow;
                    info->f2[j] = ((InvTotem *)io->extra)->timeToDie.MinutesDifference(&ltNow);
                }
                else
                    info->f2[j] = 0;
                break;
            }
        }
        else
        {
            info->text[j][0] = 0; // blank means no object in this slot.
        }
        if (io)
            io = (InventoryObject *) inv->objects.Next();
    }

    info->invPtr = (long)inv;
    info->money = avatarCash;
//	info->moneyDelta = inv->moneyDelta;
    info->offset = *(indexPtr);

    lserver->SendMsg(sizeof(infoBase),(void *)info, 0, &tempReceiptList);


}
*/
//*******************************************************************************
int BBOServer::TransferItem(BBOSAvatar *avatar, 
                                     MessInventoryTransferRequest *transferRequestPtr,
                                     long amount, int isGiving)
{
    char tempText[1024], dateString[128], timeString[128];
    MessInfoText infoText;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);

    SharedSpace *ss = NULL;
    FindAvatar(avatar->socketIndex, &ss);

    avatar->StateNoAgreement(ss);

    InventoryObject *io2;
    MessUnWield messUnWield;

    _strdate( dateString );
    _strtime( timeString );

    Inventory *inv = 
            (avatar->charInfoArray[avatar->curCharacterIndex].inventory);

    if (avatar->controlledMonster)
    {
        SharedSpace *sx;

        BBOSMonster * theMonster = FindMonster(
                  avatar->controlledMonster, &sx);
        if (theMonster)
        {
            inv = (theMonster->inventory);
        }
    }

    Inventory *partner = 
            avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner;

    SharedSpace *sx;
    BBOSAvatar *partnerAvatar = NULL;
    if (avatar->tradingPartner)
    {
        partnerAvatar = FindAvatar(avatar->tradingPartner, &sx);
        if (transferRequestPtr->ptr && partnerAvatar && partnerAvatar->tradingPartner == avatar)
        {

            if (transferRequestPtr->isPlayerInfo)
            {
                InventoryObject *io = (InventoryObject *) inv->objects.Find(
                                        (InventoryObject *) transferRequestPtr->ptr);
                if (io) // if found the object to transfer
                {
                    // 255 mean transfer all
                    if (255 == amount)
                        amount = io->amount;

                    if (io->amount < amount)
                        amount = io->amount;

                    TransferAmount(io, inv, avatar->trade, amount);

                    // tell other player
                    TellClientAboutInventory(avatar, MESS_INVENTORY_YOUR_SECURE);
                    TellClientAboutInventory(partnerAvatar, MESS_INVENTORY_HER_SECURE);

                    return 2;
                }
            }
            else
            {
                InventoryObject *io = (InventoryObject *) avatar->trade->objects.Find(
                                        (InventoryObject *) transferRequestPtr->ptr);
                if (io) // if found the object to transfer
                {
                    // 255 mean transfer all
                    if (255 == amount)
                        amount = io->amount;

                    if (io->amount < amount)
                        amount = io->amount;

                    TransferAmount(io, avatar->trade, inv, amount);

                    // tell other player
                    TellClientAboutInventory(avatar, MESS_INVENTORY_YOUR_SECURE);
                    TellClientAboutInventory(partnerAvatar, MESS_INVENTORY_HER_SECURE);

                    return 2;
                }
            }
        }
        return 0;
    }

    if (partner)
    {
        // from trainer to your skill inventory ************
        if (MESS_INVENTORY_TRAINER == partner->subType && 
             partner == (Inventory *) transferRequestPtr->partner &&
             transferRequestPtr->ptr)
        {
            // no longer item inv, now it's skills inv
            inv = (avatar->charInfoArray[avatar->curCharacterIndex].skills);

            // can't buy more than one instance of a skill
            amount = 1;

            InventoryObject *io = (InventoryObject *) partner->objects.Find(
                                    (InventoryObject *) transferRequestPtr->ptr);
            if (io) // if found the object to transfer
            {
                // if there's already an object by that name
                io2 = (InventoryObject *) inv->objects.Find(io->WhoAmI());
                if (io2)
                {
                    sprintf(tempText,"You already know that skill.");
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                    infoText.text[MESSINFOTEXTLEN-1] = 0;
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 0;
                }
                else if (io->value * amount <= 
                          avatar->charInfoArray[avatar->curCharacterIndex].inventory->money)
                {
                    if( avatar->charInfoArray[avatar->curCharacterIndex].magical < 10 && !strcmp( io->do_name,"Totem Shatter" ) )
                    {
                        sprintf(tempText,"You must have a magic of at least 10 to use this skill.");
                        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                        infoText.text[MESSINFOTEXTLEN-1] = 0;

                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        avatar->charInfoArray[avatar->curCharacterIndex].inventory->money -= io->value * amount;
                        sprintf(tempText,"You spent %dg to learn %4s.",
                                (int)io->value, io->WhoAmI());
                        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                        infoText.text[MESSINFOTEXTLEN-1] = 0;

                        TransferAmount(io, partner, inv, 1);

                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }

                    return 0;
                }
                else
                {
                    sprintf(tempText,"You don't have %dg for %4s.",
                             (int)io->value, io->WhoAmI());
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                    infoText.text[MESSINFOTEXTLEN-1] = 0;
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 0;
                }
            }
        }
        // from your item inventory to non-trainer ************
        else if (MESS_INVENTORY_TRAINER != partner->subType && 
                  transferRequestPtr->isPlayerInfo &&
                  transferRequestPtr->ptr)
        {
            InventoryObject *io = (InventoryObject *) inv->objects.Find(
                                    (InventoryObject *) transferRequestPtr->ptr);
            if (io) // if found the object to transfer
            {
                // 255 mean transfer all
                if (255 == amount)
                    amount = io->amount;

                if (io->amount < amount)
                    amount = io->amount;

                int all = FALSE;
                if (amount == io->amount)
                    all = TRUE;

                int isInFingle = FALSE;
                if (abs(townList[2].x - avatar->cellX) <= 5 && 
                     abs(townList[2].y - avatar->cellY) <= 5)
                {
                    isInFingle = TRUE;
                }
                
                if( partner->subType == MESS_INVENTORY_TRADER ) {
                    sprintf(tempText,"DROP-SELL, %d, %s, %s, %s, %s, %s, Merchant, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  avatar->cellX, avatar->cellY);
                        LogOutput("tradelog.txt", tempText);
                }
                else {
                    sprintf(tempText,"DROP-SELL, %d, %s, %s, %s, %s, %s, %s, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), avatar->cellX, avatar->cellY);
                        LogOutput("groundtradelog.txt", tempText);
                }

                if (INVOBJ_BLADE == io->type)
                {
                    sprintf(tempText,"DROP-SELL, %d, %s, %s, %s, %s, %s, %s, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), avatar->cellX, avatar->cellY);
                        LogOutput("swordTransferLog.txt", tempText);
                }

                if (INVOBJ_BLADE == io->type && ((InvBlade *)io->extra)->isWielded)
                {
                    messUnWield.bladeID = (long)avatar->socketIndex;
                    ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                        sizeof(messUnWield),(void *)&messUnWield);
                    ((InvBlade *)io->extra)->isWielded = FALSE;
                }

                if (isGiving)
                {
                    if (MESS_INVENTORY_TOWER == partner->subType)
                    {
                        sprintf(tempText,"INSERT, %d, %s, %s, %s, %s, %s, %s\n", amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI());
                        LogOutput("towerChestLog.txt", tempText);
                    }
                    TransferAmount(io, inv, partner, amount);

                    return 1;
                }
                else if (isInFingle && INVOBJ_EARTHKEY == io->type)
                {
                    sprintf(tempText,"We don't buy those here.  Please try another town.");
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                    infoText.text[MESSINFOTEXTLEN-1] = 0;
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 0;
                }
                else
                {
                    float value = io->value * amount * 7 / 10;
                    if (value < 1)
                        value = 1;
                    avatar->charInfoArray[avatar->curCharacterIndex].inventory->money += value;

                    sprintf(tempText,"You got %dg for %ld %4s.",
                             (int)value, amount, io->WhoAmI());
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);

                    infoText.text[MESSINFOTEXTLEN-1] = 0;

                    if (INVOBJ_MEAT == io->type)
                    {
                        SaltThisMeat(io);
                    }

                    TransferAmount(io, inv, partner, amount);

                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 1;
                }
            }

        }
        // from non-trainer to your item inventory ************
        else if (MESS_INVENTORY_TRAINER != partner->subType && 
                  partner == (Inventory *) transferRequestPtr->partner &&
                    transferRequestPtr->ptr)
        {
//			if (partner->
            InventoryObject *io = (InventoryObject *) partner->objects.Find(
                                    (InventoryObject *) transferRequestPtr->ptr);
            if (io) // if found the object to transfer
            {
                // 255 mean transfer all
                if (255 == amount)
                    amount = io->amount;

                if (io->amount < amount)
                    amount = io->amount;

                if( partner->subType == MESS_INVENTORY_TRADER ) {
                    sprintf(tempText,"BUY-GET, %d, %s, %s, %s, %s, %s, Merchant, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  avatar->cellX, avatar->cellY);
                        LogOutput("tradelog.txt", tempText);
                }
                else {
                    sprintf(tempText,"BUY-GET, %d, %s, %s, %s, %s, %s, %s, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), avatar->cellX, avatar->cellY);
                        LogOutput("groundtradelog.txt", tempText);
                }

                if (INVOBJ_BLADE == io->type)
                {
                    sprintf(tempText,"BUY-GET, %d, %s, %s, %s, %s, %s, %s, %d, %d\n", 
                                amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI(), avatar->cellX, avatar->cellY);
                        LogOutput("swordTransferLog.txt", tempText);
                }


                if (isGiving)
                {
                    if (MESS_INVENTORY_BANK != partner->subType)
                    {
                        if (amount < 2)
                            sprintf(tempText,"%s got a %s.",
                                avatar->charInfoArray[avatar->curCharacterIndex].name, io->WhoAmI());
                        else
                            sprintf(tempText,"%s got %d %ss.",
                                avatar->charInfoArray[avatar->curCharacterIndex].name, amount, io->WhoAmI());
                        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                        infoText.text[MESSINFOTEXTLEN-1] = 0;
                        ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                            sizeof(infoText),(void *)&infoText,1);
                    }
                    avatar->QuestPickupItem(ss, io);

                    if (MESS_INVENTORY_TOWER == partner->subType)
                    {
                        sprintf(tempText,"REMOVE, %d, %s, %s, %s, %s, %s, %s\n", amount, io->WhoAmI(),
                                 avatar->charInfoArray[avatar->curCharacterIndex].name,
                                 avatar->name, 
                                  dateString, timeString,
                                  ss->WhoAmI());
                        LogOutput("towerChestLog.txt", tempText);
                    }

                    TransferAmount(io, partner, inv, amount);

                    // save in 15 seconds
                    avatar->lastSaveTime = timeGetTime() - 1000 * 60 * 5 + 1000 * 15;
                    return 1;
                }
                else if (io->value * amount <= 
                          avatar->charInfoArray[avatar->curCharacterIndex].inventory->money)
                {
                    avatar->charInfoArray[avatar->curCharacterIndex].inventory->money -= io->value * amount;
                    sprintf(tempText,"You spent %dg for the %ld %4s.",
                             (int)io->value * amount, amount, io->WhoAmI());
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                    infoText.text[MESSINFOTEXTLEN-1] = 0;

                    TransferAmount(io, partner, inv, amount);

                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 1;
                }
                else
                {
                    sprintf(tempText,"You don't have %dg for the %ld %4s.",
                             (int)io->value * amount, amount, io->WhoAmI());
                    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                    infoText.text[MESSINFOTEXTLEN-1] = 0;
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    return 0;
                }
            }

        }
    }
    return 0;

}


//*******************************************************************************
int BBOServer::ShiftItem(BBOSAvatar *avatar, MessInventoryChange *inventoryChangePtr)
{
    if (!inventoryChangePtr->ptr)
        return FALSE;

    char tempText[1024];
    MessInfoText infoText;
    MessUnWield messUnWield;

    int amount = inventoryChangePtr->amount;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);

    SharedSpace *ss = NULL;
    FindAvatar(avatar->socketIndex, &ss);

    Inventory *src, *dst;
    switch(inventoryChangePtr->srcListType)
    {
    case GTM_BUTTON_LIST_INV:
    default:
        src = (avatar->charInfoArray[avatar->curCharacterIndex].inventory);
        break;
    case GTM_BUTTON_LIST_WRK:
        src = (avatar->charInfoArray[avatar->curCharacterIndex].workbench);
        break;
    case GTM_BUTTON_LIST_WLD:
        src = (avatar->charInfoArray[avatar->curCharacterIndex].wield);
        break;
    }

    switch(inventoryChangePtr->dstListType)
    {
    case GTM_BUTTON_LIST_INV:
        dst = (avatar->charInfoArray[avatar->curCharacterIndex].inventory);
        break;
    case GTM_BUTTON_LIST_WRK:
        dst = (avatar->charInfoArray[avatar->curCharacterIndex].workbench);
        break;
    case GTM_BUTTON_LIST_WLD:
    default:
        dst = (avatar->charInfoArray[avatar->curCharacterIndex].wield);
        break;
    }

    InventoryObject *io = (InventoryObject *) src->objects.Find(
                                    (InventoryObject *) inventoryChangePtr->ptr);
    if (io) // if found the object to transfer
    {
        // 255 mean transfer all
        if (255 == amount)
            amount = io->amount;

        if (io->amount < amount)
            amount = io->amount;
        if (amount < 1)
            amount = 1;

        if (GTM_BUTTON_LIST_WLD == inventoryChangePtr->dstListType)
        {
            sprintf(tempText,"You wielded %d %4s.", amount, io->WhoAmI());
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        }
        else if (GTM_BUTTON_LIST_WLD == inventoryChangePtr->srcListType)
        {
            sprintf(tempText,"You removed %d %4s.", amount, io->WhoAmI());
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
/*
            if (INVOBJ_BLADE == io->type)
            {
                messUnWield.bladeID = (long)avatar->socketIndex;
                ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                    sizeof(messUnWield),(void *)&messUnWield);
                ((InvBlade *)io->extra)->isWielded = FALSE;
            }
*/
        }
        else
        {
            sprintf(tempText,"You transferred %d %4s.", amount, io->WhoAmI());
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        }
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        TransferAmount(io, src, dst, amount);

        if (GTM_BUTTON_LIST_WLD == inventoryChangePtr->dstListType ||
             GTM_BUTTON_LIST_WLD == inventoryChangePtr->srcListType)
        {
            InventoryObject *iObject = (InventoryObject *) 
                avatar->charInfoArray[avatar->curCharacterIndex].wield->objects.First();
            int hasBlade = FALSE;
            while (iObject)
            {
                if (INVOBJ_BLADE == iObject->type)
                {
                    MessBladeDesc messBladeDesc;
                    FillBladeDescMessage(&messBladeDesc, iObject, avatar);
                    ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                        sizeof(messBladeDesc),(void *)&messBladeDesc);
                    iObject = (InventoryObject *) 
                        avatar->charInfoArray[avatar->curCharacterIndex].wield->objects.Last();
                    hasBlade = TRUE;
                    
                }
                if (INVOBJ_STAFF == iObject->type)
                {
                    InvStaff *iStaff = (InvStaff *) iObject->extra;
                    
                    MessBladeDesc messBladeDesc;
                    messBladeDesc.bladeID = (long)iObject;
                    messBladeDesc.size    = 4;
                    messBladeDesc.r       = staffColor[iStaff->type][0];
                    messBladeDesc.g       = staffColor[iStaff->type][1];
                    messBladeDesc.b       = staffColor[iStaff->type][2];
                    messBladeDesc.avatarID= avatar->socketIndex;
                    messBladeDesc.trailType  = 0;
                    messBladeDesc.meshType = BLADE_TYPE_STAFF1;
                    ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                        sizeof(messBladeDesc),(void *)&messBladeDesc);
                    iObject = (InventoryObject *) 
                        avatar->charInfoArray[avatar->curCharacterIndex].wield->objects.Last();
                    hasBlade = TRUE;
                }

                iObject = (InventoryObject *) 
                    avatar->charInfoArray[avatar->curCharacterIndex].wield->objects.Next();
            }

            if (!hasBlade)
            {
                messUnWield.bladeID = (long)avatar->socketIndex;
                ss->SendToEveryoneNearBut(0, avatar->cellX, avatar->cellY,
                    sizeof(messUnWield),(void *)&messUnWield);
            }
        }


        return TRUE;
    }
    return FALSE;

}


//*******************************************************************************
void BBOServer::TransferAmount(InventoryObject *io, Inventory *inv,
                                         Inventory *partner,  long amount)
{
//	char tempText[1024];

    if (io->amount < amount)
        amount = io->amount;

    InventoryObject *io2 = new InventoryObject( INVOBJ_SIMPLE, 0, "LOADED" );

    io->CopyTo( io2 );

    io->amount -= amount;
    io2->amount = amount;

    partner->AddItemSorted(io2);
    
    if (io->amount < 1)
    {
        inv->objects.Remove(io);
        delete io;
    }

}


//*******************************************************************************
void BBOServer::SetWield(int isWielding, InventoryObject *iObject, Inventory *inventory)
{
    InvBlade *ib = (InvBlade *) iObject->extra;  // better be a blade!!!

    InventoryObject *io;

    if (!isWielding)
    {
        ib->isWielded = FALSE;
    }
    else
    {
        io = (InventoryObject *) inventory->objects.First();
        while (io)
        {
            if (INVOBJ_BLADE == io->type)
            {
                ib = (InvBlade *) io->extra;
                ib->isWielded = FALSE;
            }

            io = (InventoryObject *) inventory->objects.Next();
        }

        ib = (InvBlade *) iObject->extra;  // better be a blade!!!
        ib->isWielded = TRUE;
    }

}


//*******************************************************************************
void BBOServer::HandleCombine(BBOSAvatar *avatar, char *skillName)
{
    char tempText[1024];
    MessInfoText infoText;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);
/*
    sprintf(tempText,"Combining using %s.", skillName);
    memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
    infoText.text[MESSINFOTEXTLEN-1] = 0;
    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
*/

    if (!strcmp(skillName,"Swordsmith"))
    {
        sprintf(avatar->combineSkillName,"Swordsmith");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Katana Expertise"))
    {
        sprintf(avatar->combineSkillName,"Katana Expertise");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Mace Expertise"))
    {
        sprintf(avatar->combineSkillName,"Mace Expertise");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Bladestaff Expertise"))
    {
        sprintf(avatar->combineSkillName,"Bladestaff Expertise");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Claw Expertise"))
    {
        sprintf(avatar->combineSkillName,"Claw Expertise");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Chaos Expertise"))
    {
        sprintf(avatar->combineSkillName,"Chaos Expertise");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Weapon Dismantle"))
    {
        sprintf(avatar->combineSkillName,"Weapon Dismantle");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Geomancy"))
    {
        sprintf(avatar->combineSkillName,"Geomancy");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Totem Shatter"))
    {
        sprintf(avatar->combineSkillName,"Totem Shatter");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        sprintf(tempText,"You begin using %s.", skillName);
        memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
        infoText.text[MESSINFOTEXTLEN-1] = 0;
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else if (!strcmp(skillName,"Explosives"))
    {
        sprintf(avatar->combineSkillName,"Explosives");
        avatar->combineStartTime = timeGetTime();
        avatar->isCombining = TRUE;

        CopyStringSafely("You begin making explosives.",1000,infoText.text,MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else
    {
        int usedMagic = -1;
        for (int i = 0; i < MAGIC_MAX; ++i)
        {
            if (!strnicmp(magicNameList[i],skillName, strlen(magicNameList[i])))
                usedMagic = i;
        }

        if (usedMagic > -1)
        {
            sprintf(avatar->combineSkillName,skillName);
            avatar->combineStartTime = timeGetTime();
            avatar->isCombining = TRUE;

            sprintf(tempText,"You begin using %s.", skillName);
            memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
            infoText.text[MESSINFOTEXTLEN-1] = 0;
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
        else
        {
            sprintf(tempText,"You can't combine with %s.", skillName);
            memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
            infoText.text[MESSINFOTEXTLEN-1] = 0;
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
    }
}


//*******************************************************************************
void BBOServer::TryToMoveAvatar(int fromSocket, MessAvatarMoveRequest *AvMoveReqPtr)
{
    SharedSpace *ss;
    BBOSAvatar *curAvatar;
    int tempX, tempY;
    MessBladeDesc messBladeDesc;
    MessAvatarMove AvMove;
    MessInfoText infoText;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(fromSocket);
    char tempText[128];

    switch(AvMoveReqPtr->x)
    {
    case 0:
        tempX = 0;
        tempY = -1;
        break;
    case 1:
        tempX = 1;
        tempY = -1;
        break;
    case 2:
        tempX = 1;
        tempY = 0;
        break;
    case 3:
        tempX = 1;
        tempY = 1;
        break;
    case 4:
        tempX = 0;
        tempY = 1;
        break;
    case 5:
        tempX = -1;
        tempY = 1;
        break;
    case 6:
        tempX = -1;
        tempY = 0;
        break;
    case 7:
        tempX = -1;
        tempY = -1;
        break;
    }
    
    curAvatar = FindAvatar(fromSocket, &ss);
    if (curAvatar)
    {

        if (curAvatar->magicEffectAmount[MONSTER_EFFECT_BIND] > 0)
        {
            MessInfoText infoText;
            sprintf(tempText,"You are bound, and cannot move!");
            memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
            infoText.text[MESSINFOTEXTLEN-1] = 0;
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }

        if (curAvatar->controlledMonster)
        {
            SharedSpace *sx;

            BBOSMonster * theMonster = FindMonster(
                     curAvatar->controlledMonster, &sx);
            if (theMonster)
            {
                curAvatar->MoveControlledMonster(sx, tempX, tempY);
            }
            return;
        }

        curAvatar->activeCounter = 0;
        // if not going out of bounds...
        if (tempX + curAvatar->cellX < MAP_SIZE_WIDTH &&
            tempX + curAvatar->cellX >= 0 &&
            tempY + curAvatar->cellY < MAP_SIZE_HEIGHT &&
            tempY + curAvatar->cellY >= 0 &&
             !curAvatar->isMoving)
        {

            // REALM OF PAIN TEMP CODE
            if( tempX + curAvatar->cellX == 161 && tempY + curAvatar->cellY == 121 ) {
                MessInfoText infoText;
                sprintf( tempText,"The doorway to the Realm of Pain is sealed shut. You cannot open it now." );
                memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                infoText.text[MESSINFOTEXTLEN-1] = 0;
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            if (SPACE_GROUND == ss->WhatAmI())
            {
                // stepping into a dungeon?
                SharedSpace *sp = (SharedSpace *) spaceList->First();
                while (sp)
                {
                    if (SPACE_DUNGEON == sp->WhatAmI() && 
                         !(((DungeonMap *) sp)->specialFlags & SPECIAL_DUNGEON_TEMPORARY))
                    {
                        if (((DungeonMap *) sp)->enterX == tempX + curAvatar->cellX &&
                             ((DungeonMap *) sp)->enterY == tempY + curAvatar->cellY)
                        {
                            int lck = FALSE;
                            if (((DungeonMap *)sp)->isLocked &&
                                 !(ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                                    ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                                    ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType))
                                return;

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;

                            // tell everyone I'm dissappearing
                            curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                            // tell my client I'm entering the dungeon
                            MessChangeMap changeMap;
                            changeMap.dungeonID = (long) sp;
                            changeMap.oldType = ss->WhatAmI();
                            changeMap.newType = sp->WhatAmI();
                            changeMap.sizeX   = ((DungeonMap *) sp)->width;
                            changeMap.sizeY   = ((DungeonMap *) sp)->height;
                            changeMap.flags   = MESS_CHANGE_NOTHING;

                            if	(((DungeonMap *) sp)->CanEdit(curAvatar))
                                changeMap.flags   = MESS_CHANGE_EDITING;

                            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                            MessInfoText infoText;
                            sprintf(tempText,"You enter the %s.", ((DungeonMap *) sp)->name);
                            memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                            infoText.text[MESSINFOTEXTLEN-1] = 0;
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            curAvatar->QuestSpaceChange(ss,sp);

                            // move me to my new SharedSpace
                            ss->avatars->Remove(curAvatar);
                            sp->avatars->Append(curAvatar);

                            curAvatar->cellX = ((DungeonMap *) sp)->width-1;
                            curAvatar->cellY = ((DungeonMap *) sp)->height-1;

                            // tell everyone about my arrival
                            curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                            // tell this player about everyone else around
                            curAvatar->UpdateClient(sp, TRUE);
                            return;
                        }
                    }
                    else if (SPACE_GUILD == sp->WhatAmI())
                    {
                        if (((TowerMap *) sp)->enterX  == tempX + curAvatar->cellX &&
                             ((TowerMap *) sp)->enterY == tempY + curAvatar->cellY)
                        {
                            if (! ((TowerMap *) sp)->IsMember(
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name) &&
                                 ACCOUNT_TYPE_ADMIN != curAvatar->accountType)
                            {
                                MessInfoText infoText;
                                sprintf(tempText,"You cannot enter the guild tower.");
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                sprintf(tempText,"You are not in the %s.", ((TowerMap *) sp)->WhoAmI());
                                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                return;
                            }

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;

                            // tell everyone I'm dissappearing
                            curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                            // tell my client I'm entering the dungeon
                            MessChangeMap changeMap;
                            changeMap.dungeonID = (long) sp;
                            changeMap.oldType = ss->WhatAmI();
                            changeMap.newType = sp->WhatAmI();
                            changeMap.sizeX   = 5;
                            changeMap.sizeY   = 5;
                            changeMap.flags   = MESS_CHANGE_NOTHING;
                            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                            MessInfoText infoText;
                            sprintf(tempText,"You enter the guild tower of %s.", ((TowerMap *) sp)->WhoAmI());
                            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            curAvatar->QuestSpaceChange(ss,sp);

                            // move me to my new SharedSpace
                            ss->avatars->Remove(curAvatar);
                            sp->avatars->Append(curAvatar);

                            ((TowerMap *)sp)->lastChangedTime.SetToNow();

                            curAvatar->cellX = 4;
                            curAvatar->cellY = 4;

                            // tell everyone about my arrival
                            curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                            // tell this player about everyone else around
                            curAvatar->UpdateClient(sp, TRUE);
                            return;
                        }
                    }
                    sp = (SharedSpace *) spaceList->Next();
                }

            }
            
            if (SPACE_DUNGEON == ss->WhatAmI())
            {
                // stepping in a locked dungeon?
                if (((DungeonMap *)ss)->isLocked &&
                     !(ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                        ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                        ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType))
                {
                    // tell everyone I'm dissappearing
                    curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                    // tell my client I'm leaving the dungeon
                    MessChangeMap changeMap;
                    changeMap.oldType = ss->WhatAmI();
                    changeMap.newType = SPACE_GROUND;
                    changeMap.sizeX   = MAP_SIZE_WIDTH;
                    changeMap.sizeY   = MAP_SIZE_HEIGHT;
                    lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                    SharedSpace *sp = (SharedSpace *) spaceList->Find(SPACE_GROUND);

                    // move me to my new SharedSpace
                    ss->avatars->Remove(curAvatar);
                    sp->avatars->Append(curAvatar);

                    curAvatar->cellX = ((DungeonMap *) ss)->enterX;
                    curAvatar->cellY = ((DungeonMap *) ss)->enterY - 1;

                    // tell everyone about my arrival
                    curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                    // tell this player about everyone else around
                    curAvatar->UpdateClient(sp, TRUE);
                    return;
                }
            }
            /*
            else if (SPACE_GUILD == ss->WhatAmI())
            {
                // stepping into the exit square of a tower?
                if (0 == tempX + curAvatar->cellX &&
                     0 == tempY + curAvatar->cellY)
                {
                    // tell everyone I'm dissappearing
                    curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                    // tell my client I'm leaving the dungeon
                    MessChangeMap changeMap;
                    changeMap.oldType = ss->WhatAmI();
                    changeMap.newType = SPACE_GROUND;
                changeMap.sizeX   = MAP_SIZE_WIDTH;
                changeMap.sizeY   = MAP_SIZE_HEIGHT;
                    lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                    SharedSpace *sp = (SharedSpace *) spaceList->Find(SPACE_GROUND);

                    // move me to my new SharedSpace
                    ss->avatars->Remove(curAvatar);
                    sp->avatars->Append(curAvatar);

                    curAvatar->cellX = ((TowerMap *) ss)->enterX;
                    curAvatar->cellY = ((TowerMap *) ss)->enterY - 1;

                    // tell everyone about my arrival
                    curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                    // tell this player about everyone else around
                    curAvatar->UpdateClient(sp, TRUE);
                    return;
                }
            }
            */

            if (ss->CanMove(curAvatar->cellX,         curAvatar->cellY, 
                              tempX + curAvatar->cellX, tempY + curAvatar->cellY) &&
                 ss->CanMove(curAvatar->cellX, curAvatar->cellY, 
                              0 + curAvatar->cellX, tempY + curAvatar->cellY) &&
                 ss->CanMove(curAvatar->cellX, curAvatar->cellY, 
                              tempX + curAvatar->cellX, 0 + curAvatar->cellY)  )
            {
                // can't move from a static dungeon monster
                if (SPACE_DUNGEON == ss->WhatAmI())
                {
                    if (!ss->CanMove(tempX + curAvatar->cellX,  curAvatar->cellY, 
                              tempX + curAvatar->cellX,        tempY + curAvatar->cellY) ||
                         !ss->CanMove(curAvatar->cellX, tempY + curAvatar->cellY, 
                              tempX + curAvatar->cellX, tempY + curAvatar->cellY))
                        return;

                    if ( ! ((DungeonMap *) ss)->CanEdit(curAvatar) )
                    {
                        BBOSMob *curMob = (BBOSMob *) ss->mobList->GetFirst(curAvatar->cellX, curAvatar->cellY);
                        while (curMob)
                        {
                            if (SMOB_MONSTER == curMob->WhatAmI())
//								&&
//								 !((BBOSMonster *) curMob)->uniqueName[0])
                            {
                                if (!((BBOSMonster *)curMob)->isWandering &&
                                     !((BBOSMonster *)curMob)->isPossessed)
                                {
                                    if (curMob->cellX == curAvatar->cellX &&
                                         curMob->cellY == curAvatar->cellY)
                                    {
                                        sprintf(infoText.text,"You are magically held to this spot!");
                                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                        return;
                                    }
                                }
                            }

                            curMob = (BBOSMob *) ss->mobList->GetNext();
                        }
                    }
                }

                BBOSMob *curMob = (BBOSMob *) ss->mobList->GetFirst(
                                      tempX + curAvatar->cellX, tempY + curAvatar->cellY);
                while (curMob)
                {
                    if (SMOB_WARP_POINT == curMob->WhatAmI())
                    {
                        if ((((BBOSWarpPoint *)curMob)->allCanUse ||
                              ACCOUNT_TYPE_ADMIN == curAvatar->accountType) &&
                             ((BBOSWarpPoint *)curMob)->spaceType < 100)
                        {
                            // tell everyone I'm dissappearing
                            curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                            SharedSpace *sp = (SharedSpace *) spaceList->First();
                            while (sp)
                            {
                                if (((BBOSWarpPoint *)curMob)->spaceType == sp->WhatAmI())
                                {
                                    if ((SPACE_REALM == sp->WhatAmI() && 
                                          ((BBOSWarpPoint *)curMob)->spaceSubType == ((RealmMap *)sp)->type)
                                        || (SPACE_LABYRINTH == sp->WhatAmI() && 
                                          ((BBOSWarpPoint *)curMob)->spaceSubType == ((LabyrinthMap *)sp)->type)
                                         || (SPACE_REALM != sp->WhatAmI() && SPACE_LABYRINTH != sp->WhatAmI())
                                        )
                                    {
                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;

                                        // tell my client I'm leaving the dungeon
                                        MessChangeMap changeMap;
                                        changeMap.oldType = ss->WhatAmI();
                                        changeMap.newType = ((BBOSWarpPoint *)curMob)->spaceType;
                                        changeMap.realmID = ((BBOSWarpPoint *)curMob)->spaceSubType;
                                        changeMap.sizeX   = sp->sizeX;
                                        changeMap.sizeY   = sp->sizeY;

                                        lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                                        curAvatar->QuestSpaceChange(ss,sp);

                                        // move me to my new SharedSpace
                                        ss->avatars->Remove(curAvatar);
                                        sp->avatars->Append(curAvatar);

                                        curAvatar->cellX = ((BBOSWarpPoint *)curMob)->targetX;
                                        curAvatar->cellY = ((BBOSWarpPoint *)curMob)->targetY;

                                        // tell everyone about my arrival
                                        curAvatar->IntroduceMyself(sp, SPECIAL_APP_DUNGEON);

                                        // tell this player about everyone else around
                                        curAvatar->UpdateClient(sp, TRUE);
                                        return;
                                    }
                                }
                                sp = (SharedSpace *) spaceList->Next();
                            }
                        }
                    }

                    curMob = (BBOSMob *) ss->mobList->GetNext();
                }

                curAvatar->isMoving = TRUE;
                curAvatar->targetCellX = tempX + curAvatar->cellX;
                curAvatar->targetCellY = tempY + curAvatar->cellY;
                curAvatar->moveStartTime = timeGetTime();
                curAvatar->moveTimeCost  = 1000 * 3; // normally a move takes 3 seconds
                if (tempX != 0 && tempY != 0)
                    curAvatar->moveTimeCost  = 4000; // diagonal moves take 4.5 seconds
                AvMove.avatarID = fromSocket;
                AvMove.x = curAvatar->cellX;
                AvMove.y = curAvatar->cellY;
                AvMove.targetX = curAvatar->targetCellX;
                AvMove.targetY = curAvatar->targetCellY;
                if (curAvatar->isInvisible)
                    lserver->SendMsg(sizeof(AvMove),(void *)&AvMove, 
                                          0, &tempReceiptList);
                else
                    ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                               sizeof(AvMove),(void *)&AvMove);

                if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner)
                {
                    SharedSpace *guildSpace;
                    if (FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                    {
                        if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                                inventory->partner ==
                             (((TowerMap *)guildSpace)->itemBox))
                             dungeonUpdateTime = 0; // force dungeons to save

                    }
                }
                curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->partner = NULL;
            }

        }
    }
}

//*******************************************************************************
void BBOServer::AddDungeonSorted(DungeonMap *dm)
{
    int dmCenterDist = abs(64 - dm->enterX) + abs(64 - dm->enterY);

    DungeonMap *curDM;
    SharedSpace *sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        if (SPACE_DUNGEON == sp->WhatAmI())
        {
            curDM = (DungeonMap *) sp;
            int curCenterDist = abs(64 - curDM->enterX) + abs(64 - curDM->enterY);
            if (curCenterDist > dmCenterDist)
            {
                spaceList->AddBefore(dm, curDM);
                return;
            }

        }
        sp = (SharedSpace *) spaceList->Next();
    }

    spaceList->Append(dm);

}

//*******************************************************************************
void BBOServer::HandleChatLine(int fromSocket, char *chatText)
{
    SharedSpace *ss, *sp;
    BBOSAvatar *curAvatar;
//	int tempX, tempY;
    MessBladeDesc messBladeDesc;
//	MessAvatarMove AvMove;
    MessPlayerChatLine chatMess;
    MessInfoText infoText;
    int linePoint, argPoint;
    FILE *source;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(fromSocket);
    char tempText[1028], tempText2[124], tempText3[124];
    char t[500], t2[100], t3[100];

    
    curAvatar = FindAvatar(fromSocket, &ss);
    if (curAvatar)
    {
        curAvatar->activeCounter = 0;
//		curAvatar = FindAvatar(fromSocket, &ss);
        if ('/' == chatText[0])
        {
            // process slash commands
            linePoint = 0;

            argPoint = NextWord(chatText,&linePoint);

            //***************************************
            if ( IsSame(&(chatText[argPoint]) , "/teleport"))
            {
                argPoint = NextWord(chatText,&linePoint);
                int targX,targY;
                int maxX = 0, maxY = 0;
                if (SPACE_GROUND == ss->WhatAmI())
                {
                    maxX = 256;
                    maxY = 256;
                }
                else if (SPACE_DUNGEON == ss->WhatAmI())
                {
                    maxX = ((DungeonMap *)ss)->width;
                    maxY = ((DungeonMap *)ss)->height;
                }
                else if (SPACE_GUILD == ss->WhatAmI())
                {
                    maxX = ((TowerMap *)ss)->width;
                    maxY = ((TowerMap *)ss)->height;
                }
                else if (SPACE_REALM == ss->WhatAmI())
                {
                    maxX = ((RealmMap *)ss)->width;
                    maxY = ((RealmMap *)ss)->height;
                }
                else if (SPACE_LABYRINTH == ss->WhatAmI())
                {
                    maxX = ((LabyrinthMap *)ss)->width;
                    maxY = ((LabyrinthMap *)ss)->height;
                }
                sscanf(&chatText[argPoint],"%d %d",&targY, &targX);
                targX = maxX - targX;
                targY = maxY - targY;
                if (targX >= 0 && targX < maxX && targY >= 0 && targY < maxY && 
                     (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                      ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                      ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                    )
                {
                    curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_ADMIN_TELEPORT);

                    curAvatar->cellX = curAvatar->targetCellX = targX;
                    curAvatar->cellY = curAvatar->targetCellY = targY;

                    // tell everyone about my arrival
                    curAvatar->IntroduceMyself(ss, SPECIAL_APP_ADMIN_TELEPORT);

                    // tell this player about everyone else around
                    curAvatar->UpdateClient(ss, TRUE);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/goto"))
            {
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv &&
                     (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                      ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                      ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                    )
                {
                    argPoint = linePoint = linePoint + len;

                    curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_ADMIN_TELEPORT);

                    // tell my client I'm entering the realm
                    MessChangeMap changeMap;
                    changeMap.flags   = 0;
                    if (SPACE_REALM == sp->WhatAmI())
                        changeMap.realmID = ((RealmMap *)sp)->type;
                    if (SPACE_LABYRINTH == sp->WhatAmI())
                        changeMap.realmID = ((LabyrinthMap *)sp)->type;
                   else if (SPACE_DUNGEON == sp->WhatAmI())
                    {
                        changeMap.dungeonID = (long) sp;
                        if (((DungeonMap *)sp)->specialFlags & SPECIAL_DUNGEON_TEMPORARY)
                            changeMap.flags   = MESS_CHANGE_TEMP;
                    }
                    changeMap.oldType = ss->WhatAmI(); 
                    changeMap.newType = sp->WhatAmI(); 
                    changeMap.sizeX   = sp->sizeX;
                    changeMap.sizeY   = sp->sizeY;

                    lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                    MessInfoText infoText;
                    sprintf(tempText,"You enter the %s.",sp->WhoAmI());
                    CopyStringSafely(tempText, 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    curAvatar->QuestSpaceChange(ss,sp);

                    // move me to my new SharedSpace
                    ss->avatars->Remove(curAvatar);
                    sp->avatars->Append(curAvatar);

                    curAvatar->cellX = curAvatar->targetCellX = targetAv->cellX;
                    curAvatar->cellY = curAvatar->targetCellY = targetAv->cellY;

                    // tell everyone about my arrival
                    curAvatar->IntroduceMyself(sp, SPECIAL_APP_ADMIN_TELEPORT);

                    // tell this player about everyone else around
                    curAvatar->UpdateClient(sp, TRUE);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/translate"))
            {
                argPoint = NextWord(chatText,&linePoint);
                int targX,targY;

                int maxX = 0, maxY = 0;
                if (SPACE_GROUND == ss->WhatAmI())
                {
                    maxX = 256;
                    maxY = 256;
                }
                else if (SPACE_DUNGEON == ss->WhatAmI())
                {
                    maxX = ((DungeonMap *)ss)->width;
                    maxY = ((DungeonMap *)ss)->height;
                }
                else if (SPACE_GUILD == ss->WhatAmI())
                {
                    maxX = ((TowerMap *)ss)->width;
                    maxY = ((TowerMap *)ss)->height;
                }
                else if (SPACE_REALM == ss->WhatAmI())
                {
                    maxX = ((RealmMap *)ss)->width;
                    maxY = ((RealmMap *)ss)->height;
                }
                else if (SPACE_LABYRINTH == ss->WhatAmI())
                {
                    maxX = ((LabyrinthMap *)ss)->width;
                    maxY = ((LabyrinthMap *)ss)->height;
                }

                sscanf(&chatText[argPoint],"%d %d",&targY, &targX);
                targX = maxX - targX;
                targY = maxY - targY;

                MessInfoText infoText;
                sprintf(tempText,"That's really %dx %dy.", targX, targY);
                CopyStringSafely(tempText, 
                                      200, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/realm"))
            {
                argPoint = NextWord(chatText,&linePoint);

                int realmID = REALM_ID_SPIRITS;
                if ('d' == chatText[argPoint])
                    realmID = REALM_ID_DEAD;
                if ('r' == chatText[argPoint])
                    realmID = REALM_ID_DRAGONS;
                if ('l' == chatText[argPoint])
                    realmID = REALM_ID_LAB1;
                if ('m' == chatText[argPoint])
                    realmID = REALM_ID_LAB2;


                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                     ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                     ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                {
                    RealmMap *rp = NULL;
                    SharedSpace *sp = (SharedSpace *) spaceList->First();
                    while (sp)
                    {
                        if (SPACE_REALM == sp->WhatAmI() && realmID == ((RealmMap *)sp)->type)
                        {
                            rp = (RealmMap *)sp;
                            sp = (SharedSpace *) spaceList->Last();
                        }
                        sp = (SharedSpace *) spaceList->Next();
                    }

                    if (rp)
                    {
                        curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_ADMIN_TELEPORT);

                        // tell my client I'm entering the realm
                        MessChangeMap changeMap;
                        changeMap.realmID = rp->type;
                        changeMap.oldType = ss->WhatAmI(); 
                        changeMap.newType = rp->WhatAmI(); 
                        changeMap.sizeX   = rp->sizeX;
                        changeMap.sizeY   = rp->sizeY;

                        lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                        MessInfoText infoText;
                        sprintf(tempText,"You enter the %s.",rp->WhoAmI());
                        CopyStringSafely(tempText, 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        curAvatar->QuestSpaceChange(ss,rp);

                        // move me to my new SharedSpace
                        ss->avatars->Remove(curAvatar);
                        rp->avatars->Append(curAvatar);

                        curAvatar->cellX = curAvatar->targetCellX = (rand() % 4) + 2;
                        curAvatar->cellY = curAvatar->targetCellY = (rand() % 4) + 2;

                        // tell everyone about my arrival
                        curAvatar->IntroduceMyself(rp, SPECIAL_APP_ADMIN_TELEPORT);

                        // tell this player about everyone else around
                        curAvatar->UpdateClient(rp, TRUE);
                    }
                    else
                    {
                        // enter labyrinth?

                        LabyrinthMap *lp;
                        sp = (SharedSpace *) spaceList->First();
                        while (sp)
                        {
                            if (SPACE_LABYRINTH == sp->WhatAmI() && realmID == ((RealmMap *)sp)->type)
                            {
                                lp = (LabyrinthMap *)sp;
                                sp = (SharedSpace *) spaceList->Last();
                            }
                            sp = (SharedSpace *) spaceList->Next();
                        }

                        if (lp)
                        {
                            curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_ADMIN_TELEPORT);

                            // tell my client I'm entering the realm
                            MessChangeMap changeMap;
                            changeMap.realmID = lp->type;
                            changeMap.oldType = ss->WhatAmI(); 
                            changeMap.newType = lp->WhatAmI(); 
                            changeMap.sizeX   = lp->sizeX;
                            changeMap.sizeY   = lp->sizeY;

                            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                            MessInfoText infoText;
                            sprintf(tempText,"You enter the %s.",lp->WhoAmI());
                            CopyStringSafely(tempText, 
                                                  200, infoText.text, MESSINFOTEXTLEN);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            curAvatar->QuestSpaceChange(ss,lp);

                            // move me to my new SharedSpace
                            ss->avatars->Remove(curAvatar);
                            lp->avatars->Append(curAvatar);

                            curAvatar->cellX = curAvatar->targetCellX = 1;
                            curAvatar->cellY = curAvatar->targetCellY = 3;
                            if (REALM_ID_LAB2 == realmID)
                            {
                                curAvatar->cellX = curAvatar->targetCellX = 53;
                                curAvatar->cellY = curAvatar->targetCellY = 41;
                            }

                            // tell everyone about my arrival
                            curAvatar->IntroduceMyself(lp, SPECIAL_APP_ADMIN_TELEPORT);

                            // tell this player about everyone else around
                            curAvatar->UpdateClient(lp, TRUE);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/egg"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    int targX,targY;
                    sscanf(&chatText[argPoint],"%d %d",&targX, &targY);

                    if (targX < 0 || targX >= 4 || targY < 0 || targY >= 7) 
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/egg <quality (0-3)> <type (0-6)>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {

                        Inventory *inv = NULL;
                        //= &curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                        InventoryObject *iObject = new InventoryObject(
                                    INVOBJ_EGG,0,dragonInfo[targX][targY].eggName);
                        iObject->mass = 1.0f;
                        iObject->value = 1000;

                        InvEgg *im = (InvEgg *) iObject->extra;
                        im->type   = targY;
                        im->quality = targX;

                        if (curAvatar->controlledMonster)
                        {
                            SharedSpace *sx;

                            BBOSMonster * theMonster = FindMonster(
                                      curAvatar->controlledMonster, &sx);
                            if (theMonster)
                                inv = (theMonster->inventory);
                        }
                        if (!inv)
                                inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                        inv->AddItemSorted(iObject);

                        TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/create"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/create <amount> <monetary value> <text name>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        int amount, value;
                        Inventory *inv = NULL;

                        sscanf(&chatText[argPoint],"%d",&amount);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&value);
                        argPoint = NextWord(chatText,&linePoint);

                        if (amount < 1)
                            amount = 1;
                        if (amount > 100000)
                            amount = 10000;
                        if (value < 0)
                            value = 0;
                        if (value > 1000000)
                            value = 1000000;

                        if (curAvatar->controlledMonster)
                        {
                            SharedSpace *sx;

                            BBOSMonster * theMonster = FindMonster(
                                      curAvatar->controlledMonster, &sx);
                            if (theMonster)
                                inv = (theMonster->inventory);
                        }
                        if (!inv)
                                inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                        InventoryObject *iObject = new InventoryObject(
                                    INVOBJ_SIMPLE,0,&chatText[argPoint]);
                        iObject->amount = amount;
                        iObject->value = value;
                        inv->AddItemSorted(iObject);

                        TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/dust"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/dust <green OR blue OR black OR white OR red OR gold OR silver>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        int type = -1;
                        if (IsSame(&chatText[argPoint], "green"))
                            type = INGR_GREEN_DUST;
                        if (IsSame(&chatText[argPoint], "blue"))
                            type = INGR_BLUE_DUST;
                        if (IsSame(&chatText[argPoint], "black"))
                            type = INGR_BLACK_DUST;
                        if (IsSame(&chatText[argPoint], "white"))
                            type = INGR_WHITE_DUST;
                        if (IsSame(&chatText[argPoint], "red"))
                            type = INGR_RED_DUST;
                        if (IsSame(&chatText[argPoint], "gold"))
                            type = INGR_GOLD_DUST;
                        if (IsSame(&chatText[argPoint], "silver"))
                            type = INGR_SILVER_DUST;

                        Inventory *inv = NULL;

                        if (type != -1)
                        {
                            if (curAvatar->controlledMonster)
                            {
                                SharedSpace *sx;

                                BBOSMonster * theMonster = FindMonster(
                                          curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                    inv = (theMonster->inventory);
                            }
                            if (!inv)
                                    inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                            InventoryObject *iObject = new InventoryObject(
                                        INVOBJ_INGREDIENT,0,dustNames[type]);
                            InvIngredient *exIn = (InvIngredient *)iObject->extra;
                            exIn->type     = type;
                            exIn->quality  = 1;

                            iObject->mass = 0.0f;
                            iObject->value = 1000;
                            iObject->amount = 1;
                            inv->AddItemSorted(iObject);

                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);

                            source = fopen("adminDustGeneration.txt","a");
                            /* Display operating system-style date and time. */
                            _strdate( tempText2 );
                            fprintf(source, "%s, ", tempText2 );
                            _strtime( tempText2 );
                            fprintf(source, "%s, ", tempText2 );
                            fprintf(source,"%s\n", dustNames[type]);
                            fclose(source);
                        }
                    }
                }
            }
            //***************************************
            else if( IsSame(&(chatText[argPoint]), "/totem")) {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/totem pumpkin only for now", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else {
                        int type = -1;
                        sscanf(&chatText[argPoint],"%d",&type);
                        type = Bracket(type,0,8);

                        Inventory *inv = NULL;

                        if (type != -1)
                        {
                            type += INGR_WHITE_SHARD;

                            if (curAvatar->controlledMonster)
                            {
                                SharedSpace *sx;

                                BBOSMonster * theMonster = FindMonster(
                                          curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                    inv = (theMonster->inventory);
                            }
                            if (!inv)
                                    inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                            // add pumpkin totem
                            InventoryObject *iObject = new InventoryObject(INVOBJ_TOTEM,0,"Unnamed Totem");
                            InvTotem *extra = (InvTotem *)iObject->extra;
                            extra->type     = 0;
                            extra->quality  = 17; // pumpkin

                            iObject->mass = 0.0f;
                            iObject->value = extra->quality * extra->quality * 14 + 1;
                            if (extra->quality > 12)
                                iObject->value = extra->quality * extra->quality * 14 + 1 + (extra->quality-12) * 1600;
                            iObject->amount = 1;
                            UpdateTotem(iObject);
                            inv->AddItemSorted(iObject);

                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/shard"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/shard <0-8>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        int type = -1;
                        sscanf(&chatText[argPoint],"%d",&type);
                        type = Bracket(type,0,8);

                        Inventory *inv = NULL;

                        if (type != -1)
                        {
                            type += INGR_WHITE_SHARD;

                            if (curAvatar->controlledMonster)
                            {
                                SharedSpace *sx;

                                BBOSMonster * theMonster = FindMonster(
                                          curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                    inv = (theMonster->inventory);
                            }
                            if (!inv)
                                    inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;

                            InventoryObject *iObject = new InventoryObject(
                                        INVOBJ_INGREDIENT,0,dustNames[type]);
                            InvIngredient *exIn = (InvIngredient *)iObject->extra;
                            exIn->type     = type;
                            exIn->quality  = 1;

                            iObject->mass = 0.0f;
                            iObject->value = 1000;
                            iObject->amount = 1;
                            inv->AddItemSorted(iObject);

                            TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);

                            source = fopen("adminDustGeneration.txt","a");
                            /* Display operating system-style date and time. */
                            _strdate( tempText2 );
                            fprintf(source, "%s, ", tempText2 );
                            _strtime( tempText2 );
                            fprintf(source, "%s, ", tempText2 );
                            fprintf(source,"%s\n", dustNames[type]);
                            fclose(source);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/testdungeon") && 
                       ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                argPoint = NextWord(chatText,&linePoint);

                // make the new space
                DungeonMap *dm = new DungeonMap(SPACE_DUNGEON,"Test Dungeon",lserver);
                dm->specialFlags = SPECIAL_DUNGEON_TEMPORARY;
                dm->InitNew(5,5,curAvatar->cellX, curAvatar->cellY, 0);
                sprintf(dm->name, "%s's %s %s", 
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                  rdNameAdjective[rand() % 6], rdNamePre [rand() % 7]);

                spaceList->Append(dm);
/*
                dm->topWall [dm->width+0] = 1;
                dm->leftWall[dm->width+0] = 0;
                dm->topWall [dm->width+1] = 1;
                dm->leftWall[dm->width+1] = 0;
                dm->topWall [dm->width+2] = 1;
                dm->leftWall[dm->width+2] = 0;
                dm->topWall [dm->width+3] = 0;
                dm->leftWall[dm->width+3] = 0;
  */
                int col = 0;

                // add static monsters
                for (int m = 0; m < (dm->height * dm->width) / 12;)
                {
                    int t  = rand() % NUM_OF_MONSTERS;
                    int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

                    if (monsterData[t][t2].name[0] && monsterData[t][t2].dungeonType >= col - 3 &&
                         monsterData[t][t2].dungeonType <= col)
                    {
                        int mx, my;
                        do
                        {
                            mx = rand() % (dm->width);
                            my = rand() % (dm->height);
                        } while (mx < 2 && my < 2);

                        BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
                        monster->cellX = mx;
                        monster->cellY = my;
                        monster->targetCellX = mx;
                        monster->targetCellY = my;
                        monster->spawnX = mx;
                        monster->spawnY = my;
                        dm->mobList->Add(monster);
                        ++m;
                    }
                }
                // add wandering monsters
                if (0 == col)
                    col = 1;

                for (int m = 0; m < (dm->height * dm->width) / 12;)
                {
                    int t, t2;
                    int done = FALSE;
                    while (!done)
                    {
                        t  = rand() % NUM_OF_MONSTERS;
                        t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

                        if (MONSTER_PLACE_DUNGEON & monsterData[t][t2].placementFlags)
                            done = TRUE;
                    }

                    if (monsterData[t][t2].name[0] && monsterData[t][t2].dungeonType >= col - 3 &&
                         monsterData[t][t2].dungeonType <= col - 1)
                    {
                        int mx, my;
                        do
                        {
                            mx = rand() % (dm->width);
                            my = rand() % (dm->height);
                        } while (mx < 4 && my < 4);

                        BBOSMonster *monster = new BBOSMonster(t,t2, NULL);
                        monster->isWandering = TRUE;
                        monster->cellX = mx;
                        monster->cellY = my;
                        monster->targetCellX = mx;
                        monster->targetCellY = my;
                        monster->spawnX = mx;
                        monster->spawnY = my;
                        dm->mobList->Add(monster);
                        ++m;
                    }
                }

                // add a portal out of it
                BBOSWarpPoint *wp = new BBOSWarpPoint(0,0);
                wp->targetX      = dm->enterX;
                wp->targetY      = dm->enterY-1;
                wp->spaceType    = SPACE_GROUND;
                wp->spaceSubType = 0;
                dm->mobList->Add(wp);

                // teleport inside

                // tell everyone I'm dissappearing
                curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                // tell my client I'm entering the dungeon
                MessChangeMap changeMap;
                changeMap.dungeonID = (long) dm;
                changeMap.oldType = ss->WhatAmI();
                changeMap.newType = dm->WhatAmI();
                changeMap.sizeX   = ((DungeonMap *) dm)->width;
                changeMap.sizeY   = ((DungeonMap *) dm)->height;
                changeMap.flags   = MESS_CHANGE_TEMP;

                lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                MessInfoText infoText;
                sprintf(tempText,"You enter the %s.", ((DungeonMap *) dm)->name);
                memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
                infoText.text[MESSINFOTEXTLEN-1] = 0;
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                curAvatar->QuestSpaceChange(ss,dm);

                // move me to my new SharedSpace
                ss->avatars->Remove(curAvatar);
                dm->avatars->Append(curAvatar);

                curAvatar->cellX = ((DungeonMap *) dm)->width-1;
                curAvatar->cellY = ((DungeonMap *) dm)->height-1;

                // tell everyone about my arrival
                curAvatar->IntroduceMyself(dm, SPECIAL_APP_DUNGEON);

                // tell this player about everyone else around
                curAvatar->UpdateClient(dm, TRUE);

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/birth"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    int type, subType, resist;
                    float change;
                    Inventory *inv = NULL;

                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/birth <type num> <subType num> <power multiplier> <resist 0-100>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        sscanf(&chatText[argPoint],"%d",&type);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&subType);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%f",&change);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&resist);

                        if (type < 0)
                            type = 0;
                        if (type >= NUM_OF_MONSTERS)
                            type = NUM_OF_MONSTERS-1;
                        if (subType < 0)
                            subType = 0;
                        if (subType >= NUM_OF_MONSTER_SUBTYPES)
                            subType = NUM_OF_MONSTER_SUBTYPES-1;

                        if (change < 0.1f)
                            change = 1;
                        if (change > 100)
                            change = 1;

                        if (resist < 0)
                            resist = 0;
                        if (resist > 100)
                            resist = 100;

                        if (monsterData[type][subType].name[0])
                        {
                            SharedSpace *sx;
                            BBOSMonster * theMonster = NULL;
                            int tX, tY;
                            tX = curAvatar->cellX;
                            tY = curAvatar->cellY;
                            if (curAvatar->controlledMonster)
                            {
                                theMonster = FindMonster(curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                {
                                    tX = theMonster->cellX;
                                    tY = theMonster->cellY;
                                }
                            }

                            if (!theMonster)
                                sx = ss;

                            BBOSMonster *monster = new BBOSMonster(type, subType, NULL);
                            monster->dontRespawn = TRUE;
                            monster->cellX       = tX;
                            monster->cellY       = tY;
                            monster->targetCellX = tX;
                            monster->targetCellY = tY;
                            monster->spawnX      = tX;
                            monster->spawnY      = tY;
                            ss->mobList->Add(monster);

                            monster->damageDone = monster->damageDone * change;
                            monster->defense    = monster->defense    * change;
                            monster->dropAmount = monster->dropAmount * change;
                            monster->health     = monster->health     * change;
                            monster->maxHealth  = monster->maxHealth  * change;
                            monster->toHit      = monster->toHit      * change;

                            monster->magicResistance = resist / 100.0f;

                            monster->isMoving = TRUE;
                            monster->moveStartTime = timeGetTime() - 10000;

                            MessMobAppear mobAppear;
                            mobAppear.mobID = (unsigned long) monster;
                            mobAppear.type = monster->WhatAmI();
                            mobAppear.monsterType = monster->type;
                            mobAppear.subType = monster->subType;
                            if(SPACE_DUNGEON == sx->WhatAmI())
                            {
                                mobAppear.staticMonsterFlag = FALSE;
                            }

                            mobAppear.x = monster->cellX;
                            mobAppear.y = monster->cellY;
                            sx->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
                                         sizeof(mobAppear), &mobAppear);

                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = -1;
                            messGE.x        = monster->cellX;
                            messGE.y        = monster->cellY;
                            messGE.r        = 255;
                            messGE.g        = 0;
                            messGE.b        = 255;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 1; // in seconds
                            sx->SendToEveryoneNearBut(0, monster->cellX, monster->cellY,
                                                 sizeof(messGE),(void *)&messGE);

                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/modbirth"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                     ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
                {
                    int type, subType, resist;
                    float change;
                    Inventory *inv = NULL;

                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/modbirth <type num> <subType num> <power multiplier> <resist 0-100>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        sscanf(&chatText[argPoint],"%d",&type);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&subType);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%f",&change);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&resist);

                        if (type < 0)
                            type = 0;
                        if (type >= NUM_OF_MONSTERS)
                            type = NUM_OF_MONSTERS-1;
                        if (subType < 0)
                            subType = 0;
                        if (subType >= NUM_OF_MONSTER_SUBTYPES)
                            subType = NUM_OF_MONSTER_SUBTYPES-1;

                        if (change < 0.1f)
                            change = 1;
                        if (change > 100)
                            change = 1;

                        if (resist < 0)
                            resist = 0;
                        if (resist > 100)
                            resist = 100;

                        if (monsterData[type][subType].name[0])
                        {
                            SharedSpace *sx;
                            BBOSMonster * theMonster = NULL;
                            int tX, tY;
                            tX = curAvatar->cellX;
                            tY = curAvatar->cellY;
                            if (curAvatar->controlledMonster)
                            {
                                theMonster = FindMonster(curAvatar->controlledMonster, &sx);
                                if (theMonster)
                                {
                                    tX = theMonster->cellX;
                                    tY = theMonster->cellY;
                                }
                            }

                            if (!theMonster)
                                sx = ss;

                            BBOSMonster *monster = new BBOSMonster(type, subType, NULL);
                            monster->dontRespawn = TRUE;
                            monster->cellX       = tX;
                            monster->cellY       = tY;
                            monster->targetCellX = tX;
                            monster->targetCellY = tY;
                            monster->spawnX      = tX;
                            monster->spawnY      = tY;
                            ss->mobList->Add(monster);

                            monster->damageDone = monster->damageDone * change;
                            monster->defense    = monster->defense    * change;
                            monster->dropAmount = monster->dropAmount * change;
                            monster->health     = monster->health     * change;
                            monster->maxHealth  = monster->maxHealth  * change;
                            monster->toHit      = monster->toHit      * change;

                            monster->magicResistance = resist / 100.0f;

                            monster->isMoving = TRUE;
                            monster->moveStartTime = timeGetTime() - 10000;

                            MessMobAppear mobAppear;
                            mobAppear.mobID = (unsigned long) monster;
                            mobAppear.type = monster->WhatAmI();
                            mobAppear.monsterType = monster->type;
                            mobAppear.subType = monster->subType;
                            if(SPACE_DUNGEON == sx->WhatAmI())
                            {
                                mobAppear.staticMonsterFlag = FALSE;
                            }

                            mobAppear.x = monster->cellX;
                            mobAppear.y = monster->cellY;
                            sx->SendToEveryoneNearBut(0, monster->cellX, monster->cellY, 
                                         sizeof(mobAppear), &mobAppear);

                            MessGenericEffect messGE;
                            messGE.avatarID = -1;
                            messGE.mobID    = -1;
                            messGE.x        = monster->cellX;
                            messGE.y        = monster->cellY;
                            messGE.r        = 255;
                            messGE.g        = 0;
                            messGE.b        = 255;
                            messGE.type     = 0;  // type of particles
                            messGE.timeLen  = 1; // in seconds
                            sx->SendToEveryoneNearBut(0, monster->cellX, monster->cellY,
                                                 sizeof(messGE),(void *)&messGE);

                        }
                        else
                        {
                            MessInfoText infoText;
                            sprintf(infoText.text,"That monster (%d,%d) doesn't exist", 
                                                  type, subType);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/effect"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                     ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                     ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType )
                {
//					int type, amount, r, g, b;
//					float change;
                    Inventory *inv = NULL;

                    if (argPoint == linePoint)
                    {
                        MessInfoText infoText;
                        CopyStringSafely("/effect <type> <intensity> <r> <g> <b>", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        BBOSGroundEffect *bboGE = new BBOSGroundEffect();

                        sscanf(&chatText[argPoint],"%d",&bboGE->type);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&bboGE->amount);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&bboGE->r);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&bboGE->g);
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint],"%d",&bboGE->b);

                        bboGE->cellX = curAvatar->cellX;
                        bboGE->cellY = curAvatar->cellY;

                        ss->mobList->Add(bboGE);

                        MessGroundEffect messGE;
                        messGE.mobID  = (unsigned long) bboGE;
                        messGE.type   = bboGE->type;
                        messGE.amount = bboGE->amount;
                        messGE.x      = bboGE->cellX;
                        messGE.y      = bboGE->cellY;
                        messGE.r      = bboGE->r;
                        messGE.g      = bboGE->g;
                        messGE.b      = bboGE->b;

                        ss->SendToEveryoneNearBut(0, 
                                   curAvatar->cellX, curAvatar->cellY, 
                               sizeof(messGE), &messGE);

                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/killeffect"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                     ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                     ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType)
                {
                    BBOSMob *curMob2 = (BBOSMob *) ss->mobList->GetFirst(
                                        curAvatar->cellX, curAvatar->cellY);
                    while (curMob2)
                    {
                        if (SMOB_GROUND_EFFECT == curMob2->WhatAmI())
                        {
                            ss->mobList->Remove(curMob2);

                            MessMobDisappear messMobDisappear;
                            messMobDisappear.mobID = (unsigned long) curMob2;
                            messMobDisappear.x = curAvatar->cellX;
                            messMobDisappear.y = curAvatar->cellY;
                            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                sizeof(messMobDisappear),(void *)&messMobDisappear);

                            delete curMob2;
                        }

                        curMob2 = (BBOSMob *) ss->mobList->GetNext();
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/invon"))
            {
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    MessAvatarDisappear aDisappear;
                    aDisappear.avatarID = curAvatar->socketIndex;
                    aDisappear.x = curAvatar->cellX;
                    aDisappear.y = curAvatar->cellY;

                    ss->SendToEveryoneNearBut(curAvatar->socketIndex, 
                                   curAvatar->cellX, curAvatar->cellY, 
                               sizeof(aDisappear), &aDisappear);

                    curAvatar->isInvisible = TRUE;
                    MessInfoText infoText;
                    CopyStringSafely("You are invisible.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/invoff"))
            {
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    curAvatar->isInvisible = FALSE;
                    MessInfoText infoText;
                    CopyStringSafely("You are NOT invisible any longer.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/mystats"))
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int s1,s2,s3;
                int start = linePoint;

                argPoint = NextWord(chatText,&linePoint);
                s1 = atoi(&(chatText[argPoint]));
                argPoint = NextWord(chatText,&linePoint);
                s2 = atoi(&(chatText[argPoint]));
                argPoint = NextWord(chatText,&linePoint);
                s3 = atoi(&(chatText[argPoint]));

                if (s1 <= 0 || s1 > 100 || s3 <= 0 || s3 > 100 || s3 <= 0 || s3 > 100)
                {
                    sprintf(&chatMess.text[1],"Invalid values: %d %d %d", s1,s2,s3);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"USAGE: /mystats <physical> <magical> <creative>");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    sprintf(&chatMess.text[1],"Your current stats are: %d %d %d", 
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].physical, 
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].magical,  
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].creative);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].physical = s1;
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].magical  = s2;
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].creative = s3;

                    sprintf(&chatMess.text[1],"New admin stats: %d %d %d", s1,s2,s3);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/myskills"))
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                InventoryObject *skill = NULL;
                BBOSAvatar *tAv = curAvatar;
                tempText[0] = 0;

                while (!found && !done)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }

                            InventoryObject *io = (InventoryObject *) 
                                tAv->charInfoArray[tAv->curCharacterIndex].skills->objects.First();
                            while (io)
                            {
                                if (!stricmp(tempText,io->WhoAmI()))
                                {
                                    skill = io;
//									skillInfo = (InvSkill *) io->extra;
                                }
                                io = (InventoryObject *) 
                                    tAv->charInfoArray[tAv->curCharacterIndex].skills->objects.Next();
                            }

                        if (skill)
                            found = TRUE;
                    }
                }

                if (found)
                {

                    argPoint = NextWord(chatText,&linePoint);
                    int levelToGive = atoi(&(chatText[argPoint]));
                    if (levelToGive <= 0 || levelToGive > 10000)
                    {
                        sprintf(&chatMess.text[1],"Invalid skill level: %d", levelToGive);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        sprintf(&chatMess.text[1],"USAGE: /myskills <skill name> <new skill level>");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else
                    {

                        InvSkill *skillInfo = (InvSkill *) skill->extra;
                        skillInfo->skillLevel = levelToGive;

                        sprintf(tempText,"skill adjusted to level %d.",levelToGive);
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"You don't have such a skill (%s).", tempText);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"USAGE: /myskills <skill name> <new skill level>");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/gettestskills"))
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType))
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                InventoryObject *skill = NULL;
                BBOSAvatar *tAv = curAvatar;
                tempText[0] = 0;

                InventoryObject *io = (InventoryObject *) 
                    tAv->charInfoArray[tAv->curCharacterIndex].skills->objects.First();
                while (io)
                {
                    if (!stricmp("Geomancy",io->WhoAmI()))
                    {
                        skill = io;
                    }
                    io = (InventoryObject *) 
                        tAv->charInfoArray[tAv->curCharacterIndex].skills->objects.Next();
                }

                if (!skill)
                {
                    skill = new InventoryObject(INVOBJ_SKILL,0,"Geomancy");
                    skill->mass = 0.0f;
                    skill->value = 1.0f;
                    skill->amount = 1;
                    tAv->charInfoArray[tAv->curCharacterIndex].skills->AddItemSorted(skill);

                    sprintf(&chatMess.text[1],"New skill added.  Thanks for testing it!");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    sprintf(&chatMess.text[1],"You already have the new skill.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }

                TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);


            }

            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/resetdungeon"))
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType))
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                InventoryObject *skill = NULL;
                BBOSAvatar *tAv = curAvatar;
                tempText[0] = 0;

                argPoint = NextWord(chatText,&linePoint);
                int val;
                sscanf(&chatText[argPoint],"%d",&val);

                if (SPACE_DUNGEON == ss->WhatAmI() && 
                     SPECIAL_DUNGEON_MODERATED & ((DungeonMap *)ss)->specialFlags)
                {
                    ((DungeonMap *)ss)->Randomize(Bracket(val,1,100));

                    MessDungeonChange messDungeonChange;
                    messDungeonChange.floor = messDungeonChange.left = 
                        messDungeonChange.outer = messDungeonChange.top = 0;
                    messDungeonChange.reset = 1;
                    messDungeonChange.x = curAvatar->cellX;
                    messDungeonChange.y = curAvatar->cellY;

                    lserver->SendMsg(sizeof(messDungeonChange),(void *)&messDungeonChange, 0, &tempReceiptList);
                }

            }
 
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/lock"))
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType))
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                InventoryObject *skill = NULL;
                BBOSAvatar *tAv = curAvatar;
                tempText[0] = 0;

                if (SPACE_DUNGEON == ss->WhatAmI())
                {
                    ((DungeonMap *)ss)->isLocked = TRUE;
                    sprintf(&chatMess.text[1],"This dungeon is now locked.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
 
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/unlock"))
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType))
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                InventoryObject *skill = NULL;
                BBOSAvatar *tAv = curAvatar;
                tempText[0] = 0;

                if (SPACE_DUNGEON == ss->WhatAmI())
                {
                    ((DungeonMap *)ss)->isLocked = FALSE;
                    sprintf(&chatMess.text[1],"This dungeon is now NOT locked.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
 
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/killworkbench"))
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;

                InventoryObject *io = (InventoryObject *) 
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].workbench->objects.First();
                while (io)
                {
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                    workbench->objects.Remove(io);
                    delete io;

                    io = (InventoryObject *) 
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].workbench->objects.First();
                }

                sprintf(&chatMess.text[1],"All items in workbench destroyed.");
                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
            }

            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/killmoney"))
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;

                argPoint = NextWord(chatText,&linePoint);
                long moneyToKill = atoi(&(chatText[argPoint]));
                if (moneyToKill <= 0)
                {
                    sprintf(&chatMess.text[1],"Invalid amount of money: %d", moneyToKill);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    long amount = 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money;

                    if (moneyToKill > amount)
                        moneyToKill = amount;

                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money -= 
                              moneyToKill;

                    sprintf(&chatMess.text[1],"%d gold destroyed.", moneyToKill);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/m") && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                argPoint = NextWord(chatText,&linePoint);
                HandleContMonstString(&chatText[argPoint], curAvatar, ss);
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/helper") && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv && targetAv->cellX == curAvatar->cellX &&
                                 targetAv->cellY == curAvatar->cellY)
                {
                    argPoint = linePoint = linePoint + len;

                    if (targetAv->charInfoArray[targetAv->curCharacterIndex].imageFlags &
                         SPECIAL_LOOK_HELPER)
                    {
                        targetAv->charInfoArray[targetAv->curCharacterIndex].imageFlags &= 
                          ~(SPECIAL_LOOK_HELPER);
                        MessAvatarStats mStats;
                        targetAv->BuildStatsMessage(&mStats);
                        ss->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                   sizeof(mStats),(void *)&mStats);

                        sprintf(infoText.text,"%s removes the belt of", 
                                 curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                               sizeof(infoText),(void *)&infoText);

                        sprintf(infoText.text,"Tribal Avatar from %s.", 
                                 targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                               sizeof(infoText),(void *)&infoText);
                    }
                    else
                    {
                        targetAv->charInfoArray[targetAv->curCharacterIndex].imageFlags |= 
                          SPECIAL_LOOK_HELPER;
                        MessAvatarStats mStats;
                        targetAv->BuildStatsMessage(&mStats);
                        ss->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                   sizeof(mStats),(void *)&mStats);

                        sprintf(infoText.text,"%s bestows the belt of", 
                                 curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                               sizeof(infoText),(void *)&infoText);

                        sprintf(infoText.text,"Tribal Avatar upon %s.", 
                                 targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                        ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                               sizeof(infoText),(void *)&infoText);
                    }

                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/h") && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                curAvatar->charInfoArray[curAvatar->curCharacterIndex].health =
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;

                MessAvatarHealth messHealth;
                messHealth.health    = curAvatar->charInfoArray[curAvatar->curCharacterIndex].health;
                messHealth.healthMax = curAvatar->charInfoArray[curAvatar->curCharacterIndex].healthMax;
                messHealth.avatarID  = curAvatar->socketIndex;
                ss->lserver->SendMsg(sizeof(messHealth),(void *)&messHealth, 0, &tempReceiptList);

                curAvatar->AnnounceSpecial(ss, SPECIAL_APP_HEAL);

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/shutdown") && 
                       ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                if (10000 == pleaseKillMe)
                {
                    pleaseKillMe =30;
                    countDownTime.SetToNow();
                }

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/startvote"))
            {

                argPoint = NextWord(chatText,&linePoint);

                if (argPoint == linePoint)
                {
                    sprintf(&chatMess.text[1],"USAGE: /startvote <PROMOTE, DEMOTE, KICK, CHANGESTYLE, CHANGENAME, FIGHTER, MAGE, or CRAFTER> <subject guild member>.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (GUILDSTYLE_NONE != ((TowerMap *)guildSpace)->guildStyle)
                {
                    sscanf(&(chatText[argPoint]),"%s", tempText);

                    argPoint = NextWord(chatText,&linePoint);

                    AttemptToStartVote(curAvatar, ((TowerMap *)guildSpace), &(chatText[argPoint]), tempText);
                }
                else
                {
                    sprintf(&chatMess.text[1],"First use the /guildtype command to set the style of your guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/stopvote"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (argPoint == linePoint)
                {
                    MessInfoText infoText;
                    CopyStringSafely("USAGE: /stopvote <bill number (1-4)>", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    CopyStringSafely("You must have sponsored the bill, and voting must still be happening.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else
                {
                    int targX;
                    sscanf(&chatText[argPoint],"%d",&targX);

                    argPoint = NextWord(chatText,&linePoint);

                    if (targX < 1 || targX > 4) 
                    {
                        MessInfoText infoText;
                        CopyStringSafely("USAGE: /stopvote <bill number (1-4)>", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        CopyStringSafely("You must have sponsored the bill, and voting must still be happening.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        --targX;
                        if ( ((TowerMap *)guildSpace)->bills[targX].type > GUILDBILL_INACTIVE &&
                              VOTESTATE_VOTING == ((TowerMap *)guildSpace)->bills[targX].voteState)
                        {
                            if (IsCompletelySame(
                                   ((TowerMap *)guildSpace)->bills[targX].sponsor,
                                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name
                                ))
                            {
                                ((TowerMap *)guildSpace)->bills[targX].type = GUILDBILL_INACTIVE;

                                CopyStringSafely("Your bill is killed.", 
                                                      200, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/passchange"))
            {
                argPoint = NextWord(chatText,&linePoint);

                if (argPoint == linePoint)
                {
                    MessInfoText infoText;
                    CopyStringSafely("USAGE: /passchange <old password> <new password>", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else
                {
                    sscanf(&chatText[argPoint], "%s", tempText);
                    if (IsCompletelySame(tempText, curAvatar->pass))
                    {
                        argPoint = NextWord(chatText,&linePoint);
                        sscanf(&chatText[argPoint], "%s", tempText);
                        GuaranteeTermination(tempText, 12);
                        CorrectString(tempText);

                        if (strlen(tempText) > 0)
                        {
                            sprintf(curAvatar->pass, tempText);
                            curAvatar->passLen = strlen(curAvatar->pass);

                            sprintf(&(tempText[2]),"Your password is changed to %s.",
                                curAvatar->pass);
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_DATA;
    
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                        }
                        else
                        {
                            CopyStringSafely("new password is too short.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                    else
                    {
                        CopyStringSafely("That's not the correct old password.", 
                                      200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/setguildstats")
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                      )
            {
                argPoint = NextWord(chatText,&linePoint);
                if (argPoint == linePoint)
                {
                    sprintf(&chatMess.text[1],"USAGE: /setguildstats <fight #> <mage #> <craft #> <guild name>");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                int n1 = atoi(&(chatText[argPoint]));

                argPoint = NextWord(chatText,&linePoint);
                int n2 = atoi(&(chatText[argPoint]));

                argPoint = NextWord(chatText,&linePoint);
                int n3 = atoi(&(chatText[argPoint]));

                argPoint = NextWord(chatText,&linePoint);

                TowerMap *tm = (TowerMap *) spaceList->Find(&(chatText[argPoint]));
                if (tm)
                {
                  tm->specLevel[0] = n1;
                    tm->specLevel[1] = n2;
                    tm->specLevel[2] = n3;

                    sprintf(infoText.text, "Specializations for %s are now", tm->WhoAmI());
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    sprintf(infoText.text, "%d FIG  %d MAG  %d CRA", 
                              tm->specLevel[0], tm->specLevel[1], tm->specLevel[2]);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame( &(chatText[argPoint]) , "/status" ) )
            {
                argPoint = NextWord( chatText, &linePoint );

                if ( IsSame(&(chatText[argPoint]) , "afk")) {
                    argPoint = NextWord(chatText,&linePoint);
                    curAvatar->status = AVATAR_STATUS_AFK;
                    sprintf( curAvatar->status_text, &(chatText[argPoint]) );

                    sprintf(&chatMess.text[1],"Your status has been changed to AFK.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else if ( IsSame(&(chatText[argPoint]) , "busy")) {
                    argPoint = NextWord(chatText,&linePoint);
                    curAvatar->status = AVATAR_STATUS_BUSY;
                    sprintf( curAvatar->status_text, &(chatText[argPoint]) );

                    sprintf(&chatMess.text[1],"Your status has been changed to Busy.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else if ( IsSame(&(chatText[argPoint]) , "available")) {
                    argPoint = NextWord(chatText,&linePoint);
                    curAvatar->status = AVATAR_STATUS_AVAILABLE;
                    sprintf( curAvatar->status_text, &(chatText[argPoint]) );

                    sprintf(&chatMess.text[1],"Your status has been changed to Available.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    sprintf(&chatMess.text[1],"Usage: /status <afk, busy or available> <message>");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/shout") ||
                  IsSame(&(chatText[argPoint]) , "/s") )
            {
//				if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime > 1)
                {
                    sprintf(&(tempText[2]),"%s shouts, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_SHOUT;
                    ss->IgnorableSendToEveryoneNear(curAvatar, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[2]));
                    fclose(source);
                }
                /*
                else
                {
                    sprintf(&chatMess.text[1],"That feature is locked for 10 minutes.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                */
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/about"))
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if( targetAv && FALSE == targetAv->isInvisible )
                {
                    LongTime now;

                    argPoint = linePoint = linePoint + len;

                    sprintf(&chatMess.text[1],"About %s : ",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    long hours, minutes, days;
                    days  = targetAv->charInfoArray[targetAv->curCharacterIndex].lifeTime / 12 / 24;
                    hours = (targetAv->charInfoArray[targetAv->curCharacterIndex].lifeTime -
                        days * 12 * 24) / 12;
                    minutes = (targetAv->charInfoArray[targetAv->curCharacterIndex].lifeTime -
                        days * 12 * 24 - hours * 12) * 5;
                    

                    sprintf(&chatMess.text[1],"     Age: %ld days, %ld hours, %ld minutes",
                        days, hours, minutes);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    if( ACCOUNT_TYPE_MODERATOR == targetAv->accountType )
                        sprintf( &chatMess.text[1],"     Account Type: Moderator" );
                    else if( ACCOUNT_TYPE_TRIAL_MODERATOR == targetAv->accountType )
                        sprintf( &chatMess.text[1],"     Account Type: Trial Moderator" );
                    else if( ACCOUNT_TYPE_ADMIN == targetAv->accountType )
                        sprintf( &chatMess.text[1],"     Account Type: Administrator" );
                    else if( targetAv->hasPaid )
                        sprintf( &chatMess.text[1],"     Account Type: Blade Mistress Patron" );
                    else
                        sprintf( &chatMess.text[1],"     Account Type: Regular" );

                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);


                    sprintf( &chatMess.text[1], "     Stats: %d/%d/%d",
                            targetAv->charInfoArray[targetAv->curCharacterIndex].physical,
                            targetAv->charInfoArray[targetAv->curCharacterIndex].magical,
                            targetAv->charInfoArray[targetAv->curCharacterIndex].creative );

                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    if( targetAv->status == AVATAR_STATUS_AVAILABLE ) {
                        sprintf( &chatMess.text[1],"     Status: Available" );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                        sprintf( &chatMess.text[1],"     Message: <none>" );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if( targetAv->status == AVATAR_STATUS_AFK ) {
                        sprintf( &chatMess.text[1],"     Status: AFK" );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                        sprintf( &chatMess.text[1],"     Message: %s", targetAv->status_text );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if( targetAv->status == AVATAR_STATUS_BUSY ) {
                        sprintf( &chatMess.text[1],"     Status: Busy" );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                        sprintf( &chatMess.text[1],"     Message: %s", targetAv->status_text );
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/kickall") ||
                        IsSame(&(chatText[argPoint]) , "/banall")) 
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_HIDDEN_ADMIN == curAvatar->accountType)
                      )
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;

                int ban = FALSE;
                if (IsSame(&(chatText[argPoint]) , "/ban") &&
                      ( ACCOUNT_TYPE_ADMIN == curAvatar->accountType || 
                      ACCOUNT_TYPE_HIDDEN_ADMIN == curAvatar->accountType ))
                    ban = TRUE;

                while (!found && !done)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }
                        targetAv = FindAvatarByPartialName((char *)tempText, &sp);
                        if (targetAv)
                            found = TRUE;
                    }
                }

                if (found)
                {
                    // boot this joker!
                    sprintf(tempText,"%s logged off by administrator.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                        sizeof(infoText),(void *)&infoText);


                    _strdate( t2 );
                    _strtime( t3 );
                    sprintf( t, "[%s %s] %s, %s removed %s, %s from play.\n", 
                        t2,
                        t3,
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        curAvatar->name,
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name, 
                        targetAv->name);
                    LogOutput("moderatorKickLog.txt", t);

                    if (ban)
                        targetAv->accountType = ACCOUNT_TYPE_BANNED;
                    HandleKickoff(targetAv, sp);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/tempban"))
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_HIDDEN_ADMIN == curAvatar->accountType)
                      )
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;
                int doAll = IsSame(&(chatText[argPoint]) , "/tempbanall");

                argPoint = NextWord(chatText,&linePoint);
                int banHours;
                sscanf(&chatText[argPoint],"%d",&banHours);

                start = linePoint;
                while (!found && !done)
                {
                    if ( doAll)
                    {
                        argPoint = NextWord(chatText,&linePoint);
                        if (argPoint == linePoint)
                            done = TRUE;
                        else
                        {
                            memcpy(tempText,&(chatText[start]), linePoint-start);
                            int back = 1;
                            tempText[linePoint-start] = 0;
                            while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                            {
                                tempText[linePoint-start-back] = 0;
                                ++back;
                            }
                            targetAv = FindAvatarByPartialName((char *)tempText, &sp);
                            if (targetAv)
                                found = TRUE;
                        }
                    }
                    else
                    {
                        argPoint = NextWord(chatText,&linePoint);
                        if (argPoint == linePoint)
                            done = TRUE;
                        else
                        {
                            memcpy(tempText,&(chatText[start]), linePoint-start);
                            int back = 1;
                            tempText[linePoint-start] = 0;
                            while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                            {
                                tempText[linePoint-start-back] = 0;
                                ++back;
                            }
                            targetAv = FindAvatarByAvatarName((char *)tempText, &sp);
                            if (targetAv)
                                found = TRUE;
                        }
                    }
                }

                if (found)
                {
                    // boot this joker!
                    sprintf(tempText,"%s excused from play for %d hours.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                              banHours);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                        sizeof(infoText),(void *)&infoText);

                    _strdate( t2 );
                    _strtime( t3 );
                    sprintf( t, "[%s %s] %s, %s removed %s, %s from play for %d hours.\n", 
                        t2,
                        t3,
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        curAvatar->name,
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name, 
                        targetAv->name,
                        banHours);
                    LogOutput("moderatorKickLog.txt", t);

                    targetAv->accountRestrictionTime.SetToNow();
                    targetAv->accountRestrictionTime.AddMinutes(banHours * 60);
                    targetAv->restrictionType = 1; // tempban

                    HandleKickoff(targetAv, sp);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/givetime"))
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv)
                {
                    argPoint = linePoint = linePoint + len;

                    argPoint = NextWord(chatText,&linePoint);
                    int timeToGive = atoi(&(chatText[argPoint]));
                    if (timeToGive <= 0 || timeToGive > 100)
                    {
                        sprintf(&chatMess.text[1],"Invalid amount of time: %d", timeToGive);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else
                    {
                        targetAv->accountExperationTime.AddMinutes(60*24*timeToGive);
                        targetAv->SaveAccount();

                        tempReceiptList.push_back(targetAv->socketIndex);

                        sprintf(tempText,"%s is given %d more days of play time.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                              timeToGive);
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        source = fopen("freeTimeGiven.txt","a");
                        /* Display operating system-style date and time. */
                        _strdate( tempText2 );
                        fprintf(source, "%s, ", tempText2 );
                        _strtime( tempText2 );
                        fprintf(source, "%s, ", tempText2 );
                        fprintf(source,"%s is given %d more days by %s.", 
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                              timeToGive,
                             curAvatar->charInfoArray[targetAv->curCharacterIndex].name);
                        fclose(source);
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( (IsSame(&(chatText[argPoint]) , "/kick") ||
                        IsSame(&(chatText[argPoint]) , "/ban")) 
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_HIDDEN_ADMIN == curAvatar->accountType)
                      )
            {
                int found = FALSE;
                int done = FALSE;

                int ban = FALSE;
                if (IsSame(&(chatText[argPoint]) , "/ban") &&
                     ( ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                     ACCOUNT_TYPE_HIDDEN_ADMIN == curAvatar->accountType ))
                    ban = TRUE;

                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv)
                {
                    argPoint = linePoint = linePoint + len;

                    // boot this joker!
                    sprintf(tempText,"%s logged off by administrator.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                        sizeof(infoText),(void *)&infoText);

                    _strdate( t2 );
                    _strtime( t3 );
                    sprintf( t, "[%s %s] %s, %s removed %s, %s from play.\n", 
                        t2,
                        t3,
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        curAvatar->name,
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name, 
                        targetAv->name);
                    LogOutput("moderatorKickLog.txt", t);

                    if (ban)
                        targetAv->accountType = 10;
                    HandleKickoff(targetAv, sp);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/uidban") 
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int showList = FALSE;
                int remove = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;

                while (!found && !done && !showList & !remove)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else if (!stricmp("list", &chatText[argPoint]))
                        showList = TRUE;
                    else if (!strnicmp("remove", &chatText[argPoint], 6))
                        remove = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }
                        targetAv = FindAvatarByAvatarName((char *)tempText, &sp);
                        if (targetAv)
                            found = TRUE;
                    }
                }

                if (found)
                {
                    // boot this joker, and ban his UID!
                    sprintf(tempText,"%s UID banned by administrator.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                        sizeof(infoText),(void *)&infoText);

                    targetAv->accountType = 10;
                    uidBanList->addBannedUID(targetAv->uniqueId,
                         targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    uidBanList->Save();
                    HandleKickoff(targetAv, sp);
                }
                else if (showList)
                {
                    sprintf(&chatMess.text[1],"Banned UID list:");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    UidBanRecord *br = (UidBanRecord *) uidBanList->bannedUIDRecords.First();
                    while (br)
                    {
                        sprintf(tempText,"%d   %s %s",
                            br->uid,
                            br->dateString, br->WhoAmI());
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        br = (UidBanRecord *) uidBanList->bannedUIDRecords.Next();
                    }
                }
                else if (remove)
                {
                    int uid = 0;
                    argPoint = NextWord(chatText,&linePoint);

                    sscanf(&chatText[argPoint],"%d", &uid);

                    UidBanRecord *br = (UidBanRecord *) uidBanList->bannedUIDRecords.First();
                    while (br)
                    {
                        if (uid == br->uid)
                        {
                            uidBanList->bannedUIDRecords.Remove(br);
                            delete br;
                            uidBanList->Save();
                            sprintf(&chatMess.text[1],"UID removed.");
                            chatMess.text[0] = TEXT_COLOR_DATA;
                            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        }
                        br = (UidBanRecord *) uidBanList->bannedUIDRecords.Next();
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/uidban <character name>   adds UID to list, kicks.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/uidban list   shows list of banned UIDs.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/uidban remove 12345  removes UID 12345 from list.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }

            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/ipban") 
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int showList = FALSE;
                int remove = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;

                while (!found && !done && !showList & !remove)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else if (!stricmp("list", &chatText[argPoint]))
                        showList = TRUE;
                    else if (!strnicmp("remove", &chatText[argPoint], 6))
                        remove = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }
                        targetAv = FindAvatarByAvatarName((char *)tempText, &sp);
                        if (targetAv)
                            found = TRUE;
                    }
                }

                if (found)
                {
                    // boot this joker, and ban his IP!
                    sprintf(tempText,"%s IP banned by administrator.",
                             targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                    sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
                                        sizeof(infoText),(void *)&infoText);

                    targetAv->accountType = 10;
                    banList->addBannedIP(targetAv->IP,
                         targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    HandleKickoff(targetAv, sp);
                }
                else if (showList)
                {
                    sprintf(&chatMess.text[1],"Banned IP list:");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    BanRecord *br = (BanRecord *) banList->bannedIPRecords.First();
                    while (br)
                    {
                        sprintf(tempText,"%d.%d.%d.%d   %s %s",
                            (int) br->ip[0], (int) br->ip[1], (int) br->ip[2], (int) br->ip[3],
                            br->dateString, br->WhoAmI());
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        br = (BanRecord *) banList->bannedIPRecords.Next();
                    }
                }
                else if (remove)
                {
                    int ipNums[4];
                    argPoint = NextWord(chatText,&linePoint);

                    sscanf(&chatText[argPoint],"%d.%d.%d.%d", 
                            &ipNums[0], &ipNums[1], &ipNums[2], &ipNums[3]);

                    BanRecord *br = (BanRecord *) banList->bannedIPRecords.First();
                    while (br)
                    {
                        if (ipNums[0] == br->ip[0] &&
                            ipNums[1] == br->ip[1] &&
                            ipNums[2] == br->ip[2] &&
                            ipNums[3] == br->ip[3])
                        {
                            banList->bannedIPRecords.Remove(br);
                            delete br;
                            sprintf(&chatMess.text[1],"IP removed.");
                            chatMess.text[0] = TEXT_COLOR_DATA;
                            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        }
                        br = (BanRecord *) banList->bannedIPRecords.Next();
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/ipban <character name>   adds IP to list, kicks.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/ipban list   shows list of banned IPs.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    sprintf(&chatMess.text[1],"/ipban remove 128.0.0.1  removes IP 128.0.0.1 from list.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/ip") 
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;

                while (!found && !done)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }
                        targetAv = FindAvatarByAvatarName((char *)tempText, &sp);
                        if (targetAv)
                            found = TRUE;
                    }
                }

                if (found)
                {
                    sprintf(tempText,"%d.%d.%d.%d   %s (%s)",
                        (int) targetAv->IP[0], (int) targetAv->IP[1], 
                        (int) targetAv->IP[2], (int) targetAv->IP[3],
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        targetAv->name);
                    CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/broadcast") ||
                  IsSame(&(chatText[argPoint]) , "/b") )
            {
                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    sprintf(&(tempText[2]),"%s",&(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_DATA;

                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, ADMIN %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[2]));
                    fclose(source);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/joinguild")
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
                      )
            {
                argPoint = NextWord(chatText,&linePoint);

                TowerMap *tm = (TowerMap *) spaceList->Find(&(chatText[argPoint]));
                if (tm)
                {
                    MemberRecord *mr = new MemberRecord(0,
                               curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                    tm->members->Append(mr);

                    tm->lastChangedTime.SetToNow();

                    sprintf(&(tempText[1]),"%s is now an honorary member of %s.",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        tm->WhoAmI());
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                       strlen(tempText) + 1,(void *)&tempText,2);

                    curAvatar->AssertGuildStatus(ss,TRUE);

                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guildstats")
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                      )
            {
                argPoint = NextWord(chatText,&linePoint);

                TowerMap *tm = (TowerMap *) spaceList->Find(&(chatText[argPoint]));
                if (tm)
                {
                    sprintf(infoText.text, "Specializations for %s are", tm->WhoAmI());
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    sprintf(infoText.text, "%d FIG  %d MAG  %d CRA", 
                              tm->specLevel[0], tm->specLevel[1], tm->specLevel[2]);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/getquest")
                         && (ACCOUNT_TYPE_ADMIN == curAvatar->accountType ||
                             ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
                      )
            {
                argPoint = NextWord(chatText,&linePoint);
                if (argPoint == linePoint)
                {
                    CopyStringSafely("USAGE: /getquest <0-12>     gives you a quest", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else
                {
                    int qType;
                    sscanf(&(chatText[argPoint]), "%d", &qType);

                    BBOSTree *t = NULL;
                    BBOSMob *curMob2 = (BBOSMob *) ss->mobList->GetFirst(0,0,1000);
                    while (curMob2 && !t)
                    {
                        if (SMOB_TREE == curMob2->WhatAmI())
                            t = (BBOSTree *) curMob2;

                        curMob2 = (BBOSMob *) ss->mobList->GetNext();
                    }

                    if (!t)
                    {
                        CopyStringSafely("Use this command on the overland map.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        return;
                    }

                    int freeSlot = -1;
                    for (int i = 0; i < QUEST_SLOTS; ++i)
                    {
                        if (-1 == curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                          quests[i].completeVal)
                        {
                            freeSlot = i;
                            i = QUEST_SLOTS;
                        }
                    }

                    if (-1 == freeSlot)
                    {
                        sprintf(infoText.text,"You are already burdened with enough tasks.");
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        CreateTreeQuest(&(curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                          quests[freeSlot]), curAvatar, ss, t, qType);
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/loadquests")
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                questMan->Load();
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/namechange")
                         && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv)
                {
                    argPoint = linePoint = linePoint + len;
                    // change this avatar's name
                    tempReceiptList.push_back(targetAv->socketIndex); // list is for me and target now

                    if (!UN_IsNameUnique(&(chatText[argPoint])))
                    {
                        sprintf(tempText,"Name change for %s failed.  Already taken.",
                                 targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        UN_RemoveName(targetAv->charInfoArray[targetAv->curCharacterIndex].name);

                        UN_AddName(&(chatText[argPoint]));

                        sprintf(tempText,"%s name changed by administrator.",
                                  targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    //					sp->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
    //										sizeof(infoText),(void *)&infoText);
    //					if (sp != ss)
    //						ss->SendToEveryoneNearBut(0, targetAv->cellX, targetAv->cellY,
    //										sizeof(infoText),(void *)&infoText);

                        int res = ChangeAvatarGuildName(
                                  targetAv->charInfoArray[targetAv->curCharacterIndex].name, 
                                  &chatText[argPoint]);

                        if (!res)
                        {
                            sprintf(&chatMess.text[1],"WARNING: no guild, or guild list change unsuccessful");
                            chatMess.text[0] = TEXT_COLOR_DATA;
                            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        }

                        CopyStringSafely(&chatText[argPoint], 
                                              strlen(&chatText[argPoint])+1,
                                              targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                                              32);
                        targetAv->SaveAccount();
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/announce") ||
                  IsSame(&(chatText[argPoint]) , "/a") )
            {
//				if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime > 1)
                {
                    sprintf(&(tempText[2]),"%s tells everyone, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_ANNOUNCE;

                    sp = (SharedSpace *) spaceList->First();
                    while (sp)
                    {
                        sp->IgnorableSendToEveryone(curAvatar, strlen(tempText) + 1,(void *)&tempText);
                        sp = (SharedSpace *) spaceList->Next();
                    }

                    //lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, NULL);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[2]));
                    fclose(source);
                }
                /*
                else
                {
                    sprintf(&chatMess.text[1],"That feature is locked for 10 minutes.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                */
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/emote") ||
                  IsSame(&(chatText[argPoint]) , "/e") )
            {
                argPoint = NextWord(chatText,&linePoint);

                if (curAvatar->controlledMonster)
                {
                    SharedSpace *sx;

                    BBOSMonster * theMonster = FindMonster(
                              curAvatar->controlledMonster, &sx);
                    if (theMonster)
                    {
                        sprintf(&(tempText[2]),"%s %s",
                            theMonster->Name(),
                            &(chatText[argPoint]));
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_EMOTE;
                        sx->IgnorableSendToEveryoneNear(curAvatar, theMonster->cellX, theMonster->cellY,
                                              strlen(tempText) + 1,(void *)&tempText,2);

                        source = fopen("logs\\chatline.txt","a");
                        /* Display operating system-style date and time. */
                        _strdate( tempText2 );
                        fprintf(source, "%s, ", tempText2 );
                        _strtime( tempText2 );
                        fprintf(source, "%s, ", tempText2 );
                        fprintf(source,"%d, %d, %s\n", theMonster->cellX, theMonster->cellY, &(tempText[2]));
                        fclose(source);
                    }
                }
                else
                {
                    sprintf(&(tempText[2]),"%s %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[argPoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_EMOTE;
                    ss->IgnorableSendToEveryoneNear(curAvatar, curAvatar->cellX, curAvatar->cellY,
                                          strlen(tempText) + 1,(void *)&tempText,2);

                    curAvatar->QuestTalk(ss);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, 
                            &(tempText[2]));
                    fclose(source);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/token"))
            {
                for (int i = 0; i < MAGIC_MAX; ++i)
                {
                    if (tokenMan.towerName[i][0])
                    {
                        LongTime now;
                        sprintf(&(tempText[2]),"%s token: %s: %d hours left.", magicNameList[i],
                            tokenMan.towerName[i],now.MinutesDifference(&tokenMan.tokenTimeLeft[i]) 
                                 / 60);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_DATA;
                        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                    }
                    else
                    {
                        LongTime now;
                        sprintf(&(tempText[2]),"%s token: unclaimed.", magicNameList[i]);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_DATA;
                        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/time"))
            {
                _strdate( tempText2 );
                _strtime( tempText3 );

                sprintf(&chatMess.text[1],"%s, %s (real time at server).", tempText2, tempText3);
                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                int oneHour = 60 * 6;
                int gameTime = dayTimeCounter / (float)(oneHour * 4) * 24.0f * 60;
                gameTime += 6 * 60;
                if (gameTime > 24 * 60)
                    gameTime -= 24 * 60;

                if (dayTimeCounter < 2.5f * oneHour)
                    sprintf(tempText2,"DAY");
                else if (dayTimeCounter >= 2.75f * oneHour && dayTimeCounter < 3.75f * oneHour)
                    sprintf(tempText2,"NIGHT");
                else if (dayTimeCounter >= 2.5f * oneHour && dayTimeCounter < 2.75f * oneHour)
                    sprintf(tempText2,"DUSK");
                else if (dayTimeCounter >= 3.75f * oneHour)
                    sprintf(tempText2,"DAWN");

                sprintf(&chatMess.text[1],"Game time of day is %d:%02d (%s)", gameTime/60,
                                            gameTime - (gameTime/60*60), tempText2);
                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
            }
            //***************************************
            else if ( false && IsSame(&(chatText[argPoint]) , "/referredby")) // no more referrals
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv && !curAvatar->isReferralDone && curAvatar->hasPaid)
                {
                    argPoint = linePoint = linePoint + len;

                    sprintf(&(tempText[2]),"You reward %s for your referral!  Thank you!",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    std::vector<TagID> targetReceiptList;
                    targetReceiptList.clear();
                    targetReceiptList.push_back(targetAv->socketIndex);

                    sprintf(&(tempText[2]),"%s acknowledges that you referred her to Blade Mistress!",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

                    curAvatar->isReferralDone = TRUE;
                    ++targetAv->patronCount;

                    // log the referral
                    source = fopen("referralLog.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%s, refers, %s, %d\n", 
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        targetAv->patronCount);
                    fclose(source);

                    if (!targetAv->hasPaid)
                    {
                        if (targetAv->patronCount < 2)
                        {
                            sprintf(&(tempText[2]),"You only need one more referral to become a paid Blade Mistress player!");
                            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);
                        }
                        else
                        {
                            sprintf(&(tempText[2]),"You are now a paid Blade Mistress player, with one more week of play time!");
                            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

                            targetAv->hasPaid = TRUE;

                            LongTime now;
                            if (now.MinutesDifference(&targetAv->accountExperationTime) > 0)
                                targetAv->accountExperationTime.AddMinutes(60*24*7);
                            else
                            {
                                targetAv->accountExperationTime.SetToNow();
                                targetAv->accountExperationTime.AddMinutes(60*24*7);
                            }
                        }
                    }
                    else
                    {
                        sprintf(&(tempText[2]),"You get one more week of play time!");
                        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

                        LongTime now;
                        if (now.MinutesDifference(&targetAv->accountExperationTime) > 0)
                            targetAv->accountExperationTime.AddMinutes(60*24*7);
                        else
                        {
                            targetAv->accountExperationTime.SetToNow();
                            targetAv->accountExperationTime.AddMinutes(60*24*7);
                        }
                    }

                    targetAv->SaveAccount();
                    curAvatar->SaveAccount();

                }
                else
                {
                    if (!targetAv)
                        sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    else if (!curAvatar->hasPaid)
                        sprintf(&chatMess.text[1],"You may only refer once you have paid.");
                    else
                        sprintf(&chatMess.text[1],"You have already used /referredby successfully.");

                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/tell") ||
                  IsSame(&(chatText[argPoint]) , "/t") )
            {
                int ignore = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv && targetAv->IsContact(
                       curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        CONTACT_IGNORE))
                {
//				  	sprintf(&(tempText[1]),"%s is ignoring you.",
//						targetAv->charInfoArray[targetAv->curCharacterIndex].name,
//						&(chatText[linePoint]));
//					tempText[0] = NWMESS_PLAYER_CHAT_LINE;
//			  		lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    ignore = TRUE;
                }

                if( targetAv && !ignore && FALSE == targetAv->isInvisible )
                {
                    argPoint = linePoint = linePoint + len;

                    sprintf(&(tempText[2]),"You tell %s, %s",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    std::vector<TagID> targetReceiptList;
                    targetReceiptList.clear();
                    targetReceiptList.push_back(targetAv->socketIndex);

                    sprintf(&(tempText[2]),"%s tells you, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

                    CopyStringSafely(
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, 32, 
                         targetAv->lastTellName, 32);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s, %s\n", curAvatar->cellX, curAvatar->cellY, 
                        &(tempText[2]), 
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    fclose(source);

                    // KARMA
                    HandleKarmaText(&(chatText[linePoint]), curAvatar, targetAv);



                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/reply") ||
                  IsSame(&(chatText[argPoint]) , "/r") )
            {
                int found = FALSE;
                int done = FALSE;
                int start = linePoint;
                BBOSAvatar *targetAv = NULL;

                if (0 == curAvatar->lastTellName[0])
                {
                    sprintf(&chatMess.text[1],"You have not yet been /telled by anyone.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                targetAv = FindAvatarByAvatarName(curAvatar->lastTellName, &sp);
                if( targetAv && FALSE == targetAv->isInvisible )
                    found = TRUE;

                if (found && targetAv->IsContact(
                       curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        CONTACT_IGNORE))
                {
//				  	sprintf(&(tempText[1]),"%s is ignoring you.",
//						targetAv->charInfoArray[targetAv->curCharacterIndex].name,
//						&(chatText[linePoint]));
//					tempText[0] = NWMESS_PLAYER_CHAT_LINE;
//			  		lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    found = FALSE;
                }

                if (found)
                {

                    sprintf(&(tempText[2]),"You tell %s, %s",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    std::vector<TagID> targetReceiptList;
                    targetReceiptList.clear();
                    targetReceiptList.push_back(targetAv->socketIndex);

                    sprintf(&(tempText[2]),"%s tells you, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[linePoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &targetReceiptList);

                    CopyStringSafely(
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, 32, 
                         targetAv->lastTellName, 32);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s, %s\n", curAvatar->cellX, curAvatar->cellY, 
                        &(tempText[2]), 
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name);
                    fclose(source);

                    // KARMA
                    HandleKarmaText(&(chatText[linePoint]), curAvatar, targetAv);

                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guildinfo"))
            {
                int found = FALSE;
                int done = FALSE, messLen = 0;
                int start = linePoint;

                TowerMap *tMap = NULL;
                while (!found && !done)
                {
                    argPoint = NextWord(chatText,&linePoint);
                    if (argPoint == linePoint)
                        done = TRUE;
                    else
                    {
                        memcpy(tempText,&(chatText[start]), linePoint-start);
                        int back = 1;
                        tempText[linePoint-start] = 0;
                        while (' ' == tempText[linePoint-start-back] && linePoint-start-back > 0)
                        {
                            tempText[linePoint-start-back] = 0;
                            ++back;
                        }

                        SharedSpace *sp = (SharedSpace *) spaceList->First();
                        while (sp)
                        {
                            if (SPACE_GUILD == sp->WhatAmI() && 
                                 IsCompletelySame(tempText, sp->WhoAmI())) 
                                tMap = (TowerMap *) sp;

                            sp = (SharedSpace *) spaceList->Next();
                        }

                        if (tMap)
                            found = TRUE;
                    }
                }

                if (found)
                {
                    sprintf(&chatMess.text[1],"%s: %s", 
                                 guildStyleNames[tMap->guildStyle], tMap->WhoAmI());
                    messLen = strlen(chatMess.text);

                    sprintf(&(chatMess.text[messLen])," (spec %d/%d/%d)", 
                               tMap->specLevel[0], tMap->specLevel[1], tMap->specLevel[2]);
                    messLen = strlen(chatMess.text);

                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                    messLen = 0;
                    int memberNum = 0;

                    MemberRecord *mr = (MemberRecord *) tMap->members->First();
                    while (mr)
                    {
                        ++memberNum;
                        mr = (MemberRecord *) tMap->members->Next();
                    }

                    LongTime now;

                    long diff = tMap->lastChangedTime.MinutesDifference(&now) / 60 / 24;

                    sprintf(&chatMess.text[1],"%dN %dE. %d members. Last activity: %ld days", 
                                 256- tMap->enterY, 256- tMap->enterX, memberNum, diff);
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                }
                else
                {
                    sprintf(&chatMess.text[1],"No such guild could be found.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/where") ||
                      IsSame(&(chatText[argPoint]) , "/whereis"))
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv && FALSE == targetAv->isInvisible)
                {
                    argPoint = linePoint = linePoint + len;

                    int maxX = 0, maxY = 0;
                    if (SPACE_GROUND == sp->WhatAmI())
                    {
                        maxX = 256;
                        maxY = 256;
                    }
                    else if (SPACE_DUNGEON == sp->WhatAmI())
                    {
                        maxX = ((DungeonMap *)sp)->width;
                        maxY = ((DungeonMap *)sp)->height;
                    }
                    else if (SPACE_GUILD == sp->WhatAmI())
                    {
                        maxX = ((TowerMap *)sp)->width;
                        maxY = ((TowerMap *)sp)->height;
                    }
                    else if (SPACE_REALM == sp->WhatAmI())
                    {
                        maxX = ((RealmMap *)sp)->width;
                        maxY = ((RealmMap *)sp)->height;
                    }
                    else if (SPACE_LABYRINTH == sp->WhatAmI())
                    {
                        maxX = ((LabyrinthMap *)sp)->width;
                        maxY = ((LabyrinthMap *)sp)->height;
                    }

                    if (SPACE_DUNGEON == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the %s (%dN %dE),", ((DungeonMap *) sp)->name,
                            256-((DungeonMap *) sp)->enterY, 256-((DungeonMap *) sp)->enterX);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_GUILD == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the guild tower (%dN %dE),",
                            256-((TowerMap *) sp)->enterY, 256-((TowerMap *) sp)->enterX);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_REALM == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the %s,", sp->WhoAmI());
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_LABYRINTH == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the Labyrinth,");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    sprintf(&chatMess.text[1],"%s is at %dN %dE.",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        maxY - targetAv->cellY, maxX - targetAv->cellX);
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/loc"))
            {
                int found = FALSE;
                int done = FALSE;
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);

                if (targetAv && FALSE == targetAv->isInvisible)
                {
                    argPoint = linePoint = linePoint + len;

                    if (SPACE_DUNGEON == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the %s (%dN %dE),", ((DungeonMap *) sp)->name,
                            ((DungeonMap *) sp)->enterY, ((DungeonMap *) sp)->enterX);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_GUILD == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the guild tower (%dN %dE),",
                            ((TowerMap *) sp)->enterY, ((TowerMap *) sp)->enterX);
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_REALM == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the %s,", sp->WhoAmI());
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    else if (SPACE_LABYRINTH == sp->WhatAmI())
                    {
                        sprintf(&chatMess.text[1],"In the Labyrinth,");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    }
                    sprintf(&chatMess.text[1],"%s is at %dN %dE.",
                        targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                        targetAv->cellY, targetAv->cellX);
                    chatMess.text[0] = TEXT_COLOR_DATA;

                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
                else
                {
                    sprintf(&chatMess.text[1],"No one of that name is logged on.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/whoall"))
            {
                ProcessWho(curAvatar,NULL);
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/who"))
            {
                ProcessWho(curAvatar,ss);
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/ignore"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (argPoint != linePoint)
                    ProcessFriend(curAvatar, FRIEND_ACTION_IGNORE, ss, &(chatText[argPoint]));
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/friend") || 
                       IsSame(&(chatText[argPoint]) , "/f"))
            {
                argPoint = NextWord(chatText,&linePoint);
                if (argPoint == linePoint)
                    ProcessFriend(curAvatar, FRIEND_ACTION_LIST, ss);
                else if ( IsSame(&(chatText[argPoint]) , "list")) 
                    ProcessFriend(curAvatar, FRIEND_ACTION_LIST, ss);
                else if ( IsSame(&(chatText[argPoint]) , "add")) 
                {
                    argPoint = NextWord(chatText,&linePoint);
                    ProcessFriend(curAvatar, FRIEND_ACTION_ADD, ss, &(chatText[argPoint]));
                }
                else if ( IsSame(&(chatText[argPoint]) , "remove")) 
                {
                    argPoint = NextWord(chatText,&linePoint);
                    ProcessFriend(curAvatar, FRIEND_ACTION_REMOVE, ss, &(chatText[argPoint]));
                }
                else
                {
                    sprintf(&(tempText[1]),"%s tells friends, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[argPoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;

                    sp = (SharedSpace *) spaceList->First();
                    while (sp)
                    {
                        sp->SendToEveryFriend(curAvatar, strlen(tempText) + 1,(void *)&tempText);
                        sp = (SharedSpace *) spaceList->Next();
                    }

                    // send to myself as well
                    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[1]));
                    fclose(source);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guildkick"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                {
                    TowerMap *tm = (TowerMap *)guildSpace;
                    MemberRecord *mr2 = (MemberRecord *) tm->members->Find(&(chatText[argPoint]));
                    if (!mr2)
                    {
                        sprintf(&chatMess.text[1],"%s isn't in the guild.", &(chatText[argPoint]));
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        return;
                    }

                    sprintf(&(tempText[2]),"%s is removed from the guild by ADMIN.",
                        mr2->WhoAmI());
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_DATA;

                    sp = (SharedSpace *) spaceList->First();
                    while (sp)
                    {
                        SendToEveryGuildMate("NILLNILL",	sp, tm, 
                            strlen(tempText) + 1,(void *)&tempText);
                        sp = (SharedSpace *) spaceList->Next();
                    }

                    tm->members->Remove(mr2);
                    tm->lastChangedTime.SetToNow();
                    delete mr2;

                }
                else if (GUILDSTYLE_MONARCHY == ((TowerMap *)guildSpace)->guildStyle ||
                         GUILDSTYLE_TYRANNY  == ((TowerMap *)guildSpace)->guildStyle)
                {

                    TowerMap *tm = (TowerMap *)guildSpace;
                    MemberRecord *mr = (MemberRecord *)
                        tm->members->Find(
                                 curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                    if (!mr)
                    {
                        sprintf(&chatMess.text[1],"You're not in your own guild.  Internal Error 2.");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        return;
                    }

                    MemberRecord *mr2 = (MemberRecord *)
                        tm->members->Find(&(chatText[argPoint]));
                    if (!mr2)
                    {
                        sprintf(&chatMess.text[1],"%s isn't in the guild.", &(chatText[argPoint]));
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        return;
                    }

                    if (mr->value1 > mr2->value1)
                    {

                        sprintf(&(tempText[2]),"%s is kicked out of the guild by %s.",
                            mr2->WhoAmI(), mr->WhoAmI());
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_DATA;

                        sp = (SharedSpace *) spaceList->First();
                        while (sp)
                        {
                            SendToEveryGuildMate("NILLNILL",	sp, tm, 
                                strlen(tempText) + 1,(void *)&tempText);
                            sp = (SharedSpace *) spaceList->Next();
                        }

                        tm->members->Remove(mr2);
                        tm->lastChangedTime.SetToNow();
                        delete mr2;
                    }
                    else
                    {

                        sprintf(&(tempText[2]),"%s tries to kick out %s, but doesn't rank higher.",
                            mr->WhoAmI(), mr2->WhoAmI());
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_DATA;

                        sp = (SharedSpace *) spaceList->First();
                        while (sp)
                        {
                            SendToEveryGuildMate("NILLNILL",	sp, tm, 
                                strlen(tempText) + 1,(void *)&tempText);
                            sp = (SharedSpace *) spaceList->Next();
                        }
                    }

                }
                else
                {
                    sprintf(&chatMess.text[1],"Your guild style doesn't allow /guildkick.  Use /startvote instead.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guildname"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (IsSame("UNNAMED GUILD",guildSpace->WhoAmI()))
                {
                    if (UN_IsNameUnique(&(chatText[argPoint])))
                    {
                        UN_AddName(&(chatText[argPoint]));

                        CopyStringSafely(&(chatText[argPoint]),1024, guildSpace->do_name,64);
                        sprintf(&(tempText[2]),"The %s guild has now been christened by %s.",
                            guildSpace->WhoAmI(),
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        tempText[1] = TEXT_COLOR_DATA;
                        ss->SendToEveryoneNearBut(0,curAvatar->cellX, curAvatar->cellY,
                                              strlen(tempText) + 1,(void *)&tempText);
                    }
                    else
                    {
                        sprintf(&chatMess.text[1],"That guild name is taken.");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        return;
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"Your guild is already named.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guildtype"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                int index = 0;
                if (GUILDSTYLE_NONE == ((TowerMap *)guildSpace)->guildStyle)
                {
                    for (index = 1; index < GUILDSTYLE_MAX; ++index)
                    {
                        if (!stricmp(&(chatText[argPoint]), guildStyleNames[index]))
                        {
                            ((TowerMap *)guildSpace)->guildStyle = index;

//							CopyStringSafely(&(chatText[argPoint]),1024, guildSpace->do_name,64);
                            sprintf(&(tempText[2]),"The %s guild is now a %s.",
                                ((TowerMap *)guildSpace)->WhoAmI(),
                                guildStyleNames[index]);
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_DATA;
                            ss->SendToEveryoneNearBut(0,curAvatar->cellX, curAvatar->cellY,
                                                  strlen(tempText) + 1,(void *)&tempText);

                            sprintf(&(tempText[2]),"This was set by by %s.",
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                            ss->SendToEveryoneNearBut(0,curAvatar->cellX, curAvatar->cellY,
                                                  strlen(tempText) + 1,(void *)&tempText);
                            index = 1000;
                        }
                    }

                    if (index < 1000)
                    {
                        sprintf(&chatMess.text[1],"That is not a valid guild type.");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                        sprintf(&chatMess.text[1],"USAGE: /guildtype <CAUCUS or COUNCIL or VESTRY or MONARCHY or TYRANNY>");
                        chatMess.text[0] = TEXT_COLOR_DATA;
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

                        return;
                    }
                }
                else
                {
                    sprintf(&chatMess.text[1],"Your guild is already a %s.",
                                  guildStyleNames[((TowerMap *)guildSpace)->guildStyle]);
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/quitguild"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, 
                              &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                sprintf(&(tempText[2]),"%s leaves the %s guild.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                    guildSpace->WhoAmI());
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                tempText[1] = TEXT_COLOR_DATA;
                ss->SendToEveryoneNearBut(0,curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText);

                DeleteNameFromGuild(
                      curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, 
                      &guildSpace);

                curAvatar->AssertGuildStatus(ss,TRUE);

            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/guild") || 
                       IsSame(&(chatText[argPoint]) , "/g"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (argPoint == linePoint)
                    ListGuild(curAvatar, (TowerMap *)guildSpace);
                else
                {
                    sprintf(&(tempText[2]),"%s tells the guild, %s",
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                        &(chatText[argPoint]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_GUILD;

                    sp = (SharedSpace *) spaceList->First();
                    while (sp)
                    {
                        SendToEveryGuildMate(
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                                sp, (TowerMap *)guildSpace, strlen(tempText) + 1,(void *)&tempText);
                        sp = (SharedSpace *) spaceList->Next();
                    }

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[2]));
                    fclose(source);
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/vote"))
            {
                argPoint = NextWord(chatText,&linePoint);
                // find the guild I belong to
                SharedSpace *guildSpace;
                if (!FindAvatarInGuild(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, &guildSpace))
                {
                    sprintf(&chatMess.text[1],"You do not belong to a guild.");
                    chatMess.text[0] = TEXT_COLOR_DATA;
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    return;
                }

                if (argPoint == linePoint)
                    ListVotes(curAvatar, (TowerMap *)guildSpace);
                else
                {
                    int targX;
                    sscanf(&chatText[argPoint],"%d",&targX);

                    argPoint = NextWord(chatText,&linePoint);

                    if (targX < 1 || targX > 4) 
                    {
                        MessInfoText infoText;
                        CopyStringSafely("USAGE: /vote <bill number (1-4)> (yes OR no)", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else if (argPoint == linePoint)
                        DetailVote(targX-1, curAvatar, (TowerMap *)guildSpace);
                    else
                    {
                        if (!stricmp(&chatText[argPoint], "yes"))
                            VoteOnBill(1, targX-1, curAvatar, (TowerMap *)guildSpace);
                        else if (!stricmp(&chatText[argPoint], "no"))
                            VoteOnBill(0, targX-1, curAvatar, (TowerMap *)guildSpace);
                        else
                        {
                            MessInfoText infoText;

                            CopyStringSafely("Your vote was NOT recorded.  Please use 'yes' or 'no' to vote.", 
                                                  200, infoText.text, MESSINFOTEXTLEN);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                            CopyStringSafely("USAGE: /vote <bill number (1-4)> (yes OR no)", 
                                                  200, infoText.text, MESSINFOTEXTLEN);
                            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/quest"))
            {
                argPoint = NextWord(chatText,&linePoint);

                if (argPoint == linePoint)
                {
                    MessInfoText infoText;
                    CopyStringSafely("USAGE: /quest LIST      shows what quests you have", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    CopyStringSafely("USAGE: /quest <number of quest>      shows more info", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }
                else
                {
                    if (!stricmp(&chatText[argPoint], "list"))
                    {
                        for (int i = 0; i < QUEST_SLOTS; ++i)
                        {
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_DATA;
                            sprintf(&(tempText[2]),"%d: ", i+1);

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                         quests[i].ShortDesc(&(tempText[strlen(tempText)]));

                            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                        }
                    }
                    else if (IsSame(&chatText[argPoint], "delete"))
                    {
                        if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType ||
                             ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                        {
                            argPoint = NextWord(chatText,&linePoint);

                            int targX;
                            sscanf(&chatText[argPoint],"%d",&targX);
                            --targX;
                            if (targX >= 0 && targX < QUEST_SLOTS)
                            {
                                curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                         quests[targX].EmptyOut();

                                CopyStringSafely("Done.", 200, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                            else
                            {
                                CopyStringSafely("Error: Bad quest number", 200, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                            }
                        }
                    }
                    else
                    {
                        int targX;
                        sscanf(&chatText[argPoint],"%d",&targX);

                        if (targX > 0 && targX <= QUEST_SLOTS)
                        {
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_DATA;
                            sprintf(&(tempText[2]),"%d: ", targX);

                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                         quests[targX-1].LongDesc(&(tempText[strlen(tempText)]));

                            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                        }
                    }
                }
            }
            //***************************************
            else if ( IsSame(&(chatText[argPoint]) , "/uid") && ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
            {
                int len;
                BBOSAvatar *targetAv = FindAvatarByStringName(&(chatText[linePoint]), len, &sp);
                
                // Return the unique number
                if (targetAv)
                    sprintf(	&chatMess.text[1],
                                "%s's Unique ID is %d.",
                                targetAv->charInfoArray[targetAv->curCharacterIndex].name,
                                targetAv->GetUniqueId()	);
                else
                    sprintf(&chatMess.text[1], "That character is currently not online.");

                chatMess.text[0] = TEXT_COLOR_DATA;
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
            }
            //***************************************
            else
            {
                sprintf(infoText.text,"unknown / command");
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
//			  	sprintf(infoText.text,"/ commands are: shout (to visual range), announce (to everyone),");
//				ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
//			  	sprintf(infoText.text,"emote, who, whoall, where, tell, and about.");
                sprintf(infoText.text,"/ commands are: shout (to visual range), announce");
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                sprintf(infoText.text,"(to everyone), emote, who, whoall, where, tell, about,");
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                sprintf(infoText.text,"ignore, and friend (+ add, remove, list).");
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                sprintf(infoText.text,"Most can be abbreviated to just the first letter.");
                ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            }

        }
        else if ( IsSame(&(chatText[0]) , "Caorael"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_REALM_SPIRITS;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.r = messChant.g = 255;
            messChant.b = 100;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Jinweise"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_REALM_DEAD;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.r = messChant.b = 255;
            messChant.g = 100;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Atronach"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_GROUP_PRAYER;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.r = messChant.g = 200;
            messChant.b = 255;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Danfratern"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_CREATE_GUILD;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Dantoporas"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_PLANT_TOWER;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Danviro"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_EDIT_TOWER;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Danduco"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_INDUCT_MEMBER;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Reto"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_LEAVE_REALM;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Metamoros"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Word of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_ROP;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 255;
            messChant.g = 0;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else if ( IsSame(&(chatText[0]) , "Polyvacus"))
        {
            if (-1 == curAvatar->chantType)
            {
                sprintf(&(tempText[1]),"%s utters a Response of Power.",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);
            }
            curAvatar->chantType = CHANT_ROP_RESPONSE;
            curAvatar->chantTime = timeGetTime();

            MessChant messChant;
            messChant.avatarID = curAvatar->socketIndex;
            messChant.b = 0;
            messChant.g = 255;
            messChant.r = 0;
            ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, 
                                       sizeof(messChant), &messChant);

            HandleWordOfPower(curAvatar, ss);

        }
        else
        {
            if (curAvatar->controlledMonster)
            {
                SharedSpace *sx;

                BBOSMonster * theMonster = FindMonster(
                          curAvatar->controlledMonster, &sx);
                if (theMonster)
                {
                    sprintf(&(tempText[1]),"%s says, %s",
                        theMonster->Name(),
                        &(chatText[0]));
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    sx->IgnorableSendToEveryoneNear(curAvatar, theMonster->cellX, theMonster->cellY,
                                          strlen(tempText) + 1,(void *)&tempText,2);

                    source = fopen("logs\\chatline.txt","a");
                    /* Display operating system-style date and time. */
                    _strdate( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    _strtime( tempText2 );
                    fprintf(source, "%s, ", tempText2 );
                    fprintf(source,"%d, %d, %s\n", theMonster->cellX, theMonster->cellY, &(tempText[1]));
                    fclose(source);
                }
            }
            else
            {
                sprintf(&(tempText[1]),"%s says, %s",
                    curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                    &(chatText[0]));
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                ss->IgnorableSendToEveryoneNear(curAvatar, curAvatar->cellX, curAvatar->cellY,
                                      strlen(tempText) + 1,(void *)&tempText,2);

                curAvatar->QuestTalk(ss);

                source = fopen("logs\\chatline.txt","a");
                /* Display operating system-style date and time. */
                _strdate( tempText2 );
                fprintf(source, "%s, ", tempText2 );
                _strtime( tempText2 );
                fprintf(source, "%s, ", tempText2 );
                fprintf(source,"%d, %d, %s\n", curAvatar->cellX, curAvatar->cellY, &(tempText[1]));
                fclose(source);
            }
        }
    }
}


//*******************************************************************************
void BBOServer::ProcessWho(BBOSAvatar *ca, SharedSpace *ss)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    SharedSpace *sp;
    MessPlayerChatLine chatMess;
    int messLen = 0;
    char fullName[300];
    char fullName2[300];
    int startingLine = TRUE;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(ca->socketIndex);

    sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob && (!ss || ss == sp))
        {
            if (SMOB_AVATAR == curMob->WhatAmI() && FALSE == ((BBOSAvatar *) curMob)->isInvisible)
            {
                curAvatar = (BBOSAvatar *) curMob;
                char *name = curAvatar->charInfoArray[curAvatar->curCharacterIndex].name;

                char paidText[16];
                if (ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
                    sprintf( paidText, "MOD" );
                else if( ACCOUNT_TYPE_TRIAL_MODERATOR == curAvatar->accountType )
                    sprintf( paidText, "TRIAL MOD" );
                else if (ACCOUNT_TYPE_ADMIN == curAvatar->accountType)
                    sprintf( paidText, "ADMIN" );
                else
                    sprintf(paidText,"");

                sprintf( fullName, "%s %d/%d/%d %s", name, 
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].physical,
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].magical,
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].creative,
                                                        paidText );

                sprintf( fullName2,", %s %d/%d/%d %s", name, 
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].physical,
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].magical,
                                                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].creative,
                                                        paidText );

                for (int testVal = 0; testVal < 1; ++testVal)
                {
                    if (strlen(fullName2) + messLen > 75)
                    {
                        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                        messLen = 0;
                        startingLine = TRUE;
                    }
                    
                    if (startingLine)
                        sprintf(&(chatMess.text[messLen]),"%s", fullName);
                    else
                        sprintf(&(chatMess.text[messLen]),"%s", fullName2);
                    messLen = strlen(chatMess.text);
                    startingLine = FALSE;
                }

            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        sp = (SharedSpace *) spaceList->Next();
    }

    if (messLen > 0)
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

    return;
}

//*******************************************************************************
void BBOServer::HandleTeleport(BBOSAvatar *ca, BBOSMob *townmage, 
                                         SharedSpace *ss, int forward)
{
    MessInfoText infoText;
    char tempText[1024];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(ca->socketIndex);

    FILE *source = fopen("teleporting.txt","a");
    
    /* Display operating system-style date and time. */
    _strdate( tempText );
    fprintf(source, "%s, ", tempText );
    _strtime( tempText );
    fprintf(source, "%s, ", tempText );

    fprintf(source,"%s, ", ca->charInfoArray[ca->curCharacterIndex].name);



    int townIndex = -1;
    for (int t = 0; t < NUM_OF_TOWNS; ++t)
    {
        if (townList[t].x == townmage->cellX && townList[t].y + 1 == townmage->cellY)
            townIndex = t;
    }

    if (townIndex < 0)
    {
        sprintf(infoText.text,"Internal Error: bad town");
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        fprintf(source,"ERROR\n");
        fclose(source);
        return;
    }

    if (ca->charInfoArray[ca->curCharacterIndex].inventory->money < 400)
    {
        sprintf(infoText.text,"Sorry, you don't have enough money.");
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        fprintf(source,"NO FUNDS\n");
        fclose(source);
        return;
    }

    int oldTownIndex = townIndex;

    if (forward)
    {
        ++townIndex;
        if (townIndex >= NUM_OF_TOWNS)
            townIndex = 0;
    }
    else
    {
        --townIndex;
        if (townIndex < 0)
            townIndex = NUM_OF_TOWNS-1;
    }

    ca->AnnounceDisappearing(ss, SPECIAL_APP_TELEPORT_AWAY);

    ca->cellX = ca->targetCellX = townList[townIndex].x;
    ca->cellY = ca->targetCellY = townList[townIndex].y;

    // tell everyone about my arrival
    ca->IntroduceMyself(ss, SPECIAL_APP_TELEPORT);

    // tell this player about everyone else around
    ca->UpdateClient(ss, TRUE);

    ca->charInfoArray[ca->curCharacterIndex].inventory->money -= 400;

    sprintf(infoText.text,"You have been teleported to %s.", townList[townIndex].name);
    ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

    fprintf(source,"MOVED, %s, %s\n", townList[oldTownIndex].name, townList[townIndex].name);

    fclose(source);

    ca->charInfoArray[ca->curCharacterIndex].spawnX = townList[townIndex].x;
    ca->charInfoArray[ca->curCharacterIndex].spawnY = townList[townIndex].y;

}


//*******************************************************************************
void BBOServer::HandleKickoff(BBOSAvatar *ca, SharedSpace *sp)
{
    MessInfoText infoText;
//	char tempText[1024];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(ca->socketIndex);

    // boot this sloth!
    ca->AnnounceDisappearing(sp, SPECIAL_APP_NOTHING);

    MessBoot boot;
    sp->lserver->SendMsg(sizeof(boot),(void *)&boot, 0, &tempReceiptList);

    ca->QuestSpaceChange(sp, NULL);

    sp->avatars->Remove(ca);
    if (SPACE_GROUND != sp->WhatAmI())
    {
        ca->charInfoArray[ca->curCharacterIndex].lastX = 
            ca->charInfoArray[ca->curCharacterIndex].spawnX;
        ca->charInfoArray[ca->curCharacterIndex].lastY = 
            ca->charInfoArray[ca->curCharacterIndex].spawnY;
    }
    else
    {
        ca->charInfoArray[ca->curCharacterIndex].lastX = 
            ca->cellX;
        ca->charInfoArray[ca->curCharacterIndex].lastY = 
            ca->cellY;
    }
    ca->SaveAccount();
    delete ca;
}



//*******************************************************************************
void BBOServer::HandleTreeTalk(BBOSAvatar *curAvatar, SharedSpace *sp, MessTalkToTree *mt)
{
    MessInfoText infoText;
    char tempText[1024];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

//	curAvatar = FindAvatar(fromSocket, &ss);

//	if	(curAvatar)
    {
        BBOSMob *curMob = (BBOSMob *) sp->mobList->GetFirst(curAvatar->cellX, curAvatar->cellY);
        while (curMob)
        {
            if (SMOB_TREE == curMob->WhatAmI() && 
                 curMob->cellX == curAvatar->cellX &&
                 curMob->cellY == curAvatar->cellY)
            {
                BBOSTree *t = (BBOSTree *) curMob;

                if (ResolveTreeQuests(curAvatar, sp, t))
                    return;

                if (0 == mt->which)
                {
                    FILE *fp = fopen("serverdata\\treeText.txt","r");
                    if (fp)
                    {
                        int done = FALSE;
                        int found = FALSE;
                        while (!done)
                        {
                            LoadLineToString(fp, tempText);
                            if (!found)
                            {
                                if (IsSame(tempText, "TREE"))
                                {
                                    if (IsSame(&(tempText[5]), magicNameList[t->index]))
                                    {
                                        found = TRUE;
                                    }
                                }
                            }
                            else
                            {
                                if (IsSame(tempText, "TREE"))
                                    done = TRUE;
                                else
                                {
                                    // send out this line in pieces
                                    int pos = 0;
                                    int size = strlen(&(tempText[pos]));
                                    while (size > 0)
                                    {
                                        int back = pos + MESSINFOTEXTLEN-2;
                                        if (pos + size < back)
                                            back = pos + size;
                                        while (' ' != tempText[back] && 0 != tempText[back] && back-1 > pos)
                                            --back;
                                        memcpy(infoText.text, &(tempText[pos]), MESSINFOTEXTLEN-2);
                                        infoText.text[back-pos] = 0;
                                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                        pos = back+1;
                                        size = strlen(&(tempText[pos]));
                                    }
                                }
                            }
                        }

                        fclose(fp);
                    }
                }
                else if (1 == mt->which)
//					&&
//						    (ACCOUNT_TYPE_ADMIN     == curAvatar->accountType || 
//							  ACCOUNT_TYPE_MODERATOR == curAvatar->accountType)
//						  )
                {
                    int freeSlot = -1;
                    for (int i = 0; i < QUEST_SLOTS; ++i)
                    {
                        if (-1 == curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                          quests[i].completeVal)
                        {
                            freeSlot = i;
                            i = QUEST_SLOTS;
                        }
                    }

                    if (-1 == freeSlot)
                    {
                        sprintf(infoText.text,"You are already burdened with enough tasks.");
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                    }
                    else
                    {
                        CreateTreeQuest(&(curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                          quests[freeSlot]), curAvatar, sp, t);
                    }
                }
                else if (1 == mt->which)
                {
                    sprintf(infoText.text,"Soon, I will be able to give you tasks for my Mistress.");
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                }

                sp->mobList->SetToLast();
            }
            else if (SMOB_WITCH == curMob->WhatAmI() && 
                 curMob->cellX == curAvatar->cellX &&
                 curMob->cellY == curAvatar->cellY)
            {
                BBOSNpc *npc = (BBOSNpc *) curMob;
                if (2 == mt->which)
                {
                    // witch talks
                    questMan->WitchTalk(npc, sp);
                }
                if (3 == mt->which)
                {
                    // witch gives quest
                    questMan->WitchGiveQuest(npc, curAvatar, sp);
                }

            }
            else if (SMOB_TOWNMAGE == curMob->WhatAmI() && 
                 curMob->cellX == curAvatar->cellX &&
                 curMob->cellY == curAvatar->cellY)
            {
                BBOSNpc *npc = (BBOSNpc *) curMob;
                if (5 == mt->which)
                {
                    FILE *fp = fopen("serverdata\\treeText.txt","r");
                    if (fp)
                    {
                        int done = FALSE;
                        int found = FALSE;
                        char tmText[64];
                        sprintf(tmText,"TOWNMAGE%d", (rand() % 4) + 1);
                        while (!done)
                        {
                            LoadLineToString(fp, tempText);
                            if (!found)
                            {
                                if (IsSame(tempText, "TREE"))
                                {
                                    if (IsSame(&(tempText[5]), tmText))
                                    {
                                        found = TRUE;
                                    }
                                }
                            }
                            else
                            {
                                if (IsSame(tempText, "TREE"))
                                    done = TRUE;
                                else
                                {
                                    // send out this line in pieces
                                    int pos = 0;
                                    int size = strlen(&(tempText[pos]));
                                    while (size > 0)
                                    {
                                        int back = pos + MESSINFOTEXTLEN-2;
                                        if (pos + size < back)
                                            back = pos + size;
                                        while (' ' != tempText[back] && 0 != tempText[back] && back-1 > pos)
                                            --back;
                                        memcpy(infoText.text, &(tempText[pos]), MESSINFOTEXTLEN-2);
                                        infoText.text[back-pos] = 0;
                                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                                        pos = back+1;
                                        size = strlen(&(tempText[pos]));
                                    }
                                }
                            }
                        }

                        fclose(fp);
                    }
                }

            }
            curMob = (BBOSMob *) sp->mobList->GetNext();
        }
    }
}


//*******************************************************************************
void BBOServer::HandleWordOfPower(BBOSAvatar *ca, SharedSpace *sp)
{
    MessInfoText infoText;
    char tempText[1024];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(ca->socketIndex);

    int num = 0;
    int x = ca->cellX;
    int y = ca->cellY;
    int chantType = ca->chantType;

    if ((SPACE_REALM == sp->WhatAmI() ||
          (SPACE_DUNGEON == sp->WhatAmI() && 
           SPECIAL_DUNGEON_TEMPORARY &((DungeonMap *)sp)->specialFlags)
         )&& CHANT_LEAVE_REALM == chantType)
    {
        // tell everyone I'm dissappearing
        ca->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

        // tell my client I'm leaving the dungeon
        MessChangeMap changeMap;
        changeMap.oldType = sp->WhatAmI(); 
        changeMap.newType = SPACE_GROUND; 
        changeMap.sizeX   = MAP_SIZE_WIDTH;
        changeMap.sizeY   = MAP_SIZE_HEIGHT;
        lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

        SharedSpace *sg = (SharedSpace *) spaceList->Find(SPACE_GROUND);

        ca->QuestSpaceChange(sp,sg);

        // move me to my new SharedSpace
        sp->avatars->Remove(ca);
        sg->avatars->Append(ca);

        ca->cellX = ca->targetCellX = 
            ((BBOSAvatar *)ca)->charInfoArray[((BBOSAvatar *)ca)->
                     curCharacterIndex].spawnX;
        ca->cellY = ca->targetCellY = 
            ((BBOSAvatar *)ca)->charInfoArray[((BBOSAvatar *)ca)->
                     curCharacterIndex].spawnY;

        // tell everyone about my arrival
        ca->IntroduceMyself(sg, SPECIAL_APP_DUNGEON);

        // tell this player about everyone else around
        ca->UpdateClient(sg, TRUE);

        MessInfoText infoText;
        CopyStringSafely("You leave the Realm.", 
                              200, infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }

    for (int g = 0; g < NUM_OF_TOWNS && SPACE_GROUND == sp->WhatAmI(); ++g)
    {
        if (x == townList[g].x && y == townList[g].y)
            return;
    }

    BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
            if (ca2->cellX == x && ca2->cellY == y &&
                 ca2->chantType > -1 && ca2->chantType == chantType)
            {
                ++num;
            }
        }
        curMob = (BBOSMob *) sp->avatars->Next();
    }

    if (num > 2 && CHANT_REALM_SPIRITS == chantType)
    {
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
                if (ca2->cellX == x && ca2->cellY == y)
                {

                    RealmMap *rp = NULL;
                    SharedSpace *sp2 = (SharedSpace *) spaceList->First();
                    while (sp2)
                    {
                        if (SPACE_GROUND == sp->WhatAmI() && SPACE_REALM == sp2->WhatAmI() && 
                             REALM_ID_SPIRITS == ((RealmMap *)sp2)->type)
                        {
                            rp = (RealmMap *)sp2;
                            sp2 = (SharedSpace *) spaceList->Last();
                        }
                        sp2 = (SharedSpace *) spaceList->Next();
                    }

                    if (rp)
                    {
                        ca2->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

                        // tell my client I'm entering the realm
                        tempReceiptList.clear();
                        tempReceiptList.push_back(ca2->socketIndex);

                        MessChangeMap changeMap;
                        changeMap.realmID = rp->type;
                        changeMap.oldType = sp->WhatAmI(); 
                        changeMap.newType = rp->WhatAmI(); 
                        changeMap.sizeX   = rp->sizeX;
                        changeMap.sizeY   = rp->sizeY;

                        lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                        MessInfoText infoText;
                        CopyStringSafely("You enter the Realm of Spirits.", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        ca2->QuestSpaceChange(sp,rp);

                        // move me to my new SharedSpace
                        sp->avatars->Remove(ca2);
                        rp->avatars->Append(ca2);

                        ca2->cellX = ca2->targetCellX = (rand() % 4) + 2;
                        ca2->cellY = ca2->targetCellY = (rand() % 4) + 2;

                        // tell everyone about my arrival
                        ca2->IntroduceMyself(rp, SPECIAL_APP_DUNGEON);

                        // tell this player about everyone else around
                        ca2->UpdateClient(rp, TRUE);
                    }

                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if (num > 2 && CHANT_REALM_DEAD == chantType)
    {
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
                if (ca2->cellX == x && ca2->cellY == y)
                {

                    RealmMap *rp = NULL;
                    SharedSpace *sp2 = (SharedSpace *) spaceList->First();
                    while (sp2)
                    {
                        if (SPACE_GROUND == sp->WhatAmI() && SPACE_REALM == sp2->WhatAmI() && 
                             REALM_ID_DEAD == ((RealmMap *)sp2)->type)
                        {
                            rp = (RealmMap *)sp2;
                            sp2 = (SharedSpace *) spaceList->Last();
                        }
                        sp2 = (SharedSpace *) spaceList->Next();
                    }

                    if (rp)
                    {
                        ca2->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

                        // tell my client I'm entering the realm
                        tempReceiptList.clear();
                        tempReceiptList.push_back(ca2->socketIndex);

                        MessChangeMap changeMap;
                        changeMap.realmID = rp->type;
                        changeMap.oldType = sp->WhatAmI(); 
                        changeMap.newType = rp->WhatAmI(); 
                        changeMap.sizeX   = rp->sizeX;
                        changeMap.sizeY   = rp->sizeY;

                        lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                        MessInfoText infoText;
                        CopyStringSafely("You enter the Realm of the Dead.", 
                                              200, infoText.text, MESSINFOTEXTLEN);
                        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        ca2->QuestSpaceChange(sp,rp);

                        // move me to my new SharedSpace
                        sp->avatars->Remove(ca2);
                        rp->avatars->Append(ca2);

                        ca2->cellX = ca2->targetCellX = (rand() % 4) + 2;
                        ca2->cellY = ca2->targetCellY = (rand() % 4) + 2;

                        // tell everyone about my arrival
                        ca2->IntroduceMyself(rp, SPECIAL_APP_DUNGEON);

                        // tell this player about everyone else around
                        ca2->UpdateClient(rp, TRUE);
                    }
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if (num > 2 && CHANT_GROUP_PRAYER == chantType)
    {
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
                if (ca2->cellX == x && ca2->cellY == y)
                {
                    ca2->QuestPrayer(sp);
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if (num > 3 && CHANT_CREATE_GUILD == chantType)
    {
//		return;
        // does anyone already belong to a guild?
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                SharedSpace *sx;
                if (ca2->cellX == x && ca2->cellY == y && 
                     FindAvatarInGuild(ca2->charInfoArray[ca2->curCharacterIndex].name, &sx))
                {
                    sprintf(&(tempText[1]),"%s is already a member of a guild.",
                        ca2->charInfoArray[ca2->curCharacterIndex].name);
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                       strlen(tempText) + 1,(void *)&tempText,2);
                    return;
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        // make a new guild
        TowerMap *dm = new TowerMap(SPACE_GUILD,"UNNAMED GUILD",lserver);
        dm->InitNew(5,5);
        spaceList->Append(dm);

//		BBOSTower *tower = new BBOSTower(dm->enterX, dm->enterY);
//		sp->mobList->Add(tower);
//		tower->ss = dm;
//		tower->isGuildTower = TRUE;

        // and add everyone to it!
        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                if (ca2->cellX == x && ca2->cellY == y)
                {
                    MemberRecord *mr = new MemberRecord(0,
                               ca2->charInfoArray[ca2->curCharacterIndex].name);
                    dm->members->Append(mr);

                    sprintf(&(tempText[1]),"%s is a member of the new guild.",
                        ca2->charInfoArray[ca2->curCharacterIndex].name);
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                       strlen(tempText) + 1,(void *)&tempText,2);
                }
            }
            sp->avatars->Find(curMob);
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        dungeonUpdateTime = 0; // save off dungeons and guilds

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if (num > 3 && CHANT_PLANT_TOWER == chantType)
    {
//		return;
        // does anyone NOT belong to this guild?
        SharedSpace *sxNorm = NULL;
        int adminHelp = FALSE;
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                SharedSpace *sx;
                if (ca2->cellX == x && ca2->cellY == y && ca2->chantType == chantType && 
                     FindAvatarInGuild(ca2->charInfoArray[ca2->curCharacterIndex].name, &sx))
                {
                    if (sxNorm && sxNorm != sx)
                    {
                        sprintf(&(tempText[1]),"%s is not a member of the guild.",
                            ca2->charInfoArray[ca2->curCharacterIndex].name);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                           strlen(tempText) + 1,(void *)&tempText,2);
                        return;
                    }
                    else
                    {
                        sxNorm = sx;
                        if (ACCOUNT_TYPE_ADMIN == ca2->accountType || 
                             ACCOUNT_TYPE_MODERATOR == ca2->accountType ||
                             ACCOUNT_TYPE_TRIAL_MODERATOR == ca2->accountType)
                            adminHelp = TRUE;
                    }
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        // move the guild tower
        if (!sxNorm || !adminHelp)
            return;

        TowerMap *dm = (TowerMap *) sxNorm;
        dm->enterX = x;
        dm->enterY = y+1; // south of here

        // find the visible tower
        BBOSTower *towerMob = NULL;
        curMob = (BBOSMob *) sp->mobList->GetFirst(0,0,1000);
        while (curMob)
        {
            if (SMOB_TOWER == curMob->WhatAmI() && ((BBOSTower *)curMob)->isGuildTower &&
                 ((BBOSTower *)curMob)->ss == dm)
            {
                towerMob = (BBOSTower *)curMob;
                sp->mobList->SetToLast();
            }
            curMob = (BBOSMob *) sp->mobList->GetNext();
        }

        if (!towerMob)
        {
            BBOSTower *tower = new BBOSTower(x,y+1);
            sp->mobList->Add(tower);
            tower->ss = dm;
            tower->isGuildTower = TRUE;
            towerMob = tower;

            BBOSChest *chest = new BBOSChest(2,2);
            dm->mobList->Add(chest);

            BBOSWarpPoint *wp = new BBOSWarpPoint(0,0);
            wp->targetX      = dm->enterX;
            wp->targetY      = dm->enterY-1;
            wp->spaceType    = SPACE_GROUND;
            wp->spaceSubType = 0;
            dm->mobList->Add(wp);

        }

        // move the visible tower
        if (towerMob)
        {
            towerMob->cellX = x;
            towerMob->cellY = y+1;
            ((TowerMap *)towerMob->ss)->enterX = x;
            ((TowerMap *)towerMob->ss)->enterY = y+1;

            BBOSWarpPoint *wp = (BBOSWarpPoint *)dm->mobList->GetFirst(0,0);
            while (wp)
            {
                if (SMOB_WARP_POINT == wp->WhatAmI())
                {
                    wp->targetX = ((TowerMap *)towerMob->ss)->enterX; 
                    wp->targetY = ((TowerMap *)towerMob->ss)->enterY-1;
                }

                wp = (BBOSWarpPoint *)dm->mobList->GetNext();
            }
            
            sp->mobList->Remove(towerMob);
            sp->mobList->Add(towerMob);

            MessMobAppear mobAppear;
            mobAppear.mobID = (unsigned long) towerMob;
            mobAppear.type  = towerMob->WhatAmI();
            mobAppear.x     = towerMob->cellX;
            mobAppear.y     = towerMob->cellY;
            sp->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY,
                           sizeof(mobAppear),(void *)&mobAppear);

            MessCaveInfo cInfo;
            cInfo.mobID       = (long) towerMob;
            cInfo.hasMistress = FALSE;
            cInfo.type        = -1;
            sp->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY,
                           sizeof(cInfo),(void *)&cInfo);

            MessGenericEffect messGE;
            messGE.avatarID = -1;
            messGE.mobID    = (long)towerMob;
            messGE.x        = towerMob->cellX;
            messGE.y        = towerMob->cellY;
            messGE.r        = 0;
            messGE.g        = 255;
            messGE.b        = 255;
            messGE.type     = 0;  // type of particles
            messGE.timeLen  = 20; // in seconds
            sp->SendToEveryoneNearBut(0, towerMob->cellX, towerMob->cellY,
                              sizeof(messGE),(void *)&messGE);

        }

        dungeonUpdateTime = 0; // save off dungeons and guilds

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if ( num > 3 && CHANT_INDUCT_MEMBER == chantType )
    {
//		return;
        // does anyone NOT belong to this guild?
        SharedSpace *sxNorm = NULL;
        BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
        int ranking_members = 0;
        int loc_x, loc_y = 0;
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                SharedSpace *sx;
                loc_x = ca2->cellX;
                loc_y = ca2->cellY;

                if (ca2->cellX == x && ca2->cellY == y && ca2->chantType == chantType && 
                     FindAvatarInGuild(ca2->charInfoArray[ca2->curCharacterIndex].name, &sx))
                {
                    if (sxNorm && sxNorm != sx)
                    {
                        sprintf(&(tempText[1]),"%s is not a member of the guild.",
                            ca2->charInfoArray[ca2->curCharacterIndex].name);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                           strlen(tempText) + 1,(void *)&tempText,2);
                        return;
                    }
                    else
                    {
                        if( GetRecordForGuildMember( ca2->charInfoArray[ca2->curCharacterIndex].name, &sx )->value1 > 0 || 
                            ca2->accountType == ACCOUNT_TYPE_ADMIN )
                        {
                            ranking_members += 1;
                            sxNorm = sx;
                        }
                    }
                }
                else if (ca2->cellX == x && ca2->cellY == y && -1 == ca2->chantType && 
                     FindAvatarInGuild(ca2->charInfoArray[ca2->curCharacterIndex].name, &sx))
                {
                    if (sxNorm && sxNorm != sx)
                    {
                        sprintf(&(tempText[1]),"%s is already a member of another guild.",
                            ca2->charInfoArray[ca2->curCharacterIndex].name);
                        tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                        sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                           strlen(tempText) + 1,(void *)&tempText,2);
                        return;
                    }
                    else
                        sxNorm = sx;
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        if( ranking_members == 0 )
        {
            sprintf( &(tempText[1] ),"You must have a ranking member of the guild present to induct new members." );
            tempText[0] = TEXT_COLOR_DATA;
            sp->SendToEveryoneNearBut(0, x, y, strlen(tempText) + 1,(void *)&tempText,2 );

            return;
        }
        if (!sxNorm)
            return;

        TowerMap *dm = (TowerMap *) sxNorm;

        if (num <= 3 && dm->members->ItemsInList() > num)
            return;

        // and add everyone new to it!
        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                if (ca2->cellX == x && ca2->cellY == y && 
                     !dm->IsMember(ca2->charInfoArray[ca2->curCharacterIndex].name))
                {
                    MemberRecord *mr = new MemberRecord(0,
                               ca2->charInfoArray[ca2->curCharacterIndex].name);
                    dm->members->Append(mr);

                    dm->lastChangedTime.SetToNow();

                    sprintf(&(tempText[1]),"%s is now a member of %s.",
                        ca2->charInfoArray[ca2->curCharacterIndex].name,
                        dm->WhoAmI());
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY,
                                       strlen(tempText) + 1,(void *)&tempText,2);

                    ca2->AssertGuildStatus(sp,TRUE);

                }
            }
            sp->avatars->Find(curMob);
            curMob = (BBOSMob *) sp->avatars->Next();
        }


        dungeonUpdateTime = 0; // save off dungeons and guilds

        ca = (BBOSAvatar *)sp->avatars->First();

    }
//	else if (num > 0 && CHANT_EDIT_TOWER == chantType)
    else if (num > 3 && CHANT_EDIT_TOWER == chantType)
    {
        if (SPACE_GUILD == sp->WhatAmI())
        {
            BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
            while (curMob)
            {
                if (SMOB_AVATAR == curMob->WhatAmI())
                {
                    BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

//					SharedSpace *sx;
                    if (ca2->cellX == x && ca2->cellY == y && ca2->chantType == chantType)
                    {
                        AvatarGuildEdit age;
                        age.avatarID = ca2->socketIndex;

                        tempReceiptList.clear();
                        tempReceiptList.push_back(ca2->socketIndex);

                        lserver->SendMsg(sizeof(age),(void *)&age, 0, &tempReceiptList);
                    }
                }
                curMob = (BBOSMob *) sp->avatars->Next();
            }
        }

        ca = (BBOSAvatar *)sp->avatars->First();

    }
    else if (CHANT_ROP_RESPONSE == chantType)
    {
        
        if (SPACE_GROUND == sp->WhatAmI())
        {
            BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
            while (curMob)
            {
                if (SMOB_AVATAR == curMob->WhatAmI())
                {
                    BBOSAvatar *ca2 = (BBOSAvatar *) curMob;

                    int inPlace = FALSE;
                    if (230 == ca2->cellX &&  53 == ca2->cellY)
                        inPlace = TRUE;
                    if ( 20 == ca2->cellX &&  23 == ca2->cellY)
                        inPlace = TRUE;
                    if (179 == ca2->cellX && 164 == ca2->cellY)
                        inPlace = TRUE;
                    if (107 == ca2->cellX && 162 == ca2->cellY)
                        inPlace = TRUE;

                    // *** temporary, so mods can test this first
//					if (ACCOUNT_TYPE_ADMIN != ca2->accountType &&
//					    ACCOUNT_TYPE_MODERATOR != ca2->accountType)
//						 inPlace = FALSE;

                    if (inPlace &&
                         abs(ca2->cellX-x) < 2 && abs(ca2->cellY-y) < 2 && 
                         ca2->chantType == CHANT_ROP && CanHaveROP(
                            ca2->charInfoArray[ca2->curCharacterIndex].age, 
                             ca2->charInfoArray[ca2->curCharacterIndex].cLevel))
                    {

                        // cut short the ROP's chant
                        ca2->chantType = -1;

                        MessChant messChant;
                        messChant.avatarID = ca2->socketIndex;
                        messChant.r = messChant.b = messChant.g = 0;	 // black means stop
                        sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY, 
                                                   sizeof(messChant), &messChant);

                        // change the ROP person
                        tempReceiptList.clear();
                        tempReceiptList.push_back(ca2->socketIndex);

                        ca2->charInfoArray[ca2->curCharacterIndex].age += 1;

                        ca2->charInfoArray[ca2->curCharacterIndex].healthMax = 20 + ca2->PhysicalStat() * 6 + ca2->charInfoArray[ca2->curCharacterIndex].cLevel; 


                        if( ca2->charInfoArray[ca2->curCharacterIndex].age < 6 )
                            ca2->charInfoArray[ca2->curCharacterIndex].healthMax *= ca2->charInfoArray[ca2->curCharacterIndex].age;
                        else
                        {
                            ca2->charInfoArray[ca2->curCharacterIndex].healthMax *= 5;

                            ca2->charInfoArray[ca2->curCharacterIndex].physical += 1;
                            ca2->charInfoArray[ca2->curCharacterIndex].creative += 1;
                            ca2->charInfoArray[ca2->curCharacterIndex].magical += 1;
                        }

                        MessAvatarStats mStats;
                        ca2->BuildStatsMessage(&mStats);
                        sp->lserver->SendMsg(sizeof(mStats),(void *)&mStats, 0, &tempReceiptList);

                        sprintf(tempText,"You have passed into a new age, that of %s.", 
                                      ageTextArray[ca2->charInfoArray[ca2->curCharacterIndex].age-1] );
                        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                        sp->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                        MessAvatarHealth messHealth;
                        messHealth.avatarID  = ca2->socketIndex;
                        messHealth.health    = ca2->charInfoArray[ca2->curCharacterIndex].health;
                        messHealth.healthMax = ca2->charInfoArray[ca2->curCharacterIndex].healthMax;
                        sp->SendToEveryoneNearBut(0, ca2->cellX, ca2->cellY, sizeof(messHealth), &messHealth);

                        MessGenericEffect messGE;
                        messGE.avatarID = -1;
                        messGE.mobID    = -1;
                        messGE.r        = 40;
                        messGE.g        = 40;
                        messGE.b        = 255;
                        messGE.type     = 0;  // type of particles
                        messGE.timeLen  = 5; // in seconds

                        for (int ly = ca2->cellY-1; ly <= ca2->cellY+1; ++ly)
                        {
                            for (int lx = ca2->cellX-1; lx <= ca2->cellX+1; ++lx)
                            {
                                messGE.x        = lx;
                                messGE.y        = ly;
                                sp->SendToEveryoneNearBut(0, messGE.x, messGE.y, sizeof(messGE),(void *)&messGE);
                            }
                        }

                        // give everyone a goodie
                        BBOSMob *curMob2 = (BBOSMob *) sp->avatars->First();
                        while (curMob2)
                        {
                            if (SMOB_AVATAR == curMob2->WhatAmI())
                            {
                                BBOSAvatar *ca3 = (BBOSAvatar *) curMob2;
                                if (ca2 != ca3 && 
                                     abs(ca2->cellX - ca3->cellX) < 2 && 
                                     abs(ca2->cellY - ca3->cellY) < 2)
                                {
                                    InventoryObject *iObject;

                                    switch(rand() % 3)
                                    {
                                    case 0:
                                        iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Ancient Dragonscale");
                                        iObject->value  = 100000;
                                        break;

                                    case 1:
                                        iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Dragon Orchid");
                                        iObject->value  = 10000;
                                        break;

                                    case 2:
                                        iObject = new InventoryObject(INVOBJ_SIMPLE,0,"Demon Amulet");
                                        iObject->value  = 50000;
                                        break;
                                    }

                                    iObject->amount = 1;
                                    ca3->charInfoArray[ca3->curCharacterIndex].inventory->AddItemSorted(iObject);
                                }

                            }
                            curMob2 = (BBOSMob *) sp->avatars->Next();
                        }

                        // reset the list
                        sp->avatars->Find(curMob);
                    }
                }
                curMob = (BBOSMob *) sp->avatars->Next();
            }
        }
        
        ca = (BBOSAvatar *)sp->avatars->First();

    }


    if (ca)
        sp->avatars->Find(ca);
}


//*******************************************************************************
void BBOServer::HandleDeadGate(int x, int y, SharedSpace *sp)
{
    MessInfoText infoText;
//	char tempText[1024];

    std::vector<TagID> tempReceiptList;
//	tempReceiptList.clear();
//	tempReceiptList.push_back(ca->socketIndex);

    BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
            if (ca2->cellX == x && ca2->cellY == y)
            {

                RealmMap *rp = NULL;
                SharedSpace *sp2 = (SharedSpace *) spaceList->First();
                while (sp2)
                {
                    if (SPACE_REALM == sp2->WhatAmI() && REALM_ID_DEAD == ((RealmMap *)sp2)->type)
                    {
                        rp = (RealmMap *)sp2;
                        sp2 = (SharedSpace *) spaceList->Last();
                    }
                    sp2 = (SharedSpace *) spaceList->Next();
                }

                if (rp)
                {
                    ca2->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

                    // tell my client I'm entering the realm
                    tempReceiptList.clear();
                    tempReceiptList.push_back(ca2->socketIndex);

                    MessChangeMap changeMap;
                    changeMap.realmID = rp->type;
                    changeMap.oldType = sp->WhatAmI(); 
                    changeMap.newType = rp->WhatAmI(); 
                    changeMap.sizeX   = rp->sizeX;
                    changeMap.sizeY   = rp->sizeY;

                    lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                    MessInfoText infoText;
                    CopyStringSafely("You enter the Realm of the Dead.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    ca2->QuestSpaceChange(sp,rp);

                    // move me to my new SharedSpace
                    sp->avatars->Remove(ca2);
                    rp->avatars->Append(ca2);

                    ca2->cellX = ca2->targetCellX = 4;
                    ca2->cellY = ca2->targetCellY = 4;

                    // tell everyone about my arrival
                    ca2->IntroduceMyself(rp, SPECIAL_APP_DUNGEON);

                    // tell this player about everyone else around
                    ca2->UpdateClient(rp, TRUE);
                }

            }
        }
        curMob = (BBOSMob *) sp->avatars->Next();
    }

}


//*******************************************************************************
void BBOServer::HandleSpiritGate(int x, int y, SharedSpace *sp)
{
    MessInfoText infoText;
//	char tempText[1024];

    std::vector<TagID> tempReceiptList;
//	tempReceiptList.clear();
//	tempReceiptList.push_back(ca->socketIndex);

    BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
            if (ca2->cellX == x && ca2->cellY == y)
            {

                RealmMap *rp = NULL;
                SharedSpace *sp2 = (SharedSpace *) spaceList->First();
                while (sp2)
                {
                    if (SPACE_REALM == sp2->WhatAmI() && REALM_ID_SPIRITS == ((RealmMap *)sp2)->type)
                    {
                        rp = (RealmMap *)sp2;
                        sp2 = (SharedSpace *) spaceList->Last();
                    }
                    sp2 = (SharedSpace *) spaceList->Next();
                }

                if (rp)
                {
                    ca2->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

                    // tell my client I'm entering the realm
                    tempReceiptList.clear();
                    tempReceiptList.push_back(ca2->socketIndex);

                    MessChangeMap changeMap;
                    changeMap.realmID = rp->type;
                    changeMap.oldType = sp->WhatAmI(); 
                    changeMap.newType = rp->WhatAmI(); 
                    changeMap.sizeX   = rp->sizeX;
                    changeMap.sizeY   = rp->sizeY;

                    lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                    MessInfoText infoText;
                    CopyStringSafely("You enter the Realm of Spirits.", 
                                          200, infoText.text, MESSINFOTEXTLEN);
                    lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                    ca2->QuestSpaceChange(sp,rp);

                    // move me to my new SharedSpace
                    sp->avatars->Remove(ca2);
                    rp->avatars->Append(ca2);

                    ca2->cellX = ca2->targetCellX = 4;
                    ca2->cellY = ca2->targetCellY = 4;

                    // tell everyone about my arrival
                    ca2->IntroduceMyself(rp, SPECIAL_APP_DUNGEON);

                    // tell this player about everyone else around
                    ca2->UpdateClient(rp, TRUE);
                }

            }
        }
        curMob = (BBOSMob *) sp->avatars->Next();
    }

}


//*******************************************************************************
void BBOServer::ProcessFriend(BBOSAvatar *srcAvatar, int type, 
                                        SharedSpace *ss, char *targetName)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    SharedSpace *sp;
    MessPlayerChatLine chatMess;
    DataObject *fName, *fCurName;
    MessInfoText infoText;

    DoublyLinkedList *holdingList = new DoublyLinkedList();

    int messLen = 0;
    char fullName[300], tempText[1024];
    char fullName2[300];
    int startingLine = TRUE;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(srcAvatar->socketIndex);

    if (FRIEND_ACTION_ADD == type && targetName)
    {
        fName = srcAvatar->contacts->First();
        while (fName)
        {
            if (IsCompletelySame(fName->WhoAmI(), targetName))
            {
                sprintf(tempText,"%s is already a friend.", fName->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            fName = srcAvatar->contacts->Next();
        }

        fName = new DataObject(CONTACT_FRIEND, targetName);
        srcAvatar->contacts->Append(fName);

        sprintf(tempText,"%s is added.", fName->WhoAmI());
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);

        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }

    if (FRIEND_ACTION_REMOVE == type && targetName)
    {
        fName = srcAvatar->contacts->First();
        while (fName)
        {
            if (IsCompletelySame(fName->WhoAmI(), targetName))
            {
                srcAvatar->contacts->Remove(fName);
                sprintf(tempText,"%s is no longer a friend.", fName->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                delete fName;
                return;
            }

            fName = srcAvatar->contacts->Next();
        }

        sprintf(tempText,"%s is not a friend.", targetName);
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }

    if (FRIEND_ACTION_IGNORE == type && targetName)
    {
        fName = srcAvatar->contacts->First();
        while (fName)
        {
            if (IsCompletelySame(fName->WhoAmI(), targetName) && 
                 CONTACT_IGNORE == fName->WhatAmI())
            {
                sprintf(tempText,"%s is no longer ignored.", fName->WhoAmI());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                srcAvatar->contacts->Remove(fName);
                delete fName;

                return;
            }

            fName = srcAvatar->contacts->Next();
        }

        fName = new DataObject(CONTACT_IGNORE, targetName);
        srcAvatar->contacts->Append(fName);

        sprintf(tempText,"%s will be ignored.", fName->WhoAmI());
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);

        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }


    if (FRIEND_ACTION_LIST == type)
    {
        sprintf(&(chatMess.text[messLen]),"Frnds: ");
        messLen = strlen(chatMess.text);
    }

    sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            curAvatar = (BBOSAvatar *) curMob;
            if (SMOB_AVATAR == curMob->WhatAmI() && 
                 curAvatar->socketIndex != srcAvatar->socketIndex)
            {
                int isContact = FALSE;
                fCurName = NULL;

                fName = srcAvatar->contacts->First();
                while (fName)
                {
                    if (FALSE == curAvatar->isInvisible && IsCompletelySame(fName->WhoAmI(),
                         curAvatar->charInfoArray[curAvatar->curCharacterIndex].name))
                    {
                        isContact = TRUE;
                        fCurName = fName;
                        fName = srcAvatar->contacts->Last();
                    }

                    fName = srcAvatar->contacts->Next();
                }

                if (isContact)
                {
                    srcAvatar->contacts->Remove(fCurName);
                    holdingList->Append(fCurName);
                }

                if (FRIEND_ACTION_LIST == type && isContact && 
                     CONTACT_FRIEND == fCurName->WhatAmI())
                {
                    char *name = curAvatar->charInfoArray[curAvatar->curCharacterIndex].name;
                    sprintf(fullName,"%s (on)",name);
                    sprintf(fullName2,", %s (on)",name);

                    for (int testVal = 0; testVal < 1; ++testVal)
                    {
                        if (strlen(fullName2) + messLen > 75)
                        {
                            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                            messLen = 0;
                            startingLine = TRUE;
                        }
                        
                        if (startingLine)
                            sprintf(&(chatMess.text[messLen]),"%s", fullName);
                        else
                            sprintf(&(chatMess.text[messLen]),"%s", fullName2);
                        messLen = strlen(chatMess.text);
                        startingLine = FALSE;
                    }
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        sp = (SharedSpace *) spaceList->Next();
    }

    // tell about offline friends
    fName = srcAvatar->contacts->First();
    while (fName)
    {
        if (FRIEND_ACTION_LIST == type && CONTACT_FRIEND == fName->WhatAmI())
        {
            sprintf(fullName,"%s",fName->WhoAmI());
            sprintf(fullName2,", %s",fName->WhoAmI());

            for (int testVal = 0; testVal < 1; ++testVal)
            {
                if (strlen(fullName2) + messLen > 75)
                {
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                    messLen = 0;
                    startingLine = TRUE;
                }
                
                if (startingLine)
                    sprintf(&(chatMess.text[messLen]),"%s", fullName);
                else
                    sprintf(&(chatMess.text[messLen]),"%s", fullName2);
                messLen = strlen(chatMess.text);
                startingLine = FALSE;
            }
        }
        fName = srcAvatar->contacts->Next();
    }

    // replace held names in srcAvatar's list
    fName = holdingList->First();
    while (fName)
    {
        holdingList->Remove(fName);
        srcAvatar->contacts->Append(fName);
        fName = holdingList->First();
    }

    delete holdingList;

    if (messLen > 0)
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);

}

//*******************************************************************************
void BBOServer::TellBuddiesImHere(BBOSAvatar *srcAvatar)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;
    SharedSpace *sp;
    MessPlayerChatLine chatMess;
    int messLen = 0;
//	char fullName[300];
//	char fullName2[300];
    int startingLine = TRUE;

    std::vector<TagID> tempReceiptList;
//	tempReceiptList.clear();
//	tempReceiptList.push_back(srcAfromSocket);

    sprintf(chatMess.text,"Your friend %s has logged on.",
              srcAvatar->charInfoArray[srcAvatar->curCharacterIndex].name);

    sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        curMob = (BBOSMob *) sp->avatars->First();
        while (curMob)
        {
            if (SMOB_AVATAR == curMob->WhatAmI())
            {
                curAvatar = (BBOSAvatar *) curMob;

                if (curAvatar->IsContact(
                         srcAvatar->charInfoArray[srcAvatar->curCharacterIndex].name, 
                          CONTACT_FRIEND))
                {
                    tempReceiptList.clear();
                    tempReceiptList.push_back(curAvatar->socketIndex);
                    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                }
            }
            curMob = (BBOSMob *) sp->avatars->Next();
        }

        sp = (SharedSpace *) spaceList->Next();
    }

}

//*******************************************************************************
void BBOServer::HandlePetFeeding(MessFeedPetRequest *mess, BBOSAvatar *curAvatar,
                                            SharedSpace *ss)
{
    if( !mess->ptr )
        return;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    char tempText[1024];
    MessInfoText infoText;
    int invType = MESS_WIELD_PLAYER;

    if (mess->which < 0)
        mess->which = 0;
    if (mess->which > 1)
        mess->which = 1;

    // find food item to feed her
    Inventory *src = (curAvatar->charInfoArray[curAvatar->curCharacterIndex].wield);

    InventoryObject *io = (InventoryObject *) src->objects.Find(
                                    (InventoryObject *) mess->ptr);
    if (!io)
    {
        src = (curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory);

        io = (InventoryObject *) src->objects.Find(
                                    (InventoryObject *) mess->ptr);

        invType = MESS_INVENTORY_PLAYER;

    }
    if (!io) 
    {
        src = (curAvatar->charInfoArray[curAvatar->curCharacterIndex].workbench);

        io = (InventoryObject *) src->objects.Find(
                                    (InventoryObject *) mess->ptr);

        invType = MESS_WORKBENCH_PLAYER;
    }
    if (!io) // if didn't find the object at all
    {
        return;
    }

    if (io->type != INVOBJ_MEAT)
        return;

    InvMeat *im = (InvMeat *)io->extra;

    // feed it to her
    PetDragonInfo *dInfo = &(curAvatar->charInfoArray[curAvatar->curCharacterIndex].petDragonInfo[mess->which]);

    LongTime ltNow;
    if (dInfo->lastEatenTime.MinutesDifference(&ltNow) < 20)
    {
        sprintf(tempText,"%s is still full.  She refuses the meat.", dInfo->name);
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }

    dInfo->lastEatenTime.SetToNow();
    dInfo->state = DRAGON_HEALTH_GREAT;

    // record it as the last item fed
    dInfo->lastEatenType    = im->type;
    dInfo->lastEatenSubType = im->quality;

    // did it make her stronger?
    if (dInfo->lifeStage < 3 && 
         dragonInfo[dInfo->quality][dInfo->type].powerMeatType[dInfo->lifeStage] == im->type)
    {
        dInfo->healthModifier += 0.3f;

        sprintf(tempText,"You feed %s; she loves it!", dInfo->name);
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }
    else
    {
        sprintf(tempText,"You feed %s.", dInfo->name);
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }


    if (DRAGON_TYPE_BLACK != dInfo->type)
    {
        if (im->age >= 24 || -2 == im->age) // if rotted
        {
            dInfo->healthModifier -= 0.3f;

            sprintf(tempText,"The rotted meat makes %s gag!", dInfo->name);
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
    }
    else
    {
        if (im->age < 24 && im->age > -2) // if NOT rotted
        {
            dInfo->healthModifier -= 0.3f;

            sprintf(tempText,"The fresh meat makes %s gag!", dInfo->name);
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            ss->lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        }
    }

    MessPet mPet;
    mPet.avatarID = curAvatar->socketIndex;
    CopyStringSafely(dInfo->name,16, 
                  mPet.name,16);
    mPet.quality = dInfo->quality;
    mPet.type    = dInfo->type;
    mPet.state   = dInfo->state;
    mPet.size    = dInfo->lifeStage + dInfo->healthModifier/10.0f;
    mPet.which   = mess->which;

    ss->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY, sizeof(mPet),(void *)&mPet);

    // remove the eaten meat
    --(io->amount);
    if (io->amount < 1)
    {
        src->objects.Remove(io);
        delete io;
    }

    TellClientAboutInventory(curAvatar, invType);
}

//*******************************************************************************
void BBOServer::HandleAdminMessage(MessAdminMessage *mess, BBOSAvatar *curAvatar,
                                            SharedSpace *ss)
{
    MessAdminMessage adminMess;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    char tempText[1024];
    MessInfoText infoText;

    if (MESS_ADMIN_TAKE_CONTROL == mess->messageType)
    {
        if (curAvatar->controlledMonster)
        {
            SharedSpace *sp;

            BBOSMonster * theMonster = FindMonster(
                     curAvatar->controlledMonster, &sp);
            if (theMonster)
            {
                curAvatar->controlledMonster->controllingAvatar = NULL;
                curAvatar->controlledMonster = NULL;

                sprintf(tempText,"You release a monster.");
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
//				return;
            }
        }

        BBOSMob *curMob = (BBOSMob *) ss->mobList->GetFirst(0,0,1000);
        while (curMob)
        {
            if (SMOB_MONSTER == curMob->WhatAmI() && mess->mobID == (long) curMob)
            {
                curAvatar->controlledMonster = (BBOSMonster *) curMob;
                curAvatar->controlledMonster->controllingAvatar = curAvatar;
                curAvatar->controlledMonster->myGenerator = NULL; // detach from armies and spawners

                sprintf(tempText,"You have controlled the %s.",
                    ((BBOSMonster *)curMob)->Name());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }

            curMob = (BBOSMob *) ss->mobList->GetNext();
        }

    }
    else if (MESS_ADMIN_RELEASE_CONTROL == mess->messageType)
    {
        if (curAvatar->controlledMonster)
        {
            curAvatar->controlledMonster->controllingAvatar = NULL;
            curAvatar->controlledMonster = NULL;

            sprintf(tempText,"You release the monster.");
            CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            adminMess.messageType = MESS_ADMIN_RELEASE_CONTROL;
            lserver->SendMsg(sizeof(adminMess),(void *)&adminMess, 0, &tempReceiptList);

            curAvatar->AnnounceDisappearing(ss, SPECIAL_APP_NOTHING);

            // tell everyone about my arrival
            curAvatar->IntroduceMyself(ss, SPECIAL_APP_NOTHING);

            // tell this player about everyone else around
            curAvatar->UpdateClient(ss, TRUE);

            return;
        }

        sprintf(tempText,"No valid monster to release.");
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }


}

//*******************************************************************************
void BBOServer::HandleContMonstString(char *chatText, BBOSAvatar *curAvatar,
                                            SharedSpace *ss)
{
    MessAdminMessage adminMess;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    char tempText[1024];
    MessInfoText infoText;
    int linePoint, argPoint;

    linePoint = 0;
    argPoint = NextWord(chatText,&linePoint);

    if (curAvatar->controlledMonster)
    {
        SharedSpace *sp;

        BBOSMonster * theMonster = FindMonster(
                 curAvatar->controlledMonster, &sp);
        if (theMonster)
        {
            if ( IsSame(&(chatText[argPoint]) , "stat"))
            {
                sprintf(tempText,"Controlling %s", theMonster->Name());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                sprintf(tempText,"health %d/%d, dam %d, toHit %d, def %d, dropVal %d resist %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "health"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->health = theMonster->maxHealth = atoi(&(chatText[argPoint]));

                sprintf(tempText,"HEALTH %d/%d, dam %d, toHit %d, def %d, dropVal %d resist %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "dam"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->damageDone = atoi(&(chatText[argPoint]));

                sprintf(tempText,"health %d/%d, DAM %d, toHit %d, def %d, dropVal %d resist %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "tohit"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->toHit = atoi(&(chatText[argPoint]));

                sprintf(tempText,"health %d/%d, dam %d, TOHIT %d, def %d, dropVal %d resist %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "def"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->defense = atoi(&(chatText[argPoint]));

                sprintf(tempText,"health %d/%d, dam %d, toHit %d, DEF %d, dropVal %d resist %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "resist"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->magicResistance = atoi(&(chatText[argPoint])) / 100.0f;
                if (theMonster->magicResistance < 0)
                    theMonster->magicResistance = 0;
                if (theMonster->magicResistance > 1)
                    theMonster->magicResistance = 1;

                sprintf(tempText,"health %d/%d, dam %d, toHit %d, def %d, dropVal %d RESIST %1.2f",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount,
                    theMonster->magicResistance);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "dropval"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->dropAmount = atoi(&(chatText[argPoint]));

                sprintf(tempText,"health %d/%d, dam %d, toHit %d, def %d, DROPVAL %d",
                    theMonster->health, theMonster->maxHealth, theMonster->damageDone, 
                    theMonster->toHit, theMonster->defense, theMonster->dropAmount);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "name"))
            {
                argPoint = NextWord(chatText,&linePoint);
                CopyStringSafely(&(chatText[argPoint]), 32, 
                                  theMonster->uniqueName, 32);

                sprintf(tempText,"Controlling %s", theMonster->Name());
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                theMonster->AnnounceMyselfCustom(sp);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "size"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->sizeCoeff = atof(&(chatText[argPoint]));

                if (0 == theMonster->uniqueName[0])
                {
                    CopyStringSafely(theMonster->Name(), 32, 
                                      theMonster->uniqueName, 32);
                }
                sprintf(tempText,"Size now %f", theMonster->sizeCoeff);
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                theMonster->AnnounceMyselfCustom(sp);
                return;
            }
            else if ( IsSame(&(chatText[argPoint]) , "color"))
            {
                argPoint = NextWord(chatText,&linePoint);
                theMonster->r = atoi(&(chatText[argPoint]));
                if (theMonster->r < 0)
                    theMonster->r = 0;
                if (theMonster->r > 255)
                    theMonster->r = 255;

                argPoint = NextWord(chatText,&linePoint);
                theMonster->g = atoi(&(chatText[argPoint]));
                if (theMonster->g < 0)
                    theMonster->g = 0;
                if (theMonster->g > 255)
                    theMonster->g = 255;

                argPoint = NextWord(chatText,&linePoint);
                theMonster->b = atoi(&(chatText[argPoint]));
                if (theMonster->b < 0)
                    theMonster->b = 0;
                if (theMonster->b > 255)
                    theMonster->b = 255;

                argPoint = NextWord(chatText,&linePoint);
                theMonster->a = atoi(&(chatText[argPoint]));
                if (theMonster->a < 0)
                    theMonster->a = 0;
                if (theMonster->a > 255)
                    theMonster->a = 255;

                if (0 == theMonster->uniqueName[0])
                {
                    CopyStringSafely(theMonster->Name(), 32, 
                                      theMonster->uniqueName, 32);
                }
                sprintf(tempText,"Color changed!");
                CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                theMonster->AnnounceMyselfCustom(sp);
                return;
            }
            else
            {
                CopyStringSafely("/m commands:", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m stat         - shows info for controlled monster", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m health <val> - sets current and maximum health points", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m dam <val>    - sets minimum damage done to opponent", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m tohit <val>  - sets chance to hit", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m def <val>    - sets chance to dodge attack", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m dropval <val>- sets general loot value", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m name <text>- sets monster's name", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m size <val>- sets size, 1.5 means 50 percent bigger", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m color <r> <g> <b> <a> - sets color, red, green, blue, alpha (0-255 each)", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                CopyStringSafely("/m resist <val>- sets magical resistance value (0-100)", 1024, infoText.text, MESSINFOTEXTLEN);
                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
                return;
            }
        }
        else
        {
            CopyStringSafely("Controlled monster not found.  Perhaps it's dead.", 1024, infoText.text, MESSINFOTEXTLEN);
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
            return;
        }
    }
    else
    {
        CopyStringSafely("No controlled monster.", 1024, infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }

}

//*******************************************************************************
void BBOServer::MaintainIncomingAvatars(void)
{
    BBOSMob *curMob;
    BBOSAvatar *curAvatar;

    DoublyLinkedList *list = incoming;

    curMob = (BBOSMob *) list->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            curAvatar = (BBOSAvatar *) curMob;
            ++(curAvatar->activeCounter);
            if (curAvatar->activeCounter > 25)
            {
                list->Remove(curAvatar);
                delete curAvatar;
            }
        }
        curMob = (BBOSMob *) list->Next();
    }

}

//*******************************************************************************
void BBOServer::HandleSellingAll(BBOSAvatar *curAvatar, BBOSNpc *curNpc, int type)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    char tempText[1024];
    MessInfoText infoText;

    if (abs(townList[2].x - curAvatar->cellX) <= 5 && 
         abs(townList[2].y - curAvatar->cellY) <= 5 && INVOBJ_EARTHKEY == type)
    {
        CopyStringSafely("We don't buy those here.  Please try another town.", 1024, 
                          infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
        return;
    }

    if (INVOBJ_SIMPLE == type || INVOBJ_MEAT == type ||
         INVOBJ_BOMB   == type || INVOBJ_EARTHKEY == type)
    {
        Inventory *inv = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory;
        Inventory *partner = curNpc->inventory;

        assert(inv->money >= 0);

        long moneyMade = 0;
        long itemsSold = 0;

        InventoryObject *io;
        io = (InventoryObject *) inv->objects.First();
        while (io)
        {
            if (type == io->type && io->value <= 1000)
            {
                if (INVOBJ_MEAT == io->type)
                {
                    SaltThisMeat(io);
                }

                inv->objects.Remove(io);
                itemsSold += io->amount;
                moneyMade += io->value * io->amount * 7 / 10;
                partner->AddItemSorted(io);

            }

            io = (InventoryObject *) inv->objects.Next();
        }

        sprintf(tempText,"You sold %ld items for %ld gold.",itemsSold, moneyMade);
        CopyStringSafely(tempText, 1024, infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

        assert(inv->money >= 0);

        inv->money += moneyMade;

        assert(inv->money >= 0);

        TellClientAboutInventory(curAvatar, MESS_INVENTORY_PLAYER);
    }
    else
    {
        CopyStringSafely("Please help us by finding and reporting bugs.  Thank you!", 1024, 
                          infoText.text, MESSINFOTEXTLEN);
        lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);
    }

}


//*******************************************************************************
void BBOServer::HandleExtendedInfo(BBOSAvatar *avatar, 
                                     MessExtendedInfoRequest *requestPtr)
{
//	char tempText[1024];
    MessInfoText infoText;

    if (!requestPtr->itemPtr)
        return;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(avatar->socketIndex);

//	InventoryObject *io2;

    Inventory *inv = NULL;
    switch(requestPtr->listType)
    {
    case MESS_INVENTORY_PLAYER:
        inv =	(avatar->charInfoArray[avatar->curCharacterIndex].inventory);
        break;
    case MESS_WORKBENCH_PLAYER:
        inv =	(avatar->charInfoArray[avatar->curCharacterIndex].workbench);
        break;
    case MESS_SKILLS_PLAYER:
        inv =	(avatar->charInfoArray[avatar->curCharacterIndex].skills);
        break;
    case MESS_WIELD_PLAYER:
        inv =	(avatar->charInfoArray[avatar->curCharacterIndex].wield);
        break;
    case MESS_INVENTORY_TRADER:
    case MESS_INVENTORY_GROUND:
        inv =	(avatar->charInfoArray[avatar->curCharacterIndex].inventory->partner);
        break;
    case MESS_INVENTORY_HER_SECURE:
        if (avatar->tradingPartner)
        {
            SharedSpace *sx;
            BBOSAvatar *partnerAvatar = NULL;
            partnerAvatar = FindAvatar(avatar->tradingPartner, &sx);
            if (partnerAvatar)
                inv =	(partnerAvatar->trade);
        }
        break;
    case MESS_INVENTORY_YOUR_SECURE:
        inv =	(avatar->trade);
        break;
    }

    if (!inv)  // no inventory, no info.
        return;

    LongTime ltNow;

    InventoryObject *io = (InventoryObject *) inv->objects.Find(
                                (InventoryObject *) requestPtr->itemPtr);
    if (io) // if found the object to transfer
    {
        Chronos::BStream *	stream		= NULL;
        stream	= new Chronos::BStream(1024);

        *stream << (unsigned char) NWMESS_EXTENDED_INFO; 

        *stream << (unsigned char) io->type;
        
        stream->write(io->WhoAmI(), strlen(io->WhoAmI()));
        *stream << (unsigned char) 0; 

        *stream << io->value;
        *stream << io->amount;
        *stream << io->status;

        short damValue;
        int minimum;
//		char smlText[128];

        switch(io->type)
        {
        case INVOBJ_BLADE:
            *stream << (short)((InvBlade *)io->extra)->toHit;

            damValue = ((InvBlade *)io->extra)->damageDone * 
                        (1.0f + 
                         avatar->PhysicalStat() * 0.15f) + 
                        avatar->totemEffects.effect[TOTEM_STRENGTH] * 1.0f;

            *stream << damValue;

            //*stream << (unsigned short)((InvBlade *)io->extra)->poison;
            //*stream << (unsigned short)((InvBlade *)io->extra)->heal;
            *stream << (unsigned short)((InvBlade *)io->extra)->slow;
            *stream << (unsigned short)((InvBlade *)io->extra)->blind;
            *stream << (unsigned short)((InvBlade *)io->extra)->lightning;
            *stream << (int)(((InvBlade *)io->extra)->numOfHits);
            *stream << (int)(((InvBlade *)io->extra)->bladeGlamourType);

            break;
        case INVOBJ_TOTEM:
            if (((InvTotem *)io->extra)->isActivated)
                *stream << (long) ((InvTotem *)io->extra)->timeToDie.MinutesDifference(&ltNow) * -1;
            else
                *stream << (long) -1;
            if ( ((InvTotem *)io->extra)->type >= TOTEM_PHYSICAL &&
                  ((InvTotem *)io->extra)->type <= TOTEM_CREATIVE)
                *stream << (short) (((InvTotem *)io->extra)->imbueDeviation);
            else
                *stream << (short) (((InvTotem *)io->extra)->quality - ((InvTotem *)io->extra)->imbueDeviation);
            break;
        case INVOBJ_STAFF:
            if (((InvStaff *)io->extra)->isActivated)
                *stream << (short) ((InvStaff *)io->extra)->charges;
            else
                *stream << (short) -1;
            *stream << (short) ((InvStaff *)io->extra)->imbueDeviation;
            break;
        case INVOBJ_INGOT:
            *stream << ((InvIngot *)io->extra)->damageVal;
            *stream << ((InvIngot *)io->extra)->challenge;
            *stream << (unsigned char) ((InvIngot *)io->extra)->r;
            *stream << (unsigned char) ((InvIngot *)io->extra)->g;
            *stream << (unsigned char) ((InvIngot *)io->extra)->b;
            break;
        case INVOBJ_POTION:
            *stream << ((InvPotion *)io->extra)->type;
            *stream << ((InvPotion *)io->extra)->subType;
            break;
        case INVOBJ_SKILL:
            *stream << ((InvSkill *)io->extra)->skillLevel;
            *stream << ((InvSkill *)io->extra)->skillPoints;
            break;
        case INVOBJ_INGREDIENT:
            *stream << ((InvIngredient *)io->extra)->type;
            *stream << ((InvIngredient *)io->extra)->quality;
            break;
        case INVOBJ_EXPLOSIVE:
            *stream << ((InvExplosive *)io->extra)->type;
            *stream << ((InvExplosive *)io->extra)->quality;
            *stream << ((InvExplosive *)io->extra)->power;
            break;
        case INVOBJ_FUSE:
            *stream << ((InvFuse *)io->extra)->type;
            *stream << ((InvFuse *)io->extra)->quality;
            break;
        case INVOBJ_GEOPART:
            *stream << ((InvGeoPart *)io->extra)->type;
            *stream << ((InvGeoPart *)io->extra)->power;
            break;
        case INVOBJ_EARTHKEY:
            *stream << ((InvEarthKey *)io->extra)->power;
            *stream << ((InvEarthKey *)io->extra)->monsterType[0];
            *stream << ((InvEarthKey *)io->extra)->monsterType[1];
            *stream << ((InvEarthKey *)io->extra)->width;
            *stream << ((InvEarthKey *)io->extra)->height;
            break;
        case INVOBJ_BOMB:
            *stream << (unsigned char) ((InvBomb *)io->extra)->r;
            *stream << (unsigned char) ((InvBomb *)io->extra)->g;
            *stream << (unsigned char) ((InvBomb *)io->extra)->b;
//			*stream << ((InvBomb *)io->extra)->flags;
//			*stream << ((InvBomb *)io->extra)->type;
            *stream << ((InvBomb *)io->extra)->power;
            *stream << ((InvBomb *)io->extra)->stability;
            *stream << ((InvBomb *)io->extra)->fuseDelay;
            break;
        case INVOBJ_EGG:
            *stream << ((InvEgg *)io->extra)->type;

            minimum = 4 + ((InvEgg *)io->extra)->quality + ((InvEgg *)io->extra)->type;
            if (minimum > 10)
                minimum = 10;

            if (avatar->charInfoArray[avatar->curCharacterIndex].magical < minimum)
                *stream << (char) avatar->charInfoArray[avatar->curCharacterIndex].magical;
            else
                *stream << (char) 0;
            break;
        case INVOBJ_MEAT:
            *stream << ((InvMeat *)io->extra)->type;
            *stream << ((InvMeat *)io->extra)->quality;
            *stream << ((InvMeat *)io->extra)->age;
            break;
        }

        lserver->SendMsg(stream->used(), stream->buffer(), 0, &tempReceiptList);

        delete stream;


    }

}


//*******************************************************************************
int BBOServer::FindAvatarInGuild(char *name, SharedSpace **sp)
{
//   BBOSMob *curMob;
//	BBOSAvatar *curAvatar;

    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        if (SPACE_GUILD == (*sp)->WhatAmI())
        {
            MemberRecord *mr = (MemberRecord *) ((TowerMap *)(*sp))->members->First();
            while (mr)
            {
                if (IsCompletelySame(mr->WhoAmI(), name))
                {
                    return TRUE;
                }
                mr = (MemberRecord *) ((TowerMap *)(*sp))->members->Next();
            }
        }
        (*sp) = (SharedSpace *) spaceList->Next();
    }

    return FALSE;
}

MemberRecord* BBOServer::GetRecordForGuildMember( char *name, SharedSpace **sp )
{
    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        if (SPACE_GUILD == (*sp)->WhatAmI())
        {
            MemberRecord *mr = (MemberRecord *) ((TowerMap *)(*sp))->members->First();
            while (mr)
            {
                if (IsCompletelySame(mr->WhoAmI(), name))
                {
                    return mr;
                }
                mr = (MemberRecord *) ((TowerMap *)(*sp))->members->Next();
            }
        }
        (*sp) = (SharedSpace *) spaceList->Next();
    }

    return NULL;
}

//*******************************************************************************
int BBOServer::FindGuild(DWORD nameCRC, SharedSpace **sp)
{
//   BBOSMob *curMob;
//	BBOSAvatar *curAvatar;

    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        if (SPACE_GUILD == (*sp)->WhatAmI() && 0 < strlen((*sp)->WhoAmI()))
        {
            DWORD c = GetCRCForString((*sp)->WhoAmI());
            if (nameCRC == c)
                return TRUE;
        }
        (*sp) = (SharedSpace *) spaceList->Next();
    }

    return FALSE;
}

//*******************************************************************************
int BBOServer::ChangeAvatarGuildName(char *name, char *newName)
{
//   BBOSMob *curMob;
//	BBOSAvatar *curAvatar;

    SharedSpace *sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        if (SPACE_GUILD == (sp)->WhatAmI())
        {
            MemberRecord *mr = (MemberRecord *) ((TowerMap *)(sp))->members->First();
            while (mr)
            {
                if (IsCompletelySame(mr->WhoAmI(), name))
                {
                    sprintf(mr->do_name,newName);
                    return TRUE;
                }
                mr = (MemberRecord *) ((TowerMap *)(sp))->members->Next();
            }
        }
        (sp) = (SharedSpace *) spaceList->Next();
    }

    return FALSE;
}

//*******************************************************************************
int BBOServer::DeleteNameFromGuild(char *name, SharedSpace **sp)
{
//   BBOSMob *curMob;
//	BBOSAvatar *curAvatar;

    (*sp) = (SharedSpace *) spaceList->First();
    while (*sp)
    {
        if (SPACE_GUILD == (*sp)->WhatAmI())
        {
            MemberRecord *mr = (MemberRecord *) ((TowerMap *)(*sp))->members->First();
            while (mr)
            {
                if (IsCompletelySame(mr->WhoAmI(), name))
                {
                    ((TowerMap *)(*sp))->members->Remove(mr);
                    delete mr;
                    return TRUE;
                }
                mr = (MemberRecord *) ((TowerMap *)(*sp))->members->Next();
            }
        }
        (*sp) = (SharedSpace *) spaceList->Next();
    }

    return FALSE;
}

//*******************************************************************************
void BBOServer::SendToEveryGuildMate(char *senderName, SharedSpace *sp, 
                                                 TowerMap *guild, int size, const void *dataPtr)
{
    std::vector<TagID> tempReceiptList;

    tempReceiptList.clear();

    BBOSMob *curMob = (BBOSMob *) sp->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI())
        {
            BBOSAvatar *curAvatar = (BBOSAvatar *) curMob;
            if (guild->IsMember(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name))
            {
                if (!curAvatar->IsContact(senderName, CONTACT_IGNORE))
                    tempReceiptList.push_back(curAvatar->socketIndex);
            }
        }
        curMob = (BBOSMob *) sp->avatars->Next();
    }

    lserver->SendMsg(size, dataPtr, 0, &tempReceiptList);

}

//*******************************************************************************
void BBOServer::ListGuild(BBOSAvatar *curAvatar, TowerMap *guild)
{
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    int messLen = 0;
//	char tempText[1024];
    char fullName[300], tempName[300];
    char fullName2[300];
    int startingLine = TRUE;

    MessPlayerChatLine chatMess;
    sprintf(&(chatMess.text[messLen]),"%s: %s", 
                 guildStyleNames[guild->guildStyle], guild->WhoAmI());
    messLen = strlen(chatMess.text);

    sprintf(&(chatMess.text[messLen])," (spec %d/%d/%d)", 
               guild->specLevel[0], guild->specLevel[1], guild->specLevel[2]);
    messLen = strlen(chatMess.text);

    lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
    messLen = 0;
    startingLine = TRUE;

    MemberRecord *mr = (MemberRecord *) guild->members->First();
    while (mr)
    {

        char *name = mr->WhoAmI();
        if (mr->value1 < 1)
            sprintf(tempName, name);
        else
        {
            sprintf(tempName, "%s %s", guildLevelNames[guild->guildStyle-1][mr->value1-1],
                     name);
        }
        sprintf(fullName,"%s",tempName);
        sprintf(fullName2,", %s",tempName);

        for (int testVal = 0; testVal < 1; ++testVal)
        {
            if (strlen(fullName2) + messLen > 70)
            {
                lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
                messLen = 0;
                startingLine = TRUE;
            }
            
            if (startingLine)
                sprintf(&(chatMess.text[messLen]),"%s", fullName);
            else
                sprintf(&(chatMess.text[messLen]),"%s", fullName2);
            messLen = strlen(chatMess.text);
            startingLine = FALSE;
        }
        mr = (MemberRecord *) guild->members->Next();
    }


    if (messLen > 0)
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
}


//*******************************************************************************
void BBOServer::ListVotes(BBOSAvatar *curAvatar, TowerMap *guild)
{
    if (!curAvatar || !guild)
        return;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    int messLen = 0;
    char tempText[1024];
//	char fullName[300];
//	char fullName2[300];
    int startingLine = TRUE;

    MessPlayerChatLine chatMess;
    MessInfoText       infoText;

    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
    tempText[1] = TEXT_COLOR_DATA;

    for (int i = 0; i < 4; ++i)
    {
        if (GUILDBILL_INACTIVE == guild->bills[i].type)
        {
            sprintf(&(tempText[2]),"%d: unused.", i+1);
            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
        }
        else
        {
            int yesVotes, allVotes;
            yesVotes = allVotes = 0;

            MemberRecord *mr = (MemberRecord *) guild->bills[i].recordedVotes.First();
            while(mr)
            {
                ++allVotes;
                if (mr->WhatAmI())
                    ++yesVotes;
                mr = (MemberRecord *) guild->bills[i].recordedVotes.Next();
            }

            int percent = 0;
            if (allVotes > 0)
                percent = yesVotes * 100 / allVotes;

            sprintf(&(tempText[2]),"%d: %s %s (%d%%).", i+1,
                         guildBillNames[guild->bills[i].type], 
                            guild->bills[i].subject, 
                            percent);
            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

            LongTime now;
            long diff = now.MinutesDifference(&(guild->bills[i].expTime));
            if (diff > 0)
            {
                sprintf(&(tempText[2]),"%ld minutes left to vote (%ld hours).",
                          diff, diff/60);
                lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            }
            else
            {
                if (VOTESTATE_PASSED == guild->bills[i].voteState)
                    sprintf(&(tempText[2]),"Bill PASSED %ld hours ago.",diff/-60);
                else
                    sprintf(&(tempText[2]),"Bill was DEFEATED %ld hours ago.",diff/-60);

                lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            }
        }
    }
}


//*******************************************************************************
void BBOServer::DetailVote(int i, BBOSAvatar *curAvatar, TowerMap *guild)
{
    if (!curAvatar || !guild)
        return;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    int messLen = 0;
    char tempText[1024];
//	char fullName[300];
//	char fullName2[300];
    int startingLine = TRUE;

    MessPlayerChatLine chatMess;
    MessInfoText       infoText;

    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
    tempText[1] = TEXT_COLOR_DATA;

    sprintf(&(tempText[2]),"%d: %s", i+1, guildBillDesc[guild->bills[i].type]);
    lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

    if (GUILDBILL_INACTIVE != guild->bills[i].type)
    {
        int yesVotes, allVotes;
        yesVotes = allVotes = 0;

        MemberRecord *mr = (MemberRecord *) guild->bills[i].recordedVotes.First();
        while(mr)
        {
            ++allVotes;
            if (mr->WhatAmI())
                ++yesVotes;
            mr = (MemberRecord *) guild->bills[i].recordedVotes.Next();
        }

        sprintf(&(tempText[2]),"Sponsor: %s.  Subject: %s.", 
            guild->bills[i].sponsor, guild->bills[i].subject);
        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

        int percent = 0;
        if (allVotes > 0)
            percent = yesVotes * 100 / allVotes;

        sprintf(&(tempText[2]),"%d votes tallied, yes %d, no %d (%d%%).", 
                     allVotes, yesVotes, allVotes - yesVotes, percent);
        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

        LongTime now;
        long diff = now.MinutesDifference(&(guild->bills[i].expTime));
        if (diff > 0)
        {
            sprintf(&(tempText[2]),"%ld minutes left to vote (%ld hours).",
                      diff, diff/60);
            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
        }
        else
        {
            if (VOTESTATE_PASSED == guild->bills[i].voteState)
                sprintf(&(tempText[2]),"Bill PASSED %ld hours ago.",diff/-60);
            else
                sprintf(&(tempText[2]),"Bill was DEFEATED %ld hours ago.",diff/-60);

            lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
        }
    }
    
}

char voteTypeText[2][12] =
{
    {"No"},
    {"Yes"}
};

//*******************************************************************************
void BBOServer::VoteOnBill(int voteVal, int i, BBOSAvatar *curAvatar, TowerMap *guild)
{
    if (!curAvatar || !guild)
        return;

    if (voteVal != 0)
        voteVal = 1;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    int messLen = 0;
    char tempText[1024];
//	char fullName[300];
//	char fullName2[300];
    int startingLine = TRUE;

    MessPlayerChatLine chatMess;
    MessInfoText       infoText;

    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
    tempText[1] = TEXT_COLOR_DATA;

//	sprintf(&(tempText[2]),"%d: %s", i, guildBillDesc[guild->bills[i].type]);
//	lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

    LongTime now;
    long diff = now.MinutesDifference(&(guild->bills[i].expTime));
    if (diff > 0 && GUILDBILL_INACTIVE != guild->bills[i].type)
    {
        int yesVotes, allVotes;
        yesVotes = allVotes = 0;

        MemberRecord *myVote = NULL;
        MemberRecord *mr = (MemberRecord *) guild->bills[i].recordedVotes.First();
        while(mr)
        {
            if (IsCompletelySame(curAvatar->charInfoArray[curAvatar->curCharacterIndex].name,
                                  mr->WhoAmI()))
                 myVote = mr;
            mr = (MemberRecord *) guild->bills[i].recordedVotes.Next();
        }

        if (myVote)
        {
            sprintf(&(tempText[2]),"You've changed your vote on bill %d to %s.", 
                i+1, voteTypeText[voteVal]);
            myVote->do_id = voteVal;
        }
        else
        {
            sprintf(&(tempText[2]),"You vote %s on bill %d.", 
                voteTypeText[voteVal], i+1);

            myVote = new MemberRecord(voteVal, 
                            curAvatar->charInfoArray[curAvatar->curCharacterIndex].name);
            myVote->do_id = voteVal;
            guild->bills[i].recordedVotes.Append(myVote);
        }

        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
    }
    else
    {
        sprintf(&(tempText[2]),"You cannot vote on this bill."); 
        lserver->SendMsg(strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
    }

    
}


//*******************************************************************************
void BBOServer::AttemptToStartVote(BBOSAvatar *curAvatar, TowerMap *guild, 
                                              char *subjectName, char *type)
{
    if (!curAvatar || !guild)
        return;

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    int messLen = 0;
    char tempText[1024];
//	char fullName[300];
//	char fullName2[300];
    int startingLine = TRUE;

    MessPlayerChatLine chatMess;
    MessInfoText       infoText;

    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
    tempText[1] = TEXT_COLOR_DATA;

    int highestRank = 0;
    MemberRecord *mr2 = NULL;
    MemberRecord *mr = (MemberRecord *) guild->members->First();
    while (mr)
    {
        if (IsCompletelySame(
                   curAvatar->charInfoArray[curAvatar->curCharacterIndex].name, mr->WhoAmI()))
            mr2 = mr;

        if (mr->value1 > highestRank)
            highestRank = mr->value1;

        mr = (MemberRecord *) guild->members->Next();
    }

    if (!mr2)
    {
        sprintf(&chatMess.text[1],"You're not in your own guild.  Internal Error 1.");
        chatMess.text[0] = TEXT_COLOR_DATA;
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
        return;
    }

    // is there a valid slot for you to start a vote in?
    int votes = 0, commonVotes = 0, openIndex = -1;

    int isRoyalSponsor = mr2->value1;

    int commonTop = commonVotesAllowed[guild->guildStyle-1];

    int i = 0;
    for (; i < 4; ++i)
    {
        if (GUILDBILL_INACTIVE == guild->bills[i].type && -1 == openIndex)
        {
            if (isRoyalSponsor && i >= commonTop)
                openIndex = i;
            else if (!isRoyalSponsor && i < commonTop)
                openIndex = i;
            else if (0 == highestRank)
                openIndex = i;
        }
    }

    if (-1 == openIndex && isRoyalSponsor)
    {
        sprintf(&chatMess.text[1],"None of the %d vote slots for ranking sponsors are open.",
                        4 - commonTop);
        chatMess.text[0] = TEXT_COLOR_DATA;
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
        return;
    }

    if (-1 == openIndex && !isRoyalSponsor)
    {
        sprintf(&chatMess.text[1],"None of the %d vote slots for unranked sponsors are open.",
                        commonTop);
        chatMess.text[0] = TEXT_COLOR_DATA;
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
        return;
    }

    i = openIndex;

    // for each type
    guild->bills[i].type = GUILDBILL_INACTIVE;

    if (!stricmp(type, "PROMOTE"))
        guild->bills[i].type = GUILDBILL_PROMOTE;
    if (!stricmp(type, "DEMOTE"))
        guild->bills[i].type = GUILDBILL_DEMOTE;
    if (!stricmp(type, "KICK"))
        guild->bills[i].type = GUILDBILL_KICKOUT;
    if (!stricmp(type, "CHANGESTYLE"))
        guild->bills[i].type = GUILDBILL_CHANGESTYLE;
    if (!stricmp(type, "CHANGENAME"))
        guild->bills[i].type = GUILDBILL_CHANGENAME;
    if (!stricmp(type, "FIGHTER"))
        guild->bills[i].type = GUILDBILL_FIGHTER_SPEC;
    if (!stricmp(type, "MAGE"))
        guild->bills[i].type = GUILDBILL_MAGE_SPEC;
    if (!stricmp(type, "CRAFTER"))
        guild->bills[i].type = GUILDBILL_CRAFTER_SPEC;
        
    if (GUILDBILL_INACTIVE == guild->bills[i].type)
    {
        sprintf(&chatMess.text[1],"%s is not a valid vote type.",type);
        chatMess.text[0] = TEXT_COLOR_DATA;
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
        sprintf(&chatMess.text[1],"Try PROMOTE, DEMOTE, KICK, CHANGESTYLE, CHANGENAME, FIGHTER, MAGE, or CRAFTER.");
        chatMess.text[0] = TEXT_COLOR_DATA;
        lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
        return;
    }

    if (GUILDBILL_CHANGESTYLE != guild->bills[i].type &&
         GUILDBILL_CHANGENAME  != guild->bills[i].type)
    {
        // make sure the subjectName IS a guild member
        SharedSpace *guildSpace;
        if (!FindAvatarInGuild(subjectName, &guildSpace))
        {
            sprintf(&chatMess.text[1],"%s is not in the guild.", subjectName);
            chatMess.text[0] = TEXT_COLOR_DATA;
            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
            guild->bills[i].type = GUILDBILL_INACTIVE;
            return;
        }
        if (guildSpace && guild != guildSpace)
        {
            sprintf(&chatMess.text[1],"%s is not in YOUR guild.", subjectName);
            chatMess.text[0] = TEXT_COLOR_DATA;
            lserver->SendMsg(sizeof(chatMess),(void *)&chatMess, 0, &tempReceiptList);
            guild->bills[i].type = GUILDBILL_INACTIVE;
            return;
        }
    }

    guild->bills[i].expTime.SetToNow();
    guild->bills[i].expTime.AddMinutes(60 * 24);
    guild->bills[i].sponsorLevel = mr2->value1;
    guild->bills[i].voteState = VOTESTATE_VOTING;
    CopyStringSafely(mr2->WhoAmI(), 64, guild->bills[i].sponsor, 64);
    CopyStringSafely(subjectName  , 64, guild->bills[i].subject, 64);

    mr = (MemberRecord *) guild->bills[i].recordedVotes.First();
    while (mr)
    {
        guild->bills[i].recordedVotes.Remove(mr);
        delete mr;

        mr = (MemberRecord *) guild->bills[i].recordedVotes.First();
    }

    sprintf(&(tempText[2]),"A new guild bill has been introduced!");

    SharedSpace *sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        SendToEveryGuildMate("NILLNILL",
                sp, guild, strlen(tempText) + 1,(void *)&tempText);
        sp = (SharedSpace *) spaceList->Next();
    }
    sprintf(&(tempText[2]),"Sponsor: %s.  Subject: %s.", 
        guild->bills[i].sponsor, guild->bills[i].subject);

    sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        SendToEveryGuildMate("NILLNILL",
                sp, guild, strlen(tempText) + 1,(void *)&tempText);
        sp = (SharedSpace *) spaceList->Next();
    }
    sprintf(&(tempText[2]),"%d: %s", i+1, guildBillDesc[guild->bills[i].type]);

    sp = (SharedSpace *) spaceList->First();
    while (sp)
    {
        bboServer->SendToEveryGuildMate("NILLNILL",
                sp, guild, strlen(tempText) + 1,(void *)&tempText);
        sp = (SharedSpace *) spaceList->Next();
    }
    
}

//*******************************************************************************
void BBOServer::SaltThisMeat(InventoryObject *io)
{
    char tempText[1024];

    InvMeat *im = (InvMeat *) io->extra;
    if (im->age >= 0)
    {
        if (im->age >= 24)
        {
            im->age = -2;
            sprintf(tempText,"Slt %s",io->WhoAmI());
            CopyStringSafely(tempText, 1024, io->do_name, DO_NAME_LENGTH);
            io->value *= 2;
        }
        else
        {
            im->age = -1;
            sprintf(tempText,"Slt %s",io->WhoAmI());
            CopyStringSafely(tempText, 1024, io->do_name, DO_NAME_LENGTH);
            io->value *= 2;
        }
    }
}

//*******************************************************************************
void BBOServer::CheckGraveyard(SharedSpace *ss, int gX, int gY)
{
    std::vector<TagID> tempReceiptList;

    for (int i = gY; i <= gY+1; ++i)
    {
        for (int j = gX; j <= gX+1; ++j)
        {
            Inventory *inv = ss->GetGroundInventory(j, i);
            int orchidCount = 0;
            InventoryObject *iObject = (InventoryObject *) inv->objects.First();
            while (iObject)
            {
                if (INVOBJ_SIMPLE == iObject->type && 
                     !strncmp("Dragon Orchid", iObject->WhoAmI(), strlen("Dragon Orchid"))
                    )
                    orchidCount += iObject->amount;

                iObject = (InventoryObject *) inv->objects.Next();
            }

            if (orchidCount > 0)
            {
                // okay, first, destroy the orchids
                iObject = (InventoryObject *) inv->objects.First();
                while (iObject)
                {
                    if (INVOBJ_SIMPLE == iObject->type && 
                         !strncmp("Dragon Orchid", iObject->WhoAmI(), strlen("Dragon Orchid"))
                        )
                    {
                        inv->objects.Remove(iObject);
                        delete iObject;
                        iObject = (InventoryObject *) inv->objects.First();
                    }
                    else
                        iObject = (InventoryObject *) inv->objects.Next();
                }

                // now, teleport anyone on this square to the dragon realm
                BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
                while (curMob)
                {
                    if (SMOB_AVATAR == curMob->WhatAmI())
                    {
                        BBOSAvatar *ca2 = (BBOSAvatar *) curMob;
                        if (ca2->cellX == j && ca2->cellY == i)
                        {

                            RealmMap *rp = NULL;
                            SharedSpace *sp2 = (SharedSpace *) spaceList->First();
                            while (sp2)
                            {
                                if (SPACE_GROUND == ss->WhatAmI() && SPACE_REALM == sp2->WhatAmI() && 
                                     REALM_ID_DRAGONS == ((RealmMap *)sp2)->type)
                                {
                                    rp = (RealmMap *)sp2;
                                    sp2 = (SharedSpace *) spaceList->Last();
                                }
                                sp2 = (SharedSpace *) spaceList->Next();
                            }

                            if (rp)
                            {
                                ca2->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

                                // tell my client I'm entering the realm
                                tempReceiptList.clear();
                                tempReceiptList.push_back(ca2->socketIndex);

                                MessChangeMap changeMap;
                                changeMap.realmID = rp->type;
                                changeMap.oldType = ss->WhatAmI(); 
                                changeMap.newType = rp->WhatAmI(); 
                                changeMap.sizeX   = rp->sizeX;
                                changeMap.sizeY   = rp->sizeY;

                                lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

                                MessInfoText infoText;
                                CopyStringSafely("You enter the Realm of Dragons.", 
                                                      200, infoText.text, MESSINFOTEXTLEN);
                                lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

                                ca2->QuestSpaceChange(ss,rp);

                                // move me to my new SharedSpace
                                ss->avatars->Remove(ca2);
                                rp->avatars->Append(ca2);

                                ca2->cellX = ca2->targetCellX = (rand() % 4) + 2;
                                ca2->cellY = ca2->targetCellY = (rand() % 4) + 2;

                                // tell everyone about my arrival
                                ca2->IntroduceMyself(rp, SPECIAL_APP_DUNGEON);

                                // tell this player about everyone else around
                                ca2->UpdateClient(rp, TRUE);
                            }

                        }
                    }
                    curMob = (BBOSMob *) ss->avatars->Next();
                }
            }
        }
    }
}


//*******************************************************************************
void BBOServer::HandleKeyCode(BBOSAvatar *avatar, MessKeyCode *keyCodePtr)
{
    // Game is free, no codes
}


//*******************************************************************************
void BBOServer::HandleKarmaText(char *string, BBOSAvatar *curAvatar, BBOSAvatar *targetAv)
{

    // KARMA
    if (StringContainsThanks(string))
    {
        int myRelationship, herRelationship;

        curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

        curAvatar->charInfoArray[curAvatar->curCharacterIndex].
              karmaGiven[myRelationship][SAMARITAN_TYPE_THANKS] += 1;
        targetAv->charInfoArray[targetAv->curCharacterIndex].
              karmaReceived[herRelationship][SAMARITAN_TYPE_THANKS] += 1;

        curAvatar->LogKarmaExchange(
                     targetAv, myRelationship, herRelationship, 
                     SAMARITAN_TYPE_THANKS, string);

        if (curAvatar->IsAGuildMate(targetAv))
        {
            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_THANKS] += 1;
            targetAv->charInfoArray[targetAv->curCharacterIndex].
                  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_THANKS] += 1;

            curAvatar->LogKarmaExchange(
                         targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
                         SAMARITAN_TYPE_THANKS, string);
        }
    }
      
    if (StringContainsWelcome(string))
    {
        int myRelationship, herRelationship;

        curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

        curAvatar->charInfoArray[curAvatar->curCharacterIndex].
              karmaGiven[myRelationship][SAMARITAN_TYPE_WELCOME] += 1;
        targetAv->charInfoArray[targetAv->curCharacterIndex].
              karmaReceived[herRelationship][SAMARITAN_TYPE_WELCOME] += 1;

        curAvatar->LogKarmaExchange(
                     targetAv, myRelationship, herRelationship, 
                     SAMARITAN_TYPE_WELCOME, string);

        if (curAvatar->IsAGuildMate(targetAv))
        {
            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_WELCOME] += 1;
            targetAv->charInfoArray[targetAv->curCharacterIndex].
                  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_WELCOME] += 1;

            curAvatar->LogKarmaExchange(
                         targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
                         SAMARITAN_TYPE_WELCOME, string);
        }
    }

    if (StringContainsPlease(string))
    {
        int myRelationship, herRelationship;

        curAvatar->CompareWith(targetAv, myRelationship, herRelationship);

        curAvatar->charInfoArray[curAvatar->curCharacterIndex].
              karmaGiven[myRelationship][SAMARITAN_TYPE_PLEASE] += 1;
        targetAv->charInfoArray[targetAv->curCharacterIndex].
              karmaReceived[herRelationship][SAMARITAN_TYPE_PLEASE] += 1;

        curAvatar->LogKarmaExchange(
                     targetAv, myRelationship, herRelationship, 
                     SAMARITAN_TYPE_PLEASE, string);

        if (curAvatar->IsAGuildMate(targetAv))
        {
            curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                  karmaGiven[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_PLEASE] += 1;
            targetAv->charInfoArray[targetAv->curCharacterIndex].
                  karmaReceived[SAMARITAN_REL_GUILD][SAMARITAN_TYPE_PLEASE] += 1;

            curAvatar->LogKarmaExchange(
                         targetAv, SAMARITAN_REL_GUILD, SAMARITAN_REL_GUILD, 
                         SAMARITAN_TYPE_PLEASE, string);
        }
    }
    // END KARMA
}

//*******************************************************************************
void BBOServer::AddWarpPair(SharedSpace *s1, int x1, int y1,
                                     SharedSpace *s2, int x2, int y2, 
                                     int allUse)
{
    BBOSWarpPoint *wp = new BBOSWarpPoint(x1,y1);
    wp->targetX      = x2;
    wp->targetY      = y2;
    wp->spaceType    = s2->WhatAmI();
    wp->spaceSubType = 0;
    wp->allCanUse    = allUse;
    if (SPACE_REALM == s2->WhatAmI())
        wp->spaceSubType = ((RealmMap *) s2)->type;
    if (SPACE_LABYRINTH == s2->WhatAmI())
        wp->spaceSubType = ((LabyrinthMap *) s2)->type;
    s1->mobList->Add(wp);

    wp = new BBOSWarpPoint(x2,y2);
    wp->targetX      = x1;
    wp->targetY      = y1;
    wp->spaceType    = s1->WhatAmI();
    wp->spaceSubType = 0;
    if (SPACE_REALM == s1->WhatAmI())
        wp->spaceSubType = ((RealmMap *) s1)->type;
    if (SPACE_LABYRINTH == s1->WhatAmI())
        wp->spaceSubType = ((LabyrinthMap *) s1)->type;
    s2->mobList->Add(wp);

}

//*******************************************************************************
void BBOServer::CreateTreeQuest(Quest *quest, BBOSAvatar *curAvatar, 
                                          SharedSpace *sp,BBOSTree *t, int forcedType)
{
    char tempText[1024];
    char tempText2[128];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    QuestPart *qp;

    int done;
    int tx, ty;
    int allDone = FALSE;
    int lowestMagic;
    InvSkill *skillInfo = NULL;
    InventoryObject *io;
    RealmMap *rp;
    DungeonMap *dp;
    SharedSpace *sp2;

    while (!allDone)
    {
        quest->EmptyOut();
        quest->completeVal = 0;  // active, but not complete
        quest->timeLeft.SetToNow();
        quest->timeLeft.AddMinutes(60*24); // add one day

        quest->questSource = t->index;  // 0-8 is the great trees

        if (-1 == forcedType)
            forcedType = rand() % 13;

        switch(forcedType)
//		switch(12)
        {
        case 0:	 // go-to location
        default:
            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_GOTO;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_LOCATION;
            
            done = FALSE;
            while (!done)
            {
                tx = (rand() % 245) + 5;
                ty = (rand() % 245) + 5;
                if (sp->CanMove(tx, ty, tx, ty))
                    done = TRUE;
            }

            qp->x = tx;
            qp->y = ty;
            qp->mapType = sp->WhatAmI();
            qp->mapSubType = 0; // needs to be correct
            qp->range = 0;

            sprintf(&(tempText[2]),"I felt a disturbance at %dN %dE.  Please go there, see if anything's happening, then report back to me.",
                                            256-ty,256-tx);
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 1:	 // kill monster type

            if (MAGIC_MOON == t->index ||
                 MAGIC_TURTLE == t->index)
                 break;

            if ((MAGIC_SUN == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_KILL;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_MONSTER_TYPE;
            
            done = FALSE;
            while (!done)
            {
                qp->monsterType    = rand() % NUM_OF_MONSTERS;
                qp->monsterSubType = rand() % NUM_OF_MONSTER_SUBTYPES;

                if (monsterData[qp->monsterType][qp->monsterSubType].name[0])
                    done = TRUE;
                if (7 == qp->monsterType && qp->monsterSubType > 2)
                    done = FALSE;
                if (MONSTER_PLACE_LABYRINTH == 
                     monsterData[qp->monsterType][qp->monsterSubType].placementFlags)
                    done = FALSE;
            }

            sprintf(&(tempText[2]),"As a test of your power, vanquish a %s in the name of my Mistress.",
                                      monsterData[qp->monsterType][qp->monsterSubType].name);
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 2:	 // go up to another player and talk

            if (MAGIC_SNAKE == t->index ||
                 MAGIC_TURTLE == t->index)
                 break;

            if ((MAGIC_EAGLE == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_VISIT;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_PLAYER;
            
            qp->playerType = rand() % QUEST_PLAYER_TYPE_MAX;

            switch(qp->playerType)
            {
            case QUEST_PLAYER_TYPE_FIGHTER:
                sprintf(&(tempText[2]),"Go to and bless one who has chosen the warrior path, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_MAGE   :
                sprintf(&(tempText[2]),"Go to and bless one who embraces the mystical powers, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_CRAFTER:
                sprintf(&(tempText[2]),"Go to and bless a creative soul in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_BALANCED:
                sprintf(&(tempText[2]),"Go to and bless one who walks all paths equally, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_YOUNG  :
                sprintf(&(tempText[2]),"Go to and bless a newcomer to our world, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_POOR   :
                sprintf(&(tempText[2]),"Go to and bless a poor woman in the name of my Mistress.");
                break;
            }

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 3:	 // craft a weapon

            if (MAGIC_MOON == t->index ||
                 MAGIC_BEAR == t->index)
                 break;

            if ((MAGIC_SNAKE == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_CRAFT;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_WEAPON;

            // find and check the Swordsmith skill!
            skillInfo = NULL;
            io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                       skills->objects.First();
            while (io)
            {
                if (!strcmp("Swordsmith",io->WhoAmI()))
                {
                    skillInfo = (InvSkill *) io->extra;
                }
                io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                             skills->objects.Next();
            }

            if (!skillInfo || skillInfo->skillLevel < 20)
                qp->playerType = QUEST_WEAPON_TYPE_SWORD;
            else
                qp->playerType = rand() % QUEST_WEAPON_TYPE_MAX;

            sprintf(&(tempText[2]),"It would please my Mistress to have you create a fine %s and sacrifice it to me.",
                questWeaponTypeDesc[qp->playerType]);

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 4:	 // imbue a totem

            if (MAGIC_WOLF == t->index ||
                MAGIC_SUN  == t->index ||
                 MAGIC_BEAR == t->index)
                 break;

            if ((MAGIC_EAGLE == t->index) && (rand() % 2))
                 break;

            if ((MAGIC_FROG == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_IMBUE;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_TOTEM;

            qp->playerType = rand() % TOTEM_PHYSICAL;

            sprintf(&(tempText[2]),"It would please my Mistress to have you imbue a perfect %s totem and sacrifice it to me.",
                totemTypeName[qp->playerType]);

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 5:	 // imbue a staff

            if (MAGIC_FROG == t->index ||
                MAGIC_EAGLE == t->index ||
                 MAGIC_BEAR == t->index)
                 break;

            if ((MAGIC_WOLF == t->index) && (rand() % 2))
                 break;

            // find and check the magic skills!
            lowestMagic = 1000;
            skillInfo = NULL;

            for (tx = 0; tx < MAGIC_MAX; ++tx)
            {
                sprintf(tempText,"%s Magic",magicNameList[tx]);
                io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                                  skills->objects.First();
                while (io)
                {
                    if (IsSame(tempText,io->WhoAmI()))
                    {
                        skillInfo = (InvSkill *) io->extra;
                        if (lowestMagic > skillInfo->skillLevel)
                            lowestMagic = skillInfo->skillLevel;
                    }
                    io = (InventoryObject *) curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                                                         skills->objects.Next();
                }
            }

            if (lowestMagic < 6 || lowestMagic > 990)
                break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_IMBUE;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_STAFF;

            qp->playerType = rand() % STAFF_MAX;

            sprintf(&(tempText[2]),"It would please my Mistress to have you imbue a perfect %s staff and sacrifice it to me.",
                staffTypeName[qp->playerType]);

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 6:	 // go up to another player and give them money

            if (MAGIC_SNAKE == t->index ||
                MAGIC_EAGLE == t->index ||
                 MAGIC_WOLF == t->index)
                 break;

            if ((MAGIC_SUN == t->index) && (rand() % 2))
                 break;

            if ((MAGIC_FROG == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_GIVEGOLD;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_PLAYER;
            
            qp->range = curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money / 10;
            if (qp->range < 0 || qp->range > 10000)
                qp->range = 10000;

            qp->playerType = rand() % QUEST_PLAYER_TYPE_MAX;

            switch(qp->playerType)
            {
            case QUEST_PLAYER_TYPE_FIGHTER:
                sprintf(&(tempText[2]),"Go to and give alms to one who has chosen the warrior path, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_MAGE   :
                sprintf(&(tempText[2]),"Go to and give alms to one who embraces the mystical powers, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_CRAFTER:
                sprintf(&(tempText[2]),"Go to and give alms to a creative soul in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_BALANCED:
                sprintf(&(tempText[2]),"Go to and give alms to one who walks all paths equally, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_YOUNG  :
                sprintf(&(tempText[2]),"Go to and give alms to a newcomer to our world, in the name of my Mistress.");
                break;
            case QUEST_PLAYER_TYPE_POOR   :
                sprintf(&(tempText[2]),"Go to and give alms to a poor woman in the name of my Mistress.");
                break;
            }

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 7:	 // retrieve from location
            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_RETRIEVE;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_LOCATION;

            qp->mapSubType = rand() % (REALM_ID_DRAGONS+1);
            
            rp = NULL;
            sp2 = (SharedSpace *) spaceList->First();
            while (sp2)
            {
                if (SPACE_REALM == sp2->WhatAmI() && qp->mapSubType == ((RealmMap *)sp2)->type)
                {
                    rp = (RealmMap *)sp2;
                    sp2 = (SharedSpace *) spaceList->Last();
                }
                sp2 = (SharedSpace *) spaceList->Next();
            }

            if (rp)
            {
                done = FALSE;
                while (!done)
                {
                    tx = (rand() % (rp->width -8)) + 4;
                    ty = (rand() % (rp->height-8)) + 4;
                    if (rp->CanMove(tx, ty, tx, ty))
                        done = TRUE;
                }

                qp->x = tx;
                qp->y = ty;
                qp->mapType = SPACE_REALM;
                qp->range = 0;

                sprintf(&(tempText[2]),"Most embarrassing.  My Mistress dropped a trifle at %dN %dE in the %s. Please go there, search the ground, and bring the item back to me.",
                                                64-ty,64-tx, rp->WhoAmI());
                tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                tempText[1] = TEXT_COLOR_TELL;
                lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                allDone = TRUE;
            }
            break;

        case 8:	 // retrieve from location
            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_RETRIEVE;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_NPC;

            do
            {
                qp->monsterType = rand() % MAGIC_MAX;
            } while (qp->monsterType == quest->questSource);
            
            sprintf(&(tempText[2]),"The Great Tree of the %s has something my Mistress wants.  Please go to the Tree and bring the item back to me.",
                                         magicNameList[qp->monsterType]);
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 9:	 // kill monster (specific)

            if (MAGIC_MOON == t->index)
                 break;
            if (MAGIC_SNAKE == t->index)
                 break;

            if ((MAGIC_TURTLE == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_KILL;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_LOCATION;
            
            CreateDemonPrince(sp, curAvatar, qp, tempText2);

            sprintf(&(tempText[2]),"The Demon King grows bold, and his brood have invaded the darkest places under our land. I can feel one right now in the %s.  Destroy it for my Mistress.",
                                      tempText2);
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 10:	 // collect stuff and sacrifice it

            if (MAGIC_BEAR == t->index ||
                MAGIC_WOLF == t->index ||
                MAGIC_FROG == t->index ||
                 MAGIC_TURTLE == t->index)
                 break;

            if ((MAGIC_SUN == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_COLLECT;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            if (rand() % 2)
            {
                qp->type = QUEST_TARGET_EGG;
                qp->monsterType = rand() % (DRAGON_TYPE_NUM-1);
                qp->monsterSubType = 1;
                sprintf(&(tempText[2]),"You seek a challenge?  Then find and give to me %d %s.",
                    qp->monsterSubType,
                    dragonInfo[0][qp->monsterType].eggName);
            }
            else
            {
                qp->type = QUEST_TARGET_DUST;
                qp->monsterType = rand() % INGR_WHITE_SHARD;
                if (0 == qp->monsterType || 2 == qp->monsterType)
                    qp->monsterType = 1;
                qp->monsterSubType = 1;
                sprintf(&(tempText[2]),"You seek a challenge?  Then find and give to me %d %s.",
                    qp->monsterSubType,
                    dustNames[qp->monsterType]);
            }

            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 11:	 // get together on a spot with others and say a power word

            if (MAGIC_SNAKE == t->index)
                 break;

            if ((MAGIC_EAGLE == t->index) && (rand() % 2))
                 break;

            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_GROUP;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_LOCATION;
            qp->mapType = SPACE_GROUND;

            switch(rand() % 4)
            {
            case 0:
            default:
                qp->x = 230;
                qp->y = 53;
                break;
            case 1:
                qp->x = 20;
                qp->y = 23;
                break;
            case 2:
                qp->x = 179;
                qp->y = 164;
                break;
            case 3:
                qp->x = 107;
                qp->y = 162;
                break;
            }

            sprintf(&(tempText[2]),"Honor my Mistress by going to the Ring and praying with two others.");
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;
            break;

        case 12: // escape dungeon

            if (MAGIC_MOON == t->index)
                 break;
            if (MAGIC_SUN == t->index)
                 break;

            if ((MAGIC_TURTLE == t->index) && (rand() % 2))
                 break;
            if ((MAGIC_BEAR == t->index) && (rand() % 2))
                 break;


            // now add questParts
            qp = new QuestPart(QUEST_PART_VERB, "VERB");
            quest->parts.Append(qp);
            qp->type = QUEST_VERB_ESCAPE;

            qp = new QuestPart(QUEST_PART_TARGET, "TARGET");
            quest->parts.Append(qp);

            qp->type = QUEST_TARGET_SPACE;
            qp->mapType = SPACE_DUNGEON;
            qp->mapSubType = SPACE_GROUND;
            
            dp = NULL;
            sp2 = NULL;
            while (!dp)
            {
                if (!sp2)
                    sp2 = (SharedSpace *) spaceList->First();

                if (SPACE_DUNGEON == sp2->WhatAmI() && !(rand() % 40))
                    dp = (DungeonMap *)sp2;

                sp2 = (SharedSpace *) spaceList->Next();
            }

            sprintf(&(tempText[2]),"You seek a challenge?  Then get out of here alive, and I'll reward you!");
            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
            tempText[1] = TEXT_COLOR_TELL;
            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
            allDone = TRUE;

            curAvatar->AnnounceDisappearing(sp, SPECIAL_APP_DUNGEON);

            // tell my client I'm entering the dungeon
            MessChangeMap changeMap;
            changeMap.dungeonID = (long) dp;
            changeMap.oldType = sp->WhatAmI();
            changeMap.newType = dp->WhatAmI();
            changeMap.sizeX   = dp->sizeX;
            changeMap.sizeY   = dp->sizeY;
            changeMap.flags   = MESS_CHANGE_NOTHING;
            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

            // move me to my new SharedSpace
            sp->avatars->Remove(curAvatar);
            dp->avatars->Append(curAvatar);

            curAvatar->cellX = 0;
            curAvatar->cellY = 0;

            // tell everyone about my arrival
            curAvatar->IntroduceMyself(dp, SPECIAL_APP_DUNGEON);

            // tell this player about everyone else around
            curAvatar->UpdateClient(dp, TRUE);
            allDone = TRUE;
            break;

        }

        forcedType = rand() % 13;

    }
}

//*******************************************************************************
int BBOServer::ResolveTreeQuests(BBOSAvatar *curAvatar, 
                                          SharedSpace *sp,BBOSTree *t)
{
    char tempText[1024];

    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(curAvatar->socketIndex);

    for (int i = 0; i < QUEST_SLOTS; ++i)
    {
        Quest *quest = &(curAvatar->charInfoArray[curAvatar->curCharacterIndex].quests[i]);

        if (10000 == quest->completeVal)  // if quest complete
        {
            if (quest->questSource == t->index) // quest came from this tree
            {
                int giveFavor = FALSE;
                QuestPart *qp = quest->GetVerb();
                if (qp && (QUEST_VERB_IMBUE == qp->type || 
                            QUEST_VERB_CRAFT == qp->type || 
                              QUEST_VERB_RETRIEVE == qp->type))
                {
                    QuestPart *qt = quest->GetTarget();
                    if (qt)
                    {
                        if (curAvatar->SacrificeQuestItem(qt->type, qt->playerType))
                        {
                            if (QUEST_VERB_RETRIEVE == qp->type)
                                sprintf(&(tempText[2]),"My Mistress thanks you for retrieving this item.  Please accept this favor as your reward.");
                            else
                                sprintf(&(tempText[2]),"My Mistress thanks you for the gift of this item.  Please accept this favor as your reward.");
                            giveFavor = TRUE;

                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

  //							curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money += 100;
                        }
                        else
                        {
                            if (QUEST_VERB_RETRIEVE == qp->type)
                                sprintf(&(tempText[2]),"You found the item I told you of, but you no longer have it to give to me.  I am disappointed.");
                            else
                                sprintf(&(tempText[2]),"You created the item I asked, but you no longer have it to give to me.  I am saddened.");

                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                        }
                    }
                }
                else
                {
                    sprintf(&(tempText[2]),"Thank you for your work on behalf of my Mistress.  Please accept this favor as your reward.");
                    tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                    tempText[1] = TEXT_COLOR_TELL;
                    lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);

                    giveFavor = TRUE;
//					curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->money += 100;
                }

                if (giveFavor)
                {
                    if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                         cLevel >= 10)
                    {
                        sprintf(tempText,"%s Favor", magicNameList[t->index]);

                        InventoryObject *iObject = 
                             new InventoryObject(INVOBJ_FAVOR, 0, tempText);
                        iObject->mass = 1.0f;
                        iObject->value = 10;
                        iObject->amount = 1;

                        ((InvFavor *) iObject->extra)->spirit = t->index;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->
                                          AddItemSorted(iObject);
                    }
                    else
                    {
                        InventoryObject *iObject = 
                             new InventoryObject(INVOBJ_SIMPLE, 0, "Paper Favor");
                        iObject->mass = 1.0f;
                        iObject->value = 200;
                        iObject->amount = 1;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->
                                          AddItemSorted(iObject);
                    }

                    MessGenericEffect messGE;
                    messGE.avatarID = curAvatar->socketIndex;
                    messGE.mobID    = 0;
                    messGE.x        = curAvatar->cellX;
                    messGE.y        = curAvatar->cellY;
                    messGE.r        = 100;
                    messGE.g        = 100;
                    messGE.b        = 255;
                    messGE.type     = 0;  // type of particles
                    messGE.timeLen  = 2; // in seconds
                    sp->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      sizeof(messGE),(void *)&messGE);

                }

                quest->EmptyOut();
                curAvatar->SaveAccount();
                return TRUE;
            }
        }
        else // incomplete quest...
        {
            if (quest->questSource == t->index) // quest came from this tree
            {
                int giveFavor = FALSE;
                QuestPart *qp = quest->GetVerb();
                if (qp && (QUEST_VERB_COLLECT == qp->type))
                {
                    int amount = 0, total = 0;

                    QuestPart *qt = quest->GetTarget();
                    switch(qt->type)
                    {
                    case QUEST_TARGET_EGG:
                    default:
                        do 
                        {
                            amount = curAvatar->SacrificeQuestItem(qt->type, qt->monsterType);
                            total += amount;
                        } while(amount && quest->completeVal + total < qt->monsterSubType);

                        quest->completeVal += total;
                        if (quest->completeVal >= qt->monsterSubType)
                        {
                            sprintf(&(tempText[2]),"I now take from you all the eggs I asked for.  Please accept this favor as your reward.");
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                            giveFavor = TRUE;
                        }
                        else if (total > 0)
                        {
                            sprintf(&(tempText[2]),"I now take from you %d of the eggs I asked for.  Your task is still to bring me %d more.",
                                         total, qt->monsterSubType - quest->completeVal);
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                            curAvatar->SaveAccount();
                        }

                        break;

                    case QUEST_TARGET_DUST:
                        do 
                        {
                            amount = curAvatar->SacrificeQuestItem(qt->type, qt->monsterType);
                            total += amount;
                        } while(amount && quest->completeVal + total < qt->monsterSubType);

                        quest->completeVal += total;
                        if (quest->completeVal >= qt->monsterSubType)
                        {
                            sprintf(&(tempText[2]),"I now take from you all the dust I asked for.  Please accept this favor as your reward.");
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                            giveFavor = TRUE;
                        }
                        else if (total > 0)
                        {
                            sprintf(&(tempText[2]),"I now take from you %d of the dust I asked for.  Your task is still to bring me %d more.",
                                         total, qt->monsterSubType - quest->completeVal);
                            tempText[0] = NWMESS_PLAYER_CHAT_LINE;
                            tempText[1] = TEXT_COLOR_TELL;
                            lserver->SendMsg( strlen(tempText) + 1,(void *)&tempText, 0, &tempReceiptList);
                            curAvatar->SaveAccount();
                        }

                        break;
                    }
                }

                if (giveFavor)
                {
                    if (curAvatar->charInfoArray[curAvatar->curCharacterIndex].
                         cLevel >= 10)
                    {
                        sprintf(tempText,"%s Favor", magicNameList[t->index]);

                        InventoryObject *iObject = 
                             new InventoryObject(INVOBJ_FAVOR, 0, tempText);
                        iObject->mass = 1.0f;
                        iObject->value = 10;
                        iObject->amount = 1;

                        ((InvFavor *) iObject->extra)->spirit = t->index;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->
                                          AddItemSorted(iObject);
                    }
                    else
                    {
                        InventoryObject *iObject = 
                             new InventoryObject(INVOBJ_SIMPLE, 0, "Paper Favor");
                        iObject->mass = 1.0f;
                        iObject->value = 200;
                        iObject->amount = 1;
                        curAvatar->charInfoArray[curAvatar->curCharacterIndex].inventory->
                                          AddItemSorted(iObject);
                    }

                    quest->EmptyOut();
                    curAvatar->SaveAccount();

                    MessGenericEffect messGE;
                    messGE.avatarID = curAvatar->socketIndex;
                    messGE.mobID    = 0;
                    messGE.x        = curAvatar->cellX;
                    messGE.y        = curAvatar->cellY;
                    messGE.r        = 100;
                    messGE.g        = 100;
                    messGE.b        = 255;
                    messGE.type     = 0;  // type of particles
                    messGE.timeLen  = 2; // in seconds
                    sp->SendToEveryoneNearBut(0, curAvatar->cellX, curAvatar->cellY,
                                      sizeof(messGE),(void *)&messGE);

                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}


//******************************************************************
void BBOServer::CreateDemonPrince(SharedSpace *ss, BBOSAvatar *curAvatar, 
                                             QuestPart *qt, char *tempText2)
{

    float totalPlayerPower = 0;
    //	totals up their power
    //		power = dodge skill + age + sword damage + sword to-hit + sword magic
    totalPlayerPower += 
             curAvatar->charInfoArray[curAvatar->curCharacterIndex].lifeTime / 20;

    totalPlayerPower += 
        curAvatar->GetDodgeLevel();

    totalPlayerPower += curAvatar->BestSwordRating();

    totalPlayerPower /= 2;

    //	randomly picks a dungeon
    SharedSpace *tempss = (SharedSpace *) spaceList->First();
    SharedSpace *dungeonPicked = NULL;
    while (!dungeonPicked)
    {
        if (SPACE_DUNGEON == tempss->WhatAmI() && 0 == ((DungeonMap *) tempss)->specialFlags )
        {
            if (36 == rand() % 100)
            {
                DungeonMap *dm = (DungeonMap *) tempss;
//				if (!IsCompletelySame(dm->masterName, curAvatar->name))
                    dungeonPicked = tempss;
            }
        }
        tempss = (SharedSpace *) spaceList->Next();
        if (!tempss)
            tempss = (SharedSpace *) spaceList->First();
    }

    //	creates a Possessed monster in the back of the the front half of the dungeon
    int mPosX = rand() % ((DungeonMap *)dungeonPicked)->width;
    int mPosY = rand() % ((DungeonMap *)dungeonPicked)->height;

    qt->monsterType    = 0; // means a demon mino
    qt->monsterSubType = totalPlayerPower;
    qt->mapType = SPACE_DUNGEON;
    qt->x = mPosX;
    qt->y = mPosY;

    DungeonMap *dm = (DungeonMap *)dungeonPicked;
    qt->mapSubType = GetCRCForString(dm->name);
    sprintf(tempText2, dm->name);
}


//******************************************************************
BBOSAvatar *BBOServer::FindAvatarByStringName(char *string, int &length, SharedSpace **retSpace) 
{
    int done = FALSE;
    int found = FALSE;
    int argPoint = 0;
    int linePoint = 0;
    SharedSpace *sp;
    char tempText[512];

    BBOSAvatar *retAvatar = NULL, *rAv;

    while (!found && !done)
    {
        argPoint = NextWord(string,&linePoint);
        if (argPoint == linePoint)
            done = TRUE;
        else
        {
            memcpy(tempText,string, linePoint);
            int back = 1;
            tempText[linePoint] = 0;
            while (' ' == tempText[linePoint-back] && linePoint-back > 0)
            {
                tempText[linePoint-back] = 0;
                ++back;
            }
            rAv = FindAvatarByAvatarName((char *)tempText, &sp);
            if (rAv)
            {
                retAvatar = rAv;
                length = linePoint;
                *retSpace = sp;
            }
        }
    }

    return retAvatar;
}

//******************************************************************
void BBOServer::HandleEarthKeyUse(BBOSAvatar *ca, InvEarthKey *iek, SharedSpace *ss) 
{
    char tempText[1024];
    std::vector<TagID> tempReceiptList;
    tempReceiptList.clear();
    tempReceiptList.push_back(ca->socketIndex);
    BBOSMonster *monster;

    if( iek->monsterType[0] == 27 || iek->monsterType[0] == 28 )
        iek->monsterType[0] = 0;

    if( iek->monsterType[1] == 27 || iek->monsterType[1] == 28 )
        iek->monsterType[1] = 0;

    // make the new space
    DungeonMap *dm = new DungeonMap(SPACE_DUNGEON,"Test Dungeon",lserver);
    dm->specialFlags = SPECIAL_DUNGEON_TEMPORARY;
    dm->InitNew(iek->width,iek->height,ca->cellX, ca->cellY, 0);
    sprintf(dm->name, "%s's %s %s", 
        ca->charInfoArray[ca->curCharacterIndex].name,
       rdNameAdjective[rand() % 6], rdNamePre [rand() % 7]);

    dm->tempPower = iek->power;

    spaceList->Append(dm);
    int col = 0;

    float change   = 1 + iek->power/20;
    float size1    = 1 + iek->power/50;
    float size2    = 1 + iek->power/80;
    float resist = Bracket(iek->power/100.0f, 0.0f, 1.0f);

    int t  = iek->monsterType[0];

    // add static monsters
    for (int m = 0; m < (dm->height * dm->width) / 6;)
    {
        int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

        if (monsterData[t][t2].name[0])
        {
            int mx, my;
            do
            {
                mx = rand() % (dm->width);
                my = rand() % (dm->height);
            } while (mx < 1);

            monster = new BBOSMonster(t,t2, NULL);
            monster->cellX = mx;
            monster->cellY = my;
            monster->targetCellX = mx;
            monster->targetCellY = my;
            monster->spawnX = mx;
            monster->spawnY = my;

            monster->damageDone = monster->damageDone * change;
            monster->defense    = monster->defense    * change;
            monster->dropAmount = monster->dropAmount * change;
            monster->health     = monster->health     * change;
            monster->maxHealth  = monster->maxHealth  * change;
            monster->toHit      = monster->toHit      * change;

            monster->magicResistance = resist;

            dm->mobList->Add(monster);
            ++m;
        }
    }
    
    if (iek->monsterType[1] > -1)
        t = iek->monsterType[1];

    for (int m = 0; m < (dm->height * dm->width) / 6;)
    {
        int t2 = rand() % NUM_OF_MONSTER_SUBTYPES;

        if (monsterData[t][t2].name[0])
        {
            int mx, my;
            do
            {
                mx = rand() % (dm->width);
                my = rand() % (dm->height);
            } while (mx < 1);

            monster = new BBOSMonster(t,t2, NULL);
            monster->cellX = mx;
            monster->cellY = my;
            monster->targetCellX = mx;
            monster->targetCellY = my;
            monster->spawnX = mx;
            monster->spawnY = my;

            monster->damageDone = monster->damageDone * change;
            monster->defense    = monster->defense    * change;
            monster->dropAmount = monster->dropAmount * change;
            monster->health     = monster->health     * change;
            monster->maxHealth  = monster->maxHealth  * change;
            monster->toHit      = monster->toHit      * change;

            monster->magicResistance = resist;

            dm->mobList->Add(monster);
            ++m;
        }
    }

    // add boss monster
    int i = 0;
    for (i = NUM_OF_MONSTER_SUBTYPES-1; 
         i >= 0 && 0 == monsterData[t][i].name[0]; 
          --i)
        ;

    monster = new BBOSMonster(t,i, NULL); 
    sprintf(monster->uniqueName,"Sentinel");
    monster->sizeCoeff = size1;

    monster->damageDone = monster->damageDone * change * 2;
    monster->defense    = monster->defense    * change * 2;
    monster->dropAmount = monster->dropAmount * change * 2;
    monster->health     = monster->health     * change * 2;
    monster->maxHealth  = monster->maxHealth  * change * 2;
    monster->toHit      = monster->toHit      * change * 2;

    monster->magicResistance = Bracket(resist, 0.5f, 1.0f);
    monster->r = 0;
    monster->g = 0;
    monster->b = 255;

    monster->targetCellX = monster->spawnX = monster->cellX = dm->width-3;
    monster->targetCellY = monster->spawnY = monster->cellY = 0;

    dm->mobList->Add(monster);

    // sometimes add a Vagabond monster
    if (!(rand() % 3))
    {
        int vagIndex = t + 1;

        if (11 == vagIndex || 16 == vagIndex || 
             23 == vagIndex || 21 == vagIndex || 
             20 == vagIndex || vagIndex >= NUM_OF_MONSTERS)
            vagIndex = 0;

        for (i = NUM_OF_MONSTER_SUBTYPES-1; 
              i >= 0 && 0 == monsterData[vagIndex][i].name[0]; 
              --i)
            ;

        monster = new BBOSMonster( vagIndex,i, NULL, true );
        sprintf(monster->uniqueName,"Vagabond");
        monster->sizeCoeff = size2;

        monster->damageDone = monster->damageDone * change * 1.5f;
        monster->defense    = monster->defense    * change * 1.5f;
        monster->dropAmount = monster->dropAmount * change * 1.5f;
        monster->health     = monster->health     * change * 1.5f;
        monster->maxHealth  = monster->maxHealth  * change * 1.5f;
        monster->toHit      = monster->toHit      * change * 1.5f;

        monster->magicResistance = Bracket(resist, 0.8f, 1.0f);
        monster->r = 0;
        monster->g = 255;
        monster->b = 255;

        if (rand() % 2)
        {
            monster->targetCellX = monster->spawnX = monster->cellX = dm->width *3/4;
            monster->targetCellY = monster->spawnY = monster->cellY = dm->height*3/4;
        }
        else
        {
            monster->targetCellX = monster->spawnX = monster->cellX = dm->width /4;
            monster->targetCellY = monster->spawnY = monster->cellY = dm->height/4;
        }
        dm->mobList->Add(monster);

        monster->AddPossessedLoot(1 + iek->power/10);

        BBOSGroundEffect *bboGE = new BBOSGroundEffect();

        bboGE->type = 4;
        bboGE->amount = 3;
        bboGE->r = 255;
        bboGE->g = 255;
        bboGE->b = 0;
        bboGE->cellX = monster->cellX;
        bboGE->cellY = monster->cellY;

        dm->mobList->Add(bboGE);

    }

    // add a portal out of it
    BBOSWarpPoint *wp = new BBOSWarpPoint(dm->width-1,0);
    wp->targetX      = dm->enterX;
    wp->targetY      = dm->enterY;
    wp->spaceType    = SPACE_GROUND;
    wp->spaceSubType = 0;
    dm->mobList->Add(wp);

    BBOSChest *chest = new BBOSChest(dm->width-2,0);
    dm->mobList->Add(chest);

    if (2 == iek->monsterType[0] && 26 == iek->monsterType[1])
    {
        // add a portal to the Deep Laby
        wp = new BBOSWarpPoint(0,0);
        wp->targetX      = 27;
        wp->targetY      = 25;
        wp->spaceType    = SPACE_LABYRINTH;
        wp->spaceSubType = REALM_ID_LAB2;
        wp->allCanUse    = TRUE;
        dm->mobList->Add(wp);
    }

    if (8 == iek->monsterType[0] && 24 == iek->monsterType[1])
    {
        // add a portal to the Laby
        wp = new BBOSWarpPoint(0,0);
        wp->targetX      = 25;
        wp->targetY      = 25;
        wp->spaceType    = SPACE_LABYRINTH;
        wp->spaceSubType = REALM_ID_LAB1;
        wp->allCanUse    = TRUE;
        dm->mobList->Add(wp);
    }

    if (9 == iek->monsterType[0] && 22 == iek->monsterType[1])
    {
        // add a portal to the Dragon realm
        wp = new BBOSWarpPoint(0,0);
        wp->targetX      = 57;
        wp->targetY      = 6;
        wp->spaceType    = SPACE_REALM;
        wp->spaceSubType = REALM_ID_DRAGONS;
        wp->allCanUse    = TRUE;
        dm->mobList->Add(wp);
    }

    if (5 == iek->monsterType[0] && 12 == iek->monsterType[1])
    {
        // add a portal to the spirit realm
        wp = new BBOSWarpPoint(0,0);
        wp->targetX      = 61;
        wp->targetY      = 8;
        wp->spaceType    = SPACE_REALM;
        wp->spaceSubType = REALM_ID_SPIRITS;
        wp->allCanUse    = TRUE;
        dm->mobList->Add(wp);
    }

    if (7 == iek->monsterType[0] && 19 == iek->monsterType[1])
    {
        // add a portal to the dead realm
        wp = new BBOSWarpPoint(0,0);
        wp->targetX      = 37;
        wp->targetY      = 5;
        wp->spaceType    = SPACE_REALM;
        wp->spaceSubType = REALM_ID_DEAD;
        wp->allCanUse    = TRUE;
        dm->mobList->Add(wp);
    }

    // teleport everyone inside
    BBOSMob *curMob = (BBOSMob *) ss->avatars->First();
    while (curMob)
    {
        if (SMOB_AVATAR == curMob->WhatAmI() && curMob->cellX == dm->enterX &&
             curMob->cellY == dm->enterY)
        {
            BBOSAvatar *anAvatar = (BBOSAvatar *) curMob;

            tempReceiptList.clear();
            tempReceiptList.push_back(anAvatar->socketIndex);

            // tell everyone I'm dissappearing
            anAvatar->AnnounceDisappearing(ss, SPECIAL_APP_DUNGEON);

            // tell my client I'm entering the dungeon
            MessChangeMap changeMap;
            changeMap.dungeonID = (long) dm;
            changeMap.oldType = ss->WhatAmI();
            changeMap.newType = dm->WhatAmI();
            changeMap.sizeX   = ((DungeonMap *) dm)->width;
            changeMap.sizeY   = ((DungeonMap *) dm)->height;
            changeMap.flags   = MESS_CHANGE_TEMP;

            lserver->SendMsg(sizeof(changeMap),(void *)&changeMap, 0, &tempReceiptList);

            MessInfoText infoText;
            sprintf(tempText,"You enter the %s.", ((DungeonMap *) dm)->name);
            memcpy(infoText.text, tempText, MESSINFOTEXTLEN-1);
            infoText.text[MESSINFOTEXTLEN-1] = 0;
            lserver->SendMsg(sizeof(infoText),(void *)&infoText, 0, &tempReceiptList);

            anAvatar->QuestSpaceChange(ss,dm);

            // move me to my new SharedSpace
            ss->avatars->Remove(anAvatar);
            dm->avatars->Append(anAvatar);

            anAvatar->targetCellX = anAvatar->cellX = 0;//((DungeonMap *) dm)->width-1;
            anAvatar->targetCellY = anAvatar->cellY = ((DungeonMap *) dm)->height-1;

            // tell everyone about my arrival
            anAvatar->IntroduceMyself(dm, SPECIAL_APP_DUNGEON);

            // tell this player about everyone else around
            anAvatar->UpdateClient(dm, TRUE);

            curMob = (BBOSMob *) ss->avatars->First();
        }
        else
            curMob = (BBOSMob *) ss->avatars->Next();
    }


}



/* end of file */
