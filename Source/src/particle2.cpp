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

#include "particle2.h"

// Helper function to stuff a FLOAT into a DWORD argument
//inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


//********************************************************************************
Particle2Type::Particle2Type(int maxParticles, char *textureName, float size)
{
	numOfParticles = maxParticles;
	particleSize = size * 2;// * 100;
	
    puma->m_pd3dDevice->CreateVertexBuffer( (numOfParticles + 1) * 
		sizeof(GLOW_QUADPOINTVERTEX) * 3, 0,	
		D3DFVF_GLOW_QUADPOINTVERTEX, D3DPOOL_MANAGED, &pVerts);
	
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, textureName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0x00000000, NULL, NULL, &pTexture);
	
	particleArray = new Particle[numOfParticles];

	for (int i = 0; i < numOfParticles; ++i)
		particleArray[i].life = -1;  // empty;

	lastParticleUsed = 0;
}

//********************************************************************************
Particle2Type::~Particle2Type()
{
	delete[] particleArray;

	SAFE_RELEASE(pVerts);
	SAFE_RELEASE(pTexture);



}

//********************************************************************************
void Particle2Type::SetEmissionPoint(D3DXVECTOR3 point)
{
	emitFromPointFlag = TRUE;
	boxLeast = point;

}

//********************************************************************************
void Particle2Type::SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most)
{
	emitFromPointFlag = FALSE;
	boxLeast = least;
	boxMost  = most;
}

char testPT2Text[1024];

//********************************************************************************
void Particle2Type::Emit(int amount, D3DXCOLOR color)
{
	int exit = FALSE;
	int last = lastParticleUsed;
	float temp1, temp2, temp3;
//	++lastParticleUsed;

//	DebugOutput("E1 ");

	for (int iter = 0; iter < amount; ++iter)
	{
//	DebugOutput("E2 ");

		while (!exit && particleArray[lastParticleUsed].life >= 0)
		{
			++lastParticleUsed;
			if (lastParticleUsed >= numOfParticles)
				lastParticleUsed = 0;

			if (last == lastParticleUsed)
				exit = TRUE;
		}

		if (!exit)
		{
//			DebugOutput("E3 ");
//			sprintf(testPT2Text,"%d %f %f %f", lastParticleUsed, boxLeast.x, boxLeast.y, boxLeast.z);
			// found an unused particle!
//			DebugOutput("E6 ");
  /*
			if (lastParticleUsed >= numOfParticles-1)
				DebugOutput("last ");

			if (lastParticleUsed >= numOfParticles)
				DebugOutput("over ");

			if (lastParticleUsed < 1)
				DebugOutput("zero ");

			if (lastParticleUsed < 0)
				DebugOutput("neg ");
	 */
//			DebugOutput(testPT2Text);

//			particleArray[lastParticleUsed].m_vPos.x = 0;
//			particleArray[lastParticleUsed].m_vPos.y = 0;
//			particleArray[lastParticleUsed].m_vPos.z = 0;

			particleArray[lastParticleUsed].m_vPos.x = boxLeast.x;
//			DebugOutput("E6.1 ");
			particleArray[lastParticleUsed].m_vPos.y = boxLeast.y;
//			DebugOutput("E6.2 ");
			particleArray[lastParticleUsed].m_vPos.z = boxLeast.z;
//			DebugOutput("E7 \n");



			particleArray[lastParticleUsed].life = rnd(lowLife, highLife);
//			DebugOutput("E4 ");

			particleArray[lastParticleUsed].startingLife = 
 										particleArray[lastParticleUsed].life;
//			DebugOutput("E5 ");


			particleArray[lastParticleUsed].color = color;

			temp1 = rnd(lowAngle, highAngle);
			temp2 = rnd(lowAzimuth, highAzimuth);
			temp3 = rnd(lowSpeed, highSpeed);

			particleArray[lastParticleUsed].m_vVel.x = sin(temp1) * temp3 * cos(temp2);
			particleArray[lastParticleUsed].m_vVel.z = cos(temp1) * temp3 * cos(temp2);
//			DebugOutput("E8 ");


			particleArray[lastParticleUsed].m_vVel.y = sin(temp2) * temp3;
		}

	}

//	DebugOutput("E9 ");

}

//********************************************************************************
void Particle2Type::Tick(float timeDelta)
{
	float *alpha;
	for (int i = 0; i < numOfParticles; ++i)
	{
		if (particleArray[i].life >= 0)
		{
			particleArray[i].life   -= 1.0f * timeDelta;
			
			alpha = (float *) &(particleArray[i].color);

			for (int j = 1; j < 4; ++j)
			{
				alpha[j] *= 0.99f;
			}
//			particleArray[i].color = D3DCOLOR_ARGB(al, 000, 255, al);

			particleArray[i].m_vPos.x += particleArray[i].m_vVel.x * timeDelta;
			particleArray[i].m_vPos.y += particleArray[i].m_vVel.y * timeDelta;
			particleArray[i].m_vPos.z += particleArray[i].m_vVel.z * timeDelta;
		}
	}

}

