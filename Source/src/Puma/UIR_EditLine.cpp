#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_EditLine.h"


float UIRectEditLine::blinkTimer = 0;

//**********************************************************************************
//**********************************************************************************
UIRectEditLine::UIRectEditLine(int doid, RECT boxToCopy, int lineLen, char *doname, 
       UIRect *xparent)
	: UIRectWindow(doid,boxToCopy, doname, xparent)
{
//	DWORD color[UIRECT_ELCOL_MAX];

   lineLength = lineLen;
   text = new char[lineLength+1];

	for (int i = 0; i < EDITLINE_HISTORY_SIZE; ++i)
	   historyText[i] = new char[lineLength+1];
	historyIndex = -1;

   font = 0;
	hasTypingFocus = FALSE;
	suppressEnter  = FALSE;
	isSecure = FALSE;
	cursorPos = 0;
	offsetPos = 0;
	numbersOnly = restricted = FALSE;
	least = 0;
	most  = 10000;

	color[UIRECT_ELCOL_NONACTIVE] = D3DCOLOR_ARGB(255, 200, 200, 255);
	color[UIRECT_ELCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
	color[UIRECT_ELCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);
}

//**********************************************************************************
UIRectEditLine::UIRectEditLine(int doid, int left, int top, int right, int bottom, 
                                   int lineLen, char *doname, UIRect *xparent)
	: UIRectWindow(doid,left, top, right, bottom, doname, xparent)
{
   lineLength = lineLen;
   text = new char[lineLength+1];

	for (int i = 0; i < EDITLINE_HISTORY_SIZE; ++i)
	   historyText[i] = new char[lineLength+1];
	historyIndex = -1;

   font = 0;
	hasTypingFocus = FALSE;
	suppressEnter  = FALSE;
	isSecure = FALSE;
	cursorPos = 0;
	offsetPos = 0;
	numbersOnly = restricted = FALSE;
	least = 0;
	most  = 10000;

	color[UIRECT_ELCOL_NONACTIVE] = D3DCOLOR_ARGB(255, 200, 200, 255);
	color[UIRECT_ELCOL_ACTIVE   ] = D3DCOLOR_ARGB(255, 100, 100, 255);
	color[UIRECT_ELCOL_DISABLED ] = D3DCOLOR_ARGB(255, 100, 100, 100);
}

//**********************************************************************************
UIRectEditLine::~UIRectEditLine()
{

	for (int i = 0; i < EDITLINE_HISTORY_SIZE; ++i)
	   delete[] historyText[i];

   if (text)
      delete[] text;

}

//**********************************************************************************
void UIRectEditLine::SetText(char * newText)
{
   strncpy(text,newText,lineLength);
   text[lineLength] = '\0';
	cursorPos = strlen(text);
	offsetPos = 0;
	historyIndex = -1;
}

//**********************************************************************************
void UIRectEditLine::DrawSelf(void)
{
   // base class first
   UIRectWindow::DrawSelf();

   DWORD *colorToUse = &color[UIRECT_ELCOL_NONACTIVE   ];
   if (hasTypingFocus)
      colorToUse = &color[UIRECT_ELCOL_ACTIVE   ];
   if (isDisabled)
      colorToUse = &color[UIRECT_ELCOL_DISABLED   ];

   // adjust top of box so text appears in the center of the button
//   int h = puma->fonts[font].dxfont->GetTextHeight() / 2;
//   int center = (innerBox.bottom + innerBox.top) / 2;

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

   RECT textRect = innerBox;
//   textRect.top  = center - h;
//   if (textRect.top < innerBox.top)
//      textRect.top = innerBox.top;
	CD3DFont *f = puma->GetDXFont(font);

	// ********  draw part before cursor
   char *localBuffer = new char[strlen(text) + 5];
	if (!isSecure)
		memcpy(localBuffer,text,cursorPos);
	else
		memset(localBuffer,'*',cursorPos);
	localBuffer[cursorPos] = 0;
	while (cursorPos < offsetPos)
		--offsetPos;

	SIZE textSize;
   HRESULT result = f->GetTextExtent( &localBuffer[offsetPos], &textSize );
	while (textSize.cx > (tempBox.right - tempBox.left - 4))
	{
		++offsetPos;
	   result = f->GetTextExtent( &localBuffer[offsetPos], &textSize );
	}

   if (hasTypingFocus)
   {
      if ((((long)blinkTimer/10)&1))
      {
			int i = strlen(localBuffer);
			localBuffer[i] = '_';
			localBuffer[i+1] = 0;
      }
   }

   f->DrawText( tempBox, &(puma->clipRect), *colorToUse, &localBuffer[offsetPos] );

//   delete[] localBuffer;

	tempBox.left += textSize.cx;

	// ********  draw part before cursor
//   char *localBuffer = new char[strlen(text) + 5];
	if (!isSecure)
		memcpy(localBuffer,&text[cursorPos],strlen(&text[cursorPos]));
	else
		memset(localBuffer,'*',strlen(&text[cursorPos]));
	localBuffer[strlen(&text[cursorPos])] = 0;

   f->DrawText( tempBox, &(puma->clipRect), *colorToUse, localBuffer );

   delete[] localBuffer;

   return;
}

//**********************************************************************************
int UIRectEditLine::Action(int type, long x, short y)
{

   if (isDisabled)
       return 0;

   switch(type)
   {
   case UIRECT_MOUSE_LDOWN:
      if (IsPointInMe(x,y))
      {
         UIRect *par = this;
         int done = FALSE;
         while (!done)
         {
            if (par->parent)
               par = par->parent;
            else
               done = TRUE;
         }

         RecursiveDeactivateEditLines(par);

         hasTypingFocus = TRUE;

         isActive = TRUE;
      }

      break;

   case UIRECT_KEY_CHAR:
      if (hasTypingFocus && y > 0)
      {
         TCHAR chCharCode = y;
         if (8 == chCharCode)  // handling backspace!
         {
				if (cursorPos > 0)
				{
					if (cursorPos == strlen(text))
					{
			         text[cursorPos-1] = 0;
					}
					else
					{
						memmove( &text[cursorPos-1], &text[cursorPos], strlen(&text[cursorPos]));
			         text[strlen(text)-1] = 0;
					}
					--cursorPos;
				}
            return 1;
         }

         if (9 == chCharCode)  // handling tab!
         {
				SwitchToNextEditLine();
            return 1;
         }

         if (13 == chCharCode)  // handling return!
         {
				// push the string onto the stack of old strings
				for (int i = EDITLINE_HISTORY_SIZE-2; i >= 0; --i)
				   strncpy(historyText[i+1],historyText[i],lineLength-1);
			   strncpy(historyText[0],text,lineLength-1);

            if (process)
               return process(this, UIRECT_DONE, (long) text, lineLength);

				historyIndex = -1;
            return 1;
         }

         if (strlen(text) >= lineLength)
            return 1;

			if (numbersOnly)
			{
				if (!isdigit(chCharCode))
					return 1;
			}

			if (restricted)
			{
				if (!isalnum(chCharCode))
					return 1;
			}
		
			if( chCharCode < 32 )		// do not take non-displayable characters
				return 1;


         char *tempString = new char[lineLength+1];
			memcpy(tempString,text,cursorPos);
			tempString[cursorPos] = chCharCode;
			memcpy(&tempString[cursorPos + 1],&text[cursorPos],strlen(text) - cursorPos);
			tempString[strlen(text)+1] = 0;
//         sprintf(tempString, "%s%c", text, chCharCode);
			if (numbersOnly)
			{
				if (atoi(tempString) < least ||
				    atoi(tempString) > most)
				{
					delete[] tempString;
					return 1;
				}
			}
         sprintf(text, "%s", tempString);

         delete[] tempString;
			++cursorPos;

//			char ttxt[1024];
//			sprintf(ttxt,"%s + %d(%c)", text, chCharCode, chCharCode);
//			DEBUG_MSG(ttxt);


         return 1;
      }
      break;

   case UIRECT_KEY_UP:
      if (hasTypingFocus && 46 == y) // && del key
      {
//			if (cursorPos > 0)
			{
				if (cursorPos == strlen(text))
				{
			//      text[cursorPos-1] = 0;
				}
				else
				{
					memmove( &text[cursorPos], &text[cursorPos+1], strlen(&text[cursorPos+1]));
			      text[strlen(text)-1] = 0;
				}
//				--cursorPos;
			}
         return 1;
		}
		break;

	case UIRECT_UP_ARROW:
		if (historyIndex < EDITLINE_HISTORY_SIZE-1)
		{
			++historyIndex;
		   strncpy(text,historyText[historyIndex],lineLength-1);
			cursorPos = 0;
			offsetPos = 0;
		}
      return 1;
		break;

	case UIRECT_DOWN_ARROW:
		if (historyIndex > 0)
		{
			--historyIndex;
		   strncpy(text,historyText[historyIndex],lineLength-1);
			cursorPos = 0;
			offsetPos = 0;
		}
		else if (0 == historyIndex)
		{
			--historyIndex;
		   text[0] = 0;
			cursorPos = 0;
			offsetPos = 0;
		}
      return 1;
		break;

	case UIRECT_BACK_ARROW:
		if (cursorPos > 0)
		{
			--cursorPos;
		}
      return 1;
		break;

	case UIRECT_FORWARD_ARROW:
		if (cursorPos < strlen(text))
		{
			++cursorPos;
		}
      return 1;
		break;

	case UIRECT_JUMP_BACK_ARROW:
		if (cursorPos > 0)
		{
			--cursorPos;
			while (cursorPos > 0 && text[cursorPos-1] != ' ')
				--cursorPos;
		}
      return 1;
		break;

	case UIRECT_JUMP_FORWARD_ARROW:
		if (cursorPos < strlen(text))
		{
			++cursorPos;
			while (cursorPos < strlen(text) && text[cursorPos-1] != ' ')
				++cursorPos;
		}
      return 1;
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

   return 0;
}

//**********************************************************************************
void UIRectEditLine::RecursiveDeactivateEditLines(UIRect *curRect)
{
   if (!strcmp(curRect->WhoAmI(),"EDITLINE"))
      ((UIRectEditLine *)curRect)->hasTypingFocus = FALSE;

   UIRect *curUIRect = (UIRect *) curRect->childRectList.Last();
   while (curUIRect)
   {
      RecursiveDeactivateEditLines(curUIRect);
      curUIRect = (UIRect *) curRect->childRectList.Prev();
   }

}

//**********************************************************************************
void UIRectEditLine::SwitchToNextEditLine(void)
{
//   if (!strcmp(curRect->WhoAmI(),"EDITLINE"))
//      ((UIRectEditLine *)curRect)->hasTypingFocus = FALSE;

   UIRect *curUIRect = (UIRect *) parent->childRectList.Find(this);
   curUIRect = (UIRect *) parent->childRectList.Next();

	while (curUIRect && strcmp(curUIRect->WhoAmI(),"EDITLINE"))
	   curUIRect = (UIRect *) parent->childRectList.Next();

	if (!curUIRect)
	{
	   curUIRect = (UIRect *) parent->childRectList.First();
		while (curUIRect && strcmp(curUIRect->WhoAmI(),"EDITLINE"))
		   curUIRect = (UIRect *) parent->childRectList.Next();
	}

	UIRectEditLine	*ed = (UIRectEditLine *) curUIRect;

   ed->RecursiveDeactivateEditLines(parent);
   ed->hasTypingFocus = TRUE;
   ed->isActive = TRUE;
}

/* end of file */



