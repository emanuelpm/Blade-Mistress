//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "./puma/puma.h"
#include "GroundObjectTiles.h"
//#include "TileDefinitions.h" // <- the index buffers are defined in here
//#include "noise.h"
#include "BBO.h"
#include "TileDefinitions.h" // <- the index buffers are defined in here
#include "./helper/GeneralUtils.h"
#include "BBOShadows.h"

#include ".\helper\autolog.h"



LPDIRECT3DSURFACE8 pTiledGroundInfoSurface[4];
D3DLOCKED_RECT tiledGrndLockInfo[4];

//*******************************************************************************
void OpenTiledGroundInfoTexture(void)
{
	aLog.Log("OpenTiledGroundInfoTexture ");

//   D3DXCreateTextureFromFile( puma->m_pd3dDevice, "dat\\terrain-info.bmp", &pTiledGroundInfoSurface );
	pTiledGroundInfoSurface[0]->LockRect(&tiledGrndLockInfo[0] , 0, 
		D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY );
	pTiledGroundInfoSurface[1]->LockRect(&tiledGrndLockInfo[1] , 0, 
		D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY );
	pTiledGroundInfoSurface[2]->LockRect(&tiledGrndLockInfo[2] , 0, 
		D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY );
	pTiledGroundInfoSurface[3]->LockRect(&tiledGrndLockInfo[3] , 0, 
		D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY );


	aLog.Log("Done\n");

}

//*******************************************************************************
void CloseTiledGroundInfoTexture(void)
{
	aLog.Log("CloseTiledGroundInfoTexture ");

	pTiledGroundInfoSurface[3]->UnlockRect();
	pTiledGroundInfoSurface[2]->UnlockRect();
	pTiledGroundInfoSurface[1]->UnlockRect();
	pTiledGroundInfoSurface[0]->UnlockRect();
	aLog.Log("Done\n");
}

//*******************************************************************************
D3DCOLOR TiledGroundInfoTexturePixel(int x, int y)
{
//	aLog.Log("TiledGroundInfoTexturePixel ");
//	aLog.Log(x);
//	aLog.Log("x");
//	aLog.Log(y);

	int index = x / 128 + (y/128*2);
	if (x > 127)
		x -= 128;
	if (y > 127)
		y -= 128;

	char *charPtr = (char *)tiledGrndLockInfo[index].pBits + tiledGrndLockInfo[index].Pitch * y;
	D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;

//	aLog.Log("Done\n");

	return pixelPtr[x];
}


//***************************************************************************************
//***************************************************************************************
GroundObjectTiles::GroundObjectTiles(int doid, char *doname)	 : DataObject(doid,doname)
{
	Init();
}

//***************************************************************************************
GroundObjectTiles::GroundObjectTiles(void) : DataObject(0,"PLASMA_TEXTURE")
{
	Init();
}

//***************************************************************************************
GroundObjectTiles::~GroundObjectTiles()
{

   SAFE_RELEASE( shadowBitmap );
   SAFE_RELEASE( shadowVBuffer );

	SAFE_RELEASE(m_pHeightData);

	// release the detail levels
	int i,j,k,l;
	for (i=0;i<TOTAL_LEVELS;++i)
	{
		for (j=0;j<16;++j)
		{
			SAFE_RELEASE(m_DetailLevel[i].TileBodies[j].pIndexBuffer);
		}
		for (k=0;k<TOTAL_SIDES;++k)
		{
			for (l=0;l<TOTAL_LEVELS;++l)
			{
				SAFE_RELEASE(m_DetailLevel[i].TileConnectors[k][l].pIndexBuffer);
			}
		}
	}

	// release the tiles
	for (i=0;i<TILE_COUNT;++i)
	{
		for (j=0;j<TILE_COUNT;++j)
		{
			SAFE_RELEASE(m_TerrainTile[i][j].VBuffer);
		}
	}


   SAFE_DELETE(roadExclusionMap);

   SAFE_DELETE(pt);
   if (slotArray)
		delete[] slotArray;
	SAFE_DELETE(flowerArray);
	ReleaseVertArray();
   SAFE_RELEASE(pTexture);

	SAFE_RELEASE(pTiledGroundInfoSurface[3]);
	SAFE_RELEASE(pTiledGroundInfoSurface[2]);
	SAFE_RELEASE(pTiledGroundInfoSurface[1]);
	SAFE_RELEASE(pTiledGroundInfoSurface[0]);

}

//***************************************************************************************
void GroundObjectTiles::Init(void)
{
	aLog.Log("GroundObjectTiles::Init ");

	roadExclusionMap = NULL;

	water      = D3DCOLOR_RGBA(  0,  0,255,255) ;
	desert     = D3DCOLOR_RGBA(255,255,  0,255) ;
	snow       = D3DCOLOR_RGBA(255,255,255,255) ;
	swamp      = D3DCOLOR_RGBA(255,  0,  0,255) ;
	forest     = D3DCOLOR_RGBA(  0,128,  0,255) ;
	deepForest = D3DCOLOR_RGBA(  0,255,  0,255) ;
	waste      = D3DCOLOR_RGBA(149,111,  7, 255) ;

	if (SUCCEEDED(puma->m_pd3dDevice->CreateImageSurface(128,128, 
		 D3DFMT_A8R8G8B8, &pTiledGroundInfoSurface[0])))
	{
		D3DXLoadSurfaceFromFile( pTiledGroundInfoSurface[0], 0, 
			NULL, _T("dat\\terrain-info.png"), NULL, D3DX_FILTER_NONE, 0, 0);
	}

	if (SUCCEEDED(puma->m_pd3dDevice->CreateImageSurface(128,128, 
		 D3DFMT_A8R8G8B8, &pTiledGroundInfoSurface[1])))
	{
		D3DXLoadSurfaceFromFile( pTiledGroundInfoSurface[1], 0, 
			NULL, _T("dat\\terrain-infob.png"), NULL, D3DX_FILTER_NONE, 0, 0);
	}

	if (SUCCEEDED(puma->m_pd3dDevice->CreateImageSurface(128,128, 
		 D3DFMT_A8R8G8B8, &pTiledGroundInfoSurface[2])))
	{
		D3DXLoadSurfaceFromFile( pTiledGroundInfoSurface[2], 0, 
			NULL, _T("dat\\terrain-infoc.png"), NULL, D3DX_FILTER_NONE, 0, 0);
	}

	if (SUCCEEDED(puma->m_pd3dDevice->CreateImageSurface(128,128, 
		 D3DFMT_A8R8G8B8, &pTiledGroundInfoSurface[3])))
	{
		D3DXLoadSurfaceFromFile( pTiledGroundInfoSurface[3], 0, 
			NULL, _T("dat\\terrain-infod.png"), NULL, D3DX_FILTER_NONE, 0, 0);
	}

	pt = NULL;
	pTexture      = NULL;
	slotArray     = NULL;
	flowerArray   = NULL;

   ZeroMemory( &groundMaterial, sizeof(D3DMATERIAL8) );
   groundMaterial.Diffuse.r = 0.5f;
   groundMaterial.Diffuse.g = 0.5f;
   groundMaterial.Diffuse.b = 0.5f;
   groundMaterial.Diffuse.a = 0.5f;
   groundMaterial.Ambient.r = 0.1f;
   groundMaterial.Ambient.g = 0.1f;
   groundMaterial.Ambient.b = 0.1f;
   groundMaterial.Ambient.a = 0.1f;

	m_pHeightData = 0;

	D3DUtil_InitMaterial(m_MeshMaterial, 0.5f, 0.6f, 0.5f, 1.0f);

	ZeroMemory(m_TerrainTile, sizeof(TILE)*TILE_COUNT*TILE_COUNT);
	ZeroMemory(m_DetailLevel, sizeof(DETAIL_LEVEL)*TOTAL_LEVELS);

	m_FacesDrawn = 0;

	GenerateTiles();
	GenerateDetailLevels();

	// load texture
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\shadow1.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xff000000, NULL, NULL, &shadowBitmap);


	// create vert list
   puma->m_pd3dDevice->CreateVertexBuffer( MAX_LAND_SHADOW_VERTS*sizeof(SHADOWVERTEX),
                                          D3DUSAGE_WRITEONLY, 0,
                                          D3DPOOL_MANAGED, &shadowVBuffer );
	aLog.Log("Done\n");


}

//***************************************************************************************
D3DXVECTOR3 GroundObjectTiles::GetVertPos(int i, int j, int height)
{

	return D3DXVECTOR3( j * CellSize(), height /255.0f * TILED_HEIGHT_COEFF, i * CellSize() );
}

