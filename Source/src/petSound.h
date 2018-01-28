#ifndef PETSOUND_H
#define PETSOUND_H

//#include "gamemode.h"
//#include "spaceTest.h"
class PumaSound;

enum
{
	PET_SOUND_CHIRP,
	PET_SOUND_CALL,
	PET_SOUND_GRUMBLE,
	PET_SOUND_BURP,
	PET_SOUND_FIRE,
	PET_SOUND_IMPACT,
	PET_SOUND_MAX
};

class PetSoundManager
{
public:

	void Activate(void);  
	void Deactivate(void);
	void Play(D3DVECTOR pos, int which, float size);

	PumaSound *sound[4][6];
	int lastUsed[6];

};

#endif
