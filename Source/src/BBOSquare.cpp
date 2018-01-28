
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOSquare.h"
#include ".\helper\GeneralUtils.h"

struct LANDSQUAREVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   DWORD color; };

#define D3DFVF_LANDSQUAREVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_DIFFUSE)

inline LANDSQUAREVERTEX InitSquareVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                             D3DCOLOR color)
{
   LANDSQUAREVERTEX v;   v.p = p;   v.n = n;   v.color = color;
   return v;
}

const int MAX_NUM_LAND_LANDSQUARES = 10;
const int MAX_LAND_LANDSQUARE_VERTS = MAX_NUM_LAND_LANDSQUARES * 3 * 2;

//******************************************************************
//******************************************************************
LandSquareManager::LandSquareManager(GroundObjectTiles *g)
{
	go = g;

	// create vert list
   puma->m_pd3dDevice->CreateVertexBuffer( MAX_LAND_LANDSQUARE_VERTS*sizeof(LANDSQUAREVERTEX),
                                          D3DUSAGE_WRITEONLY, 0,
                                          D3DPOOL_MANAGED, &vBuffer );
}

//******************************************************************
LandSquareManager::~LandSquareManager()
{
	//delete all
   SAFE_RELEASE( vBuffer );

}

//******************************************************************
void LandSquareManager::StartFrame(void)
{
	triIndex = 0;
}

//DWORD

//******************************************************************
void LandSquareManager::AddSquare(int x, int y)
{
	if (x < 0 || y < 0 || x > 126 || y > 126)
		return;

	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	LANDSQUAREVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// copy the first land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+0].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+0].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+0].p.x,
				 				  pVertices[triIndex*3+0].p.z, NULL);
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+1].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+1].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+1].p.x,
				 				  pVertices[triIndex*3+1].p.z, NULL);
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+2].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+2].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+2].p.x,
				 				  pVertices[triIndex*3+2].p.z, NULL);
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

	// copy the second land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+0].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+0].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+0].p.x,
				 				  pVertices[triIndex*3+0].p.z, NULL);
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+1].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+1].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+1].p.x,
				 				  pVertices[triIndex*3+1].p.z, NULL);
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+2].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+2].p.y = 0.01f + 
		  go->HeightAtPoint(pVertices[triIndex*3+2].p.x,
				 				  pVertices[triIndex*3+2].p.z, NULL);
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

   vBuffer->Unlock();

}

//******************************************************************
void LandSquareManager::AddDungeonSquare(int x, int y)
{

	if (x < 0 || y < 0 || x > 126 || y > 126)
		return;

	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	LANDSQUAREVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// copy the first land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+0].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+0].p.y = 0.01f;
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+1].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+1].p.y = 0.01f;
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+2].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+2].p.y = 0.01f;
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

	// copy the second land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+0].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+0].p.y = 0.01f;
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = go->GetXForPoint(x);
	pVertices[triIndex*3+1].p.z = go->GetZForPoint(y+1);
	pVertices[triIndex*3+1].p.y = 0.01f;
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = go->GetXForPoint(x+1);
	pVertices[triIndex*3+2].p.z = go->GetZForPoint(y);
	pVertices[triIndex*3+2].p.y = 0.01f;
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

   vBuffer->Unlock();

}

//******************************************************************
void LandSquareManager::AddRealmSquare(int x, int y)
{

	if (x < 0 || y < 0 || x > ro->sizeW-2 || y > ro->sizeH-2)
		return;

	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	LANDSQUAREVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// copy the first land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = ro->GetXForPoint(x);
	pVertices[triIndex*3+0].p.z = ro->GetZForPoint(y);
	pVertices[triIndex*3+0].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+0].p.x,
				 				  pVertices[triIndex*3+0].p.z, NULL);
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = ro->GetXForPoint(x+1);
	pVertices[triIndex*3+1].p.z = ro->GetZForPoint(y);
	pVertices[triIndex*3+1].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+1].p.x,
				 				  pVertices[triIndex*3+1].p.z, NULL);
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = ro->GetXForPoint(x);
	pVertices[triIndex*3+2].p.z = ro->GetZForPoint(y+1);
	pVertices[triIndex*3+2].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+2].p.x,
				 				  pVertices[triIndex*3+2].p.z, NULL);
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

	// copy the second land tri into the shadow tri buffer
	pVertices[triIndex*3+0].color = 0x2f2f2f2f;
	pVertices[triIndex*3+0].n.x = 0;
	pVertices[triIndex*3+0].n.y = 1;
	pVertices[triIndex*3+0].n.z = 0;
	pVertices[triIndex*3+0].p.x = ro->GetXForPoint(x+1);
	pVertices[triIndex*3+0].p.z = ro->GetZForPoint(y+1);
	pVertices[triIndex*3+0].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+0].p.x,
				 				  pVertices[triIndex*3+0].p.z, NULL);
	
	pVertices[triIndex*3+1].color = 0x2f2f2f2f;
	pVertices[triIndex*3+1].n.x = 0;
	pVertices[triIndex*3+1].n.y = 1;
	pVertices[triIndex*3+1].n.z = 0;
	pVertices[triIndex*3+1].p.x = ro->GetXForPoint(x);
	pVertices[triIndex*3+1].p.z = ro->GetZForPoint(y+1);
	pVertices[triIndex*3+1].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+1].p.x,
				 				  pVertices[triIndex*3+1].p.z, NULL);
	
	pVertices[triIndex*3+2].color = 0x2f2f2f2f;
	pVertices[triIndex*3+2].n.x = 0;
	pVertices[triIndex*3+2].n.y = 1;
	pVertices[triIndex*3+2].n.z = 0;
	pVertices[triIndex*3+2].p.x = ro->GetXForPoint(x+1);
	pVertices[triIndex*3+2].p.z = ro->GetZForPoint(y);
	pVertices[triIndex*3+2].p.y = 0.01f + 
		  ro->HeightAtPoint(pVertices[triIndex*3+2].p.x,
				 				  pVertices[triIndex*3+2].p.z, NULL);
	
	++triIndex;
	if (triIndex >= MAX_LAND_LANDSQUARE_VERTS / 3)
	{
	   vBuffer->Unlock();
		return;
	}

   vBuffer->Unlock();

}


//******************************************************************
void LandSquareManager::DrawSquares(void)
{

   // Set the render states for doing shadows
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , FALSE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_NONE);
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,   D3DTADDRESS_CLAMP  );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,   D3DTADDRESS_CLAMP  );
//   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

//	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_ZERO);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCCOLOR);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_SRCCOLOR);
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CW );
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

	puma->m_pd3dDevice->SetTexture( 0, NULL );

   puma->m_pd3dDevice->SetVertexShader( D3DFVF_LANDSQUAREVERTEX );
   puma->m_pd3dDevice->SetStreamSource( 0, vBuffer, sizeof(LANDSQUAREVERTEX) );

   if( triIndex > 0 )
      puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, triIndex );

	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,   D3DTADDRESS_WRAP  );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,   D3DTADDRESS_WRAP  );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MIPFILTER, D3DTEXF_POINT);
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE , TRUE );
	puma->m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );


}





/* end of file */



