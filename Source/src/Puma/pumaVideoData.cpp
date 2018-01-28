//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "puma.h"
#include "..\helper\GeneralUtils.h"
#include "d3dutil.h"
#include "pumaVideoData.h"
#include "..\helper\autolog.h"

//***************************************************************************************
VideoDataObject::VideoDataObject(unsigned long rawSize, DWORD vertexFormat)	// mesh
{

	aLog.Log("VideoDataObject vertList creation\n");

	isReady = FALSE;
	vertexBufferPtr = NULL;
	backupVertexBufferPtr = NULL;
	pTexture = NULL;
	vertexBufferLen = rawSize;
	vertFormat = vertexFormat;

	puma->m_pd3dDevice->CreateVertexBuffer( rawSize, 0, vertexFormat,
                             D3DPOOL_MANAGED, &vertexBufferPtr );

	if (vertexBufferPtr)
	{
//		backupVertexBufferPtr = new char[rawSize];
//		if (backupVertexBufferPtr)
		isReady = TRUE;
		aLog.Log("success.\n");

	}

}

//***************************************************************************************
VideoDataObject::VideoDataObject(char *fileName, DWORD transparentColor)	// texture
{

	aLog.Log("VideoDataObject bitmap creation\n");

	isReady = FALSE;
	vertexBufferPtr = NULL;
	backupVertexBufferPtr = NULL;
	pTexture = NULL;
	tColor = transparentColor;

	HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, fileName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, transparentColor, NULL, NULL, &pTexture);

	if (D3D_OK == hr)
	{
		isReady = TRUE;
		sprintf(textureName, fileName);
		aLog.Log("success.\n");
	}
}

//***************************************************************************************
VideoDataObject::~VideoDataObject()
{
	if (backupVertexBufferPtr)
	{
		delete[] backupVertexBufferPtr;
		backupVertexBufferPtr = NULL;
	}

   SAFE_RELEASE(vertexBufferPtr);
   SAFE_RELEASE(pTexture);

}


//PumaMesh *MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice);
/*
int isReady;

long vertexBufferLen;
LPDIRECT3DVERTEXBUFFER8 vertexBufferPtr; // Buffer to hold vertices
LPDIRECT3DVERTEXBUFFER8 backupVertexBufferPtr; // Buffer to hold backed up vertices
LPDIRECT3DTEXTURE8 pTexture; // Our texture
char textureName[256];
*/



//***************************************************************************************
VideoDataManager::VideoDataManager(void)
{

}

//***************************************************************************************
VideoDataManager::~VideoDataManager()
{

}

//***************************************************************************************
void VideoDataManager::EraseAll  (void)
{

	VideoDataObject *vdo = (VideoDataObject *) list.First();
	while (vdo)
	{

	   SAFE_RELEASE(vdo->vertexBufferPtr);
		SAFE_RELEASE(vdo->pTexture);
		vdo->isReady = FALSE;

		vdo = (VideoDataObject *) list.Next();
	}

}

//***************************************************************************************
void VideoDataManager::RecoverAll(void)
{

	VideoDataObject *vdo = (VideoDataObject *) list.First();
	while (vdo)
	{

		if (vdo->backupVertexBufferPtr) // must be a mesh
		{
			puma->m_pd3dDevice->CreateVertexBuffer( vdo->vertexBufferLen, 0, vdo->vertFormat,
                             D3DPOOL_MANAGED, &vdo->vertexBufferPtr );

			if (vdo->vertexBufferPtr)
			{
//				void *realVerts = LockVerts(vdo);
//				memcpy(realVerts,vdo->backupVertexBufferPtr,vdo->vertexBufferLen);
//				UnlockVerts(vdo);
				vdo->isReady = TRUE;
			}
		}
		else
		{
			HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, vdo->textureName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, vdo->tColor, NULL, NULL, &vdo->pTexture);
			if (D3D_OK == hr)
				vdo->isReady = TRUE;
		}

		vdo = (VideoDataObject *) list.Next();
	}

}

//***************************************************************************************
void *VideoDataManager::LockVerts  (VideoDataObject *object)
{
	void *retVal;

	object->vertexBufferPtr->Lock( 0, 0, (BYTE**)&retVal, 0 );

	return retVal;
}

//***************************************************************************************
void  VideoDataManager::UnlockVerts(VideoDataObject *object)
{

   object->vertexBufferPtr->Unlock();

	void *realVerts = LockVerts(object);
//	memcpy(object->backupVertexBufferPtr, realVerts, object->vertexBufferLen);

   object->vertexBufferPtr->Unlock();

}

//***************************************************************************************
void VideoDataManager::AddObject   (VideoDataObject *object)
{

	list.Append(object);

}

//***************************************************************************************
void VideoDataManager::DeleteObject(VideoDataObject *object)
{

	if (object && list.Find(object))
	{
		list.Remove(object);
		delete object;
	}

}




/* end of file */