//***************************************************************************************
int GroundObjectTiles::GrassCanGoAt(float x, float y)
{
	// make incloming positions safe
	int tx = GetGridX(x);
	if (tx < 2)
		return FALSE;
	if (tx >= sizeW-2)
		return FALSE;

	int ty = GetGridY(y);
	if (ty < 2)
		return FALSE;
	if (ty >= sizeH-2)
		return FALSE;

	// get info
	D3DCOLOR pixel = TiledGroundInfoTexturePixel(tx, ty);

	if (pixel == water)
		return FALSE;

	if (roadExclusionMap[ty*sizeW+tx] > 20) // if on a road
		return FALSE;


	return TRUE;
}

//***************************************************************************************
D3DCOLOR GroundObjectTiles::GetVertColor(int i, int j, int height)
{
	D3DCOLOR retVal;

	// okay, terrain types:
	//		grass, swamp, water, snow, desert
	D3DCOLOR pixel = TiledGroundInfoTexturePixel(j,i);

	if (pixel == water)
		retVal = D3DCOLOR_RGBA(0,0,100,255) ; // water
	else if (pixel == snow)
		retVal = D3DCOLOR_RGBA(235,235,255,255) ; // snow
	else if (pixel == desert)
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // desert
	else if (pixel == swamp)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else if (pixel == forest)
//		retVal = D3DCOLOR_RGBA(20,80,20,255) ; // grass
		retVal = D3DCOLOR_RGBA(65,74,57,255) ; // grass
	else if (pixel == deepForest)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else if (pixel == waste)
		retVal = D3DCOLOR_RGBA(100,90,70,255) ; // waste
	else
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // beach

	return retVal;
}

//***************************************************************************************
int GroundObjectTiles::GetTerrainTypeOnce(int i, int j)
{
	int retVal;

	OpenTiledGroundInfoTexture();

	D3DCOLOR pixel = TiledGroundInfoTexturePixel(j,i);

	if (pixel == water)
		retVal = 6 ; // water
	else if (pixel == waste)
		retVal = 7 ; // waste
	else if (pixel == snow)
		retVal = 5 ; // snow
	else if (pixel == desert)
		retVal = 4 ; // desert
	else if (pixel == swamp)
		retVal = 3 ; // dark grass
	else if (pixel == forest)
		retVal = 1 ; // grass
	else if (pixel == deepForest)
		retVal = 2 ; // dark grass
	else
		retVal = 6 ; // beach

	CloseTiledGroundInfoTexture();

	return retVal;
}

//***************************************************************************************
int GroundObjectTiles::GetTerrainType(int i, int j)
{
	int retVal;

//	OpenTiledGroundInfoTexture();

	D3DCOLOR pixel = TiledGroundInfoTexturePixel(j,i);

	if (pixel == water)
		retVal = 6 ; // water
	else if (pixel == snow)
		retVal = 5 ; // snow
	else if (pixel == waste)
		retVal = 5 ; // snow
	else if (pixel == desert)
		retVal = 4 ; // desert
	else if (pixel == swamp)
		retVal = 3 ; // dark grass
	else if (pixel == forest)
		retVal = 1 ; // grass
	else if (pixel == deepForest)
		retVal = 2 ; // dark grass
	else
		retVal = 6 ; // beach

//	CloseTiledGroundInfoTexture();

	return retVal;
}

//***************************************************************************************
D3DXVECTOR3 GroundObjectTiles::GetVertNormal(int i, int j)
{

	D3DXVECTOR3 retVal;
	retVal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); 
   D3DXVec3Normalize( &retVal, &retVal );

	return retVal;
}

//***************************************************************************************
void GroundObjectTiles::Generate(LPDIRECT3DDEVICE8 m_pd3dDevice, int w, int h, unsigned long randSeed)
{

	aLog.Log("GroundObjectTiles::Generate ");

	if (0 == randSeed)
		randSeed = rand();

	// *** delete old, valid stuff
   SAFE_DELETE(pt);

	w = h = 256;

	sizeW = w;
	sizeH = h;

	// open up slots for each location
	slotArray = new LocationSlots[sizeW * sizeH];
	for (int i = 0; i < sizeH; ++i)
	{
		for (int j = 0; j < sizeW; ++j)
		{
			for (int k = 0; k < NUM_OF_SLOTS_PER_SPACE; ++k)
			{
				slotArray[i*sizeW+j].used[k] = FALSE;
//				slotArray[i*sizeW+j].toughestMonsterPoints = 0;
			}
		}
	}

	roadExclusionMap = new unsigned char[sizeH * sizeW];

	FILE *fp;

	fp = fopen("dat\\roads-256.raw","rb");
	fread(roadExclusionMap, 1, sizeH * sizeW, fp);
	fclose(fp);

	aLog.Log("Done\n");

}

//***************************************************************************************
void GroundObjectTiles::InitVertArray(LPDIRECT3DDEVICE8 m_pd3dDevice)
{
	aLog.Log("GroundObjectTiles::InitVertArray ");

	ReleaseVertArray();

	aLog.Log("Done\n");

}

//***************************************************************************************
void GroundObjectTiles::ReleaseVertArray(void)
{
}

//***************************************************************************************
void GroundObjectTiles::Draw(int x, int y)
{
	aLog.Log("GroundObjectTiles::Draw  ");

	// calc the minnimim distance for each detail level
	float detail_step = terrainDrawValue;
	float min_level3 = detail_step;
	float min_level2 = detail_step * 2.0f;
	float min_level1 = detail_step * 3.0f;

	// choose detail levels for each tile
	for (int i=0;i<TILE_COUNT;++i)
	{
		for (int j=0;j<TILE_COUNT;++j)
		{
			D3DXVECTOR3 VectorToCamera(m_TerrainTile[i][j].Center - m_CameraPos);
			float DistanceToCamera = D3DXVec3Length(&VectorToCamera);

			if (DistanceToCamera < min_level3)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_3;
			}
			else if (DistanceToCamera < min_level2)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_2;
			}
			else if (DistanceToCamera < min_level1)
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_1;
			}
			else 
			{
				m_TerrainTile[i][j].DetailLevel = LEVEL_0;
			}
		}
	}

//   puma->m_pd3dDevice->SetTexture( 0, NULL );
	if (pTexture)
	{
	   puma->m_pd3dDevice->SetTexture( 0, pTexture );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,     TRUE );
		puma->m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	   puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,      TRUE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );

	}
	puma->m_pd3dDevice->SetMaterial(&groundMaterial);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_FILLMODE , D3DFILL_WIREFRAME );
//	puma->ClearMaterial();

	DrawTerrain();

	aLog.Log("Done\n");

}

//***************************************************************************************
void GroundObjectTiles::DrawFog(void)
{
   puma->m_pd3dDevice->SetTexture( 0, NULL );

	puma->ClearMaterial();

   D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_VIEW , &matWorld );

}

//***************************************************************************************
void GroundObjectTiles::LoadTexture(LPDIRECT3DDEVICE8 m_pd3dDevice, char *fileName)
{

	aLog.Log("GroundObjectTiles::LoadTexture ");

   SAFE_RELEASE(pTexture);

   // Use D3DX to create a texture from a file based image
//   D3DXCreateTextureFromFile( puma->m_pd3dDevice, fileName, &pTexture );
	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, fileName,
							0,0,4,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &pTexture);

	aLog.Log((int) hr);


	aLog.Log(" Done\n");

}

//*******************************************************************************
float GroundObjectTiles::GetXForPoint(int gridX)
{
   return gridX * CellSize();
}

//*******************************************************************************
float GroundObjectTiles::GetYForPoint(int x, int y)
{
	x *= 4;
	if (x < mapOffsetX || x > mapOffsetX + 127)
		return 0;
	y *= 4;
	if (y < mapOffsetY || y > mapOffsetY + 127)
		return 0;

	return TerrainVerts[x - mapOffsetX][y - mapOffsetY].vert.y;

}

//*******************************************************************************
float GroundObjectTiles::GetYForTruePoint(int x, int y)
{
	if (x < mapOffsetX || x > mapOffsetX + 127)
		return 0;
	if (y < mapOffsetY || y > mapOffsetY + 127)
		return 0;

	return HeightAtPoint(x * 10/4.0f, y * 10/4.0f, NULL);
}

//*******************************************************************************
void GroundObjectTiles::SetYForPoint(int x, int y, float newH)
{
   while (x < 0)
      x += sizeW;
   while (y < 0)
      y += sizeH;
   while (x >= sizeW)
      x -= sizeW;
   while (y >= sizeH)
      y -= sizeH;

//   mapData[y * sizeW + x] = (newH + 480 - 80) / MAP_DATA_TILED_HEIGHT_COEFF;
}

