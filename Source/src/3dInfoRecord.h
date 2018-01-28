
#ifndef INFO_RECORDTD_H
#define INFO_RECORDTD_H

#include ".\puma\puma.h"
#include ".\helper\linklist.h"

class InfoRecord3D : public DataObject
{
public:

	InfoRecord3D(float angle, float x, float y, float z, 
		          char *text, float healthBarValue, DWORD color, bool bold = FALSE);
	virtual ~InfoRecord3D();

	void DrawBar(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar);
	void Draw(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar);

	D3DXVECTOR3 position;
	float angle, scale;
	char *text;
	float healthBarValue;
	bool bBold;
	DWORD color;

};

#endif
