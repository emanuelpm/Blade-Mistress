//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "puma.h"
#include "pumamesh.h"
#include "pumaAnim.h"
#include "..\helper\GeneralUtils.h"
#include "d3dutil.h"
#include "pumaRawVerts.h"

#include "..\helper\autolog.h"


D3DXVECTOR3 collTestPos[100];
int collTestCount;

//***************************************************************************************
PumaMesh::PumaMesh(int doid, char *doname)	 : DataObject(doid,doname)
{

	isReady = FALSE;

	position = D3DXVECTOR3( 0,0,0 );
	rotation = D3DXVECTOR3( 0,0,0 );

	numOfVertices = 0;
	vertexList = texture = NULL;

   ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
   mtrl.Diffuse.r = 0.5f;
   mtrl.Diffuse.g = 0.5f;
   mtrl.Diffuse.b = 0.5f;
   mtrl.Diffuse.a = 0.5f;
   mtrl.Ambient.r = 0.3f;
   mtrl.Ambient.g = 0.3f;
   mtrl.Ambient.b = 0.3f;
   mtrl.Ambient.a = 0.3f;


}

//***************************************************************************************
PumaMesh::PumaMesh(void) : DataObject(0,"PUMAMESH")
{

	isReady = FALSE;

	position = D3DXVECTOR3( 0,0,0 );
	rotation = D3DXVECTOR3( 0,0,0 );

	numOfVertices = 0;
	vertexList = texture = NULL;

   ZeroMemory( &mtrl, sizeof(D3DMATERIAL8) );
   mtrl.Diffuse.r = 0.5f;
   mtrl.Diffuse.g = 0.5f;
   mtrl.Diffuse.b = 0.5f;
   mtrl.Diffuse.a = 0.5f;
   mtrl.Ambient.r = 0.3f;
   mtrl.Ambient.g = 0.3f;
   mtrl.Ambient.b = 0.3f;
   mtrl.Ambient.a = 0.3f;
}

//***************************************************************************************
PumaMesh::~PumaMesh()
{

	vdMan->DeleteObject(vertexList);
	vdMan->DeleteObject(texture);

}

