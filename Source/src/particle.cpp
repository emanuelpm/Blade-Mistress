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

#include "particle.h"

// Helper function to stuff a FLOAT into a DWORD argument
//inline DWORD FtoDW( FLOAT f ) { return *((DWORD*)&f); }


//********************************************************************************
ParticleType::ParticleType(int maxParticles, char *textureName, float size)
{
	numOfParticles = maxParticles;
	particleSize = size;
	
    puma->m_pd3dDevice->CreateVertexBuffer( (numOfParticles + 1) * 
		sizeof(PARTICLEPOINTVERTEX), 0,	D3DFVF_PARTICLEPOINTVERTEX, D3DPOOL_MANAGED, &pVerts);

	
	D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, textureName,
							0,0,0,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0x00000000, NULL, NULL, &pTexture);
	
	particleArray = new Particle[numOfParticles];

	for (int i = 0; i < numOfParticles; ++i)
		particleArray[i].life = -1;  // empty;

	lastParticleUsed = 0;
}

//********************************************************************************
ParticleType::~ParticleType()
{
	delete[] particleArray;

	SAFE_RELEASE(pVerts);
	SAFE_RELEASE(pTexture);



}

//********************************************************************************
void ParticleType::SetEmissionPoint(D3DXVECTOR3 point)
{
	emitFromPointFlag = TRUE;
	boxLeast = point;

}

//********************************************************************************
void ParticleType::SetEmissionBox  (D3DXVECTOR3 least, D3DXVECTOR3 most)
{
	emitFromPointFlag = FALSE;
	boxLeast = least;
	boxMost  = most;
}

char testPTText[1024];

//********************************************************************************
void ParticleType::Emit(int amount, D3DXCOLOR color)
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
//			sprintf(testPTText,"%d %f %f %f", lastParticleUsed, boxLeast.x, boxLeast.y, boxLeast.z);
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
//			DebugOutput(testPTText);

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
void ParticleType::Tick(float timeDelta)
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
void ParticleType::PrepareToDraw(void)
{

    puma->m_pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );

    // Set the render states for using point sprites
    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );

    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(0.00f) );
    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(0.00f) );
    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(0.00f) );
    puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(1.00f) );

    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTEXF_LINEAR );
    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTEXF_LINEAR );
    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
    puma->m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

    puma->m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ONE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING,  FALSE );
    puma->m_pd3dDevice->SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    puma->m_pd3dDevice->SetRenderState( D3DRS_SHADEMODE, D3DSHADE_FLAT );
}

//********************************************************************************
void ParticleType::Draw(void)
{

	D3DXCOLOR tempColor;
	float *alpha, tempF;

   puma->m_pd3dDevice->SetRenderState( D3DRS_POINTSIZE,     FtoDW(particleSize) ); // size!

	// lock the vert buffer
	PARTICLEPOINTVERTEX *pMyVertices;

	pVerts->Lock( 0, 0, (unsigned char **)&pMyVertices, 0 );

	// fill in the blanks from particleArray
	for (int i = 0; i < numOfParticles; ++i)
	{
		if (particleArray[i].life >= 0)
		{
			pMyVertices[i].v     = particleArray[i].m_vPos;
			tempColor = particleArray[i].color;
			alpha  = (float *) &(tempColor);
//			alpha2 = (float *) &(particleArray[i].color);
			for (int j = 0; j < 4; ++j)
			{
				tempF = (particleArray[i].life / particleArray[i].startingLife) * alpha[j];
				alpha[j] = tempF;
			}
			pMyVertices[i].color = tempColor;
		}
		else
		{
			pMyVertices[i].v.x = -1000.0f;
			pMyVertices[i].v.z = -1000.0f;
			pMyVertices[i].v.y = -1000.0f;
			pMyVertices[i].color = 0;
		}
	}

	// null out the extra last one.
	pMyVertices[numOfParticles].v.x = -1000.0f;
	pMyVertices[numOfParticles].v.z = -1000.0f;
	pMyVertices[numOfParticles].v.y = -1000.0f;
	pMyVertices[numOfParticles].color = 0;

	// unlock the vert buffer
	pVerts->Unlock();

	// assert the texture
	int res = puma->m_pd3dDevice->SetTexture( 0, pTexture );
	assert(D3D_OK == res);

	// draw it!
	puma->m_pd3dDevice->SetStreamSource( 0, pVerts, sizeof(PARTICLEPOINTVERTEX) );
	puma->m_pd3dDevice->SetVertexShader( D3DFVF_PARTICLEPOINTVERTEX );

	for (int countBase = 0; countBase < numOfParticles;
		  countBase += puma->m_d3dCaps.MaxPrimitiveCount/4)
	{
		int numOut = numOfParticles - countBase;
		if (puma->m_d3dCaps.MaxPrimitiveCount < numOfParticles - countBase)
			numOut = puma->m_d3dCaps.MaxPrimitiveCount/4;
		puma->m_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST , countBase, numOut );
	}
}








/* end of file */