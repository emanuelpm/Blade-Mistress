#ifndef PARTICLE_ROT_H
#define PARTICLE_ROT_H

#include "particle2.h"

struct RotParticle
{
   D3DXVECTOR3 m_vPos;       // Current position
   D3DXVECTOR3 m_vVel;       // Current velocity
	float angle, speed, dist, distSpeed;
	float life; // < 0 == dead and unused
	float startingLife;
	D3DXCOLOR color;
};

// derive the specific particle type from this, and overload Tick()
class ParticleRotType
{
public:
	ParticleRotType(int maxParticles, char *textureName, float size);
	~ParticleRotType();

	void SetEmissionPoint(D3DXVECTOR3 point);
	void SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most);

	void Emit(int amount, D3DXCOLOR color);
	virtual void Tick(float timeDelta);

	void PrepareToDraw(void);
	void Draw(D3DXMATRIX matTrans);

    int numOfParticles;
	LPDIRECT3DVERTEXBUFFER8 pVerts;
	RotParticle *particleArray;
	LPDIRECT3DTEXTURE8 pTexture; // Our texture

	D3DXVECTOR3 boxLeast, boxMost;
	int emitFromPointFlag;
	int lastParticleUsed;

	float particleSize;
	float highAngle, lowAngle;
	float highAzimuth, lowAzimuth;
	float highSpeed, lowSpeed;
	float highLife, lowLife;
	float highRotSpeed, lowRotSpeed;
	float highDistSpeed, lowDistSpeed;
	float highDist, lowDist;

};



#endif
