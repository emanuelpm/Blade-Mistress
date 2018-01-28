#ifndef WIKIMODE_H
#define WIKIMODE_H

#include ".\puma\gamemode.h"
#include "inventory.h"
#include ".\network\NetWorldMessages.h"


class PumaMesh;
class	UIRectWindow;
class UIRectEditLine;
class UIRectTextButton;
class UIRectScrollWindow;
class BBOMob;
class UIRectTextBox;

class WikiMode : public GameMode
{
public:

	WikiMode(int doid, char *doname);
	virtual ~WikiMode();

	virtual int Init(void); 		// do this when instantiated.
	virtual int Activate(void); 	// do this when the mode becomes the forground mode.
	virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
	virtual void SetEnvironment(void);
	virtual int Tick(void);
	long        WindowServicer(void);
	void        HandleMessages(void);

	void        LoadPage(char *pageURL);
	void        AddTextBox(char *text, char *altText, int isButton);

	unsigned long flashCounter;

	LPDIRECT3DTEXTURE8 mouseArt, uiPopUpArt, uiPopUpLongArt;
	//, skyArt, rosetteArt[5], barArt[2], 
//		                buttonArt, uiArt, uiPlayerListArt, uiOtherListArt,
//							 uiPopUpArt;

	UIRectWindow *fullWindow;
//	UIRectEditLine *edLine;

	UIRectScrollWindow *scrollWin;
	UIRectTextButton   *linkUsed;
};

#endif
