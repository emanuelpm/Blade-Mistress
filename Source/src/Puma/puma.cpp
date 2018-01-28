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

#include "puma.h"

// A structure for 2D Verts
struct PUMA2DVERTEX
{
    FLOAT x, y, z, rhw; // The transformed position for the vertex
    DWORD color;        // The vertex color
    FLOAT       tu, tv;   // The texture coordinates
};

// Our custom FVF, which describes our PUMA 2D vertex structure
#define D3DFVF_PUMA2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)



CPuma *puma = NULL;
VideoDataManager *vdMan = NULL;


//***************************************************************************************
char ScanCodeToAscii(DWORD scancode)
//static int scan2ascii(DWORD scancode, unsigned short* result)
{
   static HKL layout=GetKeyboardLayout(0);
   static unsigned char State[256];

   if (GetKeyboardState(State)==FALSE)
      return 0;
   UINT vk=MapVirtualKeyEx(scancode,1,layout);
	unsigned short result;
	int numOfChars = ToAsciiEx(vk,scancode,State,&result,0,layout);

	return (char)result;
}




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
/*
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CPuma d3dApp;

    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}
*/



//-----------------------------------------------------------------------------
// Name: CPuma()
// Desc: Application constructor. Sets attributes for the app.
//-----------------------------------------------------------------------------
CPuma::CPuma() : CD3DApplication()
{
   m_strWindowTitle    = _T("CPuma application");
   m_bUseDepthBuffer   = TRUE;

	pumaInfoCounter = 30;
   ZeroMemory( &lastKeyboardState, sizeof(lastKeyboardState) );

	isRendering = FALSE;
	srand(timeGetTime());

	mouseX = mouseY = -100;       // invalid

	for (int i = 0; i < NUM_OF_PUMA_FONTS; i++)
		fonts[i].dxfont = NULL;

   pumaCamera= NULL;
   pMouse = NULL;
   pKeyboard = NULL;
   pDI = NULL;
   needScreenShot = false;
}


void CPuma::TakeScreenshot() {
	char dbm[80];
	LPDIRECT3DSURFACE8 BackBuffer = NULL;

	if( m_pd3dDevice->GetBackBuffer( 0, D3DBACKBUFFER_TYPE_MONO, &BackBuffer ) == D3D_OK ) {
		for( int v = 0; v < 999; v++ ) {
			sprintf( dbm, "bm_screenshot_%03d.bmp", v );
			
			WIN32_FIND_DATA FindData;
			
			//--Try to find the file
			HANDLE FileHandle = FindFirstFile( dbm, &FindData );
			
			//--If the file handle turned out to be INVALID_HANDLE_VALUE,
			//--then the screen shot does not exist
			bool Exists = ( FileHandle != INVALID_HANDLE_VALUE ? true : false );
			
			//--Close the file
			FindClose(FileHandle);
				
			if( !Exists ) 
				break;
		}

		D3DXSaveSurfaceToFile( dbm, D3DXIFF_BMP, BackBuffer, NULL, NULL );

		BackBuffer->Release();
	}
}


//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Called during initial app startup, this function performs all the
//       permanent initialization.
//-----------------------------------------------------------------------------
HRESULT CPuma::OneTimeSceneInit()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CPuma::FrameMove()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CPuma::Render()
{
    // Update the camera here rather than in FrameMove() so you can
    // move the camera even when the scene is paused
    UpdateCamera();

    if( FAILED( m_pd3dDevice->BeginScene() ) )
        return S_OK;

    // Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
                         0x00000000, 1.0f, 0L );
    
    // End the scene.
    m_pd3dDevice->EndScene();
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateCamera()
// Desc: 
//-----------------------------------------------------------------------------
VOID CPuma::UpdateCamera()
{
}


