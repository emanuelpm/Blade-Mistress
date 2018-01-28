#ifndef PARTICLE3_H
#define PARTICLE3_H

#include "particle2.h"

// derive the specific particle type from this, and overload Tick()
class Particle3Type
{
public:
	Particle3Type(int maxParticles, char *textureName, float size);
	~Particle3Type();

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