//*******************************************************************************
float GroundObjectTiles::GetZForPoint(int gridY)
{
   return gridY * CellSize();
}

//*******************************************************************************
int GroundObjectTiles::GetGridX(float x)
{
   return (int) (x / CellSize());
}

//*******************************************************************************
int GroundObjectTiles::GetGridY(float z)
{
   return (int) (z / CellSize());
}

//*******************************************************************************
float GroundObjectTiles::CellSize(void)
{

   return 10.0f;

}

//*******************************************************************************
float GroundObjectTiles::HeightAtPoint(float pointX, float pointZ, D3DXVECTOR3 *targetNormal)
{
	int x = (int) (pointX / (10/4.0f) - mapOffsetX);
	int y = (int) (pointZ / (10/4.0f) - mapOffsetY);

//	x *= 4;
	if (x < 0 || x > 126)
		return 0;
//	y *= 4;
	if (y < 0 || y > 126)
		return 0;

//   float cellSize = CellSize();

   D3DXVECTOR3 mapPoint, targetPoint, firstTriBase, secondTriBase, triPoint[3];
//   int x2, y2;
   float height = 0.0; //, h1, h2, h3;
   float divisor = 0;

//	int index;
//	float ux1,ux2,uy1,uy2;
	int tVertX1, tVertX2;
	int tVertY1, tVertY2;
	D3DXVECTOR3 tVect[2][2];

	TILE *tile = &m_TerrainTile[x/8][y/8];

	int detailStep = 1 << (3-tile->DetailLevel);

	x /= detailStep;
	x *= detailStep;
	y /= detailStep;
	y *= detailStep;

	tVertX1 = x;
	tVertX2 = x + detailStep;
	if (tVertX2 > 126)
		tVertX2 = 126;

	tVertY1 = y;
	tVertY2 = y + detailStep;
	if (tVertY2 > 126)
		tVertY2 = 126;

	tVect[0][0] = TerrainVerts[tVertY1][tVertX1].vert;
	tVect[1][0] = TerrainVerts[tVertY2][tVertX1].vert;
	tVect[0][1] = TerrainVerts[tVertY1][tVertX2].vert;
	tVect[1][1] = TerrainVerts[tVertY2][tVertX2].vert;

   // normalize targetPoint
//   targetPoint.x = (pointX - (x+mapOffsetX) * (10/4.0f)) / (10/4.0f);//cellSize;
//   targetPoint.z = (pointZ - (y+mapOffsetY) * (10/4.0f)) / (10/4.0f);//cellSize;
	if (0 != tVect[1][1].x - tVect[0][0].x)
	   targetPoint.x = (pointX - tVect[0][0].x) / (tVect[1][1].x - tVect[0][0].x);
	else
	   targetPoint.x = pointX;

	if (0 != tVect[1][1].z - tVect[0][0].z)
	   targetPoint.z = (pointZ - tVect[0][0].z) / (tVect[1][1].z - tVect[0][0].z);
	else
	   targetPoint.z = pointZ;

	float lowZ  = tVect[0][0].y   * (1.0f - targetPoint.z) +
		           tVect[1][0].y   * targetPoint.z;
	float highZ = tVect[0][1].y   * (1.0f - targetPoint.z) +
		           tVect[1][1].y   * targetPoint.z;

	float midX  = lowZ * (1.0f - targetPoint.x) + highZ * targetPoint.x;

   if (targetNormal)
	{
	//   if (targetPoint.x + targetPoint.z > 1.0)
	   if ((1.0 - targetPoint.z) + targetPoint.x <= 1.0)
//		  if ((1.0-targetPoint.x) + (1.0-targetPoint.z) <= 1.0)
	   {
		   triPoint[0] = tVect[0][0];
			triPoint[1] = tVect[0][1];
		   triPoint[2] = tVect[1][0];
		}
	   else
		{
			triPoint[0] = tVect[1][1];
		   triPoint[1] = tVect[1][0];
		   triPoint[2] = tVect[0][1];
	   }

	   D3DXVECTOR3 normal = CalculateNormal(triPoint[0], triPoint[1], triPoint[2]);
	   *targetNormal = normal;
	}
	return midX;

//   return GetGroundHeight(triPoint[0], triPoint[1], triPoint[2], normal, pointX, pointZ);

}
//*******************************************************************************
// circle of 4, then offset circle of 4, then circle of 8, then circle of 12
void GroundObjectTiles::GetSlotPosition(int slotIndex, float &x, float &y, float &ang)
{
	float radius = CellSize() *0.1f;
	if (slotIndex > 3)
		radius = CellSize() *0.2f;
	if (slotIndex > 7)
		radius = CellSize() *0.3f;
	if (slotIndex > 15)
		radius = CellSize() *0.4f;

	float angle = 0;
	if (slotIndex > 15)
		angle = (float)(slotIndex-16) / 12 * D3DX_PI * 2;
	else if (slotIndex > 7)
		angle = (float)(slotIndex-8) / 8 * D3DX_PI * 2;
	else if (slotIndex > 3)
		angle = (float)(slotIndex-4) / 4 * D3DX_PI * 2 + D3DX_PI/4;
	else
		angle = (float)(slotIndex) / 4 * D3DX_PI * 2;

	x = CellSize()/2.0f + (float)sin(angle) * radius;
	y = CellSize()/2.0f + (float)cos(angle) * radius;

	ang = -1 * angle;

}

//*******************************************************************************
int GroundObjectTiles::GetFirstOpenSlot(int x, int y, int backwards)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	if (!backwards)
	{
		for (int i = 0; i < NUM_OF_SLOTS_PER_SPACE; ++i)
		{
			if (!slot->used[i])
				return i;
		}
	}
	else
	{
		for (int i = NUM_OF_SLOTS_PER_SPACE - 1; i >= 0; --i)
		{
			if (!slot->used[i])
				return i;
		}
	}

	return -1;
}

//*******************************************************************************
void GroundObjectTiles::ClaimSlot(int x, int y, int index, int type)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	slot->used[index] = type;
}

//*******************************************************************************
void GroundObjectTiles::ReleaseSlot(int x, int y, int index)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	slot->used[index] = FALSE;
}

