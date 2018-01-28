#ifndef UIRECT_DRAG_BAR_H
#define UIRECT_DRAG_BAR_H

//***************************************************************

#include "UIR_Window.h"


enum
{
   UIRECT_DBCOL_NORMAL,
   UIRECT_DBCOL_ACTIVE,
   UIRECT_DBCOL_DISABLED,
   UIRECT_DBCOL_MAX
};

//***************************************************************
class UIRectDragBar : public UIRectWindow
{
public:

   UIRectDragBar(int doid, RECT boxToCopy, char *doname = "DRAGBAR", UIRect *parent = NULL);
   UIRectDragBar(int doid, int left, int top, int right, int bottom, 
                   char *doname = "DRAGBAR", UIRect *parent = NULL);
   virtual ~UIRectDragBar();

   virtual void DrawSelf(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);

   virtual float GetXVal(void);  // returns 0.0 to 1.0
   virtual float GetYVal(void);  // returns 0.0 to 1.0
   virtual void  SetXVal(float a); // returns 0.0 to 1.0
   virtual void  SetYVal(float a); // returns 0.0 to 1.0

   RECT  GetKnobBox(void);
   void  ModifyParent(void);

   DWORD color[UIRECT_DBCOL_MAX];

   int isXFree, isYFree;  // determines how the knob can move
   int xPos, yPos;        // position of the knob in pixels from top/left
   int xWidth, yWidth;    // pixel size of the knob in the free directions, non-free width is across innerBox
   int xMaxPos, yMaxPos;  // maximum position of xPos/yPos; right-left-xWidth

   int orgMouseX, orgMouseY, orgBoxX, orgBoxY;

};


#endif
