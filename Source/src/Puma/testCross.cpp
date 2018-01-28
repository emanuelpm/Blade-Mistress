//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "testCross.h"
#include "pumaAnim.h"
#include "GeneralUtils.h"
#include "d3dutil.h"



//***************************************************************************************
TestCross::TestCross(void) : DataObject(0,"TEST_CROSS")
{

	vertexBufferPtr = NULL; // Buffer to hold vertices
	int numOfVertices = 4 * 3;

   // Create the vertex buffer.
   if( FAILED( puma->m_pd3dDevice->CreateVertexBuffer( numOfVertices*sizeof(PUMAMESHVERTEXSTRUCT),
                             0, PUMAMESH_VERTEXDESC,
                             D3DPOOL_MANAGED, &vertexBufferPtr ) ) )
   {
     return;
   }

   // Fill the vertex buffer. We are setting the tu and tv texture
   // coordinates, which range from 0.0 to 1.0
   PUMAMESHVERTEXSTRUCT* pVertices;
   if( FAILED( vertexBufferPtr->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
     return;

	int tri = 0;

   pVertices[3*tri+0].position = D3DXVECTOR3( 1.0f, 0.0f, 1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0x7fffff00;
   pVertices[3*tri+0].tu      = 0.0f;
   pVertices[3*tri+0].tv      = 0.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0x7fffff00;
   pVertices[3*tri+1].tu      = 0.0f;
   pVertices[3*tri+1].tv      = 1.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0x7fffff00;
   pVertices[3*tri+2].tu      = 1.0f;
   pVertices[3*tri+2].tv      = 0.0f;

	tri = 1;

   pVertices[3*tri+0].position = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0x7fffff00;
   pVertices[3*tri+0].tu      = 1.0f;
   pVertices[3*tri+0].tv      = 1.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0x7fffff00;
   pVertices[3*tri+1].tu      = 1.0f;
   pVertices[3*tri+1].tv      = 0.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0x7fffff00;
   pVertices[3*tri+2].tu      = 0.0f;
   pVertices[3*tri+2].tv      = 1.0f;

	tri = 2;

   pVertices[3*tri+0].position = D3DXVECTOR3( 1.0f, 0.0f, 1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0x7fffff00;
   pVertices[3*tri+0].tu      = 0.0f;
   pVertices[3*tri+0].tv      = 0.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0x7fffff00;
   pVertices[3*tri+1].tu      = 0.0f;
   pVertices[3*tri+1].tv      = 1.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0x7fffff00;
   pVertices[3*tri+2].tu      = 1.0f;
   pVertices[3*tri+2].tv      = 0.0f;

	tri = 3;

   pVertices[3*tri+0].position = D3DXVECTOR3( -1.0f, 0.0f, -1.0f );
   pVertices[3*tri+0].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+0].color   = 0x7fffff00;
   pVertices[3*tri+0].tu      = 1.0f;
   pVertices[3*tri+0].tv      = 1.0f;

   pVertices[3*tri+1].position = D3DXVECTOR3( -1.0f, 0.0f, 1.0f );
   pVertices[3*tri+1].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+1].color   = 0x7fffff00;
   pVertices[3*tri+1].tu      = 1.0f;
   pVertices[3*tri+1].tv      = 0.0f;

   pVertices[3*tri+2].position = D3DXVECTOR3( 1.0f, 0.0f, -1.0f );
   pVertices[3*tri+2].normal   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
   pVertices[3*tri+2].color   = 0x7fffff00;
   pVertices[3*tri+2].tu      = 0.0f;
   pVertices[3*tri+2].tv      = 1.0f;

   vertexBufferPtr->Unlock();

}

//***************************************************************************************
TestCross::~TestCross()
{

   SAFE_RELEASE(vertexBufferPtr);

}



//***************************************************************************************
void TestCross::Draw(LPDIRECT3DDEVICE8 pd3dDevice,D3DXVECTOR3 position, float width, float y)
{

	pd3dDevice->SetTexture( 0, NULL );
   pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
   pd3dDevice->SetRenderState( D3DRS_ALPHATESTENABLE,  TRUE );
   pd3dDevice->SetRenderState( D3DRS_ALPHAREF,         0x08 );
   pd3dDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );

   // Fill the vertex buffer. We are setting the tu and tv texture
   // coordinates, which range from 0.0 to 1.0
   PUMAMESHVERTEXSTRUCT* pVertices;
   if( FAILED( vertexBufferPtr->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
     return;

	int tri = 0;

   pVertices[3*tri+0].position = position;
   pVertices[3*tri+0].position.y += y;
   pVertices[3*tri+0].position.x += width;

   pVertices[3*tri+1].position = position;
   pVertices[3*tri+1].position.y += 0;
   pVertices[3*tri+1].position.x += width;

   pVertices[3*tri+2].position = position;
   pVertices[3*tri+2].position.y += y;
   pVertices[3*tri+2].position.x -= width;

	++tri;

   pVertices[3*tri+0].position = position;
   pVertices[3*tri+0].position.y += 0;
   pVertices[3*tri+0].position.x += width;

   pVertices[3*tri+1].position = position;
   pVertices[3*tri+1].position.y += y;
   pVertices[3*tri+1].position.x -= width;

   pVertices[3*tri+2].position = position;
   pVertices[3*tri+2].position.y += 0;
   pVertices[3*tri+2].position.x -= width;

	++tri;

   pVertices[3*tri+0].position = position;
   pVertices[3*tri+0].position.y += y;
   pVertices[3*tri+0].position.z += width;

   pVertices[3*tri+1].position = position;
   pVertices[3*tri+1].position.y += 0;
   pVertices[3*tri+1].position.z += width;

   pVertices[3*tri+2].position = position;
   pVertices[3*tri+2].position.y += y;
   pVertices[3*tri+2].position.z -= width;

	++tri;

   pVertices[3*tri+0].position = position;
   pVertices[3*tri+0].position.y += 0;
   pVertices[3*tri+0].position.z += width;

   pVertices[3*tri+1].position = position;
   pVertices[3*tri+1].position.y += y;
   pVertices[3*tri+1].position.z -= width;

   pVertices[3*tri+2].position = position;
   pVertices[3*tri+2].position.y += 0;
   pVertices[3*tri+2].position.z -= width;


   vertexBufferPtr->Unlock();

   // Render the vertex buffer contents
   pd3dDevice->SetStreamSource( 0, vertexBufferPtr, sizeof(PUMAMESHVERTEXSTRUCT) );
   pd3dDevice->SetVertexShader( PUMAMESH_VERTEXDESC );
   pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST , 0, 4 );

}



/* end of file */
