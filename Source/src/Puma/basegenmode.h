#ifndef BASEGENMODE_H
#define BASEGENMODE_H

#include "pumamesh.h"
#include "gamemode.h"

class BaseGenMode : public GameMode
{
public:

	BaseGenMode(int doid, char *doname);
	virtual ~BaseGenMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);

	void GenerateShip(int targetVerts);

	PumaMesh *partArray;
	PumaMesh *currentShip;
	int numOfMeshes, targetVerts;
	float shipAngle;

	LPDIRECT3DTEXTURE8 *texturePtrArray;
	int numOfTextures, currentTextureIndex;

   BYTE    oldKeyState[256];   // DirectInput keyboard state buffer 

};

#endif
