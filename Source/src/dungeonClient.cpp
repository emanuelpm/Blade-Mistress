//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include ".\puma\puma.h"
#include "DungeonClient.h"
#include ".\puma\noise.h"
#include "BBO.h"
#include "BBOClient.h"

extern Client *	lclient;

const float WALL_HEIGHT = 5.0f;

//***************************************************************************************
//***************************************************************************************
DungeonPiece::DungeonPiece(int doid, char *doname)	 : DataObject(doid,doname)
{
	vertPtrArray = NULL;

	InitVertArray(puma->m_pd3dDevice);

	// open up slots for each location
	slotArray = new LocationSlots[DUNGEON_PIECE_SIZE * DUNGEON_PIECE_SIZE];
	for (int i = 0; i < DUNGEON_PIECE_SIZE; ++i)
	{
		for (int j = 0; j < DUNGEON_PIECE_SIZE; ++j)
		{
			for (int k = 0; k < NUM_OF_SLOTS_PER_SPACE; ++k)
			{
				slotArray[i*DUNGEON_PIECE_SIZE+j].used[k] = FALSE;
//				slotArray[i*DUNGEON_PIECE_SIZE+j].toughestMonsterPoints = 0;
			}
		}
	}

}

//***************************************************************************************
DungeonPiece::~DungeonPiece()
{
   if (slotArray)
		delete[] slotArray;
	ReleaseVertArray();

}

//***************************************************************************************
void DungeonPiece::InitVertArray(LPDIRECT3DDEVICE8 m_pd3dDevice)
{

	ReleaseVertArray();

	vertPtrArray = new LPDIRECT3DVERTEXBUFFER8[NUM_OF_DUNGEON_WALL_TYPES + 1];

	for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		vertPtrArray[i] = NULL;

/*
	numOfVertices = sizeW * 2 * 3;

	for (i = 0; i < NUM_OF_DUNGEON_WALL_TYPES; i++)
	{
	   if( FAILED( m_pd3dDevice->CreateVertexBuffer( numOfVertices*sizeof(GROUNDVERTEXSTRUCT),
		                          0, GROUND_VERTEXDESC,
			                       D3DPOOL_MANAGED, &(vertPtrArray[i]) ) ) )
	   {
			ReleaseVertArray();
			return;
	   }

	}
*/


}

//***************************************************************************************
void DungeonPiece::ReleaseVertArray(void)
{
	if (vertPtrArray)
	{
		for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		{
			SAFE_RELEASE(vertPtrArray[i]);
		}

		delete[] vertPtrArray;
		vertPtrArray = NULL;
	}

}

//***************************************************************************************
void DungeonPiece::BuildFace(GROUNDVERTEXSTRUCT* pVertices, 
									  float mx , float my , float mz,
									  float mx2, float my2, float mz2,
									  float uvMax)
{
	for (int j = 0; j < 2; ++j)
	{
		switch(j)
		{
		case 0:
			pVertices[j*3+0].position.x = mx;
			pVertices[j*3+0].position.y = my;
			pVertices[j*3+0].position.z = mz;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = 0;
			pVertices[j*3+0].tv = 0;

			pVertices[j*3+1].position.x = mx + mx2;
			pVertices[j*3+1].position.y = my;
			pVertices[j*3+1].position.z = mz;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = uvMax;
			pVertices[j*3+1].tv = 0;

			pVertices[j*3+2].position.x = mx + mx2;
			pVertices[j*3+2].position.y = my + my2;
			pVertices[j*3+2].position.z = mz + mz2;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = uvMax;
			pVertices[j*3+2].tv = uvMax;
			break;

		case 1:
			pVertices[j*3+0].position.x = mx + mx2;
			pVertices[j*3+0].position.y = my + my2;
			pVertices[j*3+0].position.z = mz + mz2;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = uvMax;
			pVertices[j*3+0].tv = uvMax;

			pVertices[j*3+1].position.x = mx;
			pVertices[j*3+1].position.y = my + my2;
			pVertices[j*3+1].position.z = mz + mz2;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = 0;
			pVertices[j*3+1].tv = uvMax;

			pVertices[j*3+2].position.x = mx;
			pVertices[j*3+2].position.y = my;
			pVertices[j*3+2].position.z = mz;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = 0;
			pVertices[j*3+2].tv = 0;
			break;
		}
		D3DXVec3Cross( &pVertices[j*3+0].normal, 
						  &(pVertices[j*3+2].position-pVertices[j*3+1].position), 
						  &(pVertices[j*3+1].position-pVertices[j*3+0].position) );
	   D3DXVec3Normalize( &pVertices[j*3+0].normal, &pVertices[j*3+0].normal );
		pVertices[j*3+1].normal = pVertices[j*3+0].normal;
		pVertices[j*3+2].normal = pVertices[j*3+0].normal;
	}
}

