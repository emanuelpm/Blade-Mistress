#ifndef AVATAR_TEXTURE_H
#define AVATAR_TEXTURE_H

#include ".\puma\puma.h"
#include "BBO.h"
#include ".\helper\linklist.h"

//***************************************************************************************
class AvatarTexture : public DataObject
{
public:

	AvatarTexture(void);
	AvatarTexture(int doid, char *doname);
	virtual ~AvatarTexture();

	void Init(void);

	void Generate(LPDIRECT3DDEVICE8 pd3dDevice, 
		              int faceIndex, int hairR, int hairG, int hairB,
		              int topIndex, int topR, int topG, int topB,
		              int bottomIndex, int bottomR, int bottomG, int bottomB,
						  unsigned short imageFlags);
	
	D3DCOLOR GetPixel(            _D3DFORMAT fmt, char *line, int x);
	void SetPixel(D3DCOLOR color, _D3DFORMAT fmt, char *line, int x);


	static LPDIRECT3DTEXTURE8 skin;
	static LPDIRECT3DTEXTURE8 hair;
	static LPDIRECT3DTEXTURE8 faces  [NUM_OF_FACES];
	static LPDIRECT3DTEXTURE8 tops   [NUM_OF_TOPS];
	static LPDIRECT3DTEXTURE8 bottoms[NUM_OF_BOTTOMS];
	static LPDIRECT3DTEXTURE8 helperMark, dokkMark, anubisMark, overlordMark;
	static int bitmapRefCount;

	LPDIRECT3DTEXTURE8 currentBitmap;
};

extern int useSimpleAvatarTextureFlag;


#endif
