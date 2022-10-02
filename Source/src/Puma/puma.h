//***************************************************************************************
// PUMA DirectX 8 wrapper class
//  incept data 1/1/02
//
// This class represents my (Thom Robertson's) needs for a way to rapid-prototype cool new
// games in the start of the 21st century.  Specifically, PUMA is geared towards quickly
// placing 3D objects and 2D UI elements on the screen.
//
// PUMA is especially focused on supporting the NEUTRON project, a multi-year effort to
// create the most amazing, complex, and fun space game ever seen.
//***************************************************************************************

#ifndef PUMA_H
#define PUMA_H


#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <dinput.h>
#include <mmsystem.h>

//#define STRICT
#include <math.h>
#include <D3DX8.h>
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DUtil.h"
#include "DXUtil.h"

#include "../helper/linklist.h"
#include "gamemode.h"
#include "../helper/tweakSystem.h"

#include "pumaVideoData.h"

#include <dsound.h>
#include "DSUtil.h"


//#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
//#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

const int NUM_OF_PUMA_FONTS = 20;

//***************************************************************************************
class PumaFont
{
public:

	CD3DFont *dxfont;
};

enum
{
	PUMA_INPUT_KEYDOWN,
	PUMA_INPUT_KEYUP,
	PUMA_INPUT_LMBDOWN,
	PUMA_INPUT_LMBUP,
	PUMA_INPUT_RMBDOWN,
	PUMA_INPUT_RMBUP,
	PUMA_INPUT_MOUSE_DELTA,
	PUMA_INPUT_MAX
};

//***************************************************************************************
class PumaInputEvent : public DataObject
{
public:
	PumaInputEvent(int doid, char *doname); // WhatAmI() = PUMA_INPUT_?

	int mouseX, mouseY;       // valid for mouse events
   D3DXVECTOR3 vPickRayDir;  // valid for mouse events
   D3DXVECTOR3 vPickRayOrig; // valid for mouse events

	char asciiValue;	// valid for keyboard events
	int scancode;		// valid for keyboard events

};

//***************************************************************************************
class SpacePoint
{
public:

	SpacePoint(void);
	
	D3DXMATRIX *GetMatrix(void);
	void MoveForwardRelative(float amount);
	void MoveRightRelative (float amount);
	void MoveUpRelative    (float amount);


	float angle, azimuth, roll;
	D3DXVECTOR3 location;
	D3DXMATRIX mat;
};


//***************************************************************************************
class PumaCamera
{
public:
	PumaCamera();

	void AssertView(LPDIRECT3DDEVICE8 pd3dDevice);
	void Calculate(void);
	void UpdateProjection(void);

	SpacePoint spacePoint;

	D3DXVECTOR3 m_vView;
	D3DXVECTOR3 m_vCross;

	D3DXMATRIX  m_matBillboard; // Special matrix for billboarding effects

	FLOAT       m_fFOV;         // Attributes for projection matrix
	FLOAT       m_fAspect;
	FLOAT       m_fNearPlane;
	FLOAT       m_fFarPlane;
	D3DXMATRIX  m_matProj;

};


//***************************************************************************************
class CPuma : public CD3DApplication
{
public:
//	Puma(int sW, int sH);
//	~Puma();

   HRESULT ConfirmDevice( D3DCAPS8*, DWORD, D3DFORMAT );

   HRESULT OneTimeSceneInit();
   HRESULT InitDeviceObjects();
   HRESULT RestoreDeviceObjects();
   HRESULT InvalidateDeviceObjects();
   HRESULT DeleteDeviceObjects();
   HRESULT Render();
   HRESULT FrameMove();
   HRESULT FinalCleanup();
   VOID    UpdateCamera();

   LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
   CPuma();

	bool GetStarted( HINSTANCE hInst, HWND hWnd, int isWin = TRUE);
	void ShutDown(void);

	void StartRenderingFrame(int r = 0, int g = 0, int b = 0);
	void FinishRenderingFrame(void);

	void HandleCamera(int debugText = TRUE);

	void CreateFont(int index, char *fontName, int size);
	CD3DFont *GetDXFont(int index);

	void PumpInput(void);
	PumaInputEvent *TakeNextInputEvent(void);

	void ClearMaterial(void);

	int  LoadTexture(char *fileName, LPDIRECT3DTEXTURE8 *tex, D3DCOLOR trans);

	void DrawMouse(LPDIRECT3DTEXTURE8 mouseArtPtr, int artOffsetX = 0, int artOffsetY = 0,
						int locX = -1, int locY = -1);
	void DrawRect(LPDIRECT3DTEXTURE8 artPtr, 
						int left, int top, int right, int bottom, 
						DWORD color, int hasTransparency = FALSE,
						int left2 = 0, int top2 = 0 );
	void DrawRect(LPDIRECT3DTEXTURE8 artPtr, 
		            RECT drawRect, DWORD color, int hasTransparency = FALSE);
	void DrawGradientRect(int left, int top, int right, int bottom, 
						  DWORD *color); // color is an array of 4 colors
	void DrawRectUV(LPDIRECT3DTEXTURE8 artPtr, 
						  int left, int top, int right, int bottom, 
						  DWORD color, int hasTransparency,
						  float u1, float v1, float u2, float v2);

   RECT GetDrawRect(void);
   void SetDrawRect(RECT r);
   void SetDrawRectToFullScreen();

	void UpdateScreenSizeInfo(void);
	int ScreenW(void);
	int ScreenH(void);


	void TakeScreenshot();


	LPDIRECTINPUT8				pDI      ;  // DInput   object       
	LPDIRECTINPUTDEVICE8		pKeyboard;  // keyboard device   
	LPDIRECTINPUTDEVICE8		pMouse   ;  // mouse    device   

//	D3DPRESENT_PARAMETERS d3dpp;

	VideoDataObject *squareVerts; // Buffer to hold 4 2D vertices
   D3DMATERIAL8 mouseMaterial;
   D3DMATERIAL8 emptyMaterial;

	PumaFont fonts[NUM_OF_PUMA_FONTS];

	PumaCamera *pumaCamera;
	int pumaInfoCounter;
	int flippedOut;

//	int screenW, screenH;
	int mouseX, mouseY;
	int isRendering;
	RECT clipRect;
	bool needScreenShot;

	DoublyLinkedList inputEventList;
   BYTE lastKeyboardState[256];   // DirectInput keyboard state buffer
   DIMOUSESTATE2 lastMouseState;      // DirectInput mouse state structure

   D3DXVECTOR3 vPickRayDir;  // valid for mouse events
   D3DXVECTOR3 vPickRayOrig; // valid for mouse events

};

extern CPuma *puma;

extern VideoDataManager *vdMan;

BOOL IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* dist);




#endif



