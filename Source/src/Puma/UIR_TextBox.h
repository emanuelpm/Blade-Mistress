#ifndef UIRECT_TEXT_BOX_H
#define UIRECT_TEXT_BOX_H

//***************************************************************

#include "UIR_Window.h"


//***************************************************************
class UIRectTextBox : public UIRectWindow
{
public:

	UIRectTextBox(int doid, RECT boxToCopy, char *doname = "TEXTBOX", UIRect *parent = NULL);
	UIRectTextBox(int doid, int left, int top, int right, int bottom, 
                    char *doname = "TEXTBOX", UIRect *parent = NULL);
	virtual ~UIRectTextBox();

   virtual void SetText(char * newText);
	virtual void DrawSelf(void);
	DWORD textColor;
   char *text;
   int font;

};


#endif