//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initialize scene objects.
//-----------------------------------------------------------------------------
HRESULT CPuma::InitDeviceObjects()
{
//    HRESULT hr;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CPuma::RestoreDeviceObjects()
{
//	HRESULT hr;


    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CPuma::InvalidateDeviceObjects()
{

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.
//-----------------------------------------------------------------------------
HRESULT CPuma::DeleteDeviceObjects()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CPuma::FinalCleanup()
{
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device intialization, this code checks the device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CPuma::ConfirmDevice( D3DCAPS8* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    // Make sure device can do ONE:ONE alphablending
    if( 0 == ( pCaps->SrcBlendCaps & D3DPBLENDCAPS_ONE ) )
        return E_FAIL;
    if( 0 == ( pCaps->DestBlendCaps & D3DPBLENDCAPS_ONE ) )
        return E_FAIL;

    // Make sure HW TnL devices can do point sprites
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->MaxPointSize <= 0.0f )
            return E_FAIL;
    }

    if( dwBehavior & D3DCREATE_PUREDEVICE )
        return E_FAIL; // GetTransform doesn't work on PUREDEVICE

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Message proc function to handle key and menu input
//-----------------------------------------------------------------------------
LRESULT CPuma::MsgProc( HWND m_hWnd, UINT uMsg, WPARAM wParam,
                                    LPARAM lParam )
{
	PumaInputEvent *pie;
	BOOL bActive;


   switch( uMsg )
   {
	   case WM_ACTIVATE:
			bActive = LOWORD(wParam);
			if (bActive != WA_INACTIVE)
			{
//				SetTimer( puma->m_hWnd, TIMER_ID, TIMER_RATE, NULL );
		  		flippedOut = FALSE;
			}
			else
			{
		  		flippedOut = TRUE;
//		  		KillTimer( puma->m_hWnd , TIMER_ID);
			}

       	break;
	   case WM_KEYDOWN:
			if( 123 == wParam )
				needScreenShot = true;
			else
			{
				pie = new PumaInputEvent(PUMA_INPUT_KEYDOWN,"KEY DOWN");
				pie->scancode = wParam;
				pie->asciiValue = 0;
				inputEventList.Append(pie);
			}
			break;

	   case WM_KEYUP:
			{
				pie = new PumaInputEvent(PUMA_INPUT_KEYUP,"KEY UP");
				pie->scancode = wParam;
				pie->asciiValue = 0;
				inputEventList.Append(pie);
			}
			break;

	   case WM_CHAR:
			{
				pie = new PumaInputEvent(PUMA_INPUT_KEYUP,"KEY UP");
				pie->scancode = 0;
				pie->asciiValue = wParam;
//				if (13 == wParam)
//					pie->asciiValue = wParam;
				inputEventList.Append(pie);
			}
			break;

		case WM_MOUSEMOVE:
			if (puma->m_bWindowed)
			{
				puma->mouseX = LOWORD(lParam);  // horizontal position of cursor  
				puma->mouseY = HIWORD(lParam);  // vertical position of cursor    
				if (puma->mouseX < 0)
					puma->mouseX = 0;
				if (puma->mouseX >= puma->ScreenW())
					puma->mouseX = puma->ScreenW()-1;
				
				if (puma->mouseY < 0)
					puma->mouseY = 0;
				if (puma->mouseY >= puma->ScreenH())
					puma->mouseY = puma->ScreenH()-1;
			}
			break;
   }

    return CD3DApplication::MsgProc( m_hWnd, uMsg, wParam, lParam );
}




















//***************************************************************************************
bool CPuma::GetStarted( HINSTANCE hInst, HWND wnd, int isWin)
{

	// Turn off culling
	m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	// Turn off D3D lighting
//	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	// Turn on the zbuffer
	m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	//****************************** end D3D setup


	//****************************** transforms setup

   // For our world matrix, we will just leave it as the identity
   D3DXMATRIX matWorld;
   D3DXMatrixIdentity( &matWorld );
   m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	pumaCamera = new PumaCamera();
	pumaCamera->Calculate();
	pumaCamera->AssertView(m_pd3dDevice);

	//****************************** end transforms setup

	//****************************** DInput setup

	int hr;

   // Create a DInput object

//   HMODULE hm = GetModuleHandle(NULL);
	
   if( FAILED( hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, 
                        IID_IDirectInput8, (VOID**)&pDI, NULL ) ) )
    return FALSE;

   // ******** Obtain an interface to the system keyboard device.
	/*
   if( FAILED( hr = pDI->CreateDevice( GUID_SysKeyboard, &pKeyboard, NULL ) ) )
     return FALSE;
   
   // Set the data format to "keyboard format" - a predefined data format 
   //
   // A data format specifies which controls on a device we
   // are interested in, and how they should be reported.
   //
   // This tells DirectInput that we will be passing an array
   // of 256 bytes to IDirectInputDevice::GetDeviceState.
   if( FAILED( hr = pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
     return FALSE;
   													  
   // Set the cooperativity level to let DirectInput know how
   // this device should interact with the system and with other
   // DirectInput applications.
   hr = pKeyboard->SetCooperativeLevel( m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE); // | DISCL_NOWINKEY );
   if( FAILED( hr ))
     return FALSE;

   // Acquire the newly created device
   pKeyboard->Acquire();
	*/
   // ******** Obtain an interface to the system mouse device.
   if( FAILED( hr = pDI->CreateDevice( GUID_SysMouse, &pMouse, NULL ) ) )
     return FALSE;
   
   // Set the data format to "mouse format" - a predefined data format 
   //
   // A data format specifies which controls on a device we
   // are interested in, and how they should be reported.
   //
   // This tells DirectInput that we will be passing a
   // DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
   if( FAILED( hr = pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
     return FALSE;
   
   // Set the cooperativity level to let DirectInput know how
   // this device should interact with the system and with other
   // DirectInput applications.
   hr = pMouse->SetCooperativeLevel( m_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
   if( FAILED(hr) )
     return FALSE;

   // Acquire the newly created device
   pMouse->Acquire();
	  
	//****************************** end DInput setup

	vdMan = new VideoDataManager();

	//****************************** setup mouse rectangle drawing data
   // Create the vertex buffer. Here we are allocating enough memory
   // (from the default pool) to hold all our 4 vertices. We also
   // specify the FVF, so the vertex buffer knows what data it contains.

	squareVerts = new VideoDataObject( 4*sizeof(PUMA2DVERTEX), D3DFVF_PUMA2DVERTEX);
	vdMan->AddObject(squareVerts);

   // Now we fill the vertex buffer. To do this, we need to Lock() the VB to
   // gain access to the vertices. This mechanism is required becuase vertex
   // buffers may be in device memory.
   PUMA2DVERTEX* pVertices = (PUMA2DVERTEX*) vdMan->LockVerts(squareVerts);

	for (int i = 0; i < 4; i++)
	{
	   pVertices[i].color = 0xffffffff;
	   pVertices[i].rhw   = 1.0f;
	   pVertices[i].z     = 0.5f;
	}

	pVertices[0].tu = 0;
	pVertices[0].tv = 0;
	pVertices[1].tu = 1;
	pVertices[1].tv = 0;
	pVertices[2].tu = 0;
	pVertices[2].tv = 1;
	pVertices[3].tu = 1;
	pVertices[3].tv = 1;

   vdMan->UnlockVerts(squareVerts);

   ZeroMemory( &mouseMaterial, sizeof(D3DMATERIAL8) );
   mouseMaterial.Diffuse.r = 0.5f;
   mouseMaterial.Diffuse.g = 0.5f;
   mouseMaterial.Diffuse.b = 0.5f;
   mouseMaterial.Diffuse.a = 0.5f;
   mouseMaterial.Ambient.r = 0.5f;
   mouseMaterial.Ambient.g = 0.5f;
   mouseMaterial.Ambient.b = 0.5f;
   mouseMaterial.Ambient.a = 0.5f;

	//****************************** end setup mouse rectangle drawing data



	SetCursor( NULL );
//   m_pd3dDevice->ShowCursor( FALSE );
	ShowCursor(FALSE);
	m_pd3dDevice->ShowCursor(FALSE);

    
//	RECT realSize = GetDrawRect();
//	ScreenW() = realSize.right;
//	ScreenH() = realSize.bottom;

	return TRUE;
}


//***************************************************************************************
void CPuma::ShutDown(void)
{

	vdMan->DeleteObject(squareVerts);

	delete vdMan;

	for (int i = 0; i < NUM_OF_PUMA_FONTS; i++)
	{
		SAFE_DELETE(fonts[i].dxfont);
	}
	
   if( pMouse ) 
    pMouse->Unacquire();
   if( pKeyboard ) 
    pKeyboard->Unacquire();

   SAFE_DELETE(pumaCamera);

   // Release any DirectInput objects.
   SAFE_RELEASE( pMouse );
   SAFE_RELEASE( pKeyboard );
   SAFE_RELEASE( pDI );

   Cleanup3DEnvironment();
}



//***************************************************************************************
void CPuma::StartRenderingFrame(int r, int g, int b)
{

	SetCursor( NULL );
	SetDrawRectToFullScreen();

   // Clear the backbuffer and the zbuffer
   m_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
               D3DCOLOR_XRGB(r,g,b), 1.0f, 0 );

   // Begin the scene
   m_pd3dDevice->BeginScene();
	isRendering = TRUE;
}


//***************************************************************************************
void CPuma::FinishRenderingFrame(void)
{
   	if( needScreenShot ) {
		TakeScreenshot();
		needScreenShot = false;
	}

	// End the scene
   m_pd3dDevice->EndScene();
	isRendering = FALSE;

   // Present the backbuffer contents to the display
   m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

//***************************************************************************************
void CPuma::SetDrawRect(RECT r)
{
	clipRect = r;
}


//***************************************************************************************
void CPuma::SetDrawRectToFullScreen(void)
{
	clipRect.left    = 0;
	clipRect.top     = 0;
	clipRect.right   = ScreenW();
	clipRect.bottom  = ScreenH();

}


//***************************************************************************************
RECT CPuma::GetDrawRect(void)
{

	D3DVIEWPORT8 vp;

	m_pd3dDevice->GetViewport(&vp);

	RECT ret;
	ret.left    = vp.X;
	ret.top     = vp.Y;
	ret.right   = vp.X + vp.Width;
	ret.bottom  = vp.Y + vp.Height;

	return ret;

}


//***************************************************************************************
void CPuma::CreateFont(int index, char *fontName, int size)
{
//	LOGFONT logfont;
//	sprintf(logfont.lfFaceName, fontName);
//	logfont.lfHeight = size;
	
   fonts[index].dxfont = new CD3DFont( fontName, size );
   fonts[index].dxfont->InitDeviceObjects( m_pd3dDevice );
   fonts[index].dxfont->RestoreDeviceObjects();
//	HRESULT hr = D3DXCreateFontIndirect(m_pd3dDevice, &logfont, &fonts[index].dxfont);
}

//***************************************************************************************
CD3DFont *CPuma::GetDXFont(int index)
{

	return fonts[index].dxfont;
}

//***************************************************************************************
void CPuma::HandleCamera(int debugText)
{

   HRESULT hr;
   TCHAR   strNewText[256*5 + 1] = TEXT("");
   TCHAR   strElement[10];   
   BYTE    diks[256];   // DirectInput keyboard state buffer 
   int     i;

	CD3DFont *font = puma->GetDXFont(0);
	RECT rect = {2,100,100,200};
	D3DCOLOR color = D3DCOLOR_RGBA(255,255,155,255);
/*
   if( NULL == pKeyboard ) 
      return;
   
   // Get the input's device state, and put the state in dims
   ZeroMemory( &diks, sizeof(diks) );
   
   hr = pKeyboard->GetDeviceState( sizeof(diks), &diks );
   if( FAILED(hr) ) 
   {
      // DirectInput may be telling us that the input stream has been
      // interrupted.  We aren't tracking any state between polls, so
      // we don't have any special reset that needs to be done.
      // We just re-acquire and try again.
      
      // If input is lost then acquire and keep trying 
      hr = pKeyboard->Acquire();
      while( hr == DIERR_INPUTLOST ) 
         hr = pKeyboard->Acquire();
	  
      // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
      // may occur when the app is minimized or in the process of 
      // switching, so just try again later 
      return; 
   }
   	 
   // Make a string of the index values of the keys that are down
   for( i = 0; i < 256; i++ ) 
   {
      if( diks[i] & 0x80 ) 
      {
         sprintf( strElement, "%d ", i );
         strcat( strNewText, strElement );
      }
   }

	if (font)
	{
	   font->DrawText( rect, NULL, color, strNewText );
	}

	D3DXVECTOR3 vec;
//	D3DXVECTOR3 side     = D3DXVECTOR3( 1,0,0 );
//	D3DXVECTOR3 front    = D3DXVECTOR3( 0,0,1 );
	D3DXVECTOR3 vertical = D3DXVECTOR3( 0,1,0 );


	if (diks[75] & 0x80 ) // if keypad 4, move left
	{
		pumaCamera->spacePoint.MoveRightRelative(1.0f);
		pumaInfoCounter = 30;
	}
	if (diks[77] & 0x80 ) // if keypad 6, move right
	{
		pumaCamera->spacePoint.MoveRightRelative(-1.0f);
		pumaInfoCounter = 30;
	}
	if (diks[72] & 0x80 ) // if keypad 8, move forward
	{
		pumaCamera->spacePoint.MoveForwardRelative(1.0f);
		pumaInfoCounter = 30;
	}
	if (diks[80] & 0x80 ) // if keypad 2, move backward
	{
		pumaCamera->spacePoint.MoveForwardRelative(-1.0f);
		pumaInfoCounter = 30;
	}
	if (diks[74] & 0x80 ) // if keypad -, move up
	{
		pumaCamera->spacePoint.location.y -= 1;
		pumaInfoCounter = 30;
	}
	if (diks[78] & 0x80 ) // if keypad +, move down
	{
		pumaCamera->spacePoint.location.y += 1;
		pumaInfoCounter = 30;
	}
	if (diks[71] & 0x80 ) // if keypad 7, rotate left
	{
		pumaCamera->spacePoint.angle -= D3DX_PI/30;
		pumaInfoCounter = 30;
	}
	if (diks[73] & 0x80 ) // if keypad 9, rotate right
	{
		pumaCamera->spacePoint.angle += D3DX_PI/30;
		pumaInfoCounter = 30;
	}
	if (diks[81] & 0x80 ) // if keypad 3, rotate down
	{
		pumaCamera->spacePoint.azimuth += D3DX_PI/40;
		pumaInfoCounter = 30;
	}
	if (diks[83] & 0x80 ) // if keypad ., rotate up
	{
		pumaCamera->spacePoint.azimuth -= D3DX_PI/40;
		pumaInfoCounter = 30;
	}
*/


	pumaCamera->Calculate();
	pumaCamera->AssertView(m_pd3dDevice);

	if (pumaInfoCounter > 0 && font && debugText)
	{
		pumaInfoCounter--;
		sprintf(strNewText,"Cam X %f Y %f Z %f", 
			        pumaCamera->spacePoint.location.x, 
					  pumaCamera->spacePoint.location.y, 
					  pumaCamera->spacePoint.location.z);
		RECT rect2 = {2,428,500,448};
	   font->DrawText( rect2, NULL, color, strNewText );
		sprintf(strNewText,"Cam Angle %f Azimuth %f", 
			        pumaCamera->spacePoint.angle, pumaCamera->spacePoint.azimuth);
		RECT rect3 = {2,410,500,440};
	   font->DrawText( rect3, NULL, color, strNewText );
	}


}

//***************************************************************************************
void CPuma::PumpInput(void)
{
   HRESULT hr;
   BYTE    diks[256];   // DirectInput keyboard state buffer 
   int     i;
	PumaInputEvent *pie;
/*
	// ******* check keyboard for state changes
   ZeroMemory( &diks, sizeof(diks) );
   
   hr = pKeyboard->GetDeviceState( sizeof(diks), &diks );
   if( FAILED(hr) ) 
   {
      // If input is lost then acquire and keep trying 
      hr = pKeyboard->Acquire();
      while( hr == DIERR_INPUTLOST ) 
         hr = pKeyboard->Acquire();
   }

   for( i = 0; i < 256; i++ ) 
   {
      if( diks[i] & 0x80 && !(lastKeyboardState[i] & 0x80)) 
      {
			pie = new PumaInputEvent(PUMA_INPUT_KEYDOWN,"KEY DOWN");
			pie->scancode = i;
			pie->asciiValue = ScanCodeToAscii(i);
			inputEventList.Append(pie);
		}
	}

   for( i = 0; i < 256; i++ ) 
   {
      if( !(diks[i] & 0x80) && (lastKeyboardState[i] & 0x80)) 
      {
			pie = new PumaInputEvent(PUMA_INPUT_KEYUP,"KEY UP");
			pie->scancode = i;
			pie->asciiValue = ScanCodeToAscii(i);
			inputEventList.Append(pie);
		}
	}

	// finally, fill the lastKeyboardState buffer with the current state, for next time.
   for( i = 0; i < 256; i++ ) 
		lastKeyboardState[i] = diks[i];
*/
	// ******* check mouse for state changes
   DIMOUSESTATE2 dims2;      // DirectInput mouse state structure

   // Get the input's device state, and put the state in dims
   ZeroMemory( &dims2, sizeof(dims2) );
   hr = pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
   if( FAILED(hr) ) 
   {
      hr = pMouse->Acquire();
      while( hr == DIERR_INPUTLOST ) 
         hr = pMouse->Acquire();
   }

	pie = new PumaInputEvent(PUMA_INPUT_MOUSE_DELTA,"MOUSE_DELTA");
	pie->mouseX = dims2.lX;
	pie->mouseY = dims2.lY;
	inputEventList.Append(pie);

	if (!m_bWindowed)
	{
		if (ScreenW() > 640)
		{
			dims2.lX = dims2.lX * (float) 1.2f;
			dims2.lY = dims2.lY * (float) 1.2f;
		}
		if (ScreenW() > 800)
		{
			dims2.lX = dims2.lX * (float) 1.2f;
			dims2.lY = dims2.lY * (float) 1.2f;
		}


		mouseX += dims2.lX;
		if (mouseX < 0)
			mouseX = 0;
		if (mouseX >= ScreenW())
			mouseX = ScreenW()-1;
		
		mouseY += dims2.lY;
		if (mouseY < 0)
			mouseY = 0;
		if (mouseY >= ScreenH())
			mouseY = ScreenH()-1;
	}
   // Compute the vector of the pick ray in screen space
   D3DXMATRIX matProj;
   hr = m_pd3dDevice->GetTransform( D3DTS_PROJECTION, &matProj );

   D3DXVECTOR3 v;
   v.x =  ( ( ( 2.0f * mouseX ) / ScreenW() ) - 1 ) / matProj._11;
   v.y = -( ( ( 2.0f * mouseY ) / ScreenH() ) - 1 ) / matProj._22;
   v.z =  1.0f;

   // Get the inverse view matrix
   D3DXMATRIX matView, m;
   hr = m_pd3dDevice->GetTransform( D3DTS_VIEW, &matView );
   D3DXMatrixInverse( &m, NULL, &matView );

	// Transform the screen space pick ray into 3D space
	vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
	vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
	vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
	vPickRayOrig.x = m._41;
	vPickRayOrig.y = m._42;
	vPickRayOrig.z = m._43;

	// left button
   if( dims2.rgbButtons[0] & 0x80 && !(lastMouseState.rgbButtons[0] & 0x80)) 
   {
		SetCapture(m_hWnd);

		pie = new PumaInputEvent(PUMA_INPUT_LMBDOWN,"LMB DOWN");
		pie->mouseX = mouseX;
		pie->mouseY = mouseY;

		// Transform the screen space pick ray into 3D space
		pie->vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		pie->vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		pie->vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		pie->vPickRayOrig.x = m._41;
		pie->vPickRayOrig.y = m._42;
		pie->vPickRayOrig.z = m._43;

		inputEventList.Append(pie);
	}
   if( !(dims2.rgbButtons[0] & 0x80) && (lastMouseState.rgbButtons[0] & 0x80)) 
   {
		ReleaseCapture();
		pie = new PumaInputEvent(PUMA_INPUT_LMBUP,"LMB UP");
		pie->mouseX = mouseX;
		pie->mouseY = mouseY;

		// Transform the screen space pick ray into 3D space
		pie->vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		pie->vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		pie->vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		pie->vPickRayOrig.x = m._41;
		pie->vPickRayOrig.y = m._42;
		pie->vPickRayOrig.z = m._43;

		inputEventList.Append(pie);
	}

	// right button
   if( dims2.rgbButtons[1] & 0x80 && !(lastMouseState.rgbButtons[1] & 0x80)) 
   {
		pie = new PumaInputEvent(PUMA_INPUT_RMBDOWN,"RMB DOWN");
		pie->mouseX = mouseX;
		pie->mouseY = mouseY;

		// Transform the screen space pick ray into 3D space
		pie->vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		pie->vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		pie->vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		pie->vPickRayOrig.x = m._41;
		pie->vPickRayOrig.y = m._42;
		pie->vPickRayOrig.z = m._43;

		inputEventList.Append(pie);
	}
   if( !(dims2.rgbButtons[1] & 0x80) && (lastMouseState.rgbButtons[1] & 0x80)) 
   {
		pie = new PumaInputEvent(PUMA_INPUT_RMBUP,"RMB UP");
		pie->mouseX = mouseX;
		pie->mouseY = mouseY;

		// Transform the screen space pick ray into 3D space
		pie->vPickRayDir.x  = v.x*m._11 + v.y*m._21 + v.z*m._31;
		pie->vPickRayDir.y  = v.x*m._12 + v.y*m._22 + v.z*m._32;
		pie->vPickRayDir.z  = v.x*m._13 + v.y*m._23 + v.z*m._33;
		pie->vPickRayOrig.x = m._41;
		pie->vPickRayOrig.y = m._42;
		pie->vPickRayOrig.z = m._43;

		inputEventList.Append(pie);
	}

	// finally, fill the lastMouseState buffer with the current state, for next time.
	memcpy(&lastMouseState,&dims2,sizeof(dims2));

}

//***************************************************************************************
// if you call this function and get something back, YOU are
// responsible for deleting it!
PumaInputEvent *CPuma::TakeNextInputEvent(void)
{
	PumaInputEvent *e = (PumaInputEvent *)inputEventList.First();
	if (e)
		inputEventList.Remove(e);
	return e;
}


//***************************************************************************************
void CPuma::DrawMouse(LPDIRECT3DTEXTURE8 mouseArtPtr, int artOffsetX, int artOffsetY,
						   int locX, int locY)
{
	if (!mouseArtPtr)
		return;

	if (!squareVerts->isReady)
		return;

	// get size of mouse art
	D3DSURFACE_DESC desc;
	mouseArtPtr->GetLevelDesc(0, &desc);

	// set vertices correctly
   PUMA2DVERTEX* pVertices = (PUMA2DVERTEX*) vdMan->LockVerts(squareVerts);

	for (int i = 0; i < 4; i++)
	{
	   pVertices[i].color = 0xffffffff;
	   pVertices[i].rhw   = 1.0f;
	   pVertices[i].z     = 0.0f;
	}

	pVertices[0].x = (float) mouseX + artOffsetX;
	pVertices[0].y = (float) mouseY + artOffsetY;
	pVertices[1].x = (float) mouseX + artOffsetX + desc.Width;
	pVertices[1].y = (float) mouseY + artOffsetY;
	pVertices[2].x = (float) mouseX + artOffsetX;
	pVertices[2].y = (float) mouseY + artOffsetY + desc.Height;
	pVertices[3].x = (float) mouseX + artOffsetX + desc.Width;
	pVertices[3].y = (float) mouseY + artOffsetY + desc.Height;

	pVertices[0].tu = 0;
	pVertices[0].tv = 0;
	pVertices[1].tu = 1;
	pVertices[1].tv = 0;
	pVertices[2].tu = 0;
	pVertices[2].tv = 1;
	pVertices[3].tu = 1;
	pVertices[3].tv = 1;

   //memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
   vdMan->UnlockVerts(squareVerts);

	// assert the material and texture
   m_pd3dDevice->SetMaterial( &mouseMaterial );
	int res = m_pd3dDevice->SetTexture( 0, mouseArtPtr );
	assert(D3D_OK == res);
   m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,         D3DTA_TEXTURE);
   m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,           D3DTOP_SELECTARG1);
   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,         D3DTA_DIFFUSE);
   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_MODULATE);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER,         D3DTFN_LINEAR);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER,         D3DTFG_LINEAR);
 
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// draw!
   m_pd3dDevice->SetStreamSource( 0, squareVerts->vertexBufferPtr, sizeof(PUMA2DVERTEX) );
   m_pd3dDevice->SetVertexShader( D3DFVF_PUMA2DVERTEX );
   m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

}

