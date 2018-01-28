#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_TextButton.h"


//**********************************************************************************
//**********************************************************************************
UIRectTextButton::UIRectTextButton(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid,boxToCopy, doname, xparent)
{
//	DWORD color[UIRECT_BCOL_MAX];

   text = NULL;
   font = 0;
   isMomentary = TRUE;
   isExclusive = FALSE;
	textOffsetX = textOffsetY = 0;

	color[UIRECT_BCOL_NORMAL   ] = D3DCOLOR_ARGB(255, 200, 200, 255);
	color[UIRECT_BCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
	color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 000, 000, 155);
	color[UIRECT_BCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);

}

//**********************************************************************************
UIRectTextButton::UIRectTextButton(int doid, int left, int top, int right, int bottom, 
                                   char *doname, UIRect *xparent)
	: UIRectWindow(doid,left, top, right, bottom, doname, xparent)
{
   text = NULL;
   font = 0;
   isMomentary = TRUE;
   isExclusive = FALSE;
	textOffsetX = textOffsetY = 0;

	color[UIRECT_BCOL_NORMAL   ] = D3DCOLOR_ARGB(255, 200, 200, 255);
	color[UIRECT_BCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
	color[UIRECT_BCOL_DEPRESSED] = D3DCOLOR_ARGB(255, 000, 000, 155);
	color[UIRECT_BCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);

}

//**********************************************************************************
UIRectTextButton::~UIRectTextButton()
{

   if (text)
      delete[] text;

}

//**********************************************************************************
void UIRectTextButton::SetText(char * newText)
{
   if (text)
      delete[] text;
   text = NULL;

   if (newText)
   {
      text = new char[strlen(newText)+1];
      assert(text);
      strcpy(text,newText);
   }

}

//**********************************************************************************
void UIRectTextButton::DrawSelf(void)
{
   // base class first
   UIRectWindow::DrawSelf();

	if (!text)
		return;

	// correction for Puma.
	RECT tempBox = innerBox;

   if (UIRECT_WINDOW_STYLE_ALL == fillStyle || 
       UIRECT_WINDOW_STYLE_ONLY_BORDER == fillStyle)
   {
      tempBox.top--;
      tempBox.bottom++;
   }

   tempBox.right--;
   tempBox.left++;
	// end correction

	tempBox.left += textOffsetX;
	tempBox.top  += textOffsetY;

   DWORD *colorToUse = &color[UIRECT_BCOL_NORMAL   ];
   if (isActive)
      colorToUse = &color[UIRECT_BCOL_ACTIVE   ];
   if (isDepressed && isActive && isMomentary)
      colorToUse = &color[UIRECT_BCOL_DEPRESSED   ];
   if (isDepressed && !isMomentary)
      colorToUse = &color[UIRECT_BCOL_DEPRESSED   ];
   if (isDisabled)
      colorToUse = &color[UIRECT_BCOL_DISABLED   ];

	CD3DFont *f = puma->GetDXFont(font);
   f->DrawText( tempBox, &(puma->clipRect), *colorToUse, text , textFlags);
		/*
   // adjust top of box so text appears in the center of the button
   int h = puma->fonts[font].dxfont->GetTextHeight() / 2;
   int center = (innerBox.bottom + innerBox.top) / 2;

   RECT textRect = innerBox;
   textRect.top  = center - h;
   if (textRect.top < innerBox.top)
      textRect.top = innerBox.top;

   RECT originalBox = puma->GetDrawRect();

   RECT diffBox = innerBox;
   if (!SubtractRects(diffBox, originalBox))
       return;
   puma->SetDrawRect(diffBox);

	CD3DFont *f = puma->GetDXFont(font);
   f->DrawText( textRect, &(puma->clipRect), *colorToUse, text );

   puma->SetDrawRect(originalBox);
  */
   return;
}

//**********************************************************************************
int UIRectTextButton::IsPointInMe(int x, int y)
{

   return UIRectWindow::IsPointInMe(x, y);
}

//**********************************************************************************
int UIRectTextButton::Action(int type, long x, short y)
{

   if (isDisabled)
       return 0;

   switch(type)
   {
   case UIRECT_MOUSE_LDOWN:
      // TextButtons don't have children, so just check if is in box
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
         if (process)
            return process(this, type, x, y);
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
      // TextButtons don't have children, so just check if is in box
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
         if (process)
            return process(this, type, x, y);
      }
      break;

   case UIRECT_MOUSE_MOVE:
      // TextButtons don't have children, so just check if is in box
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
		break;

   }
/*
   if (process)
   {
      return process(this, type, x, y);
   }
   */
   return 0;
}


/* end of file */



