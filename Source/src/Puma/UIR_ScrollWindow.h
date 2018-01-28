#ifndef UIRECT_SCROLL_WINDOW_H
#define UIRECT_SCROLL_WINDOW_H

//***************************************************************

#include "UIR_Window.h"


//***************************************************************
class UIRectScrollWindow : public UIRectWindow
{
public:

   UIRectScrollWindow(int doid, RECT boxToCopy, char *doname = "SCROLL_WINDOW", UIRect *parent = NULL);
   UIRectScrollWindow(int doid, int left, int top, int right, int bottom, 
         char *doname = "SCROLL_WINDOW", UIRect *parent = NULL);
   virtual ~UIRectScrollWindow();

   virtual void DrawSelf(void);
   virtual void DrawChildren(void);
   virtual int Action(int type, long x, short y);
   virtual void Arrange(void);

   RECT innerBounds;
   int xOffset, yOffset;
};


#endif