//***************************************************************************************
void PumaMesh::LoadFromASC(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName)
{

	FILE *fp;

	int numFaces   = 0;
	int numVertPos = 0;
	float *vertPosArray = NULL;
	int numVertUVs = 0;
	float *vertUVArray  = NULL;
	PUMAMESHVERTEXSTRUCT* pVertices;
	float transparency = 0;

	int tempInt, tempIndex, curFaceForNormals, curVertForNormals;
	float normalVals[3];
	int faceRead = 0;

	fp = fopen(fileName,"r");

	char fileBuffer[1028];
	int done = FALSE;

	while (!done)
	{
		int result = fscanf(fp,"%s",fileBuffer);

//	*MESH_NUMVERTEX 20
//		*MESH_NUMFACES 36
		if (!strncmp("*MESH_NUMVERTEX",fileBuffer,strlen("*MESH_NUMVERTEX")))
		{
			result = fscanf(fp,"%d",&numVertPos);
			vertPosArray = new float[numVertPos * 3];
		}
		else if (!strncmp("*MESH_VERTEXNORMAL",fileBuffer,strlen("*MESH_VERTEXNORMAL")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%f",&(normalVals[0]));
			result = fscanf(fp,"%f",&(normalVals[2]));
			result = fscanf(fp,"%f",&(normalVals[1]));

			pVertices[curFaceForNormals * 3 + curVertForNormals].normal =
							D3DXVECTOR3(normalVals[0],
										normalVals[1],
										normalVals[2]);
			curVertForNormals++;

		}
		else if (!strncmp("*MESH_VERTEX_LIST",fileBuffer,strlen("*MESH_VERTEX_LIST")))
		{
			;
		}
		else if (!strncmp("*MESH_VERTEX",fileBuffer,strlen("*MESH_VERTEX")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 0]));
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 2]));
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 1]));
//			vertPosArray = new float[numVertPos * 3];
		}
		else if (!strncmp("*MESH_NUMTVERTEX",fileBuffer,strlen("*MESH_NUMTVERTEX")))
		{
			result = fscanf(fp,"%d",&numVertUVs);
			vertUVArray = new float[numVertUVs * 2];
		}
		else if (!strncmp("*MESH_TVERTLIST",fileBuffer,strlen("*MESH_TVERTLIST")))
		{
		}
		else if (!strncmp("*MESH_TVERT",fileBuffer,strlen("*MESH_TVERT")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%f",&(vertUVArray[tempIndex * 2 + 0]));
			result = fscanf(fp,"%f",&(vertUVArray[tempIndex * 2 + 1]));
		}
		else if (!strncmp("*MESH_NUMFACES",fileBuffer,strlen("*MESH_NUMFACES")))
		{
			result = fscanf(fp,"%d",&numFaces);
			numOfVertices = numFaces * 3;
			// Create the vertex buffer.
			vdMan->DeleteObject(vertexList);

			vertexList = new VideoDataObject(
				                      numOfVertices*sizeof(PUMAMESHVERTEXSTRUCT), 
											 PUMAMESH_VERTEXDESC);
			vdMan->AddObject(vertexList);

			// Prepare to fill the vertex buffer, by locking it.
			pVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

		}
		else if (!strncmp("*MESH_FACE_LIST",fileBuffer,strlen("*MESH_FACE_LIST")))
		{
			;
		}
		else if (!strncmp("*MESH_FACENORMAL",fileBuffer,strlen("*MESH_FACENORMAL")))
		{
			result = fscanf(fp,"%d",&curFaceForNormals);
			curVertForNormals = 0;
			
		}
		else if (!strncmp("*MATERIAL_TRANSPARENCY",fileBuffer,strlen("*MATERIAL_TRANSPARENCY")))
		{
			result = fscanf(fp,"%f",&transparency);
			
		}
		else if (!strncmp("*MESH_FACE",fileBuffer,strlen("*MESH_FACE")))
		{
			result = fscanf(fp,"%s",fileBuffer);
			result = fscanf(fp,"%s",fileBuffer);
			result = fscanf(fp,"%d",&tempInt);
			pVertices[faceRead * 3 + 0].position =
							D3DXVECTOR3(vertPosArray[tempInt * 3 + 0],
										vertPosArray[tempInt * 3 + 1],
										vertPosArray[tempInt * 3 + 2]);

			result = fscanf(fp,"%s",fileBuffer);
			result = fscanf(fp,"%d",&tempInt);
			pVertices[faceRead * 3 + 1].position =
							D3DXVECTOR3( vertPosArray[tempInt * 3 + 0],
										vertPosArray[tempInt * 3 + 1],
										vertPosArray[tempInt * 3 + 2]);

			result = fscanf(fp,"%s",fileBuffer);
			result = fscanf(fp,"%d",&tempInt);
			pVertices[faceRead * 3 + 2].position =
							D3DXVECTOR3( vertPosArray[tempInt * 3 + 0],
										vertPosArray[tempInt * 3 + 1],
										vertPosArray[tempInt * 3 + 2]);

			pVertices[faceRead*3+0].color   = D3DCOLOR_RGBA(255,255,255,(int)(255 * (1.0 - transparency)));
			pVertices[faceRead*3+1].color   = D3DCOLOR_RGBA(255,255,255,(int)(255 * (1.0 - transparency)));
			pVertices[faceRead*3+2].color   = D3DCOLOR_RGBA(255,255,255,(int)(255 * (1.0 - transparency)));
			faceRead++;

		}
		else if (!strncmp("*MESH_TFACELIST",fileBuffer,strlen("*MESH_TFACELIST")))
		{
			;
		}
		else if (!strncmp("*MESH_TFACE",fileBuffer,strlen("*MESH_TFACE")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%d",&tempInt);
			pVertices[tempIndex * 3 + 0].tu = vertUVArray[tempInt * 2 + 0];
			pVertices[tempIndex * 3 + 0].tv = vertUVArray[tempInt * 2 + 1];

			result = fscanf(fp,"%d",&tempInt);
			pVertices[tempIndex * 3 + 1].tu = vertUVArray[tempInt * 2 + 0];
			pVertices[tempIndex * 3 + 1].tv = vertUVArray[tempInt * 2 + 1];

			result = fscanf(fp,"%d",&tempInt);
			pVertices[tempIndex * 3 + 2].tu = vertUVArray[tempInt * 2 + 0];
			pVertices[tempIndex * 3 + 2].tv = vertUVArray[tempInt * 2 + 1];


		}

		if (EOF == result)
			done = TRUE;
	}

	fclose(fp);

	delete[] vertPosArray;
	delete[] vertUVArray;


	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::LoadTestPattern(LPDIRECT3DDEVICE8 pd3dDevice)
{

	numOfVertices = 2 * 3;
   // Create the vertex buffer.
	vdMan->DeleteObject(vertexList);

	vertexList = new VideoDataObject(
				                      numOfVertices*sizeof(PUMAMESHVERTEXSTRUCT), 
											 PUMAMESH_VERTEXDESC);
	vdMan->AddObject(vertexList);

   // Fill the vertex buffer. We are setting the tu and tv texture
   // coordinates, which range from 0.0 to 1.0
   PUMAMESHVERTEXSTRUCT* pVertices;
	pVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int tri = 0;

   pVertices[3*tri+0].position = D3DXVECTOR3( 1.0f, 0.0f, 1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0xffffffff;
   pVertices[3*tri+0].tu      = 0.0f;
   pVertices[3*tri+0].tv      = 0.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0xffffffff;
   pVertices[3*tri+1].tu      = 0.0f;
   pVertices[3*tri+1].tv      = 1.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0xffffffff;
   pVertices[3*tri+2].tu      = 1.0f;
   pVertices[3*tri+2].tv      = 0.0f;

	tri = 1;

   pVertices[3*tri+0].position = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0xffffffff;
   pVertices[3*tri+0].tu      = 1.0f;
   pVertices[3*tri+0].tv      = 1.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0xffffffff;
   pVertices[3*tri+1].tu      = 1.0f;
   pVertices[3*tri+1].tv      = 0.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0xffffffff;
   pVertices[3*tri+2].tu      = 0.0f;
   pVertices[3*tri+2].tv      = 1.0f;

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int alphaType)
{

	aLog.Log("PumaMesh::LoadTexture ");
	aLog.Log(fileName);
	aLog.Log("\n");

	vdMan->DeleteObject(texture);

	DWORD alpha = 0;
	if (1 == alphaType)
		alpha = 0xff000000;

	texture = new VideoDataObject(fileName, alpha);
	vdMan->AddObject(texture);

}

//***************************************************************************************
void PumaMesh::Assimilate(LPDIRECT3DDEVICE8 pd3dDevice, PumaMesh *pm)
{
	PUMAMESHVERTEXSTRUCT *pNewVertices, *pMyVertices;
	int totalVerts = pm->numOfVertices + numOfVertices;

	VideoDataObject *newBuffer; // Buffer to hold vertices

	newBuffer = new VideoDataObject(	totalVerts*sizeof(PUMAMESHVERTEXSTRUCT), 
											 PUMAMESH_VERTEXDESC);
	vdMan->AddObject(newBuffer);

	// Prepare to fill the vertex buffer, by locking it.
	pNewVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(newBuffer);

	int i = 0;
	// copy my verts first.
   if(vertexList)
		//&& !FAILED( vertexBufferPtr->Lock( 0, 0, (BYTE**)&pMyVertices, 0 ) ) )
	{
		pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

		for (i = 0; i < numOfVertices; ++i)
		{
			pNewVertices[i] = pMyVertices[i];
		}

		vdMan->UnlockVerts(vertexList);
	}
	// now copy the verts from pm.
	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(pm->vertexList);

	for (; i < totalVerts; ++i)
	{
		pNewVertices[i] = pMyVertices[i-numOfVertices];
	}

	vdMan->UnlockVerts(pm->vertexList);

	// clean up
	vdMan->UnlockVerts(newBuffer);

	vdMan->DeleteObject(vertexList);

	vertexList = newBuffer;
	numOfVertices   = totalVerts;

}

//***************************************************************************************
void PumaMesh::Draw(LPDIRECT3DDEVICE8 pd3dDevice, LPDIRECT3DTEXTURE8 otherArt)
{

   // Setup our texture. Using textures introduces the texture stage states,
   // which govern how textures get blended together (in the case of multiple
   // textures) and lighting information. In this case, we are modulating
   // (blending) our texture with the diffuse color of the vertices.
   pd3dDevice->SetMaterial( &mtrl );
	if ((texture && texture->isReady) || otherArt)
	{
	   int res;
		if (otherArt)
			res = pd3dDevice->SetTexture( 0, otherArt );
		else
		   res = pd3dDevice->SetTexture( 0, texture->pTexture );
		assert(D3D_OK == res);
		SetTextureColorStage( pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
//		SetTextureColorStage( pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG2  , D3DTA_DIFFUSE );
		SetTextureAlphaStage( pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
      pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
      pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
      pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
      pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
      pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
      pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );

//	   pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);

	}
   // Render the vertex buffer contents
	if (vertexList->isReady)
	{
	   pd3dDevice->SetStreamSource( 0, vertexList->vertexBufferPtr, sizeof(PUMAMESHVERTEXSTRUCT) );
		pd3dDevice->SetVertexShader( PUMAMESH_VERTEXDESC );

		for (int countBase = 0; countBase < numOfVertices / 3;
			  countBase += puma->m_d3dCaps.MaxPrimitiveCount)
		{
			int numOut = numOfVertices / 3 - countBase;
			if (puma->m_d3dCaps.MaxPrimitiveCount < numOfVertices / 3 - countBase)
				numOut = puma->m_d3dCaps.MaxPrimitiveCount;
			puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
		}

//	   pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0, numOfVertices/3 );
	}

//   pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
//   pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

}


//***************************************************************************************
void PumaMesh::Scale(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].position.x *= x;
		pMyVertices[i].position.y *= y;
		pMyVertices[i].position.z *= z;
	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::Shift(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].position.x += x;
		pMyVertices[i].position.y += y;
		pMyVertices[i].position.z += z;
	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::Mirror(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;
	PUMAMESHVERTEXSTRUCT tempVert;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; i += 3)
	{
		// flip
		pMyVertices[i+0].position.x *= -1;
		pMyVertices[i+1].position.x *= -1;
		pMyVertices[i+2].position.x *= -1;

		// adjust vert normals
		pMyVertices[i+0].normal.x *= -1;
		pMyVertices[i+1].normal.x *= -1;
		pMyVertices[i+2].normal.x *= -1;

		// adjust face normal
		tempVert         = pMyVertices[i+1];
		pMyVertices[i+1] = pMyVertices[i+2];
		pMyVertices[i+2] = tempVert;

	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::MirrorZ(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;
	PUMAMESHVERTEXSTRUCT tempVert;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; i += 3)
	{
		// flip
		pMyVertices[i+0].position.z *= -1;
		pMyVertices[i+1].position.z *= -1;
		pMyVertices[i+2].position.z *= -1;

		// adjust vert normals
		pMyVertices[i+0].normal.z *= -1;
		pMyVertices[i+1].normal.z *= -1;
		pMyVertices[i+2].normal.z *= -1;

		// adjust face normal
		tempVert         = pMyVertices[i+1];
		pMyVertices[i+1] = pMyVertices[i+2];
		pMyVertices[i+2] = tempVert;

	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
PumaMesh *PumaMesh::MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PumaMesh *p = new PumaMesh(WhatAmI(),"PUMAMESH_COPY");

	PUMAMESHVERTEXSTRUCT *pNewVertices, *pMyVertices;
	*p = *this;
	p->texture = NULL;

	p->vertexList = new VideoDataObject(
				                      numOfVertices*sizeof(PUMAMESHVERTEXSTRUCT), 
											 PUMAMESH_VERTEXDESC);
	vdMan->AddObject(p->vertexList);

	// Prepare to fill the vertex buffer, by locking it.
	pNewVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(p->vertexList);

	// copy my verts first.
	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		pNewVertices[i] = pMyVertices[i];
	}

	vdMan->UnlockVerts(vertexList);
	vdMan->UnlockVerts(p->vertexList);

	return p;

}

//***************************************************************************************
void PumaMesh::Save(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals)
{
	if (resetNormals)
		RebuildNormals(pd3dDevice);

	PUMAMESHVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;

	if (!fileName)
	{
      OPENFILENAME ofn;
	   memset(&ofn, 0, sizeof(OPENFILENAME));

	   ofn.lStructSize = sizeof(OPENFILENAME);
	   shipFileName[0] = 0;
	   ofn.lpstrInitialDir = ".\\";
	   ofn.lpstrFile= shipFileName;
	   ofn.nMaxFile = sizeof(shipFileName);
	   ofn.lpstrFilter = "PUMA MESH files\0*.pum\0\0";
      ofn.hwndOwner = (HWND) puma->m_hWnd;
//	   ofn.Flags = OFN_NOCHANGEDIR;

  	   if (!GetSaveFileName(&ofn))
			return;

		fileName = shipFileName;

      int len = strlen(shipFileName);
      if (len <= 0)
         return;
		
      int tempLen = 0;
		
      while(tempLen < len && shipFileName[tempLen] != '.')
         tempLen++;
		
      char tempString[256];
		
      if (tempLen < len)
         shipFileName[tempLen] = 0;
		
      sprintf(tempString,"%s.pum",shipFileName);
      sprintf(shipFileName,"%s",tempString);


	}

	fp = fopen(fileName,"wb");
	if (!fp)
		return;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	fwrite(&numOfVertices, 1, sizeof(numOfVertices), fp);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		fwrite(&(pMyVertices[i]), 1, sizeof(PUMAMESHVERTEXSTRUCT), fp);
	}

	vdMan->UnlockVerts(vertexList);
	fclose(fp);

}


//***************************************************************************************
void PumaMesh::Load(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName)
{
	aLog.Log("PumaMesh::Load ");
	aLog.Log(fileName);

	PUMAMESHVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;

	if (!fileName)
	{
      OPENFILENAME ofn;
	   memset(&ofn, 0, sizeof(OPENFILENAME));

	   ofn.lStructSize = sizeof(OPENFILENAME);
	   shipFileName[0] = 0;
	   ofn.lpstrInitialDir = ".\\";
	   ofn.lpstrFile= shipFileName;
	   ofn.nMaxFile = sizeof(shipFileName);
	   ofn.lpstrFilter = "PUMA MESH files\0*.pum\0\0";
      ofn.hwndOwner = (HWND) puma->m_hWnd;
	   ofn.Flags = OFN_NOCHANGEDIR;

  	   if (!GetOpenFileName(&ofn))
			return;

		fileName = shipFileName;

      int len = strlen(shipFileName);
      if (len <= 0)
         return;
		
      int tempLen = 0;
		
      while(tempLen < len && shipFileName[tempLen] != '.')
         tempLen++;
		
      char tempString[256];
		
      if (tempLen < len)
         shipFileName[tempLen] = 0;
		
      sprintf(tempString,"%s.pum",shipFileName);
      sprintf(shipFileName,"%s",tempString);



	}

	fp = fopen(fileName,"rb");
	if (!fp)
		return;

	vdMan->DeleteObject(vertexList);
	vdMan->DeleteObject(texture);

	fread(&numOfVertices, 1, sizeof(numOfVertices), fp);

	vertexList = new VideoDataObject(
				                      numOfVertices*sizeof(PUMAMESHVERTEXSTRUCT), 
											 PUMAMESH_VERTEXDESC);
	vdMan->AddObject(vertexList);

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		fread(&(pMyVertices[i]), 1, sizeof(PUMAMESHVERTEXSTRUCT), fp);
	}

	vdMan->UnlockVerts(vertexList);
	fclose(fp);

	aLog.Log(" DONE\n");

}

//***************************************************************************************
void PumaMesh::Center(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	D3DXVECTOR3 least, most;
	least.x = least.y = least.z =  10000.0f;
	most.x  = most.y  = most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (pMyVertices[i].position.x > most.x)
			 most.x  = pMyVertices[i].position.x;
		if (pMyVertices[i].position.x < least.x)
			 least.x = pMyVertices[i].position.x;

		if (pMyVertices[i].position.y > most.y)
			 most.y  = pMyVertices[i].position.y;
		if (pMyVertices[i].position.y < least.y)
			 least.y = pMyVertices[i].position.y;

		if (pMyVertices[i].position.z > most.z)
			 most.z  = pMyVertices[i].position.z;
		if (pMyVertices[i].position.z < least.z)
			 least.z = pMyVertices[i].position.z;
	}

	// figure out delta to center
	D3DXVECTOR3 center;
	center.x = most.x - ((most.x - least.x) / 2);
	center.y = most.y - ((most.y - least.y) / 2);
	center.z = most.z - ((most.z - least.z) / 2);

	// apply that delta!
	for (int i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].position.x -= center.x;
		pMyVertices[i].position.y -= center.y;
		pMyVertices[i].position.z -= center.z;
	}


	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::GetBounds(LPDIRECT3DDEVICE8 pd3dDevice, D3DXVECTOR3 &least, D3DXVECTOR3 &most)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	least.x = least.y = least.z =  10000.0f;
	most.x  = most.y  = most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (pMyVertices[i].position.x > most.x)
			 most.x  = pMyVertices[i].position.x;
		if (pMyVertices[i].position.x < least.x)
			 least.x = pMyVertices[i].position.x;

		if (pMyVertices[i].position.y > most.y)
			 most.y  = pMyVertices[i].position.y;
		if (pMyVertices[i].position.y < least.y)
			 least.y = pMyVertices[i].position.y;

		if (pMyVertices[i].position.z > most.z)
			 most.z  = pMyVertices[i].position.z;
		if (pMyVertices[i].position.z < least.z)
			 least.z = pMyVertices[i].position.z;
	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::Rotate90(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	float tempX, tempZ;

	// apply that delta!
	for (int i = 0; i < numOfVertices; ++i)
	{
		tempX = pMyVertices[i].position.x;
		tempZ = pMyVertices[i].position.z;
		pMyVertices[i].position.x = -tempZ;
		pMyVertices[i].position.z = tempX;
	}


	vdMan->UnlockVerts(vertexList);

}


