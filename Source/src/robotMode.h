#ifndef ROBOTMODE_H
#define ROBOTMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"

class VideoDataObject;
class	UIRectWindow;
class UIRectEditLine;

class RobotMode : public GameMode
{
public:

	RobotMode(int doid, char *doname);
	virtual ~RobotMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	VideoDataObject *mouseArt, *backArt, *wallArt, *uiBackground;
	D3DMATERIAL8 neutralMaterial, planetMaterial;
	UIRectWindow *fullWindow;
	int isConnected, tryingConnection;
	DWORD connectTime;

};

#endif