//********************************************************************************
void Particle2Type::PrepareToDraw(void)
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
void Particle2Type::Draw(D3DXMATRIX matTrans)
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

	D3DXMatrixTranslation( &localMat[0], 0,-0.5 * particleSize,-0.5 * particleSize);
	D3DXMatrixTranslation( &localMat[1], 0,-0.5 * particleSize, 1.5 * particleSize);
	D3DXMatrixTranslation( &localMat[2], 0, 1.5 * particleSize,-0.5 * particleSize);

	D3DXMatrixMultiply( &localMat[0], &localMat[0], &matTrans );
	D3DXMatrixMultiply( &localMat[1], &localMat[1], &matTrans );
	D3DXMatrixMultiply( &localMat[2], &localMat[2], &matTrans );


	D3DXCOLOR tempColor;
	float *alpha, tempF;

//   puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(particleSize) ); // size!

	// lock the vert buffer
	GLOW_QUADPOINTVERTEX *pMyVertices;

	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );

	// fill in the blanks from particleArray
	for (int i = 0; i < numOfParticles; ++i)
	{
		int vIndex = i * 3;
		if (particleArray[i].life >= 0)
		{
			tempColor = particleArray[i].color;
			alpha  = (float *) &(tempColor);
			for (int j = 0; j < 4; ++j)
			{
				tempF = (particleArray[i].life / particleArray[i].startingLife) * alpha[j];
				alpha[j] = tempF;
			}

			//matWorld._41 = curMob->spacePoint.location.x;
			//matWorld._42 = curMob->spacePoint.location.y;
		  //	matWorld._43 = curMob->spacePoint.location.z;

			pMyVertices[vIndex].v.x = particleArray[i].m_vPos.x + localMat[0]._43;
			pMyVertices[vIndex].v.y = particleArray[i].m_vPos.y + localMat[0]._42;
			pMyVertices[vIndex].v.z = particleArray[i].m_vPos.z - localMat[0]._41;
			pMyVertices[vIndex].color = tempColor;
			pMyVertices[vIndex].tu = 0;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = particleArray[i].m_vPos.x + localMat[1]._43;
			pMyVertices[vIndex].v.y = particleArray[i].m_vPos.y + localMat[1]._42;
			pMyVertices[vIndex].v.z = particleArray[i].m_vPos.z - localMat[1]._41;
			pMyVertices[vIndex].color = tempColor;
			pMyVertices[vIndex].tu = 1;
			pMyVertices[vIndex].tv = 0;
			++vIndex;

			pMyVertices[vIndex].v.x = particleArray[i].m_vPos.x + localMat[2]._43;
			pMyVertices[vIndex].v.y = particleArray[i].m_vPos.y + localMat[2]._42;
			pMyVertices[vIndex].v.z = particleArray[i].m_vPos.z - localMat[2]._41;
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
	pMyVertices[numOfParticles+0].v.x = -1000.0f;
	pMyVertices[numOfParticles+0].v.z = -1000.0f;
	pMyVertices[numOfParticles+0].v.y = -1000.0f;
	pMyVertices[numOfParticles+0].color = 0;
	pMyVertices[numOfParticles+0].tu = 0;
	pMyVertices[numOfParticles+0].tv = 0;

	pMyVertices[numOfParticles+1].v.x = -1000.0f;
	pMyVertices[numOfParticles+1].v.z = -1000.0f;
	pMyVertices[numOfParticles+1].v.y = -1000.0f;
	pMyVertices[numOfParticles+1].color = 0;
	pMyVertices[numOfParticles+1].tu = 0;
	pMyVertices[numOfParticles+1].tv = 0;

	pMyVertices[numOfParticles+2].v.x = -1000.0f;
	pMyVertices[numOfParticles+2].v.z = -1000.0f;
	pMyVertices[numOfParticles+2].v.y = -1000.0f;
	pMyVertices[numOfParticles+2].color = 0;
	pMyVertices[numOfParticles+2].tu = 0;
	pMyVertices[numOfParticles+2].tv = 0;

	// unlock the vert buffer
	pVerts->Unlock();

	// assert the texture
	int res = puma->m_pd3dDevice->SetTexture( 0, pTexture );
	assert(D3D_OK == res);

	// draw it!
	puma->m_pd3dDevice->SetStreamSource( 0, pVerts, sizeof(GLOW_QUADPOINTVERTEX) );
	puma->m_pd3dDevice->SetVertexShader( D3DFVF_GLOW_QUADPOINTVERTEX );

	for (int countBase = 0; countBase < numOfParticles;
		  countBase += puma->m_d3dCaps.MaxPrimitiveCount/4)
	{
		int numOut = numOfParticles - countBase;
		if (puma->m_d3dCaps.MaxPrimitiveCount/4 < numOfParticles - countBase)
			numOut = puma->m_d3dCaps.MaxPrimitiveCount/4;
		puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , countBase, numOut );
	}
}



/* end of file */