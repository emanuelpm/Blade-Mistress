#ifndef LOGINMODE_H
#define LOGINMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"

class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;

class LoginMode : public GameMode
{
public:

	LoginMode(int doid, char *doname);
	virtual ~LoginMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	LPDIRECT3DTEXTURE8 mouseArt, backArt;
	D3DMATERIAL8 neutralMaterial, planetMaterial;

	UIRectWindow *fullWindow;

};

#endif
