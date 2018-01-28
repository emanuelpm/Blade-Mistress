//***************************************************************************************
// Mesh object designed for animated PUMA and spaceship parts.
//***************************************************************************************

#include "pumaanim.h"
#include "pumamesh.h"

#include "..\helper\autolog.h"

//***************************************************************************************
PumaAnimFrame::PumaAnimFrame(int doid)	 : DataObject(doid,"ANIM_FRAME")
{
	vertexList = NULL;
}

//***************************************************************************************
PumaAnimFrame::~PumaAnimFrame()
{
   vdMan->DeleteObject(vertexList);
}

//***************************************************************************************
PumaAnim::PumaAnim(int doid, char *doname)	 : DataObject(doid,doname)
{

	texture = NULL;

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
PumaAnim::~PumaAnim()
{

//   vdMan->DeleteObject(vertexList);
   vdMan->DeleteObject(texture);

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		frames.Remove(paf);
		delete paf;
		paf = (PumaAnimFrame *) frames.First();
	}

}

//***************************************************************************************
void PumaAnim::LoadFromASC(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName)
{

	FILE *fp;

	int numFaces   = 0;
	int numVertPos = 0;
	float *vertPosArray = NULL;
	int numVertUVs = 0;
	float *vertUVArray  = NULL;
	PUMAANIMVERTEXSTRUCT* pVertices;
	PumaAnimFrame *paf = NULL;
	int curAnimFrame = 0;

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
			result = fscanf(fp,"%f",&(normalVals[1]));
			result = fscanf(fp,"%f",&(normalVals[2]));

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
		else if (!strncmp("*GEOMOBJECT",fileBuffer,strlen("*GEOMOBJECT")))
		{
			if (paf)
			{
				frames.Append(paf);
			   vdMan->UnlockVerts(paf->vertexList);
				delete[] vertPosArray;
				delete[] vertUVArray;
			}
			paf = new PumaAnimFrame(curAnimFrame);
			++curAnimFrame;
			numFaces   = 0;
			numVertPos = 0;
			vertPosArray = NULL;
			numVertUVs = 0;
			vertUVArray  = NULL;
			pVertices = NULL;
			faceRead = 0;
		}
		else if (!strncmp("*MESH_VERTEX",fileBuffer,strlen("*MESH_VERTEX")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 0]));
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 2]));
			result = fscanf(fp,"%f",&(vertPosArray[tempIndex * 3 + 1]));

			if (60 == tempIndex)
			{
				paf->bladePosition.x = vertPosArray[tempIndex * 3 + 0];
				paf->bladePosition.y = vertPosArray[tempIndex * 3 + 1];
				paf->bladePosition.z = vertPosArray[tempIndex * 3 + 2];
			}
			if (62 == tempIndex)
			{
				paf->bladeDirection.x = paf->bladePosition.x - vertPosArray[tempIndex * 3 + 0];
				paf->bladeDirection.y = paf->bladePosition.y - vertPosArray[tempIndex * 3 + 1];
				paf->bladeDirection.z = paf->bladePosition.z - vertPosArray[tempIndex * 3 + 2];

				D3DXVec3Normalize(&(paf->bladeDirection), &(paf->bladeDirection));
			}

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
			paf->numOfVertices = numFaces * 3;
			// Create the vertex buffer.
			vdMan->DeleteObject(paf->vertexList);

			paf->vertexList = new VideoDataObject(
				                     paf->numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
											PUMAANIM_VERTEXDESC);
			vdMan->AddObject(paf->vertexList);

		   // Prepare to fill the vertex buffer, by locking it.
			pVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

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

			pVertices[faceRead*3+0].color   = 0xffffffff;
			pVertices[faceRead*3+1].color   = 0xffffffff;
			pVertices[faceRead*3+2].color   = 0xffffffff;
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

	if (paf)
	{
		frames.Append(paf);
	   vdMan->UnlockVerts(paf->vertexList);
		delete[] vertPosArray;
		delete[] vertUVArray;
	}

	numOfFrames = curAnimFrame;
}

