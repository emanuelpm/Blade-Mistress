#ifndef POINTGLOW_H
#define POINTGLOW_H

#include "particle2.h"

struct PointGlow
{
   D3DXVECTOR3 m_vPos;       // Current position
	D3DXCOLOR color;
	char used;
};

// derive the specific pointGlow type from this, and overload Tick()
class PointGlowSet
{
public:
	PointGlowSet(int maxPointGlows, char *textureName, float size);
	~PointGlowSet();

	void AddAGlow(D3DXVECTOR3 point, D3DXCOLOR color);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

   int numOfPointGlows;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	PointGlow *pointGlowArray;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	int lastPointGlowUsed;

	float pointGlowSize;
};



#endif
