//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "./puma/puma.h"
#include "GroundObject.h"
#include ".\puma\noise.h"
#include "BBO.h"

LPDIRECT3DTEXTURE8 pGroundInfoTexture;
D3DLOCKED_RECT groundInfoLockInfo;

//*******************************************************************************
void OpenGroundInfoTexture(void)
{
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\terrain-info.png",
				0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pGroundInfoTexture);
//   D3DXCreateTextureFromFile( puma->m_pd3dDevice, "dat\\terrain-info.bmp", &pGroundInfoTexture );
	pGroundInfoTexture->LockRect(0, &groundInfoLockInfo, NULL, 0);
}

//*******************************************************************************
void CloseGroundInfoTexture(void)
{
	pGroundInfoTexture->UnlockRect(0);
	SAFE_RELEASE(pGroundInfoTexture);
}

//*******************************************************************************
D3DCOLOR GroundInfoTexturePixel(int x, int y)
{
	char *charPtr = (char *)groundInfoLockInfo.pBits + groundInfoLockInfo.Pitch * y;
	D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;
	return pixelPtr[x];
}


//***************************************************************************************
//***************************************************************************************
GroundObject::GroundObject(int doid, char *doname)	 : DataObject(doid,doname)
{
	Init();
}

//***************************************************************************************
GroundObject::GroundObject(void) : DataObject(0,"PLASMA_TEXTURE")
{
	Init();
}

//***************************************************************************************
GroundObject::~GroundObject()
{
//	delete fog3;
//	delete fog2;
//	delete fog1;

   SAFE_DELETE(pt);
   if (slotArray)
		delete[] slotArray;
	ReleaseVertArray();
   SAFE_RELEASE(pTexture);

}

//***************************************************************************************
void GroundObject::Init(void)
{
	pt = NULL;
	numOfVertices = 0;
	vertPtrArray = NULL; // Buffer to hold vertices
	pTexture = NULL;
	slotArray = NULL;

   ZeroMemory( &groundMaterial, sizeof(D3DMATERIAL8) );
   groundMaterial.Diffuse.r = 0.5f;
   groundMaterial.Diffuse.g = 0.5f;
   groundMaterial.Diffuse.b = 0.5f;
   groundMaterial.Diffuse.a = 0.5f;
   groundMaterial.Ambient.r = 0.1f;
   groundMaterial.Ambient.g = 0.1f;
   groundMaterial.Ambient.b = 0.1f;
   groundMaterial.Ambient.a = 0.1f;
/*
	fog1 = new PumaMesh();
	fog1->LoadFromASC(puma->m_pd3dDevice, "dat\\fogbank.ase");
	fog2 = new PumaMesh();
	fog2->LoadFromASC(puma->m_pd3dDevice, "dat\\fogbank.ase");
	fog3 = new PumaMesh();
	fog3->LoadFromASC(puma->m_pd3dDevice, "dat\\fogbank.ase");

	fog1->Scale(puma->m_pd3dDevice, 0.7f, 0.7f, 0.7f);
	fog2->Scale(puma->m_pd3dDevice, 1.1f, 1.1f, 1.1f);
	fog3->Scale(puma->m_pd3dDevice, 1.3f, 1.3f, 1.3f);
*/
}

//***************************************************************************************
D3DXVECTOR3 GroundObject::GetVertPos(int i, int j, int height)
{

	return D3DXVECTOR3( j * CellSize(), height /255.0f * HEIGHT_COEFF, i * CellSize() );
}

