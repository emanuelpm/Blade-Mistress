
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOClient.h"
#include "BBOmob.h"
#include ".\network\NetWorldMessages.h"
#include "MonsterData.h"

#include "particle2.h"
#include "particle3.h"
#include "particleScale.h"
#include "particleSmoke.h"
#include "particleRot.h"
#include "pointGlow.h"
#include "glowQuad.h"

#include "simpleGrass.h"
#include "staffData.h"
#include "sharedSpace.h"
#include "3DInfoRecord.h"
#include "BBOShadows.h"
#include "BBOSquare.h"

#include "clientOnly.h"
#include ".\helper\autolog.h"

extern long gGamePlayerLevel, gGamePlayerHP, gGamePlayerMaxHP;
extern char gGamePlayerAge, gTeleportHappenedFlag;

extern Client *	lclient;

extern int POMSezQuit;

//extern long screenHealth, screenMaxHealth, moveLockoutTimer;
extern long moveLockoutTimer;

// from optionMode
extern unsigned long playerInfoFlags;
extern unsigned long localInfoFlags;

extern void DisplayTextInfo(char *text);

Particle2Type		*flareParticle = NULL;
Particle2Type		*magicPlaceParticle = NULL;
Particle2Type		*swordParticle = NULL;
Particle2Type		*driftDownParticle = NULL;
Particle2Type		*eventParticle = NULL;
Particle2Type		*petHitParticle = NULL;
Particle2Type		*superSplashParticle = NULL;
Particle3Type		*mistParticle = NULL;
ParticleScaleType *bombFireParticle = NULL;
ParticleSmokeType	*smokeParticle = NULL;
ParticleRotType	*spellParticle = NULL;

PointGlowSet   	*haloGlowSet = NULL;
GlowQuadType      *bladeTrail = NULL;

SimpleGrass *simpleGrass[4];

float bombShakeVal = 0;

int curMapType, playerIsAdmin;
long lastDungeonID = 0, lastRealmID = -1;

char monsterSoundFileNames[NUM_OF_MONSTERS][32] =
{
	{"dat\\hurtGhost.wav"},
	{"dat\\hurtGolem.wav"},
	{"dat\\hurtMinotaur.wav"},
	{"dat\\hurtTiger.wav"},
	{"dat\\hurtSkeleton.wav"},
	{"dat\\hurtPlant.wav"},
	{"dat\\hurtWerewolf.wav"},
	{"dat\\hurtDragon.wav"},
	{"dat\\hurtSpider.wav"},
	{"dat\\hurtGhost.wav"},

	{"dat\\hurtGolem.wav"},
	{"dat\\hurtMinotaur.wav"},
	{"dat\\hurtTiger.wav"},
	{"dat\\hurtSkeleton.wav"},
	{"dat\\hurtPlant.wav"},
	{"dat\\hurtWerewolf.wav"},
	{"dat\\hurtDragon.wav"},

	{"dat\\hurtSpider.wav"},
	{"dat\\hurtGhost.wav"},
	{"dat\\hurtSkeleton.wav"},

	{"dat\\hurtWerewolf.wav"},
	{"dat\\hurtGhost.wav"},
	{"dat\\hurtMinotaur.wav"},
	{"dat\\hurtDragon.wav"},
	{"dat\\hurtSpider.wav"},
	{"dat\\hurtGhost.wav"},

	{"dat\\hurtSpider.wav"},
	{"dat\\hurtGhost.wav"},
	{"dat\\hurtGhost.wav"}
};

char bombSoundFileNames[7][32] =
{
	{"dat\\puff.wav"},
	{"dat\\small.wav"},
	{"dat\\smallFar.wav"},
	{"dat\\big.wav"},
	{"dat\\bigFar.wav"},
	{"dat\\nuke.wav"},
	{"dat\\nukeFar.wav"}
};

char weatherSoundFileNames[3][32] =
{
	{"dat\\thunder1.wav"},
	{"dat\\thunder2.wav"},
	{"dat\\thunder3.wav"}
};

//******************************************************************
BBOClient::BBOClient(void)
{
	DrawPleaseWaitScreen();

	LoadOptions();

	lineOverlayPtr = NULL;
	lineOverlayGroundPtr = NULL;

	// init camera position
	puma->pumaCamera->spacePoint.angle = -1.0f * D3DX_PI/2;
	puma->pumaCamera->spacePoint.azimuth = -0.37f;
	puma->pumaCamera->spacePoint.location.x = 165.0f;
	puma->pumaCamera->spacePoint.location.y = -66.0f;
	puma->pumaCamera->spacePoint.location.z = -125.0f;

	SetEnvironment();

	skyDome = new PumaMesh();
	skyDome->LoadFromASC(puma->m_pd3dDevice, "dat\\skydome.ase");
	skyDome->LoadTexture(puma->m_pd3dDevice,	"dat\\sky.png");

	ground = new GroundObjectTiles();
	ground->Generate(puma->m_pd3dDevice, MAP_SIZE_WIDTH,MAP_SIZE_HEIGHT,0);
	ground->LoadTexture( puma->m_pd3dDevice, "dat\\grassfull.png" );
	ground->CreateStaticPositions();
	DrawPleaseWaitScreen();


//	lsMan      = new LandShadowManager(ground);
	lSquareMan = new LandSquareManager(ground);

	curMapType = SPACE_GROUND;
	isEditingDungeon = isInSpecialDungeon = drunkenWalk = FALSE;
	needToName       = FALSE;
	playerIsAdmin    = FALSE;

	controlledMonsterID = -1;

	dungeon = NULL;
	realm = NULL;
	labyrinth = NULL;

	cameraType = 0;

   flockSet = new FlockSet();

	dummyAvatar = new BBOMob(0,"PLAYER");
	playerAvatar = dummyAvatar;

	playerAvatar->flockPointIndex = flockSet->MakePoint(playerAvatar);

	FlockPoint *fp = flockSet->GetPoint(playerAvatar->flockPointIndex, playerAvatar);

	playerAvatar->cellX = playerAvatar->cellY = 40;
	playerAvatar->spacePoint.angle = 0;
	playerAvatar->spacePoint.azimuth = 0;
	playerAvatar->spacePoint.location.x = ground->GetXForPoint(playerAvatar->cellX);
	playerAvatar->spacePoint.location.z = ground->GetZForPoint(playerAvatar->cellY);
	playerAvatar->spacePoint.location.y = HeightAtPoint(playerAvatar->spacePoint.location.x, playerAvatar->spacePoint.location.z);

	camPitch = -0.32f;

	fp->angle = fp->targetAngle = playerAvatar->spacePoint.angle;
	fp->maxSpeed = 0.12f;
	fp->maxTurn = D3DX_PI/15;
	fp->x = fp->targetX = playerAvatar->spacePoint.location.x;
	fp->y = fp->targetY = playerAvatar->spacePoint.location.z;
	fp->size = FPNORMALSIZE;

	monsterList = new DoublyLinkedList();
	staticList  = new DoublyLinkedList();

	avatarStand = new PumaAnim(1,"AVATAR_STAND");
//	avatarStand->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar-stand.ase");
//	avatarStand->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);
//	avatarStand->SaveCompressed(puma->m_pd3dDevice, "dat\\avatar-stand.anc");
	avatarStand->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-stand.anc");
	int topIndex = avatarStand->FindTopVert(puma->m_pd3dDevice);
	avatarStand->SetExtraPosition(puma->m_pd3dDevice,0,topIndex);
	avatarStand->AdjustExtraPosition(puma->m_pd3dDevice,0, 0,-0.1f,0);

	avatarRun = new PumaAnim(1,"AVATAR_RUN");
//	avatarRun->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar-run.ase");
//	avatarRun->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);
//	avatarRun->SaveCompressed(puma->m_pd3dDevice, "dat\\avatar-run.anc");
	avatarRun->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-run.anc");
	avatarRun->SetExtraPosition(puma->m_pd3dDevice,0,topIndex);
	avatarRun->AdjustExtraPosition(puma->m_pd3dDevice,0, 0,-0.1f,0);

	DrawPleaseWaitScreen();

	avatarAttack = new PumaAnim(1,"AVATAR_ATTACK");
//	avatarAttack->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar-attack.ase");
//	avatarAttack->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);
//	avatarAttack->SaveCompressed(puma->m_pd3dDevice, "dat\\avatar-attack.anc");
	avatarAttack->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-attack.anc");
	avatarAttack->SetExtraPosition(puma->m_pd3dDevice,0,topIndex);
	avatarAttack->AdjustExtraPosition(puma->m_pd3dDevice,0, 0,-0.1f,0);

	avatarAttack2 = new PumaAnim(1,"AVATAR_ATTACK2");
//	avatarAttack2->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar-attack2.ase");
//	avatarAttack2->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);
//	avatarAttack2->SaveCompressed(puma->m_pd3dDevice, "dat\\avatar-attack2.anc");
	avatarAttack2->LoadCompressed(puma->m_pd3dDevice, "dat\\avatar-attack2.anc");
	avatarAttack2->SetExtraPosition(puma->m_pd3dDevice,0,topIndex);
	avatarAttack2->AdjustExtraPosition(puma->m_pd3dDevice,0, 0,-0.1f,0);


/*
	monsterMesh = new PumaMesh();
	monsterMesh->LoadFromASC(puma->m_pd3dDevice, "dat\\monster1.ase");
	monsterMesh->LoadTexture(puma->m_pd3dDevice,	"dat\\monster1.png");
	monsterMesh->Scale(puma->m_pd3dDevice, 0.007f, 0.007f, 0.007f);

  merchantMesh = new PumaMesh();
	merchantMesh->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar.ase");
	merchantMesh->LoadTexture(puma->m_pd3dDevice,	"dat\\merchant1.png");
	merchantMesh->Scale(puma->m_pd3dDevice, 0.0045f, 0.0045f, 0.0045f);

	trainerMesh = new PumaMesh();
	trainerMesh->LoadFromASC(puma->m_pd3dDevice, "dat\\avatar.ase");
	trainerMesh->LoadTexture(puma->m_pd3dDevice,	"dat\\trainer1.png");
	trainerMesh->Scale(puma->m_pd3dDevice, 0.0045f, 0.0045f, 0.0045f);
*/

	merchantAnim = new PumaAnim(1,"MERCHANT_ANIM");
//	merchantAnim->LoadFromASC(puma->m_pd3dDevice, "dat\\merchant.ase");
//	merchantAnim->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);
//	merchantAnim->SaveCompressed(puma->m_pd3dDevice, "dat\\merchant.anc");
	merchantAnim->LoadCompressed(puma->m_pd3dDevice, "dat\\merchant.anc");

	witchStandAnim = new PumaAnim(1,"WITCHSTAND_ANIM");
//	witchStandAnim->LoadFromASC(puma->m_pd3dDevice, "dat\\witch-shout.ASE");
//	witchStandAnim->Scale(puma->m_pd3dDevice, 0.008f, 0.008f, 0.008f);
//	witchStandAnim->SaveCompressed(puma->m_pd3dDevice, "dat\\witch-shout.anc");
	witchStandAnim->LoadCompressed(puma->m_pd3dDevice, "dat\\witch-idle.anc");
	witchStandAnim->LoadTexture(puma->m_pd3dDevice,	"dat\\witch.PNG");
  
	witchTalkAnim = new PumaAnim(1,"WITCHTALK_ANIM");
//	witchTalkAnim->LoadFromASC(puma->m_pd3dDevice, "dat\\witch-shout.ASE");
//	witchTalkAnim->Scale(puma->m_pd3dDevice, 0.008f, 0.008f, 0.008f);
//	witchTalkAnim->SaveCompressed(puma->m_pd3dDevice, "dat\\witch-shout.anc");
	witchTalkAnim->LoadCompressed(puma->m_pd3dDevice, "dat\\witch-talk.anc");
	witchTalkAnim->LoadTexture(puma->m_pd3dDevice,	"dat\\witch.PNG");

	witchTalkCounter = 0;

	DrawPleaseWaitScreen();

	sackMesh = new PumaMesh();
	sackMesh->LoadFromASC(puma->m_pd3dDevice, "dat\\sack.ase");
	sackMesh->LoadTexture(puma->m_pd3dDevice,	"dat\\sack.png");
	sackMesh->Scale(puma->m_pd3dDevice, 0.0035f, 0.0035f, 0.0035f);

	greatTreeMesh = new PumaMesh();
	greatTreeMesh->LoadFromASC(puma->m_pd3dDevice,"dat\\tree1.ase");
	greatTreeMesh->LoadTexture(puma->m_pd3dDevice,"dat\\f_tree.png",1);
	greatTreeMesh->Scale(puma->m_pd3dDevice,0.03f, 0.03f, 0.03f);

	towerMesh = new PumaMesh();
	towerMesh->LoadFromASC(puma->m_pd3dDevice,"dat\\tower.ase");
	towerMesh->LoadTexture(puma->m_pd3dDevice,"dat\\tower.png",1);
	towerMesh->Scale(puma->m_pd3dDevice,0.007f, 0.005f, 0.007f);


	// Add the mesh for the Realm of Pain enterance tower
	ropEnteranceMesh = new PumaMesh();
	ropEnteranceMesh->LoadFromASC( puma->m_pd3dDevice, "dat\\rop_tower.ase" );
	ropEnteranceMesh->LoadTexture( puma->m_pd3dDevice, "dat\\rop_tower.png" );
	ropEnteranceMesh->Scale( puma->m_pd3dDevice, 0.007f, 0.005f, 0.007f );

	/*
	// Add TEMP MESH FOR CASTLE
	castleMesh = new PumaMesh();
	castleMesh->LoadFromASC( puma->m_pd3dDevice, "dat\\warriorg.ase" );
	castleMesh->LoadTexture( puma->m_pd3dDevice, "dat\\warriorg.png" );
	castleMesh->Scale( puma->m_pd3dDevice, 0.028f, 0.020f, 0.028f );
	*/


	caveMesh[0] = new PumaMesh();
//	caveMesh[0]->LoadFromASC(puma->m_pd3dDevice, "dat\\cave.ase");
//	caveMesh[0]->Scale(puma->m_pd3dDevice, 0.040f, 0.040f, 0.040f);
//	caveMesh[0]->SaveCompressed(puma->m_pd3dDevice,"dat\\cave.MEC");
	caveMesh[0]->LoadCompressed(puma->m_pd3dDevice,"dat\\cave.MEC");
	caveMesh[0]->LoadTexture(puma->m_pd3dDevice,	"dat\\cave.png");

	caveMesh[1] = new PumaMesh();
//	caveMesh[1]->LoadFromASC(puma->m_pd3dDevice, "dat\\cave2.ase");
//	caveMesh[1]->Scale(puma->m_pd3dDevice, 0.040f, 0.040f, 0.040f);
//	caveMesh[1]->SaveCompressed(puma->m_pd3dDevice,"dat\\cave2.MEC");
	caveMesh[1]->LoadCompressed(puma->m_pd3dDevice,"dat\\cave2.MEC");
	caveMesh[1]->LoadTexture(puma->m_pd3dDevice,	"dat\\cave.png");

	DrawPleaseWaitScreen();

	caveMesh[2] = new PumaMesh();
	caveMesh[2]->LoadCompressed(puma->m_pd3dDevice,"dat\\cave.MEC");
	caveMesh[2]->LoadTexture(puma->m_pd3dDevice,	"dat\\caveRed.png");

	caveMesh[3] = new PumaMesh();
	caveMesh[3]->LoadCompressed(puma->m_pd3dDevice,"dat\\cave2.MEC");
	caveMesh[3]->LoadTexture(puma->m_pd3dDevice,	"dat\\caveRed.png");

	caveMesh[4] = new PumaMesh();
	caveMesh[4]->LoadCompressed(puma->m_pd3dDevice,"dat\\cave.MEC");
	caveMesh[4]->LoadTexture(puma->m_pd3dDevice,	"dat\\caveBlack.png");

	standinMesh = new PumaMesh();
	standinMesh->LoadFromASC(puma->m_pd3dDevice, "dat\\standin-box.ASE");
	standinMesh->LoadTexture(puma->m_pd3dDevice,	"dat\\bladeMetal.png");
	standinMesh->Scale(puma->m_pd3dDevice, 0.005f, 0.005f, 0.005f);

	chestMesh[0] = new PumaMesh();
	chestMesh[0]->LoadFromASC(puma->m_pd3dDevice, "dat\\chestClosed.ase");
	chestMesh[0]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
	chestMesh[0]->LoadTexture(puma->m_pd3dDevice,	"dat\\chest.png");

	chestMesh[1] = new PumaMesh();
	chestMesh[1]->LoadFromASC(puma->m_pd3dDevice, "dat\\chestOpen.ase");
	chestMesh[1]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
	chestMesh[1]->LoadTexture(puma->m_pd3dDevice,	"dat\\chest.png");

	DrawPleaseWaitScreen();

	char tempText[1024];
	for (int i = 0; i < MAGIC_MAX; ++i)
	{
		tokenMesh[i] = new PumaMesh();
//		tokenMesh[i]->LoadFromASC(puma->m_pd3dDevice, "dat\\token.ase");
//		tokenMesh[i]->Scale(puma->m_pd3dDevice, 1/300.0f, 1/300.0f, 1/300.0f);
//		tokenMesh[i]->SaveCompressed(puma->m_pd3dDevice,"dat\\token.MEC");
		tokenMesh[i]->LoadCompressed(puma->m_pd3dDevice,"dat\\token.MEC");
		sprintf(tempText,"dat\\token_%s.png",magicNameList[i]);
		tokenMesh[i]->LoadTexture(puma->m_pd3dDevice,	tempText);
	}
	curCamAngle = realCamAngle = (float)NormalizeAngle(D3DX_PI/2);
	DrawPleaseWaitScreen();

	flashCounter = 0;
//	animCounter = 0;

	staticMap = new char[ground->sizeW * ground->sizeH];
	for (int i = 0; i < ground->sizeW * ground->sizeH; ++i)
		staticMap[i] = 0;

	avatarNameChange = NULL;
	selectedMOB = NULL;
	wieldedPtr = 0;
	showFrameRate = FALSE;

	lastTick = 0;

	// set up the particle types
	flareParticle = new Particle2Type(300, "dat\\flare-white.png", 0.6f);
//	flareParticle->SetEmissionPoint(thePlayer->spacePoint.location);
	flareParticle->lowAngle = 0;
	flareParticle->highAngle = D3DX_PI * 2;
	flareParticle->lowAzimuth = -D3DX_PI/2;
	flareParticle->highAzimuth = D3DX_PI/2;
//	flareParticle->lowAzimuth = 0;
//	flareParticle->highAzimuth = 0;
	flareParticle->highLife = 80;
	flareParticle->lowLife = 60;
	flareParticle->lowSpeed = 0.02f;
	flareParticle->highSpeed = 0.06f;
	
	DrawPleaseWaitScreen();

	magicPlaceParticle = new Particle2Type(300, "dat\\flare-white.png", 0.1f);
	magicPlaceParticle->lowAngle = 0;
	magicPlaceParticle->highAngle = 0;
	magicPlaceParticle->lowAzimuth = D3DX_PI/2;
	magicPlaceParticle->highAzimuth = D3DX_PI/2;
	magicPlaceParticle->highLife = 80;
	magicPlaceParticle->lowLife = 60;
	magicPlaceParticle->lowSpeed = 0.01f;
	magicPlaceParticle->highSpeed = 0.01f;

	swordParticle = new Particle2Type(300, "dat\\flare-white.png", 0.06f);
	swordParticle->lowAngle = 0;
	swordParticle->highAngle = D3DX_PI*2;
	swordParticle->lowAzimuth = D3DX_PI/3;
	swordParticle->highAzimuth = D3DX_PI/2;
	swordParticle->highLife = 20;
	swordParticle->lowLife = 5;
	swordParticle->lowSpeed = 0.01f;
	swordParticle->highSpeed = 0.01f;

	driftDownParticle = new Particle2Type(300, "dat\\flare-white.png", 0.1f);
	driftDownParticle->lowAngle = 0;
	driftDownParticle->highAngle = 0;
	driftDownParticle->lowAzimuth = -D3DX_PI/2;
	driftDownParticle->highAzimuth = -D3DX_PI/2;
	driftDownParticle->highLife = 100;
	driftDownParticle->lowLife = 60;
	driftDownParticle->lowSpeed = 0.016f;
	driftDownParticle->highSpeed = 0.016f;

	eventParticle = new Particle2Type(100, "dat\\flare-white.png", 0.3f);
	eventParticle->lowAngle = 0;
	eventParticle->highAngle = D3DX_PI * 2;
	eventParticle->lowAzimuth = 0;
	eventParticle->highAzimuth = 0;
	eventParticle->highLife = 180;
	eventParticle->lowLife = 160;
	eventParticle->lowSpeed = 0.01f;
	eventParticle->highSpeed = 0.02f;

	petHitParticle = new Particle2Type(100, "dat\\flare-white.png", 0.15f);
	petHitParticle->lowAngle = 0;
	petHitParticle->highAngle = D3DX_PI * 2;
	petHitParticle->lowAzimuth = 0;
	petHitParticle->highAzimuth = 0;
	petHitParticle->highLife = 80;
	petHitParticle->lowLife = 40;
	petHitParticle->lowSpeed = 0.02f;
	petHitParticle->highSpeed = 0.03f;

	superSplashParticle = new Particle2Type(300, "dat\\flare-white.png", 1.0f);
	superSplashParticle->lowAngle = 0;
	superSplashParticle->highAngle = D3DX_PI * 2;
	superSplashParticle->lowAzimuth = 0;
	superSplashParticle->highAzimuth = D3DX_PI/4;
	superSplashParticle->highLife = 180;
	superSplashParticle->lowLife = 100;
	superSplashParticle->lowSpeed = 0.02f;
	superSplashParticle->highSpeed = 0.08f;

	mistParticle = new Particle3Type(100, "dat\\mist1.png", 0.90f);
	mistParticle->lowAngle = 1.0f;
	mistParticle->highAngle = 1.1f;
	mistParticle->lowAzimuth = 0;
	mistParticle->highAzimuth = 0;
	mistParticle->highLife = 210;
	mistParticle->lowLife = 210;
	mistParticle->lowSpeed = 0.02f;
	mistParticle->highSpeed = 0.03f;

	DrawPleaseWaitScreen();

	bombFireParticle = new ParticleScaleType(100, "dat\\bombFire.png", 0.0f);
	bombFireParticle->lowAngle = 0;
	bombFireParticle->highAngle = D3DX_PI * 2;
	bombFireParticle->lowAzimuth = -D3DX_PI/2;
	bombFireParticle->highAzimuth = D3DX_PI/2;
	bombFireParticle->highLife = 80;
	bombFireParticle->lowLife = 60;
	bombFireParticle->lowSpeed = 0.2f;
	bombFireParticle->highSpeed = 0.4f;

	smokeParticle = new ParticleSmokeType(100, "dat\\smoke.png", 0.003f, 0.2f);
	smokeParticle->lowAngle = 0;
	smokeParticle->highAngle = D3DX_PI /3;
	smokeParticle->lowAzimuth = -D3DX_PI/2;
	smokeParticle->highAzimuth = D3DX_PI/2;
	smokeParticle->highLife = 80;
	smokeParticle->lowLife = 60;
	smokeParticle->lowSpeed = 0.01f;
	smokeParticle->highSpeed = 0.02f;

	spellParticle = new ParticleRotType(100, "dat\\flare-white.png", 0.1f);
	spellParticle->lowAngle = 0;
	spellParticle->highAngle = 0;
	spellParticle->lowAzimuth = 0;
	spellParticle->highAzimuth = 0;
	spellParticle->highLife = 70;
	spellParticle->lowLife = 40;
	spellParticle->lowSpeed  = 0;
	spellParticle->highSpeed = 0;

	spellParticle->highDist  = 0.7f;
	spellParticle->lowDist   = 0.4f;
	spellParticle->highDistSpeed = 0;
	spellParticle->lowDistSpeed  = 0;
	spellParticle->highRotSpeed = 0.04f;
	spellParticle->lowRotSpeed  = 0.03f;

	haloGlowSet = new PointGlowSet(30, "dat\\flare-white.png", 0.3f);

	bladeTrail = new GlowQuadType(100);



	simpleGrass[0] = new SimpleGrass(200, "dat\\grass-bill-1.png", 0.2f);
	simpleGrass[1] = new SimpleGrass(200, "dat\\grass-bill-2.png", 0.25f);
	simpleGrass[2] = new SimpleGrass(200, "dat\\grass-bill-3.png", 0.3f);
	simpleGrass[3] = new SimpleGrass(200, "dat\\grass-bill-4.png", 0.3f);


	DrawPleaseWaitScreen();

	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\merchant.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &merchantBitmap);
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\trainer.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &trainerBitmap);
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\townmage.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
							D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &townmageBitmap);

	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bar-red.png",
	 				0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
	 				D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &barArt[0]);
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\bar-green.png",
	 				0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
	 				D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &barArt[1]);

	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\specialDunSky.png",
	 				0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
	 				D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &specialDunSkyTexture);

	DrawPleaseWaitScreen();

	bsMan.Activate();
	psMan.Activate();

	footstep[0] = new PumaSound("dat\\footstep1.wav");
	footstep[1] = new PumaSound("dat\\footstep2.wav");
	footstep[2] = new PumaSound("dat\\footstep3.wav");
	footstep[3] = new PumaSound("dat\\footstep4.wav");

	for (int i = 0; i < NUM_OF_MONSTERS; ++i)
		monsterPain[i] = new PumaSound(monsterSoundFileNames[i]);

	for (int i = 0; i < 7; ++i)
		bombSound[i] = new PumaSound(bombSoundFileNames[i]);

	for (int i = 0; i < 3; ++i)
		weatherSound[i] = new PumaSound(weatherSoundFileNames[i]);

	avatarPain    = new PumaSound("dat\\hurtAv.wav");
	teleportSound = new PumaSound("dat\\teleport.wav");
	healSound     = new PumaSound("dat\\heal.wav");
	thiefSound    = new PumaSound("dat\\thief.wav");

	DrawPleaseWaitScreen();

	for (int i = 0; i < 5; ++i)
		extraLights[i].y = 1000; // means unused
	extraLightsUsed = 5;

	timeOfDay = 0;
	lastTenSecondTimer = 0;

	curMapSizeX = MAP_SIZE_WIDTH;
	curMapSizeY = MAP_SIZE_HEIGHT;

	isLightning = lightningCount = 0;


}