//***************************************************************************************
void PumaMesh::SetColor(int r, int g, int b, int a)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// apply that color!
	for (int i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].color   = D3DCOLOR_RGBA(r,g,b,a);
	}

	vdMan->UnlockVerts(vertexList);

}


//***************************************************************************************
void PumaMesh::RebuildNormals(LPDIRECT3DDEVICE8 pd3dDevice)
{

	PUMAMESHVERTEXSTRUCT *pMyVertices;
	D3DXVECTOR3 norm;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// for each vertex
	for (int i = 0; i < numOfVertices; ++i)
	{
		int j = i/3;
		int count;
		norm = FindNormalForPoint(pMyVertices, pMyVertices[i].position, count);
		
		float length = D3DXVec3Length(&norm);
		if (length < count * 0.5f)
			D3DXVec3Cross( &norm, 
				  &(pMyVertices[j*3+2].position-pMyVertices[j*3+1].position), 
				  &(pMyVertices[j*3+1].position-pMyVertices[j*3+0].position) );

	   D3DXVec3Normalize( &norm, &norm );

		pMyVertices[i].normal = norm;
	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
void PumaMesh::SetNormalsPure(LPDIRECT3DDEVICE8 pd3dDevice)
{

	PUMAMESHVERTEXSTRUCT *pVertices;
	D3DXVECTOR3 norm, edge1, edge2;

	pVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// for each vertex
	for (int i = 0; i < numOfVertices; i+=3)
	{
		edge1 = pVertices[i+2].position - pVertices[i+1].position;
		edge2 = pVertices[i+1].position - pVertices[i+0].position;
		D3DXVec3Cross( &norm, &edge1, &edge2 );
		D3DXVec3Normalize( &norm, &norm );
		pVertices[i+0].normal = norm;
		pVertices[i+1].normal = norm;
		pVertices[i+2].normal = norm;
	}

	vdMan->UnlockVerts(vertexList);

}

//***************************************************************************************
D3DXVECTOR3 PumaMesh::FindNormalForPoint(
					PUMAMESHVERTEXSTRUCT *pVertices, D3DXVECTOR3 point, int &count)
{

	D3DXVECTOR3 retVal, norm;
	count = 0;
	retVal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 

	// for each vertex
	for (int i = 0; i < numOfVertices/3; ++i)
	{
		if (pVertices[i*3+0].position == point ||
			 pVertices[i*3+1].position == point ||
			 pVertices[i*3+2].position == point)
		{
			D3DXVec3Cross( &norm, 
				  &(pVertices[i*3+2].position-pVertices[i*3+1].position), 
				  &(pVertices[i*3+1].position-pVertices[i*3+0].position) );

			retVal += norm;
			count++;
		}
	}

	return retVal;
}


//***************************************************************************************
float PumaMesh::SizeX(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	D3DXVECTOR3 least, most;
	least.x =  10000.0f;
	most.x  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (pMyVertices[i].position.x > most.x)
			 most.x  = pMyVertices[i].position.x;
		if (pMyVertices[i].position.x < least.x)
			 least.x = pMyVertices[i].position.x;
	}

	vdMan->UnlockVerts(vertexList);

	return most.x - least.x;

}

//***************************************************************************************
float PumaMesh::SizeY(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	D3DXVECTOR3 least, most;
	least.y =  10000.0f;
	most.y  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (pMyVertices[i].position.y > most.y)
			 most.y  = pMyVertices[i].position.y;
		if (pMyVertices[i].position.y < least.y)
			 least.y = pMyVertices[i].position.y;
	}

	vdMan->UnlockVerts(vertexList);

	return most.y - least.y;

}

