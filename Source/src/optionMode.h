#ifndef OPTIONMODE_H
#define OPTIONMODE_H

#include "./puma/gamemode.h"
#include "inventory.h"
#include "./network/NetWorldMessages.h"


class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;
class BBOMob;
class UIRectTextBox;

class OptionMode : public GameMode
{
public:

	OptionMode(int doid, char *doname);
	virtual ~OptionMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	void        RefreshSettingsButtons(void);

	static void SaveLocalSettings(void);
	static void LoadLocalSettings(void);

	unsigned long flashCounter;

	LPDIRECT3DTEXTURE8 mouseArt, uiPopUpArt, uiPopUpLongArt;
	//, skyArt, rosetteArt[5], barArt[2], 
//		                buttonArt, uiArt, uiPlayerListArt, uiOtherListArt,
//							 uiPopUpArt;

	UIRectWindow *fullWindow;
//	UIRectEditLine *edLine;

};

extern unsigned long localInfoFlags;

#endif
