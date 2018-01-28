
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "3DInfoRecord.h"
#include ".\helper\GeneralUtils.h"

//******************************************************************
//******************************************************************
InfoRecord3D::InfoRecord3D(float ang, float x, float y, float z, 
									char *t, float h, DWORD col, bool bold)	 : DataObject(0,"InfoRecord3D")
{
	angle = ang;
	position.x = x;
	position.y = y;
	position.z = z;
	text = t;
	healthBarValue = h;
	color = col;
	scale = 15.0f;
	bBold = bold;
}

//******************************************************************
InfoRecord3D::~InfoRecord3D()
{
}

//******************************************************************
void InfoRecord3D::Draw(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar)
{
   D3DXMATRIX matWorld, mat2;

   D3DXMatrixIdentity( &matWorld );
   D3DXMatrixRotationY(&matWorld, (float) angle);
   matWorld._41 = position.x;
   matWorld._42 = position.y;
   matWorld._43 = position.z;
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	CD3DFont *pf = puma->GetDXFont(1);
	float oldScale = pf->m_fTextScale;
	pf->m_fTextScale = scale;

//	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	if (healthBarValue > -1)
	{
	   matWorld._41 = position.x;
		matWorld._42 = position.y + 0.19f;
	   matWorld._43 = position.z;
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		DrawBar(redBar, greenBar);

	   matWorld._41 = position.x;
		matWorld._42 = position.y;
	   matWorld._43 = position.z;
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	}

	int flags = D3DFONT_CENTERED | D3DFONT_TWOSIDED | D3DFONT_FILTERED;

	if (bBold)
		flags |= D3DFONT_BOLD;

	if (text)
		pf->Render3DText(text, flags, color);
	pf->m_fTextScale = oldScale;
//	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}


struct RECORD3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   DWORD color;    FLOAT tu, tv; };

#define D3DFVF_RECORD3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE|D3DFVF_TEX1)

inline RECORD3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                             D3DCOLOR color, FLOAT tu, FLOAT tv )
{
   RECORD3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv; v.color = color;
   return v;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void InfoRecord3D::DrawBar(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar)
{
   if( puma->m_pd3dDevice == NULL )
      return;

   LPDIRECT3DVERTEXBUFFER8 vBuffer;        // VertexBuffer for rendering text

   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( 6*2*sizeof(RECORD3DVERTEX),
                                          D3DUSAGE_WRITEONLY, 0,
                                          D3DPOOL_MANAGED, &vBuffer ) ) )
   {
      return ;
   }


	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , FALSE );

   // Setup renderstate
//   puma->m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
//   puma->m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
   puma->m_pd3dDevice->SetVertexShader( D3DFVF_RECORD3DVERTEX );
   puma->m_pd3dDevice->SetStreamSource( 0, vBuffer, sizeof(RECORD3DVERTEX) );

   // Set filter states
//   if( dwFlags & D3DRECORD_FILTERED )
   {
//      puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
//      puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   }

   puma->m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1,         D3DTA_TEXTURE);
   puma->m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,           D3DTOP_SELECTARG1);
   puma->m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
   puma->m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,         D3DTA_DIFFUSE);
   puma->m_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_MODULATE);
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
 
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,   TRUE);

   // Position
   FLOAT w = 20 / 50.0f;
   FLOAT h = 4 / 50.0f;
   FLOAT x = w * -0.5f;
   FLOAT y = h * -0.5f;

   // Turn off culling for two-sided text
//   if( dwFlags & D3DFONT_TWOSIDED )
   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

   FLOAT fStartX = x;
//   TCHAR c;

   // Fill vertex buffer
   RECORD3DVERTEX* pVertices;
   DWORD       dwNumTriangles = 0L;
   vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 );

   FLOAT tx1 = 0;
   FLOAT ty1 = 0;
   FLOAT tx2 = 1;
   FLOAT ty2 = 1;

   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty1 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   dwNumTriangles += 2;

   // Unlock and render the vertex buffer
   vBuffer->Unlock();

	puma->m_pd3dDevice->SetTexture( 0, redBar );

   if( dwNumTriangles > 0 )
      puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

	// draw green overlay
   dwNumTriangles = 0L;
   vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 );

	if (healthBarValue < 0)
		healthBarValue = 0;
	if (healthBarValue > 1)
		healthBarValue = 1;

   tx2 = healthBarValue;

   w = (20 / 50.0f) * healthBarValue;

   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty1 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitFont3DVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   dwNumTriangles += 2;

   // Unlock and render the vertex buffer
   vBuffer->Unlock();

	puma->m_pd3dDevice->SetTexture( 0, greenBar );

   if( dwNumTriangles > 0 )
      puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

   SAFE_RELEASE( vBuffer );

	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );

   return;
}





/* end of file */