//***************************************************************************************
void CPuma::DrawRect(LPDIRECT3DTEXTURE8 artPtr, 
						  int left, int top, int right, int bottom, 
						  DWORD color, int hasTransparency, int left2, int top2 )
{

	if (!squareVerts->isReady)
		return;

	// get size of mouse art
	D3DSURFACE_DESC desc;
	if (artPtr)
		artPtr->GetLevelDesc(0, &desc);
	
	else
	{
		desc.Width  = right  - left;
		desc.Height = bottom - top;
	}
	  
	// set vertices correctly
   PUMA2DVERTEX* pVertices = (PUMA2DVERTEX*) vdMan->LockVerts(squareVerts);

	for (int i = 0; i < 4; i++)
	{
	   pVertices[i].color = color;
	   pVertices[i].rhw   = 1.0f;
	   pVertices[i].z     = 0.0f;
	}

	pVertices[0].x = (float) left + left2;
	pVertices[0].y = (float) top + top2;
	pVertices[1].x = (float) right;
	pVertices[1].y = (float) top + top2;
	pVertices[2].x = (float) left + left2;
	pVertices[2].y = (float) bottom;
	pVertices[3].x = (float) right;
	pVertices[3].y = (float) bottom;

//	if (top2 > 0 || left2 > 0)
//		top2 = top2;

	pVertices[0].tu = (float)(left2) / (float) desc.Width;
	pVertices[0].tv = (float)(top2) / (float) desc.Height;
	pVertices[1].tu = (float)(right - left) / (float) desc.Width;
	pVertices[1].tv = (float)(top2) / (float) desc.Height;
	pVertices[2].tu = (float)(left2) / (float) desc.Width;
	pVertices[2].tv = (float)(bottom - top) / (float) desc.Height;
	pVertices[3].tu = (float)(right - left) / (float) desc.Width;
	pVertices[3].tv = (float)(bottom - top) / (float) desc.Height;

   //memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
   vdMan->UnlockVerts(squareVerts);

	// assert the material and texture
   m_pd3dDevice->SetMaterial( &mouseMaterial );

//	int res = m_pd3dDevice->SetTexture( 0, NULL );
//	assert(D3D_OK == res);

	int res = m_pd3dDevice->SetTexture( 0, artPtr );
	assert(D3D_OK == res);
/*
        m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRS_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,          FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPING,         TRUE );
        m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND,      FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
        m_pd3dDevice->SetRenderState( D3DRS_FOGENABLE,        FALSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );
*/


//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,         D3DTA_TEXTURE);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,           D3DTOP_SELECTARG1);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,         D3DTA_DIFFUSE);

	SetTextureColorStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
	SetTextureAlphaStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
//	SetTextureAlphaStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_TEXTURE  );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
   m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
//	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS  );
   m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );

