#ifndef BACKSOUND_H
#define BACKSOUND_H

//#include "gamemode.h"
//#include "spaceTest.h"
class PumaSound;

class BackSoundManager
{
public:

	void Activate(void);  
	void Deactivate(void);
	void Tick(D3DXVECTOR3 camPoint);
	void SetType(int type);

	PumaSound *sound[8];
	int lastType;
	int oldSoundIndex, newSoundIndex, oldSoundShift, newSoundShift;

};

#endif
