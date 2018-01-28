#ifndef UIRECT_CHATWINDOW_H
#define UIRECT_CHATWINDOW_H

//***************************************************************

#include "UIR_Window.h"
#include "UIR_EditLine.h"

enum
{
	CHAT_ART_TOP_BAR,
	CHAT_ART_RIGHT_CORNER,
	CHAT_ART_BAR_LEFT,
	CHAT_ART_BAR_RIGHT,
	CHAT_ART_SIDE,
	CHAT_ART_EMBELLISH,
	CHAT_ART_MAX
};

//***************************************************************
class ChatWindow : public UIRectWindow
{
public:

   ChatWindow(int doid, int left, int top, int right, int bottom, UIRect *parent = NULL);
   virtual ~ChatWindow();

   virtual void DrawSelf(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);
   virtual void Arrange(void);

   LPDIRECT3DTEXTURE8 art[CHAT_ART_MAX];

	UIRectEditLine *edLine;
};


#endif
