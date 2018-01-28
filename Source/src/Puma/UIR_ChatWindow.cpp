#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_ChatWindow.h"

//**********************************************************************************
//**********************************************************************************
ChatWindow::ChatWindow(int doid, int left, int top, int right, int bottom, UIRect *xparent)
: UIRectWindow(doid, left, top, right, bottom, "CHAT_WINDOW", xparent)
{
	puma->LoadTexture("dat\\chatTopBar.png"     ,&art[CHAT_ART_TOP_BAR     ],0);
	puma->LoadTexture("dat\\chatRightCorner.png",&art[CHAT_ART_RIGHT_CORNER],0);
	puma->LoadTexture("dat\\chatBarLeft.png"    ,&art[CHAT_ART_BAR_LEFT    ],0);
	puma->LoadTexture("dat\\chatBarRight.png"   ,&art[CHAT_ART_BAR_RIGHT   ],0);
	puma->LoadTexture("dat\\chatSide.png"       ,&art[CHAT_ART_SIDE        ],0);
	puma->LoadTexture("dat\\chatEmbellish.png"  ,&art[CHAT_ART_EMBELLISH   ],0);
}

//**********************************************************************************
ChatWindow::~ChatWindow()
{
	for (int i = 0; i < CHAT_ART_MAX ; ++i)
		SAFE_RELEASE(art[i]);

}

//**********************************************************************************
void ChatWindow::DrawSelf(void)
{
//   int r,g,b;

   RECT originalBox = puma->clipRect;

//   RECT diffBox = innerBox;
   RECT diffBox = absBox;
   if (!SubtractRects(diffBox, originalBox))
       return;
//   puma->SetDrawRect(diffBox);

   originalBox = puma->clipRect; // do NOT use GetDrawRect(), you'll get the whole screen

   // base class first
//   UIRect::DrawSelf();

   // now me

	// draw the translucency background
	fillColor        = D3DCOLOR_ARGB(90, 0, 0, 100);
	puma->DrawRect(NULL,diffBox, fillColor);

	// draw the art pieces
	D3DSURFACE_DESC desc;

	art[CHAT_ART_SIDE]->GetLevelDesc(0, &desc);
	RECT dRect4 = absBox;
//	dRect4.right = dRect4.left + 9;
//	puma->DrawRect(art[CHAT_ART_SIDE],dRect4,0xffffffff);
//	dRect4 = absBox;
	dRect4.left = dRect4.right - 3;
	dRect4.right += 3;
	dRect4.top   += 6;
	puma->DrawRect(art[CHAT_ART_SIDE],dRect4,0xffffffff);

	art[CHAT_ART_BAR_LEFT]->GetLevelDesc(0, &desc);
	RECT dRect1 = {0, puma->ScreenH() - desc.Height, desc.Width-1, puma->ScreenH()};
	puma->DrawRect(art[CHAT_ART_BAR_LEFT],dRect1,0xffffffff);

	art[CHAT_ART_BAR_RIGHT]->GetLevelDesc(0, &desc);
	RECT dRect2 = {255, puma->ScreenH() - desc.Height, 255+desc.Width-1, puma->ScreenH()};
	puma->DrawRect(art[CHAT_ART_BAR_RIGHT],dRect2,0xffffffff);

	art[CHAT_ART_TOP_BAR]->GetLevelDesc(0, &desc);
	RECT dRect3 = absBox;
	dRect3.bottom = dRect3.top + 9;
	dRect3.right -= 8;
	puma->DrawRect(art[CHAT_ART_TOP_BAR],dRect3,0xffffffff);

	art[CHAT_ART_TOP_BAR]->GetLevelDesc(0, &desc);
	RECT dRect5 = absBox;
	dRect5.bottom = dRect5.top + 35;
	dRect5.top -= (64+16);
	dRect5.right = dRect5.left + 128;
	puma->DrawRect(art[CHAT_ART_EMBELLISH],dRect5,0xffffffff);

	art[CHAT_ART_RIGHT_CORNER]->GetLevelDesc(0, &desc);
	RECT dRect6 = absBox;
	dRect6.bottom = dRect6.top + 25;
	dRect6.top -= 0;
	dRect6.left = dRect6.right - 25;
	puma->DrawRect(art[CHAT_ART_RIGHT_CORNER],dRect6,0xffffffff);

  
   diffBox = absBox;
	diffBox.top += 8;
   if (!SubtractRects(diffBox, originalBox))
       return;
   puma->SetDrawRect(diffBox);

//   puma->SetDrawRect(originalBox);

   return;
}

//**********************************************************************************
int ChatWindow::IsPointInMe(int x, int y)
{

   return UIRect::IsPointInMe(x, y);
}

//**********************************************************************************
int ChatWindow::Action(int type, long x, short y)
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
void ChatWindow::Arrange(void)
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
//				if (curUIRect->absBox.top < innerBox.top)
//					curUIRect->absBox.top = innerBox.top;
            innerBox.bottom         -= curUIRect->box.top;
//				if (innerBox.bottom < innerBox.top)
//					 innerBox.bottom = innerBox.top;
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

	edLine->absBox.left   = 10;
	edLine->absBox.top    = puma->ScreenH() - 24;
	edLine->absBox.right  = 425;
	edLine->absBox.bottom = puma->ScreenH() - 4;
   edLine->Arrange();

   return;
}

/* end of file */