//******************************************************************
BBOClient::~BBOClient()
{
	SaveOptions();

	delete thiefSound;
	delete healSound;
	delete teleportSound;
	delete avatarPain;

	for (int i = 0; i < NUM_OF_MONSTERS; ++i)
		delete monsterPain[i];
	for (int i = 0; i < 7; ++i)
		delete bombSound[i];
	for (int i = 0; i < 3; ++i)
		delete weatherSound[i];


	delete footstep[0];
	delete footstep[1];
	delete footstep[2];
	delete footstep[3];

	psMan.Deactivate();
	bsMan.Deactivate();

	delete simpleGrass[0];
	delete simpleGrass[1];
	delete simpleGrass[2];
	delete simpleGrass[3];

   SAFE_RELEASE(barArt[1]);
   SAFE_RELEASE(barArt[0]);
   SAFE_RELEASE(merchantBitmap);
   SAFE_RELEASE(trainerBitmap);
   SAFE_RELEASE(townmageBitmap);
   SAFE_RELEASE(specialDunSkyTexture);
	delete bladeTrail;
	delete haloGlowSet;
	delete spellParticle;
	delete flareParticle;
	delete magicPlaceParticle;
	delete swordParticle;
	delete driftDownParticle;
	delete eventParticle;
	delete petHitParticle;
	delete bombFireParticle;
	delete smokeParticle;
	delete mistParticle;
	delete superSplashParticle;
	delete staticMap;
	delete staticList;
	delete monsterList;
	delete flockSet;
	delete dummyAvatar;
	delete avatarAttack;
	delete merchantAnim;
	delete witchStandAnim;
	delete witchTalkAnim;
	delete avatarAttack2;
	delete avatarRun;
	delete avatarStand;
	delete greatTreeMesh;
	delete sackMesh;
	delete standinMesh;
	delete towerMesh;
	delete ropEnteranceMesh;

	for (int i = 0; i < 5; ++i)
		delete caveMesh[i];

	for (int i = 0; i < MAGIC_MAX; ++i)
		delete tokenMesh[i];

	for (int i = 0; i < 2; ++i)
	{
		delete chestMesh[i];
	}

   SAFE_RELEASE(lineOverlayGroundPtr);
   SAFE_RELEASE(lineOverlayPtr);

	delete skyDome;
	delete ground;
	delete lSquareMan;
//	delete lsMan;
	if (dungeon)
		delete dungeon;

}

D3DXCOLOR trainerParticleColor  = D3DCOLOR_ARGB(55, 0, 255, 0);
D3DXCOLOR townmageParticleColor = D3DCOLOR_ARGB(55, 255, 0, 0);

//******************************************************************
void BBOClient::Tick(void)
{
//	char tempText[1024];

	bladeTrail->StartAddingQuads();

//	DebugOutput("St BBOClient Tick\n");
   D3DXMATRIX matWorld, mat2;
//	float tweakF;
	char curText[1024];

	if (lastTick == 0)
		lastTick = timeGetTime();
	else
	{
		DWORD now = timeGetTime();
		float delta = (now-lastTick) / 30.0f;
		lastTick = now;
		flockSet->Tick(delta, 0,0);//ground->CellSize()/2, ground->CellSize()/2);

		flareParticle->Tick(delta);
		magicPlaceParticle->Tick(delta);
		swordParticle->Tick(delta);
		driftDownParticle->Tick(delta);
		eventParticle->Tick(delta);
		petHitParticle->Tick(delta);
		mistParticle->Tick(delta);
		bombFireParticle->Tick(delta);
		smokeParticle->Tick(delta);
		superSplashParticle->Tick(delta);
		spellParticle->Tick(delta);

		bladeTrail->Tick();
	}

//	DebugOutput("1\n");

	FlockPoint *fp = flockSet->GetPoint(dummyAvatar->flockPointIndex,dummyAvatar);
	dummyAvatar->spacePoint.location.x = fp->x;
	dummyAvatar->spacePoint.location.z = fp->y;
	dummyAvatar->spacePoint.angle = fp->angle;
	dummyAvatar->spacePoint.location.y = ground->HeightAtPoint(dummyAvatar->spacePoint.location.x, dummyAvatar->spacePoint.location.z,NULL);

	camCenterX = playerAvatar->spacePoint.location.x;
	camCenterZ = playerAvatar->spacePoint.location.z;

	D3DXVECTOR3 playerPosForClient = playerAvatar->spacePoint.location;

	if (controlledMonsterID != -1)
	{
		BBOMob *curMob = (BBOMob *) monsterList->First();
		while (curMob)
		{
			if (curMob->mobID == controlledMonsterID)
			{
				playerPosForClient = curMob->spacePoint.location;

				camCenterX = curMob->spacePoint.location.x;
				camCenterZ = curMob->spacePoint.location.z;
//				controlledMonsterY = curMob->spacePoint.location.y;

				curMob = (BBOMob *) monsterList->Last();
			}

			curMob = (BBOMob *) monsterList->Next();
		}
	}

	if (SPACE_GROUND == curMapType && grassDensity)
	{
		simpleGrass[0]->AddTuft(ground, 
					playerPosForClient, 
					playerPosForClient);
		simpleGrass[1]->AddTuft(ground, 
					playerPosForClient, 
					playerPosForClient);
		simpleGrass[2]->AddTuft(ground, 
					playerPosForClient, 
					playerPosForClient);
		simpleGrass[3]->AddTuft(ground, 
					playerPosForClient, 
					playerPosForClient);
	}

//	DebugOutput("2\n");

	// handle dead mobs
	BBOMob *curMob = (BBOMob *) monsterList->First();
	while (curMob)
	{
		if (SMOB_PARTICLE_STREAM == curMob->type && 
			 curMob->particleStreamAge < timeGetTime())
		{
			monsterList->Remove(curMob);
			delete curMob;
			curMob = (BBOMob *) monsterList->First();
		}
		else if (curMob->dying && curMob->animCounter > 30)
		{
			D3DXVECTOR3 pos = curMob->spacePoint.location;
			pos.y += 1;
			flareParticle->SetEmissionPoint(pos);
			flareParticle->Emit(20, D3DCOLOR_ARGB(155, 255, 0, 0));

			monsterList->Remove(curMob);
			if (curMob->cellSlot > -1)
			{
				ground->ReleaseSlot(curMob->cellX, curMob->cellY, curMob->cellSlot);
				flockSet->FreePoint(curMob->flockPointIndex, curMob);
			}
			if (SMOB_MONSTER == curMob->type && curMob->animCounter < 13)
			{
				sprintf(curText,"The %s is destroyed! It drops something.", curMob->name);
				DisplayTextInfo(curText);
			}

			if (curMob->mobID == controlledMonsterID)
				controlledMonsterID = -1;

			delete curMob;
			curMob = (BBOMob *) monsterList->First();
		}
		else if (curMob->beyondRange > 300 && curMob != playerAvatar)
		{
			monsterList->Remove(curMob);
			if (curMob->cellSlot > -1)
			{
				ground->ReleaseSlot(curMob->cellX, curMob->cellY, curMob->cellSlot);
				flockSet->FreePoint(curMob->flockPointIndex, curMob);
			}
			delete curMob;
			curMob = (BBOMob *) monsterList->First();
		}
		else
			curMob = (BBOMob *) monsterList->Next();
	}

//	DebugOutput("3\n");

	D3DXVECTOR3 pos;

//	if (lsMan)
//		lsMan->StartFrame();
	ground->StartFrame();

	// move mobs
	curMob = (BBOMob *) monsterList->First();
	while (curMob)
	{

		if ((SPACE_REALM == curMapType || SPACE_GROUND == curMapType) &&
			 abs(curMob->spacePoint.location.x - 
			   playerPosForClient.x) < 30 &&
		    abs(curMob->spacePoint.location.z - 
			   playerPosForClient.z) < 30)
		{
			if (SPACE_REALM == curMapType)
			{
				if ( SMOB_AVATAR  == curMob->type || 
					  SMOB_TRADER  == curMob->type || 
					  SMOB_TRAINER == curMob->type )
					;//ground->AddRealmShadow(curMob->spacePoint.location.x, curMob->spacePoint.location.z,0.8f);
				else if (SMOB_MONSTER == curMob->type)
				{
					D3DXVECTOR3 shadowSize = curMob->monsterAnims[MONSTER_STATE_STAND]
																[curMob->monsterType]
																[curMob->monsterSubType]->size;
					float shadowRadius = (shadowSize.x + shadowSize.y + shadowSize.z) / 3;
					;//ground->AddRealmShadow(curMob->spacePoint.location.x, 
					  //					  curMob->spacePoint.location.z, shadowRadius);
				}
			}
			else
			{
				if ( SMOB_AVATAR   == curMob->type || 
					  SMOB_TOWNMAGE == curMob->type || 
					  SMOB_TRADER   == curMob->type || 
					  SMOB_WITCH    == curMob->type || 
					  SMOB_TRAINER  == curMob->type )
					ground->AddShadow(curMob->spacePoint.location.x, curMob->spacePoint.location.z,0.8f);
				else if (SMOB_MONSTER == curMob->type)
				{
					D3DXVECTOR3 shadowSize = curMob->monsterAnims[MONSTER_STATE_STAND]
																[curMob->monsterType]
																[curMob->monsterSubType]->size;
					float shadowRadius = (shadowSize.x + shadowSize.y + shadowSize.z) / 3;
					ground->AddShadow(curMob->spacePoint.location.x, 
										  curMob->spacePoint.location.z, shadowRadius);
				}
			}
		}

		if ((SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType) && 
			 abs(curMob->spacePoint.location.x - 
			   playerPosForClient.x) < 13 &&
		    abs(curMob->spacePoint.location.z - 
			   playerPosForClient.z) < 13)
		{
			if ( SMOB_AVATAR  == curMob->type || 
				  SMOB_TRADER  == curMob->type || 
				  SMOB_TRAINER == curMob->type )
				;//lsMan->AddDungeonShadow(curMob->spacePoint.location.x, curMob->spacePoint.location.z,0.8f);
			else if (SMOB_MONSTER == curMob->type)
			{
				D3DXVECTOR3 shadowSize = curMob->monsterAnims[MONSTER_STATE_STAND]
								                     [curMob->monsterType]
															[curMob->monsterSubType]->size;
				float shadowRadius = (shadowSize.x + shadowSize.y + shadowSize.z) / 3;
				//lsMan->AddDungeonShadow(curMob->spacePoint.location.x, 
				//	              curMob->spacePoint.location.z, shadowRadius);

			}
		}
		if (curMob->screenHealthTimer > 0)
			curMob->screenHealthTimer -= 1;

		if (SMOB_AVATAR == curMob->type || SMOB_MONSTER == curMob->type)
		{
//			D3DXVECTOR3 pos = curMob->spacePoint.location;
//			pos.y += 1;
//			eventParticle->SetEmissionPoint(pos);
//			eventParticle->Emit(1, trainerParticleColor);


			FlockPoint *fp = flockSet->GetPoint(curMob->flockPointIndex, curMob);

//			sprintf(tempText,"%ld %ld\n", &fp, curMob->flockPointIndex);
//			DebugOutput(tempText);

			curMob->spacePoint.location.x = fp->x;
			curMob->spacePoint.location.z = fp->y;
			curMob->spacePoint.angle = fp->angle;
			curMob->moving = FALSE;
			if (fp->lastDistMovedSQR > 0.001f)
			{
				curMob->moving = TRUE;
				float suggestedSpeed = Distance(fp->x, fp->y, fp->targetX, fp->targetY);
				suggestedSpeed *= 0.12f / 5.0f;
				if (0.12f < suggestedSpeed)
					suggestedSpeed = 0.12f;
				if (suggestedSpeed < 0.06f ) 
					suggestedSpeed = 0.06f ;
				fp->maxSpeed = suggestedSpeed;
			}
			else
			{
				fp->maxSpeed = fp->maxSpeed;
//				fp->maxSpeed = 0.005f;
			}
			curMob->spacePoint.location.y = HeightAtPoint(curMob->spacePoint.location.x, curMob->spacePoint.location.z);


			if (SMOB_MONSTER == curMob->type && curMob->staticMonsterFlag)
			{
				pos.x = ground->GetXForPoint(curMob->cellX);
				pos.z = ground->GetZForPoint(curMob->cellY);
				pos.y = 0;
				pos.x += rnd(0.0f,10.0f);
				pos.z += rnd(0.0f,10.0f);
				magicPlaceParticle->SetEmissionPoint(pos);
				magicPlaceParticle->Emit(1, townmageParticleColor);
			}

			if (SMOB_AVATAR == curMob->type && curMob->chantType > -1)
			{
				pos = curMob->spacePoint.location;
				pos.y += 1;
				pos.x += rnd(-0.2f,0.2f);
				pos.z += rnd(-0.2f,0.2f);
				driftDownParticle->SetEmissionPoint(pos);
				driftDownParticle->Emit(1, curMob->chantColor);
			}

		}
		else if (SMOB_TRAINER == curMob->type)
		{
			curMob->spacePoint.location.y = HeightAtPoint(curMob->spacePoint.location.x, curMob->spacePoint.location.z);

//			DebugOutput("-");

//			DebugOutput("1");

			pos = curMob->spacePoint.location;
//			DebugOutput("2");

			pos.x += rnd(-0.2f,0.2f);
			pos.z += rnd(-0.2f,0.2f);
//			pos.x = 660;
//			pos.y += 1;
//			pos.z = 550;
//			DebugOutput("3");

			magicPlaceParticle->SetEmissionPoint(pos);

//			sprintf(tempText,"%ld %f %f %f\n", magicPlaceParticle, pos.x, pos.y, pos.z); 
//			DebugOutput(tempText);

//			DebugOutput("4");


			magicPlaceParticle->Emit(1, trainerParticleColor);
		}
		else if (SMOB_TOWNMAGE == curMob->type)
		{
			curMob->spacePoint.location.y = HeightAtPoint(curMob->spacePoint.location.x, curMob->spacePoint.location.z);

			pos = curMob->spacePoint.location;

			pos.x += rnd(-0.2f,0.2f);
			pos.z += rnd(-0.2f,0.2f);
			magicPlaceParticle->SetEmissionPoint(pos);
			magicPlaceParticle->Emit(1, townmageParticleColor);
		}
		else if (SMOB_WITCH == curMob->type)
		{
			curMob->spacePoint.location.y = HeightAtPoint(curMob->spacePoint.location.x, curMob->spacePoint.location.z);

			pos = curMob->spacePoint.location;

			pos.x += rnd(-0.2f,0.2f);
			pos.z += rnd(-0.2f,0.2f);
			magicPlaceParticle->SetEmissionPoint(pos);
			magicPlaceParticle->Emit(1, D3DCOLOR_ARGB(55, 100,0,255));
		}
		else if (SMOB_TRADER == curMob->type)
		{
			curMob->spacePoint.location.y = HeightAtPoint(curMob->spacePoint.location.x, curMob->spacePoint.location.z);
		}
		else if (SMOB_TOWER == curMob->type && curMob->towerHasMistress)
		{
			pos = curMob->spacePoint.location;
			pos.x += rnd(-0.5f,0.5f);
			pos.z += rnd(-0.5f,0.5f);
			pos.z -= 3.0f;
			magicPlaceParticle->SetEmissionPoint(pos);
			magicPlaceParticle->Emit(1, trainerParticleColor);
		}
		else if (SMOB_WARP_POINT == curMob->type)
		{
			pos = curMob->spacePoint.location;
			pos.y += rnd(0,1.2f);
			spellParticle->SetEmissionPoint(pos);
			if (1 == curMob->monsterSubType)
				spellParticle->Emit(5, trainerParticleColor);
			else
				spellParticle->Emit(5, townmageParticleColor);
		}

		curMob = (BBOMob *) monsterList->Next();
	}


	DWORD now = timeGetTime();

	DWORD delta;

	if (0 == lastTenSecondTimer  || 
		 now < lastTenSecondTimer)
	{
		delta = 1000 * 10  + 1;
	}
	else
		delta = now - lastTenSecondTimer;

	// write out every 10 seconds
	if (delta > 1000 * 10)	
	{
		lastTenSecondTimer = now;

		++timeOfDay;
		if (timeOfDay >= 4 * 60 * 6)
//		if (timeOfDay >= 4 *  5 * 6)
			timeOfDay = 0;
	}





	/*
	if (curCamAngle != realCamAngle)
	{
		float delta = curCamAngle - realCamAngle;

		tweakF = gTweakSystem.GetTweak("CAMROTATIONCOEFF", 0.17f, 
			                            0.001f, 1.0f,
												 "adjusts speed of camera rotation");
		realCamAngle += delta * tweakF;
	
		tweakF = gTweakSystem.GetTweak("CAMROTATIONSTOP", D3DX_PI/490, 
			                            D3DX_PI/40090, D3DX_PI/2,
												 "finish camera turn when camera angle is this close to target angle");
		if (fabs(curCamAngle - realCamAngle) < tweakF)
			realCamAngle = curCamAngle;
	}
	*/
//	DebugOutput("En BBOClient Tick\n");
}

DWORD lastTime1 = 0;
int whiteAvatarCount = 0;
int whiteAvatarID = 0;
D3DXVECTOR3 playerPosForClient;