//***************************************************************************************
void PumaAnim::LoadTexture(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int alphaType)
{
	aLog.Log("PumaAnim::LoadTexture ");
	aLog.Log(fileName);


	vdMan->DeleteObject(texture);

	alpha = 0;
	if (1 == alphaType)
		alpha = 0xff000000;

	texture = new VideoDataObject(fileName,alpha);
	vdMan->AddObject(texture);

	aLog.Log(" DONE\n");
}

//***************************************************************************************
void PumaAnim::Draw(LPDIRECT3DDEVICE8 pd3dDevice, int frameIndex)
{

   // Setup our texture. Using textures introduces the texture stage states,
   // which govern how textures get blended together (in the case of multiple
   // textures) and lighting information. In this case, we are modulating
   // (blending) our texture with the diffuse color of the vertices.
   pd3dDevice->SetMaterial( &mtrl );
	if (texture && texture->isReady)
	{
	   pd3dDevice->SetTexture( 0, texture->pTexture );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	   pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
      pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
      pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
		if (!alpha)
		{
		   pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
		}
		else
		{
			SetTextureColorStage( pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
			SetTextureAlphaStage( pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
			pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
			pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
			pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
		}
	}
   // Render the vertex buffer contents

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.Find(frameIndex);
	if (paf && paf->vertexList && paf->vertexList->isReady)
	{
	   pd3dDevice->SetStreamSource( 0, paf->vertexList->vertexBufferPtr, sizeof(PUMAANIMVERTEXSTRUCT) );
		pd3dDevice->SetVertexShader( PUMAANIM_VERTEXDESC );

		for (int countBase = 0; countBase < paf->numOfVertices / 3;
			  countBase += puma->m_d3dCaps.MaxPrimitiveCount)
		{
			int numOut = paf->numOfVertices / 3 - countBase;
			if (puma->m_d3dCaps.MaxPrimitiveCount < paf->numOfVertices / 3 - countBase)
				numOut = puma->m_d3dCaps.MaxPrimitiveCount;
			puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
		}

//	   pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0, paf->numOfVertices/3 );
	}
//   pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
//   pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );

}


//***************************************************************************************
void PumaAnim::Scale(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		D3DXVECTOR3 least, most;
		least.x = least.y = least.z = 10000.0;
		most.x  = most.y  = most.z  = -10000.0;
		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			pMyVertices[i].position.x *= x;
			pMyVertices[i].position.y *= y;
			pMyVertices[i].position.z *= z;

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

		size.x = most.x - least.x;
		size.y = most.y - least.y;
		size.z = most.z - least.z;

		vdMan->UnlockVerts(paf->vertexList);

		paf->bladePosition.x *= x;
		paf->bladePosition.y *= y;
		paf->bladePosition.z *= z;

		paf = (PumaAnimFrame *) frames.Next();
	}

}

//***************************************************************************************
void PumaAnim::Shift(LPDIRECT3DDEVICE8 pd3dDevice, float x, float y, float z)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{

		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			pMyVertices[i].position.x += x;
			pMyVertices[i].position.y += y;
			pMyVertices[i].position.z += z;
		}

		vdMan->UnlockVerts(paf->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}
}

//***************************************************************************************
void PumaAnim::Mirror(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;
	PUMAANIMVERTEXSTRUCT tempVert;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{

		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; i += 3)
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

		vdMan->UnlockVerts(paf->vertexList);
		paf = (PumaAnimFrame *) frames.Next();
	}

}

