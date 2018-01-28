#ifndef UIRECT_H
#define UIRECT_H

//***************************************************************

#include "../helper/linklist.h"
#include "puma.h"

enum
{
   UIRECT_MOUSE_LDOWN,
   UIRECT_MOUSE_RDOWN,
   UIRECT_MOUSE_LUP,
   UIRECT_MOUSE_RUP,
   UIRECT_MOUSE_MOVE,
   UIRECT_MOUSE_TEST_POS,
   UIRECT_KEY_DOWN,
   UIRECT_KEY_UP,
   UIRECT_KEY_CHAR,
   UIRECT_DONE,
   UIRECT_BACK_ARROW,
   UIRECT_FORWARD_ARROW,
   UIRECT_UP_ARROW,
   UIRECT_DOWN_ARROW,
   UIRECT_JUMP_BACK_ARROW,
   UIRECT_JUMP_FORWARD_ARROW,
   UIRECT_MOUSE_MAX
};

const unsigned long EXTRA_KEY_SHIFT_DOWN = 0x01;
const unsigned long EXTRA_KEY_CTRL_DOWN  = 0x02;

class UIRect;

typedef int (CALLBACK* UIRectProcess)(UIRect *, int, long, short);

//***************************************************************
class UIRect : public DataObject
{
public:

   // a doid of -1 means this items behavior will never want to be
   // noticed or used by the app that creates it.
   // the doname is used for the broad class of item this is, so the
   // base-class pointer can be cast properly.
   UIRect(int doid, RECT boxToCopy, char *doname = "RECT", UIRect *parent = NULL);
   UIRect(int doid, int left, int top, int right, int bottom, char *doname = "RECT", 
         UIRect *parent = NULL);
   virtual ~UIRect();

   virtual void AddChild(UIRect *child);

   virtual void Draw(void);
   virtual void DrawSelf(void);
   virtual void DrawChildren(void);
   virtual int IsPointInMe(int x, int y);

   virtual int Action(int type, long x, short y);
   int PassToChildren(int type, long x, short y);

   void ClearChildrensActiveFlag(void);
   virtual void Arrange(void);

   int SubtractRects(RECT &first, RECT second);

   UIRect *GetNextChild(UIRect *curChild, int wrap = TRUE);
   UIRect *GetPrevChild(UIRect *curChild, int wrap = TRUE);

	void SetDisableFlag(int flag);

   RECT box, innerBox, absBox;
   char isDepressed, isActive, isDisabled, ownsKeyboard;

   UIRect *parent;
   DoublyLinkedList childRectList;

   UIRectProcess process;

   void *generalUsePtr;
};


extern void UIRectStep(int x, int y);
extern void UIRectShift(int x, int y);
extern long IURectWindowServicer( UIRect *firstParent, HWND hWnd, UINT message, 
                          WPARAM wParam, LPARAM lParam );

#define UIRECTRET_MOVED_OFF -2
#define UIRECTRET_NO_CHANGE -1

extern UIRect *gPressedUIRect;
extern UIRect *gTouchedUIRect;


extern int uiAlpha;
extern unsigned long extraKeyFlags;



//extern int 

/*
//***************************************************************
class UIRectManager
{
public:


	UIRectManager(void);
	virtual ~UIRectManager();

//	int AddNewUIRect(UIRect *uirect);
	int MouseMove(int x, int y);
	int Draw(void);
	int KeyHit(int whichKey);
	void HandleCharInput( TCHAR chCharCode );
   int MakeActive(int id);

	DWORD normalTextColor    , depressedTextColor;
	DWORD mousedOverTextColor, disabledTextColor;

   UIRect *firstParent;

   ThomBM *art;

   int flashCounter;
};
*/

#endif
