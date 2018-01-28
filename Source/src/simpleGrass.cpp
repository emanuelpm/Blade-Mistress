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
#include "GroundObjectTiles.h"

#include "simpleGrass.h"



//********************************************************************************
SimpleGrass::SimpleGrass(int maxTufts, char *textureName, float s)
{
	numOfTufts = maxTufts;
	size = s;
	
    puma->m_pd3dDevice->CreateVertexBuffer( (numOfTufts + 1) * 
		sizeof(SIMPLEGRASSPOINTVERTEX) * 6, 0,	
		D3DFVF_SIMPLEGRASSPOINTVERTEX, D3DPOOL_MANAGED, &pVerts);
	
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, textureName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xff000000, NULL, NULL, &pTexture);
	

	// lock the vert buffer
	SIMPLEGRASSPOINTVERTEX *pMyVertices;

	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );

	for (int i = 0; i < numOfTufts+1; ++i)
	{
		for (int vIndex = i * 6; vIndex < i * 6 + 6; ++vIndex)
		{
			pMyVertices[vIndex].v.x = 0;
			pMyVertices[vIndex].v.y = -1000;
			pMyVertices[vIndex].v.z = 0;
		}
	}

	// unlock the vert buffer
	pVerts->Unlock();

	curTuft = 0;

	lastPos.y = -1000.0f;
	lastPos.x = lastPos.z = 0;
	distTravelled = 0;
}

//********************************************************************************
SimpleGrass::~SimpleGrass()
{

	SAFE_RELEASE(pVerts);
	SAFE_RELEASE(pTexture);

}

extern void OpenTiledGroundInfoTexture(void);
extern void CloseTiledGroundInfoTexture(void);

//********************************************************************************
void SimpleGrass::AddTuft(GroundObjectTiles *ground, D3DXVECTOR3 lp, D3DXVECTOR3 curPos)
{

	OpenTiledGroundInfoTexture();

	D3DXVECTOR3 drawPos = curPos;

	float spread = 20;

	DWORD vertColor = 0x00ffffff;

	// lock the vert buffer
	SIMPLEGRASSPOINTVERTEX *pMyVertices;
	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );

	int times = 1;
	if (-1000 == lastPos.y)
	{
		times = numOfTufts/3;
		spread = 50;
	}
	else if (lastPos == curPos)
	{
		times = 0;
		distTravelled = 0;
	}
	else
	{
		drawPos.x += (curPos.x - lastPos.x) * 250.0f;
		drawPos.z += (curPos.z - lastPos.z) * 250.0f;

		distTravelled += Distance(curPos.x, curPos.z, lastPos.x, lastPos.z);
		times = 0;
		if (distTravelled > 9.0f)
		{
			drawPos = curPos;
			times = numOfTufts/3;
			spread = 50;
		}
		else
		{
			while (distTravelled > 0.6f / grassDensity * 3.0f)
			{
				distTravelled -= 0.6f / grassDensity * 3.0f;
				++times;
			}
		}
	}

	for (int i = 0; i < times; ++i)
	{
		int vIndex = curTuft * 6;

		D3DXVECTOR3 pos = drawPos;
		pos.x += rnd(-spread,spread);
		pos.z += rnd(-spread,spread);

		if (ground->GrassCanGoAt(pos.x, pos.z))
		{
			pos.y = ground->HeightAtPoint(pos.x, pos.z, NULL);

			// first poly
			pMyVertices[vIndex].v.x = pos.x - size;
			pMyVertices[vIndex].v.y = pos.y + 0;
			pMyVertices[vIndex].v.z = pos.z + 0;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 1;
			++vIndex;

			pMyVertices[vIndex].v.x = pos.x - size;
			pMyVertices[vIndex].v.y = pos.y + size*3;
			pMyVertices[vIndex].v.z = pos.z + 0;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = pos.x + size*2;
			pMyVertices[vIndex].v.y = pos.y + 0;
			pMyVertices[vIndex].v.z = pos.z + 0;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 1;
			pMyVertices[vIndex].tv = 1;
			++vIndex;

			// second poly
			pMyVertices[vIndex].v.x = pos.x + 0;
			pMyVertices[vIndex].v.y = pos.y + 0;
			pMyVertices[vIndex].v.z = pos.z - size;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 1;
			++vIndex;

			pMyVertices[vIndex].v.x = pos.x + 0;
			pMyVertices[vIndex].v.y = pos.y + size*3;
			pMyVertices[vIndex].v.z = pos.z - size;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = pos.x + 0;
			pMyVertices[vIndex].v.y = pos.y + 0;
			pMyVertices[vIndex].v.z = pos.z + size*2;
			pMyVertices[vIndex].color = vertColor;
			pMyVertices[vIndex].tu = 1;
			pMyVertices[vIndex].tv = 1;
			++vIndex;

			// go back and make previous tufts more solid

			int backIndex = vIndex - 6;
			for (int b = 1; b <= 4; ++b)
			{
				for (int j = 0; j < 6; ++j)
				{
					--backIndex;
					if (backIndex < 0)
						backIndex = numOfTufts * 6 - 1;

					pMyVertices[backIndex].color &= 0x00ffffff;
					pMyVertices[backIndex].color |= (255/4*b)<<24;
				}
			}

			++curTuft;

			if (curTuft >= numOfTufts)
				curTuft = 0;
		}
	}
	// unlock the vert buffer
	pVerts->Unlock();

	lastPos = curPos;

	CloseTiledGroundInfoTexture();

}

//********************************************************************************
void SimpleGrass::PrepareToDraw(void)
{
				
   puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

   // Set the render states for using point sprites
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
   puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	puma->m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND,           D3DBLEND_SRCALPHA);
	puma->m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND,          D3DBLEND_INVSRCALPHA);
   puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );

   puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_NONE );
  	
}

//********************************************************************************
void SimpleGrass::Draw(D3DXMATRIX matTrans)
{

	// assert the texture
	int res = puma->m_pd3dDevice->SetTexture( 0, pTexture );
	assert(D3D_OK == res);

	// draw it!
	puma->m_pd3dDevice->SetStreamSource( 0, pVerts, sizeof(SIMPLEGRASSPOINTVERTEX) );
	puma->m_pd3dDevice->SetVertexShader( D3DFVF_SIMPLEGRASSPOINTVERTEX );
	puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0, numOfTufts * 2 );
}



/* end of file */