//***************************************************************************************
PumaAnim *PumaAnim::MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PumaAnim *p = new PumaAnim(WhatAmI(),"PUMAANIM_COPY");

	PUMAANIMVERTEXSTRUCT *pNewVertices, *pMyVertices;
	*p = *this;
	p->texture = NULL;
	PumaAnimFrame *paf2;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		paf2 = new PumaAnimFrame(paf->WhatAmI());
		paf2->numOfVertices = paf->numOfVertices;

		paf2->vertexList = new VideoDataObject(
			                       paf->numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
                                PUMAANIM_VERTEXDESC);
		vdMan->AddObject(paf2->vertexList);

		// Prepare to fill the vertex buffer, by locking it.
		pNewVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf2->vertexList);

		// copy my verts first.
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			pNewVertices[i] = pMyVertices[i];
		}

		vdMan->UnlockVerts(paf->vertexList);
		vdMan->UnlockVerts(paf2->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}

	return p;

}

//***************************************************************************************
PumaMesh *PumaAnim::MakeFrameCopy(LPDIRECT3DDEVICE8 pd3dDevice, int frame)
{
	PUMAANIMVERTEXSTRUCT *pNewVertices, *pMyVertices;
	PumaMesh *p = NULL;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.Find(frame);
	if (paf)
	{
		p = new PumaMesh();
		p->numOfVertices = paf->numOfVertices;

		p->vertexList = new VideoDataObject(
			                       paf->numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
                                PUMAANIM_VERTEXDESC);
		vdMan->AddObject(p->vertexList);

		// Prepare to fill the vertex buffer, by locking it.
		pNewVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(p->vertexList);

		// copy my verts first.
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			pNewVertices[i] = pMyVertices[i];
		}

		vdMan->UnlockVerts(paf->vertexList);
		vdMan->UnlockVerts(p->vertexList);

		p->isReady = TRUE;

		p->texture = texture; // Our texture
		p->mtrl    = mtrl;

	}

	return p;

}

//***************************************************************************************
void PumaAnim::Save(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals)
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
	   ofn.lpstrFilter = "PUMA ANI files\0*.ani\0\0";
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
		
      sprintf(tempString,"%s.ani",shipFileName);
      sprintf(shipFileName,"%s",tempString);


	}

	fp = fopen(fileName,"wb");
	if (!fp)
		return;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		nextFlag = 1;
		fwrite(&nextFlag, 1, 1, fp);

		fwrite(&paf->bladeDirection, 1, sizeof(paf->bladeDirection), fp);
		fwrite(&paf->bladePosition , 1, sizeof(paf->bladePosition ), fp);

		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		fwrite(&paf->numOfVertices, 1, sizeof(paf->numOfVertices), fp);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			fwrite(&(pMyVertices[i]), 1, sizeof(PUMAANIMVERTEXSTRUCT), fp);
		}

		vdMan->UnlockVerts(paf->vertexList);
		paf = (PumaAnimFrame *) frames.Next();
	}
	nextFlag = 0;
	fwrite(&nextFlag, 1, 1, fp);

	fclose(fp);

}


//***************************************************************************************
void PumaAnim::Load(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName)
{
	aLog.Log("PumaAnim::Load ");
	aLog.Log(fileName);

	PUMAANIMVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;
	PumaAnimFrame *paf;
	int index = 0;
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
	   ofn.lpstrFilter = "PUMA ANI files\0*.ani\0\0";
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
		
      sprintf(tempString,"%s.ani",shipFileName);
      sprintf(shipFileName,"%s",tempString);

	}

	fp = fopen(fileName,"rb");
	if (!fp)
		return;

	paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		frames.Remove(paf);
		delete paf;
		paf = (PumaAnimFrame *) frames.First();
	}

	vdMan->DeleteObject(texture);

	fread(&nextFlag, 1, 1, fp);
	while (nextFlag)
	{
		paf = new PumaAnimFrame(index);
		if (!paf)
		{
			paf = (PumaAnimFrame *) frames.First();
			while (paf)
			{
				frames.Remove(paf);
				delete paf;
				paf = (PumaAnimFrame *) frames.First();
			}
			index = 0;
			nextFlag = FALSE;
		}
		else
		{
			++index;
			frames.Append(paf);

			fread(&paf->bladeDirection, 1, sizeof(paf->bladeDirection), fp);
			fread(&paf->bladePosition , 1, sizeof(paf->bladePosition ), fp);

			fread(&paf->numOfVertices, 1, sizeof(paf->numOfVertices), fp);

			paf->vertexList = new VideoDataObject(
											  paf->numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
											  PUMAANIM_VERTEXDESC);
			vdMan->AddObject(paf->vertexList);
			if (!(paf->vertexList))
			{
				frames.Remove(paf);
				delete paf;
				--index;
				nextFlag = FALSE;
			}
			else if (!(paf->vertexList->isReady))
			{
				frames.Remove(paf);
				delete paf;
				--index;
				nextFlag = FALSE;
			}
			else
			{
				pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

				int i = 0;
				for (i = 0; i < paf->numOfVertices; ++i)
				{
					fread(&(pMyVertices[i]), 1, sizeof(PUMAANIMVERTEXSTRUCT), fp);
				}

				vdMan->UnlockVerts(paf->vertexList);

				fread(&nextFlag, 1, 1, fp);
			}
		}
	}

	fclose(fp);

	numOfFrames = index;

	aLog.Log(" DONE\n");
}

