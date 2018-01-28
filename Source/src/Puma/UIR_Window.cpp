#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_Window.h"

DWORD UIRectWindow::topLeftColorGlobal     = D3DCOLOR_ARGB(uiAlpha, 100, 100, 100);
DWORD UIRectWindow::bottomRightColorGlobal = D3DCOLOR_ARGB(uiAlpha, 070, 070, 70);
DWORD UIRectWindow::fillColorGlobal        = D3DCOLOR_ARGB(uiAlpha, 050, 050, 50);


//**********************************************************************************
//**********************************************************************************
UIRectWindow::UIRectWindow(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRect(doid, boxToCopy, doname, xparent)
{

   topLeftColor     = topLeftColorGlobal    ;
	topLeftColor     = (uiAlpha << 12) + (topLeftColor & 0x00ffffff);
   bottomRightColor = bottomRightColorGlobal;
	bottomRightColor = (uiAlpha << 12) + (topLeftColor & 0x00ffffff);
   fillColor        = fillColorGlobal       ;
	fillColor        = (uiAlpha << 12) + (topLeftColor & 0x00ffffff);

   fillArt = 0;
   fillStyle = UIRECT_WINDOW_STYLE_ALL;
   drawBorder = TRUE;

	textFlags = 0;
}

//**********************************************************************************
UIRectWindow::UIRectWindow(int doid, int left, int top, int right, int bottom, 
                           char *doname, UIRect *xparent)
	: UIRect(doid, left, top, right, bottom, doname, xparent)
{
   topLeftColor     = topLeftColorGlobal    ;
	topLeftColor     = (uiAlpha << 24) + (topLeftColor & 0x00ffffff);
   bottomRightColor = bottomRightColorGlobal;
	bottomRightColor = (uiAlpha << 24) + (bottomRightColor & 0x00ffffff);
   fillColor        = fillColorGlobal       ;
	fillColor        = (uiAlpha << 24) + (fillColor & 0x00ffffff);
/*
   topLeftColor     = topLeftColorGlobal    ;
   bottomRightColor = bottomRightColorGlobal;
   fillColor        = fillColorGlobal       ;
  */
   fillArt = 0;
   fillStyle = UIRECT_WINDOW_STYLE_ALL;
   drawBorder = TRUE;

	textFlags = 0;
}

//**********************************************************************************
UIRectWindow::~UIRectWindow()
{

}

//**********************************************************************************
void UIRectWindow::DrawSelf(void)
{
//   int r,g,b;

   RECT originalBox = puma->clipRect;

//   RECT diffBox = innerBox;
   RECT diffBox = absBox;
   if (!SubtractRects(diffBox, originalBox))
       return;
   puma->SetDrawRect(diffBox);

   originalBox = puma->clipRect; // do NOT use GetDrawRect(), you'll get the whole screen

   // base class first
   UIRect::DrawSelf();

   // now me
   if (fillArt)
   {
		puma->DrawRect(fillArt,absBox, 0xffffffff);
   }
   else if (UIRECT_WINDOW_STYLE_ALL == fillStyle || UIRECT_WINDOW_STYLE_ONLY_FILLED == fillStyle)
   {
		puma->DrawRect(NULL,diffBox, fillColor);
   }

   if (UIRECT_WINDOW_STYLE_ALL == fillStyle || UIRECT_WINDOW_STYLE_ONLY_BORDER == fillStyle)
   {
      DWORD tempColor = topLeftColor;
      if (isActive)
         tempColor = topLeftColor;
      if (isDepressed)
         tempColor = bottomRightColor;

      // left line
      RECT lineBox = absBox;
      lineBox.right = lineBox.left + 1;

      if (SubtractRects(lineBox, originalBox))
      {
         puma->SetDrawRect(lineBox);
			puma->DrawRect(NULL,lineBox, tempColor);
      }
      // top line
      lineBox = absBox;
      lineBox.bottom = lineBox.top + 1;

      if (SubtractRects(lineBox, originalBox))
      {
         puma->SetDrawRect(lineBox);
			puma->DrawRect(NULL,lineBox, tempColor);
      }

      tempColor = bottomRightColor;
      if (isActive)
         tempColor = bottomRightColor;
      if (isDepressed)
         tempColor = topLeftColor;

      // right line
      lineBox = absBox;
      lineBox.left = lineBox.right - 1;

      if (SubtractRects(lineBox, originalBox))
      {
         puma->SetDrawRect(lineBox);
			puma->DrawRect(NULL,lineBox, tempColor);
      }

      // bottom line
      lineBox = absBox;
      lineBox.top = lineBox.bottom - 1;

      if (SubtractRects(lineBox, originalBox))
      {
         puma->SetDrawRect(lineBox);
			puma->DrawRect(NULL,lineBox, tempColor);
      }

   }
  
   puma->SetDrawRect(originalBox);

   return;
}

//**********************************************************************************
int UIRectWindow::IsPointInMe(int x, int y)
{

   return UIRect::IsPointInMe(x, y);
}

//**********************************************************************************
int UIRectWindow::Action(int type, long x, short y)
{

   int retVal = UIRect::Action(type, x, y);

   if (0 == retVal && UIRECT_KEY_DOWN == type && (38 == y || 40 == y))
   {
      UIRect *curUIRect = (UIRect *) childRectList.First();
      if (curUIRect)
         curUIRect->isActive = TRUE;
   }

   return retVal;
}


//**********************************************************************************
void UIRectWindow::Arrange(void)
{
   // reset my innerBox (my children may constrict it)
   innerBox = absBox;
   if (UIRECT_WINDOW_STYLE_ALL == fillStyle || 
       UIRECT_WINDOW_STYLE_ONLY_BORDER == fillStyle)
   {
      innerBox.left++;
      innerBox.top++;
      innerBox.right--;
      innerBox.bottom--;
   }
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
				if (innerBox.right < innerBox.left)
					 innerBox.right = innerBox.left;
         }
         if (-1 != curUIRect->box.right)
         {
            curUIRect->absBox.right  = innerBox.left + curUIRect->box.right;
            innerBox.left           += curUIRect->box.right;
				if (innerBox.left > innerBox.right)
					 innerBox.left = innerBox.right;
         }
         if (-1 != curUIRect->box.top)
         {
            curUIRect->absBox.top    = innerBox.bottom - curUIRect->box.top;
            innerBox.bottom         -= curUIRect->box.top;
				if (innerBox.bottom < innerBox.top)
					 innerBox.bottom = innerBox.top;
         }
         if (-1 != curUIRect->box.bottom)
         {
            curUIRect->absBox.bottom = innerBox.top + curUIRect->box.bottom;
            innerBox.top            += curUIRect->box.bottom;
				if (innerBox.top > innerBox.bottom)
					 innerBox.top = innerBox.bottom;
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
      }

      curUIRect->Arrange();

      curUIRect = (UIRect *) childRectList.Next();
   }

   return;
}

/* end of file */



