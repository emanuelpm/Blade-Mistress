#ifndef PARTICLESMOKE_H
#define PARTICLESMOKE_H

#include "particle2.h"

struct ParticleSmoke
{
   D3DXVECTOR3 m_vPos;       // Current position
   D3DXVECTOR3 m_vVel;       // Current velocity
	float life; // < 0 == dead and unused
	float startingLife;
	float size;
	D3DXCOLOR color;
};

// derive the specific particle type from this, and overload Tick()
class ParticleSmokeType
{
public:
	ParticleSmokeType(int maxParticles, char *textureName, float scaleDelta, float yD);
	~ParticleSmokeType();

	void SetEmissionPoint(D3DXVECTOR3 point);
	void SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most);

	void Emit(int amount, D3DXCOLOR color, float scale);
	virtual void Tick(float timeDelta);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

    int numOfParticles;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	ParticleSmoke *particleArray;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	D3DXVECTOR3 boxLeast, boxMost;
	int emitFromPointFlag;
	int lastParticleUsed;

	float scaleDelta;
	float highAngle, lowAngle;
	float highAzimuth, lowAzimuth;
	float highSpeed, lowSpeed;
	float highLife, lowLife;

	float yDelta;
};



#endif
