#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_ScrollWindow.h"


//**********************************************************************************
//**********************************************************************************
UIRectScrollWindow::UIRectScrollWindow(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid, boxToCopy, doname, xparent)
{
   innerBounds = innerBox;
   xOffset = yOffset = 0;
}

//**********************************************************************************
UIRectScrollWindow::UIRectScrollWindow(int doid, int left, int top, int right, int bottom, 
                           char *doname, UIRect *xparent)
	: UIRectWindow(doid, left, top, right, bottom, doname, xparent)
{
   innerBounds = innerBox;
   xOffset = yOffset = 0;
}

//**********************************************************************************
UIRectScrollWindow::~UIRectScrollWindow()
{

}


//**********************************************************************************
int UIRectScrollWindow::Action(int type, long x, short y)
{

   return UIRect::Action(type, x, y);
}


//**********************************************************************************
void UIRectScrollWindow::Arrange(void)
{
   innerBounds.left = innerBounds.top = innerBounds.right = innerBounds.bottom = 0;

   // reset my innerBox (my children may constrict it)
   innerBox = absBox;
   innerBox.left++;
   innerBox.top++;
   innerBox.right--;
   innerBox.bottom--;

   // reset the children's rects based on myself.
   UIRect *curUIRect = (UIRect *) childRectList.First();
   while (curUIRect)
   {
      int numGlueSides = 0;
      if (-1 == curUIRect->box.left)
         numGlueSides++;
      if (-1 == curUIRect->box.right)
         numGlueSides++;
      if (-1 == curUIRect->box.top)
         numGlueSides++;
      if (-1 == curUIRect->box.bottom)
         numGlueSides++;

      if (4 == numGlueSides)
      {
         // This child wants to occupy all the space of its parent.  Okay, I guess.
         curUIRect->absBox = innerBox;
      }
      else if (3 == numGlueSides)
      {
         // this child sticks to a whole side of the parent, constricting it's innerBox
         curUIRect->absBox = innerBox;
         if (-1 != curUIRect->box.left)
         {
            curUIRect->absBox.left   = innerBox.right - curUIRect->box.left;
            innerBox.right          -= curUIRect->box.left;
         }
         if (-1 != curUIRect->box.right)
         {
            curUIRect->absBox.right  = innerBox.left + curUIRect->box.right;
            innerBox.left           += curUIRect->box.right;
         }
         if (-1 != curUIRect->box.top)
         {
            curUIRect->absBox.top    = innerBox.bottom - curUIRect->box.top;
            innerBox.bottom         -= curUIRect->box.top;
         }
         if (-1 != curUIRect->box.bottom)
         {
            curUIRect->absBox.bottom = innerBox.top + curUIRect->box.bottom;
            innerBox.top            += curUIRect->box.bottom;
         }
      }
      else
      {
         // a normal child
         curUIRect->absBox.left = curUIRect->box.left + innerBox.left;
         if (-1 == curUIRect->box.left)
            curUIRect->absBox.left   = innerBox.left;

         curUIRect->absBox.right = curUIRect->box.right + innerBox.left;
         if (-1 == curUIRect->box.right)
            curUIRect->absBox.right   = innerBox.right;

         curUIRect->absBox.top = curUIRect->box.top + innerBox.top;
         if (-1 == curUIRect->box.top)
            curUIRect->absBox.top   = innerBox.top;

         curUIRect->absBox.bottom = curUIRect->box.bottom + innerBox.top;
         if (-1 == curUIRect->box.bottom)
            curUIRect->absBox.bottom   = innerBox.bottom;

         // push out inner bounds
         if (innerBounds.left   > curUIRect->box.left)
             innerBounds.left   = curUIRect->box.left;
         if (innerBounds.top    > curUIRect->box.top)
             innerBounds.top    = curUIRect->box.top;
         if (innerBounds.right  < curUIRect->box.right)
             innerBounds.right  = curUIRect->box.right;
         if (innerBounds.bottom < curUIRect->box.bottom)
             innerBounds.bottom = curUIRect->box.bottom;

         // shift child by offsets
         curUIRect->absBox.left   += xOffset;
         curUIRect->absBox.top    += yOffset;
         curUIRect->absBox.right  += xOffset;
         curUIRect->absBox.bottom += yOffset;
      }

      curUIRect->Arrange();

      curUIRect = (UIRect *) childRectList.Next();
   }

   return;
}

//**********************************************************************************
void UIRectScrollWindow::DrawChildren(void)
{
	UIRect::DrawChildren();
}
//**********************************************************************************
void UIRectScrollWindow::DrawSelf(void)
{
	UIRectWindow::DrawSelf();
}



/* end of file */



