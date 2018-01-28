#ifndef PICKCHARMODE_H
#define PICKCHARMODE_H

#include ".\puma\gamemode.h"
#include "LongTime.h"
#include ".\network\NetWorldMessages.h"


class PumaAnim;
class AvatarTexture;

class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;

class PickCharMode : public GameMode
{
public:

	PickCharMode(int doid, char *doname);
	virtual ~PickCharMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	LPDIRECT3DTEXTURE8 mouseArt, infoArtRight, infoArtLeft, urFrame, 
		uiArt, edgeLow, edgeHigh, uiPopUpLongArt;
	D3DMATERIAL8 neutralMaterial, planetMaterial;
	PumaAnim *anim;
	int animCounter;
	float animSpin;
	AvatarTexture *avTexture;

	UIRectWindow *fullWindow, *confirmWindow, *confirm2Window;
	int confirming;
   UIRectTextButton *newChar, *enter;
	UIRectEditLine *newName;

	int curCharacterIndex, numChars;
	int charExists;
	char lastName[32];
	MessAvatarStats *statsToChange, lastStats;

	LongTime accountTime;
	int showCodeEdLine;

//					tempMob->avTexture->Generate(puma->pd3dDevice, messAvStats->artIndex,
//											messAvStats->hairR, messAvStats->hairG, messAvStats->hairB);

};

#endif
