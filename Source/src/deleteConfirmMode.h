#ifndef STARTNEWCHARMODE_H
#define STARTNEWCHARMODE_H

#include "gamemode.h"
//#include "spaceTest.h"

class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;

class StartNewCharMode : public GameMode
{
public:

	StartNewCharMode(char *name, char *pass, int doid, char *doname);
	virtual ~StartNewCharMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	LPDIRECT3DTEXTURE8 mouseArt, backArt;
	D3DMATERIAL8 neutralMaterial, planetMaterial;

	UIRectWindow *fullWindow;
//	char string[2048];
	char name[32], pass[12];

};

#endif
