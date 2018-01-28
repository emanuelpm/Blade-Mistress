#ifndef UIRECT_BIGINFOWINDOW_H
#define UIRECT_BIGINFOWINDOW_H

//***************************************************************

#include "UIR_Window.h"
#include "UIR_EditLine.h"

enum
{
	BIGINFO_ART_CORNER,
	BIGINFO_ART_BAR_TOP,
	BIGINFO_ART_BAR_LEFT,
	BIGINFO_ART_MAX
};

//***************************************************************
class BigInfoWindow : public UIRectWindow
{
public:

   BigInfoWindow(int doid, int left, int top, int right, int bottom, UIRect *parent = NULL);
   virtual ~BigInfoWindow();

   virtual void DrawSelf(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);
//   virtual void Arrange(void);

   LPDIRECT3DTEXTURE8 art[BIGINFO_ART_MAX];

};


#endif