//***************************************************************************************
void DungeonPiece::BuildTopFace(GROUNDVERTEXSTRUCT* pVertices, 
									  float mx , float my , float mz,
									  float mx2, float my2, float mz2,
									  float uvMax)
{
	for (int j = 0; j < 2; ++j)
	{
		switch(j)
		{
		case 0:
			pVertices[j*3+0].position.x = mx;
			pVertices[j*3+0].position.y = my;
			pVertices[j*3+0].position.z = mz;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = 0;
			pVertices[j*3+0].tv = 0;

			pVertices[j*3+1].position.x = mx + mx2;
			pVertices[j*3+1].position.y = my + my2;
			pVertices[j*3+1].position.z = mz;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = 0;
			pVertices[j*3+1].tv = uvMax;

			pVertices[j*3+2].position.x = mx + mx2;
			pVertices[j*3+2].position.y = my + my2;
			pVertices[j*3+2].position.z = mz + mz2;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = uvMax;
			pVertices[j*3+2].tv = uvMax;
			break;

		case 1:
			pVertices[j*3+0].position.x = mx + mx2;
			pVertices[j*3+0].position.y = my + my2;
			pVertices[j*3+0].position.z = mz + mz2;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = uvMax;
			pVertices[j*3+0].tv = uvMax;

			pVertices[j*3+1].position.x = mx;
			pVertices[j*3+1].position.y = my;
			pVertices[j*3+1].position.z = mz + mz2;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = uvMax;
			pVertices[j*3+1].tv = 0;

			pVertices[j*3+2].position.x = mx;
			pVertices[j*3+2].position.y = my;
			pVertices[j*3+2].position.z = mz;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = 0;
			pVertices[j*3+2].tv = 0;
			break;
		}
		D3DXVec3Cross( &pVertices[j*3+0].normal, 
						  &(pVertices[j*3+2].position-pVertices[j*3+1].position), 
						  &(pVertices[j*3+1].position-pVertices[j*3+0].position) );
	   D3DXVec3Normalize( &pVertices[j*3+0].normal, &pVertices[j*3+0].normal );
		pVertices[j*3+1].normal = pVertices[j*3+0].normal;
		pVertices[j*3+2].normal = pVertices[j*3+0].normal;
	}
}