//***************************************************************************************
float PumaMesh::SizeZ(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAMESHVERTEXSTRUCT *pMyVertices;

	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	D3DXVECTOR3 least, most;
	least.z =  10000.0f;
	most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (pMyVertices[i].position.z > most.z)
			 most.z  = pMyVertices[i].position.z;
		if (pMyVertices[i].position.z < least.z)
			 least.z = pMyVertices[i].position.z;
	}

	vdMan->UnlockVerts(vertexList);

	return most.z - least.z;

}


//***************************************************************************************
void PumaMesh::SaveCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals)
{
	if (resetNormals)
		RebuildNormals(pd3dDevice);

	PUMAANIMVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;
	unsigned char nextFlag;

	if (!fileName)
	{
      OPENFILENAME ofn;
	   memset(&ofn, 0, sizeof(OPENFILENAME));

	   ofn.lStructSize = sizeof(OPENFILENAME);
	   shipFileName[0] = 0;
	   ofn.lpstrInitialDir = ".\\";
	   ofn.lpstrFile= shipFileName;
	   ofn.nMaxFile = sizeof(shipFileName);
	   ofn.lpstrFilter = "PUMA MEC files\0*.mec\0\0";
      ofn.hwndOwner = (HWND) puma->m_hWnd;
	   ofn.Flags = OFN_NOCHANGEDIR;

  	   if (!GetSaveFileName(&ofn))
			return;

		fileName = shipFileName;

      int len = strlen(shipFileName);
      if (len <= 0)
         return;
		
      int tempLen = 0;
		
      while(tempLen < len && shipFileName[tempLen] != '.')
         tempLen++;
		
      char tempString[256];
		
      if (tempLen < len)
         shipFileName[tempLen] = 0;
		
      sprintf(tempString,"%s.mec",shipFileName);
      sprintf(shipFileName,"%s",tempString);


	}

	fp = fopen(fileName,"wb");
	if (!fp)
		return;

	// ******* here's how we do it

	// write number of verts
	fwrite(&numOfVertices, 1, sizeof(numOfVertices), fp);

	// write all the UVs as UCHARs (from the first frame)
	pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		unsigned char uc;
		float f = pMyVertices[i].tu * 255;
		while (f < 0)
			f += 256;
		if (f > 255)
			f = 255;
		uc = (unsigned char) f;
		fwrite(&(uc), 1, 1, fp);

		f = pMyVertices[i].tv * 255;
		while (f < 0)
			f += 256;
//		if (f < 0)
//			f = 0;
		if (f > 255)
			f = 255;
		uc = (unsigned char) f;
		fwrite(&(uc), 1, 1, fp);
	}

	vdMan->UnlockVerts(vertexList);

	// now, for each frame...
	
	pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// get the bounding box, and make the offsets and scale values
	D3DXVECTOR3 least, most, scale;

	least.x = least.y = least.z = 10000.0;
	most.x  = most.y  = most.z  = -10000.0;
	for (i = 0; i < numOfVertices; ++i)
	{
		if (least.x > pMyVertices[i].position.x)
			least.x  = pMyVertices[i].position.x;
		if (most.x  < pMyVertices[i].position.x)
			most.x   = pMyVertices[i].position.x;

		if (least.y > pMyVertices[i].position.y)
			least.y  = pMyVertices[i].position.y;
		if (most.y  < pMyVertices[i].position.y)
			most.y   = pMyVertices[i].position.y;

		if (least.z > pMyVertices[i].position.z)
			least.z  = pMyVertices[i].position.z;
		if (most.z  < pMyVertices[i].position.z)
			most.z   = pMyVertices[i].position.z;
	}

	scale.x = most.x - least.x;
	scale.y = most.y - least.y;
	scale.z = most.z - least.z;

	// save all that out
	fwrite(&least, 1, sizeof(least), fp);
	fwrite(&scale, 1, sizeof(scale), fp);

	// dont save color; we don't use it right now anyway

	// write out the normals as signed bytes
	for (i = 0; i < numOfVertices; ++i)
	{
		char cval;

		cval = (char)(pMyVertices[i].normal.x * 126);
		fwrite(&cval, 1, 1, fp);
		cval = (char)(pMyVertices[i].normal.y * 126);
		fwrite(&cval, 1, 1, fp);
		cval = (char)(pMyVertices[i].normal.z * 126);
		fwrite(&cval, 1, 1, fp);
	}

	// write out the positions as signed shorts
	for (i = 0; i < numOfVertices; ++i)
	{
		short sval;

		sval = (short)((pMyVertices[i].position.x - least.x) / scale.x * 0x0fff);
		fwrite(&sval, 1, 2, fp);
		sval = (short)((pMyVertices[i].position.y - least.y) / scale.y * 0x0fff);
		fwrite(&sval, 1, 2, fp);
		sval = (short)((pMyVertices[i].position.z - least.z) / scale.z * 0x0fff);
		fwrite(&sval, 1, 2, fp);
	}

	vdMan->UnlockVerts(vertexList);

	nextFlag = 0;
	fwrite(&nextFlag, 1, 1, fp);

	fclose(fp);

}

