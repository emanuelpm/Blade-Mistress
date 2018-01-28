#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

//#include "thomdebug.h"
#include "UIRect.h"

RECT gUIRectLastBox = {0,0  ,100,100};

UIRect *gPressedUIRect = NULL;

//**********************************************************************************
//**********************************************************************************
UIRect::UIRect(int doid, RECT boxToCopy, char *doname, 
       UIRect *xparent)
	: DataObject(doid,doname)
{
   parent= xparent;
   if (-2 == boxToCopy.left)
      boxToCopy.left = gUIRectLastBox.left;
   if (-2 == boxToCopy.right)
      boxToCopy.right = gUIRectLastBox.right;
   if (-2 == boxToCopy.top)
      boxToCopy.top = gUIRectLastBox.top;
   if (-2 == boxToCopy.bottom)
      boxToCopy.bottom = gUIRectLastBox.bottom;

   gUIRectLastBox = innerBox = absBox = box = boxToCopy;

   isDepressed = isDisabled = FALSE;
   isActive = TRUE;

   process = NULL;
   generalUsePtr = NULL;
}

//**********************************************************************************
UIRect::UIRect(int doid, int left, int top, int right, int bottom, char *doname, 
       UIRect *xparent)
	: DataObject(doid,doname)
{
   parent= xparent;
   if (-2 == left)
      left = gUIRectLastBox.left;
   if (-2 == right)
      right = gUIRectLastBox.right;
   if (-2 == top)
      top = gUIRectLastBox.top;
   if (-2 == bottom)
      bottom = gUIRectLastBox.bottom;

   box.top    = top;
   box.left   = left;
   box.right  = right;
   box.bottom = bottom;

   gUIRectLastBox = innerBox = absBox = box;

   isDepressed = isActive = isDisabled = FALSE;
   isActive = TRUE;


   process = NULL;
   generalUsePtr = NULL;
}

//**********************************************************************************
UIRect::~UIRect()
{

}

//**********************************************************************************
void UIRect::Draw(void)
{
    DrawSelf();
    DrawChildren();
}

//**********************************************************************************
void UIRect::DrawSelf(void)
{
   // this, most base of classes, doesn't draw at all.
}

//**********************************************************************************
void UIRect::DrawChildren(void)
{

//   puma->SetDrawRect(innerBox);
   UIRect *curUIRect = (UIRect *) childRectList.Last();
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

      if (numGlueSides > 2)
         puma->SetDrawRect(absBox);
      else
         puma->SetDrawRect(innerBox);

      curUIRect->Draw();

      curUIRect = (UIRect *) childRectList.Prev();
   }

   puma->SetDrawRectToFullScreen();

}

//**********************************************************************************
void UIRect::AddChild(UIRect *child)
{
   if (childRectList.IsListEmpty())
      child->isActive = TRUE;
   else
      child->isActive = FALSE;
   childRectList.Append(child);
   child->parent = this;
}

//**********************************************************************************
void UIRect::Arrange(void)
{
   // reset my innerBox (my children may constrict it)
   innerBox = absBox;

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
         }
         if (-1 != curUIRect->box.right)
         {
            curUIRect->absBox.right  = innerBox.left + curUIRect->box.right;
            innerBox.left           += curUIRect->box.right;
         }
         if (-1 != curUIRect->box.top)
         {
            curUIRect->absBox.top    = innerBox.bottom - curUIRect->box.top;
            innerBox.bottom         -= curUIRect->box.top;
         }
         if (-1 != curUIRect->box.bottom)
         {
            curUIRect->absBox.bottom = innerBox.top + curUIRect->box.bottom;
            innerBox.top            += curUIRect->box.bottom;
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

   return;
}

//**********************************************************************************
int UIRect::IsPointInMe(int x, int y)
{
   if (x >= absBox.left && x <= absBox.right && y >= absBox.top && y <= absBox.bottom)
      return TRUE;
   return FALSE;
}