//***************************************************************************************
D3DCOLOR GroundObject::GetVertColor(int i, int j, int height)
{
	D3DCOLOR retVal;

	// okay, terrain types:
	//		grass, swamp, water, snow, desert
	D3DCOLOR water      = D3DCOLOR_RGBA(  0,  0,255,255) ;
	D3DCOLOR desert     = D3DCOLOR_RGBA(255,255,  0,255) ;
	D3DCOLOR snow       = D3DCOLOR_RGBA(255,255,255,255) ;
	D3DCOLOR swamp      = D3DCOLOR_RGBA(255,  0,  0,255) ;
	D3DCOLOR forest     = D3DCOLOR_RGBA(  0,128,  0,255) ;
	D3DCOLOR deepForest = D3DCOLOR_RGBA(  0,255,  0,255) ;

	D3DCOLOR pixel = GroundInfoTexturePixel(j,i);

	if (pixel == water)
		retVal = D3DCOLOR_RGBA(0,0,100,255) ; // water
	else if (pixel == snow)
		retVal = D3DCOLOR_RGBA(235,235,255,255) ; // snow
	else if (pixel == desert)
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // desert
	else if (pixel == swamp)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else if (pixel == forest)
		retVal = D3DCOLOR_RGBA(20,80,20,255) ; // grass
	else if (pixel == deepForest)
		retVal = D3DCOLOR_RGBA(20,60,20,255) ; // dark grass
	else
		retVal = D3DCOLOR_RGBA(190,160,70,255) ; // beach

	return retVal;
}

//***************************************************************************************
int GroundObject::GetTerrainType(int i, int j)
{
	int retVal;

	OpenGroundInfoTexture();

	// okay, terrain types:
	//		grass, swamp, water, snow, desert
	D3DCOLOR water      = D3DCOLOR_RGBA(  0,  0,255,255) ;
	D3DCOLOR desert     = D3DCOLOR_RGBA(255,255,  0,255) ;
	D3DCOLOR snow       = D3DCOLOR_RGBA(255,255,255,255) ;
	D3DCOLOR swamp      = D3DCOLOR_RGBA(255,  0,  0,255) ;
	D3DCOLOR forest     = D3DCOLOR_RGBA(  0,128,  0,255) ;
	D3DCOLOR deepForest = D3DCOLOR_RGBA(  0,255,  0,255) ;

	D3DCOLOR pixel = GroundInfoTexturePixel(j,i);

	if (pixel == water)
		retVal = 6 ; // water
	else if (pixel == snow)
		retVal = 5 ; // snow
	else if (pixel == desert)
		retVal = 4 ; // desert
	else if (pixel == swamp)
		retVal = 3 ; // dark grass
	else if (pixel == forest)
		retVal = 1 ; // grass
	else if (pixel == deepForest)
		retVal = 1 ; // dark grass
	else
		retVal = 6 ; // beach

	CloseGroundInfoTexture();

	return retVal;
}

//***************************************************************************************
D3DXVECTOR3 GroundObject::GetVertNormal(int i, int j)
{

	D3DXVECTOR3 retVal;
	retVal = D3DXVECTOR3(0.0f, -1.0f, 0.0f); 
   D3DXVec3Normalize( &retVal, &retVal );

	return retVal;
}

