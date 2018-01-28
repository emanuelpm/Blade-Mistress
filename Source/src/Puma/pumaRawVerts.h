#ifndef PUMA_RAW_VERTS_H
#define PUMA_RAW_VERTS_H

#include "puma.h"
#include "..\helper\dataobje.h"
#include "..\helper\linklist.h"

//    D3DXVECTOR3 position; // The position

//***************************************************************************************
class RawVerts : public DataObject
{
public:

	RawVerts(void);
	RawVerts(int doid, char *doname);
	virtual ~RawVerts();

	void LoadFromASC(char *fileName = NULL);

	void Assimilate(RawVerts *pm);
	void Shift(float x, float y, float z);
	void Scale(float x, float y, float z);
	void Mirror(void);
	void MirrorZ(void);
	void Center(void);
	void Rotate90(void);
	RawVerts *MakeCopy(void);

	float SizeX(void);
	float SizeY(void);
	float SizeZ(void);
	void  GetBounds(D3DXVECTOR3 &least, D3DXVECTOR3 &most);

	int isReady;

	int numOfVertices;
	D3DXVECTOR3 *vertexList;

};

#endif
