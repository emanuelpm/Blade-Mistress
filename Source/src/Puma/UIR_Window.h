#ifndef UIRECT_WINDOW_H
#define UIRECT_WINDOW_H

//***************************************************************

#include "UIRect.h"


enum
{
   UIRECT_WINDOW_STYLE_NOTHING,
   UIRECT_WINDOW_STYLE_ONLY_BORDER,
   UIRECT_WINDOW_STYLE_ONLY_FILLED,
   UIRECT_WINDOW_STYLE_ALL,
   UIRECT_WINDOW_STYLE_MAX
};

//***************************************************************
class UIRectWindow : public UIRect
{
public:

   UIRectWindow(int doid, RECT boxToCopy, char *doname = "WINDOW", UIRect *parent = NULL);
   UIRectWindow(int doid, int left, int top, int right, int bottom, 
         char *doname = "WINDOW", UIRect *parent = NULL);
   virtual ~UIRectWindow();

   virtual void DrawSelf(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);
   virtual void Arrange(void);

   DWORD topLeftColor, bottomRightColor, fillColor;
   LPDIRECT3DTEXTURE8 fillArt;
   char fillStyle, drawBorder;

	DWORD textFlags;

	static DWORD topLeftColorGlobal, bottomRightColorGlobal, fillColorGlobal;

};


#endif
