#ifndef BBOPET_H
#define BBOPET_H

//#include "BBOmob.h"

struct MessPet;
class PumaAnim;

class BBOPet
{
public:

	void Init(MessPet *petInfo);
	void Clear(void);
	void Draw(SpacePoint *spacePoint, int petIndex);
	void SetState(int which) {nextState = which;};
	void SetAttackType(int which) {petAttackType = which;};
	void DrawPetAttackEffect(int which, int job);
	void PositionPetAnim(int petIndex);

	void SetMistressInfo(D3DXVECTOR3 p, float a) { pos[0] = p; angle[0] = a; };

//	SpacePoint spacePoint;

	PumaAnim *petDragonAnims[2];
	char petName[16];
	D3DXVECTOR3 petAttackStart;
	D3DXVECTOR3 petAttackEnd;
	int petAttackType;
	int petEmotion, petLifeStage;

	int curState, nextState;
	int stateCounter;

	D3DXVECTOR3 pos[2];
	D3DXVECTOR3 offset;
	float angle[2], petAngle;
	float petAzimuth, petRoll;

	float petScaleVal;

	D3DXMATRIX matDragon;
	D3DXVECTOR3 posDragon;

};

#endif
