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

#include "pointGlow.h"

// Helper function to stuff a FLOAT into a DWORD argument
//inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


//********************************************************************************
PointGlowSet::PointGlowSet(int maxPointGlows, char *textureName, float size)
{
	numOfPointGlows = maxPointGlows;
	pointGlowSize = size * 2;// * 100;
	
   puma->m_pd3dDevice->CreateVertexBuffer( (numOfPointGlows + 1) * 
		sizeof(GLOW_QUADPOINTVERTEX) * 3, 0,	
		D3DFVF_GLOW_QUADPOINTVERTEX, D3DPOOL_MANAGED, &pVerts);
	
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, textureName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0x00000000, NULL, NULL, &pTexture);
	
	pointGlowArray = new PointGlow[numOfPointGlows];

	for (int i = 0; i < numOfPointGlows; ++i)
		pointGlowArray[i].used = FALSE;

	lastPointGlowUsed = 0;
}

//********************************************************************************
PointGlowSet::~PointGlowSet()
{
	delete[] pointGlowArray;

	SAFE_RELEASE(pVerts);
	SAFE_RELEASE(pTexture);



}


//char testPT2Text[1024];

//********************************************************************************
void PointGlowSet::AddAGlow(D3DXVECTOR3 point, D3DXCOLOR color)
{
	int exit = FALSE;
	int last = lastPointGlowUsed;
//	++lastPointGlowUsed;

//	DebugOutput("E1 ");

	{
//	DebugOutput("E2 ");

		while (!exit && pointGlowArray[lastPointGlowUsed].used)
		{
			++lastPointGlowUsed;
			if (lastPointGlowUsed >= numOfPointGlows)
				lastPointGlowUsed = 0;

			if (last == lastPointGlowUsed)
				exit = TRUE;
		}

		if (!exit)
		{
//			DebugOutput("E3 ");
//			sprintf(testPT2Text,"%d %f %f %f", lastPointGlowUsed, boxLeast.x, boxLeast.y, boxLeast.z);
			// found an unused pointGlow!
//			DebugOutput("E6 ");
  /*
			if (lastPointGlowUsed >= numOfPointGlows-1)
				DebugOutput("last ");

			if (lastPointGlowUsed >= numOfPointGlows)
				DebugOutput("over ");

			if (lastPointGlowUsed < 1)
				DebugOutput("zero ");

			if (lastPointGlowUsed < 0)
				DebugOutput("neg ");
	 */
//			DebugOutput(testPT2Text);

//			pointGlowArray[lastPointGlowUsed].m_vPos.x = 0;
//			pointGlowArray[lastPointGlowUsed].m_vPos.y = 0;
//			pointGlowArray[lastPointGlowUsed].m_vPos.z = 0;

			pointGlowArray[lastPointGlowUsed].m_vPos = point;
			pointGlowArray[lastPointGlowUsed].color = color;
			pointGlowArray[lastPointGlowUsed].used = TRUE;
		}

	}

//	DebugOutput("E9 ");

}

//********************************************************************************
void PointGlowSet::PrepareToDraw(void)
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
    puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CW );
    puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
  	 
}

