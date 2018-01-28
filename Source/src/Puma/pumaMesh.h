#ifndef PUMA_MESH_H
#define PUMA_MESH_H

#include "puma.h"
#include "../helper/dataobje.h"
#include "../helper/linklist.h"

class RawVerts;

// A structure for our custom vertex type.
struct PUMAMESHVERTEXSTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DXVECTOR3 normal;   // The surface normal for the vertex
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define PUMAMESH_VERTEXDESC (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//***************************************************************************************
class PumaMesh : public DataObject
{
public:

	PumaMesh(void);
	PumaMesh(int doid, char *doname);
	virtual ~PumaMesh();

	void LoadFromASC(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL);
	void LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL, int alphaType = 0);

	void LoadTestPattern(LPDIRECT3DDEVICE8 pd3dDevice);

	void Assimilate(LPDIRECT3DDEVICE8 pd3dDevice, PumaMesh *pm);
	void Shift(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z);
	void Scale(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z);
	void Mirror(LPDIRECT3DDEVICE8 pd3dDevice);
	void MirrorZ(LPDIRECT3DDEVICE8 pd3dDevice);
	void Center(LPDIRECT3DDEVICE8 pd3dDevice);
	void Rotate90(LPDIRECT3DDEVICE8 pd3dDevice);
	void SetColor(int r, int g, int b, int a);
	PumaMesh *MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice);

	void Save(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL, int resetNormals = TRUE);
	void Load(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL);
	void SaveCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals = TRUE);
	void LoadCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName);

	void Draw(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 otherArt = NULL);
	
	void SetNormalsPure(LPDIRECT3DDEVICE8 pd3dDevice);
	void RebuildNormals(LPDIRECT3DDEVICE8 pd3dDevice);
	D3DXVECTOR3 FindNormalForPoint(PUMAMESHVERTEXSTRUCT *pMyVertices, D3DXVECTOR3 point, int &count);

	float SizeX(LPDIRECT3DDEVICE8 pd3dDevice);
	float SizeY(LPDIRECT3DDEVICE8 pd3dDevice);
	float SizeZ(LPDIRECT3DDEVICE8 pd3dDevice);
	void  GetBounds(LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 &least, D3DXVECTOR3 &most);

	static BOOL IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* dist);

	void FindCollisionsWithRawVertObject(
			LPDIRECT3DDEVICE8 pd3dDevice,      // need the device to lock my verts
			D3DXMATRIX &meshMatrix,            // orientation of myself
			RawVerts &rv,                      // the verts to collide against
			D3DXMATRIX &rawOldMatrix,          // orientation of raw verts last frame
			D3DXMATRIX &rawNewMatrix,          // orientation of raw verts NOW
			D3DXVECTOR3 *collisionPointList,  // array to place collision info
			D3DXVECTOR3 *collisionNormalList, // array to place collision info
			int &listSize							  // size of collision array, modified to numOfCollisions
			);


	int isReady;

	D3DXVECTOR3 position;
	D3DXVECTOR3 rotation;

	int numOfVertices;
	VideoDataObject *vertexList; // Buffer to hold vertices
	VideoDataObject *texture; // Our texture
   D3DMATERIAL8 mtrl;

};

extern D3DXVECTOR3 collTestPos[100];
extern int collTestCount;

#endif