//**********************************************************************************
// returns 0 if the message was not handled by me
int UIRect::Action(int type, long x, short y)
{
   UIRect *other;

   if (isDisabled)
      return 0;

   switch(type)
   {
   case UIRECT_MOUSE_LDOWN:
   case UIRECT_MOUSE_RDOWN:
      if (!IsPointInMe(x,y))
         return 0;

      gPressedUIRect = this;

      if (PassToChildren(type, x, y))
         return 1;

      if (process)
         return process(this, type, x, y);

      break;

   case UIRECT_MOUSE_LUP  :
   case UIRECT_MOUSE_RUP  :
      gPressedUIRect = NULL;

      if (!IsPointInMe(x,y) && !isDepressed)
         return 0;

      if (PassToChildren(type, x, y))
         return 1;

      if (process)
         return process(this, type, x, y);

      break;

   case UIRECT_MOUSE_MOVE :

//       ClearChildrensActiveFlag
      if (!IsPointInMe(x,y) && isActive)
      {
         isActive = FALSE;
         // remove any peer's active flags
      }
/*
      if ((!IsPointInMe(x,y)) && isDepressed)
      {
         isDepressed = FALSE;
         // remove any peer's active flags
      }
*/
      if (IsPointInMe(x,y))
      {
         // remove any peer's active flags
         if (!isActive && parent)
            parent->ClearChildrensActiveFlag();
         isActive = TRUE;
      }

//      if (!IsPointInMe(x,y))
//          return 0;

      if (PassToChildren(type, x, y))
         return 1;

      if (process)
         return process(this, type, x, y);

      break;

	case UIRECT_KEY_UP		:
		if (!isActive)
			return 0;

		if (PassToChildren(type, x, y))
			return 1;

		if (process)
			return process(this, type, x, y);


	case UIRECT_KEY_CHAR	:
		if (PassToChildren(type, x, y))
			return 1;

		if (process)
			return process(this, type, x, y);


   case UIRECT_KEY_DOWN   :
      if (!isActive)
         return 0;

      if (38 == y) // up arrow
      {
         if (parent)
         {
            other = parent->GetPrevChild(this);
            isActive = FALSE;
            other->isActive = TRUE;
            return 1;
         }
      }

      if (40 == y) // down arrow
      {
         if (parent)
         {
            other = parent->GetNextChild(this);
            isActive = FALSE;
            other->isActive = TRUE;
            return 1;
         }
      }

      if (13 == y || 32 == y) // ENTER or SPACE
      {
         if (PassToChildren(type, x, y))
            return 1;

         Action(UIRECT_MOUSE_LDOWN, innerBox.left, innerBox.top);
         Action(UIRECT_MOUSE_LUP, innerBox.left, innerBox.top);
         return 1;
      }

      if (PassToChildren(type, x, y))
         return 1;

      if (process)
         return process(this, type, x, y);


       break;

   }


   return 0;

}

//**********************************************************************************
int UIRect::PassToChildren(int type, long x, short y)
{

   int result;
   UIRect *curUIRect = (UIRect *) childRectList.First();
   while (curUIRect)
   {
      result = curUIRect->Action(type,x,y);
      if (result != 0)
         return result;

      childRectList.Find(curUIRect);
      curUIRect = (UIRect *) childRectList.Next();
   }


   return 0;
}


//**********************************************************************************
void UIRect::ClearChildrensActiveFlag(void)
{
   UIRect *curUIRect = (UIRect *) childRectList.First();
   while (curUIRect)
   {
      curUIRect->isActive =FALSE;
//      curUIRect->ClearChildrensActiveFlag();
      curUIRect = (UIRect *) childRectList.Next();
   }
}


//**********************************************************************************
UIRect *UIRect::GetNextChild(UIRect *curChild, int wrap)
{
   if (!curChild)
      return (UIRect *) childRectList.First();

   UIRect *curUIRect = (UIRect *) childRectList.Find(curChild);

   if (!curUIRect)
      return (UIRect *) childRectList.First();

   curUIRect = (UIRect *) childRectList.Next();

   if (curUIRect || !wrap)
      return (UIRect *) curUIRect;

   return (UIRect *) childRectList.First();
}