//*******************************************************************************
void GroundObjectTiles::CreateStaticPositions(void)
{
	srand(1);

	// add town buildings
	for (int t = 0; t < NUM_OF_TOWNS; ++t)
	{
		for (int t2 = 0; t2 < townList[t].size * 2; ++t2)
		{
			LocationSlots *slot;
			int x,y;
			do
			{
				x = townList[t].x + (rand() % (3 + townList[t].size)) - townList[t].size/2;
				y = townList[t].y + (rand() % (3 + townList[t].size)) - townList[t].size/2;
				if (x < 1)
					x = 1;
				if (y < 1)
					y = 1;
				if (x > 255)
					x = 255;
				if (y > 255)
					y = 255;
				slot = &(slotArray[y*sizeW+x]);
			} while ((x == townList[t].x && y == townList[t].y) ||
						slot->used[NUM_OF_SLOTS_PER_SPACE- 1]> 0 ||
						roadExclusionMap[y*sizeW+x] > 0);

			slot = &(slotArray[y*sizeW+x]);
//			slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_CABIN + (rand() % 2);
			slot->used[NUM_OF_SLOTS_PER_SPACE- 1] = SLOT_CABIN + (rand() % 2);
		}
	}

	OpenTiledGroundInfoTexture();

	for (int y = 0; y < sizeH; ++y)
	{
		for (int x = 0; x < sizeW; ++x)
		{
			if (y >= 2 && y < sizeH-2 && x >= 2 && x < sizeW-2 &&
			    roadExclusionMap[y*sizeW+x] <= 20) // if not on a road
			{

				LocationSlots *slot = &(slotArray[y*sizeW+x]);

				D3DCOLOR pixel = TiledGroundInfoTexturePixel(x,y);

				if (pixel == water)
					; // add nothing
				else if (pixel == waste)
				{
					if (!(rand() % 7))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_COLUMN;
				}
				else if (pixel == snow)
				{
					if (!(rand() % 7))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_COLUMN;
					if (!(rand() % 3))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TUNDRA_TREE1 + (rand() % 2);
				}
				else if (pixel == desert)
				{
					if (!(rand() % 7))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_COLUMN;
					if (!(rand() % 6))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_DESERT_TREE1 + (rand() % 2);
				}
				else if (pixel == swamp)
				{
					slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_SWAMP_TREE1 + (rand() % 2);
					if (!(rand() % 3))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_SWAMP_TREE1 + (rand() % 2);
				}
				else if (pixel == forest)
				{
	//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
					if (!(rand() % 8))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_COLUMN;
					if (!(rand() % 3))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_FOREST_TREE1 + (rand() % 4);
					if (!(rand() % 43))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_SWAMP_TREE1 + (rand() % 2);
				}
				else if (pixel == deepForest)
				{
					slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_FOREST_TREE1 + (rand() % 4);
					if (!(rand() % 2))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_FOREST_TREE1 + (rand() % 4);
					else if (!(rand() % 2))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_GIANT_MUSHROOM;
	//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
	//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
	//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
				}
				else // must be beach!
				{
					if (!(rand() % 2))
						slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_BEACH_TREE1;
				}
			}

		}
	}

	flowerArray = new FlowerRecord[sizeW * sizeH];
	for (int i = 0; i < sizeH; ++i)
	{
		for (int j = 0; j < sizeW; ++j)
		{
			flowerArray[i*sizeW+j].type = 0; // nothing

			if (i >= 2 && i < sizeH-2 && j >= 2 && j < sizeW-2 &&
				6 == (rand() % 10) && roadExclusionMap[i*sizeW+j] <= 20) // if not on a road
			{
				int gType = GetTerrainType(i, j);

				if (3 == gType || 2 == gType || 1 == gType)
				{
					flowerArray[i*sizeW+j].type = (rand() % 5) + 1; // 4 flowers plus mushrooms
					if (2 == gType && 4 == flowerArray[i*sizeW+j].type)
						flowerArray[i*sizeW+j].type = 5;
					if (1 == gType && 4 == flowerArray[i*sizeW+j].type)
						flowerArray[i*sizeW+j].type = 1;
					if (3 == gType && 4 == flowerArray[i*sizeW+j].type)
						flowerArray[i*sizeW+j].type = 2;
					flowerArray[i*sizeW+j].x    = rand() % 256;
					flowerArray[i*sizeW+j].y    = rand() % 256;
					flowerArray[i*sizeW+j].amount = (rand() % 70) + 20;
				}
				if (6 == gType)
				{
					flowerArray[i*sizeW+j].type = 4; // 4 flowers plus mushrooms
					flowerArray[i*sizeW+j].x    = rand() % 256;
					flowerArray[i*sizeW+j].y    = rand() % 256;
					flowerArray[i*sizeW+j].amount = (rand() % 70) + 20;
				}
			}
		}
	}

	CloseTiledGroundInfoTexture();

	// add special static items

	BuildRing(230, 53, SLOT_FOREST_TREE1);
	BuildRing( 20, 23, SLOT_COLUMN);
	BuildRing(179,164, SLOT_COLUMN);
	BuildRing(107,162, SLOT_COLUMN);

	// pet graveyard 58 88 and 70 30
	BuildJumble(90, 53,SLOT_PETHEADSTONE1);
	BuildJumble(71,206,SLOT_PETHEADSTONE1);
//	ClearStaticPositionsFor(int x, int y);

  // 63 73
  // 23 29
  // 77 27
  // 15 115


	srand(timeGetTime());

}


//*******************************************************************************
void GroundObjectTiles::BuildRing(int x, int y, int objectType)
{
	for (int i = 0; i < sizeH; ++i)
	{
		for (int j = 0; j < sizeW; ++j)
		{
			float dist = Distance(j,i,x,y);
			if (dist < 2.8f)
				ClearStaticPositionsFor(j,i);
		}
	}

	LocationSlots *slot = &(slotArray[(y-1)*sizeW+(x)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 4 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 5 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 6 - 1] = objectType;

	slot = &(slotArray[(y-1)*sizeW+(x+1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 6 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 7 - 1] = objectType;

	slot = &(slotArray[(y)*sizeW+(x+1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 7 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 8 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 9 - 1] = objectType;

	slot = &(slotArray[(y+1)*sizeW+(x+1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 9 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 10 - 1] = objectType;

	slot = &(slotArray[(y+1)*sizeW+(x)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 10 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 11 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 0 - 1] = objectType;

	slot = &(slotArray[(y+1)*sizeW+(x-1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 0 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 1 - 1] = objectType;

	slot = &(slotArray[(y)*sizeW+(x-1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 1 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 2 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 3 - 1] = objectType;

	slot = &(slotArray[(y-1)*sizeW+(x-1)]);
	slot->used[NUM_OF_SLOTS_PER_SPACE - 3 - 1] = objectType;
	slot->used[NUM_OF_SLOTS_PER_SPACE - 4  - 1] = objectType;

}

//*******************************************************************************
void GroundObjectTiles::BuildJumble(int x, int y, int objectType)
{
	for (int i = y; i <= y+1; ++i)
	{
		for (int j = x; j <= x+1; ++j)
		{
			ClearStaticPositionsFor(j,i);

			LocationSlots *slot = &(slotArray[(i)*sizeW+(j)]);
			for (int k = 0; k < 10; ++k)
			{
				slot->used[rand() % NUM_OF_SLOTS_PER_SPACE] = objectType + (rand() % 2);
			}
		}
	}
}

//*******************************************************************************
void GroundObjectTiles::ClearStaticPositionsFor(int x, int y)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	for (int i = 0; i < NUM_OF_SLOTS_PER_SPACE; ++i)
	{
		slot->used[i] = 0;
	}

	flowerArray[y*sizeW+x].type = 0; // nothing

}

//*******************************************************************************
void GroundObjectTiles::DrawTerrain()
{
	m_FacesDrawn = 0;

	if (SUCCEEDED(puma->m_pd3dDevice->SetVertexShader(FVF)))
	{
		// render each tile
		for (int i=0;i<TILE_COUNT;++i)
		{
			for (int j=0;j<TILE_COUNT;++j)
			{
				float dCamX = m_TerrainTile[i][j].Center.x - m_CameraPos.x;
				float dCamY = m_TerrainTile[i][j].Center.z - m_CameraPos.z;
				float dCamAngle = (float)atan2(-dCamX, -dCamY);
				dCamAngle   = (float)NormalizeAngle(dCamAngle);
				dCamAngle  += m_CameraAngle;
				dCamAngle   = (float)NormalizeAngle2(dCamAngle);

				if (fabs(dCamAngle) < (D3DX_PI/3) || (fabs(dCamX) < 26 && fabs(dCamY) < 26))
				{

					// activate the vertex buffer for this tile
					if (SUCCEEDED(puma->m_pd3dDevice->SetStreamSource(0,m_TerrainTile[i][j].VBuffer, sizeof(TERRAIN_VERTEX))))
					{
						int body_tile = 0;
						LEVEL MyLevel = m_TerrainTile[i][j].DetailLevel;

						// examine the tile above this tile
						if (i && m_TerrainTile[i-1][j].DetailLevel < MyLevel)
						{
							LEVEL ThisLevel = m_TerrainTile[i-1][j].DetailLevel;
							body_tile |= 1<<TOP;

							// draw the connecting piece needed
							if (SUCCEEDED(puma->m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].pIndexBuffer, 0)))
							{
								puma->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].TriangleCount);
								m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[TOP][ThisLevel].TriangleCount;
							}
						}

						// examine the tile below this tile
						if (i<(TILE_COUNT-1) && m_TerrainTile[i+1][j].DetailLevel < MyLevel)
						{
							LEVEL ThisLevel = m_TerrainTile[i+1][j].DetailLevel;
							body_tile |= 1<<BOTTOM;

							// draw the connecting piece needed
							if (SUCCEEDED(puma->m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].pIndexBuffer, 0)))
							{
								puma->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].TriangleCount);
								m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[BOTTOM][ThisLevel].TriangleCount;
							}
						}

						// examine the tile to the left this tile
						if (j && m_TerrainTile[i][j-1].DetailLevel < MyLevel)
						{
							LEVEL ThisLevel = m_TerrainTile[i][j-1].DetailLevel;
							body_tile |= 1<<LEFT;

							// draw the connecting piece needed
							if (SUCCEEDED(puma->m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].pIndexBuffer, 0)))
							{
								puma->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].TriangleCount);
								m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[LEFT][ThisLevel].TriangleCount;
							}
						}

						// examine the tile to the right this tile
						if (j<(TILE_COUNT-1) && m_TerrainTile[i][j+1].DetailLevel < MyLevel)
						{
							LEVEL ThisLevel = m_TerrainTile[i][j+1].DetailLevel;
							body_tile |= 1<<RIGHT;

							// draw the connecting piece needed
							if (SUCCEEDED(puma->m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].pIndexBuffer, 0)))
							{
								puma->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].TriangleCount);
								m_FacesDrawn += m_DetailLevel[MyLevel].TileConnectors[RIGHT][ThisLevel].TriangleCount;
							}
						}

						// finally, draw the body tile needed
						if (m_DetailLevel[MyLevel].TileBodies[body_tile].pIndexBuffer)
						{
							if (SUCCEEDED(puma->m_pd3dDevice->SetIndices(m_DetailLevel[MyLevel].TileBodies[body_tile].pIndexBuffer, 0)))
							{
								puma->m_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 81, 0, m_DetailLevel[MyLevel].TileBodies[body_tile].TriangleCount);
								m_FacesDrawn += m_DetailLevel[MyLevel].TileBodies[body_tile].TriangleCount;
							}
						}
					}
				}
			}
		}
	}
}



