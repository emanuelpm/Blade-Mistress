#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIR_BigInfoWindow.h"

//**********************************************************************************
//**********************************************************************************
BigInfoWindow::BigInfoWindow(int doid, int left, int top, int right, int bottom, UIRect *xparent)
: UIRectWindow(doid, left, top, right, bottom, "BIGINFO_WINDOW", xparent)
{
	puma->LoadTexture("dat\\biginfoCorner.png"  ,&art[BIGINFO_ART_CORNER  ],0);
	puma->LoadTexture("dat\\biginfoTop.png"     ,&art[BIGINFO_ART_BAR_TOP ],0);
	puma->LoadTexture("dat\\biginfoSide.png"    ,&art[BIGINFO_ART_BAR_LEFT],0);
}

//**********************************************************************************
BigInfoWindow::~BigInfoWindow()
{
	for (int i = 0; i < BIGINFO_ART_MAX ; ++i)
		SAFE_RELEASE(art[i]);

}

//**********************************************************************************
void BigInfoWindow::DrawSelf(void)
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
	fillColor        = D3DCOLOR_ARGB(255, 0, 0, 30);
	puma->DrawRect(NULL,diffBox, fillColor);

	// draw the art pieces
	D3DSURFACE_DESC desc;

	// sides
	art[BIGINFO_ART_BAR_LEFT]->GetLevelDesc(0, &desc);
	RECT dRect1 = absBox;
	dRect1.right = dRect1.left + desc.Width;
	puma->DrawRect(art[BIGINFO_ART_BAR_LEFT],dRect1,0xffffffff);

	dRect1 = absBox;
	dRect1.left = dRect1.right - desc.Width;
	puma->DrawRectUV(art[BIGINFO_ART_BAR_LEFT],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.99f, 0.0f, 0.0f, 
						  (dRect1.bottom - dRect1.top) / desc.Height);

	// top and bottom
	art[BIGINFO_ART_BAR_TOP]->GetLevelDesc(0, &desc);
	dRect1 = absBox;
	dRect1.bottom = dRect1.top + desc.Height/2;
	puma->DrawRectUV(art[BIGINFO_ART_BAR_TOP],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.0f, 0.0f,  
						  (dRect1.right - dRect1.left) / desc.Width, 0.7f);

//	puma->DrawRect(art[BIGINFO_ART_BAR_TOP],dRect1,0xffffffff);

	dRect1 = absBox;
	dRect1.top = dRect1.bottom - desc.Height;
	puma->DrawRectUV(art[BIGINFO_ART_BAR_TOP],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.0f, 0.99f,  
						  (dRect1.right - dRect1.left) / desc.Width, 0.0f);

	// corners
	art[BIGINFO_ART_CORNER]->GetLevelDesc(0, &desc);
	dRect1 = absBox;
	dRect1.bottom = dRect1.top + desc.Height;
	dRect1.right = dRect1.left + desc.Width;
	puma->DrawRect(art[BIGINFO_ART_CORNER],dRect1,0xffffffff);

	dRect1 = absBox;
	dRect1.top = dRect1.bottom - desc.Height;
	dRect1.right = dRect1.left + desc.Width;
	puma->DrawRectUV(art[BIGINFO_ART_CORNER],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.0f, 0.99f,  
						  (dRect1.right - dRect1.left) / desc.Width, 0.0f);

	dRect1 = absBox;
	dRect1.left = dRect1.right - desc.Width;
	dRect1.bottom = dRect1.top + desc.Height;
	puma->DrawRectUV(art[BIGINFO_ART_CORNER],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.99f, 0.0f, 0.0f, 
						  (dRect1.bottom - dRect1.top) / desc.Height);

	dRect1 = absBox;
	dRect1.left = dRect1.right - desc.Width;
	dRect1.top = dRect1.bottom - desc.Height;
	puma->DrawRectUV(art[BIGINFO_ART_CORNER],
		              dRect1.left,dRect1.top,dRect1.right,dRect1.bottom,
						  0xffffffff, FALSE, 0.99f, 0.99f, 0.0f, 0.0f);

   puma->SetDrawRect(diffBox);

   return;
}

//**********************************************************************************
int BigInfoWindow::IsPointInMe(int x, int y)
{

   return UIRect::IsPointInMe(x, y);
}

//**********************************************************************************
int BigInfoWindow::Action(int type, long x, short y)
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



/* end of file */



