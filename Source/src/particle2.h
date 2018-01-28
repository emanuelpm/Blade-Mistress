#ifndef PARTICLE2_H
#define PARTICLE2_H

struct GLOW_QUADPOINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
    FLOAT       tu, tv;   // The texture coordinates
};

#define D3DFVF_GLOW_QUADPOINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct Particle
{
    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity
	float life; // < 0 == dead and unused
	float startingLife;
	D3DXCOLOR color;
};

// derive the specific particle type from this, and overload Tick()
class Particle2Type
{
public:
	Particle2Type(int maxParticles, char *textureName, float size);
	~Particle2Type();

	void SetEmissionPoint(D3DXVECTOR3 point);
	void SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most);

	void Emit(int amount, D3DXCOLOR color);
	virtual void Tick(float timeDelta);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

    int numOfParticles;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	Particle *particleArray;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	D3DXVECTOR3 boxLeast, boxMost;
	int emitFromPointFlag;
	int lastParticleUsed;

	float particleSize;
	float highAngle, lowAngle;
	float highAzimuth, lowAzimuth;
	float highSpeed, lowSpeed;
	float highLife, lowLife;

};



#endif