//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_MODULATE);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER,         D3DTFN_LINEAR);
//   m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER,         D3DTFG_LINEAR);
 
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
//	m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	// draw!
   m_pd3dDevice->SetStreamSource( 0, squareVerts->vertexBufferPtr, sizeof(PUMA2DVERTEX) );
   m_pd3dDevice->SetVertexShader( D3DFVF_PUMA2DVERTEX );

   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

   m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

}


//***************************************************************************************
void CPuma::DrawRectUV(LPDIRECT3DTEXTURE8 artPtr, 
						  int left, int top, int right, int bottom, 
						  DWORD color, int hasTransparency,
						  float u1, float v1, float u2, float v2)
{

	if (!squareVerts->isReady)
		return;

	// get size of art
	D3DSURFACE_DESC desc;
	if (artPtr)
		artPtr->GetLevelDesc(0, &desc);
	
	else
	{
		desc.Width  = right  - left;
		desc.Height = bottom - top;
	}
	  
	// set vertices correctly
   PUMA2DVERTEX* pVertices = (PUMA2DVERTEX*) vdMan->LockVerts(squareVerts);

	for (int i = 0; i < 4; i++)
	{
	   pVertices[i].color = color;
	   pVertices[i].rhw   = 1.0f;
	   pVertices[i].z     = 0.0f;
	}

	pVertices[0].x = (float) left;
	pVertices[0].y = (float) top;
	pVertices[1].x = (float) right;
	pVertices[1].y = (float) top;
	pVertices[2].x = (float) left;
	pVertices[2].y = (float) bottom;
	pVertices[3].x = (float) right;
	pVertices[3].y = (float) bottom;

	pVertices[0].tu = u1;//(float)(left2) / (float) desc.Width;
	pVertices[0].tv = v1;//(float)(top2) / (float) desc.Height;
	pVertices[1].tu = u2;//(float)(right - left) / (float) desc.Width;
	pVertices[1].tv = v1;//(float)(top2) / (float) desc.Height;
	pVertices[2].tu = u1;//(float)(left2) / (float) desc.Width;
	pVertices[2].tv = v2;//(float)(bottom - top) / (float) desc.Height;
	pVertices[3].tu = u2;//(float)(right - left) / (float) desc.Width;
	pVertices[3].tv = v2;//(float)(bottom - top) / (float) desc.Height;

   vdMan->UnlockVerts(squareVerts);

	// assert the material and texture
   m_pd3dDevice->SetMaterial( &mouseMaterial );

	int res = m_pd3dDevice->SetTexture( 0, artPtr );
	assert(D3D_OK == res);

	SetTextureColorStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
	SetTextureAlphaStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
   m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
   m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );

	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);
	
	// draw!
   m_pd3dDevice->SetStreamSource( 0, squareVerts->vertexBufferPtr, sizeof(PUMA2DVERTEX) );
   m_pd3dDevice->SetVertexShader( D3DFVF_PUMA2DVERTEX );

   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

   m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

}


