#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_TextBox.h"


//**********************************************************************************
//**********************************************************************************
UIRectTextBox::UIRectTextBox(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid,boxToCopy, doname, xparent)
{
   text = NULL;
   font = 0;
	textColor = D3DCOLOR_ARGB(255, 255, 255, 255);
}

//**********************************************************************************
UIRectTextBox::UIRectTextBox(int doid, int left, int top, int right, int bottom, 
                                   char *doname, UIRect *xparent)
	: UIRectWindow(doid,left, top, right, bottom, doname, xparent)
{
   text = NULL;
   font = 0;
	textColor = D3DCOLOR_ARGB(255, 255, 255, 255);
}

//**********************************************************************************
UIRectTextBox::~UIRectTextBox()
{

   if (text)
      delete[] text;

}

//**********************************************************************************
void UIRectTextBox::SetText(char * newText)
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
void UIRectTextBox::DrawSelf(void)
{
   // base class first
   UIRectWindow::DrawSelf();

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

   DWORD *colorToUse = &textColor;

	CD3DFont *f = puma->GetDXFont(font);
   f->DrawText( tempBox, &(puma->clipRect), *colorToUse, text, textFlags);
//   puma->TextDraw(font, text, innerBox, NULL, DT_CENTER | DT_WORDBREAK);

   return;
}

/* end of file */



