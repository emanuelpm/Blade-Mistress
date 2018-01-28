#ifndef UIRECT_EDIT_LINE_H
#define UIRECT_EDIT_LINE_H

//***************************************************************

#include "UIR_Window.h"

enum
{
   UIRECT_ELCOL_NONACTIVE,
   UIRECT_ELCOL_ACTIVE,
   UIRECT_ELCOL_DISABLED,
   UIRECT_ELCOL_MAX
};

const int EDITLINE_HISTORY_SIZE = 25;

//***************************************************************
class UIRectEditLine : public UIRectWindow
{
public:

	UIRectEditLine(int doid, RECT boxToCopy, int lineLen = 128, char *doname = "EDITLINE", UIRect *parent = NULL);
	UIRectEditLine(int doid, int left, int top, int right, int bottom, int lineLen = 128, 
                    char *doname = "EDITLINE", UIRect *parent = NULL);
	virtual ~UIRectEditLine();

   virtual void SetText(char * newText);
	virtual void DrawSelf(void);

	virtual int Action(int type, long x, short y);

   void RecursiveDeactivateEditLines(UIRect *curRect);
	void SwitchToNextEditLine(void);

	DWORD color[UIRECT_ELCOL_MAX];
   char *text;
   char *historyText[EDITLINE_HISTORY_SIZE];
   int lineLength, cursorPos, offsetPos, historyIndex;
   int font, hasTypingFocus, suppressEnter, isSecure;
	int numbersOnly, restricted;
	int least, most;


   static float blinkTimer;



};


#endif