//***************************************************************************************
D3DXVECTOR3 GroundObject::SetNormal(int x, int y, D3DXVECTOR3 point)
{

	D3DXVECTOR3 retVal, norm;
	retVal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 

	for (int i = 0; i < sizeH; ++i)
	{

		if (abs(i - y) < 3)
		{
			GROUNDVERTEXSTRUCT* pVertices;
			if( FAILED( vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
				return retVal;

			for (int j = 0; j < numOfVertices/3; ++j)
			{
				if (abs((j/2) - x) < 3)
				{
					if (pVertices[j*3+0].position == point ||
						 pVertices[j*3+1].position == point ||
						 pVertices[j*3+2].position == point)
					{
						D3DXVec3Cross( &norm, 
									  &(pVertices[j*3+2].position-pVertices[j*3+1].position), 
									  &(pVertices[j*3+1].position-pVertices[j*3+0].position) );
						retVal.x = norm.x;
						retVal.y = norm.y * -1;
						retVal.z = norm.z;
					}
				}
			}
			vertPtrArray[i]->Unlock();
		}
	}

   D3DXVec3Normalize( &retVal, &retVal );

	return retVal;
}

//***************************************************************************************
void GroundObject::Generate(LPDIRECT3DDEVICE8 m_pd3dDevice, int w, int h, unsigned long randSeed)
{

	if (0 == randSeed)
		randSeed = rand();

	// *** delete old, valid stuff
   SAFE_DELETE(pt);

	w = h = 128;

	sizeW = w;
	sizeH = h;

	// *** create fractal height map
//	pt = new PlasmaTexture();
//	pt->Generate(m_pd3dDevice, sizeW, sizeH, randSeed);

	LPDIRECT3DTEXTURE8 pTexture; // Our texture
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\terrain-new.png",
				0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
				D3DX_DEFAULT, D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture);
//   D3DXCreateTextureFromFile( m_pd3dDevice, "dat\\terrain-new.bmp", &pTexture );

	D3DLOCKED_RECT lockInfo;

//	pt->pTexture->LockRect(0, &lockInfo, NULL, 0);
	pTexture->LockRect(0, &lockInfo, NULL, 0);


	// *** create triangles
	InitVertArray(m_pd3dDevice);

	float topUV = 4.0f;

   GROUNDVERTEXSTRUCT* pVertices;
	OpenGroundInfoTexture();

   // Fill the vertex buffer. We are setting the tu and tv texture
   // coordinates, which range from 0.0 to topUV
	for (int i = 0; i < sizeH; ++i)
	{
		char *charPtr = (char *)lockInfo.pBits + lockInfo.Pitch * i;
		D3DCOLOR *pixelPtr = (D3DCOLOR *) charPtr;
		D3DCOLOR *pixelPtrAhead = NULL;
		if (i+1 < sizeH)
			pixelPtrAhead = (D3DCOLOR *) ((char *)lockInfo.pBits + lockInfo.Pitch * (i+1));



		if( FAILED( vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
			return;

		for (int j = 0; j < sizeW; ++j)
		{

			int height[4];
			height[0] = pixelPtr[j] & 0xff;
			if (j+1 < sizeW)
				height[1] = pixelPtr[j+1] & 0xff;
			else
				height[1] = 0;

			if (pixelPtrAhead)
			{
				height[2] = pixelPtrAhead[j] & 0xff;
				if (j+1 < sizeW)
					height[3] = pixelPtrAhead[j+1] & 0xff;
				else
					height[3] = 0;
			}
			else
			{
				height[2] = height[3] = 0;
			}

			int tri = 0;

			pVertices[3*(j*2+tri)+0].position = GetVertPos   (i, j, height[0]);
			pVertices[3*(j*2+tri)+0].normal   = GetVertNormal(i, j);
			pVertices[3*(j*2+tri)+0].color    = GetVertColor(i, j, height[0]);
			pVertices[3*(j*2+tri)+0].tu      = 0.0f;
			pVertices[3*(j*2+tri)+0].tv      = 0.0f;

			pVertices[3*(j*2+tri)+1].position = GetVertPos   (i+1, j, height[2]);
			pVertices[3*(j*2+tri)+1].normal   = GetVertNormal(i+1, j);
			pVertices[3*(j*2+tri)+1].color    = GetVertColor(i+1, j, height[2]);
			pVertices[3*(j*2+tri)+1].tu      = 0.0f;
			pVertices[3*(j*2+tri)+1].tv      = topUV;

			pVertices[3*(j*2+tri)+2].position = GetVertPos   (i, j+1, height[1]);
			pVertices[3*(j*2+tri)+2].normal   = GetVertNormal(i, j+1);
			pVertices[3*(j*2+tri)+2].color    = GetVertColor(i, j+1, height[1]);
			pVertices[3*(j*2+tri)+2].tu      = topUV;
			pVertices[3*(j*2+tri)+2].tv      = 0.0f;

			tri = 1;

			pVertices[3*(j*2+tri)+0].position = GetVertPos   (i+1, j+1, height[3]);
			pVertices[3*(j*2+tri)+0].normal   = GetVertNormal(i+1, j+1);
			pVertices[3*(j*2+tri)+0].color    = GetVertColor(i+1, j+1, height[3]);
			pVertices[3*(j*2+tri)+0].tu      = topUV;
			pVertices[3*(j*2+tri)+0].tv      = topUV;

			pVertices[3*(j*2+tri)+1].position = GetVertPos   (i, j+1, height[1]);
			pVertices[3*(j*2+tri)+1].normal   = GetVertNormal(i, j+1);
			pVertices[3*(j*2+tri)+1].color    = GetVertColor(i, j+1, height[1]);
			pVertices[3*(j*2+tri)+1].tu      = topUV;
			pVertices[3*(j*2+tri)+1].tv      = 0.0f;

			pVertices[3*(j*2+tri)+2].position = GetVertPos   (i+1, j, height[2]);
			pVertices[3*(j*2+tri)+2].normal   = GetVertNormal(i+1, j);
			pVertices[3*(j*2+tri)+2].color    = GetVertColor(i+1, j, height[2]);
			pVertices[3*(j*2+tri)+2].tu      = 0.0f;
			pVertices[3*(j*2+tri)+2].tv      = topUV;
		}

		vertPtrArray[i]->Unlock();
	}
	
	pTexture->UnlockRect(0);
	SAFE_RELEASE(pTexture);
	CloseGroundInfoTexture();


	// reset normals for each point
	for (int i = 0; i < sizeH; ++i)
	{

		for (int j = 0; j < numOfVertices; ++j)
		{

			if( FAILED( vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
				return;

			D3DXVECTOR3 point = pVertices[j].position;

			vertPtrArray[i]->Unlock();

			point = SetNormal(j/6,i,point);

			if( FAILED( vertPtrArray[i]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
				return;

			pVertices[j].normal = point;

			vertPtrArray[i]->Unlock();

		}

	}

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

}

//***************************************************************************************
void GroundObject::InitVertArray(LPDIRECT3DDEVICE8 m_pd3dDevice)
{
	ReleaseVertArray();

	vertPtrArray = new LPDIRECT3DVERTEXBUFFER8[sizeH];

	for (int i = 0; i < sizeH; i++)
		vertPtrArray[i] = NULL;

	numOfVertices = sizeW * 2 * 3;

	for (int i = 0; i < sizeH; i++)
	{
	   if( FAILED( m_pd3dDevice->CreateVertexBuffer( numOfVertices*sizeof(GROUNDVERTEXSTRUCT),
		                          0, GROUND_VERTEXDESC,
			                       D3DPOOL_MANAGED, &(vertPtrArray[i]) ) ) )
	   {
			ReleaseVertArray();
			return;
	   }

	}



}

//***************************************************************************************
void GroundObject::ReleaseVertArray(void)
{
	if (vertPtrArray)
	{
		for (int i = 0; i < sizeH; i++)
		{
			SAFE_RELEASE(vertPtrArray[i]);
		}

		delete[] vertPtrArray;
		vertPtrArray = NULL;
	}

}

//***************************************************************************************
void GroundObject::Draw(int x, int y)
{
//   puma->m_pd3dDevice->SetTexture( 0, NULL );
	if (pTexture)
	{
	   puma->m_pd3dDevice->SetTexture( 0, pTexture );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	}
	puma->m_pd3dDevice->SetMaterial(&groundMaterial);
//	puma->ClearMaterial();


	if (vertPtrArray)
	{
		for (int i = 0; i < sizeH; i++)
		{
//			SAFE_RELEASE(vertPtrArray[i]);
			if (abs(i-y) < 10)
			{
				int startTri = (x - 10) * 2;
				int endTri = (x + 10) * 2;
				if (startTri < 0)
					startTri = 0;
				if (endTri >= sizeW * 2)
					endTri = sizeW * 2 - 1;
			   puma->m_pd3dDevice->SetStreamSource( 0, vertPtrArray[i], sizeof(GROUNDVERTEXSTRUCT) );
			   puma->m_pd3dDevice->SetVertexShader( GROUND_VERTEXDESC );
			   puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 
//					   0, sizeW * 2 );
					   startTri * 3, (endTri - startTri) );
			}
		}
	}

}

//***************************************************************************************
void GroundObject::DrawFog(void)
{
   puma->m_pd3dDevice->SetTexture( 0, NULL );
	/*
	if (pTexture)
	{
	   puma->m_pd3dDevice->SetTexture( 0, pTexture );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	}
	*/
//	puma->m_pd3dDevice->SetMaterial(&groundMaterial);
	puma->ClearMaterial();

   D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	puma->m_pd3dDevice->SetTransform( D3DTS_VIEW , &matWorld );

//	fog3->Draw(puma->m_pd3dDevice);
//	fog2->Draw(puma->m_pd3dDevice);
//	fog1->Draw(puma->m_pd3dDevice);
}

//***************************************************************************************
void GroundObject::LoadTexture(LPDIRECT3DDEVICE8 m_pd3dDevice, char *fileName)
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

//*******************************************************************************
float GroundObject::GetXForPoint(int gridX)
{
   return gridX * CellSize();
}

//*******************************************************************************
float GroundObject::GetYForPoint(int x, int y)
{
   while (x < 0)
      x += sizeW;
   while (y < 0)
      y += sizeH;
   while (x >= sizeW)
      x -= sizeW;
   while (y >= sizeH)
      y -= sizeH;

   GROUNDVERTEXSTRUCT* pVertices;
	if( FAILED( vertPtrArray[y]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return 0.0f;

	float retVal = pVertices[3*2*x].position.y;

	vertPtrArray[y]->Unlock();

   return retVal;
}

//*******************************************************************************
void GroundObject::SetYForPoint(int x, int y, float newH)
{
   while (x < 0)
      x += sizeW;
   while (y < 0)
      y += sizeH;
   while (x >= sizeW)
      x -= sizeW;
   while (y >= sizeH)
      y -= sizeH;

//   mapData[y * sizeW + x] = (newH + 480 - 80) / MAP_DATA_HEIGHT_COEFF;
}

//*******************************************************************************
float GroundObject::GetZForPoint(int gridY)
{
   return gridY * CellSize();
}

//*******************************************************************************
int GroundObject::GetGridX(float x)
{
   return (int) (x) / CellSize();
}

//*******************************************************************************
int GroundObject::GetGridY(float z)
{
   return (int) (z) / CellSize();
}

//*******************************************************************************
float GroundObject::CellSize(void)
{

   return 10.0f;

}

//*******************************************************************************
float GroundObject::HeightAtPoint(float pointX, float pointZ, D3DXVECTOR3 *targetNormal)
{

   D3DXVECTOR3 mapPoint, targetPoint, firstTriBase, secondTriBase, triPoint[3];

   triPoint[1].x = 0.0;
   triPoint[1].z = 0.0;
   triPoint[1].y = 0.0;

   triPoint[0].x = 100.0;
   triPoint[0].z = 0.0;
   triPoint[0].y = 0.0;

   triPoint[2].x = 100.0;
   triPoint[2].z = 100.0;
   triPoint[2].y = 0.0;

   targetPoint = CalculateNormal(triPoint[0], triPoint[1], triPoint[2]);

   float test = GetGroundHeight(triPoint[0], triPoint[1], triPoint[2],
                        targetPoint, 0.0, 0.0);


   float cellSize = CellSize();

   // Find the triangle we're gonna be in
   int cellX = GetGridX(pointX);
   int cellY = GetGridY(pointZ);
   int x2, y2;
   float height = 0.0; //, h1, h2, h3;
   float divisor = 0;
//   float firstTriDist, secondTriDist;

   targetPoint.x = pointX;
   targetPoint.z = pointZ;
   targetPoint.y = 0;
   mapPoint.y    = 0;

   firstTriBase.y    = 0;
   secondTriBase.y   = 0;

   // normalize targetPoint
   targetPoint.x = (pointX - cellX * cellSize) / cellSize;
   targetPoint.z = (pointZ - cellY * cellSize) / cellSize;

   GROUNDVERTEXSTRUCT* pVertices;
	if( FAILED( vertPtrArray[cellY]->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return 0.0f;

   if (targetPoint.x + targetPoint.z <= 1.0)
//   if ((1.0 - targetPoint.x) + targetPoint.z <= 1.0)
//   if ((1.0-targetPoint.x) + (1.0-targetPoint.z) <= 1.0)
   {
	   triPoint[0] = pVertices[cellX * 6 + 0].position;
	   triPoint[1] = pVertices[cellX * 6 + 1].position;
	   triPoint[2] = pVertices[cellX * 6 + 2].position;
   }
   else
   {
	   triPoint[0] = pVertices[cellX * 6 + 3].position;
	   triPoint[1] = pVertices[cellX * 6 + 4].position;
	   triPoint[2] = pVertices[cellX * 6 + 5].position;
   }

   vertPtrArray[cellY]->Unlock();

   D3DXVECTOR3 normal = CalculateNormal(triPoint[0], triPoint[1], triPoint[2]);
   if (targetNormal)
	  *targetNormal = normal;

   return GetGroundHeight(triPoint[0], triPoint[1], triPoint[2], normal, pointX, pointZ);



}

//*******************************************************************************
// circle of 4, then offset circle of 4, then circle of 8, then circle of 12
void GroundObject::GetSlotPosition(int slotIndex, float &x, float &y, float &ang)
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

	x = CellSize()/2.0f + sin(angle) * radius;
	y = CellSize()/2.0f + cos(angle) * radius;

	ang = -1 * angle;

}

//*******************************************************************************
int GroundObject::GetFirstOpenSlot(int x, int y, int backwards)
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
void GroundObject::ClaimSlot(int x, int y, int index, int type)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	slot->used[index] = type;
}

//*******************************************************************************
void GroundObject::ReleaseSlot(int x, int y, int index)
{
	LocationSlots *slot = &(slotArray[y*sizeW+x]);

	slot->used[index] = FALSE;
}

//*******************************************************************************
void GroundObject::CreateStaticPositions(void)
{
	srand(1);

	// add town buildings
	for (int t = 0; t < 12; ++t)
	{
		for (int t2 = 0; t2 < townList[t].size * 2; ++t2)
		{
			LocationSlots *slot;
			int x,y;
			do
			{
				x = townList[t].x + (rand() % (1 + townList[t].size)) - townList[t].size/2;
				y = townList[t].y + (rand() % (1 + townList[t].size)) - townList[t].size/2;
				slot = &(slotArray[y*sizeW+x]);
			} while ((x == townList[t].x && y == townList[t].y) ||
						slot->used[NUM_OF_SLOTS_PER_SPACE- 1]> 0);

			slot = &(slotArray[y*sizeW+x]);
//			slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 4) - 1] = SLOT_CABIN + (rand() % 2);
			slot->used[NUM_OF_SLOTS_PER_SPACE- 1] = SLOT_CABIN + (rand() % 2);
		}
	}

	OpenGroundInfoTexture();

	D3DCOLOR water      = D3DCOLOR_RGBA(  0,  0,255,255) ;
	D3DCOLOR desert     = D3DCOLOR_RGBA(255,255,  0,255) ;
	D3DCOLOR snow       = D3DCOLOR_RGBA(255,255,255,255) ;
	D3DCOLOR swamp      = D3DCOLOR_RGBA(255,  0,  0,255) ;
	D3DCOLOR forest     = D3DCOLOR_RGBA(  0,128,  0,255) ;
	D3DCOLOR deepForest = D3DCOLOR_RGBA(  0,255,  0,255) ;

	for (int y = 0; y < sizeH; ++y)
	{
		for (int x = 0; x < sizeW; ++x)
		{

			LocationSlots *slot = &(slotArray[y*sizeW+x]);

			D3DCOLOR pixel = GroundInfoTexturePixel(x,y);

			if (pixel == water)
				; // add nothing
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
//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
//				slot->used[NUM_OF_SLOTS_PER_SPACE - (rand() % 8) - 1] = SLOT_TREE1 + (rand() % 2);
			}
			

		}
	}

	CloseGroundInfoTexture();

	srand(timeGetTime());

}




/* end of file */
