#ifndef FIRSTOPTIONMODE_H
#define FIRSTOPTIONMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"

class VideoDataObject;
class	UIRectWindow;
class UIRectEditLine;

class FirstOptionMode : public GameMode
{
public:

	FirstOptionMode(int doid, char *doname);
	virtual ~FirstOptionMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	VideoDataObject *mouseArt, *backArt, *backArt2, *urFrame, *wallArt, *uiBackground,
		             *login1, *login2, *cornerArt[6], *serverArt[4];
	D3DMATERIAL8 neutralMaterial, planetMaterial;
	UIRectWindow *fullWindow, *tempWindow;
	int isConnected, tryingConnection, messSent, chooseServer;
	DWORD connectTime;

};

#endif