//******************************************************************
void BBOClient::Draw(void)
{
	D3DXMATRIX matTrans, matRotateX, matRotateY, matRotateZ, matTemp1, matTemp2;
   D3DXMATRIX matWorld, mat2;
	float tweakF;
	int frameCalc;

	++flashCounter;

	bombShakeVal = bombShakeVal * 0.85f;
	if (bombShakeVal < 0)
		bombShakeVal = 0;

	float bombOffset = sin(bombShakeVal * 10) * bombShakeVal * 0.006f;

//	DebugOutput("St Draw\n");
	// render starsphere

	playerPosForClient = playerAvatar->spacePoint.location;

	if (controlledMonsterID != -1)
	{
		BBOMob *curMob = (BBOMob *) monsterList->First();
		while (curMob)
		{
			if (curMob->mobID == controlledMonsterID)
			{
				playerPosForClient = curMob->spacePoint.location;
				curMob = (BBOMob *) monsterList->Last();
			}

			curMob = (BBOMob *) monsterList->Next();
		}
	}

	SetMainLights();

	{

		puma->m_pd3dDevice->SetMaterial( &neutralMaterial );
		puma->m_pd3dDevice->LightEnable( 1, TRUE );
	//   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(255,255,255,255)  );


		D3DXMatrixIdentity( &matWorld );
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		D3DXMatrixRotationY( &matTemp1, curCamAngle + bombOffset); //puma->pumaCamera->spacePoint.angle); //camAngle[1]);
		D3DXMatrixRotationX( &matRotateX, camPitch);
		D3DXMatrixMultiply( &matTemp1, &matTemp1, &matRotateX);
		D3DXMatrixMultiply( &matWorld, &matWorld, &matTemp1 );

		puma->m_pd3dDevice->SetTransform( D3DTS_VIEW , &matWorld );

		if (SPACE_DUNGEON != curMapType && SPACE_GUILD != curMapType && SPACE_LABYRINTH != curMapType)
		{
			// Turn off the zbuffer
			puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

			if ((SPACE_REALM == curMapType))
				skyDome->Draw(puma->m_pd3dDevice, realm->skyTexture);
			else
				skyDome->Draw(puma->m_pd3dDevice);

			// Turn on the zbuffer
			puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		}
		else if (SPACE_DUNGEON == curMapType && isInSpecialDungeon)
		{
			// Turn off the zbuffer
			puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

			skyDome->Draw(puma->m_pd3dDevice, specialDunSkyTexture);

			// Turn on the zbuffer
			puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
		}


		puma->m_pd3dDevice->LightEnable( 1, FALSE );
		puma->m_pd3dDevice->LightEnable( 0, TRUE );
	}
//	puma->HandleCamera();

   puma->m_pd3dDevice->SetLight( 0, &mainLight[2] ); // ground light

	D3DXVECTOR3 pOut, vecDir;
	D3DLIGHT8 light;

	for (int i = 0; 
	     i < extraLightsUsed && SPACE_LABYRINTH == curMapType && (localInfoFlags & LOCAL_FLAGS_MIST_ON); 
		  ++i)
	{
		if (1000 != extraLights[i].y)
//		if (FALSE)
		{
			pOut = extraLights[i]; 

			D3DUtil_InitLight( light, D3DLIGHT_POINT, -4,0,2);
			light.Position = extraLights[i];
			light.Range = extraLightsRange[i];
			light.Falloff = 1.0f;
			light.Attenuation1 = 0.50f;
			light.Diffuse.r  = extraLightsColorR[i];
			light.Diffuse.g  = extraLightsColorG[i];
			light.Diffuse.b  = extraLightsColorB[i];
			light.Ambient.r  = 0.0f;
			light.Ambient.g  = 0.0f;
			light.Ambient.b  = 0.0f;
			puma->m_pd3dDevice->SetLight( 2+i, &light );
			puma->m_pd3dDevice->LightEnable( 2+i, TRUE );
		}
	}

	for (int i = 0; i < 5; ++i)
	{
		if (SPACE_LABYRINTH != curMapType || !(localInfoFlags & LOCAL_FLAGS_MIST_ON))
		{
			puma->m_pd3dDevice->LightEnable( 2+i, FALSE );
		}
	}

	for (int i = 0; i < extraLightsUsed; ++i)
	{
		extraLights[i].y = 1000;
	}

	// position camera to view current sector
	
	// translate, then rotate, to position relative to sector
	
	D3DXMatrixRotationY( &matTemp1, curCamAngle + bombOffset);
	float zOffset = 4;//gTweakSystem.GetTweak("NORMCAMDISTZ", ground->CellSize() * 0.4f,
		               //             ground->CellSize() * 0.4f, ground->CellSize() * 1.5f,
							//				 "Normal Camera moveback from target");
	if (1 == cameraType)
		zOffset = 20.0f;
	if (2 == cameraType)
		zOffset = 20.0f;
	D3DXMatrixTranslation( &matTemp2, 0,0, zOffset);
	D3DXMatrixMultiply( &matTemp1, &matTemp1, &matTemp2 );

	// build position at sector
	tweakF = 2.2f;//gTweakSystem.GetTweak("NORMCAMDISTY", 2.2f,
		           //                 1.0f, 10.0f,
					  //						 "Normal Camera moveup from target");
	if (1 == cameraType)
		tweakF = 47.0f;
	if (2 == cameraType)
		tweakF = 10.0f;

	D3DXMatrixTranslation( &matTrans, 
		       -1* camCenterX, 
				 -1 * (tweakF + playerPosForClient.y), 
				 -1 *camCenterZ );

	float camPosX = matTrans._41 + sin(curCamAngle) * zOffset;
	float camPosY = matTrans._43 - cos(curCamAngle) * zOffset;

	tweakF = camPitch;//gTweakSystem.GetTweak("NORMCAMPITCH", -0.32f,
		             //               -D3DX_PI, 0,
						 //					 "Normal Camera pitch angle");
	if (1 == cameraType)
		tweakF = -0.92f;
	if (2 == cameraType)
		tweakF = -0.42f;

	D3DXMatrixRotationX( &matRotateX, tweakF);

	// turn around to face sector
	D3DXMatrixRotationY( &matRotateY, (D3DX_PI));

	// pitch down
	D3DXMatrixMultiply( &matTemp1, &matTemp1, &matRotateX );

	// add the rotate-to-face
	D3DXMatrixMultiply( &matRotateY, &matRotateY, &matTemp1 );

	// and finally translate to the position of the sector itself
	D3DXMatrixMultiply( &matTrans, &matTrans, &matRotateY );
/*
	if (!(SPACE_DUNGEON == curMapType) && !(SPACE_REALM == curMapType))
	{
		float gHeight = ground->HeightAtPoint(camPosX, camPosY, NULL);
		if (matTrans._42 < gHeight + 0.2f)
			matTrans._42 = gHeight + 0.2f;
	}
*/
	puma->m_pd3dDevice->SetTransform( D3DTS_VIEW, &matTrans );
	
	D3DXVECTOR3 camPoint;
	camPoint.x = -camPosX;
	camPoint.z = -camPosY;
	camPoint.y = playerPosForClient.y;

	bsMan.Tick(camPoint);
	PumaSound::UpdateAll3DSound(camPoint, -curCamAngle);

	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	puma->PumpInput();

	int centerCellX, centerCellY;
	centerCellX = playerAvatar->cellX;
	centerCellY = playerAvatar->cellY;

	if (controlledMonsterID != -1)
	{
		BBOMob *curMob = (BBOMob *) monsterList->First();
		while (curMob)
		{
			if (curMob->mobID == controlledMonsterID)
			{
				centerCellX = curMob->cellX;
				centerCellY = curMob->cellY;
				curMob = (BBOMob *) monsterList->Last();
			}
			curMob = (BBOMob *) monsterList->Next();
		}
	}

	D3DXVECTOR3 pos;

	// put particles in town centers
	for (int g = 0; g < NUM_OF_TOWNS && SPACE_GROUND == curMapType; ++g)
	{
		if (abs(centerCellX - townList[g].x) < 5 && 
			 abs(centerCellY - townList[g].y) < 5)
		{
			pos.x = ground->GetXForPoint(townList[g].x);
			pos.z = ground->GetZForPoint(townList[g].y);
			pos.x += rnd(0.0f,10.0f);
			pos.z += rnd(0.0f,10.0f);
			pos.y = HeightAtPoint(pos.x, pos.z);
			magicPlaceParticle->SetEmissionPoint(pos);
			if (rand() % 2)
				magicPlaceParticle->Emit(1, townmageParticleColor);
			else
				magicPlaceParticle->Emit(1, trainerParticleColor);
		}
	}

   // Enable fog blending.
	if (0 == cameraType && (localInfoFlags & LOCAL_FLAGS_FOG_ON))
	{
		if (SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType || SPACE_LABYRINTH == curMapType)
		{
			float Start = 20.0f;
			float End = 45.0f;

			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
			// Set the fog color.
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 0, 0, 30));
			// Set fog parameters.
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
		}
		else if ((SPACE_REALM == curMapType))
		{
			float Start, End;

			switch(lastRealmID)
			{
			case REALM_ID_SPIRITS:
			default:
				Start = 30.0f;
				End = 70.0f;

				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
				// Set the fog color.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 90, 147, 241));
				// Set fog parameters.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
				break;

			case REALM_ID_DEAD:
				Start = 30.0f;
				End = 70.0f;

				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
				// Set the fog color.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 110,115,143));
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 0,0,0));
				// Set fog parameters.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
				break;

			case REALM_ID_DRAGONS:
				Start = 20.0f;
				End = 55.0f;

				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
				// Set the fog color.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, D3DCOLOR_ARGB(255, 150,150,190));
				// Set fog parameters.
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
				puma->m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
				break;

			}
		}
		else //if (!(SPACE_REALM == curMapType))
		{
			float Start = 30.0f;
			float End = 100.0f;

			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
			// Set the fog color.
			float localPercent = (0.2f + lightPercent) / 1.2f;
			int r,g,b;
			r = 154.0f * localPercent;
			g = 187.0f * localPercent;
			b = 218.0f * localPercent;
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 
				       D3DCOLOR_ARGB(255, r, g, b));
			// Set fog parameters.
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&Start));
			puma->m_pd3dDevice->SetRenderState(D3DRS_FOGEND,   *(DWORD *)(&End));
		}
	}
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CCW );
	if (SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType)
	{
		puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
		dungeon->Draw(centerCellX, centerCellY);
//		if (0 == cameraType)
//			->DrawShadows();
//		if (0 == cameraType)
//			lSquareMan->DrawSquares();
	}
	else if ((SPACE_REALM == curMapType))
	{
		realm->Draw(centerCellX, centerCellY);
//		lsMan->DrawShadows();
//		lSquareMan->DrawSquares();
	}
	else if ((SPACE_LABYRINTH == curMapType))
	{
		labyrinth->Draw(centerCellX, centerCellY);
//		lsMan->DrawShadows();
//		lSquareMan->DrawSquares();
	}
	else
	{

		ground->m_CameraPos   = camPoint;
		ground->m_CameraAngle = curCamAngle;
		ground->Draw(centerCellX, centerCellY);

		ground->DrawShadows();
//		lSquareMan->DrawSquares();
	}


	lSquareMan->StartFrame();


	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );

	puma->m_pd3dDevice->SetTexture( 0, NULL);
	if (SPACE_DUNGEON != curMapType && SPACE_GUILD != curMapType)
	{
		if (SPACE_REALM != curMapType && SPACE_LABYRINTH != curMapType)
			DrawOverlayGroundLines();
		else
			DrawOverlayLines();
	}



/*
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY(&matWorld, (float) playerAvatar->spacePoint.angle * -1);

//	D3DXMATRIX mat2;
	D3DXMatrixIdentity( &mat2 );
	D3DXMatrixTranslation(&mat2, playerPosForClient.x,
			                         playerPosForClient.y,
									 playerPosForClient.z);

	D3DXMatrixMultiply(&matWorld, &matWorld, &mat2);

	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
//	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, playerAvatar->spacePoint.GetMatrix());

//	avatar->Draw(puma->m_pd3dDevice);
*/
	DoublyLinkedList infoList;


	//************
	puma->PumpInput();


	curCamAngle = NormalizeAngle(curCamAngle);

	BBOMob *curMob = (BBOMob *) monsterList->First();
	while (curMob)
	{
		float dCamX = curMob->spacePoint.location.x + camPosX;
		float dCamY = curMob->spacePoint.location.z + camPosY;
		float dCamAngle = atan2(dCamX, -dCamY);
		dCamAngle   = NormalizeAngle(dCamAngle);
		dCamAngle  -= curCamAngle;
		dCamAngle   = NormalizeAngle2(dCamAngle);

		if (fabs(dCamAngle) < (D3DX_PI/5) || (fabs(dCamX) < 13 && fabs(dCamY) < 13))


//		if (abs(curMob->spacePoint.location.x - 
//			   playerPosForClient.x) < 50 &&
//		    abs(curMob->spacePoint.location.z - 
//			   playerPosForClient.z) < 50)
		{
/*
			D3DXMatrixIdentity( &matWorld );
			D3DXMatrixRotationY(&matWorld, (float) curMob->spacePoint.angle * -1);

			D3DXMatrixIdentity( &mat2 );
			D3DXMatrixTranslation(&mat2, curMob->spacePoint.location.x,
													 curMob->spacePoint.location.y,
											 curMob->spacePoint.location.z);

//			D3DXMatrixMultiply(&matWorld, &matWorld, &mat2);
			matWorld._41 = curMob->spacePoint.location.x;
			matWorld._42 = curMob->spacePoint.location.y;
			matWorld._43 = curMob->spacePoint.location.z;
//			puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat2 );
			puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
			*/
			SetupObjectOrientation(curMob->spacePoint.angle * -1, curMob->spacePoint.location.x,
													 curMob->spacePoint.location.y,
											 curMob->spacePoint.location.z);

			if (curMob->avatarID > -1)
			{
				PumaAnimFrame *paf;

				if (curMob->avTexture && curMob->avTexture->currentBitmap)
				{
					puma->m_pd3dDevice->SetTexture( 0, curMob->avTexture->currentBitmap );
				}
				else
				{
					puma->m_pd3dDevice->SetTexture( 0, curMob->avTexture->currentBitmap );
					++whiteAvatarCount;
					whiteAvatarID = curMob->avatarID;
				}
				puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
				puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
				puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
				puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

				if (curMob->attacking)
				{
					if (1 == curMob->attacking && avatarAttack->numOfFrames > 0)
					{
						avatarAttack->Draw(puma->m_pd3dDevice, (curMob->animCounter/2) % avatarAttack->numOfFrames);
						if (curMob->animCounter/2 >= avatarAttack->numOfFrames)
							curMob->attacking = 0;

						paf = (PumaAnimFrame *) avatarAttack->frames.Find(
									(curMob->animCounter/2) % avatarAttack->numOfFrames);

						DrawHalo(paf, 
							curMob->spacePoint.angle, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y, curMob->spacePoint.location.z);
					}
					else if (avatarAttack2->numOfFrames > 0)
					{
						avatarAttack2->Draw(puma->m_pd3dDevice, (curMob->animCounter/2) % avatarAttack2->numOfFrames);
						if (curMob->animCounter/2 >= avatarAttack2->numOfFrames)
							curMob->attacking = 0;

						paf = (PumaAnimFrame *) avatarAttack2->frames.Find(
									(curMob->animCounter/2) % avatarAttack2->numOfFrames);

						DrawHalo(paf, 
							curMob->spacePoint.angle, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y, curMob->spacePoint.location.z);
					}
				}
				else if (curMob->moving)
				{
					if (avatarRun->numOfFrames > 0)
					{
						int fr = ((curMob->animCounter/2) + ((int)curMob % 10)) % avatarRun->numOfFrames;

						avatarRun->Draw(puma->m_pd3dDevice, fr);

						paf = (PumaAnimFrame *) avatarRun->frames.Find(fr);

						DrawHalo(paf, 
							curMob->spacePoint.angle, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y, curMob->spacePoint.location.z);

						if ((1 == fr || 8 == fr) && curMob->lastStepIndex != fr)
						{
							int ind = rand() % 4;
							curMob->lastStepIndex = fr;
							footstep[ind]->PositionSound(curMob->spacePoint.location, MAX_SOUND_DIST);
							footstep[ind]->Play();
						}
					}

				}
				else
				{
					if (avatarStand->numOfFrames > 0)
					{
						avatarStand->Draw(puma->m_pd3dDevice, ((curMob->animCounter/4) + ((int)curMob % 10)) % avatarStand->numOfFrames);
						paf = (PumaAnimFrame *) avatarStand->frames.Find(
									((curMob->animCounter/4) + ((int)curMob % 10)) % avatarStand->numOfFrames);

						DrawHalo(paf, 
							curMob->spacePoint.angle, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y, curMob->spacePoint.location.z);
					}
				}

				if (curMob->magicAttackType > -1)
				{
					++(curMob->magicAttackCounter);
					if (curMob->magicAttackCounter > 30)
						curMob->magicAttackType = -1;

					if (curMob->magicAttackCounter > 10)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						DWORD color = curMob->magicAttackColor;
						color |= 0x5f000000;
						pos.y += rnd(0,1.2f);
						spellParticle->SetEmissionPoint(pos);
						spellParticle->Emit(5, color);
					}


				}
				// draw sword!
				if (curMob->blade)
				{
					PumaAnimFrame *paf;

					if (1 == curMob->attacking && avatarAttack->numOfFrames > 0)
					{
						paf = (PumaAnimFrame *) avatarAttack->frames.Find(
									(curMob->animCounter/2) % avatarAttack->numOfFrames);

						SetupBladeOrientation(paf,
							curMob->spacePoint.angle * -1, 
							curMob->spacePoint.location.x,
							curMob->spacePoint.location.y,
							curMob->spacePoint.location.z);

						if (curMob->bladeParticleAmount > 0)
							DrawBladeEffect(curMob);
					}
					else if (2 == curMob->attacking && avatarAttack2->numOfFrames > 0)
					{
						paf = (PumaAnimFrame *) avatarAttack2->frames.Find(
									(curMob->animCounter/2) % avatarAttack2->numOfFrames);

						SetupBladeOrientation(paf,
							curMob->spacePoint.angle * -1, 
							curMob->spacePoint.location.x,
							curMob->spacePoint.location.y,
							curMob->spacePoint.location.z);

						if (curMob->bladeParticleAmount > 0)
							DrawBladeEffect(curMob);
					}
					else if (curMob->moving && avatarRun->numOfFrames > 0)
					{
						paf = (PumaAnimFrame *) avatarRun->frames.Find(
									((curMob->animCounter/2) + ((int)curMob % 10)) % avatarRun->numOfFrames);

						SetupBladeOrientation(paf,
							curMob->spacePoint.angle * -1, 
							curMob->spacePoint.location.x,
							curMob->spacePoint.location.y,
							curMob->spacePoint.location.z);

						if (curMob->bladeParticleAmount > 0)
							DrawBladeEffect(curMob);
					}
					else if (avatarStand->numOfFrames > 0)
					{
						paf = (PumaAnimFrame *) avatarStand->frames.Find(
									((curMob->animCounter/4) + ((int)curMob % 10)) % avatarStand->numOfFrames);
						SetupBladeOrientation(paf,
							curMob->spacePoint.angle * -1, 
							curMob->spacePoint.location.x,
							curMob->spacePoint.location.y,
							curMob->spacePoint.location.z);

						if (curMob->bladeParticleAmount > 0)
							DrawBladeEffect(curMob);
					}

					curMob->blade->Draw(puma->m_pd3dDevice);
				}

				// draw baby dragons
				for (int petIndex = 0; petIndex < 2; ++petIndex)
				{
					if (curMob->pet[petIndex].petDragonAnims[0])
						curMob->pet[petIndex].Draw(&curMob->spacePoint, petIndex);
				}

				// draw name
				if (abs(centerCellX - curMob->cellX) < 2 && 
					 abs(centerCellY - curMob->cellY) < 2)
				{
					DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);
					DWORD colorGrn = D3DCOLOR_ARGB(255, 100, 255, 100);
					DWORD colorYellow = D3DCOLOR_ARGB(255, 255, 255, 0);
					DWORD colorGray = D3DCOLOR_ARGB(255, 0, 191, 255);
					if (curMob == selectedMOB && ((flashCounter/6)&1))
						color = D3DCOLOR_ARGB(255, 80, 255, 80);

					float barPos = -1;
					if (curMob == playerAvatar)
					{
						barPos = curMob->screenHealth / (float)curMob->screenMaxHealth;
					}
					if (barPos >= 1)
						barPos = -1;  // perfect health?  Don't show the bar.
					
					InfoRecord3D *itd = new InfoRecord3D(
							curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y + 1, curMob->spacePoint.location.z,
							curMob->name, barPos, color);
					infoList.Append(itd);

					if (curMob->guildName[0])
					{
						if (IsSame(curMob->guildName, "Administrator"))
							itd = new InfoRecord3D(
									curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
									curMob->spacePoint.location.y + 1.1f, curMob->spacePoint.location.z,
									curMob->guildName, -1, colorGrn, TRUE);
						else if (IsSame(curMob->guildName, "Moderator") || IsSame(curMob->guildName, "Trial Moderator"))
							itd = new InfoRecord3D(
									curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
									curMob->spacePoint.location.y + 1.1f, curMob->spacePoint.location.z,
									curMob->guildName, -1, colorYellow, TRUE);
						else
							itd = new InfoRecord3D(
									curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
									curMob->spacePoint.location.y + 1.1f, curMob->spacePoint.location.z,
									curMob->guildName, -1, colorGray, TRUE);
						itd->scale = 20.0f;
						infoList.Append(itd);
					}

					// draw baby dragon names
					for (int petIndex = 0; petIndex < 2 && curMob == selectedMOB; ++petIndex)
					{
						if (curMob->pet[petIndex].petDragonAnims[0])
						{
							DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);

							D3DXVECTOR3 pOut, pV;
							pV.x = 0; 
							pV.y = 0;
							pV.z = -0.2f;

							float offsetAngle = D3DX_PI*0.4f;
							if (1 == petIndex)
								offsetAngle = -D3DX_PI*0.4f;

							SetupObjectOrientation(curMob->spacePoint.angle * -1, 
								curMob->spacePoint.location.x + sin(-curMob->spacePoint.angle + offsetAngle) * 0.4f,
								curMob->spacePoint.location.y + 0.8f,
								curMob->spacePoint.location.z + cos(-curMob->spacePoint.angle + offsetAngle) * 0.4f);

							D3DXVec3TransformCoord(&pOut, &pV, &matWorldValue);

							InfoRecord3D *itd = new InfoRecord3D(
												curCamAngle * -1 + D3DX_PI, pOut.x, pOut.y, pOut.z,
												curMob->pet[petIndex].petName, -1, color);
							infoList.Append(itd);
						}
					}
				}

//				SetupObjectOrientation(0, 0, 0, 0);
//				tCross->Draw(puma->m_pd3dDevice, curMob->spacePoint.location, 0.2f, 1.0f);

			}