//*******************************************************************************
void GroundObjectTiles::UpdateTilesIfNeeded(int oX, int oY)
{
	int aX = oX * 4;
	int aY = oY * 4;


	if (abs(aX - (mapOffsetX+64)) > 30)
	{
		if (0 == mapOffsetX && aX <= mapOffsetX + 64)
			;
		else if (1024 - 128 == mapOffsetX && aX > 1024 - 64)
			;
		else
		{
			GenerateTiles(oX, oY);
			GenerateDetailLevels();
			return;
		}
	}
	if (abs(aY - (mapOffsetY+64)) > 30)
	{
		if (0 == mapOffsetY && aY <= mapOffsetY + 64)
			;
//		else if (512 - 128 == mapOffsetY && aY > mapOffsetY - 64)
		else if (1024 - 128 == mapOffsetY && aY > 1024 - 64)
			;
		else
		{
			GenerateTiles(oX, oY);
			GenerateDetailLevels();
		}
	}
	
};

LPDIRECT3DSURFACE8	tileMapRoadData;			// greyscale road color information

//*******************************************************************************
void GroundObjectTiles::GenerateTiles(int oX, int oY)
{
	aLog.Log("GroundObjectTiles::GenerateTiles ");

	OpenTiledGroundInfoTexture();
	// visible space is 32x32 game squares


	// oX and oY in game spaces

	// offsets in verts
	mapOffsetX = oX * 4 - VERTEX_COUNT/2;
	if (mapOffsetX < 0)
		mapOffsetX = 0;
	if (mapOffsetX > 1024 - 128)
		mapOffsetX = 1024 - 128;

	mapOffsetY = oY * 4 - VERTEX_COUNT/2;
	if (mapOffsetY < 0)
		mapOffsetY = 0;
	if (mapOffsetY > 1024 - 128)
		mapOffsetY = 1024 - 128;

	RECT src,dst;
	src.left   = mapOffsetX;
	src.top    = mapOffsetY;
	src.right  = mapOffsetX + 128;
	src.bottom = mapOffsetY + 128;

	dst.left  = dst.top    = 0;
	dst.right = dst.bottom = 128;
	 
	SAFE_RELEASE(m_pHeightData);

	// release the detail levels
	int i,j,k,l;
	for (i=0;i<TOTAL_LEVELS;++i)
	{
		for (j=0;j<16;++j)
		{
			SAFE_RELEASE(m_DetailLevel[i].TileBodies[j].pIndexBuffer);
		}
		for (k=0;k<TOTAL_SIDES;++k)
		{
			for (l=0;l<TOTAL_LEVELS;++l)
			{
				SAFE_RELEASE(m_DetailLevel[i].TileConnectors[k][l].pIndexBuffer);
			}
		}
	}

	// release the tiles
	for (i=0;i<TILE_COUNT;++i)
	{
		for (j=0;j<TILE_COUNT;++j)
		{
			SAFE_RELEASE(m_TerrainTile[i][j].VBuffer);
		}
	}

	puma->m_pd3dDevice->CreateImageSurface(129,129, D3DFMT_R8G8B8, &tileMapRoadData);

	if (SUCCEEDED(puma->m_pd3dDevice->CreateImageSurface(129,129, D3DFMT_R8G8B8, &m_pHeightData)))
	{
		RECT src2[4],dst2[4];

		for (int i = 0; i < 4; ++i)
		{
			src2[i] = src;
			dst2[i] = dst;
		}

		if (src.right > 511)
		{
			src2[0].right -= (src.right - 512);
			dst2[0].right -= (src.right - 512);

			src2[2].right -= (src.right - 512);
			dst2[2].right -= (src.right - 512);
		}
		if (src.left < 512)
		{
			src2[1].left += (512 - src.left);
			dst2[1].left += (512 - src.left);
								        
			src2[3].left += (512 - src.left);
			dst2[3].left += (512 - src.left);
		}
		if (src.bottom > 511)
		{
			src2[0].bottom -= (src.bottom - 512);
			dst2[0].bottom -= (src.bottom - 512);

			src2[1].bottom -= (src.bottom - 512);
			dst2[1].bottom -= (src.bottom - 512);
		}
		if (src.top < 512)
		{
			src2[2].top += (512 - src.top);
			dst2[2].top += (512 - src.top);
								        
			src2[3].top += (512 - src.top);
			dst2[3].top += (512 - src.top);
		}

		if (src2[0].left < src2[0].right && src2[0].top < src2[0].bottom)
			D3DXLoadSurfaceFromFile( m_pHeightData, 0, &dst2[0], 
			                        _T("dat\\terrain-512.png"), &src2[0], D3DX_FILTER_NONE, 0, 0);
		if (src2[1].left < src2[1].right && src2[1].top < src2[1].bottom)
		{
			src2[1].left   -= 512;
			src2[1].right  -= 512;
			D3DXLoadSurfaceFromFile( m_pHeightData, 0, &dst2[1], 
			                        _T("dat\\terrain-512b.png"), &src2[1], D3DX_FILTER_NONE, 0, 0);
		}
		if (src2[2].left < src2[2].right && src2[2].top < src2[2].bottom)
		{
			src2[2].top    -= 512;
			src2[2].bottom -= 512;
			D3DXLoadSurfaceFromFile( m_pHeightData, 0, &dst2[2], 
			                        _T("dat\\terrain-512c.png"), &src2[2], D3DX_FILTER_NONE, 0, 0);
		}
		if (src2[3].left < src2[3].right && src2[3].top < src2[3].bottom)
		{
			src2[3].left   -= 512;
			src2[3].right  -= 512;
			src2[3].top    -= 512;
			src2[3].bottom -= 512;
			D3DXLoadSurfaceFromFile( m_pHeightData, 0, &dst2[3], 
			                        _T("dat\\terrain-512d.png"), &src2[3], D3DX_FILTER_NONE, 0, 0);
		}

		if (src2[0].left < src2[0].right && src2[0].top < src2[0].bottom)
			D3DXLoadSurfaceFromFile( tileMapRoadData, 0, &dst2[0], 
			                        _T("dat\\roads-512.png"), &src2[0], D3DX_FILTER_NONE, 0, 0);
		if (src2[1].left < src2[1].right && src2[1].top < src2[1].bottom)
			D3DXLoadSurfaceFromFile( tileMapRoadData, 0, &dst2[1], 
			                        _T("dat\\roads-512b.png"), &src2[1], D3DX_FILTER_NONE, 0, 0);
		if (src2[2].left < src2[2].right && src2[2].top < src2[2].bottom)
			D3DXLoadSurfaceFromFile( tileMapRoadData, 0, &dst2[2], 
			                        _T("dat\\roads-512c.png"), &src2[2], D3DX_FILTER_NONE, 0, 0);
		if (src2[3].left < src2[3].right && src2[3].top < src2[3].bottom)
			D3DXLoadSurfaceFromFile( tileMapRoadData, 0, &dst2[3], 
			                        _T("dat\\roads-512d.png"), &src2[3], D3DX_FILTER_NONE, 0, 0);
	}
	//
	// Generate tiles based on the values in the height map
	//

//	int i,j;
	float x= 0;
	float y= 0;

	// lock the height map
	D3DLOCKED_RECT LockedRect;
	D3DLOCKED_RECT LockedRect2;

	int safe = TRUE;
		if (!m_pHeightData)
			safe = FALSE;
		if (!tileMapRoadData)
			safe = FALSE;

	if (safe)
	{
		if (!SUCCEEDED(m_pHeightData->LockRect(&LockedRect , 0, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY )))
			safe = FALSE;
		if (!SUCCEEDED(tileMapRoadData->LockRect  (&LockedRect2, 0, D3DLOCK_NO_DIRTY_UPDATE|D3DLOCK_NOSYSLOCK|D3DLOCK_READONLY )))
			safe = FALSE;
	}

	if (safe)
	{
		BYTE* pNextRow     = (BYTE*)LockedRect.pBits;
		BYTE* pNextRoadRow = (BYTE*)LockedRect2.pBits;

		for (i=0;i<VERTEX_COUNT;++i)
		{
			BYTE* pPixel = pNextRow;
			pNextRow = pPixel + LockedRect.Pitch;

			BYTE* pRoadPixel = pNextRoadRow;
			pNextRoadRow = pRoadPixel + LockedRect2.Pitch;

			for (j=0;j<VERTEX_COUNT;++j)
			{
				BYTE height_value = *pPixel;
				float height = (float)height_value /255.0f * TILED_HEIGHT_COEFF;
				pPixel += 3; // assuming a 24 bit heightmap

				// set the vertex position
				TerrainVerts[i][j].vert.x = x + mapOffsetX * 10.0f/4;
				TerrainVerts[i][j].vert.y = height + *pRoadPixel / 512.0f;
				TerrainVerts[i][j].vert.z = y + mapOffsetY * 10.0f/4;

				// set the texture coordinates
				TerrainVerts[i][j].tu = (i * 2.0f);
				TerrainVerts[i][j].tv = (j * 2.0f);

				// set the color
				TerrainVerts[i][j].color = GetTileGroundColor(
					       (i+mapOffsetY), (j+mapOffsetX), *pRoadPixel);
				pRoadPixel += 3; // assuming a 24 bit heightmap

				if ((0 == j && 0 == mapOffsetX)   ||
					 (0 == i && 0 == mapOffsetY)   ||
					 (127 == j && 896 == mapOffsetX) ||
					 (128 == i && 896 == mapOffsetY))
				{
					TerrainVerts[i][j].color   = 0xffffffff;
					TerrainVerts[i][j].vert.y += 5 + rnd(-1,1);
				}

				// calc the normal from the last pixel and row
				D3DXVECTOR3 RowVector(-1.0f, 0.0f, 0.0f);
				D3DXVECTOR3 CollumnVector(0.0f, -1.0f, 0.0f);
				D3DXVECTOR3 TempNorm;
				if (i)
				{
					RowVector = TerrainVerts[i-1][j].vert - TerrainVerts[i][j].vert;
				}
				if (j)
				{
					CollumnVector = TerrainVerts[i][j-1].vert - TerrainVerts[i][j].vert;
				}
				D3DXVec3Cross(&TempNorm, &RowVector, &CollumnVector);
				D3DXVec3Normalize(&TerrainVerts[i][j].norm, &TempNorm);
				x += 10.0f/4;
			}
			y += 10.0f/4;
			x= 0;
		}

		tileMapRoadData->UnlockRect();
		SAFE_RELEASE(tileMapRoadData);

		// unlock the height map
		m_pHeightData->UnlockRect();

		// now create the vertex buffers from the global vertex data
		int center_vertex = TILE_VERTS>>1;
		for (i=0;i<TILE_COUNT;++i)
		{
			int verty = i*(TILE_VERTS-1);

			for (j=0;j<TILE_COUNT;++j)
			{
				int vertx = j*(TILE_VERTS-1);

				// create a vertex buffer for this tile
				m_TerrainTile[i][j].DetailLevel = LEVEL_3;  // was LEVEL_0
				m_TerrainTile[i][j].VBuffer = 0;

				if (SUCCEEDED(puma->m_pd3dDevice->CreateVertexBuffer(sizeof(TERRAIN_VERTEX)*TILE_VERTS*TILE_VERTS, D3DUSAGE_WRITEONLY, FVF, D3DPOOL_MANAGED, &m_TerrainTile[i][j].VBuffer)))
				{
					TERRAIN_VERTEX *pData;

					if (SUCCEEDED(m_TerrainTile[i][j].VBuffer->Lock(0,0, (BYTE**)&pData, D3DLOCK_NOSYSLOCK)))
					{
						for (int y=0;y<TILE_VERTS;++y)
						{
							for (int x=0;x<TILE_VERTS;++x)
							{
								// if this is the center of the tile, store it for distance checking
								if (y==center_vertex && x==center_vertex)
								{
									m_TerrainTile[i][j].Center = TerrainVerts[vertx+x][verty+y].vert;
								}

								// copy the vertex to our buffer
								memcpy(pData, &TerrainVerts[vertx+x][verty+y], sizeof(TERRAIN_VERTEX));
								++pData;
							}
						}
						m_TerrainTile[i][j].VBuffer->Unlock();
					}
				}
			}
		}
	}

	CloseTiledGroundInfoTexture();

	aLog.Log(" Done\n");

}



