#ifndef GLOWQUADTYPE_H
#define GLOWQUADTYPE_H

struct COLORQUADPOINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
};

struct ColorQuad
{
	D3DXVECTOR3 vec[4];
	int r,g,b;
};

#define D3DFVF_COLORQUADPOINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

// derive the specific particle type from this, and overload Tick()
class GlowQuadType
{
public:
	GlowQuadType(int maxQuads);
	~GlowQuadType();

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

	void StartAddingQuads(void);
	void AddQuad(ColorQuad &quad);
	void Tick(void);
	void FinishAddingQuads(void);

   int numOfQuads, nextQuad;
	LPDIRECT3DVERTEXBUFFER8 pVerts;

	COLORQUADPOINTVERTEX *pMyVertices;

};



#endif