/*
			LEFT OFF
				have to increase size of bat scaling, plus move bat upward
				(it's flying on the floor)

  */



			else
			{
				if (SMOB_MONSTER == curMob->type)
				{
					puma->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);

					if (curMob->monsterType > 22)  // wurm
						puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
					else if (curMob->monsterType > 8)
						puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_CW );
					else
						puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

					PumaAnim *mAnim[MONSTER_STATE_MAX];

					if (curMob->customMonsterAnims[MONSTER_STATE_STAND])
						mAnim[MONSTER_STATE_STAND] = curMob->customMonsterAnims[MONSTER_STATE_STAND];
					else
						mAnim[MONSTER_STATE_STAND] = curMob->monsterAnims[MONSTER_STATE_STAND]
								                     [curMob->monsterType]
															[curMob->monsterSubType];

					if (curMob->customMonsterAnims[MONSTER_STATE_RUN])
						mAnim[MONSTER_STATE_RUN] = curMob->customMonsterAnims[MONSTER_STATE_RUN];
					else
						mAnim[MONSTER_STATE_RUN] = curMob->monsterAnims[MONSTER_STATE_RUN]
								                     [curMob->monsterType]
															[curMob->monsterSubType];

					if (curMob->customMonsterAnims[MONSTER_STATE_ATTACK])
						mAnim[MONSTER_STATE_ATTACK] = curMob->customMonsterAnims[MONSTER_STATE_ATTACK];
					else
						mAnim[MONSTER_STATE_ATTACK] = curMob->monsterAnims[MONSTER_STATE_ATTACK]
								                     [curMob->monsterType]
															[curMob->monsterSubType];


					if (27 != curMob->monsterType || 0 == curMob->inSecondForm)
					{
						if (curMob->dying && curMob->animCounter > 20)
						{
							if (curMob->animCounter & 1)
							{
								if (mAnim[MONSTER_STATE_STAND] &&
									 mAnim[MONSTER_STATE_STAND]->numOfFrames > 0)
								{
									mAnim[MONSTER_STATE_STAND]->
								       Draw(puma->m_pd3dDevice, 
										      mAnim[MONSTER_STATE_STAND]->numOfFrames / 2);
								}
								else
									standinMesh->Draw(puma->m_pd3dDevice);
							}
						}
						else if (curMob->attacking)
						{
							if (mAnim[MONSTER_STATE_ATTACK] &&
								 mAnim[MONSTER_STATE_ATTACK]->numOfFrames > 0)
							{
								mAnim[MONSTER_STATE_ATTACK]->
							       Draw(puma->m_pd3dDevice, 
									        (curMob->animCounter/2) % 
											   mAnim[MONSTER_STATE_ATTACK]->numOfFrames);
							}
							else
								standinMesh->Draw(puma->m_pd3dDevice);
							if (curMob->animCounter/2 >= 
								 curMob->monsterAnims[MONSTER_STATE_ATTACK][curMob->monsterType][curMob->monsterSubType]->numOfFrames)
								curMob->attacking = FALSE;
						}
						else if (curMob->attackingSpecial)
						{
							if (mAnim[MONSTER_STATE_RUN] &&
								 mAnim[MONSTER_STATE_RUN]->numOfFrames > 0)
							{
								SetupObjectOrientation(
									 curMob->spacePoint.angle * curMob->animCounter / 10.0f, 
									 curMob->spacePoint.location.x,
									 curMob->spacePoint.location.y,
									 curMob->spacePoint.location.z);

								mAnim[MONSTER_STATE_RUN]->
							       Draw(puma->m_pd3dDevice, 
									        (curMob->animCounter/2) % 
											   mAnim[MONSTER_STATE_RUN]->numOfFrames);
							}
							else
								standinMesh->Draw(puma->m_pd3dDevice);
							if (curMob->animCounter/12 >= 
								 curMob->monsterAnims[MONSTER_STATE_RUN][curMob->monsterType][curMob->monsterSubType]->numOfFrames)
								curMob->attackingSpecial = FALSE;
						}
						else if (curMob->moving)
						{
							if (mAnim[MONSTER_STATE_RUN] &&
								 mAnim[MONSTER_STATE_RUN]->numOfFrames > 0)
							{
								mAnim[MONSTER_STATE_RUN]->
							       Draw(puma->m_pd3dDevice, 
									        ((curMob->animCounter/2) + 
											  ((int)curMob % 10)) % 
											   mAnim[MONSTER_STATE_RUN]->numOfFrames);
							}
							else
								standinMesh->Draw(puma->m_pd3dDevice);
						}
						else
						{
							if (mAnim[MONSTER_STATE_STAND] &&
								 mAnim[MONSTER_STATE_STAND]->numOfFrames > 0)
							{
								mAnim[MONSTER_STATE_STAND]->
								    Draw(puma->m_pd3dDevice, 
								        ((curMob->animCounter/4) + 
										  ((int)curMob % 10)) % 
										   mAnim[MONSTER_STATE_STAND]->numOfFrames);
							}
							else
								standinMesh->Draw(puma->m_pd3dDevice);
						}
					}
					if (27 == curMob->monsterType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						DWORD color = 0xffffffff;
						color |= 0x5f000000;
						if (0 == curMob->inSecondForm)
						{
							pos.y += rnd(0,0.2f);
							swordParticle->SetEmissionPoint(pos);
							swordParticle->Emit(5, color);
						}
						else
						{
							pos.y += rnd(0,0.8f);
							magicPlaceParticle->SetEmissionPoint(pos);
							magicPlaceParticle->Emit(5, color);

							spellParticle->SetEmissionPoint(pos);
							spellParticle->Emit(5, color);
						}
					}

					if (curMob->magicAttackType > -1)
					{
						++(curMob->magicAttackCounter);
						if (curMob->magicAttackCounter > 30)
							curMob->magicAttackType = -1;

						if (curMob->magicAttackCounter > 10)
						{
							D3DXVECTOR3 pos = curMob->spacePoint.location;
							DWORD color = curMob->magicAttackColor;
							color |= 0x5f000000;
							pos.y += rnd(0,1.2f);
							spellParticle->SetEmissionPoint(pos);
							spellParticle->Emit(5, color);
						}


					}
				}
				else if (SMOB_WARP_POINT == curMob->type)
				{
				}
				else if (SMOB_ITEM_SACK == curMob->type)
				{
					sackMesh->Draw(puma->m_pd3dDevice);
				}
				else if (SMOB_TOKEN == curMob->type)
				{
					tokenMesh[curMob->mobID]->Draw(puma->m_pd3dDevice);
					curMob->spacePoint.angle += 0.02f;
				}
				else if (SMOB_TREE == curMob->type)
				{
					greatTreeMesh->Draw(puma->m_pd3dDevice);
				}
				else if (SMOB_TOWER == curMob->type)
				{
					if (255 == curMob->towerType)
						towerMesh->Draw(puma->m_pd3dDevice);
					else if (curMob->towerType < 5)
						caveMesh[curMob->towerType]->Draw(puma->m_pd3dDevice);
				}
				else if( SMOB_ROPENTERANCE == curMob->type )
				{
					curMob->spacePoint.angle = 0;
					ropEnteranceMesh->Draw(puma->m_pd3dDevice);
				}
				else if( SMOB_CASTLE == curMob->type)
				{
					castleMesh->Draw(puma->m_pd3dDevice);
				}
				else if (SMOB_PARTICLE_STREAM == curMob->type)
				{
					D3DXVECTOR3 pos = curMob->spacePoint.location;
//					pos.x += rnd(-0.05f,0.05f);
//					pos.z += rnd(-0.05f,0.05f);
					superSplashParticle->SetEmissionPoint(pos);
					superSplashParticle->Emit(1, curMob->particleStreamColor);
				}
				else if (SMOB_GROUND_EFFECT == curMob->type)
				{
					float visAmount = curMob->particleStreamAge / 10.0f;
					if (visAmount < 1)
					{
						if ((rand() % 10) < (flashCounter%10))
							visAmount = 1;
					}

					if (0 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
	//					pos.x += rnd(-0.05f,0.05f);
	//					pos.z += rnd(-0.05f,0.05f);
						superSplashParticle->SetEmissionPoint(pos);
						superSplashParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (1 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.x += rnd(-5,5);
						pos.z += rnd(-5,5);
						superSplashParticle->SetEmissionPoint(pos);
						superSplashParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (2 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.y += rnd(0,1.2f);
						spellParticle->SetEmissionPoint(pos);
						spellParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (3 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.x += rnd(-5,5);
						pos.z += rnd(-5,5);
						magicPlaceParticle->SetEmissionPoint(pos);
						magicPlaceParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (4 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.y += 1.3f;
						pos.x += rnd(-5,5);
						pos.z += rnd(-5,5);
						driftDownParticle->SetEmissionPoint(pos);
						driftDownParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (5 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.y += 0.6f;
						eventParticle->SetEmissionPoint(pos);
						eventParticle->Emit(visAmount, curMob->particleStreamColor);
					}
					else if (6 == curMob->particleStreamType)
					{
						D3DXVECTOR3 pos = curMob->spacePoint.location;
						pos.y += 0.7f;
						pos.x += rnd(-5,5);
						pos.z += rnd(-5,5);
						mistParticle->SetEmissionPoint(pos);
						mistParticle->Emit(visAmount, curMob->particleStreamColor);
					}
				}
				else if (SMOB_CHEST == curMob->type)
				{
					chestMesh[curMob->chestType]->Draw(puma->m_pd3dDevice);
				}
				else if (SMOB_TRAINER == curMob->type)
				{
//					trainerMesh->Draw(puma->m_pd3dDevice);
					puma->m_pd3dDevice->SetTexture( 0, trainerBitmap );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

					frameCalc = (curMob->animCounter/3) % (merchantAnim->numOfFrames + 10);
					frameCalc -= 5;
					if (frameCalc < 0)
						frameCalc = 0;
					if (frameCalc > 7)
					{
						frameCalc -= 5;
						if (frameCalc < 7)
							frameCalc = 7;
					}
					merchantAnim->Draw(puma->m_pd3dDevice, frameCalc);
				}
				else if (SMOB_TOWNMAGE == curMob->type)
				{
					puma->m_pd3dDevice->SetTexture( 0, townmageBitmap );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

					frameCalc = (curMob->animCounter/3) % (merchantAnim->numOfFrames + 10);
					frameCalc -= 5;
					if (frameCalc < 0)
						frameCalc = 0;
					if (frameCalc > 7)
					{
						frameCalc -= 5;
						if (frameCalc < 7)
							frameCalc = 7;
					}
					merchantAnim->Draw(puma->m_pd3dDevice, frameCalc);

//					if (!(curMob->animCounter%6))
//					{
//							footstep->PositionSound(curMob->spacePoint.location, MAX_SOUND_DIST);
//							footstep->Play();
//					}
				}
				else if (SMOB_WITCH == curMob->type)
				{
					if (witchTalkCounter > 0)
					{
						--witchTalkCounter;
						frameCalc = (curMob->animCounter/4) % (witchTalkAnim->numOfFrames);
						witchTalkAnim->Draw(puma->m_pd3dDevice, frameCalc);
					}
					else
					{
						frameCalc = (curMob->animCounter/4) % (witchStandAnim->numOfFrames);
						witchStandAnim->Draw(puma->m_pd3dDevice, frameCalc);
					}
				}
				else
				{
					puma->m_pd3dDevice->SetTexture( 0, merchantBitmap );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
					puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

					frameCalc = (curMob->animCounter/3) % (merchantAnim->numOfFrames + 10);
					frameCalc -= 5;
					if (frameCalc < 0)
						frameCalc = 0;
					if (frameCalc > 7)
					{
						frameCalc -= 5;
						if (frameCalc < 7)
							frameCalc = 7;
					}
					merchantAnim->Draw(puma->m_pd3dDevice, frameCalc);
//					tCross->Draw(puma->m_pd3dDevice, curMob->spacePoint.location, 0.2f, 1.0f);
				}

				if (centerCellX == curMob->cellX && 
					 centerCellY == curMob->cellY)
				{
					DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);
					if (curMob == selectedMOB && ((flashCounter/6)&1))
						color = D3DCOLOR_ARGB(255, 80, 255, 80);
					else if (controlledMonsterID == curMob->mobID)
						color = D3DCOLOR_ARGB(255, 255, 0, 80);

					float barPos = -1;
					if (curMob->screenHealthTimer > 0)
						barPos = curMob->screenHealth / (float)curMob->screenMaxHealth;

					InfoRecord3D *itd = new InfoRecord3D(
							curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
							curMob->spacePoint.location.y + 1, curMob->spacePoint.location.z,
							curMob->name, barPos, color);
					infoList.Append(itd);
/*
					SetupObjectOrientation(curCamAngle * -1 + D3DX_PI, curMob->spacePoint.location.x,
														 curMob->spacePoint.location.y + 1,
												 curMob->spacePoint.location.z);

					CD3DFont *pf = puma->GetDXFont(1);
					float oldScale = pf->m_fTextScale;
					pf->m_fTextScale = 15.0f;
					DWORD color = D3DCOLOR_ARGB(255, 255, 255, 255);
					if (curMob == selectedMOB)
						color = D3DCOLOR_ARGB(255, 80, 255, 80);

					puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
					pf->Render3DText(curMob->name, D3DFONT_CENTERED | D3DFONT_TWOSIDED | D3DFONT_FILTERED, color);
					pf->m_fTextScale = oldScale;
					puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
					*/
				}
			}
		}
		else if (fabs(dCamX) >= 50 || fabs(dCamY) >= 50)
		{
			++(curMob->beyondRange);
		}
		else
		{
			curMob->beyondRange = 0;
		}
		++curMob->animCounter;
		curMob = (BBOMob *) monsterList->Next();
	}

	bladeTrail->FinishAddingQuads();

//	puma->PumpInput();
	if (whiteAvatarCount > 100)
	{
		whiteAvatarCount = 0;
		MessRequestAvatarInfo info;
		info.avatarID = whiteAvatarID;
		lclient->SendMsg(sizeof(info),&info);
	}
	
	//************
	if (SPACE_DUNGEON != curMapType && SPACE_GUILD != curMapType)
	{

		puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
		puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
		puma->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
		puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		BBOStatic *curStatic = (BBOStatic *) staticList->First();
		while (curStatic)
		{
			if (abs(curStatic->spacePoint.location.x - 
					playerPosForClient.x) < 50 &&
				 abs(curStatic->spacePoint.location.z - 
					playerPosForClient.z) < 50)
			{
				float dCamX = curStatic->spacePoint.location.x + camPosX;
				float dCamY = curStatic->spacePoint.location.z + camPosY;
				float dCamAngle = atan2(dCamX, -dCamY);
				dCamAngle   = NormalizeAngle(dCamAngle);
				dCamAngle  -= curCamAngle;
				dCamAngle   = NormalizeAngle2(dCamAngle);

				if (fabs(dCamAngle) < (D3DX_PI/5) || (fabs(dCamX) < 13 && fabs(dCamY) < 13))
				{
					if (SPACE_REALM != curMapType && SPACE_LABYRINTH != curMapType)
					{
						if (curStatic->WhatAmI() > 16)
						{
							int amount = curStatic->flowerLocListSize;
							if (0 == grassDensity)
								amount = 0;
							else if (1 == grassDensity)
								amount /= 3;
							else if (2 == grassDensity)
								amount = amount * 2 / 3;
							for (int t = 0; t < amount; ++t)
							{
								SetupObjectOrientation(t/20.0f, 
									       curStatic->flowerLocList[t].x,
									       curStatic->flowerLocList[t].y,
									       curStatic->flowerLocList[t].z);

								curStatic->meshes[curStatic->WhatAmI()]->Draw(puma->m_pd3dDevice);
							}
						}
						else
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							curStatic->meshes[curStatic->WhatAmI()]->Draw(puma->m_pd3dDevice);
						}
					}
					else if (SPACE_REALM == curMapType)
					{
						D3DXVECTOR3 pos = curStatic->spacePoint.location;
						if (SLOT_COLUMN == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							pos.x += rnd(-0.05f,0.05f);
							pos.z += rnd(-0.05f,0.05f);
							magicPlaceParticle->SetEmissionPoint(pos);
							magicPlaceParticle->Emit(1, D3DCOLOR_ARGB(55, 
								     ((int)pos.x % 255), 
									  ((int)pos.x + 80) % 255, 255));
						}
						else if (SLOT_DESERT_TREE1 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							realm->ruinMesh[1]->Draw(puma->m_pd3dDevice);
						}
						else if (SLOT_DESERT_TREE2 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							realm->ruinMesh[2]->Draw(puma->m_pd3dDevice);
						}
						else if (SLOT_CABIN == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							realm->ruinMesh[0]->Draw(puma->m_pd3dDevice);
						}
						else if (!(rand() % 4) && (localInfoFlags & LOCAL_FLAGS_MIST_ON))
						{
							pos.x += rnd(-5.0f,5.0f);
							pos.z += rnd(-5.0f,5.0f);
							pos.y += 0.7f;
							mistParticle->SetEmissionPoint(pos);
							mistParticle->Emit(1, D3DCOLOR_ARGB(55, 255,255,255));
						}
					}
					else if (SPACE_LABYRINTH == curMapType)
					{
						D3DXVECTOR3 pos = curStatic->spacePoint.location;

						if (SLOT_TUNDRA_TREE1 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							labyrinth->objectMesh[0]->Draw(puma->m_pd3dDevice);
						}
						else if (SLOT_TUNDRA_TREE2 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							labyrinth->objectMesh[1]->Draw(puma->m_pd3dDevice);
						}
						else if (SLOT_COLUMN == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y - 1,
													 curStatic->spacePoint.location.z);

							labyrinth->objectMesh[2]->Draw(puma->m_pd3dDevice);
						}
						else if (SLOT_DESERT_TREE1 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
						{
							SetupObjectOrientation(curStatic->spacePoint.angle * -1, curStatic->spacePoint.location.x,
													 curStatic->spacePoint.location.y,
													 curStatic->spacePoint.location.z);

							labyrinth->objectMesh[3]->Draw(puma->m_pd3dDevice);

							if (!(rand() % 4) && (localInfoFlags & LOCAL_FLAGS_MIST_ON))
							{
								pos.y += 2.0f;
								magicPlaceParticle->SetEmissionPoint(pos);
								if (REALM_ID_LAB2 == labyrinth->labyrinthType)
									magicPlaceParticle->Emit(1, D3DCOLOR_ARGB(55, 255,60,0));
								else
									magicPlaceParticle->Emit(1, D3DCOLOR_ARGB(55, 255,255,0));
							}
							/*
							if (localInfoFlags & LOCAL_FLAGS_MIST_ON)
							{
								AssertPointLight(	curStatic->spacePoint.location.x,
									               curStatic->spacePoint.location.y + 2.0f,
														curStatic->spacePoint.location.z,
														1.0f,1.0f,0.2f, 30);
							}
							*/
						}
						/*
						else if (!(rand() % 4) && (localInfoFlags & LOCAL_FLAGS_MIST_ON))
						{
							pos.x += rnd(-5.0f,5.0f);
							pos.z += rnd(-5.0f,5.0f);
							pos.y += 0.7f;
							mistParticle->SetEmissionPoint(pos);
							mistParticle->Emit(1, D3DCOLOR_ARGB(55, 255,255,255));
						}
						*/
					}
				}  // end check for inside frustrum


				if (SPACE_LABYRINTH == curMapType)
				{
					if (SLOT_DESERT_TREE1 == curStatic->WhatAmI()+SLOT_DESERT_TREE1)
					{
						if (localInfoFlags & LOCAL_FLAGS_MIST_ON)
						{
							if (REALM_ID_LAB2 == labyrinth->labyrinthType)
								AssertPointLight(	curStatic->spacePoint.location.x,
									               curStatic->spacePoint.location.y + 2.0f,
														curStatic->spacePoint.location.z,
														1.0f,0.2f,0.2f, 30);
							else
								AssertPointLight(	curStatic->spacePoint.location.x,
									               curStatic->spacePoint.location.y + 2.0f,
														curStatic->spacePoint.location.z,
														1.0f,1.0f,0.2f, 30);
						}
					}
				}



			}
			else
				curStatic->pleaseDelete = TRUE;

			curStatic = (BBOStatic *) staticList->Next();
		}

		curStatic = (BBOStatic *) staticList->First();
		while (curStatic)
		{
			if (curStatic->pleaseDelete)
			{
				staticList->Remove(curStatic);
				if (curStatic->cellSlot > -1)
				{
//					ground->ReleaseSlot(curStatic->cellX, curStatic->cellY, curStatic->cellSlot);
					flockSet->FreePoint(curStatic->flockPointIndex, curStatic);
				}
				delete curStatic;
			}
			curStatic = (BBOStatic *) staticList->Next();
		}
	}

	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	if (SPACE_GROUND == curMapType && grassDensity)
	{
		simpleGrass[0]->PrepareToDraw();
		simpleGrass[0]->Draw(matTrans);
		simpleGrass[1]->Draw(matTrans);
		simpleGrass[2]->Draw(matTrans);
		simpleGrass[3]->Draw(matTrans);
	}
   puma->m_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);

	// draw text info 
	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	InfoRecord3D *itd = (InfoRecord3D *)infoList.First();
	while (itd)
	{
		itd->Draw(barArt[0], barArt[1]);
		itd = (InfoRecord3D *)infoList.Next();
	}
	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

//	puma->PumpInput();
	// Turn off the zbuffer
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , FALSE );

	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	flareParticle->PrepareToDraw();
	flareParticle->Draw(matTrans);
	magicPlaceParticle->Draw(matTrans);
	swordParticle->Draw(matTrans);
	driftDownParticle->Draw(matTrans);
	eventParticle->Draw(matTrans);
	petHitParticle->Draw(matTrans);
	mistParticle->Draw(matTrans);
	superSplashParticle->Draw(matTrans);
	bombFireParticle->Draw(matTrans);
	spellParticle->Draw(matTrans);
	haloGlowSet->Draw(matTrans);

	bladeTrail->Draw(matTrans);

//	smokeParticle->PrepareToDraw();
	smokeParticle->Draw(matTrans);

	// Turn on the zbuffer
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );

	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

	DWORD thisTime = timeGetTime();
	if (lastTime1 && showFrameRate)
	{
		CD3DFont *pf = puma->GetDXFont(0);
		char tempText[128];
		sprintf(tempText,"FPS %d", 1000 / (thisTime - lastTime1));
		RECT rect = {puma->ScreenW() - 180,puma->ScreenH()-12,puma->ScreenW()-130,puma->ScreenH()};
		pf->DrawText(rect,NULL,0xffffffff,tempText);

	}
	lastTime1 = thisTime;

	aLog.Log("BBOClient::Draw 13 **\n");

//	DebugOutput("En Draw\n");
}


//***************************************************************
void BBOClient::HandleMessage(char *messData, int dataSize)
{
	MessInventoryRequestInfo requestInfo;
	MessRequestDungeonInfo   messDInfo;

	MessAvatarAppear    *messAvAppearPtr;
	MessAvatarAppearSpecial    *messAvAppearSpecialPtr;
	MessAvatarDisappear *messAvDisappearPtr;
	MessAvatarMove *AvMovePtr;
	MessAvatarStats     *messAvStats;

	MessMobAppear    *messMobAppearPtr;
	MessMobAppearCustom    *messMobAppearCustomPtr;
	MessMobDisappear *messMobDisappearPtr;
	MessTokenDisappear *messTokenDisappearPtr;

	MessMobBeginMove *mobBMovePtr;
	MessMobStats     *messMobStats;
	MessBladeDesc    *messBladeDescPtr;
	MessUnWield      *messUnWieldPtr;
	MessAvatarAttack *messAAPtr;
	MessMonsterAttack*messMAPtr;
	MessAvatarDeath  *messADPtr;
	MessMonsterDeath *messMDPtr;
//	MessAvatarHealth *messAHPtr;
	MessChangeMap       *messChangeMapPtr;
	MessDungeonInfo     *messDungeonInfoPtr;
	MessDungeonChange   *messDungeonChangePtr;
	MessAvatarHealth    *messHealth;
	MessMonsterHealth   *messMH;
	MessCaveInfo        *messCInfoPtr;
	MessChestInfo       *messChestPtr;
	MessGenericEffect	  *messGE;
	MessChant      	  *messChant;
	MessPet         	  *messPet;
	MessPetAttack       *messPAPtr;
	MessInfoFlags       *infoFlagsPtr;
	AvatarGuildEdit     *avGuildEditPtr;
	MessExplosion       *explosionPtr;
	MessMagicAttack     *messMagAPtr;
	MessTimeOfDay       *messTimeOfDayPtr;
	MessWeatherState    *messWeatherStatePtr;
	MessGroundEffect    *messGroundEffectPtr;
	MessMonsterSpecialAttack *messMASPtr;
	MessMonsterChangeForm    *messMonsterChangePtr;


	D3DXVECTOR3 vecDir;
//	D3DLIGHT8 light;

	Chronos::BStream *	stream		= NULL;
//	DebugOutput("St Handle mess\n");

	char tempText[1024];


	BBOMob *tempMob, *tempMob2, *tempMob3;
//	BBOStatic *curStatic;
	FlockPoint *fp;
	int found, slotIndex; //, x, y;
	float slotX, slotY, slotAngle;

	unsigned char tempUC;
	int tempInt;
//	float petScaleVal = 0.005f;

	switch(messData[0])
	{
	case NWMESS_AVATAR_APPEAR:
	case NWMESS_AVATAR_APPEAR_SPECIAL:
		messAvAppearPtr = (MessAvatarAppear *) messData;

		found = FALSE;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == playerAvatar->avatarID)
				{
					if (playerAvatar->pet[0].petDragonAnims[0] &&
						 0 == playerAvatar->pet[0].petName[0])
						needToName = TRUE;
					if (playerAvatar->pet[1].petDragonAnims[0] &&
						 0 == playerAvatar->pet[1].petName[0])
						needToName = TRUE;
				}

				if (tempMob->avatarID == messAvAppearPtr->avatarID)
				{
					if (tempMob->cellSlot != -1)
					{
						fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);
						fp->maxSpeed = 0.12f;
						ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					}
					else
					{
						tempMob->flockPointIndex = flockSet->MakePoint(tempMob);
						fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);
						fp->maxSpeed = 0.12f;
					}
					tempMob->cellX = messAvAppearPtr->x;
					tempMob->cellY = messAvAppearPtr->y;
					slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);
					if (-1 == slotIndex)
					{
						tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX);
						tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY);
						fp->x = fp->targetX = tempMob->spacePoint.location.x;
						fp->y = fp->targetY = tempMob->spacePoint.location.z;
					}
					else
					{
						ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
						ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
						tempMob->cellSlot = slotIndex;
						fp->targetX = ground->GetXForPoint(tempMob->cellX) + slotX;
						fp->targetY = ground->GetZForPoint(tempMob->cellY) + slotY;
						fp->targetAngle = slotAngle;
						fp->isCaring = FALSE;
					}

