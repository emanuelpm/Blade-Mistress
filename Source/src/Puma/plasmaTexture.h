#ifndef PLASMA_TEXTURE_H
#define PLASMA_TEXTURE_H

#include "puma.h"
#include "../helper/linklist.h"

//***************************************************************************************
class PlasmaTexture : public DataObject
{
public:

	PlasmaTexture(void);
	PlasmaTexture(int doid, char *doname);
	virtual ~PlasmaTexture();

	void Generate(LPDIRECT3DDEVICE8 pd3dDevice, int w, int h, unsigned long randSeed = 0);
	void Colorize(void);
	void MakeTranslucent(void);
	
	LPDIRECT3DTEXTURE8 pTexture; // Our texture
	int sizeW, sizeH;
};

#endif