//********************************************************************************
void PointGlowSet::Draw(D3DXMATRIX matTrans)
{
//	camAngle *= 1;
//	camAngle = NormalizeAngle(camAngle);

//	float val = sin(camAngle);
//	val = cos(camAngle);
//	camAngle *= 2;
//	camAngle += D3DX_PI;
   D3DXMatrixInverse( &matTrans, NULL, &matTrans );


	D3DXMATRIX localMat[3];
	matTrans._41 = 0;
	matTrans._42 = 0;
	matTrans._43 = 0;

	D3DXMatrixTranslation( &localMat[0], 0,-0.5 * pointGlowSize,-0.5 * pointGlowSize);
	D3DXMatrixTranslation( &localMat[1], 0,-0.5 * pointGlowSize, 1.5 * pointGlowSize);
	D3DXMatrixTranslation( &localMat[2], 0, 1.5 * pointGlowSize,-0.5 * pointGlowSize);

	D3DXMatrixMultiply( &localMat[0], &localMat[0], &matTrans );
	D3DXMatrixMultiply( &localMat[1], &localMat[1], &matTrans );
	D3DXMatrixMultiply( &localMat[2], &localMat[2], &matTrans );


	D3DXCOLOR tempColor;
	float *alpha, tempF;

//   puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(pointGlowSize) ); // size!

	// lock the vert buffer
	GLOW_QUADPOINTVERTEX *pMyVertices;

	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );

	// fill in the blanks from pointGlowArray
	for (int i = 0; i < numOfPointGlows; ++i)
	{
		int vIndex = i * 3;
		if (pointGlowArray[i].used)
		{
			tempColor = pointGlowArray[i].color;
			alpha  = (float *) &(tempColor);
			for (int j = 0; j < 4; ++j)
			{
				tempF = alpha[j];
				alpha[j] = tempF;
			}

			//matWorld._41 = curMob->spacePoint.location.x;
			//matWorld._42 = curMob->spacePoint.location.y;
		  //	matWorld._43 = curMob->spacePoint.location.z;

			pMyVertices[vIndex].v.x = pointGlowArray[i].m_vPos.x + localMat[0]._43;
			pMyVertices[vIndex].v.y = pointGlowArray[i].m_vPos.y + localMat[0]._42;
			pMyVertices[vIndex].v.z = pointGlowArray[i].m_vPos.z - localMat[0]._41;
			pMyVertices[vIndex].color = tempColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = pointGlowArray[i].m_vPos.x + localMat[1]._43;
			pMyVertices[vIndex].v.y = pointGlowArray[i].m_vPos.y + localMat[1]._42;
			pMyVertices[vIndex].v.z = pointGlowArray[i].m_vPos.z - localMat[1]._41;
			pMyVertices[vIndex].color = tempColor;
			pMyVertices[vIndex].tu = 1;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = pointGlowArray[i].m_vPos.x + localMat[2]._43;
			pMyVertices[vIndex].v.y = pointGlowArray[i].m_vPos.y + localMat[2]._42;
			pMyVertices[vIndex].v.z = pointGlowArray[i].m_vPos.z - localMat[2]._41;
			pMyVertices[vIndex].color = tempColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 1;
		}
		else
		{
			pMyVertices[vIndex].v.x = -1000.0f;
			pMyVertices[vIndex].v.z = -1000.0f;
			pMyVertices[vIndex].v.y = -1000.0f;
			pMyVertices[vIndex].color = 0;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;
			pMyVertices[vIndex].v.x = -1000.0f;
			pMyVertices[vIndex].v.z = -1000.0f;
			pMyVertices[vIndex].v.y = -1000.0f;
			pMyVertices[vIndex].color = 0;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;
			pMyVertices[vIndex].v.x = -1000.0f;
			pMyVertices[vIndex].v.z = -1000.0f;
			pMyVertices[vIndex].v.y = -1000.0f;
			pMyVertices[vIndex].color = 0;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
		}
	}

	// null out the extra last one.
	pMyVertices[numOfPointGlows+0].v.x = -1000.0f;
	pMyVertices[numOfPointGlows+0].v.z = -1000.0f;
	pMyVertices[numOfPointGlows+0].v.y = -1000.0f;
	pMyVertices[numOfPointGlows+0].color = 0;
	pMyVertices[numOfPointGlows+0].tu = 0;
	pMyVertices[numOfPointGlows+0].tv = 0;

	pMyVertices[numOfPointGlows+1].v.x = -1000.0f;
	pMyVertices[numOfPointGlows+1].v.z = -1000.0f;
	pMyVertices[numOfPointGlows+1].v.y = -1000.0f;
	pMyVertices[numOfPointGlows+1].color = 0;
	pMyVertices[numOfPointGlows+1].tu = 0;
	pMyVertices[numOfPointGlows+1].tv = 0;

	pMyVertices[numOfPointGlows+2].v.x = -1000.0f;
	pMyVertices[numOfPointGlows+2].v.z = -1000.0f;
	pMyVertices[numOfPointGlows+2].v.y = -1000.0f;
	pMyVertices[numOfPointGlows+2].color = 0;
	pMyVertices[numOfPointGlows+2].tu = 0;
	pMyVertices[numOfPointGlows+2].tv = 0;

	// unlock the vert buffer
	pVerts->Unlock();

	// assert the texture
	int res = puma->m_pd3dDevice->SetTexture( 0, pTexture );
	assert(D3D_OK == res);

	// draw it!
	puma->m_pd3dDevice->SetStreamSource( 0, pVerts, sizeof(GLOW_QUADPOINTVERTEX) );
	puma->m_pd3dDevice->SetVertexShader( D3DFVF_GLOW_QUADPOINTVERTEX );

	for (int countBase = 0; countBase < numOfPointGlows;
		  countBase += puma->m_d3dCaps.MaxPrimitiveCount/4)
	{
		int numOut = numOfPointGlows - countBase;
		if (puma->m_d3dCaps.MaxPrimitiveCount/4 < numOfPointGlows - countBase)
			numOut = puma->m_d3dCaps.MaxPrimitiveCount/4;
		puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
	}

	for (int i = 0; i < numOfPointGlows; ++i)
		pointGlowArray[i].used = FALSE;

	lastPointGlowUsed = 0;
}



/* end of file */