//					tempMob->spacePoint.location.y = HeightAtPoint(tempMob->spacePoint.location.x, tempMob->spacePoint.location.z,NULL);
					found = TRUE;
					if (tempMob->avatarID == playerAvatarID && -1 == controlledMonsterID)
					{
						ground->m_CameraPos   = tempMob->spacePoint.location;
						ground->m_CameraAngle = 0;
						ground->Draw(tempMob->cellX, tempMob->cellY);
						HandlePlayerAppearance(tempMob->cellX, tempMob->cellY);
						monsterList->Find(tempMob);
					}
					if (NWMESS_AVATAR_APPEAR_SPECIAL == messData[0])
					{
						messAvAppearSpecialPtr = (MessAvatarAppearSpecial *) messData;
						HandleSpecialAppearance(tempMob, messAvAppearSpecialPtr->typeOfAppearance);
					}

				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (found)
		{
			break;
		}
		tempMob = new BBOMob(0,"AVATAR");
		monsterList->Append(tempMob);
		tempMob->avTexture = new AvatarTexture();

		tempMob->flockPointIndex = flockSet->MakePoint(tempMob);

		fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);

		tempMob->cellX = messAvAppearPtr->x;
		tempMob->cellY = messAvAppearPtr->y;
		slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);
		if (-1 == slotIndex)
		{
			tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX);
			tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY);
		}
		else
		{
			ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
			ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
			tempMob->cellSlot = slotIndex;
			tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX) + slotX;
			tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY) + slotY;
		}
		tempMob->spacePoint.angle = rnd(0,D3DX_PI);
		tempMob->spacePoint.azimuth = 0;
		tempMob->spacePoint.location.y = HeightAtPoint(tempMob->spacePoint.location.x, tempMob->spacePoint.location.z);

		fp->angle = fp->targetAngle = tempMob->spacePoint.angle;
		fp->maxSpeed = 0.12f;
		fp->maxTurn = D3DX_PI/15;
		fp->targetAngle = slotAngle;
		fp->x = fp->targetX = tempMob->spacePoint.location.x;
		fp->y = fp->targetY = tempMob->spacePoint.location.z;
		fp->size = FPNORMALSIZE;


		tempMob->avatarID = messAvAppearPtr->avatarID;
		if (tempMob->avatarID == playerAvatarID && -1 == controlledMonsterID)
		{
			playerAvatar = tempMob;
			requestInfo.type = MESS_INVENTORY_PLAYER;
			requestInfo.offset = 0;
			lclient->SendMsg(sizeof(requestInfo),(void *)&requestInfo);

			ClearStatics();
			ground->m_CameraPos   = tempMob->spacePoint.location;
			ground->m_CameraAngle = 0;
			ground->Draw(tempMob->cellX, tempMob->cellY);
			HandlePlayerAppearance(tempMob->cellX, tempMob->cellY);
			monsterList->Find(tempMob);

			if (SPACE_GROUND != curMapType)
				;
			else
			{
				D3DXVECTOR3 dumPos;
				dumPos.x = dumPos.y = dumPos.z = -500.0f;

				if ((SPACE_GROUND == curMapType) && grassDensity)
				{
					simpleGrass[0]->AddTuft(ground, dumPos, dumPos);
					simpleGrass[1]->AddTuft(ground, dumPos, dumPos);
					simpleGrass[2]->AddTuft(ground, dumPos, dumPos);
					simpleGrass[3]->AddTuft(ground, dumPos, dumPos);
				}
			}
//			MaintainStaticObjects(tempMob->cellX, tempMob->cellY);

		}

		if (NWMESS_AVATAR_APPEAR_SPECIAL == messData[0])
		{
			messAvAppearSpecialPtr = (MessAvatarAppearSpecial *) messData;
			HandleSpecialAppearance(tempMob, messAvAppearSpecialPtr->typeOfAppearance);
		}

	   break;

	case NWMESS_AVATAR_DISAPPEAR:
		messAvDisappearPtr = (MessAvatarDisappear *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messAvDisappearPtr->avatarID)
				{
					monsterList->Remove(tempMob);
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
					tempMob->cellSlot = -1;

					delete tempMob;
					tempMob = (BBOMob *) monsterList->First();
					if (messAvDisappearPtr->avatarID == playerAvatarID)
					{
						playerAvatar = dummyAvatar;
					}
				}
				else
					tempMob = (BBOMob *) monsterList->Next();
			}
			else
				tempMob = (BBOMob *) monsterList->Next();
		}

	   break;

	case NWMESS_AVATAR_MOVE:
		AvMovePtr = (MessAvatarMove *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == AvMovePtr->avatarID)
				{
					fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);
					fp->maxSpeed = 0.12f;

					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					tempMob->cellX = AvMovePtr->targetX;
					tempMob->cellY = AvMovePtr->targetY;
					slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);
					if (-1 == slotIndex)
					{
						fp->targetX = ground->GetXForPoint(AvMovePtr->targetX);
						fp->targetY = ground->GetZForPoint(AvMovePtr->targetY);
					}
					else
					{
						ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
						ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
						tempMob->cellSlot = slotIndex;
						fp->targetX = ground->GetXForPoint(AvMovePtr->targetX) + slotX;
						fp->targetY = ground->GetZForPoint(AvMovePtr->targetY) + slotY;
						fp->targetAngle = slotAngle;
					}
					if (tempMob->avatarID == playerAvatarID)
					{
							moveLockoutTimer = 30 * 3;
					}
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

	   break;

	case NWMESS_AVATAR_STATS:
		messAvStats = (MessAvatarStats *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messAvStats->avatarID)
				{
					tempMob->imageFlags = messAvStats->imageFlags;
					tempMob->avTexture->Generate(puma->m_pd3dDevice, 
						messAvStats->faceIndex,	
						messAvStats->hairR, messAvStats->hairG, messAvStats->hairB,
						messAvStats->topIndex,	
						messAvStats->topR, messAvStats->topG, messAvStats->topB,
						messAvStats->bottomIndex,	
						messAvStats->bottomR, messAvStats->bottomG, messAvStats->bottomB,
						tempMob->imageFlags);

					sprintf(tempMob->name,messAvStats->name);
					if (playerAvatar->avatarID == messAvStats->avatarID)
					{
						avatarNameChange = playerAvatar->name;
						gGamePlayerLevel = messAvStats->cLevel;
						gGamePlayerAge   = messAvStats->age;
					}
				}

			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_MOB_APPEAR:
	case NWMESS_MOB_APPEAR_CUSTOM:
		messMobAppearPtr = (MessMobAppear *) messData;

		found = FALSE;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (-1 == tempMob->avatarID)
			{
				if (tempMob->mobID == messMobAppearPtr->mobID)
				{
					if (SMOB_ITEM_SACK       != messMobAppearPtr->type && 
						 SMOB_TOKEN           != messMobAppearPtr->type &&
						 SMOB_TOWER           != messMobAppearPtr->type &&
						 SMOB_WARP_POINT      != messMobAppearPtr->type &&
						 SMOB_GROUND_EFFECT   != messMobAppearPtr->type &&
						 SMOB_PARTICLE_STREAM != messMobAppearPtr->type)
					{
						fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);
						ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
						tempMob->cellX = messMobAppearPtr->x;
						tempMob->cellY = messMobAppearPtr->y;
						slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);
						if (-1 == slotIndex)
						{
							tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX);
							tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY);
							fp->x = fp->targetX = tempMob->spacePoint.location.x;
							fp->y = fp->targetY = tempMob->spacePoint.location.z;
						}
						else
						{
							ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
							ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
							tempMob->cellSlot = slotIndex;
							fp->targetAngle = slotAngle;
							fp->targetX = ground->GetXForPoint(tempMob->cellX) + slotX;
							fp->targetY = ground->GetZForPoint(tempMob->cellY) + slotY;
						}
					}
					if (controlledMonsterID == tempMob->mobID)
					{
						ground->m_CameraPos   = tempMob->spacePoint.location;
						ground->m_CameraAngle = 0;
						ground->Draw(tempMob->cellX, tempMob->cellY);
						HandlePlayerAppearance(tempMob->cellX, tempMob->cellY);
						monsterList->Find(tempMob);
					}
					if (SMOB_MONSTER == messMobAppearPtr->type && 
						 NWMESS_MOB_APPEAR_CUSTOM == messData[0])
					{
						messMobAppearCustomPtr = (MessMobAppearCustom *) messData;
						tempMob->InitCustomMonster(
							 messMobAppearCustomPtr->name,
							 messMobAppearCustomPtr->a,
							 messMobAppearCustomPtr->r,
							 messMobAppearCustomPtr->g,
							 messMobAppearCustomPtr->b,
							 messMobAppearCustomPtr->sizeCoeff);
					}

					found = TRUE;
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (found)
			break;
		tempMob = new BBOMob(0,"MOB");
		monsterList->Append(tempMob);
		tempMob->type = messMobAppearPtr->type;  // type of object is set here.

		// break point trap.
		if (tempMob->type >= SMOB_MAX)
			tempMob->type = messMobAppearPtr->type;  // type of object is set here.

		if (SMOB_MONSTER == messMobAppearPtr->type)
		{
			tempMob->InitMonsterInfo(messMobAppearPtr->monsterType, messMobAppearPtr->subType);
			if ((SPACE_DUNGEON == curMapType) || (SPACE_GUILD == curMapType))
				tempMob->staticMonsterFlag = messMobAppearPtr->staticMonsterFlag;

			if (NWMESS_MOB_APPEAR_CUSTOM == messData[0])
			{
				messMobAppearCustomPtr = (MessMobAppearCustom *) messData;
				tempMob->InitCustomMonster(
					 messMobAppearCustomPtr->name,
					 messMobAppearCustomPtr->a,
					 messMobAppearCustomPtr->r,
					 messMobAppearCustomPtr->g,
					 messMobAppearCustomPtr->b,
					 messMobAppearCustomPtr->sizeCoeff);
			}

		}
		else if (SMOB_TREE == messMobAppearPtr->type)
		{
			tempMob->monsterType = messMobAppearPtr->monsterType; // actually tree index
			sprintf(tempMob->name,"GREAT TREE OF THE %s", magicNameList[tempMob->monsterType]);
		}
		else if (SMOB_TOKEN == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"");
		}
		else if (SMOB_ITEM_SACK == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"Items");
		}
		else if (SMOB_TOWER == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"");

			BBOStatic *curStatic = (BBOStatic *) staticList->First();
			while (curStatic)
			{
				if (curStatic->cellX == messMobAppearPtr->x &&
					 curStatic->cellY == messMobAppearPtr->y)
					curStatic->pleaseDelete = TRUE;
				curStatic = (BBOStatic *) staticList->Next();
			}

			ground->ClearStaticPositionsFor(messMobAppearPtr->x,messMobAppearPtr->y);
		}
		else if (SMOB_CHEST == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"");
		}
		else if (SMOB_WARP_POINT == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"");
			tempMob->monsterSubType = messMobAppearPtr->subType;
		}
		else if (SMOB_TRAINER == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"Trainer");
		}
		else if (SMOB_TOWNMAGE == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"Town Mage");
		}
		else if (SMOB_WITCH == messMobAppearPtr->type)
		{
			sprintf(tempMob->name,"Witch");
		}
		else
		{
			sprintf(tempMob->name,"Merchant");
		}

		if (SMOB_ITEM_SACK != messMobAppearPtr->type && 
			 SMOB_TOKEN != messMobAppearPtr->type &&
			 SMOB_WARP_POINT != messMobAppearPtr->type &&
			 SMOB_TOWER != messMobAppearPtr->type)
		{
			tempMob->flockPointIndex = flockSet->MakePoint(tempMob);

			fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);

			tempMob->cellX = messMobAppearPtr->x;
			tempMob->cellY = messMobAppearPtr->y;

			if (SMOB_TREE == messMobAppearPtr->type)
				slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY, TRUE);
			else
				slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);

			if (-1 == slotIndex)
			{
				tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX);
				tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY);
			}
			else
			{
				ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
				ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
				tempMob->cellSlot = slotIndex;
				tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX) + slotX;
				tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY) + slotY;
			}
			tempMob->spacePoint.angle = rnd(0,D3DX_PI);
			tempMob->spacePoint.azimuth = 0;
			tempMob->spacePoint.location.y = HeightAtPoint(tempMob->spacePoint.location.x, tempMob->spacePoint.location.z);

			fp->angle = fp->targetAngle = tempMob->spacePoint.angle;
			fp->maxSpeed = 0.12f;
			fp->maxTurn = D3DX_PI/15;
			fp->targetAngle = slotAngle;
			fp->x = fp->targetX = tempMob->spacePoint.location.x;
			fp->y = fp->targetY = tempMob->spacePoint.location.z;
			fp->size = FPNORMALSIZE;
			if (SMOB_TREE == messMobAppearPtr->type)
				fp->size = 1.0f;
		}
		else
		{
			tempMob->cellX = messMobAppearPtr->x;
			tempMob->cellY = messMobAppearPtr->y;
			tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX) + ground->CellSize()/2;
			tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY) + ground->CellSize()/2;
			tempMob->spacePoint.angle = 0;
			tempMob->spacePoint.azimuth = 0;
			tempMob->spacePoint.location.y = HeightAtPoint(tempMob->spacePoint.location.x, tempMob->spacePoint.location.z);
		}

		tempMob->mobID = messMobAppearPtr->mobID;
	   break;

	case NWMESS_MOB_DISAPPEAR:
		messMobDisappearPtr = (MessMobDisappear *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (-1 == tempMob->avatarID && tempMob->mobID == messMobDisappearPtr->mobID)
			{
				monsterList->Remove(tempMob);
				if (SMOB_ITEM_SACK       != tempMob->type && 
				    SMOB_PARTICLE_STREAM != tempMob->type && 
				    SMOB_GROUND_EFFECT   != tempMob->type && 
				    SMOB_TOKEN           != tempMob->type && 
				    SMOB_WARP_POINT      != tempMob->type && 
					 SMOB_TOWER           != tempMob->type)
				{
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				}
				delete tempMob;
				tempMob = (BBOMob *) monsterList->First();
			}
			else
				tempMob = (BBOMob *) monsterList->Next();
		}

	   break;

	case NWMESS_GROUND_EFFECT:
		messGroundEffectPtr = (MessGroundEffect *) messData;

		found = FALSE;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (-1 == tempMob->avatarID)
			{
				if (tempMob->mobID == messGroundEffectPtr->mobID)
				{
					found = TRUE;
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (found)
			break;
		tempMob = new BBOMob(0,"MOB");
		monsterList->Append(tempMob);
		tempMob->type = SMOB_GROUND_EFFECT;  // type of object is set here.
		sprintf(tempMob->name,"");

		tempMob->cellX = messGroundEffectPtr->x;
		tempMob->cellY = messGroundEffectPtr->y;
		tempMob->spacePoint.location.x = ground->GetXForPoint(tempMob->cellX) + ground->CellSize()/2;
		tempMob->spacePoint.location.z = ground->GetZForPoint(tempMob->cellY) + ground->CellSize()/2;
		tempMob->spacePoint.angle = 0;
		tempMob->spacePoint.azimuth = 0;
		tempMob->spacePoint.location.y = HeightAtPoint(tempMob->spacePoint.location.x, tempMob->spacePoint.location.z);
		
		tempMob->mobID = messGroundEffectPtr->mobID;

		tempMob->particleStreamColor = D3DCOLOR_ARGB(55, 
			           messGroundEffectPtr->r, 
						  messGroundEffectPtr->g, 
						  messGroundEffectPtr->b);

		tempMob->particleStreamType = messGroundEffectPtr->type;
		tempMob->particleStreamAge  = messGroundEffectPtr->amount;

	   break;

	case NWMESS_TOKEN_DISAPPEAR:
		messTokenDisappearPtr = (MessTokenDisappear *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (messTokenDisappearPtr->type == tempMob->type && tempMob->mobID == messTokenDisappearPtr->mobID)
			{
				monsterList->Remove(tempMob);

				D3DXVECTOR3 pos = tempMob->spacePoint.location;
				pos.y += 1.1f;
				eventParticle->SetEmissionPoint(pos);
				D3DXCOLOR pColor      = D3DCOLOR_ARGB(55, 255, 240, 0);
				eventParticle->Emit(60, pColor);

				delete tempMob;

				tempMob = (BBOMob *) monsterList->First();
			}
			else
				tempMob = (BBOMob *) monsterList->Next();
		}

	   break;

	case NWMESS_MOB_BEGIN_MOVE:
	case NWMESS_MOB_BEGIN_MOVE_SPECIAL:
		mobBMovePtr = (MessMobBeginMove *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (-1 == tempMob->avatarID)
			{
				if (tempMob->mobID == mobBMovePtr->mobID)
				{
					fp = flockSet->GetPoint(tempMob->flockPointIndex, tempMob);

					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					tempMob->cellX = mobBMovePtr->targetX;
					tempMob->cellY = mobBMovePtr->targetY;
					slotIndex = ground->GetFirstOpenSlot(tempMob->cellX, tempMob->cellY);
					if (-1 == slotIndex)
					{
						fp->targetX = ground->GetXForPoint(mobBMovePtr->targetX);
						fp->targetY = ground->GetZForPoint(mobBMovePtr->targetY);
					}
					else
					{
						ground->ClaimSlot(tempMob->cellX, tempMob->cellY, slotIndex);
						ground->GetSlotPosition(slotIndex,slotX, slotY, slotAngle);
						tempMob->cellSlot = slotIndex;
						fp->targetX = ground->GetXForPoint(mobBMovePtr->targetX) + slotX;
						fp->targetY = ground->GetZForPoint(mobBMovePtr->targetY) + slotY;
						fp->targetAngle = slotAngle;
					}

					if (NWMESS_MOB_BEGIN_MOVE_SPECIAL == messData[0])
					{
						MessMobBeginMoveSpecial *moveSPtr = (MessMobBeginMoveSpecial *) messData;
						if (1 == moveSPtr->specialType)
						{
							controlledMonsterID = tempMob->mobID;
						}
					}
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

	   break;

	case NWMESS_MOB_STATS:
		messMobStats = (MessMobStats *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->mobID == messMobStats->mobID)
				{
					;
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_BLADE_DESC:

		messBladeDescPtr = (MessBladeDesc *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messBladeDescPtr->avatarID)
				{
					if (playerAvatar->avatarID == messBladeDescPtr->avatarID)
						wieldedPtr = messBladeDescPtr->bladeID;

					if (tempMob->blade)
						delete tempMob->blade;

					tempMob->blade = new PumaMesh();
					for (int i = 0; i < 4; ++i)
					{
						tempMob->bladeTrailVec[i].x = 0;
						tempMob->bladeTrailVec[i].y = 0;
						tempMob->bladeTrailVec[i].z = 0;
					}

					tempMob->bladeType = messBladeDescPtr->meshType;
					if (BLADE_TYPE_STAFF1 <= messBladeDescPtr->meshType)
					{
						tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\staff.ase");
						tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\staff.png");

						tempMob->bladeParticleColor[0] = D3DCOLOR_ARGB(55, messBladeDescPtr->r, 
							               messBladeDescPtr->g, messBladeDescPtr->b);
//						tempMob->bladeParticleAmount[0] = 1;
					}
					else
					{
						if (BLADE_TYPE_KATANA == messBladeDescPtr->meshType)
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\katana.ase");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\katana.png");
						}
						else if (BLADE_TYPE_CLAWS == messBladeDescPtr->meshType)
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\claws.ASE");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\claws.png");
						}
						else if (BLADE_TYPE_CHAOS == messBladeDescPtr->meshType)
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\chaosBlade.ASE");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\chaosBlade.png");
						}
						else if (BLADE_TYPE_DOUBLE == messBladeDescPtr->meshType)
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\bladeStaff.ASE");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\bladeStaff.png");
						}
						else if (BLADE_TYPE_MACE == messBladeDescPtr->meshType)
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\mace.ASE");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\mace.png");
						}
						else
						{
							tempMob->blade->LoadFromASC(puma->m_pd3dDevice, "dat\\blade.ase");
							tempMob->blade->LoadTexture(puma->m_pd3dDevice,	"dat\\bladeMetal.png");
						}
					}

					tempMob->blade->Scale(puma->m_pd3dDevice, 
												 0.001f * messBladeDescPtr->size,
												 0.001f * messBladeDescPtr->size,
												 0.001f * messBladeDescPtr->size);
					tempMob->blade->RebuildNormals(puma->m_pd3dDevice);
					tempMob->blade->SetColor(messBladeDescPtr->r, messBladeDescPtr->g,
						                      messBladeDescPtr->b, 255);
					tempMob->bladeLength = messBladeDescPtr->size;

					tempMob->bladeParticleAmount[DRAGON_TYPE_RED]   = messBladeDescPtr->red;
					tempMob->bladeParticleAmount[DRAGON_TYPE_WHITE] = messBladeDescPtr->white;
					tempMob->bladeParticleAmount[DRAGON_TYPE_BLUE]  = messBladeDescPtr->blue;
					tempMob->bladeParticleAmount[DRAGON_TYPE_GREEN] = messBladeDescPtr->green;
					tempMob->bladeParticleAmount[DRAGON_TYPE_BLACK] = messBladeDescPtr->black;

					tempMob->totalBPA = 0;
					for (int i = 0; i < 5; ++i)
						tempMob->totalBPA += tempMob->bladeParticleAmount[i];

					tempMob->bladeParticleColor[DRAGON_TYPE_BLUE]   = D3DCOLOR_ARGB(55, 0,0,255);
					tempMob->bladeParticleColor[DRAGON_TYPE_WHITE]  = D3DCOLOR_ARGB(55, 255,255,255);
					tempMob->bladeParticleColor[DRAGON_TYPE_RED]    = D3DCOLOR_ARGB(55, 255,0,0);
					tempMob->bladeParticleColor[DRAGON_TYPE_GREEN]  = D3DCOLOR_ARGB(55, 0,255,0);
					tempMob->bladeParticleColor[DRAGON_TYPE_BLACK]  = D3DCOLOR_ARGB(55, 125,65,125);

					switch(messBladeDescPtr->trailType)
					{
					default:
						tempMob->hasTrail = FALSE;
						break;

					case BLADE_GLAMOUR_TRAILWHITE:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 255, 255);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILRED:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 0,0);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILBLUE:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 0, 0, 255);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILGREEN:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 0, 255, 0);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILAQUA:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 0, 255, 255);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILPURPLE:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 0, 255);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILYELLOW:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 255, 0);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILORANGE:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 128, 0);
						tempMob->hasTrail = TRUE;
						break;
					case BLADE_GLAMOUR_TRAILPINK:
						tempMob->bladeTrailColor = D3DCOLOR_ARGB(55, 255, 155, 155);
						tempMob->hasTrail = TRUE;
						break;
					}

				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_UNWIELD:

		messUnWieldPtr = (MessUnWield *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messUnWieldPtr->bladeID)
				{
					if (playerAvatar->avatarID == messUnWieldPtr->bladeID)
						wieldedPtr = 0;

					if (tempMob->blade)
						delete tempMob->blade;
					tempMob->blade = NULL;
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_AVATAR_ATTACK:

		messAAPtr = (MessAvatarAttack *) messData;
		tempMob2 = tempMob3 = NULL;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messAAPtr->avatarID)
				{
					tempMob->attacking   = (rand() % 2) + 1;
					tempMob->animCounter = 0;
					tempMob2 = tempMob;
					if (messAAPtr->health != -1000)
					{
						tempMob->screenHealth    = messAAPtr->health;
						tempMob->screenMaxHealth = messAAPtr->healthMax;
					}
				}
			}
			else if (tempMob->mobID == messAAPtr->mobID)
				tempMob3 = tempMob;
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (tempMob2 && tempMob3)
		{
			if (-1 == messAAPtr->damage)
			{
				if (playerInfoFlags & INFO_FLAGS_MISSES)
				{
					sprintf(tempText,"%s misses the %s.",tempMob2->name, tempMob3->name);
					DisplayTextInfo(tempText);
				}
			}
			else if (-2 == messAAPtr->damage)
			{
				if (playerInfoFlags & INFO_FLAGS_HITS)
				{
					sprintf(tempText,"%s uses her staff.",tempMob2->name);
					DisplayTextInfo(tempText);
				}
			}
			else
			{
				if (playerInfoFlags & INFO_FLAGS_HITS)
				{
					sprintf(tempText,"%s hits the %s for %d damage.",
				        tempMob2->name, tempMob3->name, messAAPtr->damage);
					DisplayTextInfo(tempText);
				}
			}
			// turn to face the opponent
			fp  = flockSet->GetPoint(tempMob2->flockPointIndex, tempMob2);
			FlockPoint *fp2 = flockSet->GetPoint(tempMob3->flockPointIndex, tempMob3);
			fp->targetAngle = TurnTowardsPoint(fp->x, fp->y, fp2->x, fp2->y, 0,0); 
		}

		break;

	case NWMESS_MONSTER_DEATH:
//		DisplayTextInfo("monster death");

		messMDPtr = (MessMonsterDeath *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID <= -1 && tempMob->mobID == messMDPtr->mobID)
			{
				tempMob->dying = TRUE;
				tempMob->animCounter = 0;
				/*
				monsterList->Remove(tempMob);
				ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
				flockSet->FreePoint(tempMob->flockPointIndex);
				sprintf(tempText,"The %s is destroyed!\nIt drops something.\n", tempMob->name);
				DisplayTextInfo(tempText);
				delete tempMob;
				*/
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_MONSTER_ATTACK:

		messMAPtr = (MessMonsterAttack *) messData;
		tempMob2 = tempMob3 = NULL;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messMAPtr->avatarID)
				{
					tempMob3 = tempMob;
				}
			}
			else if (tempMob->mobID == messMAPtr->mobID)
			{
				tempMob->attacking   = 1;
				tempMob->animCounter = 0;
				tempMob2 = tempMob;
			}
			else if (tempMob->mobID == messMAPtr->avatarID)
			{
				tempMob3 = tempMob;
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (tempMob2 && tempMob3)
		{
			if (-1 == messMAPtr->damage)
			{
				if (playerInfoFlags & INFO_FLAGS_MISSES)
				{
					sprintf(tempText,"The %s misses %s.",tempMob2->name, tempMob3->name);
					DisplayTextInfo(tempText);
				}
			}
			else
			{
				if (playerInfoFlags & INFO_FLAGS_HITS)
				{
					sprintf(tempText,"The %s hits %s for %d damage.",
					        tempMob2->name, tempMob3->name, messMAPtr->damage);
					DisplayTextInfo(tempText);
				}
				if (tempMob3 == playerAvatar)
				{
					playerAvatar->screenHealth -= messMAPtr->damage;
					gGamePlayerHP    = playerAvatar->screenHealth;
					gGamePlayerMaxHP = playerAvatar->screenMaxHealth;
				}
				avatarPain->PositionSound(tempMob3->spacePoint.location, MAX_SOUND_DIST);
				avatarPain->Play();
			}

			// turn to face the opponent
			fp  = flockSet->GetPoint(tempMob2->flockPointIndex, tempMob2);
			FlockPoint *fp2 = flockSet->GetPoint(tempMob3->flockPointIndex, tempMob3);
			fp->targetAngle = TurnTowardsPoint(fp->x, fp->y, fp2->x, fp2->y, 0,0); 

		}

		break;

	case NWMESS_MONSTER_SPECIAL_ATTACK:

		messMASPtr = (MessMonsterSpecialAttack *) messData;
		tempMob2 = tempMob3 = NULL;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->mobID == messMASPtr->mobID)
			{
				tempMob->attackingSpecial = 1;
				tempMob->animCounter = 0;
				tempMob2 = tempMob;
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (tempMob2)
		{
			if (playerInfoFlags & INFO_FLAGS_MISSES)
			{
				sprintf(tempText,"The %s attacks everyone!",tempMob2->name);
				DisplayTextInfo(tempText);
			}
		}

		break;

	case NWMESS_MONSTER_CHANGE_FORM:

		messMonsterChangePtr = (MessMonsterChangeForm *) messData;
		tempMob2 = tempMob3 = NULL;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->mobID == messMonsterChangePtr->mobID)
			{
				tempMob->inSecondForm = messMonsterChangePtr->form;
				tempMob->animCounter = 0;
				tempMob2 = tempMob;
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		break;

	case NWMESS_AVATAR_DEATH:

		messADPtr = (MessAvatarDeath *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID == messADPtr->avatarID)
			{
				monsterList->Remove(tempMob);
				ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
				flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				sprintf(tempText,"%s is defeated!", tempMob->name);
				DisplayTextInfo(tempText);
				if (playerAvatar == tempMob)
					playerAvatar = dummyAvatar;
				delete tempMob;
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		break;

	case NWMESS_CHANGE_MAP:

		keysPressed &= ~(KEY_ACCEL);  // stop extra step

		DrawPleaseWaitScreen();

		messChangeMapPtr = (MessChangeMap *) messData;
		curMapSizeX = messChangeMapPtr->sizeX;
		curMapSizeY = messChangeMapPtr->sizeY;

		isEditingDungeon = isInSpecialDungeon = FALSE;
		cameraType = 0;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob != playerAvatar)
			{
				monsterList->Remove(tempMob);
				if (tempMob->cellSlot > -1)
				{
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				}
				delete tempMob;
				tempMob = (BBOMob *) monsterList->First();
			}
			else
			{
				tempMob = (BBOMob *) monsterList->Next();
			}
		}

		DrawPleaseWaitScreen();

		ClearStatics();

		DrawPleaseWaitScreen();

		if (messChangeMapPtr->oldType)
		{

		}

//		(SPACE_DUNGEON == curMapType) = TRUE;
		curMapType = messChangeMapPtr->newType;

		if (SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType)
		{
			int val = 0;
			if (SPACE_GUILD == curMapType)
			{
				bsMan.SetType(-4); // tower
				val = 1;
			}
			else
				bsMan.SetType(-2); // dungeon

			DrawPleaseWaitScreen();

			if (!dungeon)
			{
				dungeon = new DungeonClient(val,"dungeon");
			}
			else if (messChangeMapPtr->dungeonID != lastDungeonID)
			{
				delete dungeon;
				dungeon = new DungeonClient(val,"dungeon");
			}

			DrawPleaseWaitScreen();

			isEditingDungeon = messChangeMapPtr->flags & MESS_CHANGE_EDITING;
			isInSpecialDungeon = messChangeMapPtr->flags & MESS_CHANGE_TEMP;
			cameraType = 0;
//			cameraType = 2;  // TEST!!!!!!!!!!!!!!

			messDInfo.x = messDInfo.y = 0;
			lclient->SendMsg(sizeof(messDInfo),(void *)&messDInfo);
		}
		else if (SPACE_REALM == curMapType)
		{
			bsMan.SetType(-2); // dungeon
			DrawPleaseWaitScreen();

			if (!realm || messChangeMapPtr->realmID != lastRealmID)
			{
				aLog.Log("delete realm ptr ");
				aLog.Log((int) realm);
				aLog.Log("\n");
				SAFE_DELETE(realm);

				DrawPleaseWaitScreen();

				switch(messChangeMapPtr->realmID)
				{
				case REALM_ID_SPIRITS:
				default:
					aLog.Log("new realm object for spirits\n");
					realm = new RealmObject(messChangeMapPtr->realmID);
					aLog.Log("init()\n");
					realm->Init("dat\\realm-spirits-color.png", "dat\\realm-spirits-sky.png");
					aLog.Log("generate()\n");
					DrawPleaseWaitScreen();
					realm->Generate("dat\\realm-spirits-height.png", puma->m_pd3dDevice, 25);
					aLog.Log("loadTexture()\n");
					realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-spirits-sky.png" );
					aLog.Log("CreateSpiritStaticPositions()\n");
					DrawPleaseWaitScreen();
					realm->CreateSpiritStaticPositions(REALM_ID_SPIRITS);
					break;

				case REALM_ID_DEAD:
					aLog.Log("new realm object for the dead\n");
					realm = new RealmObject(messChangeMapPtr->realmID);
					realm->Init("dat\\realm-dead-color.png", "dat\\realm-dead-sky.png");
					DrawPleaseWaitScreen();
					realm->Generate("dat\\realm-dead-height.png", puma->m_pd3dDevice, 25);
					realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-dead-color.png" );
					DrawPleaseWaitScreen();
					realm->CreateSpiritStaticPositions(REALM_ID_DEAD);
					break;

				case REALM_ID_DRAGONS:
					aLog.Log("new realm object for dragons\n");
					realm = new RealmObject(messChangeMapPtr->realmID);
					realm->Init("dat\\realm-dragon-color.png", "dat\\realm-dragon-sky.png");
					DrawPleaseWaitScreen();
					realm->Generate("dat\\realm-dragons-height.png", puma->m_pd3dDevice, 25);
					realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-dragon-color.png" );
					DrawPleaseWaitScreen();
					realm->CreateSpiritStaticPositions(REALM_ID_DRAGONS);
					break;

				}
			}

			lastRealmID = messChangeMapPtr->realmID;

			lSquareMan->ro = realm;

			cameraType = 0;

		}
		else if (SPACE_LABYRINTH == curMapType)
		{
			bsMan.SetType(-3); // laby
			DrawPleaseWaitScreen();

			if (!labyrinth || messChangeMapPtr->realmID != lastRealmID)
			{
				aLog.Log("delete labyrinth ptr ");
				aLog.Log((int) labyrinth);
				aLog.Log("\n");
				SAFE_DELETE(labyrinth);

				DrawPleaseWaitScreen();

				switch(messChangeMapPtr->realmID)
				{
				case REALM_ID_LAB1:
				default:
					aLog.Log("new labyrinth object\n");
					labyrinth = new LabyrinthObject(messChangeMapPtr->realmID);
					aLog.Log("init()\n");
					labyrinth->Init("dat\\labyrinth-color.png");
					aLog.Log("generate()\n");
					DrawPleaseWaitScreen();
					labyrinth->Generate("dat\\labyrinth-height.png", puma->m_pd3dDevice, 25);
					aLog.Log("loadTexture()\n");
					labyrinth->LoadTexture( puma->m_pd3dDevice, "dat\\dunflr-marble.png" );
					aLog.Log("CreateStaticPositions()\n");
					DrawPleaseWaitScreen();
					labyrinth->CreateStaticPositions();
					break;

				case REALM_ID_LAB2:
					aLog.Log("new labyrinth object\n");
					labyrinth = new LabyrinthObject(messChangeMapPtr->realmID);
					aLog.Log("init()\n");
					labyrinth->Init("dat\\labyrinth-color.png", "dat\\labyrinth2-texture.png");
					aLog.Log("generate()\n");
					DrawPleaseWaitScreen();
					labyrinth->Generate("dat\\labyrinth2-height.png", puma->m_pd3dDevice, 25);
					aLog.Log("loadTexture()\n");
					labyrinth->LoadTexture( puma->m_pd3dDevice, "dat\\labyrinth2-texture.png" );
					aLog.Log("CreateStaticPositions()\n");
					DrawPleaseWaitScreen();
					labyrinth->CreateStaticPositions();
					break;
				}
			}

			lastRealmID = messChangeMapPtr->realmID;

			lSquareMan->ro = realm;

			cameraType = 0;

		}
/*
		if (SPACE_LABYRINTH == curMapType)
		{
			ZeroMemory( &light, sizeof(D3DLIGHT8) );
			light.Type      = D3DLIGHT_DIRECTIONAL;
			light.Diffuse.r  = 1.0f;
			light.Diffuse.g  = 1.0f;
			light.Diffuse.b  = 1.0f;
			light.Ambient.r  = 0.10f;
			light.Ambient.g  = 0.10f;
			light.Ambient.b  = 0.10f;
			vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
			D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
			light.Range      = 1000.0f;
			puma->m_pd3dDevice->SetLight( 0, &light );
		}
		else
		{
			ZeroMemory( &light, sizeof(D3DLIGHT8) );
			light.Type      = D3DLIGHT_DIRECTIONAL;
			light.Diffuse.r  = 1.0f;
			light.Diffuse.g  = 1.0f;
			light.Diffuse.b  = 1.0f;
			light.Ambient.r  = 1.0f;
			light.Ambient.g  = 1.0f;
			light.Ambient.b  = 1.0f;
			vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
			D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
			light.Range      = 1000.0f;
			puma->m_pd3dDevice->SetLight( 0, &light );
		}
		*/
		break;

		/*
	case NWMESS_LEAVING_DUNGEON:

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob != playerAvatar)
			{
				monsterList->Remove(tempMob);
				if (tempMob->cellSlot > -1)
				{
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				}
				delete tempMob;
				tempMob = (BBOMob *) monsterList->First();
			}
			else
			{
				tempMob = (BBOMob *) monsterList->Next();
			}
		}

		ClearStatics();
		break;
		 */
	case NWMESS_DUNGEON_INFO:
		messDungeonInfoPtr = (MessDungeonInfo *) messData;

		if (dungeon)
		{
			dungeon->ProcessMessage(messDungeonInfoPtr);
		}

		break;
		
	case NWMESS_DUNGEON_CHANGE:
		messDungeonChangePtr = (MessDungeonChange *) messData;

		if (dungeon)
		{
			dungeon->ProcessChangeMessage(messDungeonChangePtr);
		}

		break;
		
	case NWMESS_TIME_OF_DAY:
		messTimeOfDayPtr = (MessTimeOfDay *) messData;
		timeOfDay = messTimeOfDayPtr->value;
		break;
		
	case NWMESS_WEATHER_STATE:
		messWeatherStatePtr = (MessWeatherState *) messData;
		isLightning = messWeatherStatePtr->value;
		break;
		
	case NWMESS_AVATAR_HEALTH:
		messHealth      = (MessAvatarHealth *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID == messHealth->avatarID)
			{
				if (tempMob->screenHealth > messHealth->health)
				{
					avatarPain->PositionSound(tempMob->spacePoint.location, MAX_SOUND_DIST);
					avatarPain->Play();
				}
				tempMob->screenHealth    = messHealth->health;
				tempMob->screenMaxHealth = messHealth->healthMax;
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
		if (playerAvatar)
		{
			gGamePlayerHP    = playerAvatar->screenHealth;
			gGamePlayerMaxHP = playerAvatar->screenMaxHealth;
		}
	   break;

	case NWMESS_MONSTER_HEALTH:
//		DisplayTextInfo("monster health");
		messMH      = (MessMonsterHealth *) messData;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->mobID == messMH->mobID)
			{
				if (tempMob->screenHealth > messMH->health)
				{
					monsterPain[tempMob->monsterType]->PositionSound(
						                 tempMob->spacePoint.location, MAX_SOUND_DIST);
					monsterPain[tempMob->monsterType]->Play();
				}
				tempMob->screenHealth    = messMH->health;
				tempMob->screenMaxHealth = messMH->healthMax;
				tempMob->screenHealthTimer = 600;
				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_MAGIC_ATTACK:
		messMagAPtr      = (MessMagicAttack *) messData;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (messMagAPtr->avatarID != -1 && tempMob->avatarID == messMagAPtr->avatarID)
			{
				tempMob->magicAttackType = messMagAPtr->type;
				tempMob->magicAttackCounter = 0;
				tempMob->magicAttackColor = D3DCOLOR_ARGB(0, 255,255,255);
//					      staffColor[messMagAPtr->type][0],
//							staffColor[messMagAPtr->type][1],
//							staffColor[messMagAPtr->type][2]);

				tempMob = (BBOMob *) monsterList->Last();
			}
			else if (-1 == messMagAPtr->avatarID && tempMob->mobID == messMagAPtr->mobID)
			{
				tempMob->magicAttackType = messMagAPtr->type;
				tempMob->magicAttackCounter = 0;
				tempMob->magicAttackColor = D3DCOLOR_ARGB(0, 
					      staffColor[messMagAPtr->type][0],
							staffColor[messMagAPtr->type][1],
							staffColor[messMagAPtr->type][2]);

				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_CAVE_INFO:
		messCInfoPtr      = (MessCaveInfo *) messData;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->mobID == messCInfoPtr->mobID)
			{
				tempMob->towerType        = messCInfoPtr->type;
				tempMob->towerHasMistress = messCInfoPtr->hasMistress;
				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_CHEST_INFO:
		messChestPtr      = (MessChestInfo *) messData;
		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->mobID == messChestPtr->mobID)
			{
				tempMob->chestType        = messChestPtr->type;
				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_BOOT:
		POMSezQuit = TRUE;
		break;
/*
	case NWMESS_ENTERING_REALM:

		aLog.Append();
		aLog.Log("Entering realm...\n");

		messEnteringRealmPtr = (MessEnteringRealm *) messData;

		(SPACE_REALM == curMapType) = TRUE;

		aLog.Log("set BSMan to -2\n");
		bsMan.SetType(-2); // dungeon

		if (!realm || messEnteringRealmPtr->realmID != lastRealmID)
		{
			aLog.Log("delete realm ptr ");
			aLog.Log((int) realm);
			aLog.Log("\n");
			SAFE_DELETE(realm);

			switch(messEnteringRealmPtr->realmID)
			{
			case REALM_ID_SPIRITS:
			default:
				aLog.Log("new realm object for spirits\n");
				realm = new RealmObject(messEnteringRealmPtr->realmID);
				aLog.Log("init()\n");
				realm->Init("dat\\realm-spirits-color.png", "dat\\realm-spirits-sky.png");
				aLog.Log("generate()\n");
				realm->Generate("dat\\realm-spirits-height.png", puma->m_pd3dDevice, 25);
				aLog.Log("loadTexture()\n");
				realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-spirits-sky.png" );
				aLog.Log("CreateSpiritStaticPositions()\n");
				realm->CreateSpiritStaticPositions(REALM_ID_SPIRITS);
				break;

			case REALM_ID_DEAD:
				aLog.Log("new realm object for the dead\n");
				realm = new RealmObject(messEnteringRealmPtr->realmID);
				realm->Init("dat\\realm-dead-color.png", "dat\\realm-dead-sky.png");
				realm->Generate("dat\\realm-dead-height.png", puma->m_pd3dDevice, 25);
				realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-dead-color.png" );
				realm->CreateSpiritStaticPositions(REALM_ID_DEAD);
				break;

			case REALM_ID_DRAGONS:
				aLog.Log("new realm object for dragons\n");
				realm = new RealmObject(messEnteringRealmPtr->realmID);
				realm->Init("dat\\realm-dragon-color.png", "dat\\realm-dragon-sky.png");
				realm->Generate("dat\\realm-dragons-height.png", puma->m_pd3dDevice, 25);
				realm->LoadTexture( puma->m_pd3dDevice, "dat\\realm-dragon-color.png" );
				realm->CreateSpiritStaticPositions(REALM_ID_DRAGONS);
				break;

			}
		}

		lastRealmID = messEnteringRealmPtr->realmID;

//		lsMan->ro = realm;
		lSquareMan->ro = realm;

		cameraType = 0;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob != playerAvatar)
			{
				aLog.Log("** remove mob\n");
				monsterList->Remove(tempMob);
				if (tempMob->cellSlot > -1)
				{
					aLog.Log("    release slot\n");
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				}
				aLog.Log("   delete mob\n");
				delete tempMob;
				tempMob = (BBOMob *) monsterList->First();
			}
			else
			{
				tempMob = (BBOMob *) monsterList->Next();
			}
		}

		aLog.Log("clearStatics\n");
		ClearStatics();
		aLog.Finish();
		break;

	case NWMESS_LEAVING_REALM:

		(SPACE_REALM == curMapType) = FALSE;
		cameraType = 0;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob != playerAvatar)
			{
				monsterList->Remove(tempMob);
				if (tempMob->cellSlot > -1)
				{
					ground->ReleaseSlot(tempMob->cellX, tempMob->cellY, tempMob->cellSlot);
					flockSet->FreePoint(tempMob->flockPointIndex, tempMob);
				}
				delete tempMob;
				tempMob = (BBOMob *) monsterList->First();
			}
			else
			{
				tempMob = (BBOMob *) monsterList->Next();
			}
		}

		ClearStatics();
		break;
*/		
	case NWMESS_GENERIC_EFFECT:
		messGE      = (MessGenericEffect *) messData;
		found = FALSE;

		if (1 == messGE->type) // thief laugh
		{
			D3DXVECTOR3 pos;
			pos.x = ground->GetXForPoint(messGE->x);
			pos.z = ground->GetZForPoint(messGE->y);
			pos.y = HeightAtPoint(pos.x, pos.z);

			thiefSound->PositionSound(pos, MAX_SOUND_DIST);
			thiefSound->Play();
		}
		else
		{
			tempMob = (BBOMob *) monsterList->First();
			while (tempMob && !found)
			{
				if ((tempMob->avatarID != -1 && tempMob->avatarID == messGE->avatarID) ||
					 (tempMob->avatarID == -1 && tempMob->mobID    == messGE->mobID))
					found = TRUE;
				else
					tempMob = (BBOMob *) monsterList->Next();
			}

			tempMob2 = new BBOMob(0,"MOB");
			monsterList->Append(tempMob2);
			tempMob2->type = SMOB_PARTICLE_STREAM;
			sprintf(tempMob2->name,"");
			tempMob2->particleStreamAge = timeGetTime() + messGE->timeLen * 1000;
			if (0 == messGE->timeLen)
				tempMob2->particleStreamAge = 0;
			tempMob2->particleStreamType = messGE->type;
			tempMob2->particleStreamColor = D3DCOLOR_ARGB(55, messGE->r, messGE->g, messGE->b);

			tempMob2->cellX = messGE->x;
			tempMob2->cellY = messGE->y;

			if (found)
				tempMob2->spacePoint = tempMob->spacePoint;
			else
			{
				if ((SPACE_REALM == curMapType))
				{
					tempMob2->spacePoint.location.x = realm->GetXForPoint(tempMob2->cellX) + realm->CellSize()/2;
					tempMob2->spacePoint.location.z = realm->GetZForPoint(tempMob2->cellY) + realm->CellSize()/2;
				}
				else
				{
					tempMob2->spacePoint.location.x = ground->GetXForPoint(tempMob2->cellX) + ground->CellSize()/2;
					tempMob2->spacePoint.location.z = ground->GetZForPoint(tempMob2->cellY) + ground->CellSize()/2;
				}
				tempMob2->spacePoint.angle = 0;
				tempMob2->spacePoint.azimuth = 0;
				tempMob2->spacePoint.location.y = HeightAtPoint(tempMob2->spacePoint.location.x, tempMob2->spacePoint.location.z) + 1;
			}
		}
	   break;

	case NWMESS_CHANT:
		messChant = (MessChant *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID == messChant->avatarID)
			{
				tempMob->chantAge = timeGetTime();
				tempMob->chantType = CHANT_REALM_SPIRITS;
				tempMob->chantColor = D3DCOLOR_ARGB(55, messChant->r, messChant->g, messChant->b);

				if (0 == messChant->r && 0 == messChant->g && 0 == messChant->b)
					tempMob->chantType = -1;

				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_PET:
		messPet = (MessPet *) messData;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID == messPet->avatarID)
			{
				tempMob->pet[messPet->which].Init(messPet);
				if (0 == messPet->name[0] && tempMob->avatarID == playerAvatar->avatarID)
					needToName = TRUE;

				tempMob = (BBOMob *) monsterList->Last();
			}
			tempMob = (BBOMob *) monsterList->Next();
		}
	   break;

	case NWMESS_PET_ATTACK:

		messPAPtr = (MessPetAttack *) messData;
		tempMob2 = tempMob3 = NULL;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID > -1)
			{
				if (tempMob->avatarID == messPAPtr->avatarID)
				{
					if (tempMob->pet[messPAPtr->which].petDragonAnims[0])
					{
//						tempMob->pet[messPAPtr->which].stateCounter = 0;
						tempMob->pet[messPAPtr->which].nextState = 1;
						tempMob2 = tempMob;
					}
				}
			}
			else if (tempMob->mobID == messPAPtr->mobID)
				tempMob3 = tempMob;
			tempMob = (BBOMob *) monsterList->Next();
		}

		if (tempMob2 && tempMob3)
		{
			if (playerInfoFlags & INFO_FLAGS_HITS)
			{
				sprintf(tempText,"%s breathes on the %s for %d damage.",
				        tempMob2->pet[messPAPtr->which].petName, tempMob3->name, messPAPtr->damage);
				DisplayTextInfo(tempText);
			}
			// turn to face the opponent
			fp  = flockSet->GetPoint(tempMob2->flockPointIndex, tempMob2);
			FlockPoint *fp2 = flockSet->GetPoint(tempMob3->flockPointIndex, tempMob3);
			fp->targetAngle = TurnTowardsPoint(fp->x, fp->y, fp2->x, fp2->y, 0,0); 

			tempMob2->pet[messPAPtr->which].petAttackEnd = tempMob3->spacePoint.location;
			tempMob2->pet[messPAPtr->which].petAttackEnd.y += 0.7f;

			tempMob2->pet[messPAPtr->which].petAttackType = messPAPtr->type;

		}

		break;

	case NWMESS_INFO_FLAGS:
		infoFlagsPtr = (MessInfoFlags *) messData;
		playerInfoFlags = infoFlagsPtr->flags;
		break;

	case NWMESS_AVATAR_GUILD_NAME:
		stream	= new Chronos::BStream(sizeof(AvatarGuildName) + 65);

		stream->write(messData, dataSize);
		stream->setPosition(0);

		*stream >> tempUC;
		assert( NWMESS_AVATAR_GUILD_NAME == tempUC);

		*stream >> tempInt;

		tempMob = (BBOMob *) monsterList->First();
		while (tempMob)
		{
			if (tempMob->avatarID == tempInt)
			{
				for (int k = 0; k < 65; ++k)
				{
					*stream >> tempMob->guildName[k];
					if (0 == tempMob->guildName[k])
						k = 1000;
				}
			}
			tempMob = (BBOMob *) monsterList->Next();
		}

		delete stream;
		break;

	case NWMESS_AVATAR_GUILD_EDIT:
		avGuildEditPtr = (AvatarGuildEdit *) messData;
		if (playerAvatar->avatarID == avGuildEditPtr->avatarID)
		{
			isEditingDungeon = TRUE;
		}
		break;

	case NWMESS_EXPLOSION:
//		sprintf(tempText,"Boom");
//		DisplayTextInfo(tempText);

		explosionPtr = (MessExplosion *) messData;

			D3DXVECTOR3 pos;
			pos.x = explosionPtr->x * 10 + 5;
			pos.z = explosionPtr->y * 10 + 5;
			pos.y = HeightAtPoint(pos.x, pos.z) + 0.3f;
			if (explosionPtr->avatarID)
			{
				tempMob = (BBOMob *) monsterList->First();
				while (tempMob)
				{
					if (tempMob->avatarID > -1)
					{
						if (tempMob->avatarID == explosionPtr->avatarID)
						{
							pos = tempMob->spacePoint.location;
							pos.y += 0.6f;
							tempMob = (BBOMob *) monsterList->Last();
						}
					}
					tempMob = (BBOMob *) monsterList->Next();
				}

			}

			float bombDist = Distance(playerAvatar->cellX, playerAvatar->cellY, 
					          explosionPtr->x, explosionPtr->y);

			int fullBang = FALSE;
			if (explosionPtr->r || explosionPtr->g || explosionPtr->b)
			{
				fullBang = TRUE;
				if (bombDist < 12)
				{
					bombFireParticle->SetEmissionPoint(pos);
					bombFireParticle->Emit(50, D3DCOLOR_ARGB(155, 255,255,0),
						      /*explosionPtr->r, explosionPtr->g, explosionPtr->b),*/ 
								explosionPtr->size/100);
				}
			}
			if (bombDist < 12)
			{
				smokeParticle->SetEmissionPoint(pos);
//				if ((int)explosionPtr->size % 2)
//					smokeParticle->Emit(50, D3DCOLOR_ARGB(155, 
//					      explosionPtr->r, explosionPtr->g, explosionPtr->b), explosionPtr->size/30);
//				else
					smokeParticle->Emit(50, D3DCOLOR_ARGB(155, 
					      explosionPtr->r, explosionPtr->g, explosionPtr->b), explosionPtr->size/30);


				bombShakeVal = explosionPtr->size / (1 + bombDist);
			}

			// sound
			if (explosionPtr->size > 1000 && fullBang)
			{
				if (playerAvatar && 	bombDist > 10)
				{
					bombSound[6]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[6]->Play();
				}
				else
				{
					bombSound[5]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[5]->Play();
				}
			}
			else if (explosionPtr->size > 100 && fullBang)
			{
				if (playerAvatar && bombDist > 7)
				{
					bombSound[4]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[4]->Play();
				}
				else
				{
					bombSound[3]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[3]->Play();
				}
			}
			else if (explosionPtr->size > 10 && fullBang)
			{
				if (playerAvatar && bombDist > 4)
				{
					bombSound[2]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[2]->Play();
				}
				else
				{
					bombSound[1]->PositionSound(pos, MAX_SOUND_DIST);
					bombSound[1]->Play();
				}
			}
			else 
			{
				bombSound[0]->PositionSound(pos, MAX_SOUND_DIST);
				bombSound[0]->Play();
			}
		break;


	}

//	DebugOutput("En Handle mess\n");

}

//***************************************************************
void BBOClient::SetupObjectOrientation(float yaw, float x, float y, float z)
{
   D3DXMATRIX matWorld, mat2;

   D3DXMatrixIdentity( &matWorld );
   D3DXMatrixRotationY(&matWorld, (float) yaw);
   matWorld._41 = x;
   matWorld._42 = y;
   matWorld._43 = z;

	matWorldValue = matWorld;

   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}

//***************************************************************
void BBOClient::SetupBladeOrientation(PumaAnimFrame *frame, float yaw, float x, float y, float z)
{
	
   D3DXMATRIX MatWorld, MatLocal, MatTemp; 
   D3DXMATRIX MatRot, MatLYaw, MatLPitch, MatLRoll;
 
   // Using the left-to-right order of matrix concatenation,
   // apply the translation to the object's world position
   // before applying the rotations.
   D3DXMatrixTranslation(&MatWorld, x,y,z);
   D3DXMatrixIdentity(&MatRot);

	D3DXMatrixTranslation( &MatLocal, frame->bladePosition.x, frame->bladePosition.y, frame->bladePosition.z);

   D3DXMatrixRotationY(&MatLYaw, atan2(frame->bladeDirection.x, frame->bladeDirection.z));           // Yaw

   D3DXMatrixRotationX(&MatLPitch, atan2( -1 * frame->bladeDirection.y, 
		Distance(0,0, frame->bladeDirection.x, frame->bladeDirection.z) ));           // Pitch

   D3DXMatrixRotationZ(&MatLRoll, D3DX_PI/-2); 

   D3DXMatrixMultiply(&MatLPitch, &MatLRoll, &MatLPitch);

   D3DXMatrixMultiply(&MatLYaw, &MatLPitch, &MatLYaw);

   D3DXMatrixMultiply(&MatLocal, &MatLYaw, &MatLocal);

   D3DXMatrixRotationY(&MatTemp, yaw);           // Yaw
   D3DXMatrixMultiply(&MatRot, &MatRot, &MatTemp);
   // Apply the rotation matrices to complete the world matrix.
   D3DXMatrixMultiply(&MatRot, &MatLocal, &MatRot);

   D3DXMatrixMultiply(&MatWorld, &MatRot, &MatWorld);

	matBlade = MatWorld;

   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &MatWorld );

/*


	D3DXMATRIX matTrans, matRotateX, matRotateY, matRotateZ, matTemp1, matTemp2;
   D3DXMATRIX matWorld, mat2;

	// rotate sword to correct orientation
   D3DXMatrixRotationY(&matWorld, 0);

	// move to hand (local space)
   D3DXMatrixIdentity( &matTrans );
	D3DXMatrixTranslation( &matTrans, frame->bladePosition.x, frame->bladePosition.y, frame->bladePosition.z);
//	D3DXMatrixTranslation( &matTrans, 0,0,0);

	// rotate same as body
   D3DXMatrixIdentity( &matWorld );
   D3DXMatrixRotationY(&matWorld, (float) yaw);

	// move same as body
   D3DXMatrixIdentity( &matTemp1 );
	D3DXMatrixTranslation( &matTemp1, x,y,z);

	// sew them all together
	D3DXMatrixMultiply( &matTemp2, &matWorld, &matTrans );
	D3DXMatrixMultiply( &matWorld, &matTemp1, &matTemp2 );
  
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matTemp1 );
  */
}

//***************************************************************
void BBOClient::DrawHalo(PumaAnimFrame *frame, float yaw, float x, float y, float z)
{
	return; // DISABLED!!!!!
	
   D3DXMATRIX MatWorld, MatLocal, MatTemp; 
   D3DXMATRIX MatRot, MatLYaw, MatLPitch, MatLRoll;
 
   // Using the left-to-right order of matrix concatenation,
   // apply the translation to the object's world position
   // before applying the rotations.
   D3DXMatrixTranslation(&MatWorld, x,y,z);
   D3DXMatrixIdentity(&MatRot);

	D3DXMatrixTranslation( &MatLocal, frame->extraPosition[0].x, 
		                               frame->extraPosition[0].y, 
												 frame->extraPosition[0].z);

   D3DXMatrixRotationY(&MatRot, -yaw);           // Yaw
   // Apply the rotation matrices to complete the world matrix.
   D3DXMatrixMultiply(&MatRot, &MatLocal, &MatRot);

   D3DXMatrixMultiply(&MatWorld, &MatRot, &MatWorld);

	D3DXVECTOR3 pOut, pV;
	pV.x = 0; 
	pV.y = 0;
	pV.z = 0;
//	pV.z = -1 * pos / 5.0f;

	D3DXVec3TransformCoord(&pOut, &pV, &MatWorld);
	haloGlowSet->AddAGlow(pOut, D3DCOLOR_ARGB(55, 255, 255, 255));

}

//***************************************************************
void BBOClient::MaintainStaticObjects(int posX, int posY)
{
	if ((SPACE_DUNGEON == curMapType))
		return;

	if ((SPACE_GUILD == curMapType))
		return;

	if (SPACE_REALM != curMapType && SPACE_LABYRINTH != curMapType)
	{

		for (int y = 0; y < ground->sizeH; ++y)
		{
			for (int x = 0; x < ground->sizeW; ++x)
			{
				if (staticMap[y*ground->sizeW+x])
				{
					if (abs(y - posY) > 4 || abs(x - posX) > 4)
					{
						// get rid of existing static objects
						staticMap[y*ground->sizeW+x] = FALSE;
					}
				}
				else
				{
					if (abs(y - posY) <= 4 && abs(x - posX) <= 4)
					{
						staticMap[y*ground->sizeW+x] = TRUE;

						// add static objects in this square
						LocationSlots *slot = &(ground->slotArray[y*ground->sizeW+x]);
						for (int index = 0; index < NUM_OF_SLOTS_PER_SPACE; ++index)
						{
							if (slot->used[index] > SLOT_MOB)
							{
								BBOStatic *sob = new BBOStatic(slot->used[index]-SLOT_DESERT_TREE1, "STATIC_OBJECT");
								staticList->Append(sob);
								float slotX, slotY, slotAngle;
								ground->GetSlotPosition(index,slotX, slotY, slotAngle);
								sob->cellSlot = index;
								sob->cellX = x;
								sob->cellY = y;
								sob->spacePoint.location.x = ground->GetXForPoint(sob->cellX) + slotX;
								sob->spacePoint.location.z = ground->GetZForPoint(sob->cellY) + slotY;
								
								sob->spacePoint.angle = rnd(0,D3DX_PI);
								sob->spacePoint.azimuth = 0;
								sob->spacePoint.location.y = HeightAtPoint(sob->spacePoint.location.x, sob->spacePoint.location.z);

								sob->flockPointIndex = flockSet->MakePoint(sob);
								FlockPoint *fp = flockSet->GetPoint(sob->flockPointIndex, sob);

								fp->angle = fp->targetAngle = sob->spacePoint.angle;
								fp->maxSpeed = 0.0f;
								fp->maxTurn = 0;
								fp->x = fp->targetX = sob->spacePoint.location.x;
								fp->y = fp->targetY = sob->spacePoint.location.z;
								fp->size = FPNORMALSIZE;
								if (SLOT_CABIN == slot->used[index] || SLOT_CABIN2 == slot->used[index])
									fp->size = 1.1f;
								fp->targetAngle = slotAngle;
							}
						}

						if (ground->flowerArray[y*ground->sizeW+x].type)
						{
							BBOStatic *sob = new BBOStatic(
								16 + ground->flowerArray[y*ground->sizeW+x].type, "STATIC_OBJECT");
							staticList->Append(sob);
							sob->cellX = x;
							sob->cellY = y;
							sob->spacePoint.location.x = ground->GetXForPoint(sob->cellX) + 
								ground->flowerArray[y*ground->sizeW+x].x / 256.0 * ground->CellSize();
							sob->spacePoint.location.z = ground->GetZForPoint(sob->cellY) +
								ground->flowerArray[y*ground->sizeW+x].y / 256.0 * ground->CellSize();
							sob->spacePoint.location.y = HeightAtPoint(
								     sob->spacePoint.location.x, sob->spacePoint.location.z);

							sob->flowerLocListSize = 
								ground->flowerArray[y*ground->sizeW+x].amount;

							sob->flowerLocList = new D3DXVECTOR3[sob->flowerLocListSize];
							for (int t = 0; t < sob->flowerLocListSize; ++t)
							{
								sob->flowerLocList[t].x = sob->spacePoint.location.x + rnd(-5,5);
								sob->flowerLocList[t].z = sob->spacePoint.location.z + rnd(-5,5);
								sob->flowerLocList[t].y = HeightAtPoint(
								     sob->flowerLocList[t].x, sob->flowerLocList[t].z);
							}
						}
					}
				}
			}
		}
	}
	else if (SPACE_REALM == curMapType)
	{
		for (int y = 0; y < realm->sizeH; ++y)
		{
			for (int x = 0; x < realm->sizeW; ++x)
			{
				if (staticMap[y*realm->sizeW+x])
				{
					if (abs(y - posY) > 4 || abs(x - posX) > 4)
					{
						// get rid of existing static objects
						staticMap[y*realm->sizeW+x] = FALSE;
					}
				}
				else
				{
					if (abs(y - posY) <= 4 && abs(x - posX) <= 4)
					{
						// add static objects in this square
						staticMap[y*realm->sizeW+x] = TRUE;
						LocationSlots *slot = &(realm->slotArray[y*realm->sizeW+x]);
						for (int index = 0; index < NUM_OF_SLOTS_PER_SPACE; ++index)
						{
							if (slot->used[index] > SLOT_MOB)
							{
								BBOStatic *sob = new BBOStatic(slot->used[index]-SLOT_DESERT_TREE1, "STATIC_OBJECT");
								staticList->Append(sob);
								float slotX, slotY, slotAngle;
								realm->GetSlotPosition(index,slotX, slotY, slotAngle);
								sob->cellSlot = index;
								sob->cellX = x;
								sob->cellY = y;
								sob->spacePoint.location.x = realm->GetXForPoint(sob->cellX) + slotX;
								sob->spacePoint.location.z = realm->GetZForPoint(sob->cellY) + slotY;
								
								sob->spacePoint.angle = rnd(0,D3DX_PI);
								sob->spacePoint.azimuth = 0;
								sob->spacePoint.location.y = HeightAtPoint(sob->spacePoint.location.x, sob->spacePoint.location.z);

								sob->flockPointIndex = flockSet->MakePoint(sob);
								FlockPoint *fp = flockSet->GetPoint(sob->flockPointIndex, sob);

								fp->angle = fp->targetAngle = sob->spacePoint.angle;
								fp->maxSpeed = 0.0f;
								fp->maxTurn = 0;
								fp->x = fp->targetX = sob->spacePoint.location.x;
								fp->y = fp->targetY = sob->spacePoint.location.z;
								fp->size = FPNORMALSIZE;
								if (SLOT_CABIN <= slot->used[index])
									fp->size = 1.1f;
								fp->targetAngle = slotAngle;
							}
						}

					}
				}

			}
		}
	}
	else if (SPACE_LABYRINTH == curMapType)
	{
		for (int y = 0; y < labyrinth->sizeH; ++y)
		{
			for (int x = 0; x < labyrinth->sizeW; ++x)
			{
				if (staticMap[y*labyrinth->sizeW+x])
				{
					if (abs(y - posY) > 4 || abs(x - posX) > 4)
					{
						// get rid of existing static objects
						staticMap[y*labyrinth->sizeW+x] = FALSE;
					}
				}
				else
				{
					if (abs(y - posY) <= 4 && abs(x - posX) <= 4)
					{
						// add static objects in this square
						staticMap[y*labyrinth->sizeW+x] = TRUE;
						LocationSlots *slot = &(labyrinth->slotArray[y*labyrinth->sizeW+x]);
						for (int index = 0; index < NUM_OF_SLOTS_PER_SPACE; ++index)
						{
							if (slot->used[index] > SLOT_MOB)
							{
								BBOStatic *sob = new BBOStatic(slot->used[index]-SLOT_DESERT_TREE1, "STATIC_OBJECT");
								staticList->Append(sob);
								float slotX, slotY, slotAngle;
								labyrinth->GetSlotPosition(index,slotX, slotY, slotAngle);
								sob->cellSlot = index;
								sob->cellX = x;
								sob->cellY = y;
								sob->spacePoint.location.x = labyrinth->GetXForPoint(sob->cellX) + slotX;
								sob->spacePoint.location.z = labyrinth->GetZForPoint(sob->cellY) + slotY;
								
								sob->spacePoint.angle = rnd(0,D3DX_PI);
								sob->spacePoint.azimuth = 0;
								sob->spacePoint.location.y = HeightAtPoint(sob->spacePoint.location.x, sob->spacePoint.location.z);

								sob->flockPointIndex = flockSet->MakePoint(sob);
								FlockPoint *fp = flockSet->GetPoint(sob->flockPointIndex, sob);

								fp->angle = fp->targetAngle = sob->spacePoint.angle;
								fp->maxSpeed = 0.0f;
								fp->maxTurn = 0;
								fp->x = fp->targetX = sob->spacePoint.location.x;
								fp->y = fp->targetY = sob->spacePoint.location.z;
								fp->size = FPNORMALSIZE;
								if (SLOT_CABIN <= slot->used[index])
									fp->size = 1.1f;
								fp->targetAngle = slotAngle;
							}
						}

					}
				}

			}
		}
	}
}



//***************************************************************
void BBOClient::SetupOverlayLines(int posX, int posY)
{
	DWORD color = 0x80ffffff;

	float yOffset = 0.01f;

	if (!lineOverlayPtr)
	{
	   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( 9*4*2*sizeof(PUMALINESTRUCT),
		                          0, PUMA_LINEDESC,
			                       D3DPOOL_DEFAULT, &lineOverlayPtr ) ) )
		   return;
	}

	int mapSizeW = MAP_SIZE_WIDTH;
	if ((SPACE_REALM == curMapType))
		mapSizeW = realm->sizeW;
	else if ((SPACE_LABYRINTH == curMapType))
		mapSizeW = labyrinth->sizeW;

	int mapSizeH = MAP_SIZE_HEIGHT;
	if ((SPACE_REALM == curMapType))
		mapSizeH = realm->sizeH;
	else if ((SPACE_LABYRINTH == curMapType))
		mapSizeH = labyrinth->sizeH;

	// Prepare to fill the line vertex buffer, by locking it.
	PUMALINESTRUCT *pVertices;
	if( FAILED( lineOverlayPtr->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	int curPoint, curSquare = 0;

	for (int i = 0; i < 3 * 2 * 5; ++i)
		clickVerts[i].x = clickVerts[i].y = clickVerts[i].z = 0;

	for (int y = posY - 1; y <= posY + 1; ++y)
	{
		for (int x = posX - 1; x <= posX + 1; ++x)
		{
			if (y >= 0 && y+1 < mapSizeH && x >= 0 && x+1 < mapSizeW)
			{
				curPoint = curSquare*4*2;
				//line 1
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x);
				pVertices[curPoint].position.z = ground->GetZForPoint(y);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x);
				pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				//line 2
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x);
				pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
				pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				//line 3
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
				pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
				pVertices[curPoint].position.z = ground->GetZForPoint(y);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				//line 4
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
				pVertices[curPoint].position.z = ground->GetZForPoint(y);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;
				++curPoint;
				pVertices[curPoint].color = color;
				pVertices[curPoint].position.x = ground->GetXForPoint(x);
				pVertices[curPoint].position.z = ground->GetZForPoint(y);
				pVertices[curPoint].position.y = 
					  HeightAtPoint(pVertices[curPoint].position.x,
							   					pVertices[curPoint].position.z) + yOffset;

				if (y < posY)
				{
					if (x == posX)
						SetClickVertQuad(1, x, y);
				}
				else if (y > posY)
				{
					if (x == posX)
						SetClickVertQuad(3, x, y);
				}
				else
				{
					if (x < posX)
					{
						SetClickVertQuad(2, x, y);
					}
					else if (x > posX)
					{
						SetClickVertQuad(4, x, y);
					}
					else
						SetClickVertQuad(0, x, y);
				}
			}
			else
			{
				for (curPoint = curSquare*4*2; curPoint < curSquare*4*2 + 8; ++curPoint)
				{
					pVertices[curPoint].color = 0x00000000;
					pVertices[curPoint].position.x = 0;
					pVertices[curPoint].position.z = 0;
					pVertices[curPoint].position.y = 0;
				}
			}
			++curSquare;
		}
	}

   lineOverlayPtr->Unlock();


}


//***************************************************************
void BBOClient::SetClickVertQuad(int quadIndex, int cellX, int cellY)
{
	// first tri
	clickVerts[(quadIndex*2+0)*3+0].x = ground->GetXForPoint(cellX);
	clickVerts[(quadIndex*2+0)*3+0].z = ground->GetZForPoint(cellY);
	clickVerts[(quadIndex*2+0)*3+0].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+0)*3+0].x, 
			 clickVerts[(quadIndex*2+0)*3+0].z);

	clickVerts[(quadIndex*2+0)*3+1].x = ground->GetXForPoint(cellX+1);
	clickVerts[(quadIndex*2+0)*3+1].z = ground->GetZForPoint(cellY);
	clickVerts[(quadIndex*2+0)*3+1].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+0)*3+1].x, 
			 clickVerts[(quadIndex*2+0)*3+1].z);

	clickVerts[(quadIndex*2+0)*3+2].x = ground->GetXForPoint(cellX);
	clickVerts[(quadIndex*2+0)*3+2].z = ground->GetZForPoint(cellY+1);
	clickVerts[(quadIndex*2+0)*3+2].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+0)*3+2].x, 
			 clickVerts[(quadIndex*2+0)*3+2].z);

	// second tri
	clickVerts[(quadIndex*2+1)*3+0].x = ground->GetXForPoint(cellX+1);
	clickVerts[(quadIndex*2+1)*3+0].z = ground->GetZForPoint(cellY+1);
	clickVerts[(quadIndex*2+1)*3+0].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+1)*3+0].x, 
			 clickVerts[(quadIndex*2+1)*3+0].z);

	clickVerts[(quadIndex*2+1)*3+1].x = ground->GetXForPoint(cellX);
	clickVerts[(quadIndex*2+1)*3+1].z = ground->GetZForPoint(cellY+1);
	clickVerts[(quadIndex*2+1)*3+1].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+1)*3+1].x, 
			 clickVerts[(quadIndex*2+1)*3+1].z);

	clickVerts[(quadIndex*2+1)*3+2].x = ground->GetXForPoint(cellX+1);
	clickVerts[(quadIndex*2+1)*3+2].z = ground->GetZForPoint(cellY);
	clickVerts[(quadIndex*2+1)*3+2].y = HeightAtPoint(
		    clickVerts[(quadIndex*2+1)*3+2].x, 
			 clickVerts[(quadIndex*2+1)*3+2].z);

}


//***************************************************************
void BBOClient::DrawOverlayLines(void)
{
   D3DXMATRIX matWorld;
   // Render the line buffer contents
   D3DXMatrixIdentity( &matWorld );
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

//	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

   puma->m_pd3dDevice->SetStreamSource( 0, lineOverlayPtr, sizeof(PUMALINESTRUCT) );
   puma->m_pd3dDevice->SetVertexShader( PUMA_LINEDESC );
   puma->m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0, 9*4 );

//	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}

//***************************************************************
void BBOClient::SetupOverlayGroundLines(int posX, int posY)
{
	DWORD color = 0x80ffffff;

	float yOffset = 0.01f;

	if (!lineOverlayGroundPtr)
	{
	   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( 9*4*2*4*sizeof(PUMALINESTRUCT),
		                          0, PUMA_LINEDESC,
			                       D3DPOOL_DEFAULT, &lineOverlayGroundPtr ) ) )
		   return;
	}

	int mapSizeW = MAP_SIZE_WIDTH;
	if ((SPACE_REALM == curMapType))
		mapSizeW = realm->sizeW;

	int mapSizeH = MAP_SIZE_HEIGHT;
	if ((SPACE_REALM == curMapType))
		mapSizeH = realm->sizeH;

	// Prepare to fill the line vertex buffer, by locking it.
	PUMALINESTRUCT *pVertices;
	if( FAILED( lineOverlayGroundPtr->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	int curPoint, curSquare = 0;

	// clear click verts
	for (int i = 0; i < 3 * 2 * 5; ++i)
		clickVerts[i].x = clickVerts[i].y = clickVerts[i].z = 0;

	for (int y = (posY - 1); y <= (posY + 1); ++y)
	{
		for (int x = (posX - 1); x <= (posX + 1); ++x)
		{
			if (y >= 0 && y+1 < mapSizeH && x >= 0 && x+1 < mapSizeW)
			{
				curPoint = curSquare*4*2*4;
				int sub;

				for (sub = 0; sub < 4; ++sub)
				{
					//line 1
					pVertices[curPoint].color = color;
					pVertices[curPoint].position.x = ground->GetXForPoint(x) + sub * 10/4.0f;
					pVertices[curPoint].position.z = ground->GetZForPoint(y);
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4+sub,y*4) + yOffset;
					++curPoint;

					pVertices[curPoint].position.x = ground->GetXForPoint(x) + (sub+1) * 10/4.0f;
					pVertices[curPoint].position.z = ground->GetZForPoint(y);
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4+(sub+1),y*4) + yOffset;
					++curPoint;
				}
				for (sub = 0; sub < 4; ++sub)
				{
					//line 2
					pVertices[curPoint].color = color;
					pVertices[curPoint].position.x = ground->GetXForPoint(x) + sub * 10/4.0f;
					pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4+sub,(y+1)*4) + yOffset;
					++curPoint;

					pVertices[curPoint].position.x = ground->GetXForPoint(x) + (sub+1) * 10/4.0f;
					pVertices[curPoint].position.z = ground->GetZForPoint(y+1);
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4+(sub+1),(y+1)*4) + yOffset;
					++curPoint;
				}
				for (sub = 0; sub < 4; ++sub)
				{
					//line 3
					pVertices[curPoint].color = color;
					pVertices[curPoint].position.x = ground->GetXForPoint(x);
					pVertices[curPoint].position.z = ground->GetZForPoint(y) + sub * 10/4.0f;
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4,y*4+sub) + yOffset;
					++curPoint;

					pVertices[curPoint].position.x = ground->GetXForPoint(x);
					pVertices[curPoint].position.z = ground->GetZForPoint(y) + (sub+1) * 10/4.0f;
					pVertices[curPoint].position.y = ground->GetYForTruePoint(x*4,y*4+(sub+1)) + yOffset;
					++curPoint;
				}
				for (sub = 0; sub < 4; ++sub)
				{
					//line 3
					pVertices[curPoint].color = color;
					pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
					pVertices[curPoint].position.z = ground->GetZForPoint(y) + sub * 10/4.0f;
					pVertices[curPoint].position.y = ground->GetYForTruePoint((x+1)*4,y*4+sub) + yOffset;
					++curPoint;

					pVertices[curPoint].position.x = ground->GetXForPoint(x+1);
					pVertices[curPoint].position.z = ground->GetZForPoint(y) + (sub+1) * 10/4.0f;
					pVertices[curPoint].position.y = ground->GetYForTruePoint((x+1)*4,y*4+(sub+1)) + yOffset;
					++curPoint;
				}

				if (y < posY)
				{
					if (x == posX)
						SetClickVertQuad(1, x, y);
				}
				else if (y > posY)
				{
					if (x == posX)
						SetClickVertQuad(3, x, y);
				}
				else
				{
					if (x < posX)
					{
						SetClickVertQuad(2, x, y);
					}
					else if (x > posX)
					{
						SetClickVertQuad(4, x, y);
					}
					else
						SetClickVertQuad(0, x, y);
				}
			}
			else
			{
				for (curPoint = curSquare*4*2*4; curPoint < curSquare*4*2 + 8; ++curPoint)
				{
					pVertices[curPoint].color = 0x00000000;
					pVertices[curPoint].position.x = 0;
					pVertices[curPoint].position.z = 0;
					pVertices[curPoint].position.y = 0;
				}
			}
			++curSquare;
		}
	}

   lineOverlayGroundPtr->Unlock();


}