//*******************************************************************************
void GroundObjectTiles::GenerateDetailLevels()
{
	//
	// Generate Tile Bodies and Connectors for our 4 sample detail levels
	//

	//
	// Detail Level 0 (lowest detail level)
	//

	m_DetailLevel[0].TileBodies[0].pIndexBuffer=0;
	if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(6*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[0].TileBodies[0].pIndexBuffer)))
	{
		WORD* pIndex;
		if (SUCCEEDED(m_DetailLevel[0].TileBodies[0].pIndexBuffer->Lock(0,6*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
		{
			pIndex[0] = BaseTile0[0];
			pIndex[1] = BaseTile0[1];
			pIndex[2] = BaseTile0[2];
			pIndex[3] = BaseTile0[3];
			pIndex[4] = BaseTile0[4];
			pIndex[5] = BaseTile0[5];

			m_DetailLevel[0].TileBodies[0].pIndexBuffer->Unlock();
			m_DetailLevel[0].TileBodies[0].IndexCount = 6;
			m_DetailLevel[0].TileBodies[0].TriangleCount = 2;
		}
	}

	//
	// Detail Level 1
	//

	// create each of the 16 tile bodies
	for (int body=0;body<16;++body)
	{
		m_DetailLevel[1].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[1].TileBodies[body].IndexCount = 0;

		int total_indexes=0;
		if (!(body & (1<<0))) total_indexes += 6;
		if (!(body & (1<<1))) total_indexes += 6;
		if (!(body & (1<<2))) total_indexes += 6;
		if (!(body & (1<<3))) total_indexes += 6;

		if (total_indexes)
		{
			if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[1].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[1].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<6;++data)
							{
								pIndex[index++] = SidesOfLevel1[side][data];
							}
						}
					}
					m_DetailLevel[1].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[1].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[1].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (int side=0;side<TOTAL_SIDES;++side)
	{
		m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(3*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer->Lock(0,3*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<3;++count)
				{
					pIndex[count] = Connect1to0[side][count];
				}
				m_DetailLevel[1].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[1].TileConnectors[side][0].IndexCount = 3;
				m_DetailLevel[1].TileConnectors[side][0].TriangleCount = 1;
			}
		}
	}

	//
	// Detail Level 2
	//

	// create each of the 16 tile bodies
	for (int body=0;body<16;++body)
	{
		m_DetailLevel[2].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[2].TileBodies[body].IndexCount = 0;

		int total_indexes=24;
		if (!(body & (1<<0))) total_indexes += 18;
		if (!(body & (1<<1))) total_indexes += 18;
		if (!(body & (1<<2))) total_indexes += 18;
		if (!(body & (1<<3))) total_indexes += 18;

		if (total_indexes)
		{
			if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[2].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					// start by copying the center portion of the tile
					for (int center_vert=0;center_vert<24;++center_vert)
					{
						pIndex[index++] = Level2_Center[center_vert];
					}

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<18;++data)
							{
								pIndex[index++] = SidesOfLevel2[side][data];
							}
						}
					}
					m_DetailLevel[2].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[2].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[2].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (int side=0;side<TOTAL_SIDES;++side)
	{
		// connections to detail level 0
		m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(9*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer->Lock(0,9*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<9;++count)
				{
					pIndex[count] = Connect2to0[side][count];
				}
				m_DetailLevel[2].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[2].TileConnectors[side][0].IndexCount = 9;
				m_DetailLevel[2].TileConnectors[side][0].TriangleCount = 3;
			}
		}

		// connections to detail level 1
		m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(12*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer->Lock(0,12*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<12;++count)
				{
					pIndex[count] = Connect2to1[side][count];
				}
				m_DetailLevel[2].TileConnectors[side][1].pIndexBuffer->Unlock();
				m_DetailLevel[2].TileConnectors[side][1].IndexCount = 12;
				m_DetailLevel[2].TileConnectors[side][1].TriangleCount = 4;
			}
		}
	}

	//
	// Detail Level 3
	//

	// create each of the 16 tile bodies
	for (int body=0;body<16;++body)
	{
		m_DetailLevel[3].TileBodies[body].pIndexBuffer=0;
		m_DetailLevel[3].TileBodies[body].IndexCount = 0;

		int total_indexes=216;
		if (!(body & (1<<0))) total_indexes += 42;
		if (!(body & (1<<1))) total_indexes += 42;
		if (!(body & (1<<2))) total_indexes += 42;
		if (!(body & (1<<3))) total_indexes += 42;

		if (total_indexes)
		{
			if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(total_indexes*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileBodies[body].pIndexBuffer)))
			{
				WORD* pIndex;
				if (SUCCEEDED(m_DetailLevel[3].TileBodies[body].pIndexBuffer->Lock(0,total_indexes*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
				{
					int index=0;

					// start by copying the center portion of the tile
					for (int center_vert=0;center_vert<216;++center_vert)
					{
						pIndex[index++] = Level3_Center[center_vert];
					}

					for (int side=0;side<TOTAL_SIDES;++side)
					{
						if (!(body & (1<<side)))
						{
							for (int data=0;data<42;++data)
							{
								pIndex[index++] = SidesOfLevel3[side][data];
							}
						}
					}
					m_DetailLevel[3].TileBodies[body].pIndexBuffer->Unlock();
					m_DetailLevel[3].TileBodies[body].IndexCount = total_indexes;
					m_DetailLevel[3].TileBodies[body].TriangleCount = total_indexes/3;
				}
			}
		}
	}

	// create the tile connectors
	for (int side=0;side<TOTAL_SIDES;++side)
	{
		// connections to detail level 0
		m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(21*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer->Lock(0,21*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<21;++count)
				{
					pIndex[count] = Connect3to0[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][0].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][0].IndexCount = 21;
				m_DetailLevel[3].TileConnectors[side][0].TriangleCount = 7;
			}
		}

		// connections to detail level 1
		m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(24*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer->Lock(0,24*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<24;++count)
				{
					pIndex[count] = Connect3to1[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][1].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][1].IndexCount = 24;
				m_DetailLevel[3].TileConnectors[side][1].TriangleCount = 8;
			}
		}

		// connections to detail level 2
		m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer = 0;
		if (SUCCEEDED(puma->m_pd3dDevice->CreateIndexBuffer(30*2, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16 , D3DPOOL_MANAGED, &m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer)))
		{
			WORD* pIndex;
			if (SUCCEEDED(m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer->Lock(0,30*2, (BYTE**)&pIndex, D3DLOCK_NOSYSLOCK)))
			{
				int index=0;

				for (int count=0;count<30;++count)
				{
					pIndex[count] = Connect3to2[side][count];
				}
				m_DetailLevel[3].TileConnectors[side][2].pIndexBuffer->Unlock();
				m_DetailLevel[3].TileConnectors[side][2].IndexCount = 30;
				m_DetailLevel[3].TileConnectors[side][2].TriangleCount = 10;
			}
		}
	}
}


