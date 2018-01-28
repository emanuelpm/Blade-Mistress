
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "./puma/puma.h"
#include "./puma/pumaSound.h"
#include "./helper/GeneralUtils.h"

#include "bbo.h"
#include "clientOnly.h"
#include "petSound.h"

#include "version.h"


char petSoundFileNames[6][32] =
{
	{"dat\\pet-chirp.wav"},
	{"dat\\pet-call.wav"},
	{"dat\\pet-grumbl.wav"},
	{"dat\\pet-burp.wav"},
	{"dat\\petFire.wav"},
	{"dat\\petImpact.wav"}
};

//******************************************************************
void PetSoundManager::Activate(void)  
{
	if (!PumaSound::systemReady)
		return;

	for (int i = 0; i < 6; ++i)
	{
		lastUsed[i] = 0;
		for (int j = 0; j < 4; ++j)
			sound[j][i] = new PumaSound(petSoundFileNames[i]);
	}

}

//******************************************************************
void PetSoundManager::Deactivate(void)
{
	if (!PumaSound::systemReady)
		return;

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 4; ++j)
			delete sound[j][i];
	}
}

//******************************************************************
void PetSoundManager::Play(D3DVECTOR pos, int which, float size)
{
	if (!PumaSound::systemReady)
		return;

   LPDIRECTSOUNDBUFFER buff;

	buff = *sound[lastUsed[which]][which]->sound->m_apDSBuffer;
	sound[lastUsed[which]][which]->PositionSound(pos, MAX_SOUND_DIST);
	buff->SetFrequency(12025 - 1000.0f * 300.0f * size);
//	buff->SetFrequency(2000);
	sound[lastUsed[which]][which]->Play();

	// roll to next sound
	++lastUsed[which];
	if (lastUsed[which] >= 4)
		lastUsed[which] = 0;
}







/* end of file */



