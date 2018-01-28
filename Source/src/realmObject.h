#ifndef REALM_OBJECT_H
#define REALM_OBJECT_H

#include "./puma/puma.h"
#include "./helper/linklist.h"
#include ".\puma\plasmaTexture.h"
#include <d3d8types.h>
#include "BBO.h"
#include "./puma/pumamesh.h"
#include "groundObject.h"

//***************************************************************************************
class RealmObject : public DataObject
{
public:

	RealmObject(int type);
	RealmObject(int doid, char *doname);
	virtual ~RealmObject();
	void Init(char *colorMapFileName, char *skyFileName);

	D3DCOLOR GetVertColor  (int i, int j, int height);
	int      GetTerrainType(int i, int j);
	D3DXVECTOR3 GetVertPos   (int y, int x, int height);
	D3DXVECTOR3 GetVertNormal(int y, int x);
	D3DXVECTOR3 SetNormal(int x, int y, D3DXVECTOR3 point);
	void Generate(char *heightMapFileName, LPDIRECT3DDEVICE8 pd3dDevice, unsigned long randSeed = 0);
	void InitVertArray(LPDIRECT3DDEVICE8 pd3dDevice);
	void ReleaseVertArray(void);
	void LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName);
	void LoadSkyTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName);

	void Draw(int x, int y);
	void DrawFog(void);

	float GetXForPoint(int gridX);
	float GetYForPoint(int x, int y);
	void  SetYForPoint(int x, int y, float newH);
	float GetZForPoint(int gridY);
	int   GetGridX(float x);
	int   GetGridY(float z);
	float CellSize(void);
	float HeightAtPoint(float pointX, float pointZ, D3DXVECTOR3 *targetNormal);
	
	void GetSlotPosition(int slotIndex, float &x, float &y, float &angle);
	int  GetFirstOpenSlot(int x, int y, int backwards = FALSE);
	void ClaimSlot(int x, int y, int index, int type = SLOT_MOB);
	void ReleaseSlot(int x, int y, int index);
	void CreateStaticPositions(void);
	void CreateSpiritStaticPositions(int type);

	void OpenGroundInfoTexture(void);
	void CloseGroundInfoTexture(void);
	D3DCOLOR GroundInfoTexturePixel(int x, int y);


	int sizeW, sizeH;
	int ntSizeW, ntSizeH;
	normalRecord *normalTable;
	LocationSlots *slotArray;

	PlasmaTexture *pt;
	int numOfVertices;
	LPDIRECT3DVERTEXBUFFER8 *vertPtrArray; // Buffer to hold vertices
	LPDIRECT3DTEXTURE8 pTexture, skyTexture; // Our textures
   D3DMATERIAL8 groundMaterial;

	LPDIRECT3DSURFACE8 pGroundInfoSurface;
	D3DLOCKED_RECT     groundInfoLockInfo;

	char colorMapFileName[64];

	int realmType;

	PumaMesh *ruinMesh[3];

};

#endif