//***************************************************************************************
D3DCOLOR GroundObjectTiles::GetTileGroundColor(int i, int j, int roadValue)
{
	D3DCOLOR retVal;

	srand(i*512+j);

	int altI = i/4;
	if (altI < 1)
		altI = 1;
	if (altI > 255)
		altI = 255;

	int altJ = j/4;
	if (altJ < 1)
		altJ = 1;
	if (altJ > 255)
		altJ = 255;


	i += (rand() % 3) - 1;
	i /= 4;
	if (i < 1)
		i = 1;
	if (i > 255)
		i = 255;

	j += (rand() % 3) - 1;
	j /= 4;
	if (j < 1)
		j = 1;
	if (j > 255)
		j = 255;

	D3DCOLOR pixel = TiledGroundInfoTexturePixel(j,i);

	int darkRoad = FALSE;

	if (pixel == water)
	{
		pixel = TiledGroundInfoTexturePixel(altJ, altI);
		if (pixel == water)
			retVal = D3DCOLOR_RGBA(0,0,100,255) ; // water
		else
			retVal = D3DCOLOR_RGBA(190,160,70,255) ; // beach
	}
	else if (pixel == snow)
	{
		retVal = D3DCOLOR_RGBA(235,235,255,255) ; // snow
		darkRoad = TRUE;
	}
	else if (pixel == desert)
	{
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // desert
		darkRoad = TRUE;
	}
	else if (pixel == swamp)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else if (pixel == forest)
//		retVal = D3DCOLOR_RGBA(20,80,20,255) ; // grass
		retVal = D3DCOLOR_RGBA(42,77,33,255) ; // grass
	else if (pixel == deepForest)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else if (pixel == waste)
		retVal = D3DCOLOR_RGBA(100,90,70,255) ; // waste
	else
	{
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // beach
		darkRoad = TRUE;
	}
	D3DXCOLOR bigColor = retVal;
	D3DXCOLOR roadColor = D3DCOLOR_RGBA(220,220,200,255);
	if (darkRoad)
		roadColor = D3DCOLOR_RGBA(70,70,40,255);

	D3DXCOLOR resultColor;
	float roadf = roadValue / 255.0f;

	resultColor.a = 1;
	resultColor.r = bigColor.r * (1 - roadf) + roadColor.r * roadf;
	resultColor.g = bigColor.g * (1 - roadf) + roadColor.g * roadf;
	resultColor.b = bigColor.b * (1 - roadf) + roadColor.b * roadf;

	retVal = resultColor;


	return retVal;
}


//******************************************************************
void GroundObjectTiles::StartFrame(void)
{
	shadowTriIndex = 0;
}

