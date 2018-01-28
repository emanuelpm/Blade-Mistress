//-----------------------------------------------------------------------------
// File: PointSprites.cpp
//-----------------------------------------------------------------------------
#define STRICT
#include <math.h>
#include <stdio.h>
#include <D3DX8.h>
//#include "D3DApp.h"
#include ".\puma\D3DFont.h"
#include ".\puma\D3DUtil.h"
#include ".\puma\DXUtil.h"
#include ".\puma\puma.h"
#include "BBO.h"

#include ".\helper\GeneralUtils.h"

#include "glowQuad.h"

// Helper function to stuff a FLOAT into a DWORD argument
//inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }

const int GLOWQUAD_DECAY = 50;

//********************************************************************************
GlowQuadType::GlowQuadType(int maxQuads)
{
	numOfQuads = maxQuads;
	
   puma->m_pd3dDevice->CreateVertexBuffer( (numOfQuads + 1) * 
		sizeof(COLORQUADPOINTVERTEX) * 6, 0,	
		D3DFVF_COLORQUADPOINTVERTEX, D3DPOOL_MANAGED, &pVerts);
	
	nextQuad = 0;
	pMyVertices = NULL;
}

//********************************************************************************
GlowQuadType::~GlowQuadType()
{
	SAFE_RELEASE(pVerts);
}

//********************************************************************************
void GlowQuadType::StartAddingQuads(void)
{
	assert(!pMyVertices);

	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );
	
}

//********************************************************************************
void GlowQuadType::FinishAddingQuads(void)
{
	assert(pMyVertices);

	pVerts->Unlock();
	pMyVertices = NULL;
	
}

//********************************************************************************
void GlowQuadType::AddQuad(ColorQuad &quad)
{
	if (0 == quad.vec[2].x && 0 == quad.vec[2].y && 0 == quad.vec[2].z)
		return;

	int exit = FALSE;
	int last = nextQuad;
	float temp1, temp2, temp3;

	while (!exit && pMyVertices[nextQuad*6].v.x != -1000.0f)
	{
		++nextQuad;
		if (nextQuad >= numOfQuads)
			nextQuad = 0;

		if (last == nextQuad)
			exit = TRUE;
	}

	if (!exit)
	{
		D3DCOLOR color  = D3DCOLOR_ARGB(255, quad.r, quad.g, quad.b);
		D3DCOLOR color2 = D3DCOLOR_ARGB(255, quad.r-GLOWQUAD_DECAY, quad.g-GLOWQUAD_DECAY, quad.b-GLOWQUAD_DECAY);

		int vIndex = nextQuad*6;
		pMyVertices[vIndex  ].v     = quad.vec[0];
		pMyVertices[vIndex++].color = color;
		pMyVertices[vIndex  ].v     = quad.vec[1];
		pMyVertices[vIndex++].color = color;
		pMyVertices[vIndex  ].v     = quad.vec[2];
		pMyVertices[vIndex++].color = color2;

		pMyVertices[vIndex  ].v     = quad.vec[1];
		pMyVertices[vIndex++].color = color;
		pMyVertices[vIndex  ].v     = quad.vec[3];
		pMyVertices[vIndex++].color = color2;
		pMyVertices[vIndex  ].v     = quad.vec[2];
		pMyVertices[vIndex++].color = color2;
	}


}

//********************************************************************************
void GlowQuadType::Tick(void)
{
	float *alpha;
	long al;

	for (int i = 0; i < numOfQuads; ++i)
	{

		if ( pMyVertices[i*6].v.x != -1000.0f)
		{
			int cleanup = FALSE;

			for (int index = i*6; index < (i+1)*6; ++index)
			{
				int b =  pMyVertices[index].color        & 0xff;
				int g = (pMyVertices[index].color >>  8) & 0xff;
				int r = (pMyVertices[index].color >> 16) & 0xff;

				r -= GLOWQUAD_DECAY;
				if (r < 0)
					r = 0;
				g -= GLOWQUAD_DECAY;
				if (g < 0)
					g = 0;
				b -= GLOWQUAD_DECAY;
				if (b < 0)
					b = 0;
				if (r <= 0 && g <= 0 && b <= 0 && i*6 == index)
				{
					cleanup = TRUE;
					index += 6; // end the loop
				}
				else
				{
					pMyVertices[index].color = D3DCOLOR_ARGB(255, r,g,b);
				}
			}

			if (cleanup)
			{
				for (int index = i*6; index < (i+1)*6; ++index)
				{
					pMyVertices[index].v.x = -1000.0f;
					pMyVertices[index].v.z = -1000.0f;
					pMyVertices[index].v.y = -1000.0f;
				}
			}
		}
	}

}

//********************************************************************************
void GlowQuadType::PrepareToDraw(void)
{
				
    puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    // Set the render states for using point sprites
//    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
//    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    puma->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
  	 
}

//********************************************************************************
void GlowQuadType::Draw(D3DXMATRIX matTrans)
{

   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
	// draw it!
	puma->m_pd3dDevice->SetStreamSource( 0, pVerts, sizeof(COLORQUADPOINTVERTEX) );
	puma->m_pd3dDevice->SetVertexShader( D3DFVF_COLORQUADPOINTVERTEX );

	int res = puma->m_pd3dDevice->SetTexture( 0, NULL );
	assert(D3D_OK == res);

	for (int countBase = 0; countBase < numOfQuads*2;
		  countBase += puma->m_d3dCaps.MaxPrimitiveCount/4)
	{
		int numOut = numOfQuads*2 - countBase;
		if (puma->m_d3dCaps.MaxPrimitiveCount/4 < numOfQuads*2 - countBase)
			numOut = puma->m_d3dCaps.MaxPrimitiveCount/4;
		puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
	}
}



/* end of file */