//***************************************************************************************
void PumaAnim::SaveCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, int resetNormals)
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
	   ofn.lpstrFilter = "PUMA ANC files\0*.anc\0\0";
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
		
      sprintf(tempString,"%s.anc",shipFileName);
      sprintf(shipFileName,"%s",tempString);


	}

	fp = fopen(fileName,"wb");
	if (!fp)
		return;

	// ******* here's how we do it

	// write number of frames
	short numFrames = 0;
	int numVerts = 0;
	PumaAnimFrame *longestPaf = NULL;
	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		++numFrames;

		if (!longestPaf || paf->numOfVertices > numVerts)
			longestPaf = paf;

		if (paf->numOfVertices > numVerts)
			numVerts = paf->numOfVertices;

		paf = (PumaAnimFrame *) frames.Next();
	}

	fwrite(&numFrames, 1, 2, fp);

	// write number of verts per frame
//	paf = (PumaAnimFrame *) frames.First();
	fwrite(&numVerts, 1, sizeof(numVerts), fp);

	// write all the UVs as UCHARs (from the first frame)
	pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(longestPaf->vertexList);

	int i = 0;
	for (i = 0; i < numVerts; ++i)
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

	vdMan->UnlockVerts(longestPaf->vertexList);

	// now, for each frame...
	paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		//write out the blade info
		fwrite(&paf->bladeDirection, 1, sizeof(paf->bladeDirection), fp);
		fwrite(&paf->bladePosition , 1, sizeof(paf->bladePosition ), fp);

		// get the bounding box, and make the offsets and scale values
		D3DXVECTOR3 least, most, scale;

		least.x = least.y = least.z = 10000.0;
		most.x  = most.y  = most.z  = -10000.0;
		for (int i = 0; i < paf->numOfVertices; ++i)
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
		for (i = 0; i < numVerts; ++i)
		{
			char cval;

			if (i < paf->numOfVertices)
			{
				cval = (char)(pMyVertices[i].normal.x * 126);
				fwrite(&cval, 1, 1, fp);
				cval = (char)(pMyVertices[i].normal.y * 126);
				fwrite(&cval, 1, 1, fp);
				cval = (char)(pMyVertices[i].normal.z * 126);
				fwrite(&cval, 1, 1, fp);
			}
			else
			{
				cval = (char)0;
				fwrite(&cval, 1, 1, fp);
				fwrite(&cval, 1, 1, fp);
				fwrite(&cval, 1, 1, fp);
			}
		}

		// write out the positions as signed shorts
		for (i = 0; i < numVerts; ++i)
		{
			short sval;

			if (i < paf->numOfVertices)
			{
				sval = (short)((pMyVertices[i].position.x - least.x) / scale.x * 0x0fff);
				fwrite(&sval, 1, 2, fp);
				sval = (short)((pMyVertices[i].position.y - least.y) / scale.y * 0x0fff);
				fwrite(&sval, 1, 2, fp);
				sval = (short)((pMyVertices[i].position.z - least.z) / scale.z * 0x0fff);
				fwrite(&sval, 1, 2, fp);
			}
			else
			{
				sval = (short)0;
				fwrite(&sval, 1, 2, fp);
				fwrite(&sval, 1, 2, fp);
				fwrite(&sval, 1, 2, fp);
			}
		}

		vdMan->UnlockVerts(paf->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}
	nextFlag = 0;
	fwrite(&nextFlag, 1, 1, fp);

	fclose(fp);

}

