
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOShadows.h"
#include ".\helper\GeneralUtils.h"
/*
//******************************************************************
//******************************************************************
LandShadowManager::LandShadowManager(GroundObjectTiles *g)
{
	go = g;

	// load texture
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\shadow1.png",
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xff000000, NULL, NULL, &bitmap);


	// create vert list
   puma->m_pd3dDevice->CreateVertexBuffer( MAX_LAND_SHADOW_VERTS*sizeof(SHADOWVERTEX),
                                          D3DUSAGE_WRITEONLY, 0,
                                          D3DPOOL_MANAGED, &vBuffer );
}

//******************************************************************
LandShadowManager::~LandShadowManager()
{
	//delete all
   SAFE_RELEASE( bitmap );
   SAFE_RELEASE( vBuffer );

}

//******************************************************************
void LandShadowManager::StartFrame(void)
{
	triIndex = 0;
}

//******************************************************************
void LandShadowManager::AddShadow(float x, float y, float scale)
{

	if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	SHADOWVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// figure out the extents of the shadow square
	float leastPosX = x - scale;
	float leastPosY = y - scale;
	float mostPosX  = x + scale;
	float mostPosY  = y + scale;

	int leastGridX = go->GetGridX(leastPosX);
	if (leastGridX < 0)
		leastGridX = 0;
	int leastGridY = go->GetGridY(leastPosY);
	if (leastGridY < 0)
		leastGridY = 0;
	int mostGridX  = go->GetGridX(mostPosX);
	if (mostGridX > 126)
		mostGridX = 126;
	int mostGridY  = go->GetGridY(mostPosY);
	if (mostGridY > 126)
		mostGridY = 126;

	// for each of the affected squares
	for (int i = leastGridY; i <= mostGridY; ++i)
	{
		for (int j = leastGridX; j <= mostGridX; ++j)
		{
			// copy the first land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - j * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - i * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = go->GetXForPoint(j);
			pVertices[triIndex*3+0].p.z = go->GetZForPoint(i);
			pVertices[triIndex*3+0].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+0].p.x,
						 				  pVertices[triIndex*3+0].p.z, NULL);
	
			pVertices[triIndex*3+1].tu = (x - (j+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - i     * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = go->GetXForPoint(j+1);
			pVertices[triIndex*3+1].p.z = go->GetZForPoint(i);
			pVertices[triIndex*3+1].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+1].p.x,
						 				  pVertices[triIndex*3+1].p.z, NULL);
	
			pVertices[triIndex*3+2].tu = (x - j     * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - (i+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = go->GetXForPoint(j);
			pVertices[triIndex*3+2].p.z = go->GetZForPoint(i+1);
			pVertices[triIndex*3+2].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+2].p.x,
						 				  pVertices[triIndex*3+2].p.z, NULL);
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}

			// copy the second land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - (j+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - (i+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = go->GetXForPoint(j+1);
			pVertices[triIndex*3+0].p.z = go->GetZForPoint(i+1);
			pVertices[triIndex*3+0].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+0].p.x,
						 				  pVertices[triIndex*3+0].p.z, NULL);
	
			pVertices[triIndex*3+1].tu = (x - j     * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - (i+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = go->GetXForPoint(j);
			pVertices[triIndex*3+1].p.z = go->GetZForPoint(i+1);
			pVertices[triIndex*3+1].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+1].p.x,
						 				  pVertices[triIndex*3+1].p.z, NULL);
	
			pVertices[triIndex*3+2].tu = (x - (j+1) * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - i     * go->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = go->GetXForPoint(j+1);
			pVertices[triIndex*3+2].p.z = go->GetZForPoint(i);
			pVertices[triIndex*3+2].p.y = 0.01f + 
				  go->HeightAtPoint(pVertices[triIndex*3+2].p.x,
						 				  pVertices[triIndex*3+2].p.z, NULL);
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}
  
		}

	}


   vBuffer->Unlock();

}

//******************************************************************
void LandShadowManager::AddDungeonShadow(float x, float y, float scale)
{

	if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	SHADOWVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// figure out the extents of the shadow square
	float leastPosX = x - scale;
	float leastPosY = y - scale;
	float mostPosX  = x + scale;
	float mostPosY  = y + scale;

	int leastGridX = leastPosX / 10;
	int leastGridY = leastPosY / 10;
	int mostGridX  = mostPosX  / 10;
	int mostGridY  = mostPosY  / 10;

	// for each of the affected squares
	for (int i = leastGridY; i <= mostGridY; ++i)
	{
		for (int j = leastGridX; j <= mostGridX; ++j)
		{
			// copy the first land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - j * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - i * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = (j)*10;
			pVertices[triIndex*3+0].p.z = (i)*10;
			pVertices[triIndex*3+0].p.y = 0.01f;
	
			pVertices[triIndex*3+1].tu = (x - (j+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - i     * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = (j+1)*10;
			pVertices[triIndex*3+1].p.z = (i)*10;
			pVertices[triIndex*3+1].p.y = 0.01f;
	
			pVertices[triIndex*3+2].tu = (x - j     * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - (i+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = (j)*10;
			pVertices[triIndex*3+2].p.z = (i+1)*10;
			pVertices[triIndex*3+2].p.y = 0.01f;
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}

			// copy the second land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - (j+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - (i+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = (j+1)*10;
			pVertices[triIndex*3+0].p.z = (i+1)*10;
			pVertices[triIndex*3+0].p.y = 0.01f;
	
			pVertices[triIndex*3+1].tu = (x - j     * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - (i+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = (j)*10;
			pVertices[triIndex*3+1].p.z = (i+1)*10;
			pVertices[triIndex*3+1].p.y = 0.01f;
	
			pVertices[triIndex*3+2].tu = (x - (j+1) * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - i     * 10) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = (j+1)*10;
			pVertices[triIndex*3+2].p.z = (i)*10;
			pVertices[triIndex*3+2].p.y = 0.01f;
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}
  
		}

	}


   vBuffer->Unlock();

}

//******************************************************************
void LandShadowManager::AddRealmShadow(float x, float y, float scale)
{

	if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
		return;

	// Prepare to fill the line vertex buffer, by locking it.
	SHADOWVERTEX *pVertices;
	if( FAILED( vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return;

	// figure out the extents of the shadow square
	float leastPosX = x - scale;
	float leastPosY = y - scale;
	float mostPosX  = x + scale;
	float mostPosY  = y + scale;

	int leastGridX = ro->GetGridX(leastPosX);
	if (leastGridX < 0)
		leastGridX = 0;
	int leastGridY = ro->GetGridY(leastPosY);
	if (leastGridY < 0)
		leastGridY = 0;
	int mostGridX  = ro->GetGridX(mostPosX);
	if (mostGridX > 126)
		mostGridX = 126;
	int mostGridY  = ro->GetGridY(mostPosY);
	if (mostGridY > 126)
		mostGridY = 126;

	// for each of the affected squares
	for (int i = leastGridY; i <= mostGridY; ++i)
	{
		for (int j = leastGridX; j <= mostGridX; ++j)
		{
			// copy the first land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - j * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - i * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = ro->GetXForPoint(j);
			pVertices[triIndex*3+0].p.z = ro->GetZForPoint(i);
			pVertices[triIndex*3+0].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+0].p.x,
						 				  pVertices[triIndex*3+0].p.z, NULL);
	
			pVertices[triIndex*3+1].tu = (x - (j+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - i     * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = ro->GetXForPoint(j+1);
			pVertices[triIndex*3+1].p.z = ro->GetZForPoint(i);
			pVertices[triIndex*3+1].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+1].p.x,
						 				  pVertices[triIndex*3+1].p.z, NULL);
	
			pVertices[triIndex*3+2].tu = (x - j     * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - (i+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = ro->GetXForPoint(j);
			pVertices[triIndex*3+2].p.z = ro->GetZForPoint(i+1);
			pVertices[triIndex*3+2].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+2].p.x,
						 				  pVertices[triIndex*3+2].p.z, NULL);
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}

			// copy the second land tri into the shadow tri buffer
			pVertices[triIndex*3+0].tu = (x - (j+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].tv = (y - (i+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+0].color = 0xffffffff;
			pVertices[triIndex*3+0].n.x = 0;
			pVertices[triIndex*3+0].n.y = 1;
			pVertices[triIndex*3+0].n.z = 0;
			pVertices[triIndex*3+0].p.x = ro->GetXForPoint(j+1);
			pVertices[triIndex*3+0].p.z = ro->GetZForPoint(i+1);
			pVertices[triIndex*3+0].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+0].p.x,
						 				  pVertices[triIndex*3+0].p.z, NULL);
	
			pVertices[triIndex*3+1].tu = (x - j     * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].tv = (y - (i+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+1].color = 0xffffffff;
			pVertices[triIndex*3+1].n.x = 0;
			pVertices[triIndex*3+1].n.y = 1;
			pVertices[triIndex*3+1].n.z = 0;
			pVertices[triIndex*3+1].p.x = ro->GetXForPoint(j);
			pVertices[triIndex*3+1].p.z = ro->GetZForPoint(i+1);
			pVertices[triIndex*3+1].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+1].p.x,
						 				  pVertices[triIndex*3+1].p.z, NULL);
	
			pVertices[triIndex*3+2].tu = (x - (j+1) * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].tv = (y - i     * ro->CellSize()) / (scale * 2.0f) + 0.5f;
			pVertices[triIndex*3+2].color = 0xffffffff;
			pVertices[triIndex*3+2].n.x = 0;
			pVertices[triIndex*3+2].n.y = 1;
			pVertices[triIndex*3+2].n.z = 0;
			pVertices[triIndex*3+2].p.x = ro->GetXForPoint(j+1);
			pVertices[triIndex*3+2].p.z = ro->GetZForPoint(i);
			pVertices[triIndex*3+2].p.y = 0.01f + 
				  ro->HeightAtPoint(pVertices[triIndex*3+2].p.x,
						 				  pVertices[triIndex*3+2].p.z, NULL);
	
			++triIndex;
			if (triIndex >= MAX_LAND_SHADOW_VERTS / 3)
			{
			   vBuffer->Unlock();
				return;
			}
  
		}

	}


   vBuffer->Unlock();

}

//******************************************************************
void LandShadowManager::DrawShadows(void)
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

	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_ZERO);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_ONE);
//	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCCOLOR);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_SRCCOLOR);
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CW );
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );

	puma->m_pd3dDevice->SetTexture( 0, bitmap );

   puma->m_pd3dDevice->SetVertexShader( D3DFVF_SHADOWVERTEX );
   puma->m_pd3dDevice->SetStreamSource( 0, vBuffer, sizeof(SHADOWVERTEX) );

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

*/

