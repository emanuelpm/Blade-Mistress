#ifndef CREDITMODE_H
#define CREDITMODE_H

#include ".\puma\gamemode.h"
#include "inventory.h"
#include ".\network\NetWorldMessages.h"


class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;
class BBOMob;
class UIRectTextBox;

class CreditMode : public GameMode
{
public:

	CreditMode(int doid, char *doname);
	virtual ~CreditMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	unsigned long flashCounter;

	LPDIRECT3DTEXTURE8 mouseArt, uiPopUpArt;
	//, skyArt, rosetteArt[5], barArt[2], 
//		                buttonArt, uiArt, uiPlayerListArt, uiOtherListArt,
//							 uiPopUpArt;

	UIRectWindow *fullWindow;
//	UIRectEditLine *edLine;

};

#endif
