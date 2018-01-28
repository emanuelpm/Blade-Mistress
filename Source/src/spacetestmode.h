#ifndef SPACETESTMODE_H
#define SPACETESTMODE_H

#include "gamemode.h"
//#include "spaceTest.h"

enum
{
   RM_SUBMODE_LOADING,
   RM_SUBMODE_START,
   RM_SUBMODE_RACING,
   RM_SUBMODE_PLAYER_FINISHED,
   RM_SUBMODE_PAUSE,
   RM_SUBMODE_QUIT_QUESTION,
   RM_SUBMODE_MAX
};


class	UIRectWindow;

class SpaceTestMode : public GameMode
{
public:

	SpaceTestMode(int doid, char *doname);
	virtual ~SpaceTestMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);


	DoublyLinkedList ships;

	unsigned long flashCounter;

	D3DMATERIAL8 neutralMaterial, planetMaterial;
	LPDIRECT3DTEXTURE8 mouseArt;

	UIRectWindow *fullWindow;

	float curCamAngle, realCamAngle;
	float camCenterX, camCenterZ;



};

#endif