//***************************************************************************************
void CPuma::DrawGradientRect(int left, int top, int right, int bottom, 
						  DWORD *color)
{

	if (!squareVerts->isReady)
		return;

	D3DSURFACE_DESC desc;
	desc.Width  = right  - left;
	desc.Height = bottom - top;
	  
	// set vertices correctly
   PUMA2DVERTEX* pVertices = (PUMA2DVERTEX*) vdMan->LockVerts(squareVerts);

	for (int i = 0; i < 4; i++)
	{
	   pVertices[i].color = color[i];
	   pVertices[i].rhw   = 1.0f;
	   pVertices[i].z     = 0.0f;
	}

	pVertices[0].x = (float) left;
	pVertices[0].y = (float) top;
	pVertices[1].x = (float) right;
	pVertices[1].y = (float) top;
	pVertices[2].x = (float) left;
	pVertices[2].y = (float) bottom;
	pVertices[3].x = (float) right;
	pVertices[3].y = (float) bottom;

   vdMan->UnlockVerts(squareVerts);

	// assert the material and texture
   m_pd3dDevice->SetMaterial( &mouseMaterial );

	int res = m_pd3dDevice->SetTexture( 0, NULL );

	SetTextureColorStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_MODULATE  , D3DTA_DIFFUSE );
	SetTextureAlphaStage( m_pd3dDevice, 0, D3DTA_TEXTURE, D3DTOP_SELECTARG1, D3DTA_DIFFUSE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
	m_pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_ALWAYS  );
   m_pd3dDevice->SetRenderState( D3DRS_EDGEANTIALIAS,    FALSE );

	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);

	// draw!
   m_pd3dDevice->SetStreamSource( 0, squareVerts->vertexBufferPtr, sizeof(PUMA2DVERTEX) );
   m_pd3dDevice->SetVertexShader( D3DFVF_PUMA2DVERTEX );

   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_POINT );
   m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE );

   m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

}