//***************************************************************************************
/*
void DungeonPiece::BuildButtEnd(GROUNDVERTEXSTRUCT* pVertices, 
									  float mx , float my , float mz,
									  float mx2, float my2, float mz2,
									  float uvMax)
{
	for (int j = 0; j < 2; ++j)
	{
		switch(j)
		{
		case 0:
			pVertices[j*3+0].position.x = mx;
			pVertices[j*3+0].position.y = my;
			pVertices[j*3+0].position.z = mz;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = 0;
			pVertices[j*3+0].tv = 0;

			pVertices[j*3+1].position.x = mx + mx2;
			pVertices[j*3+1].position.y = my + my2;
			pVertices[j*3+1].position.z = mz;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = 0;
			pVertices[j*3+1].tv = uvMax;

			pVertices[j*3+2].position.x = mx + mx2;
			pVertices[j*3+2].position.y = my + my2;
			pVertices[j*3+2].position.z = mz + mz2;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = uvMax;
			pVertices[j*3+2].tv = uvMax;
			break;

		case 1:
			pVertices[j*3+0].position.x = mx + mx2;
			pVertices[j*3+0].position.y = my + my2;
			pVertices[j*3+0].position.z = mz + mz2;
			pVertices[j*3+0].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+0].tu = uvMax;
			pVertices[j*3+0].tv = uvMax;

			pVertices[j*3+1].position.x = mx;
			pVertices[j*3+1].position.y = my;
			pVertices[j*3+1].position.z = mz + mz2;
			pVertices[j*3+1].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+1].tu = uvMax;
			pVertices[j*3+1].tv = 0;

			pVertices[j*3+2].position.x = mx;
			pVertices[j*3+2].position.y = my;
			pVertices[j*3+2].position.z = mz;
			pVertices[j*3+2].color = D3DCOLOR_RGBA(255,255,255,255);
			pVertices[j*3+2].tu = 0;
			pVertices[j*3+2].tv = 0;
			break;
		}
		D3DXVec3Cross( &pVertices[j*3+0].normal, 
						  &(pVertices[j*3+2].position-pVertices[j*3+1].position), 
						  &(pVertices[j*3+1].position-pVertices[j*3+0].position) );
	   D3DXVec3Normalize( &pVertices[j*3+0].normal, &pVertices[j*3+0].normal );
		pVertices[j*3+1].normal = pVertices[j*3+0].normal;
		pVertices[j*3+2].normal = pVertices[j*3+0].normal;
	}
}
*/
//***************************************************************************************
void DungeonPiece::ProcessMessage(MessDungeonInfo *info)
{
	if (vertPtrArray)
	{
		GROUNDVERTEXSTRUCT* pVertices;

		// release old vert lists
		for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		{
			SAFE_RELEASE(vertPtrArray[i]);
		}

		// copy relavent data
		x              = info->x;
		y              = info->y;
		floorIndex     = info->floor;
		outerWallIndex = info->outerWall;

		// make the floor verts
		numOfVertices[0] = 1 * 2 * 3; // one quad * 2 tris * 3 verts

	   puma->m_pd3dDevice->CreateVertexBuffer( numOfVertices[0]*sizeof(GROUNDVERTEXSTRUCT),
		                          0, GROUND_VERTEXDESC,
			                       D3DPOOL_MANAGED, &(vertPtrArray[0]) );

		// fill in the floor verts
		if( vertPtrArray[0] && D3D_OK == vertPtrArray[0]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) )
		{
			BuildFace(&(pVertices[0]), 
				       x * DUNGEON_PIECE_SIZE * 10, 0, y * DUNGEON_PIECE_SIZE * 10,
				       DUNGEON_PIECE_SIZE * 10, 
						 0, 
						 DUNGEON_PIECE_SIZE * 10,
						 5);
			vertPtrArray[0]->Unlock();
		}

		// if needed, make the outer wall verts
		numOfVertices[1] = 0;
		if (x <= 0)
			numOfVertices[1] += 1;
		if (y <= 0)
			numOfVertices[1] += 1;
		if (x >= info->sizeX/DUNGEON_PIECE_SIZE)
			numOfVertices[1] += 1;
		if (y >= info->sizeY/DUNGEON_PIECE_SIZE)
			numOfVertices[1] += 1;

		if (numOfVertices[1] > 0)
		{
			numOfVertices[1] *= 2 * 3 * DUNGEON_PIECE_SIZE;
			puma->m_pd3dDevice->CreateVertexBuffer( numOfVertices[1]*sizeof(GROUNDVERTEXSTRUCT),
											  0, GROUND_VERTEXDESC,
											  D3DPOOL_MANAGED, &(vertPtrArray[1]) );
			// fill in the wall verts
			if( vertPtrArray[1] && D3D_OK == vertPtrArray[1]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) )
			{
				int k = 0;

				// left outer wall
				if (x <= 0)
				{
					for (int j = 0; j < DUNGEON_PIECE_SIZE; ++j)
					{
						float posY = j; 
						float posX = 0;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						BuildTopFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								0, WALL_HEIGHT, 10, 1);
							++k;
					}
				}

				// top outer wall
				if (y <= 0)
				{
					for (int j = 0; j < DUNGEON_PIECE_SIZE; ++j)
					{
						float posY = 0; 
						float posX = j;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						BuildFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								10, WALL_HEIGHT, 0, 1);
							++k;
					}
				}

				// right outer wall
				if (x >= info->sizeX/DUNGEON_PIECE_SIZE)
				{
					for (int j = 0; j < DUNGEON_PIECE_SIZE; ++j)
					{
						float posY = j; 
						float posX = 0;
//						float posX = DUNGEON_PIECE_SIZE;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						BuildTopFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								0, WALL_HEIGHT, 10, 1);
							++k;
					}
				}

				// bottom outer wall
				if (y >= info->sizeY/DUNGEON_PIECE_SIZE)
				{
					for (int j = 0; j < DUNGEON_PIECE_SIZE; ++j)
					{
//						float posY = DUNGEON_PIECE_SIZE; 
						float posY = 0; 
						float posX = j;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						BuildFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								10, WALL_HEIGHT, 0, 1);
							++k;
					}
				}

				vertPtrArray[1]->Unlock();
			}

		}

		// run through the walls, making associated verts
		int typeRef[NUM_OF_DUNGEON_WALL_TYPES + 1];              // 0 = floor texture

		for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
			typeRef[i] = 0;

		for (int j = 0; j < DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE; ++j)
		{
			leftWall[j] = info->leftWall[j];
			++typeRef[leftWall[j] + 1];

			topWall[j]  = info->topWall[j];
			++typeRef[topWall[j] + 1];
		}

		for (int i = 2; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		{
			if (typeRef[i] > 0)
			{

				numOfVertices[i] = typeRef[i] * 2 * 3; // one quad * 2 tris * 3 verts

				puma->m_pd3dDevice->CreateVertexBuffer( numOfVertices[i]*sizeof(GROUNDVERTEXSTRUCT),
												  0, GROUND_VERTEXDESC,
												  D3DPOOL_MANAGED, &(vertPtrArray[i]) );

				// fill in the wall verts
				if( vertPtrArray[i] && D3D_OK == vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) )
				{
					int k = 0;
					for (int j = 0; j < DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE; ++j)
					{
						float posY = (float)((int)j / (int)DUNGEON_PIECE_SIZE); 
						float posX = j - posY * DUNGEON_PIECE_SIZE;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						if (leftWall[j] + 1 == i)
						{
							BuildTopFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								0, WALL_HEIGHT, 10, 1);
							++k;
						}
						if (topWall[j] + 1 == i)
						{
							BuildFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								10, WALL_HEIGHT, 0, 1);
							++k;
						}
					}
					vertPtrArray[i]->Unlock();
				}
			}
