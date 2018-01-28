#ifndef GROUND_OBJECT_H
#define GROUND_OBJECT_H

#include "./puma/puma.h"
#include "./helper/linklist.h"
#include ".\puma\plasmaTexture.h"
#include <d3d8types.h>
#include "BBO.h"
#include "./puma/pumamesh.h"
#include "clientOnly.h"


const float HEIGHT_COEFF = 30.0f;

// A structure for our custom vertex type.
struct GROUNDVERTEXSTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DXVECTOR3 normal;   // The surface normal for the vertex
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define GROUND_VERTEXDESC (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//***************************************************************************************
class GroundObject : public DataObject
{
public:

	GroundObject(void);
	GroundObject(int doid, char *doname);
	virtual ~GroundObject();
	void Init(void);

	D3DCOLOR GetVertColor  (int i, int j, int height);
	int      GetTerrainTypeOnce(int i, int j);
	int      GetTerrainType(int i, int j);
	D3DXVECTOR3 GetVertPos   (int y, int x, int height);
	D3DXVECTOR3 GetVertNormal(int y, int x);
	D3DXVECTOR3 SetNormal(int x, int y, D3DXVECTOR3 point);
	void Generate(LPDIRECT3DDEVICE8 pd3dDevice, int w, int h, unsigned long randSeed = 0);
	void InitVertArray(LPDIRECT3DDEVICE8 pd3dDevice);
	void ReleaseVertArray(void);
	void LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName);

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

	int sizeW, sizeH;
	int ntSizeW, ntSizeH;
	normalRecord *normalTable;
	LocationSlots *slotArray;

	PlasmaTexture *pt;
	int numOfVertices;
	LPDIRECT3DVERTEXBUFFER8 *vertPtrArray; // Buffer to hold vertices
	LPDIRECT3DTEXTURE8 pTexture; // Our texture
   D3DMATERIAL8 groundMaterial;
	PumaMesh *fog1, *fog2, *fog3;

};

extern D3DXVECTOR3 CalculateNormal(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2);
extern float GetGroundHeight(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2, D3DXVECTOR3 normal, float X, float Z);


#endif