//***************************************************************************************
void PumaAnim::LoadCompressed(LPDIRECT3DDEVICE8 pd3dDevice, char *fileName, DWORD vertColor)
{
	aLog.Log("PumaAnim::LoadCompressed ");
	aLog.Log(fileName);

	PUMAANIMVERTEXSTRUCT *pMyVertices;
   char  shipFileName[128];
	FILE *fp;
	PumaAnimFrame *paf;
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
	   ofn.lpstrFilter = "PUMA ANC files\0*.anc\0\0";
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
		
      sprintf(tempString,"%s.anc",shipFileName);
      sprintf(shipFileName,"%s",tempString);

	}

	fp = fopen(fileName,"rb");
	if (!fp)
		return;

	// delete any old data
	paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		frames.Remove(paf);
		delete paf;
		paf = (PumaAnimFrame *) frames.First();
	}

	vdMan->DeleteObject(texture);

	// ******* here's how we do it

	// read number of frames

	short numFrames = 0;
	fread(&numFrames, 1, 2, fp);
	numOfFrames = numFrames;

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

	// now, for each frame...
	for (int frameNum = 0; frameNum < numFrames; ++frameNum)
	{
		paf = new PumaAnimFrame(frameNum);
		if (!paf)
		{
			paf = (PumaAnimFrame *) frames.First();
			while (paf)
			{
				frames.Remove(paf);
				delete paf;
				paf = (PumaAnimFrame *) frames.First();
			}
			numOfFrames = frameNum;
			frameNum = numFrames;
		}
		else
		{
			frames.Append(paf);

			//read in the blade info
			fread(&paf->bladeDirection, 1, sizeof(paf->bladeDirection), fp);
			fread(&paf->bladePosition , 1, sizeof(paf->bladePosition ), fp);

			// read the base and scale vectors
			D3DXVECTOR3 base, scale;

			fread(&base , 1, sizeof(base ), fp);
			fread(&scale, 1, sizeof(scale), fp);


			paf->numOfVertices = numVertices;
			paf->vertexList = new VideoDataObject(
											  paf->numOfVertices*sizeof(PUMAANIMVERTEXSTRUCT),
											  PUMAANIM_VERTEXDESC);
			vdMan->AddObject(paf->vertexList);

			if (!(paf->vertexList))
			{
				frames.Remove(paf);
				delete paf;
				numOfFrames = frameNum;
				frameNum = numFrames;
			}
			else if (!(paf->vertexList->isReady))
			{
				frames.Remove(paf);
				delete paf;
				numOfFrames = frameNum;
				numOfFrames = numFrames;
				frameNum = numFrames;
			}
			else
			{
				pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

				// read in the normals as signed bytes
				for (int i = 0; i < paf->numOfVertices; ++i)
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
				for (int i = 0; i < paf->numOfVertices; ++i)
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
				for (int i = 0; i < paf->numOfVertices; ++i)
				{
					pMyVertices[i].tu = ((float)uvTable[i*2+0]) / 255.0f;
					pMyVertices[i].tv = ((float)uvTable[i*2+1]) / 255.0f;
				}

				// set the colors
				for (int i = 0; i < paf->numOfVertices; ++i)
				{
					pMyVertices[i].color = vertColor;
				}


				vdMan->UnlockVerts(paf->vertexList);
			}
		}

	}

	fclose(fp);

	delete[] uvTable;

	aLog.Log(" DONE\n");
}

