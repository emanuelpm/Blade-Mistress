#ifndef PARTICLESCALE_H
#define PARTICLESCALE_H

#include "particle2.h"

struct ParticleScale
{
   D3DXVECTOR3 m_vPos;       // Current position
   D3DXVECTOR3 m_vVel;       // Current velocity
	float life; // < 0 == dead and unused
	float startingLife;
	float size;
	D3DXCOLOR color;
};

// derive the specific particle type from this, and overload Tick()
class ParticleScaleType
{
public:
	ParticleScaleType(int maxParticles, char *textureName, float scaleDelta);
	~ParticleScaleType();

	void SetEmissionPoint(D3DXVECTOR3 point);
	void SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most);

	void Emit(int amount, D3DXCOLOR color, float scale);
	virtual void Tick(float timeDelta);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

    int numOfParticles;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	ParticleScale *particleArray;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	D3DXVECTOR3 boxLeast, boxMost;
	int emitFromPointFlag;
	int lastParticleUsed;

	float scaleDelta;
	float highAngle, lowAngle;
	float highAzimuth, lowAzimuth;
	float highSpeed, lowSpeed;
	float highLife, lowLife;

};



#endif
