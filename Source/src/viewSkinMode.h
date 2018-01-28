#ifndef VIEWSKINMODE_H
#define VIEWSKINMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"
#include ".\network\NetWorldMessages.h"

class PumaAnim;
class AvatarTexture;

class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;

class ViewSkinMode : public GameMode
{
public:

	ViewSkinMode(int doid, char *doname);
	virtual ~ViewSkinMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);
//	void			UpdateStats(int type, int change);

	LPDIRECT3DTEXTURE8 mouseArt, texture, modelTexture;
	D3DMATERIAL8 neutralMaterial;
	PumaAnim *anim;
	float animSpin, animPitch;
	int animCounter, animateFlag, flashCounter, flashSize;

	D3DCOLOR oldColor;
	int oldColorX, oldColorY;

	int mouseStartX, mouseStartY;
	int mouseDrag, mouseDrag2;
	float meshX, meshY;

	UIRectWindow *fullWindow;

	MessAvatarStats *statsToChange;


//					tempMob->avTexture->Generate(puma->pd3dDevice, messAvStats->artIndex,
//											messAvStats->hairR, messAvStats->hairG, messAvStats->hairB);

};

#endif
