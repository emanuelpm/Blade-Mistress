#ifndef DUNGEON_CLIENT_H
#define DUNGEON_CLIENT_H

#include "puma.h"
#include "../helper/linklist.h"
#include "plasmaTexture.h"
#include <d3d8types.h>
#include "../BBO.h"
#include "pumamesh.h"
#include "../groundObject.h"  // gets VERTEXSTRUCT + other things from here
#include "../network/NetWorldMessages.h"

/*
	short sizeX, sizeY; // total dungeon size
	short x,y; // in 5s
	char leftWall[DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE], 
		  topWall [DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE];
	char floor;
*/
//***************************************************************************************
class DungeonPiece : public DataObject
{
public:
	DungeonPiece(int doid, char *doname);
//	DungeonPiece(void) {vertPtrArray = NULL;};
	virtual ~DungeonPiece();

	void InitVertArray(LPDIRECT3DDEVICE8 pd3dDevice);
	void ReleaseVertArray(void);

	void ProcessMessage(MessDungeonInfo *info);
	void RebuildWalls(void);
	void BuildFace(GROUNDVERTEXSTRUCT* pVertices, 
									  float mx , float my , float mz,
									  float mx2, float my2, float mz2,
									  float uvMax);
	void BuildTopFace(GROUNDVERTEXSTRUCT* pVertices, 
									  float mx , float my , float mz,
									  float mx2, float my2, float mz2,
									  float uvMax);


	LocationSlots *slotArray;

	short x,y; // in 5s
	unsigned char leftWall[DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE], 
		           topWall [DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE];
	unsigned char floorIndex, outerWallIndex;

	int numOfVertices[NUM_OF_DUNGEON_WALL_TYPES + 1]; // outer wall needs verts, too
	LPDIRECT3DVERTEXBUFFER8 *vertPtrArray; // Buffer to hold vertices
//	LPDIRECT3DVERTEXBUFFER8 *floorVerts;   // Buffer to hold vertices

};

//***************************************************************************************
class DungeonClient : public DataObject
{
public:

	DungeonClient(int doid, char *doname);
	virtual ~DungeonClient();
	void Init(void);
	void ProcessMessage(MessDungeonInfo *info);
	void ProcessChangeMessage(MessDungeonChange *info);


	void Draw(int x, int y);
	
	void AskForNeededParts(int px, int py);

	void GetSlotPosition(int slotIndex, float &x, float &y, float &angle);
	int  GetFirstOpenSlot(int x, int y);
	void ClaimSlot(int x, int y, int index, int type = SLOT_MOB);
	void ReleaseSlot(int x, int y, int index);
//	void CreateStaticPositions(void);
	float CellSize(void);

	int sizeW, sizeH;

   D3DMATERIAL8 material;
	LPDIRECT3DTEXTURE8 pTexture[NUM_OF_DUNGEON_WALL_TYPES+1]; // 0 = floor, 1 = outer wall
	int textureRef[NUM_OF_DUNGEON_WALL_TYPES+1];              // 0 = floor, 1 = outer wall
	char floorType, outerWallType;

	DoublyLinkedList *pieces;

	PumaMesh *wallMesh;

};

#endif
