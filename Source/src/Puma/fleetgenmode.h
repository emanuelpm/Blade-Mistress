#ifndef FLEETGENMODE_H
#define FLEETGENMODE_H

#include "pumamesh.h"
#include "gamemode.h"

class FleetGenMode : public GameMode
{
public:

	FleetGenMode(int doid, char *doname);
	virtual ~FleetGenMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);

	void GenerateShip(int targetVerts, int reDoIndex = -1);

	PumaMesh *partArray;
	PumaMesh *currentShip[10], *currentBase;
	int numOfMeshes, targetVerts;
	float shipAngle;

	LPDIRECT3DTEXTURE8 *texturePtrArray;
	int numOfTextures, currentTextureIndex;

   BYTE    oldKeyState[256];   // DirectInput keyboard state buffer 

};

#endif
