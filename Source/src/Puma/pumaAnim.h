#ifndef PUMA_ANIM_H
#define PUMA_ANIM_H

#include "puma.h"
#include "../helper/dataobje.h"
#include "../helper/linklist.h"

class PumaMesh;
// A structure for our custom vertex type.
struct PUMAANIMVERTEXSTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DXVECTOR3 normal;   // The surface normal for the vertex
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define PUMAANIM_VERTEXDESC (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//***************************************************************************************
class PumaAnimFrame : public DataObject
{
public:

	PumaAnimFrame(int index);
	virtual ~PumaAnimFrame();

	int numOfVertices;
	VideoDataObject *vertexList; // Buffer to hold vertices

	D3DXVECTOR3 bladePosition, bladeDirection;
	D3DXVECTOR3 extraPosition[5];

};

//***************************************************************************************
class PumaAnim : public DataObject
{
public:

	PumaAnim(int doid, char *doname);
	virtual ~PumaAnim();

	void LoadFromASC(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL);
	void LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL, int alphaType = 0);

	void Shift(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z);
	void Scale(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z);
	void Mirror(LPDIRECT3DDEVICE8 pd3dDevice);
	void Center(LPDIRECT3DDEVICE8 pd3dDevice);
	PumaAnim *MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice);
	PumaMesh *MakeFrameCopy(LPDIRECT3DDEVICE8 pd3dDevice, int frame);

	void Save(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL, int resetNormals = TRUE);
	void Load(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName = NULL);
	void SaveCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals = TRUE);
	void LoadCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, DWORD vertColor = 0xffffffff);

	void Draw(LPDIRECT3DDEVICE8 pd3dDevice, int frameNumber);
	
	void RebuildNormals(LPDIRECT3DDEVICE8 pd3dDevice);
	D3DXVECTOR3 FindNormalForPoint(PUMAANIMVERTEXSTRUCT *pMyVertices, int vertNum, 
		                            D3DXVECTOR3 point, int &count);
	void SetExtraPosition(LPDIRECT3DDEVICE8 pd3dDevice, int index, int vertIndex);
	void AdjustExtraPosition(LPDIRECT3DDEVICE8 pd3dDevice, int index, 
											  float dx, float dy, float dz);
	int  FindTopVert(LPDIRECT3DDEVICE8 pd3dDevice);

	VideoDataObject *texture; // Our texture
   D3DMATERIAL8 mtrl;

	DoublyLinkedList frames;
	int numOfFrames;
	DWORD alpha;
	D3DXVECTOR3 size;

};

#endif