//******************************************************************
void GroundObjectTiles::AddShadow(float x, float y, float scale)
{

	if (shadowTriIndex >= MAX_LAND_SHADOW_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	SHADOWVERTEX *pVertices;
	if( FAILED( shadowVBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// figure out the extents of the shadow square
	float leastPosX = x - scale;
	float leastPosY = y - scale;
	float mostPosX  = x + scale;
	float mostPosY  = y + scale;

	int leastGridX = (GetGridX(leastPosX) - mapOffsetX/4)/2;
	if (leastGridX < 0)
		leastGridX = 0;
	int leastGridY = (GetGridY(leastPosY) - mapOffsetY/4)/2;
	if (leastGridY < 0)
		leastGridY = 0;
	int mostGridX  = (GetGridX(mostPosX) - mapOffsetX/4)/2;
	if (mostGridX > 126/2)
		mostGridX = 126/2;
	int mostGridY  = (GetGridY(mostPosY) - mapOffsetY/4)/2;
	if (mostGridY > 126/2)
		mostGridY = 126/2;

	int index;
	float ux1,ux2,uy1,uy2;
	int tVertX1, tVertX2;
	int tVertY1, tVertY2;
	D3DXVECTOR3 tVect[2][2];

	// for each drawing tile that could be effected
	for (int i = leastGridY; i <= mostGridY; ++i)
	{
		for (int j = leastGridX; j <= mostGridX; ++j)
		{
			TILE *tile = &m_TerrainTile[j][i];

			int detailStep = 1 << (3-tile->DetailLevel);

			// for each height-point effected (given the current level of detail)
			for (int i2  = i*8; 
						i2 <= i*8 + 8; 
						i2 += detailStep)
			{
				for (int j2  = j*8; 
							j2 <= j*8 + 8; 
							j2 += detailStep)
				{
					index = shadowTriIndex*3;
					
					ux1 = (x - (j2+mapOffsetX)  *10/4.0f) / (scale * 2.0f) + 0.5f;
					ux2 = (x - (j2+detailStep+mapOffsetX)*10/4.0f) / (scale * 2.0f) + 0.5f;
					uy1 = (y - (i2+mapOffsetY)  *10/4.0f) / (scale * 2.0f) + 0.5f;
					uy2 = (y - (i2+detailStep+mapOffsetY)*10/4.0f) / (scale * 2.0f) + 0.5f;

					if (ux2 > 1.0f || ux1 < 0.0f || uy2 > 1.0f || uy1 < 0.0f)
						;
					else
					{
						tVertX1 = j2;
						tVertX2 = j2 + detailStep;
						if (tVertX2 > 126)
		 					tVertX2 = 126;

						tVertY1 = i2;
						tVertY2 = i2 + detailStep;
						if (tVertY2 > 126)
		 					tVertY2 = 126;

						tVect[0][0] = TerrainVerts[tVertY1][tVertX1].vert;
						tVect[1][0] = TerrainVerts[tVertY2][tVertX1].vert;
						tVect[0][1] = TerrainVerts[tVertY1][tVertX2].vert;
						tVect[1][1] = TerrainVerts[tVertY2][tVertX2].vert;

						// copy the first land tri into the shadow tri buffer
						pVertices[index].tu = ux1;
						pVertices[index].tv = uy1;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[0][0].x;
						pVertices[index].p.z = tVect[0][0].z;
						pVertices[index].p.y = 0.01f + tVect[0][0].y;
						++index;

						pVertices[index].tu = ux2;
						pVertices[index].tv = uy1;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[0][1].x;
						pVertices[index].p.z = tVect[0][1].z;
						pVertices[index].p.y = 0.01f + tVect[0][1].y;
						++index;

						pVertices[index].tu = ux1;
						pVertices[index].tv = uy2;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[1][0].x;
						pVertices[index].p.z = tVect[1][0].z;
						pVertices[index].p.y = 0.01f + tVect[1][0].y;
				
						++shadowTriIndex;
						if (shadowTriIndex >= MAX_LAND_SHADOW_VERTS / 3)
						{
							shadowVBuffer->Unlock();
							return;
						}

						index = shadowTriIndex*3;

						// copy the second tri into the shadow tri buffer
						pVertices[index].tu = ux2;
						pVertices[index].tv = uy2;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[1][1].x;
						pVertices[index].p.z = tVect[1][1].z;
						pVertices[index].p.y = 0.01f + tVect[1][1].y;
						++index;

						pVertices[index].tu = ux1;
						pVertices[index].tv = uy2;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[1][0].x;
						pVertices[index].p.z = tVect[1][0].z;
						pVertices[index].p.y = 0.01f + tVect[1][0].y;
						++index;

						pVertices[index].tu = ux2;
						pVertices[index].tv = uy1;
						pVertices[index].color = 0xffffffff;
						pVertices[index].n.x = 0;
						pVertices[index].n.y = 1;
						pVertices[index].n.z = 0;
						pVertices[index].p.x = tVect[0][1].x;
						pVertices[index].p.z = tVect[0][1].z;
						pVertices[index].p.y = 0.01f + tVect[0][1].y;
				
						++shadowTriIndex;
						if (shadowTriIndex >= MAX_LAND_SHADOW_VERTS / 3)
						{
							shadowVBuffer->Unlock();
							return;
						}
					}
				}
			}
		}
	}


	 /*
	int leastGridX = GetGridX(leastPosX);
	if (leastGridX < 0)
		leastGridX = 0;
	int leastGridY = GetGridY(leastPosY);
	if (leastGridY < 0)
		leastGridY = 0;
	int mostGridX  = GetGridX(mostPosX);
	if (mostGridX > 126)
		mostGridX = 126;
	int mostGridY  = GetGridY(mostPosY);
	if (mostGridY > 126)
		mostGridY = 126;

	// for each of the affected squares
	for (int i = leastGridY; i <= mostGridY; ++i)
	{
		for (int j = leastGridX; j <= mostGridX; ++j)
		{
			// copy the first land tri into the shadow tri buffer
			pVertices[shadowTriIndex*3+0].tu = (x - j * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+0].tv = (y - i * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+0].color = 0xffffffff;
			pVertices[shadowTriIndex*3+0].n.x = 0;
			pVertices[shadowTriIndex*3+0].n.y = 1;
			pVertices[shadowTriIndex*3+0].n.z = 0;
			pVertices[shadowTriIndex*3+0].p.x = GetXForPoint(j);
			pVertices[shadowTriIndex*3+0].p.z = GetZForPoint(i);
			pVertices[shadowTriIndex*3+0].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+0].p.x,
						 				  pVertices[shadowTriIndex*3+0].p.z, NULL);
	
			pVertices[shadowTriIndex*3+1].tu = (x - (j+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+1].tv = (y - i     * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+1].color = 0xffffffff;
			pVertices[shadowTriIndex*3+1].n.x = 0;
			pVertices[shadowTriIndex*3+1].n.y = 1;
			pVertices[shadowTriIndex*3+1].n.z = 0;
			pVertices[shadowTriIndex*3+1].p.x = GetXForPoint(j+1);
			pVertices[shadowTriIndex*3+1].p.z = GetZForPoint(i);
			pVertices[shadowTriIndex*3+1].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+1].p.x,
						 				  pVertices[shadowTriIndex*3+1].p.z, NULL);
	
			pVertices[shadowTriIndex*3+2].tu = (x - j     * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+2].tv = (y - (i+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+2].color = 0xffffffff;
			pVertices[shadowTriIndex*3+2].n.x = 0;
			pVertices[shadowTriIndex*3+2].n.y = 1;
			pVertices[shadowTriIndex*3+2].n.z = 0;
			pVertices[shadowTriIndex*3+2].p.x = GetXForPoint(j);
			pVertices[shadowTriIndex*3+2].p.z = GetZForPoint(i+1);
			pVertices[shadowTriIndex*3+2].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+2].p.x,
						 				  pVertices[shadowTriIndex*3+2].p.z, NULL);
	
			++shadowTriIndex;
			if (shadowTriIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   shadowVBuffer->Unlock();
				return;
			}

			// copy the second land tri into the shadow tri buffer
			pVertices[shadowTriIndex*3+0].tu = (x - (j+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+0].tv = (y - (i+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+0].color = 0xffffffff;
			pVertices[shadowTriIndex*3+0].n.x = 0;
			pVertices[shadowTriIndex*3+0].n.y = 1;
			pVertices[shadowTriIndex*3+0].n.z = 0;
			pVertices[shadowTriIndex*3+0].p.x = GetXForPoint(j+1);
			pVertices[shadowTriIndex*3+0].p.z = GetZForPoint(i+1);
			pVertices[shadowTriIndex*3+0].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+0].p.x,
						 				  pVertices[shadowTriIndex*3+0].p.z, NULL);
	
			pVertices[shadowTriIndex*3+1].tu = (x - j     * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+1].tv = (y - (i+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+1].color = 0xffffffff;
			pVertices[shadowTriIndex*3+1].n.x = 0;
			pVertices[shadowTriIndex*3+1].n.y = 1;
			pVertices[shadowTriIndex*3+1].n.z = 0;
			pVertices[shadowTriIndex*3+1].p.x = GetXForPoint(j);
			pVertices[shadowTriIndex*3+1].p.z = GetZForPoint(i+1);
			pVertices[shadowTriIndex*3+1].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+1].p.x,
						 				  pVertices[shadowTriIndex*3+1].p.z, NULL);
	
			pVertices[shadowTriIndex*3+2].tu = (x - (j+1) * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+2].tv = (y - i     * CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[shadowTriIndex*3+2].color = 0xffffffff;
			pVertices[shadowTriIndex*3+2].n.x = 0;
			pVertices[shadowTriIndex*3+2].n.y = 1;
			pVertices[shadowTriIndex*3+2].n.z = 0;
			pVertices[shadowTriIndex*3+2].p.x = GetXForPoint(j+1);
			pVertices[shadowTriIndex*3+2].p.z = GetZForPoint(i);
			pVertices[shadowTriIndex*3+2].p.y = 0.01f + 
				  HeightAtPoint(pVertices[shadowTriIndex*3+2].p.x,
						 				  pVertices[shadowTriIndex*3+2].p.z, NULL);
	
			++shadowTriIndex;
			if (shadowTriIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   shadowVBuffer->Unlock();
				return;
			}
  
		}

	}
	*/

   shadowVBuffer->Unlock();

}

//******************************************************************
void GroundObjectTiles::DrawShadows(void)
{

   // Set the render states for doing shadows
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , FALSE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,   D3DTADDRESS_CLAMP  );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,   D3DTADDRESS_CLAMP  );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_ZERO);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCCOLOR);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_SRCCOLOR);
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CW );
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

	puma->m_pd3dDevice->SetTexture( 0, shadowBitmap );

   puma->m_pd3dDevice->SetVertexShader( D3DFVF_SHADOWVERTEX );
   puma->m_pd3dDevice->SetStreamSource( 0, shadowVBuffer, sizeof(SHADOWVERTEX) );

   if( shadowTriIndex > 0 )
      puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, shadowTriIndex );

	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,   D3DTADDRESS_WRAP  );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,   D3DTADDRESS_WRAP  );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );


}

//*******************************************************************************
D3DXVECTOR3 CalculateNormal(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2)
{
	D3DXVECTOR3	 norm, diff1, diff2;

	diff1 = point2 - point1;
	diff2 = point1 - point0;


	D3DXVec3Cross( &norm, &diff1, &diff2);
   D3DXVec3Normalize( &norm, &norm );

   return norm;
}

//*******************************************************************************
float GetGroundHeight(D3DXVECTOR3 point0, D3DXVECTOR3 point1, D3DXVECTOR3 point2, D3DXVECTOR3 normal, float X, float Z)
{
	D3DXVECTOR3	vNormal,	vPoint;

	vNormal = normal ;

   if (vNormal.y == 0)
   {
      // degenerate region
      return point0.y ;
   }
   else
   {
      // use corner 0 for point on plane
		vPoint = point0 ;

      return ((vPoint.z - Z)*vNormal.z + (vPoint.x - X)*vNormal.x + vPoint.y*vNormal.y)/vNormal.y;
   }
}




/* end of file */