//**********************************************************************************
UIRect *UIRect::GetPrevChild(UIRect *curChild, int wrap)
{
   if (!curChild)
      return (UIRect *) childRectList.Last();

   UIRect *curUIRect = (UIRect *) childRectList.Find(curChild);

   if (!curUIRect)
      return (UIRect *) childRectList.Last();

   curUIRect = (UIRect *) childRectList.Prev();

   if (curUIRect || !wrap)
      return (UIRect *) curUIRect;

   return (UIRect *) childRectList.Last();
}


//**********************************************************************************
int UIRect::SubtractRects(RECT &first, RECT second)
{

   if (first.left < second.left)
      first.left = second.left;
   if (first.right > second.right)
      first.right = second.right;
   if (first.top < second.top)
      first.top = second.top;
   if (first.bottom > second.bottom)
      first.bottom = second.bottom;

   if (first.left >= first.right || first.top >= first.bottom)
       return FALSE;

   return TRUE;
}

/*
   int spatial = FALSE;
   switch(type)
   {
   case UIRECT_MOUSE_LDOWN:
   case UIRECT_MOUSE_RDOWN:
   case UIRECT_MOUSE_LUP  :
   case UIRECT_MOUSE_RUP  :
   case UIRECT_MOUSE_MOVE :
      spatial = TRUE;
   }

   if (spatial && !IsPointInMe(x,y))
      return 0;

   int result;

   UIRect *curUIRect = (UIRect *) childRectList.First();
   while (curUIRect)
   {
      if (spatial)
      {
         result = curUIRect->Action(type,x,y);
         if (result != 0)
            return result;
      }
      else
      {
         if (curUIRect->isActive)
         {
            result = curUIRect->Action(type,x,y);
            if (result != 0)
               return result;
         }
      }

      curUIRect = (UIRect *) childRectList.Next();
   }

//   if (process)
//      return process(this, type, x, y);

   return -1;
}
*/

//**********************************************************************************
void UIRectShift(int x, int y)
{
   gUIRectLastBox.right += x;
   gUIRectLastBox.left  += x;
   gUIRectLastBox.bottom+= y;
   gUIRectLastBox.top   += y;
}


//**********************************************************************************
void UIRectStep(int x, int y)
{
   int w = gUIRectLastBox.right - gUIRectLastBox.left;
   int h = gUIRectLastBox.bottom - gUIRectLastBox.top;

   if (x > 0)
   {
      while(x)
      {
         gUIRectLastBox.right += w;
         gUIRectLastBox.left  += w;
         x--;
      }
   }
   else if (x < 0)
   {
      while(x)
      {
         gUIRectLastBox.right -= w;
         gUIRectLastBox.left  -= w;
         x++;
      }
   }

   if (y > 0)
   {
      while(y)
      {
         gUIRectLastBox.bottom += h;
         gUIRectLastBox.top    += h;
         y--;
      }
   }
   else if (y < 0)
   {
      while(y)
      {
         gUIRectLastBox.bottom -= h;
         gUIRectLastBox.top    -= h;
         y++;
      }
   }
}

//*******************************************************************************
long IURectWindowServicer( UIRect *firstParent, HWND hWnd, UINT message, 
                          WPARAM wParam, LPARAM lParam )
{

	int x,y;

   switch (message)
   {

    	case WM_KEYDOWN:
         firstParent->Action(UIRECT_KEY_DOWN, lParam, wParam);
        	break;

    	case WM_KEYUP:
         firstParent->Action(UIRECT_KEY_UP, lParam, wParam);
        	break;

		case WM_CHAR:
         firstParent->Action(UIRECT_KEY_CHAR, lParam, wParam);
         break;

		case WM_MOUSEMOVE:
			x = LOWORD(lParam);  // horizontal position of cursor  
			y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_MOVE, x,y);
			break;

      case WM_RBUTTONDOWN:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_RDOWN, x,y);
         break;

      case WM_LBUTTONDOWN:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_LDOWN, x,y);
         break;

      case WM_RBUTTONUP:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_RUP, x,y);
         break;

      case WM_LBUTTONUP:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_LUP, x,y);
    }

    return 0;
}


/* end of file */



