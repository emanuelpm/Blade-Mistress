#ifndef GENERATORMODE_H
#define GENERATORMODE_H

#include "pumamesh.h"
#include "gamemode.h"

class GeneratorMode : public GameMode
{
public:

	GeneratorMode(int doid, char *doname);
	virtual ~GeneratorMode();

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
