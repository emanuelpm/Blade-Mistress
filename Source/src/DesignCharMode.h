#ifndef DESIGNCHARMODE_H
#define DESIGNCHARMODE_H

#include ".\puma\gamemode.h"
//#include "spaceTest.h"
#include ".\network\NetWorldMessages.h"

class PumaAnim;
class AvatarTexture;

class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;

class DesignCharMode : public GameMode
{
public:

	DesignCharMode(MessAvatarStats *statsToChange, int doid, char *doname);
	virtual ~DesignCharMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);
	void			UpdateStats(int type, int change);

	LPDIRECT3DTEXTURE8 mouseArt, backArt, backArt2, urFrame, uiArt, statBackArt;
	LPDIRECT3DTEXTURE8 controlBack1, controlBack2, cornerArt[6], sliderArt;
	D3DMATERIAL8 neutralMaterial;
	PumaAnim *anim;
	float animSpin, colorBoxLeft;
	AvatarTexture *avTexture;
	int animCounter;

	UIRectWindow *fullWindow;

	MessAvatarStats *statsToChange;


//					tempMob->avTexture->Generate(puma->pd3dDevice, messAvStats->artIndex,
//											messAvStats->hairR, messAvStats->hairG, messAvStats->hairB);

};

#endif
