#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_DragEdge.h"
#include "UIR_ScrollWindow.h"

enum
{
	DRAG_EDGE_TOP,
	DRAG_EDGE_LEFT,
	DRAG_EDGE_BOTTOM,
	DRAG_EDGE_RIGHT,
};

//**********************************************************************************
//**********************************************************************************
UIRectDragEdge::UIRectDragEdge(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid,boxToCopy, doname, xparent)
{
	if (boxToCopy.bottom > -1)
		side = DRAG_EDGE_TOP;
	if (boxToCopy.left > -1)
		side = DRAG_EDGE_RIGHT;
	if (boxToCopy.top > -1)
		side = DRAG_EDGE_BOTTOM;
	if (boxToCopy.right > -1)
		side = DRAG_EDGE_LEFT;
}

//**********************************************************************************
UIRectDragEdge::UIRectDragEdge(int doid, int left, int top, int right, int bottom, 
                                   char *doname, UIRect *xparent)
	: UIRectWindow(doid,left, top, right, bottom, doname, xparent)
{
	if (bottom > -1)
		side = DRAG_EDGE_TOP;
	if (left > -1)
		side = DRAG_EDGE_RIGHT;
	if (top > -1)
		side = DRAG_EDGE_BOTTOM;
	if (right > -1)
		side = DRAG_EDGE_LEFT;
}

//**********************************************************************************
UIRectDragEdge::~UIRectDragEdge()
{
    ;
}

//**********************************************************************************
//void UIRectDragEdge::DrawSelf(int drawIntoBuffer)
void UIRectDragEdge::DrawSelf(void)
{
   // base class first
	absBox.top -= 1;
   UIRectWindow::DrawSelf();
	absBox.top += 1;
/*
   RECT originalBox = DrawLib::GetDrawRect();

   RECT diffBox = GetKnobBox();
   if (!SubtractRects(diffBox, originalBox))
       return;
   DrawLib::SetDrawRect(diffBox);

   DrawLib::ClearDrawRect(GetRValue(*colorToUse),GetGValue(*colorToUse),GetBValue(*colorToUse));
*/
   return;
}

//**********************************************************************************
int UIRectDragEdge::IsPointInMe(int x, int y)
{

   return UIRectWindow::IsPointInMe(x, y);
}

//**********************************************************************************
int UIRectDragEdge::Action(int type, long x, short y)
{

   if (isDisabled)
       return 0;

   // DragEdges don't have children, so just check if is in box
   if (!isDepressed)
   {
      if (!IsPointInMe(x,y) && isActive)
      {
         isActive = FALSE;
         // remove any peer's active flags
      }

      if (!IsPointInMe(x,y))
         return 0;
   }
   int deltaA;
/*
   RECT diffBox = GetKnobBox();

   float tempF;
   int onKnob = FALSE;
   if (x >= diffBox.left && x <= diffBox.right && y >= diffBox.top && y <= diffBox.bottom)
      onKnob = TRUE;
*/
   switch(type)
   {
   case UIRECT_MOUSE_LUP:
      isDepressed = FALSE;
      break;

   case UIRECT_MOUSE_LDOWN:
      isDepressed = TRUE;
      orgMouseX = x;
      orgMouseY = y;
		orgBoxY   = parent->box.top;
		if (DRAG_EDGE_BOTTOM == side)
			orgBoxY   = parent->box.bottom;
		orgBoxX   = parent->box.left;
		if (DRAG_EDGE_RIGHT == side)
			orgBoxX   = parent->box.right;

		return 1;

      if (process)
         return process(this, type, x, y);
      break;

   case UIRECT_MOUSE_MOVE:

      if (isDepressed)
      {
         deltaA = y - orgMouseY;
         yPos   = orgBoxY + deltaA;
			if (DRAG_EDGE_BOTTOM == side)
				parent->box.bottom = yPos;
			if (DRAG_EDGE_TOP == side)
				parent->box.top = yPos;

         deltaA = x - orgMouseX;
         xPos   = orgBoxX + deltaA;
			if (DRAG_EDGE_LEFT == side)
				parent->box.left = xPos;
			if (DRAG_EDGE_RIGHT == side)
				parent->box.right = xPos;

	      parent->parent->Arrange();

			return 1;

			/*
         if (isXFree)
         {
            deltaA = x - orgMouseX;
//            deltaB = orgMouseX - orgBoxX;
            xPos   = orgBoxX + deltaA;
            if (xPos < 0)
               xPos = 0;
            if (xPos > innerBox.right - innerBox.left - xWidth)
               xPos = innerBox.right - innerBox.left - xWidth;

            deltaA = innerBox.right - innerBox.left - xWidth;
            tempF = xPos / (float) deltaA * 1000;

            ModifyParent();

            if (process)
               return process(this, UIRECT_DONE, tempF, y);
         }
			*/
      }

      UIRectWindow::Action(type,x,y);

      break;
   }

   return 0;
}


//**********************************************************************************
void UIRectDragEdge::ModifyParent(void)
{

   if (parent)
	{

	}
		/*
		&& !strcmp("SCROLL_WINDOW", parent->WhoAmI()))
   {
      UIRectScrollWindow *scrollWin = (UIRectScrollWindow *) parent;
      if (isYFree)
      {
         int deltaA = innerBox.bottom - innerBox.top - yWidth;
         float tempF = yPos / (float) deltaA;
//         scrollWin->yOffset = scrollWin->innerBounds.top +
         scrollWin->yOffset = 
                    (float)(scrollWin->innerBounds.bottom - 
                            scrollWin->innerBounds.top -
                            (scrollWin->innerBox.bottom - scrollWin->innerBox.top)
                           ) * tempF;
         if (scrollWin->yOffset < 0)
            scrollWin->yOffset = 0;

         scrollWin->yOffset *= -1;


      }
      if (isXFree)
      {
         int deltaA = innerBox.right - innerBox.left - xWidth;
         float tempF = xPos / (float) deltaA;
         scrollWin->xOffset = scrollWin->innerBounds.left +
                    (float)(scrollWin->innerBounds.right - 
                            scrollWin->innerBounds.left -
                            (scrollWin->innerBox.right - scrollWin->innerBox.left)
                           ) * tempF;
         if (scrollWin->xOffset < 0)
            scrollWin->xOffset = 0;

         scrollWin->xOffset *= -1;

      }

      scrollWin->Arrange();

   }
		  */
}




/* end of file */