/*
//******************************************************************
void LandShadowManager::Draw(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar)
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
	pf->m_fTextScale = 15.0f;

//	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	if (healthBarValue > -1)
	{
	   matWorld._41 = position.x;
		matWorld._42 = position.y + 0.12f;
	   matWorld._43 = position.z;
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

		DrawBar(redBar, greenBar);

	   matWorld._41 = position.x;
		matWorld._42 = position.y;
	   matWorld._43 = position.z;
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	}
	if (text)
		pf->Render3DText(text, D3DFONT_CENTERED | D3DFONT_TWOSIDED | D3DFONT_FILTERED, color);
	pf->m_fTextScale = oldScale;
//	puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
}
*/

/*
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void LandShadowManager::DrawBar(LPDIRECT3DTEXTURE8 redBar, LPDIRECT3DTEXTURE8 greenBar)
{
   if( puma->m_pd3dDevice == NULL )
      return;

   LPDIRECT3DVERTEXBUFFER8 vBuffer;        // VertexBuffer for rendering text

   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( 6*2*sizeof(SHADOWVERTEX),
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
   puma->m_pd3dDevice->SetVertexShader( D3DFVF_SHADOWVERTEX );
   puma->m_pd3dDevice->SetStreamSource( 0, vBuffer, sizeof(SHADOWVERTEX) );

   // Set filter states
//   if( dwFlags & D3DRECORD_FILTERED )
   {
      puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
      puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   }

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
   SHADOWVERTEX* pVertices;
   DWORD       dwNumTriangles = 0L;
   vBuffer->Lock( 0, 0, (BYTE**)&pVertices, 0 );

   FLOAT tx1 = 0;
   FLOAT ty1 = 0;
   FLOAT tx2 = 1;
   FLOAT ty2 = 1;

   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty1 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
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

   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty1 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+w,y+0,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx2, ty2 );
   *pVertices++ = InitShadowVertex( D3DXVECTOR3(x+0,y+h,0), D3DXVECTOR3(0,0,-1), 0xffffffff, tx1, ty1 );
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
*/




/* end of file */