//***************************************************************************************
void CPuma::DrawRect(LPDIRECT3DTEXTURE8 artPtr, RECT drawRect, 
						  DWORD color, int hasTransparency )
{
	DrawRect(artPtr, drawRect.left, drawRect.top, drawRect.right, drawRect.bottom, 
		      color, hasTransparency);
}

//***************************************************************************************
void CPuma::ClearMaterial(void)
{
   m_pd3dDevice->SetMaterial( &emptyMaterial );
}

//***************************************************************************************
void CPuma::UpdateScreenSizeInfo(void)
{
	GetClientRect( m_hWnd, &m_rcWindowClient );
}

//***************************************************************************************
int CPuma::ScreenW(void)
{
	return m_d3dpp.BackBufferWidth;
}

//***************************************************************************************
int CPuma::ScreenH(void)
{
	return m_d3dpp.BackBufferHeight;
}


//***************************************************************************************
int CPuma::LoadTexture(char *fileName, LPDIRECT3DTEXTURE8 *tex, D3DCOLOR trans)
{
	return D3DXCreateTextureFromFileEx( m_pd3dDevice, fileName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, trans, NULL, NULL, tex);
};


//***************************************************************************************
//***************************************************************************************
PumaInputEvent::PumaInputEvent(int doid, char *doname) : DataObject(doid,doname)
{
	mouseX = mouseY = -1;       // invalid
}








