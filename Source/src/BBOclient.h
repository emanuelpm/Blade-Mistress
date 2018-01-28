#ifndef BBOCLIENT_H
#define BBOCLIENT_H

#include "./helper/linklist.h"
#include "./puma/puma.h"
#include "./puma/pumamesh.h"
#include "./puma/pumaAnim.h"
#include "./helper/GeneralUtils.h"
#include "./puma/d3dutil.h"

#include "GroundObjectTiles.h"
#include "realmObject.h"
#include "labyrinthObject.h"
#include "dungeonClient.h"

#include "./network/client.h"
#include "BBO.h"
#include "BBOmob.h"
#include "BBOstatic.h"
#include "FlockPoints.h"
#include "backSound.h"
#include "petSound.h"

const int NUM_OF_CONTACT_OBJECTS = 25;

// A structure for our line type.
struct PUMALINESTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
};

// Our custom FVF, which describes our custom vertex structure
#define PUMA_LINEDESC (D3DFVF_XYZ|D3DFVF_DIFFUSE)

class LandShadowManager;
class LandSquareManager;


//***********************************************************************************
class BBOClient
{
public:

	BBOClient();
	virtual ~BBOClient();

	void Tick(void);
	void Draw(void);
	void HandleMessage(char *messData, int dataSize);

	void SetupObjectOrientation(float yaw, float x, float y, float z);
	void SetupBladeOrientation(PumaAnimFrame *frame, float yaw, float x, float y, float z);

	void ClearStatics(void);
	void MaintainStaticObjects(int posX, int posY);
	void SetupOverlayLines(int posX, int posY);
	void SetupOverlayGroundLines(int posX, int posY);
	void SetClickVertQuad(int quadIndex, int cellX, int cellY);
	void DrawOverlayLines(void);
	void DrawOverlayGroundLines(void);
	float HeightAtPoint(float x, float z);

	void HandleSpecialAppearance(BBOMob *mob, unsigned short effect);
	void HandlePlayerAppearance(int locX, int locY);
	void DrawBladeEffect(BBOMob *mob);
	void DrawHalo(PumaAnimFrame *frame, float yaw, float x, float y, float z);
	void DrawPetAttackEffect(int which, BBOMob *mob, int job);

	void AssertPointLight( float x, float y, float z,
								  float r, float g, float b, float range);

	void InitMainLights(void);
	void SetMainLights (void);
	void SetMainLightValues(int index, 
         float ar, float ag, float ab, float dr = -1, float dg = -1, float db = -1);

	void SetEnvironment(void);

	FlockSet *flockSet;
	BBOMob *playerAvatar, *dummyAvatar, *selectedMOB;
//	BBOMob *contactMobs[NUM_OF_CONTACT_OBJECTS];
//	int contactListHasChanged;
	char *avatarNameChange;
	DoublyLinkedList *monsterList, *staticList;
	D3DMATERIAL8 neutralMaterial, planetMaterial;
	PumaMesh *skyDome, *monsterMesh, *sackMesh, *caveMesh[5], *standinMesh, *chestMesh[2],
		      *greatTreeMesh, *flowerMesh[3], *towerMesh, *tokenMesh[MAGIC_MAX], *ropEnteranceMesh, *castleMesh;
	PumaAnim *avatarStand, *avatarRun, *avatarAttack, *avatarAttack2, *merchantAnim,
		      *witchStandAnim, *witchTalkAnim;
	LPDIRECT3DTEXTURE8 merchantBitmap, trainerBitmap, townmageBitmap, barArt[2];
	LPDIRECT3DTEXTURE8 specialDunSkyTexture;
	GroundObjectTiles *ground;
	LandShadowManager *lsMan;
	LandSquareManager *lSquareMan;
	DungeonClient *dungeon;
	RealmObject *realm;
	LabyrinthObject *labyrinth;

	char *staticMap;
	D3DXMATRIX matBlade, matWorldValue;

	float curCamAngle, realCamAngle, camPitch;
	float camCenterX, camCenterZ;

	int playerAvatarID, flashCounter;
	long controlledMonsterID;
	float controlledMonsterY;

	int avatarCellX, avatarCellY;
	int showAttack, witchTalkCounter;
	SpacePoint avatarSP;
	long wieldedPtr;
	int isEditingDungeon, isInSpecialDungeon, showFrameRate, 
		 cameraType, needToName, drunkenWalk;

	D3DXVECTOR3 clickVerts[3*2*5];

	DWORD lastTick;

	LPDIRECT3DVERTEXBUFFER8 lineOverlayPtr; // Buffer to hold vertices
	int numOfLines;
	LPDIRECT3DVERTEXBUFFER8 lineOverlayGroundPtr; // Buffer to hold vertices
	int numOfGroundLines;

	DWORD timeOfDay, lastTenSecondTimer;


	BackSoundManager bsMan;
	PetSoundManager  psMan;
	PumaSound *footstep[4];
	PumaSound *monsterPain[NUM_OF_MONSTERS];
	PumaSound *avatarPain, *teleportSound, *healSound, *thiefSound;
	PumaSound *bombSound[7];
	PumaSound *weatherSound[3];
	
	D3DXVECTOR3 extraLights[5];
	unsigned char extraLightsColorR[5], extraLightsColorG[5], extraLightsColorB[5];
	float extraLightsRange[5];
	int extraLightsUsed;

   D3DLIGHT8   mainLight[3];
	float lightPercent;
	int isLightning, lightningCount;

	int curMapSizeX, curMapSizeY;
};



#endif
