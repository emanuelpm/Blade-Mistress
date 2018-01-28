
//***************************************************************
// base class for a game mode which likes to belong to a stack.
//***************************************************************
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "puma.h"
#include "pumamesh.h"
#include "testmode.h"

// ******** data for test triangle
LPDIRECT3DVERTEXBUFFER8 g_pVB     = NULL; // Buffer to hold vertices

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
   FLOAT x, y, z;    // The untransformed, 3D position for the vertex
   DWORD color;     // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
// ******** end data for test triangle

PumaMesh *pm = NULL;


//******************************************************************
TestMode::TestMode(int doid, char *doname) : GameMode(doid,doname)
{
}

//******************************************************************
TestMode::~TestMode()
{

}

//******************************************************************
int TestMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int TestMode::Activate(void) // do this when the mode becomes the forground mode.
{
   // Initialize three vertices for rendering a triangle
   CUSTOMVERTEX g_Vertices[] =
   {
     { -1.0f,-1.0f, 0.0f, 0xffff0000, },
     {  1.0f,-1.0f, 0.0f, 0xff0000ff, },
     {  0.0f, 1.0f, 0.0f, 0xffffffff, },
   };

   // Create the vertex buffer.
   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( 3*sizeof(CUSTOMVERTEX),
                             0, D3DFVF_CUSTOMVERTEX,
                             D3DPOOL_DEFAULT, &g_pVB ) ) )
   {
     return E_FAIL;
   }

   // Fill the vertex buffer.
   VOID* pVertices;
   if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (BYTE**)&pVertices, 0 ) ) )
     return E_FAIL;
   memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
   g_pVB->Unlock();

	pm = new PumaMesh(0,"TEST_MESH");

//	pm->LoadFromASC(puma->m_pd3dDevice, "dat\\cone.ASE");
//	pm->LoadFromASC(puma->m_pd3dDevice, "dat\\testOldShip.ASE");
	pm->LoadFromASC(puma->m_pd3dDevice, "dat\\Sphere.ASE");
	pm->LoadTexture(puma->m_pd3dDevice, "dat\\standardPartTexture.bmp");


	// Set up a white, directional light, with an oscillating direction.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
   ZeroMemory( &light, sizeof(D3DLIGHT8) );
   light.Type      = D3DLIGHT_DIRECTIONAL;
   light.Diffuse.r  = 1.0f;
   light.Diffuse.g  = 1.0f;
   light.Diffuse.b  = 1.0f;
   light.Ambient.r  = 1.0f;
   light.Ambient.g  = 1.0f;
   light.Ambient.b  = 1.0f;
   vecDir = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
   light.Range      = 1000.0f;
   puma->m_pd3dDevice->SetLight( 0, &light );
   puma->m_pd3dDevice->LightEnable( 0, TRUE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   // Finally, turn on some ambient light.
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(128,128,128,255)  );


	return(0);
}

//******************************************************************
int TestMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
	SAFE_RELEASE(g_pVB);

	return(0);
}

float shipAngle = 0.0f;

//******************************************************************
int TestMode::Tick(void)
{
	puma->StartRenderingFrame();

	shipAngle += 0.04f;

   D3DXMATRIX matWorld;
   D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY(&matWorld, shipAngle);
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

   // Render the vertex buffer contents
	/*
   puma->m_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
   puma->m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
   puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	*/
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00808080 );

	pm->Draw(puma->m_pd3dDevice);

	CD3DFont *font = puma->GetDXFont(0);

	RECT tempRect;
	tempRect.left   = 10;
	tempRect.top   = 10;
	tempRect.right  = 200;
	tempRect.bottom = 40;

	D3DCOLOR color;

	color = D3DCOLOR_RGBA(255,0,0,255);

	
//   font->DrawText( 10, 10, color, "Test String!" );

//	HRESULT hr = font->DrawText("Test string!", strlen("Test string!"), &tempRect,	DT_LEFT, color);

	puma->HandleCamera();

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


/* end of file */



