
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include ".\puma\puma.h"
#include ".\puma\pumaSound.h"
#include ".\helper\GeneralUtils.h"

#include "bbo.h"
#include "clientOnly.h"
#include "backSound.h"

#include "version.h"


char backSoundFileNames[8][32] =
{
	{"dat\\Env-dungeon.wav"},
	{"dat\\Env-forest.wav"},
	{"dat\\Env-swamp.wav"},
	{"dat\\Env-snow.wav"},
	{"dat\\Env-desert.wav"},
	{"dat\\Env-beach.wav"},
	{"dat\\Env-laby.wav"},
	{"dat\\Env-tower.wav"}
};

//******************************************************************
void BackSoundManager::Activate(void)  
{
	if (!PumaSound::systemReady)
		return;

	for (int i = 0; i < 8; ++i)
		sound[i] = new PumaSound(backSoundFileNames[i]);

	PumaSound *userMusic = new PumaSound("dat\\guild.wav");
	if (userMusic->sound)
	{
		delete sound[7];
		sound[7] = userMusic;
	}
	else
		delete userMusic;

	lastType = -1;
	oldSoundIndex = newSoundIndex = -1;

}

//******************************************************************
void BackSoundManager::Deactivate(void)
{
	if (!PumaSound::systemReady)
		return;

	for (int i = 0; i < 8; ++i)
		delete sound[i];
}

//******************************************************************
void BackSoundManager::Tick(D3DXVECTOR3 camPoint)
{

	if (!PumaSound::systemReady)
		return;

	if (!PumaSound::playSounds)
	{
		if (-1 != oldSoundIndex)
		{
			if (sound[oldSoundIndex]->sound)
			{
				sound[oldSoundIndex]->Stop();
				oldSoundIndex = -1;
			}
		}
		if (-1 != newSoundIndex)
		{
			if (sound[newSoundIndex]->sound)
			{
				sound[newSoundIndex]->Stop();
				newSoundIndex = -1;
			}
		}
		return;
	}

	if (-1 != oldSoundIndex)
	{
		++oldSoundShift;	//	0 - 100

		sound[oldSoundIndex]->PositionSound(camPoint, MAX_SOUND_DIST);

	   LPDIRECTSOUNDBUFFER buff;
		if (sound[oldSoundIndex]->sound)
		{
			buff = *sound[oldSoundIndex]->sound->m_apDSBuffer;
			buff->SetVolume(DSBVOLUME_MIN + (DSBVOLUME_MAX-DSBVOLUME_MIN) / 100 * (100 - oldSoundShift));
			if (oldSoundShift >= 100)
			{
				sound[oldSoundIndex]->Stop();
				oldSoundIndex = -1;
			}
		}

	}

	if (-1 != newSoundIndex)
		sound[newSoundIndex]->PositionSound(camPoint, MAX_SOUND_DIST);

	if (-1 != newSoundIndex && newSoundShift < 100)
	{
		++newSoundShift;	//	0 - 100

	   LPDIRECTSOUNDBUFFER buff;
		if (sound[newSoundIndex]->sound)
		{
			buff = *sound[newSoundIndex]->sound->m_apDSBuffer;
			buff->SetVolume(DSBVOLUME_MIN + (DSBVOLUME_MAX-DSBVOLUME_MIN) / 100 * (newSoundShift));
		}
	}


}

//******************************************************************
void BackSoundManager::SetType(int type)
{
	if (!PumaSound::systemReady)
		return;

	if (type == lastType)
		return;

	lastType = type;

//	for (int i = 0; i < 6; ++i)
//		sound[i]->Stop();

   LPDIRECTSOUNDBUFFER buff;

	oldSoundIndex = newSoundIndex;

	switch (type)
	{
	case 1:
	case 2:
		newSoundIndex = 1;
		break;

	case 3:
		newSoundIndex = 2;
		break;

	case 4:
	case 7:
		newSoundIndex = 4;
		break;

	case 5:
		newSoundIndex = 3;
		break;

	case -2:
		newSoundIndex = 0;
		break;

	case -3:
		newSoundIndex = 6;
		break;

	case -4:
		newSoundIndex = 7;
		break;

	case -1:
		newSoundIndex = -1;
		return;
//		sound[0]->PlayLooping();  // stop sounds!
		break;

	default:
		newSoundIndex = 5;
		break;

	}

	if (sound[newSoundIndex]->sound)
	{
		buff = *sound[newSoundIndex]->sound->m_apDSBuffer;
		buff->SetVolume(DSBVOLUME_MIN);
		sound[newSoundIndex]->PlayLooping();
		oldSoundShift = 100 - newSoundShift;
		newSoundShift = 0;
	}

}







/* end of file */



