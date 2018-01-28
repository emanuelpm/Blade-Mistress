#ifndef GROUND_OBJECT_TILES_H
#define GROUND_OBJECT_TILES_H

#include "./puma/puma.h"
#include "./helper/linklist.h"
#include "./puma/plasmaTexture.h"
#include <d3d8types.h>
#include "BBO.h"
#include "./puma/pumamesh.h"
#include "clientOnly.h"

enum SIDES
{
	TOP=0,
	LEFT,
	RIGHT,
	BOTTOM,
	TOTAL_SIDES
};

const float TILED_HEIGHT_COEFF = 30.0f;

#define TILE_VERTS 9	// our tiles are 9x9 vertices
#define TILE_COUNT 16	// our terrain in 16x16 tiles

// the overall terrain is a set of VERTEX_COUNT x VERTEX_COUNT vertices
#define VERTEX_COUNT ((TILE_COUNT*(TILE_VERTS-1))+1) 

//extern struct normalRecord;
/*
{
	float x, y;
//	int numOfNeighbors;
};
*/

// A structure for our custom vertex type.
struct GROUNDTILEDVERTEXSTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DXVECTOR3 normal;   // The surface normal for the vertex
    D3DCOLOR    color;    // The color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our custom vertex structure
#define GROUND_TILED_VERTEXDESC (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

//struct LocationSlots;
/*
{
	unsigned char used[NUM_OF_SLOTS_PER_SPACE];
//	long toughestMonsterPoints;
};
*/

struct FlowerRecord
{
	unsigned char x,y,type, amount;

};

//***************************************************************************************
class GroundObjectTiles : public DataObject
{
public:

	// Data Types & Constants...

	// our sample contains 4 possible detail levels per tile
	enum LEVEL
	{
		LEVEL_0 = 0,
		LEVEL_1,
		LEVEL_2,
		LEVEL_3,
		TOTAL_LEVELS
	};

	// data about each index buffer we create
	struct INDEX_BUFFER
	{
		IDirect3DIndexBuffer8*	pIndexBuffer;
		int						IndexCount;
		int						TriangleCount;
	};

	// the index buffers needed for each of our detail levels
	struct DETAIL_LEVEL
	{
		INDEX_BUFFER 	TileBodies[16];
		INDEX_BUFFER	TileConnectors[TOTAL_SIDES][TOTAL_LEVELS];
	};

	// a tile of our terrain
	struct TILE
	{
		LEVEL					DetailLevel;// the current detail level to use
		IDirect3DVertexBuffer8* VBuffer;	// a pile of vertices for this tile
		D3DXVECTOR3				Center;		// the center point of the tile
	};

	// our vertex format for this sample
	struct TERRAIN_VERTEX
	{
		D3DXVECTOR3  vert;
		D3DXVECTOR3  norm;
	   D3DCOLOR    color;    // The color
		float		tu,tv;
	};

	// the FVF code for this sample
	enum VERTEX_FVF
	{
		FVF = D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1
	};

	// Creators...

	GroundObjectTiles(void);
	GroundObjectTiles(int doid, char *doname);
	virtual ~GroundObjectTiles();
	void Init(void);

	// Operators...

	// Mutators...
	void GenerateDetailLevels();
	void GenerateTiles(int oX = 0, int oY = 0);
	void DrawTerrain();
	void UpdateTilesIfNeeded(int oX, int oY);

	int      GrassCanGoAt(float x, float y);
	D3DCOLOR GetTileGroundColor(int i, int j, int roadValue);
	D3DCOLOR GetVertColor  (int i, int j, int height);
	int      GetTerrainTypeOnce(int i, int j);
	int      GetTerrainType(int i, int j);
	D3DXVECTOR3 GetVertPos   (int y, int x, int height);
	D3DXVECTOR3 GetVertNormal(int y, int x);
//	D3DXVECTOR3 SetNormal(int x, int y, D3DXVECTOR3 point);
	void Generate(LPDIRECT3DDEVICE8 pd3dDevice, int w, int h, unsigned long randSeed = 0);
	void InitVertArray(LPDIRECT3DDEVICE8 pd3dDevice);
	void ReleaseVertArray(void);
	void LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName);

	void Draw(int x, int y);
	void DrawFog(void);

	float GetXForPoint(int gridX);
	float GetYForPoint(int x, int y);
	float GetYForTruePoint(int x, int y);
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
	void ClearStaticPositionsFor(int x, int y);
	void BuildRing(int x, int y, int objectType);
	void BuildJumble(int x, int y, int objectType);

	int sizeW, sizeH;
	int ntSizeW, ntSizeH;
	normalRecord *normalTable;
	LocationSlots *slotArray;
	FlowerRecord *flowerArray;


	PlasmaTexture *pt;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture
   D3DMATERIAL8 groundMaterial;
	PumaMesh *fog1, *fog2, *fog3;

	int mapOffsetX, mapOffsetY;

	D3DXVECTOR3			m_CameraPos;		  // position of the camera
	float				m_CameraAngle;

	unsigned char *roadExclusionMap;

	// shadow data
	void StartFrame(void);
	void AddShadow(float x, float y, float scale);
	void DrawShadows(void);

	LPDIRECT3DTEXTURE8 shadowBitmap;
	LPDIRECT3DVERTEXBUFFER8 shadowVBuffer;
	int shadowTriIndex;
//	char terrainDetailLevel[TILE_COUNT*TILE_COUNT];

	D3DCOLOR water, desert, snow, swamp, forest, deepForest, waste;


private:

	// Data...
	TILE				m_TerrainTile[TILE_COUNT][TILE_COUNT];	// our sample terrain of 32x32 tiles
	DETAIL_LEVEL		m_DetailLevel[TOTAL_LEVELS]; // data for each detail level

//   LPDIRECT3DTEXTURE8	m_pMeshTexture;			// simple texture for our terrain
	LPDIRECT3DSURFACE8	m_pHeightData;			// greyscale height information
   D3DMATERIAL8		m_MeshMaterial;			// a basic material for the terrain

   D3DXMATRIX          m_matWorld;			  // the world transform matrix

	int					m_FacesDrawn;		  // count of faces actually rendered each frame

	float				m_CameraOffset;		// camera's zoom factor

	TERRAIN_VERTEX TerrainVerts[VERTEX_COUNT][VERTEX_COUNT];
};

extern D3DXVECTOR3 CalculateNormal(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2);
extern float GetGroundHeight(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2, D3DXVECTOR3 normal, float X, float Z);


#endif
