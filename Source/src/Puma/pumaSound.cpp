//***************************************************************************************
// Main file designed to be the logical main for using PUMA.
//***************************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "puma.h"
#include "pumaSound.h"
#include "..\helper\autolog.h"


//extern void aLog.Log(char *text);

CSoundManager*          PumaSound::soundManager       = NULL;
LPDIRECTSOUND3DLISTENER PumaSound::listener         = NULL;   // 3D listener object
DS3DLISTENER            PumaSound::listenerParams;             // Listener properties

D3DVECTOR PumaSound::lastPos;
float PumaSound::lastMaxDist = 100;
int PumaSound::systemReady = FALSE;
int PumaSound::playSounds = TRUE;
int PumaSound::playUISounds = TRUE;

//******************************************************************
PumaSound::PumaSound(char *fileName)
	: sound(NULL)
{
#ifndef NOSOUND
	if (!systemReady)
		return;
	// load and start a test looping sound

	aLog.Log("loading sound: ");
	aLog.Log(fileName);
	aLog.Log("\n");

	soundManager->Create( &sound, fileName, 
		                     DSBCAPS_LOCSOFTWARE | DSBCAPS_CTRL3D | 
									DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY, 
									DS3DALG_NO_VIRTUALIZATION );  

	aLog.Log("sound ptr: ");
	aLog.Log((int)sound);
	aLog.Log("\n");

	p3DBuffer = NULL;
	if (sound)
	{
		//Get the 3D buffer from the secondary buffer
		sound->Get3DBufferInterface( 0, &p3DBuffer );
		//Get the 3D buffer parameters
		bufferParams.dwSize = sizeof(DS3DBUFFER);
		p3DBuffer->GetAllParameters( &bufferParams );

		//Set new 3D buffer parameters
		bufferParams.dwMode = DS3DMODE_NORMAL ;
		p3DBuffer->SetAllParameters( &bufferParams, DS3D_IMMEDIATE );
	}
#endif

}

//******************************************************************
PumaSound::~PumaSound()
{
#ifndef NOSOUND
	if (!systemReady)
		return;
	SAFE_RELEASE( p3DBuffer );
	SAFE_DELETE( sound );
#endif

}

//******************************************************************
void PumaSound::SetupSoundSystem(void)
{
#ifndef NOSOUND
	systemReady = FALSE;
//	return;

	// Create a static IDirectSound in the CSound class.  
	// Set coop level to DSSCL_PRIORITY, and set primary buffer 
	// format to stereo, 22kHz and 16-bit output.
	soundManager = new CSoundManager();

	aLog.Log("loading soundManager: ");
	aLog.Log((int)soundManager);
	aLog.Log("\n");

	HRESULT hr = soundManager->Initialize( puma->m_hWnd, DSSCL_PRIORITY, 2, 22050, 16 );
	if (hr != S_OK)
	{
		aLog.Log("init failure\n");

		delete soundManager;
		soundManager = NULL;
		return;
	}
	systemReady = TRUE;

	//Get the 3D listener, so we can control its params
	soundManager->Get3DListenerInterface( &listener );

   // Get listener parameters
   listenerParams.dwSize = sizeof(DS3DLISTENER);
   listener->GetAllParameters( &listenerParams );

#endif

}

//******************************************************************
void PumaSound::ShutdownSoundSystem(void)
{

#ifndef NOSOUND
	if (!systemReady)
		return;
	SAFE_RELEASE( listener );
	SAFE_DELETE( soundManager );
#endif

}

//******************************************************************
HRESULT PumaSound::Play          (void)
{
#ifndef NOSOUND
	if (!systemReady || !playSounds)
		return 0;
	if (!sound)
		return 0;
	aLog.Log("playing sound\n");
	sound->Stop();
	sound->Reset();
	return sound->Play(0,0);
#else
	return 0;
#endif

}

//******************************************************************
HRESULT PumaSound::PlayNo3D       (void)
{
#ifndef NOSOUND
	if (!systemReady || !playUISounds)
		return 0;
	if (!sound)
		return 0;
	aLog.Log("playing sound No3D\n");
	sound->Stop();
	sound->Reset();
	PositionSound(lastPos, lastMaxDist);
	return sound->Play(0,0);
#else
	return 0;
#endif

}

//******************************************************************
HRESULT PumaSound::PlayLooping   (void)
{
#ifndef NOSOUND
	if (!systemReady || !playSounds)
		return 0;
	if (!sound)
		return 0;
	aLog.Log("playing sound looping\n");
	sound->Stop();
	sound->Reset();
	return sound->Play(0,DSBPLAY_LOOPING);
#else
	return 0;
#endif

}

//******************************************************************
HRESULT PumaSound::Stop          (void)
{
#ifndef NOSOUND
	if (!systemReady)
		return 0;
	if (!sound)
		return 0;
	aLog.Log("stopping sound\n");
	return sound->Stop();
#else
	return 0;
#endif

}

//******************************************************************
HRESULT PumaSound::Reset         (void)
{
#ifndef NOSOUND
	if (!systemReady)
		return 0;
	if (!sound)
		return 0;
	aLog.Log("sound reset\n");
	return sound->Reset();
#else
	return 0;
#endif

}

//******************************************************************
BOOL    PumaSound::IsSoundPlaying(void)
{
#ifndef NOSOUND
	if (!systemReady)
		return 0;
	if (!sound)
		return 0;
	return sound->IsSoundPlaying();
#else
	return 0;
#endif

}

//******************************************************************
void PumaSound::PositionSound(D3DVECTOR pos, float maxDist)
{
#ifndef NOSOUND
	if (!systemReady)
		return;
	if (!sound)
		return;
	bufferParams.vPosition = pos;
	bufferParams.flMaxDistance = maxDist;
	bufferParams.flMinDistance = 2.0f;
	p3DBuffer->SetAllParameters( &bufferParams, DS3D_DEFERRED );
	lastMaxDist = maxDist;
#endif

}

//******************************************************************
void PumaSound::UpdateAll3DSound(D3DVECTOR pos, float angle)
{
#ifndef NOSOUND
	if (!systemReady)
		return;
	lastPos = pos;
	if (listener)
	{
		D3DXVECTOR3 dx;
		D3DVECTOR d;

//		angle *= -1 + D3DX_PI;
		angle += D3DX_PI;

		dx.x = sin(angle);
		dx.z = cos(angle);
		dx.y = 0;
		D3DXVec3Normalize( &dx, &dx );
		d = dx;
		listenerParams.vOrientFront = d;
		listenerParams.vPosition    = pos;
//		listenerParams.flRolloffFactor = 2.0f; 
	   listener->SetAllParameters( &listenerParams, DS3D_DEFERRED );

		listener->CommitDeferredSettings();
	}
#endif

}




/* end of file */