//***************************************************************************************
void PumaAnim::Center(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();

	pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

	D3DXVECTOR3 least, most;
	least.x = least.y = least.z =  10000.0f;
	most.x  = most.y  = most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < paf->numOfVertices; ++i)
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

   vdMan->UnlockVerts(paf->vertexList);

	// apply that delta!
	paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		for (int i = 0; i < paf->numOfVertices; ++i)
		{
			pMyVertices[i].position.x -= center.x;
			pMyVertices[i].position.y -= center.y;
			pMyVertices[i].position.z -= center.z;
		}

		vdMan->UnlockVerts(paf->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}

}

//***************************************************************************************
void PumaAnim::RebuildNormals(LPDIRECT3DDEVICE8 pd3dDevice)
{

	PUMAANIMVERTEXSTRUCT *pMyVertices;
	D3DXVECTOR3 norm;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		// for each vertex
		for (int i = 0; i < paf->numOfVertices; ++i)
		{
			int j = i/3;
			int count;
			norm = FindNormalForPoint(pMyVertices, paf->numOfVertices, 
				                       pMyVertices[i].position, count);
			
//			float length = D3DXVec3Length(&norm);
//			if (length < count * 0.5f)
//				D3DXVec3Cross( &norm, 
//					  &(pMyVertices[j*3+2].position-pMyVertices[j*3+1].position), 
//					  &(pMyVertices[j*3+1].position-pMyVertices[j*3+0].position) );

			D3DXVec3Normalize( &norm, &norm );

			pMyVertices[i].normal = norm;
		}

		vdMan->UnlockVerts(paf->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}
}

//***************************************************************************************
D3DXVECTOR3 PumaAnim::FindNormalForPoint(
					PUMAANIMVERTEXSTRUCT *pVertices, int vertNum, D3DXVECTOR3 point, int &count)
{

	D3DXVECTOR3 retVal, norm;
	count = 0;
	retVal = D3DXVECTOR3(0.0f, 0.0f, 0.0f); 

	// for each vertex
	for (int i = 0; i < vertNum/3; ++i)
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
void PumaAnim::SetExtraPosition(LPDIRECT3DDEVICE8 pd3dDevice, int index, int vertIndex)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{

		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		paf->extraPosition[index].x = pMyVertices[vertIndex].position.x; 
		paf->extraPosition[index].y = pMyVertices[vertIndex].position.y; 
		paf->extraPosition[index].z = pMyVertices[vertIndex].position.z; 

		vdMan->UnlockVerts(paf->vertexList);

		paf = (PumaAnimFrame *) frames.Next();
	}
}


//***************************************************************************************
void PumaAnim::AdjustExtraPosition(LPDIRECT3DDEVICE8 pd3dDevice, int index, 
											  float dx, float dy, float dz)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		paf->extraPosition[index].x += dx; 
		paf->extraPosition[index].y += dy; 
		paf->extraPosition[index].z += dz; 

		paf = (PumaAnimFrame *) frames.Next();
	}
}


//***************************************************************************************
int PumaAnim::FindTopVert(LPDIRECT3DDEVICE8 pd3dDevice)
{
	PUMAANIMVERTEXSTRUCT *pMyVertices;

	int topIndex = -1;
	float topVal = -10000;

	PumaAnimFrame *paf = (PumaAnimFrame *) frames.First();
	while (paf)
	{
		pMyVertices = (PUMAANIMVERTEXSTRUCT *) vdMan->LockVerts(paf->vertexList);

		int i = 0;
		for (i = 0; i < paf->numOfVertices; ++i)
		{
			if (topVal  < pMyVertices[i].position.y)
			{
				topVal   = pMyVertices[i].position.y;
				topIndex = i;
			}
		}

		vdMan->UnlockVerts(paf->vertexList);

		paf = NULL;
	}

	return topIndex;
}



/* end of file */