//***************************************************************************************
//***************************************************************************************
PumaCamera::PumaCamera()
{
   // Set attributes for the view matrix
	spacePoint.location = D3DXVECTOR3( 0.0f, -50.0f,200.0f);
	spacePoint.angle    = 0;
	spacePoint.azimuth  = -0.3f;

   // Set attributes for the projection matrix
   m_fFOV        = D3DX_PI/4;
   m_fAspect     = 640.0f/480.0f;
   m_fNearPlane  = 1.0f;
   m_fFarPlane   = 1000.0f;

}


//***************************************************************************************
void PumaCamera::AssertView(LPDIRECT3DDEVICE8 m_pd3dDevice)
{
   m_pd3dDevice->SetTransform( D3DTS_VIEW,     spacePoint.GetMatrix() );
   m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
}


//***************************************************************************************
void PumaCamera::Calculate(void)
{
/*
	D3DXMATRIX facing;
   D3DXMatrixIdentity( &facing );
	D3DXMatrixRotationZ(&facing, azimuth);
	D3DXMatrixRotationY(&facing, angle);

   D3DXMatrixIdentity( &m_matView );
	D3DXMatrixTranslation(&m_matView, location.x, location.y, location.z);

	D3DXMatrixMultiply(&m_matView, &m_matView, &facing);
*/
//   facing._41 = location.x;
//   facing._42 = location.y;
//   facing._43 = location.z;

//	m_matView = facing;

//   m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat );
  /*
   // Point ahead of camera
   forward.x = 0+sinf(angle);
   forward.z = 0+cosf(angle);
	forward.y = 0;

   // Point to side of camera
   right.x = 0+sinf(angle + D3DX_PI/2.0f);
   right.z = 0+cosf(angle + D3DX_PI/2.0f);
   right.y = 0;

   // Point above camera
   up.x = 0;
   up.z = 0;
   up.y = 0+sinf(azimuth);

   // Compute vectors of the ship's orientation
   D3DXVECTOR3 vForwardDir = forward - location;
   D3DXVECTOR3 vSideDir    = right   - location;
   D3DXVECTOR3 vUpDir      = up      - location;
   D3DXVECTOR3 vNormalDir;
   D3DXVec3Cross( &vNormalDir, &vForwardDir, &vSideDir );

   // Construct matrix to orient camera
   D3DXMATRIX matWorld, matLookAt, matRotateZ;
//   D3DXMatrixRotationZ( &matRotateZ, g_pShip->fRoll );
   D3DXMatrixLookAtLH( &matLookAt, &location, &(location-vForwardDir), &vNormalDir );
   D3DXMatrixInverse( &matLookAt, NULL, &matLookAt );
   D3DXMatrixIdentity( &matWorld );
//   D3DXMatrixMultiply( &matWorld, &matWorld, &matRotateZ );
   D3DXMatrixMultiply( &matWorld, &matWorld, &matLookAt );

   // Set attributes for the view matrix
//   location   = vEyePt;
//   m_vLookatPt = vLookatPt;
//   m_vUpVec   = vUpVec;
	D3DXMATRIX facing;
   D3DXMatrixIdentity( &facing );
	facing
	D3DXMatrixRotationZ(&facing, azimuth);
	D3DXMatrixRotationY(&facing, angle);

   D3DXVec3Normalize( &m_vView, &(m_vLookatPt - location) );
   D3DXVec3Cross( &m_vCross, &m_vView, &m_vUpVec );

   D3DXMatrixLookAtLH( &m_matView, &location, &m_vLookatPt, &m_vUpVec );
	*/
//   D3DXMatrixInverse( &m_matBillboard, NULL, &m_matView );
//   m_matBillboard._41 = 0.0f;
//   m_matBillboard._42 = 0.0f;
//   m_matBillboard._43 = 0.0f;

   // Set attributes for the projection matrix
//   m_fFOV     = fFOV;
//   m_fAspect   = fAspect;
//   m_fNearPlane  = fNearPlane;
//   m_fFarPlane   = fFarPlane;

   D3DXMatrixPerspectiveFovLH( &m_matProj, m_fFOV, m_fAspect, m_fNearPlane, m_fFarPlane );
}


