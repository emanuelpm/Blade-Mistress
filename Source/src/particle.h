#ifndef PARTICLE_H
#define PARTICLE_H


struct PARTICLEPOINTVERTEX
{
    D3DXVECTOR3 v;
    D3DCOLOR    color;
};

#define D3DFVF_PARTICLEPOINTVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)

struct Particle
{
    D3DXVECTOR3 m_vPos;       // Current position
    D3DXVECTOR3 m_vVel;       // Current velocity
	float life; // < 0 == dead and unused
	float startingLife;
	D3DXCOLOR color;
};

// derive the specific particle type from this, and overload Tick()
class ParticleType
{
public:
	ParticleType(int maxParticles, char *textureName, float size);
	~ParticleType();

	void SetEmissionPoint(D3DXVECTOR3 point);
	void SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most);

	void Emit(int amount, D3DXCOLOR color);
	virtual void Tick(float timeDelta);

	void PrepareToDraw(void);
	void Draw(void);

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
