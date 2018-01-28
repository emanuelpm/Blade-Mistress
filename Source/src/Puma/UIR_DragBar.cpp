#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_DragBar.h"
#include "UIR_ScrollWindow.h"


//**********************************************************************************
//**********************************************************************************
UIRectDragBar::UIRectDragBar(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid,boxToCopy, doname, xparent)
{
   xPos = yPos = xMaxPos = yMaxPos = 0;
   xWidth = yWidth = 4;
   isXFree = isYFree = FALSE;
   if (-1 == box.left && -1 == box.right)
      isXFree = TRUE;
   if (-1 == box.top && -1 == box.bottom)
      isYFree = TRUE;

   color[UIRECT_DBCOL_NORMAL   ] = D3DCOLOR_ARGB(255, 200, 200, 255);
   color[UIRECT_DBCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
   color[UIRECT_DBCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);

}

//**********************************************************************************
UIRectDragBar::UIRectDragBar(int doid, int left, int top, int right, int bottom, 
                                   char *doname, UIRect *xparent)
	: UIRectWindow(doid,left, top, right, bottom, doname, xparent)
{
   xPos = yPos = xMaxPos = yMaxPos = 0;
   xWidth = yWidth = 4;
   isXFree = isYFree = FALSE;
   if (-1 == box.left && -1 == box.right)
      isXFree = TRUE;
   if (-1 == box.top && -1 == box.bottom)
      isYFree = TRUE;

   color[UIRECT_DBCOL_NORMAL   ] = D3DCOLOR_ARGB(255, 200, 200, 255);
   color[UIRECT_DBCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
   color[UIRECT_DBCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);

}

//**********************************************************************************
UIRectDragBar::~UIRectDragBar()
{
    ;
}

//**********************************************************************************
void UIRectDragBar::DrawSelf(void)
{
   // base class first
//   UIRectWindow::DrawSelf();

   RECT originalBox = puma->GetDrawRect();

   RECT diffBox = GetKnobBox();
   if (!SubtractRects(diffBox, originalBox))
       return;
   puma->SetDrawRect(diffBox);

   DWORD *colorToUse = &color[UIRECT_DBCOL_NORMAL   ];
   if (isActive)
      colorToUse = &color[UIRECT_DBCOL_ACTIVE   ];
   if (isDisabled)
      colorToUse = &color[UIRECT_DBCOL_DISABLED   ];

	RECT curRect = diffBox; //puma->GetDrawRect();
//		puma->DrawRect(fillArt,absBox, 0xffffffff);
	if (fillArt)
		puma->DrawRect(fillArt, curRect, *colorToUse);
	else
		puma->DrawRect(NULL, curRect, *colorToUse);

   return;
}

//**********************************************************************************
int UIRectDragBar::IsPointInMe(int x, int y)
{

   return UIRectWindow::IsPointInMe(x, y);
}

//**********************************************************************************
int UIRectDragBar::Action(int type, long x, short y)
{

   if (isDisabled)
       return 0;

   // DragBars don't have children, so just check if is in box
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

   RECT diffBox = GetKnobBox();

   int deltaA, deltaB;
   float tempF;
   int onKnob = FALSE;
   if (x >= diffBox.left && x <= diffBox.right && y >= diffBox.top && y <= diffBox.bottom)
      onKnob = TRUE;

   switch(type)
   {
   case UIRECT_MOUSE_LUP:
      isDepressed = FALSE;
      break;

   case UIRECT_MOUSE_LDOWN:
      if (onKnob)
      {
         isDepressed = TRUE;
         orgMouseX = x;
         orgMouseY = y;
         orgBoxX = xPos;
         orgBoxY = yPos;
      }
      else
      {
//         orgMouseX = -1;
         if (isXFree)
         {
            if (x < diffBox.left)
            {
               xPos -= xWidth;
               if (xPos < 0)
                  xPos = 0;
            }
            if (x > diffBox.right)
            {
               xPos += xWidth;
               if (xPos > innerBox.right - innerBox.left - xWidth)
                  xPos = innerBox.right - innerBox.left - xWidth;
            }

            deltaA = innerBox.right - innerBox.left - xWidth;
            tempF = xPos / (float) deltaA * 1000;

            ModifyParent();

            if (process)
               return process(this, UIRECT_DONE, tempF, y);

         }
         if (isYFree)
         {
            if (y < diffBox.top)
            {
               yPos -= yWidth;
               if (yPos < 0)
                  yPos = 0;
            }
            if (y > diffBox.bottom)
            {
               yPos += yWidth;
               if (yPos > innerBox.bottom - innerBox.top - yWidth)
                  yPos = innerBox.bottom - innerBox.top - yWidth;
            }
            deltaA = innerBox.bottom - innerBox.top - yWidth;
            tempF = yPos / (float) deltaA * 1000;

            ModifyParent();

            if (process)
               return process(this, UIRECT_DONE, tempF, y);

         }
      }
      break;

   case UIRECT_MOUSE_MOVE:
      if (!IsPointInMe(x,y))
         return 0;

      if (isDepressed)
      {
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
         if (isYFree)
         {
            deltaA = y - orgMouseY;
            yPos   = orgBoxY + deltaA;
            if (yPos < 0)
               yPos = 0;
            if (yPos > innerBox.bottom - innerBox.top - yWidth)
               yPos = innerBox.bottom - innerBox.top - yWidth;

            deltaA = innerBox.bottom - innerBox.top - yWidth;
            tempF = yPos / (float) deltaA * 1000;

            ModifyParent();

            if (process)
               return process(this, UIRECT_DONE, tempF, y);
         }
      }

      UIRectWindow::Action(type,x,y);

      break;

	case UIRECT_MOUSE_TEST_POS  :
		if (IsPointInMe(x,y))
		{
			if (gTouchedUIRect != this)
			{
				if (process)
				{
					gTouchedUIRect = this;
					return process(this, type, x, y);
				}
			}
			return 1;
		}
   }
/*
   if (process)
   {
      return process(this, type, x, y);
   }
   */
   return 0;
}

//**********************************************************************************
RECT UIRectDragBar::GetKnobBox(void)
{

   if (!isXFree || isDisabled)
   {
       xWidth = innerBox.right - innerBox.left - 2;
       xPos = 1;
   }
   if (!isYFree || isDisabled)
   {
       yWidth = innerBox.bottom - innerBox.top - 2;
       yPos = 1;
   }

   RECT diffBox = innerBox;
   diffBox.left   += xPos;
   diffBox.top    += yPos;
   diffBox.right   = diffBox.left + xWidth;
   diffBox.bottom  = diffBox.top + yWidth;

   return diffBox;
}


//**********************************************************************************
void UIRectDragBar::ModifyParent(void)
{

   if (parent && !strcmp("SCROLL_WINDOW", parent->WhoAmI()))
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

}


//**********************************************************************************
float UIRectDragBar::GetYVal(void)
{
   int deltaA = innerBox.bottom - innerBox.top - yWidth;
   float tempF = yPos / (float) deltaA;
   return tempF;
}

//**********************************************************************************
float UIRectDragBar::GetXVal(void)
{
   int deltaA = innerBox.right - innerBox.left - xWidth;
   float tempF = xPos / (float) deltaA;
   return tempF;
}

//**********************************************************************************
void UIRectDragBar::SetYVal(float val)
{
   int size = innerBox.bottom - innerBox.top - yWidth;
   yPos = (float) size * val;
}

//**********************************************************************************
void UIRectDragBar::SetXVal(float val)
{
   int size = innerBox.right - innerBox.left - xWidth;
   xPos = (float) size * val;
}







/* end of file */



