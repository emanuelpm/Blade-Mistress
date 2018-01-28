#ifndef SIMPLEGRASS_H
#define SIMPLEGRASS_H

class GroundObjectTiles;

struct SIMPLEGRASSPOINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    FLOAT       tu, tv;   // The texture coordinates
};

#define D3DFVF_SIMPLEGRASSPOINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)


class SimpleGrass
{
public:
	SimpleGrass(int maxTufts, char *textureName, float size);
	~SimpleGrass();

	void AddTuft(GroundObjectTiles *ground, D3DXVECTOR3 lastPos, D3DXVECTOR3 curPos);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

   int numOfTufts, curTuft;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	float size, distTravelled;
	D3DXVECTOR3 lastPos;

};



#endif
