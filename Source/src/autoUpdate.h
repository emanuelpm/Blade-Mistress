#ifndef AVATAR_TEXTURE_H
#define AVATAR_TEXTURE_H

#include "puma.h"
#include "linklist.h"

//***************************************************************************************
class AutoUpdater : public DataObject
{
public:

	AvatarTexture(void);
	AvatarTexture(int doid, char *doname);
	virtual ~AvatarTexture();

	void Init(void);

	void Generate(LPDIRECT3DDEVICE8 pd3dDevice, int skinIndex, int hairR, int hairG, int hairB);
	
	static LPDIRECT3DTEXTURE8 skins[NUM_OF_SKINS];
	static LPDIRECT3DTEXTURE8 hair;
	static int bitmapRefCount;

	LPDIRECT3DTEXTURE8 currentBitmap;
};

#endif
