#ifndef UIRECT_TEXT_BUTTON_H
#define UIRECT_TEXT_BUTTON_H

//***************************************************************

#include "UIR_Window.h"


enum
{
   UIRECT_BCOL_NORMAL,
   UIRECT_BCOL_ACTIVE,
   UIRECT_BCOL_DEPRESSED,
   UIRECT_BCOL_DISABLED,
   UIRECT_BCOL_MAX
};

//***************************************************************
class UIRectTextButton : public UIRectWindow
{
public:

	UIRectTextButton(int doid, RECT boxToCopy, char *doname = "TEXTBUTTON", UIRect *parent = NULL);
	UIRectTextButton(int doid, int left, int top, int right, int bottom, 
                    char *doname = "TEXTBUTTON", UIRect *parent = NULL);
	virtual ~UIRectTextButton();

   virtual void SetText(char * newText);
	virtual void DrawSelf(void);
	virtual int IsPointInMe(int x, int y);

	virtual int Action(int type, long x, short y);

	DWORD color[UIRECT_BCOL_MAX];
   char *text;
   int font;
   char isMomentary, isExclusive;
	int textOffsetX, textOffsetY;

};


#endif