//***************************************************************************************
void PumaMesh::LoadCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName)
{
	aLog.Log("PumaMesh::LoadCompressed ");
	aLog.Log(fileName);

	PUMAANIMVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;
	int index = 0;
//	unsigned char nextFlag;

	if (!fileName)
	{
      OPENFILENAME ofn;
	   memset(&ofn, 0, sizeof(OPENFILENAME));

	   ofn.lStructSize = sizeof(OPENFILENAME);
	   shipFileName[0] = 0;
	   ofn.lpstrInitialDir = ".\\";
	   ofn.lpstrFile= shipFileName;
	   ofn.nMaxFile = sizeof(shipFileName);
	   ofn.lpstrFilter = "PUMA MEC files\0*.mec\0\0";
      ofn.hwndOwner = (HWND) puma->m_hWnd;
	   ofn.Flags = OFN_NOCHANGEDIR;

  	   if (!GetOpenFileName(&ofn))
			return;

		fileName = shipFileName;

      int len = strlen(shipFileName);
      if (len <= 0)
         return;
		
      int tempLen = 0;
		
      while(tempLen < len && shipFileName[tempLen] != '.')
         tempLen++;
		
      char tempString[256];
		
      if (tempLen < len)
         shipFileName[tempLen] = 0;
		
      sprintf(tempString,"%s.mec",shipFileName);
      sprintf(shipFileName,"%s",tempString);

	}

	fp = fopen(fileName,"rb");
	if (!fp)
		return;

	// delete any old data
	vdMan->DeleteObject(vertexList);
	vdMan->DeleteObject(texture);

	// ******* here's how we do it

	// read number of verts per frame
	int numVertices;
	fread(&numVertices, 1, sizeof(numVertices), fp);

	// read all the UVs as UCHARs (from the first frame)
	unsigned char *uvTable = new unsigned char[numVertices * 2];

	for (int i = 0; i < numVertices; ++i)
	{
		fread(&uvTable[i*2+0], 1, 1, fp);
		fread(&uvTable[i*2+1], 1, 1, fp);
	}

	// read the base and scale vectors
	D3DXVECTOR3 base, scale;

	fread(&base , 1, sizeof(base ), fp);
	fread(&scale, 1, sizeof(scale), fp);


	numOfVertices = numVertices;
	vertexList = new VideoDataObject(
									  numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
									  PUMAANIM_VERTEXDESC);
	vdMan->AddObject(vertexList);

	
	pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// read in the normals as signed bytes
	for (int i = 0; i < numOfVertices; ++i)
	{
		char cval;

		fread(&cval, 1, 1, fp);
		pMyVertices[i].normal.x  = (float)cval / 126.0f;
		fread(&cval, 1, 1, fp);
		pMyVertices[i].normal.y  = (float)cval / 126.0f;
		fread(&cval, 1, 1, fp);
		pMyVertices[i].normal.z  = (float)cval / 126.0f;

		D3DXVec3Normalize( &pMyVertices[i].normal, &pMyVertices[i].normal );
	}

	// read in the positions as signed shorts
	for (int i = 0; i < numOfVertices; ++i)
	{
		short sval;

		fread(&sval, 1, 2, fp);
		pMyVertices[i].position.x  = 
			               ((float)sval) / ((float)0x0fff) * scale.x + base.x;
		fread(&sval, 1, 2, fp);
		pMyVertices[i].position.y  = 
			               ((float)sval) / ((float)0x0fff) * scale.y + base.y;
		fread(&sval, 1, 2, fp);
		pMyVertices[i].position.z  = 
			               ((float)sval) / ((float)0x0fff) * scale.z + base.z;
	}

	// add the common UVs
	for (int i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].tu = ((float)uvTable[i*2+0]) / 255.0f;
		pMyVertices[i].tv = ((float)uvTable[i*2+1]) / 255.0f;
	}

	// set the colors
	for (int i = 0; i < numOfVertices; ++i)
	{
		pMyVertices[i].color = 0xffffffff;
	}


	vdMan->UnlockVerts(vertexList);
	

	fclose(fp);

	delete[] uvTable;

	aLog.Log(" DONE\n");

}


