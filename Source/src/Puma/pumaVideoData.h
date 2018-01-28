#ifndef PUMA_VIDEO_DATA_H
#define PUMA_VIDEO_DATA_H

#include "puma.h"
#include "../helper/dataobje.h"
#include "../helper/linklist.h"

//***************************************************************************************
class VideoDataObject : public DataObject
{
public:

	VideoDataObject(unsigned long rawSize, DWORD vertexFormat);        // mesh
	VideoDataObject(char *fileName,        DWORD transparentColor);    // texture
	virtual ~VideoDataObject();

//	PumaMesh *MakeCopy(LPDIRECT3DDEVICE8 pd3dDevice);

	int isReady;

	unsigned long vertexBufferLen;
	DWORD vertFormat;
	LPDIRECT3DVERTEXBUFFER8 vertexBufferPtr; // Buffer to hold vertices
	char *            backupVertexBufferPtr; // Buffer to hold backed up vertices
	LPDIRECT3DTEXTURE8 pTexture; // Our texture
	char textureName[256];
	DWORD tColor;

};


//***************************************************************************************
class VideoDataManager
{
public:

	VideoDataManager(void);
	virtual ~VideoDataManager();

	void EraseAll  (void);
	void RecoverAll(void);

	void *LockVerts  (VideoDataObject *object);
	void  UnlockVerts(VideoDataObject *object);

	void AddObject   (VideoDataObject *object);
	void DeleteObject(VideoDataObject *object);

	DoublyLinkedList list;

};


#endif