//			SAFE_RELEASE(vertPtrArray[i]);
		}

	}

}


//***************************************************************************************
void DungeonPiece::RebuildWalls(void)
{
	if (vertPtrArray)
	{
		GROUNDVERTEXSTRUCT* pVertices;

		// release old vert lists
		for (int i = 2; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		{
			SAFE_RELEASE(vertPtrArray[i]);
		}

		// run through the walls, making associated verts
		int typeRef[NUM_OF_DUNGEON_WALL_TYPES + 1];              // 0 = floor texture

		for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
			typeRef[i] = 0;

		for (int j = 0; j < DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE; ++j)
		{
			++typeRef[leftWall[j] + 1];

			++typeRef[topWall[j] + 1];
		}

		for (int i = 2; i < NUM_OF_DUNGEON_WALL_TYPES + 1; i++)
		{
			if (typeRef[i] > 0)
			{

				numOfVertices[i] = typeRef[i] * 2 * 3; // one quad * 2 tris * 3 verts

				puma->m_pd3dDevice->CreateVertexBuffer( numOfVertices[i]*sizeof(GROUNDVERTEXSTRUCT),
												  0, GROUND_VERTEXDESC,
												  D3DPOOL_MANAGED, &(vertPtrArray[i]) );

				// fill in the wall verts
				if( vertPtrArray[i] && D3D_OK == vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) )
				{
					int k = 0;
					for (int j = 0; j < DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE; ++j)
					{
						float posY = (float)((int)j / (int)DUNGEON_PIECE_SIZE); 
						float posX = j - posY * DUNGEON_PIECE_SIZE;
						posY *= 10;
						posY += y * DUNGEON_PIECE_SIZE * 10;
						posX *= 10;
						posX += x * DUNGEON_PIECE_SIZE * 10;

						if (leftWall[j] + 1 == i)
						{
							BuildTopFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								0, WALL_HEIGHT, 10, 1);
							++k;
						}
						if (topWall[j] + 1 == i)
						{
							BuildFace(&(pVertices[k * 2 * 3]), 
								posX,  0, posY,
								10, WALL_HEIGHT, 0, 1);
							++k;
						}
					}
					vertPtrArray[i]->Unlock();
				}
			}
//			SAFE_RELEASE(vertPtrArray[i]);
		}

	}

}




