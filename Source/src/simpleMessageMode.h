#ifndef SIMPLEMESSAGEMODE_H
#define SIMPLEMESSAGEMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"

class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;

class SimpleMessageMode : public GameMode
{
public:

	SimpleMessageMode(char *string, int doid, char *doname);
	virtual ~SimpleMessageMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	LPDIRECT3DTEXTURE8 mouseArt, backArt;
	D3DMATERIAL8 neutralMaterial, planetMaterial;

	UIRectWindow *fullWindow;
	char string[2048];

};

#endif