//***************************************************************************************
void PumaCamera::UpdateProjection(void)
{
   D3DXMatrixPerspectiveFovLH( &m_matProj, m_fFOV, m_fAspect, m_fNearPlane, m_fFarPlane );
   puma->m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_matProj );
}














//***************************************************************************************
//***************************************************************************************
SpacePoint::SpacePoint(void)
{

	angle = azimuth = roll = 0.0f;
	location = D3DXVECTOR3(0.0f,0.0f,0.0f);
   D3DXMatrixIdentity( &mat );

}
	
//***************************************************************************************
D3DXMATRIX *SpacePoint::GetMatrix(void)
{
/*
				D3DXMatrixIdentity( &matWorld );
				D3DXMatrixRotationY(&matWorld, (float) afoShip->spacePoint.angle * -1 + D3DX_PI);

		//  /	   D3DXMATRIX mat2;
				D3DXMatrixIdentity( &mat2 );
				D3DXMatrixTranslation(&mat2, afoShip->spacePoint.location.x,
													 afoShip->spacePoint.location.y,
											 afoShip->spacePoint.location.z);

				D3DXMatrixMultiply(&matWorld, &matWorld, &mat2);
*/
	D3DXMATRIX facing, rollMat, azimuthMat;
   D3DXMatrixIdentity( &rollMat );
	D3DXMatrixRotationZ(&rollMat, roll);
   D3DXMatrixIdentity( &azimuthMat );
	D3DXMatrixRotationX(&azimuthMat, azimuth);
   D3DXMatrixIdentity( &facing );
	D3DXMatrixRotationY(&facing, angle * -1 + D3DX_PI);

	D3DXMatrixMultiply(&azimuthMat, &azimuthMat, &rollMat);
	D3DXMatrixMultiply(&facing, &facing, &azimuthMat);

   D3DXMatrixIdentity( &mat );
	D3DXMatrixTranslation(&mat, location.x, location.y, location.z);

//	D3DXMatrixMultiply(&facing, &facing, &mat);
	D3DXMatrixMultiply(&mat, &mat, &facing);

	return &mat;

}

//***************************************************************************************
void SpacePoint::MoveForwardRelative(float amount)
{
   location.x += sinf(angle) * amount;
   location.z += cosf(angle) * amount;
}

//***************************************************************************************
void SpacePoint::MoveRightRelative (float amount)
{
   location.x += sinf(angle + D3DX_PI/2.0f) * amount;
   location.z -= cosf(angle + D3DX_PI/2.0f) * amount;
}

//***************************************************************************************
void SpacePoint::MoveUpRelative    (float amount)
{
   location.y = sinf(azimuth) * amount;
}


//-----------------------------------------------------------------------------
// Name: IntersectTriangle()
// Desc: Given a ray origin (orig) and direction (dir), and three vertices of
//       of a triangle, this function returns TRUE and the distance to Vert0
//			if an intersection occurs
//-----------------------------------------------------------------------------
BOOL IntersectTriangle( const D3DXVECTOR3& orig,
                                       const D3DXVECTOR3& dir, D3DXVECTOR3& v0,
                                       D3DXVECTOR3& v1, D3DXVECTOR3& v2,
                                       FLOAT* dist)
{
	FLOAT u,v,t;

    // Find vectors for two edges sharing vert0
    D3DXVECTOR3 edge1 = v1 - v0;
    D3DXVECTOR3 edge2 = v2 - v0;

    // Begin calculating determinant - also used to calculate U parameter
    D3DXVECTOR3 pvec;
    D3DXVec3Cross( &pvec, &dir, &edge2 );

    // If determinant is near zero, ray lies in plane of triangle
    FLOAT det = D3DXVec3Dot( &edge1, &pvec );
    if( det < 0.0001f )
        return FALSE;

    // Calculate distance from vert0 to ray origin
    D3DXVECTOR3 tvec = orig - v0;
	 *dist = D3DXVec3Length(&tvec);

    // Calculate U parameter and test bounds
    u = D3DXVec3Dot( &tvec, &pvec );
    if( u < 0.0f || u > det )
        return FALSE;

    // Prepare to test V parameter
    D3DXVECTOR3 qvec;
    D3DXVec3Cross( &qvec, &tvec, &edge1 );

    // Calculate V parameter and test bounds
    v = D3DXVec3Dot( &dir, &qvec );
    if( v < 0.0f || u + v > det )
        return FALSE;

    // Calculate t, scale parameters, ray intersects triangle
    t = D3DXVec3Dot( &edge2, &qvec );
    FLOAT fInvDet = 1.0f / det;
    t *= fInvDet;
    u *= fInvDet;
    v *= fInvDet;

    return TRUE;
}






/* end of file */
