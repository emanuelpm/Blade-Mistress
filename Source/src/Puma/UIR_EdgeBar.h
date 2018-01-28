#ifndef UIRECT_EDGE_BAR_H
#define UIRECT_EDGE_BAR_H

//***************************************************************

#include "UIR_Window.h"


enum
{
   UIRECT_EBCOL_NORMAL,
   UIRECT_EBCOL_ACTIVE,
   UIRECT_EBCOL_DEPRESSED,
   UIRECT_EBCOL_DISABLED,
   UIRECT_EBCOL_MAX
};

//***************************************************************
class UIRectEdgeBar : public UIRectWindow
{
public:

	UIRectEdgeBar(int doid, int left, int top, int right, int bottom, UIRect *parent);
	virtual ~UIRectEdgeBar();

	virtual void DrawSelf(void);
	virtual int IsPointInMe(int x, int y);

	virtual int Action(int type, long x, short y);

	DWORD color[UIRECT_EBCOL_MAX];
   char isMomentary, isExclusive, barType;

};


#endif
