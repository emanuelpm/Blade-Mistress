#ifndef UIRECT_ARTTEXT_BUTTON_H
#define UIRECT_ARTTEXT_BUTTON_H

//***************************************************************

#include "UIR_TextButton.h"

//***************************************************************
class UIRectArtTextButton : public UIRectTextButton
{
public:

	UIRectArtTextButton(int doid, RECT boxToCopy, char *doname = "ARTTEXTBUTTON", UIRect *parent = NULL);
	UIRectArtTextButton(int doid, int left, int top, int right, int bottom, 
                    char *doname = "ARTTEXTBUTTON", UIRect *parent = NULL);
	virtual ~UIRectArtTextButton();

	void SetArt(int type, char *fileName, DWORD transparency);

	virtual void DrawSelf(void);

	LPDIRECT3DTEXTURE8 art[UIRECT_BCOL_MAX];

	int artOffsetX, artOffsetY;
};


#endif