//***************************************************************************************
//***************************************************************************************
DungeonClient::DungeonClient(int doid, char *doname)	 : DataObject(doid,doname)
{
	Init();

	wallMesh = new PumaMesh();
	if (0 == WhatAmI())
	{
		wallMesh->LoadFromASC(puma->m_pd3dDevice,"dat\\wall1.ASE");
		wallMesh->Scale(puma->m_pd3dDevice,0.13f,0.13f/2,0.13f);
	}
	else
	{
		wallMesh->LoadFromASC(puma->m_pd3dDevice,"dat\\wallPlain.ASE");
		wallMesh->Scale(puma->m_pd3dDevice,0.13f,0.13f/2,0.13f);
	}

}

//***************************************************************************************
DungeonClient::~DungeonClient()
{
	delete wallMesh;

	delete pieces;

	for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; ++i)
	{
		SAFE_RELEASE(pTexture[i]);
//		pTexture[i]
//		if (textureRef[i] > 0)
//		{
//		}
	}

}

//***************************************************************************************
void DungeonClient::Init(void)
{
//	pTexture = NULL;
	pieces = new DoublyLinkedList();

	for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; ++i)
	{
		textureRef[i] = 0;
		pTexture[i] = NULL;
	}




   ZeroMemory( &material, sizeof(D3DMATERIAL8) );
   material.Diffuse.r = 0.5f;
   material.Diffuse.g = 0.5f;
   material.Diffuse.b = 0.5f;
   material.Diffuse.a = 0.5f;
   material.Ambient.r = 0.1f;
   material.Ambient.g = 0.1f;
   material.Ambient.b = 0.1f;
   material.Ambient.a = 0.1f;

}


char dungeonFloorNames[NUM_OF_DUNGEON_FLOOR_TYPES][64] =
{
	{"dat\\dunflr-marble.png"},
	{"dat\\dunflr-dark.png"},
	{"dat\\dunflr-black.png"},
	{"dat\\dunflr-cracked.png"}
};

char dungeonTextureNames[NUM_OF_DUNGEON_WALL_TYPES][64] =
{
	{"dat\\dunwal-arch.png"},
	{"dat\\dunwal-brick.png"},
	{"dat\\dunwal-stones1.png"},
	{"dat\\dunwal-pattern1.png"},
	{"dat\\dunwal-pattern2.png"},
	{"dat\\dunwal-pattern3.png"},
	{"dat\\dunwal-pattern4.png"},
	{"dat\\dunwal-stones3.png"},
	{"dat\\dunwal-brick2.png"},
	{"dat\\dunwal-wood1.png"},
	{"dat\\dunwal-stones2.png"}
};

char towerFloorNames[NUM_OF_TOWER_FLOOR_TYPES][64] =
{
	{"dat\\towflr-1.png"},
	{"dat\\towflr-2.png"},
	{"dat\\towflr-3.png"},
	{"dat\\towflr-4.png"},
	{"dat\\towflr-5.png"},
	{"dat\\towflr-6.png"},
	{"dat\\towflr-7.png"}
};

char towerTextureNames[NUM_OF_TOWER_WALL_TYPES][64] =
{
	{"dat\\towwal-1.png"},
	{"dat\\towwal-2.png"},
	{"dat\\towwal-3.png"},
	{"dat\\towwal-4.png"},
	{"dat\\towwal-5.png"},
	{"dat\\towwal-6.png"},
	{"dat\\towwal-7.png"},
	{"dat\\towwal-8.png"}
};