//***************************************************************
void BBOClient::DrawOverlayGroundLines(void)
{
   D3DXMATRIX matWorld;
   // Render the line buffer contents
   D3DXMatrixIdentity( &matWorld );
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

//	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );

   puma->m_pd3dDevice->SetStreamSource( 0, lineOverlayGroundPtr, sizeof(PUMALINESTRUCT) );
   puma->m_pd3dDevice->SetVertexShader( PUMA_LINEDESC );
   puma->m_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0, 9*4*4 );

//	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

}


//***************************************************************
float BBOClient::HeightAtPoint(float x, float z)
{
	if ((SPACE_DUNGEON == curMapType || SPACE_GUILD == curMapType))
		return 0;
	else if ((SPACE_REALM == curMapType))
		return realm->HeightAtPoint(x,z,NULL);
	else if ((SPACE_LABYRINTH == curMapType))
		return labyrinth->HeightAtPoint(x,z,NULL);
	else
		return ground->HeightAtPoint(x,z,NULL);
}

//***************************************************************
void BBOClient::HandleSpecialAppearance(BBOMob *mob, unsigned short effect)
{
	D3DXCOLOR teleportParticleColor  = D3DCOLOR_ARGB(55, 255, 0, 255);
	D3DXCOLOR teleAwayParticleColor  = D3DCOLOR_ARGB(55, 255, 0, 122);
	D3DXCOLOR dungeonParticleColor   = D3DCOLOR_ARGB(55, 255, 255, 0);
	D3DXCOLOR deathParticleColor     = D3DCOLOR_ARGB(55, 100, 0, 0);
	D3DXCOLOR resParticleColor       = D3DCOLOR_ARGB(55, 0, 100, 0);
	D3DXCOLOR adminTeleParticleColor = D3DCOLOR_ARGB(55, 255, 255, 255);
	D3DXCOLOR enterParticleColor     = D3DCOLOR_ARGB(55, 0, 0, 255);
	D3DXCOLOR leaveParticleColor     = D3DCOLOR_ARGB(55, 0, 0, 255);
	D3DXCOLOR healParticleColor      = D3DCOLOR_ARGB(55, 0, 255, 100);

	int num = 60;

	D3DXVECTOR3 pos = mob->spacePoint.location;
	pos.y += 0.6f;
	eventParticle->SetEmissionPoint(pos);
	switch(effect)
	{
	case SPECIAL_APP_TELEPORT:
		eventParticle->Emit(num, teleportParticleColor);
		break;
	case SPECIAL_APP_ADMIN_TELEPORT:
		eventParticle->Emit(num, adminTeleParticleColor);
		break;
	case SPECIAL_APP_TELEPORT_AWAY:
		eventParticle->Emit(num, teleAwayParticleColor);
		if (playerAvatar == mob)
			DrawPleaseWaitScreen();
		break;
	case SPECIAL_APP_DEATH:
		eventParticle->Emit(num, deathParticleColor);
		if (playerAvatar == mob)
			DrawPleaseWaitScreen();
		break;
	case SPECIAL_APP_RES:
		eventParticle->Emit(num, resParticleColor);
		break;
	case SPECIAL_APP_DUNGEON:
		eventParticle->Emit(num, dungeonParticleColor);
		break;
	case SPECIAL_APP_ENTER_GAME:
		eventParticle->Emit(num, enterParticleColor);
		break;
	case SPECIAL_APP_LEAVE_GAME:
		eventParticle->Emit(num, leaveParticleColor);
		break;
	case SPECIAL_APP_HEAL:
		eventParticle->Emit(num, healParticleColor);
		break;
	}

	switch(effect)
	{
	case SPECIAL_APP_TELEPORT:
	case SPECIAL_APP_ADMIN_TELEPORT:
	case SPECIAL_APP_TELEPORT_AWAY:
	case SPECIAL_APP_DEATH:
	case SPECIAL_APP_RES:
	case SPECIAL_APP_DUNGEON:
	case SPECIAL_APP_ENTER_GAME:
	case SPECIAL_APP_LEAVE_GAME:
		teleportSound->PositionSound(mob->spacePoint.location, MAX_SOUND_DIST);
		teleportSound->Play();
		break;
	case SPECIAL_APP_HEAL:
		healSound->PositionSound(mob->spacePoint.location, MAX_SOUND_DIST);
		healSound->Play();
		break;
	}




}

//***************************************************************
void BBOClient::DrawBladeEffect(BBOMob *mob)
{
	if (!(localInfoFlags & LOCAL_FLAGS_GLAMOUR))
		return;

	float len = mob->bladeLength;
	if (BLADE_TYPE_MACE == mob->bladeType)
		len *= 0.5f;
	if (BLADE_TYPE_CLAWS == mob->bladeType)
		len *= 0.3f;
	if (BLADE_TYPE_DOUBLE == mob->bladeType)
		len *= 0.7f;

	if (mob->totalBPA && BLADE_TYPE_STAFF1 > mob->bladeType)
	{
		for (int i = 0; i < (1 + mob->totalBPA/25) && i < 5 && (mob->animCounter%3); ++i)
		{

			float pos = len * rnd(0.25f, 1.0f);

			int colorCount = 1 + (rand() % mob->totalBPA);

			int j = 0;
			for (; j < 5 && colorCount > mob->bladeParticleAmount[j]; ++j)
				colorCount -= mob->bladeParticleAmount[j];

			if (j > 4)
				j = 4;

			D3DXVECTOR3 pOut, pV;
			pV.x = 0; 
			pV.y = 0;
			pV.z = -1 * pos / 5.0f;

			if (BLADE_TYPE_CLAWS == mob->bladeType)
			{
				pV.x = 1 * pos / 5.0f; 
				pV.z = -1 * pos / 15.0f;
				pV.y = 1 * pos / 5.0f;
			}
			D3DXVec3TransformCoord(&pOut, &pV, &matBlade);
			swordParticle->SetEmissionPoint(pOut);
			swordParticle->Emit(1, mob->bladeParticleColor[j]); 

			if (BLADE_TYPE_DOUBLE == mob->bladeType)
			{
				pV.x = 0; 
				pV.y = 0;
				pV.z = 1 * pos / 5.0f;

				D3DXVec3TransformCoord(&pOut, &pV, &matBlade);
				swordParticle->SetEmissionPoint(pOut);
				swordParticle->Emit(1, mob->bladeParticleColor[j]); 

			}
		}
	}

	if (BLADE_TYPE_STAFF1 <= mob->bladeType && mob->attacking)
	{
		len /= 2.0f;
		float pos = len * rnd(0.25f, 1.0f);

		D3DXVECTOR3 pOut, pV;
		for (int i = 0; i < 10; ++i)
		{
			pV.x = rnd(-0.09f, 0.09f); 
			pV.y = rnd(-0.09f, 0.09f);
			pV.z = -1 * pos / 5.0f;

			D3DXVec3TransformCoord(&pOut, &pV, &matBlade);
			swordParticle->SetEmissionPoint(pOut);
			swordParticle->Emit(1, mob->bladeParticleColor[0]); 
		}
	}

	if (mob->hasTrail && BLADE_TYPE_STAFF1 > mob->bladeType)
	{
		D3DXVECTOR3 p1, p2, pV;
		pV.x = 0; 
		pV.y = 0;
		pV.z = 0;

		D3DXVec3TransformCoord(&p1, &pV, &matBlade);

		pV.x = 0; 
		pV.y = 0;
		pV.z = -1 * mob->bladeLength / 5.4f;

		if (BLADE_TYPE_KATANA == mob->bladeType)
			pV.x = -1 * mob->bladeLength / 100.f;
		else if (BLADE_TYPE_DOUBLE == mob->bladeType)
		{
			pV.x = 0; 
			pV.y = 0;
			pV.z = -1 * mob->bladeLength / 15.0f;
			D3DXVec3TransformCoord(&p1, &pV, &matBlade);

			pV.x = 0; 
			pV.y = 0;
			pV.z = -1 * mob->bladeLength / 6.5f;
		}
		else if (BLADE_TYPE_MACE == mob->bladeType)
		{
			pV.x = 0; 
			pV.y = 0;
			pV.z = -1 * mob->bladeLength / 11.0f;
			D3DXVec3TransformCoord(&p1, &pV, &matBlade);

			pV.x = 0; 
			pV.y = 0;
			pV.z = -1 * mob->bladeLength / 8.2f;
		}
		else if (BLADE_TYPE_CLAWS == mob->bladeType)
		{
			pV.x = 1 * len / 5.0f; 
			pV.z = -1 * len / 15.0f;
			pV.y = 1 * len / 5.0f;
		}

		D3DXVec3TransformCoord(&p2, &pV, &matBlade);

		ColorQuad cQuad;
		cQuad.r = mob->bladeTrailColor.r * 255;
		cQuad.g = mob->bladeTrailColor.g * 255;
		cQuad.b = mob->bladeTrailColor.b * 255;
		cQuad.vec[0] = p1;
		cQuad.vec[1] = p2;
		cQuad.vec[2] = mob->bladeTrailVec[0];
		cQuad.vec[3] = mob->bladeTrailVec[1];
		bladeTrail->AddQuad(cQuad);

		mob->bladeTrailVec[0] = p1;
		mob->bladeTrailVec[1] = p2;

		if (BLADE_TYPE_DOUBLE == mob->bladeType)
		{
			pV.x = 0; 
			pV.y = 0;
			pV.z = 1 * mob->bladeLength / 15.0f;
			D3DXVec3TransformCoord(&p1, &pV, &matBlade);

			pV.x = 0; 
			pV.y = 0;
			pV.z = 1 * mob->bladeLength / 6.5f;

			D3DXVec3TransformCoord(&p2, &pV, &matBlade);

			ColorQuad cQuad;
			cQuad.r = mob->bladeTrailColor.r * 255;
			cQuad.g = mob->bladeTrailColor.g * 255;
			cQuad.b = mob->bladeTrailColor.b * 255;
			cQuad.vec[0] = p1;
			cQuad.vec[1] = p2;
			cQuad.vec[2] = mob->bladeTrailVec[2];
			cQuad.vec[3] = mob->bladeTrailVec[3];
			bladeTrail->AddQuad(cQuad);

			mob->bladeTrailVec[2] = p1;
			mob->bladeTrailVec[3] = p2;
		}
	}

}


//***************************************************************
void BBOClient::HandlePlayerAppearance(int locX, int locY)
{

	if (SPACE_GROUND != curMapType)
		SetupOverlayLines(locX, locY);
	else
	{
		ground->UpdateTilesIfNeeded(locX, locY);
		SetupOverlayGroundLines(locX, locY);
	}

	MaintainStaticObjects(locX, locY);

	if (SPACE_DUNGEON == curMapType)
		bsMan.SetType(-2); // dungeon
	else if (SPACE_REALM == curMapType)
		bsMan.SetType(7); // desert
	else if (SPACE_GUILD == curMapType)
		bsMan.SetType(-4); // tower
	else if (SPACE_LABYRINTH == curMapType)
		bsMan.SetType(-3); // laby
	else
	{
		bsMan.SetType(ground->GetTerrainTypeOnce(locX, locY));
	}

	moveLockoutTimer = 0;

	// drop mobs that are too far away
	BBOMob *tempMob2 = (BBOMob *) monsterList->First();
	while (tempMob2)
	{
		if ((abs(tempMob2->cellX - locX) > 4 ||
		     abs(tempMob2->cellY - locY) > 4) && 
		     tempMob2 != playerAvatar)
		{
			monsterList->Remove(tempMob2);
			if (SMOB_TOWER           != tempMob2->type && 
				 SMOB_ITEM_SACK       != tempMob2->type  && 
				 SMOB_TOKEN           != tempMob2->type  && 
				 SMOB_WARP_POINT      != tempMob2->type  && 
				 SMOB_GROUND_EFFECT   != tempMob2->type  && 
				 SMOB_PARTICLE_STREAM != tempMob2->type)
			{
				ground->ReleaseSlot(tempMob2->cellX, tempMob2->cellY, tempMob2->cellSlot);
				flockSet->FreePoint(tempMob2->flockPointIndex, tempMob2);
			}
			delete tempMob2;
			tempMob2 = (BBOMob *) monsterList->First();
		}
		else
			tempMob2 = (BBOMob *) monsterList->Next();
	}

//	monsterList->Find(tempMob);

	// if in a dungeon, load any new visible parts
	if ((SPACE_DUNGEON == curMapType) || (SPACE_GUILD == curMapType))
	{
		dungeon->AskForNeededParts(locX, locY);
	}

	gTeleportHappenedFlag = TRUE;
}


//***************************************************************
void BBOClient::ClearStatics(void)
{
	BBOStatic *curStatic = (BBOStatic *) staticList->First();
	while (curStatic)
	{
		staticList->Remove(curStatic);
		if (-1 != curStatic->cellSlot)
			flockSet->FreePoint(curStatic->flockPointIndex, curStatic);
		delete curStatic;
		
		curStatic = (BBOStatic *) staticList->Next();
	}

	for (int y = 0; y < ground->sizeH; ++y)
	{
		for (int x = 0; x < ground->sizeW; ++x)
		{
			staticMap[y*ground->sizeW+x] = FALSE;
		}
	}
}

//***************************************************************
void BBOClient::AssertPointLight( float x, float y, float z,
											 float r, float g, float b, float range)
{

	D3DXVECTOR3 diff, source;
	source.x = x;
	source.y = y;
	source.z = z;
/*
	for (int i = 0; i < extraLightsUsed; ++i)
	{
		if (1000 != extraLights[i].y)
		{
			D3DXVec3Subtract(&diff, &extraLights[i], &playerPosForClient);
			if (D3DXVec3Length(&diff) > 50)
				extraLights[i].y = 1000;
		}
	}
*/
	// use any unused slots
	for (int i = 0; i < extraLightsUsed; ++i)
	{
		if (1000 == extraLights[i].y)
		{
			extraLights[i] = source;
			extraLightsColorR[i] = r;
			extraLightsColorG[i] = g;
			extraLightsColorB[i] = b;
			extraLightsRange[i] = range;
			return;
		}

	}

	int candidate = 0;

	// find farthest light
	for (int i = 0; i < extraLightsUsed; ++i)
	{
		D3DXVec3Subtract(&diff, &extraLights[candidate], &playerPosForClient);
		float dist1 = D3DXVec3Length(&diff);

		D3DXVec3Subtract(&diff, &extraLights[i], &playerPosForClient);
		float dist2 = D3DXVec3Length(&diff);

		if (dist1 < dist2)
		{
			candidate = i;
		}

	}

	D3DXVec3Subtract(&diff, &extraLights[candidate], &playerPosForClient);
	float dist1 = D3DXVec3Length(&diff);

	D3DXVec3Subtract(&diff, &source, &playerPosForClient);
	float dist2 = D3DXVec3Length(&diff);

	if (dist1 > dist2)
	{
		extraLights[candidate] = source;
		extraLightsColorR[candidate] = r;
		extraLightsColorG[candidate] = g;
		extraLightsColorB[candidate] = b;
		extraLightsRange[candidate] = range;
		return;
	}

}


//***************************************************************
void BBOClient::InitMainLights(void)
{

   D3DXVECTOR3 sunDirection[2];

	int lightIndex = 0;
   ZeroMemory( &mainLight[lightIndex], sizeof(D3DLIGHT8) );
   mainLight[lightIndex].Type      = D3DLIGHT_DIRECTIONAL;
   mainLight[lightIndex].Diffuse.r  = 1.0f;
   mainLight[lightIndex].Diffuse.g  = 1.0f;
   mainLight[lightIndex].Diffuse.b  = 1.0f;
   mainLight[lightIndex].Ambient.r  = 1.0f;
   mainLight[lightIndex].Ambient.g  = 1.0f;
   mainLight[lightIndex].Ambient.b  = 1.0f;
   sunDirection[lightIndex] = D3DXVECTOR3(1.0f, -1.0f, 0.5f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&mainLight[lightIndex].Direction, &sunDirection[lightIndex] );
   mainLight[lightIndex].Range      = 1000.0f;

	mainLight[2] = mainLight[0];
	mainLight[1] = mainLight[0];
	lightIndex = 1;

   sunDirection[lightIndex] = D3DXVECTOR3(-1.0f, -1.0f, -0.5f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&mainLight[lightIndex].Direction, &sunDirection[lightIndex] );

}

//***************************************************************
void BBOClient::SetMainLights(void)
{
	int ambR, ambG, ambB;
	ambR = ambG = ambB = 128;

	int oneHour = 60 * 6;
//	int oneHour = 5 * 6;

	if (SPACE_GROUND == curMapType)
	{
		if (isLightning && (localInfoFlags & LOCAL_FLAGS_WEATHER))
		{
			if (lightningCount > 0)
				--lightningCount;
			else if (27 == rand() % 180)
			{
				lightningCount = 4;
				int sndIndex = rand() % 3;
				weatherSound[sndIndex]->PlayNo3D();
			}
		}
	}

	if (SPACE_LABYRINTH == curMapType)
	{
		SetMainLightValues(0,    0.10f, 0.10f, 0.10f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(1,    0.10f, 0.10f, 0.10f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(2,    0.10f, 0.10f, 0.10f,      1.0f, 1.0f, 1.0f); 
		ambR = ambG = ambB = 128;
	}
	else if (SPACE_GROUND == curMapType)
	{
		if (timeOfDay < 2.5f * oneHour)
		{
			lightPercent = 1;
			SetMainLightValues(0,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
			SetMainLightValues(1,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
			SetMainLightValues(2,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
			ambR = ambG = ambB = 128;
		}
		else if (isLightning && lightningCount > 0)
		{
			lightPercent = 0.3f;
			SetMainLightValues(0,    0.30f, 0.30f, 0.30f,      0.30f, 0.30f, 0.30f); 
			SetMainLightValues(1,    0.30f, 0.30f, 0.30f,      0.30f, 0.30f, 0.30f); 
			SetMainLightValues(2,    0.70f, 0.70f, 0.70f,      0.50f, 0.50f, 0.80f); 
			ambR = ambG = ambB = 80;
		}
		else if (timeOfDay >= 2.75f * oneHour && timeOfDay < 3.75f * oneHour)
		{
			lightPercent = 0;
			SetMainLightValues(0,    0.10f, 0.10f, 0.10f,      0.10f, 0.10f, 0.10f); 
			SetMainLightValues(1,    0.10f, 0.10f, 0.10f,      0.10f, 0.10f, 0.10f); 
			SetMainLightValues(2,    0.70f, 0.70f, 0.70f,      0.50f, 0.50f, 0.80f); 
			ambR = ambG = ambB = 40;
		}
		else if (timeOfDay >= 2.5f * oneHour && timeOfDay < 2.75f * oneHour)
		{
			lightPercent = 1.0f - (timeOfDay - 2.5f * oneHour) / (0.25f * oneHour);

			SetMainLightValues(0,    
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent,
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent); 
			SetMainLightValues(1,    
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent,
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent); 
			SetMainLightValues(2,    
				        0.70f + 0.3f * lightPercent, 
						  0.70f + 0.3f * lightPercent, 
						  0.70f + 0.3f * lightPercent,
				        0.50f + 0.5f * lightPercent, 
						  0.50f + 0.5f * lightPercent, 
						  0.80f + 0.2f * lightPercent); 

			ambR = ambG = ambB = 40.0f + (128 - 40) * lightPercent;
		}
		else if (timeOfDay >= 3.75f * oneHour)
		{
			lightPercent = (timeOfDay - 3.75f * oneHour) / (0.25f * oneHour);

			SetMainLightValues(0,    
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent,
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent); 
			SetMainLightValues(1,    
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent,
				        0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent, 
						  0.10f + 0.9f * lightPercent); 
			SetMainLightValues(2,    
				        0.70f + 0.3f * lightPercent, 
						  0.70f + 0.3f * lightPercent, 
						  0.70f + 0.3f * lightPercent,
				        0.50f + 0.5f * lightPercent, 
						  0.50f + 0.5f * lightPercent, 
						  0.80f + 0.2f * lightPercent); 

			ambR = ambG = ambB = 40.0f + (128 - 40) * lightPercent;
		}
	}
	else if (SPACE_DUNGEON == curMapType && isInSpecialDungeon)
	{
		SetMainLightValues(0,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(1,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(2,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		ambR = 30;
		ambG = ambB = 128;
	}
	else
	{
		SetMainLightValues(0,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(1,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		SetMainLightValues(2,    1.0f, 1.0f, 1.0f,      1.0f, 1.0f, 1.0f); 
		ambR = ambG = ambB = 128;
	}

   puma->m_pd3dDevice->SetLight( 0, &mainLight[0] );
   puma->m_pd3dDevice->LightEnable( 0, TRUE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   puma->m_pd3dDevice->SetLight( 1, &mainLight[1] );

   // Finally, turn on some ambient light.
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(ambR, ambG, ambB, 255)  );

}


//***************************************************************
void BBOClient::SetMainLightValues(int index, 
         float ar, float ag, float ab, float dr, float dg, float db)
{
	mainLight[index].Ambient.r  = ar;
	mainLight[index].Ambient.g  = ag;
	mainLight[index].Ambient.b  = ab;

	if (dr > -1)
		mainLight[index].Diffuse.r  = dr;
	if (dg > -1)
		mainLight[index].Diffuse.g  = dg;
	if (db > -1)
		mainLight[index].Diffuse.b  = db;

}


//***************************************************************
void BBOClient::SetEnvironment(void)
{
   ZeroMemory( &neutralMaterial, sizeof(D3DMATERIAL8) );
   neutralMaterial.Diffuse.r = 0.5f;
   neutralMaterial.Diffuse.g = 0.5f;
   neutralMaterial.Diffuse.b = 0.5f;
   neutralMaterial.Diffuse.a = 0.5f;
   neutralMaterial.Ambient.r = 0.5f;
   neutralMaterial.Ambient.g = 0.5f;
   neutralMaterial.Ambient.b = 0.5f;
   neutralMaterial.Ambient.a = 0.5f;

	InitMainLights();

	aLog.Log("BBOClient::SetEnvironment ********************\n");

}

/* end of file */



