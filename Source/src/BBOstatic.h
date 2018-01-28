#ifndef BBOSTATIC_H
#define BBOSTATIC_H

#include ".\helper\linklist.h"
#include ".\puma\puma.h"
#include ".\puma\pumamesh.h"
#include "flockPoints.h"
#include "BBO.h"
#include "avatarTexture.h"
				  
const int NUM_OF_STATIC_OBJECTS = 16 +2 +5; // +2 for pet graves, +5 for flowers

class BBOStatic : public DataObject
{
public:

	BBOStatic(int doid, char *doname);
	virtual ~BBOStatic();

	SpacePoint spacePoint;

	int pleaseDelete;
	int cellX, cellY, cellSlot;
	unsigned long flockPointIndex;

	static LPDIRECT3DTEXTURE8 skins[NUM_OF_STATIC_OBJECTS];
	static PumaMesh *meshes[NUM_OF_STATIC_OBJECTS];
	static int refCount;

	D3DXVECTOR3 *flowerLocList;
	char flowerType;
	int flowerLocListSize;

};

#endif