//***************************************************************************************
void DungeonClient::Draw(int x, int y)
{

	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	puma->m_pd3dDevice->SetMaterial(&material);

//	SetTextureColorStage( puma->m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
//	SetTextureAlphaStage( puma->m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
//   puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
//   puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
//   puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
//   puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
  

	for (int i = 0; i < NUM_OF_DUNGEON_WALL_TYPES + 1; ++i)
	{
		DungeonPiece *dp = (DungeonPiece *) pieces->First();
		while (dp)
		{

			if (0 == i) // draw floor
			{
				if (dp->vertPtrArray[i])
				{
					if (!(pTexture[i]))
					{
						// load the texture now, then.
						if (0 == WhatAmI())
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, dungeonFloorNames[dp->floorIndex],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
						else
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, towerFloorNames[dp->floorIndex],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
					}
					if (pTexture[i])
					{
						puma->m_pd3dDevice->SetTexture( 0, pTexture[i] );
					   puma->m_pd3dDevice->SetStreamSource( 0, dp->vertPtrArray[i], 
							                                sizeof(GROUNDVERTEXSTRUCT) );
					   puma->m_pd3dDevice->SetVertexShader( GROUND_VERTEXDESC );

						for (int countBase = 0; countBase < dp->numOfVertices[i] / 3;

							  countBase += puma->m_d3dCaps.MaxPrimitiveCount)
						{
							int numOut = dp->numOfVertices[i] / 3 - countBase;
							if (puma->m_d3dCaps.MaxPrimitiveCount < dp->numOfVertices[i] / 3 - countBase)
								numOut = puma->m_d3dCaps.MaxPrimitiveCount;
							puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
						}
					}
				}
			}
			else if (1 == i) // draw outer wall
			{
				if (dp->vertPtrArray[i])
				{
					if (!(pTexture[i]))
					{
						// load the texture now, then.
						if (0 == WhatAmI())
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, dungeonTextureNames[dp->outerWallIndex],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
						else
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, towerTextureNames[dp->outerWallIndex],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
					}
					if (pTexture[i])
					{
						puma->m_pd3dDevice->SetTexture( 0, pTexture[i] );
					   puma->m_pd3dDevice->SetStreamSource( 0, dp->vertPtrArray[i], 
							                                sizeof(GROUNDVERTEXSTRUCT) );
					   puma->m_pd3dDevice->SetVertexShader( GROUND_VERTEXDESC );
						for (int countBase = 0; countBase < dp->numOfVertices[i] / 3;

							  countBase += puma->m_d3dCaps.MaxPrimitiveCount)
						{
							int numOut = dp->numOfVertices[i] / 3 - countBase;
							if (puma->m_d3dCaps.MaxPrimitiveCount < dp->numOfVertices[i] / 3 - countBase)
								numOut = puma->m_d3dCaps.MaxPrimitiveCount;
							puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
						}
					}
				}
			}
			else
			{
				if (dp->vertPtrArray[i])
				{
					if (!(pTexture[i]))
					{
						// load the texture now, then.
						if (0 == WhatAmI())
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, dungeonTextureNames[i-2],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
						else
						{
							D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, towerTextureNames[i-2],
								0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
								D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture[i]);
						}
					}
					if (pTexture[i])
					{

						D3DXMATRIX matWorld, mat2;
						puma->m_pd3dDevice->SetTexture( 0, pTexture[i] );
						
						for (int j = 0; j < DUNGEON_PIECE_SIZE*DUNGEON_PIECE_SIZE; ++j)
						{
							float posY = (float)((int)j / (int)DUNGEON_PIECE_SIZE); 
							float posX = j - posY * DUNGEON_PIECE_SIZE;
							posY *= 10;
							posY += dp->y * DUNGEON_PIECE_SIZE * 10;
							posX *= 10;
							posX += dp->x * DUNGEON_PIECE_SIZE * 10;

							if (dp->leftWall[j] + 1 == i)
							{
								D3DXMatrixIdentity( &matWorld );
								D3DXMatrixRotationY(&matWorld, (float) -PI/2);
								matWorld._41 = posX;
								matWorld._42 = 0;
								matWorld._43 = posY;
								puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

								wallMesh->Draw(puma->m_pd3dDevice);

							}
							if (dp->topWall[j] + 1 == i)
							{
								D3DXMatrixIdentity( &matWorld );
								D3DXMatrixRotationY(&matWorld, (float) 0);
								matWorld._41 = posX;
								matWorld._42 = 0;
								matWorld._43 = posY;
								puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

								wallMesh->Draw(puma->m_pd3dDevice);
							}
						}

						





						/*
						puma->m_pd3dDevice->SetTexture( 0, pTexture[i] );
					   puma->m_pd3dDevice->SetStreamSource( 0, dp->vertPtrArray[i], 
							                                sizeof(GROUNDVERTEXSTRUCT) );
					   puma->m_pd3dDevice->SetVertexShader( GROUND_VERTEXDESC );
						for (int countBase = 0; countBase < dp->numOfVertices[i] / 3;

							  countBase += puma->m_d3dCaps.MaxPrimitiveCount)
						{
							int numOut = dp->numOfVertices[i] / 3 - countBase;
							if (puma->m_d3dCaps.MaxPrimitiveCount < dp->numOfVertices[i] / 3 - countBase)
								numOut = puma->m_d3dCaps.MaxPrimitiveCount;
							puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
						}
						*/
					}
				}
			}

			dp = (DungeonPiece *) pieces->Next();
		}
	}		

}
/*
//***************************************************************************************
void DungeonClient::LoadTexture(LPDIRECT3DDEVICE8 m_pd3dDevice, char *fileName)
{

   SAFE_RELEASE(pTexture);

   // Use D3DX to create a texture from a file based image
//   D3DXCreateTextureFromFile( m_pd3dDevice, fileName, &pTexture );
	HRESULT hr = D3DXCreateTextureFromFileEx( m_pd3dDevice, fileName,
							0,0,4,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &pTexture);


}
  */