//***************************************************************************************
BOOL PumaMesh::IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* dist)
{
	FLOAT u,v,t;

    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );
    if( det < 0.0001f )
        return FALSE;

    // Calculate distance from vert0 to ray origin
    D3DXVECTOR3 tvec = orig - v0;
	 *dist = D3DXVec3Length(&tvec);

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &dir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

	 *dist = t;

    return TRUE;
}


//***************************************************************************************
void PumaMesh::FindCollisionsWithRawVertObject(
			LPDIRECT3DDEVICE8 pd3dDevice,      // need the device to lock my verts
			D3DXMATRIX &meshMatrix,            // orientation of myself
			RawVerts &rv,                      // the verts to collide against
			D3DXMATRIX &rawOldMatrix,          // orientation of raw verts last frame
			D3DXMATRIX &rawNewMatrix,          // orientation of raw verts NOW
			D3DXVECTOR3 *collisionPointList,   // array to place collision info
			D3DXVECTOR3 *collisionNormalList,  // array to place collision info
			int &listSize							  // size of collision array, modified to numOfCollisions
			)
{

	D3DXVECTOR3 faceVert[3], lineStart, lineDir;

	int listArraySize = listSize;
	listSize = 0;

	PUMAMESHVERTEXSTRUCT *pMyVertices;
	pMyVertices = (PUMAMESHVERTEXSTRUCT *) vdMan->LockVerts(vertexList);

	// for each face
	for (int i = 0; i < numOfVertices/3 && listSize < listArraySize; ++i)
	{
		// transform the three verts of this face
		D3DXMATRIX localMat[3];
//		matTrans._41 = 0;
//		matTrans._42 = 0;
//		matTrans._43 = 0;

		for (int k = 0; k < 3; ++k)
		{
			D3DXMatrixTranslation( &localMat[k],
				       pMyVertices[i*3+k].position.x,
						 pMyVertices[i*3+k].position.y,
						 pMyVertices[i*3+k].position.z);

			D3DXMatrixMultiply( &localMat[k], &localMat[k], &meshMatrix );

			faceVert[k].x = localMat[k]._41;
			faceVert[k].y = localMat[k]._42;
			faceVert[k].z = localMat[k]._43;
		}

		// for each RawVert
		for (int j = 0; j < rv.numOfVertices && listSize < listArraySize; ++j)
		{
			// transform this vert into the before and after verts
			D3DXMATRIX beforeMat, afterMat;

			D3DXMatrixTranslation( &beforeMat,
				       rv.vertexList[j].x,
						 rv.vertexList[j].y,
						 rv.vertexList[j].z);

			D3DXMatrixMultiply( &beforeMat, &beforeMat, &rawOldMatrix );

			lineStart.x = beforeMat._41;
			lineStart.y = beforeMat._42;
			lineStart.z = beforeMat._43;

			D3DXMatrixTranslation( &afterMat,
				       rv.vertexList[j].x,
						 rv.vertexList[j].y,
						 rv.vertexList[j].z);

			D3DXMatrixMultiply( &afterMat, &afterMat, &rawNewMatrix );

			lineDir.x = afterMat._41 - beforeMat._41;
			lineDir.y = afterMat._42 - beforeMat._42;
			lineDir.z = afterMat._43 - beforeMat._43;
			float lineLen = D3DXVec3Length(&lineDir);
			D3DXVec3Normalize(&lineDir, &lineDir);

			float dist;

			// find out if there's an intersect
			if (IntersectTriangle( lineStart, lineDir,
				                    faceVert[0], faceVert[2], faceVert[1],
										  &dist))
			{
				if (dist > 0 && dist < lineLen)
				{
					// set the next element in the pointList (lineStart)
					collisionPointList[listSize] = lineStart;

					// set the next element in the normalList (normal of the face)
					D3DXVECTOR3 edge1 = faceVert[2] - faceVert[1];
					D3DXVECTOR3 edge2 = faceVert[1] - faceVert[0];
					D3DXVECTOR3 norm;
					D3DXVec3Cross( &norm, &edge1, &edge2 );
					D3DXVec3Normalize( &norm, &norm );

					collisionNormalList[listSize] = norm;
				
					++listSize;
				}

				if (0 == j && collTestCount < 100)
				{
					collTestPos[collTestCount].x = lineStart.x + lineDir.x * dist;
					collTestPos[collTestCount].y = lineStart.y + lineDir.y * dist;
					collTestPos[collTestCount].z = lineStart.z + lineDir.z * dist;
					++collTestCount;
				}

			}
		}

	}

	vdMan->UnlockVerts(vertexList);

}


/* end of file */
