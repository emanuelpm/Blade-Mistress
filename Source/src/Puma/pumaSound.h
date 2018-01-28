#ifndef PUMASOUND_H
#define PUMASOUND_H

#include "puma.h"

class PumaSound
{
public:

	PumaSound(char *fileName);
	virtual ~PumaSound();

	static void SetupSoundSystem(void);
	static void ShutdownSoundSystem(void);
	static void UpdateAll3DSound(D3DVECTOR pos, float angle);

   HRESULT Play          (void);
   HRESULT PlayNo3D      (void);
   HRESULT PlayLooping   (void);
   HRESULT Stop          (void);
   HRESULT Reset         (void);
   BOOL    IsSoundPlaying(void);
	void    PositionSound(D3DVECTOR pos, float maxDist = 140.0f);

	CSound* sound;
	LPDIRECTSOUND3DBUFFER   p3DBuffer;   // 3D sound buffer
	DS3DBUFFER              bufferParams;               // 3D buffer properties

	static CSoundManager*          soundManager;
	static LPDIRECTSOUND3DLISTENER listener;       // 3D listener object
	static DS3DLISTENER            listenerParams; // Listener properties

	static D3DVECTOR lastPos;
	static float lastMaxDist;
	static int systemReady, playSounds, playUISounds;

};


#endif