//*******************************************************************************
float DungeonClient::CellSize(void)
{

   return 10.0f;

}

//*******************************************************************************
// circle of 4, then offset circle of four, then circle of 8, then circle of 12
void DungeonClient::GetSlotPosition(int slotIndex, float &x, float &y, float &ang)
{
	float radius = CellSize() *0.1f;
	if (slotIndex > 3)
		radius = CellSize() *0.2f;
	if (slotIndex > 7)
		radius = CellSize() *0.3f;
	if (slotIndex > 15)
		radius = CellSize() *0.4f;

	float angle = 0;
	angle = (float)(slotIndex) / 4 * D3DX_PI * 2;
	if (slotIndex > 15)
		angle = (float)(slotIndex-16) / 12 * D3DX_PI * 2;
	if (slotIndex > 7)
		angle = (float)(slotIndex-8) / 8 * D3DX_PI * 2;
	if (slotIndex > 3)
		angle = (float)(slotIndex-4) / 4 * D3DX_PI * 2 + D3DX_PI/4;

	x = CellSize()/2.0f + sin(angle) * radius;
	y = CellSize()/2.0f + cos(angle) * radius;

	ang = -1 * angle;

}

//*******************************************************************************
int DungeonClient::GetFirstOpenSlot(int x, int y)
{
	LocationSlots *slot = NULL;

	int fiveX = x / 5;
	int fiveY = y / 5;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		if (dp->x == fiveX && dp->y == fiveY)
		{
			x -= fiveX * 5;
			y -= fiveY * 5;
			slot = &(dp->slotArray[y*DUNGEON_PIECE_SIZE+x]);

			for (int i = 0; i < NUM_OF_SLOTS_PER_SPACE; ++i)
			{
				if (!slot->used[i])
					return i;
			}
			return-1;
		}

		dp = (DungeonPiece *) pieces->Next();
	}

	return -1;
}

//*******************************************************************************
void DungeonClient::ClaimSlot(int x, int y, int index, int type)
{
	LocationSlots *slot = NULL;

	int fiveX = x / 5;
	int fiveY = y / 5;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		if (dp->x == fiveX && dp->y == fiveY)
		{
			x -= fiveX * 5;
			y -= fiveY * 5;
			slot = &(dp->slotArray[y*DUNGEON_PIECE_SIZE+x]);
			slot->used[index] = type;
			return;
		}

		dp = (DungeonPiece *) pieces->Next();
	}

//	LocationSlots *slot = &(slotArray[y*sizeW+x]);

//	slot->used[index] = type;
}

//*******************************************************************************
void DungeonClient::ReleaseSlot(int x, int y, int index)
{
//	LocationSlots *slot = &(slotArray[y*sizeW+x]);
	LocationSlots *slot = NULL;

	int fiveX = x / 5;
	int fiveY = y / 5;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		if (dp->x == fiveX && dp->y == fiveY)
		{
			x -= fiveX * 5;
			y -= fiveY * 5;
			slot = &(dp->slotArray[y*DUNGEON_PIECE_SIZE+x]);
			slot->used[index] = FALSE;
			return;
		}

		dp = (DungeonPiece *) pieces->Next();
	}

}


//*******************************************************************************
void DungeonClient::ProcessMessage(MessDungeonInfo *info)
{
	LocationSlots *slot = NULL;

	int fiveX = info->x;
	int fiveY = info->y;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		if (dp->x == fiveX && dp->y == fiveY)
		{
			dp->ProcessMessage(info);
			return;
		}

		dp = (DungeonPiece *) pieces->Next();
	}

	// didn't find it, so make it!
	dp = new DungeonPiece(0,"DUNGEON_PIECE");
	pieces->Append(dp);
	dp->ProcessMessage(info);

}

