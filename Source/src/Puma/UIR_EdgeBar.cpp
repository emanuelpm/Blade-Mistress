#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_EdgeBar.h"
#include "puma.h"


//**********************************************************************************
//**********************************************************************************
UIRectEdgeBar::UIRectEdgeBar(int doid, int left, int top, int right, int bottom, UIRect *xparent)
	: UIRectWindow(doid, left, top, right, bottom, "EDGE_BAR", xparent)
{
   isMomentary = TRUE;
   isExclusive = FALSE;

	color[UIRECT_EBCOL_NORMAL   ] = D3DCOLOR_ARGB(255, 200, 255, 200);
	color[UIRECT_EBCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 255, 100);
	color[UIRECT_EBCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 000, 155, 000);
	color[UIRECT_EBCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);

   topLeftColor     = D3DCOLOR_ARGB(uiAlpha, 100, 100, 100);
   bottomRightColor = D3DCOLOR_ARGB(uiAlpha, 070, 070, 70);
   fillColor        = D3DCOLOR_ARGB(uiAlpha, 00, 150, 0);

	if (bottom != -1)
		barType = 0;
	if (right != -1)
		barType = 1;
}


//**********************************************************************************
UIRectEdgeBar::~UIRectEdgeBar()
{

}

//**********************************************************************************
void UIRectEdgeBar::DrawSelf(void)
{
   // base class first
	absBox.top -= 2;
   UIRectWindow::DrawSelf();
	absBox.top += 2;

/*
   DWORD *colorToUse = &color[UIRECT_EBCOL_NORMAL   ];
   if (isActive)
      colorToUse = &color[UIRECT_EBCOL_ACTIVE   ];
   if (isDepressed && isActive && isMomentary)
      colorToUse = &color[UIRECT_EBCOL_DEPRESSED   ];
   if (isDepressed && !isMomentary)
      colorToUse = &color[UIRECT_EBCOL_DEPRESSED   ];
   if (isDisabled)
      colorToUse = &color[UIRECT_EBCOL_DISABLED   ];

   RECT originalBox = puma->GetDrawRect();

   RECT diffBox = innerBox;
   if (!SubtractRects(diffBox, originalBox))
       return;
   puma->SetDrawRect(diffBox);

   puma->SetDrawRect(originalBox);
*/
   return;
}

//**********************************************************************************
int UIRectEdgeBar::IsPointInMe(int x, int y)
{

   return UIRectWindow::IsPointInMe(x, y);
}

//**********************************************************************************
int UIRectEdgeBar::Action(int type, long x, short y)
{
	int delta;

   if (isDisabled)
       return 0;

   switch(type)
   {
   case UIRECT_MOUSE_LDOWN:
      // EdgeBars don't have children, so just check if is in box
      if (!IsPointInMe(x,y) && isActive)
      {
         isActive = FALSE;
         // remove any peer's active flags
      }

      if ((!IsPointInMe(x,y)) && isDepressed && isMomentary)
      {
         isDepressed = FALSE;
         // remove any peer's active flags
      }

      if (!IsPointInMe(x,y))
         return 0;

      if (isMomentary)
      {
         isDepressed = TRUE;
         gPressedUIRect = this;
      }
      else
      {
         if (isDepressed)
            isDepressed = FALSE;
         else
            isDepressed = TRUE;
         if (process)
            return process(this, type, x, y);
      }
      break;

   case UIRECT_MOUSE_LUP:
      // EdgeBars don't have children, so just check if is in box
      if (!IsPointInMe(x,y) && isActive)
      {
         isActive = FALSE;
         // remove any peer's active flags
      }

      if ((!IsPointInMe(x,y)) && isDepressed && isMomentary)
      {
         isDepressed = FALSE;
         // remove any peer's active flags
      }

      if (!IsPointInMe(x,y))
         return 0;

      if (isMomentary)
      {
         isDepressed = FALSE;
         gPressedUIRect = NULL;

			switch(barType)
			{
			case 0: // bottom edge of screen
				if (parent->box.bottom == puma->ScreenH())
				{
					delta = (parent->box.bottom - parent->box.top) - (absBox.bottom - absBox.top);
					parent->box.bottom += delta;
					parent->box.top    += delta;
			      parent->parent->Arrange();
				}
				else
				{
					delta = (parent->box.bottom - parent->box.top);
					parent->box.bottom = puma->ScreenH();
					parent->box.top    = puma->ScreenH() - delta;
			      parent->parent->Arrange();
				}
				break;

			case 1: // right edge of screen
				if (parent->box.right == puma->ScreenW())
				{
					delta = (parent->box.right - parent->box.left) - (absBox.right - absBox.left);
					parent->box.right += delta;
					parent->box.left  += delta;
			      parent->parent->Arrange();
				}
				else
				{
					delta = (parent->box.right - parent->box.left);
					parent->box.right = puma->ScreenW();
					parent->box.left  = puma->ScreenW() - delta;
			      parent->parent->Arrange();
				}
				break;

			}

         if (process)
            return process(this, type, x, y);
      }
      break;

   case UIRECT_MOUSE_MOVE:
      // EdgeBars don't have children, so just check if is in box
      if (!IsPointInMe(x,y) && isActive)
      {
         isActive = FALSE;
         // remove any peer's active flags
      }

      if ((!IsPointInMe(x,y)) && isDepressed && isMomentary)
      {
         isDepressed = FALSE;
         // remove any peer's active flags
      }

      if ((IsPointInMe(x,y)) && gPressedUIRect == this)
      {
         isDepressed = TRUE;
         // remove any peer's active flags
      }

      if (!IsPointInMe(x,y))
         return 0;

   case UIRECT_KEY_DOWN   :
   case UIRECT_KEY_UP     :

      return UIRectWindow::Action(type,x,y);
      break;
   }
   return 0;
}


/* end of file */



