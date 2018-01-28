#ifndef UIRECT_DRAG_EDGE_H
#define UIRECT_DRAG_EDGE_H

//***************************************************************

#include "UIR_Window.h"


//***************************************************************
class UIRectDragEdge : public UIRectWindow
{
public:

   UIRectDragEdge(int doid, RECT boxToCopy, char *doname = "DRAGEDGE", UIRect *parent = NULL);
   UIRectDragEdge(int doid, int left, int top, int right, int bottom, 
                   char *doname = "DRAGEDGE", UIRect *parent = NULL);
   virtual ~UIRectDragEdge();

//   virtual void DrawSelf(int drawIntoBuffer = FALSE);
   virtual void DrawSelf(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);

   void  ModifyParent(void);

   int orgMouseX, orgMouseY, orgBoxX, orgBoxY, xPos, yPos, side;

};


#endif