//*******************************************************************************
void DungeonClient::ProcessChangeMessage(MessDungeonChange *info)
{
	LocationSlots *slot = NULL;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		if (info->left)
		{
			int fx = info->x / 5;
			int fy = info->y / 5;
			if (dp->x == fx && dp->y == fy)
			{
				dp->leftWall[(info->y - dp->y * DUNGEON_PIECE_SIZE) * 
					           DUNGEON_PIECE_SIZE + 
								  (info->x - dp->x * DUNGEON_PIECE_SIZE)] = info->left - 1;
				dp->RebuildWalls();
				return;
			}		
		}
		else if (info->top)
		{
			int fx = info->x / 5;
			int fy = info->y / 5;
			if (dp->x == fx && dp->y == fy)
			{
				dp->topWall[(info->y - dp->y * DUNGEON_PIECE_SIZE) * 
					           DUNGEON_PIECE_SIZE + 
								  (info->x - dp->x * DUNGEON_PIECE_SIZE)] = info->top - 1;
				dp->RebuildWalls();
				return;
			}		
		}
		else if (info->floor)
		{
			dp->floorIndex = info->floor - 1;
			SAFE_RELEASE(pTexture[0]);
			pTexture[0] = NULL;
		}
		else if (info->outer)
		{
			dp->outerWallIndex = info->outer - 1;
			SAFE_RELEASE(pTexture[1]);
			pTexture[1] = NULL;
		}
		else if (info->reset)
		{
			pieces->Remove(dp);
			delete dp;
		}

		dp = (DungeonPiece *) pieces->Next();
	}

	if (info->reset)
	{
		AskForNeededParts(info->x, info->y);
	}

}

//*******************************************************************************
void DungeonClient::AskForNeededParts(int px, int py)
{
	MessRequestDungeonInfo   messDInfo;
	int visX[4], visY[4];

	visX[0] = (px - 4)/DUNGEON_PIECE_SIZE;
	visY[0] = (py - 4)/DUNGEON_PIECE_SIZE;
	visX[1] = (px + 4)/DUNGEON_PIECE_SIZE;
	visY[1] = (py - 4)/DUNGEON_PIECE_SIZE;
	visX[2] = (px - 4)/DUNGEON_PIECE_SIZE;
	visY[2] = (py + 4)/DUNGEON_PIECE_SIZE;
	visX[3] = (px + 4)/DUNGEON_PIECE_SIZE;
	visY[3] = (py + 4)/DUNGEON_PIECE_SIZE;

	DungeonPiece *dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (dp->x == visX[i] && dp->y == visY[i])
				visX[i] = 0;
		}
		dp = (DungeonPiece *) pieces->Next();
	}

	for (int i = 0; i < 4; ++i)
	{
		if (visX[i] != 0)
		{
			messDInfo.x = visX[i];
			messDInfo.y = visY[i];
			lclient->SendMsg(sizeof(messDInfo),(void *)&messDInfo);
		}
	}

	visX[0] = (px - 4)/DUNGEON_PIECE_SIZE;
	visY[0] = (py - 0)/DUNGEON_PIECE_SIZE;
	visX[1] = (px + 4)/DUNGEON_PIECE_SIZE;
	visY[1] = (py - 0)/DUNGEON_PIECE_SIZE;
	visX[2] = (px - 0)/DUNGEON_PIECE_SIZE;
	visY[2] = (py + 4)/DUNGEON_PIECE_SIZE;
	visX[3] = (px + 0)/DUNGEON_PIECE_SIZE;
	visY[3] = (py - 4)/DUNGEON_PIECE_SIZE;

	dp = (DungeonPiece *) pieces->First();
	while (dp)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (dp->x == visX[i] && dp->y == visY[i])
			{
				visX[i] = 0;
				visY[i] = 0;
			}
		}
		dp = (DungeonPiece *) pieces->Next();
	}

	for (int i = 0; i < 4; ++i)
	{
		if (visX[i] != 0 || visY[i] != 0)
		{
			messDInfo.x = visX[i];
			messDInfo.y = visY[i];
			lclient->SendMsg(sizeof(messDInfo),(void *)&messDInfo);
		}
	}

}

/* end of file */
