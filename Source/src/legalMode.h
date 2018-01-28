#ifndef LEGALMODE_H
#define LEGALMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"

class VideoDataObject;
class	UIRectWindow;
class UIRectEditLine;

class LegalMode : public GameMode
{
public:

	LegalMode(int doid, char *doname);
	virtual ~LegalMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	VideoDataObject *mouseArt, *backArt, *wallArt, *uiBackground;
	D3DMATERIAL8 neutralMaterial, planetMaterial;
	UIRectWindow *fullWindow;
	int isConnected;

};

#endif
