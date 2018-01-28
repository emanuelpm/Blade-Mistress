
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "puma.h"
#include "pumamesh.h"
#include "generatormode.h"
#include "../helper/GeneralUtils.h"


/*
char partFileNames[8][64] = 
{
	{"dat\\part1.ASE"},
	{"dat\\part2.ASE"},
	{"dat\\part3.ASE"},
	{"dat\\part4.ASE"},
	{"dat\\part5.ASE"},
	{"dat\\part6.ASE"},
	{"dat\\part7.ASE"},
	{"dat\\part8.ASE"}
};
*/

//******************************************************************
GeneratorMode::GeneratorMode(int doid, char *doname) : GameMode(doid,doname)
{
	currentShip     = partArray = NULL;
	texturePtrArray = NULL;
	numOfTextures   = 0;
	targetVerts     = 2000;
	shipAngle       = 0;
	currentTextureIndex = 0;
   ZeroMemory( &oldKeyState, sizeof(oldKeyState) );
}

//******************************************************************
GeneratorMode::~GeneratorMode()
{

}

//******************************************************************
int GeneratorMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int GeneratorMode::Activate(void) // do this when the mode becomes the forground mode.
{

	FileNameList *fileNameList = GetNameList("parts\\*.ASE", "parts\\");

	numOfMeshes = fileNameList->numOfFiles;

	partArray = new PumaMesh[numOfMeshes];

	for (int i = 0; i < numOfMeshes; i++)
	{
		partArray[i].LoadFromASC(puma->m_pd3dDevice, fileNameList->nameList[i].name);
		partArray[i].LoadTexture(puma->m_pd3dDevice, "dat\\standardPartTexture.bmp");
	}

	delete fileNameList;

	fileNameList = GetNameList("Textures\\*.BMP", "textures\\");

	numOfTextures = fileNameList->numOfFiles;

	texturePtrArray = new LPDIRECT3DTEXTURE8[numOfTextures];

	for (int i = 0; i < numOfTextures; i++)
	{
	   D3DXCreateTextureFromFile( puma->m_pd3dDevice, 
			                        fileNameList->nameList[i].name, 
											&(texturePtrArray[i]) );
	}


	// Set up a white, directional light.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
   ZeroMemory( &light, sizeof(D3DLIGHT8) );
   light.Type      = D3DLIGHT_DIRECTIONAL ;
//   light.Type      = D3DLIGHT_POINT ;
   light.Diffuse.r  = 1.0f;
   light.Diffuse.g  = 1.0f;
   light.Diffuse.b  = 1.0f;
   light.Ambient.r  = 1.0f;
   light.Ambient.g  = 1.0f;
   light.Ambient.b  = 1.0f;
   vecDir = D3DXVECTOR3(1.0f, -1.0f, 0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
   light.Position = D3DXVECTOR3(1000.0f, 0.0f, 0.0f);
   light.Range      = 10000.0f;
   light.Falloff   = 1.0f;
   light.Attenuation0 = 1.0f;
   puma->m_pd3dDevice->SetLight( 0, &light );
   puma->m_pd3dDevice->LightEnable( 0, TRUE );
   puma->m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   // Finally, turn on some ambient light.
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(48,48,48,255)  );

	GenerateShip(targetVerts);

	return(0);
}

//******************************************************************
int GeneratorMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
//	SAFE_RELEASE(g_pVB);
	for (int i = 0; i < numOfTextures; i++)
	{
	   SAFE_RELEASE(texturePtrArray[i]);
	}

	delete[] texturePtrArray;



	delete[] partArray;
	SAFE_DELETE(currentShip);

	return(0);
}

//******************************************************************
int GeneratorMode::Tick(void)
{
	puma->StartRenderingFrame();

	shipAngle += 0.04f;

   // Render the vertex buffer contents
	/*
   puma->m_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
   puma->m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
   puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	*/
   puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000010 );

   D3DXMATRIX matWorld;
   D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationY(&matWorld, shipAngle);
   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	puma->m_pd3dDevice->SetTexture( 0, texturePtrArray[currentTextureIndex] );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	if (currentShip)
		currentShip->Draw(puma->m_pd3dDevice);

//   D3DXMatrixIdentity( &matWorld );
//   puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	CD3DFont *font = puma->GetDXFont(0);

	RECT tempRect;
	tempRect.left   = 10;
	tempRect.top   = 10;
	tempRect.right  = 200;
	tempRect.bottom = 40;

	D3DCOLOR color;

	color = D3DCOLOR_RGBA(255,100,100,255);
	RECT titleRect = {1,1,500,28};
   font->DrawText( titleRect, NULL, color, "Thom's Random Spaceship Generator V2.0" );

	color = D3DCOLOR_RGBA(200,200,200,255);
	titleRect.top    += 24;
	titleRect.bottom += 24;
   font->DrawText( titleRect, NULL, color, "SPACE makes a new ship, S saves it, and L loads it again." );
	titleRect.top    += 14;
	titleRect.bottom += 14;
   font->DrawText( titleRect, NULL, color, "The keypad controls the camera. T toggles thru the textures. < and > adjust verts." );
	titleRect.top    += 14;
	titleRect.bottom += 14;
   font->DrawText( titleRect, NULL, color, "This tool automatically loads all ASEs in the PARTS directory," );
	titleRect.top    += 14;
	titleRect.bottom += 14;
   font->DrawText( titleRect, NULL, color, "and all BMPs in the TEXTURES directory, so add your own!" );

	color = D3DCOLOR_RGBA(255,0,0,255);
	titleRect.top    += 24;
	titleRect.bottom += 24;
   font->DrawText( titleRect, NULL, color, "Share your ships with me!" );

	color = D3DCOLOR_RGBA(200,200,100,255);
	char tempText[128];
	sprintf(tempText, "Faces: %d", currentShip->numOfVertices/3);
	RECT infoRect = {500,1,640,28};
   font->DrawText( infoRect, NULL, color, tempText );
	infoRect.top    += 14;
	infoRect.bottom += 14;
	sprintf(tempText, "Target Faces: %d", targetVerts/3);
   font->DrawText( infoRect, NULL, color, tempText );


//	HRESULT hr = font->DrawText("Generator string!", strlen("Generator string!"), &tempRect,	DT_LEFT, color);

	puma->HandleCamera();

	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	//********** check keyboard
   BYTE    diks[256];   // DirectInput keyboard state buffer 
//   int     i;

   if( NULL == puma->pKeyboard ) 
      return(0);
   
   // Get the input's device state, and put the state in dims
   ZeroMemory( &diks, sizeof(diks) );
   
   HRESULT hr = puma->pKeyboard->GetDeviceState( sizeof(diks), &diks );
   if( FAILED(hr) ) 
   {
      // DirectInput may be telling us that the input stream has been
      // interrupted.  We aren't tracking any state between polls, so
      // we don't have any special reset that needs to be done.
      // We just re-acquire and try again.
      
      // If input is lost then acquire and keep trying 
      hr = puma->pKeyboard->Acquire();
      while( hr == DIERR_INPUTLOST ) 
         hr = puma->pKeyboard->Acquire();
	  
      // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
      // may occur when the app is minimized or in the process of 
      // switching, so just try again later 
      return(0); 
   }

	if ((oldKeyState[57] & 0x80) && !(diks[57] & 0x80) )
	{
		GenerateShip(targetVerts);
	}
	if ((oldKeyState[31] & 0x80) && !(diks[31] & 0x80) )
	{
		currentShip->Save(puma->m_pd3dDevice);
	}
	if ((oldKeyState[38] & 0x80) && !(diks[38] & 0x80) )
	{
		currentShip->Load(puma->m_pd3dDevice);
	}
	if ((oldKeyState[51] & 0x80) && !(diks[51] & 0x80) )
	{
		targetVerts -= 200;
	}
	if ((oldKeyState[52] & 0x80) && !(diks[52] & 0x80) )
	{
		targetVerts += 200;
	}
	if ((oldKeyState[20] & 0x80) && !(diks[20] & 0x80) )
	{
		currentTextureIndex++;
		if (currentTextureIndex >= numOfTextures)
			currentTextureIndex = 0;
	}




	memcpy(oldKeyState,diks,256 * sizeof(BYTE));

	return(0);
}

//******************************************************************
void GeneratorMode::GenerateShip(int targetVerts)
{
	SAFE_DELETE(currentShip);

	currentShip = new PumaMesh(0,"RANDOM_SHIP");

	int numOfParts = (rand() % 5) + 4;

	int verts = 0;
//	for (int i = 0; i < numOfParts; i++)
	while (verts < targetVerts)
	{
		int partIndex = rand() % numOfMeshes;
		PumaMesh *pm = partArray[partIndex].MakeCopy(puma->m_pd3dDevice);
		pm->Scale(puma->m_pd3dDevice, rnd(0.5f,1.8f), rnd(0.5f,1.8f), rnd(0.5f,1.8f));
		pm->Shift(puma->m_pd3dDevice, rnd(-20.0f,20.0f), rnd(-5.0f,5.0f), rnd(-20.0f,20.0f));
		PumaMesh *pm2 = pm->MakeCopy(puma->m_pd3dDevice);
		pm2->Mirror(puma->m_pd3dDevice);

		currentShip->Assimilate(puma->m_pd3dDevice, pm);
		currentShip->Assimilate(puma->m_pd3dDevice, pm2);

		SAFE_DELETE(pm2);
		SAFE_DELETE(pm);

		verts = currentShip->numOfVertices;
	}

	currentShip->Center(puma->m_pd3dDevice);

//	currentShip->RebuildNormals(puma->m_pd3dDevice);

/*
	currentShip->mtrl.Diffuse.r = 0.5f;
	currentShip->mtrl.Diffuse.g = 0.5f;
	currentShip->mtrl.Diffuse.b = 0.5f;
	currentShip->mtrl.Diffuse.a = 0.5f;
	currentShip->mtrl.Ambient.r = 0.1f;
	currentShip->mtrl.Ambient.g = 0.1f;
	currentShip->mtrl.Ambient.b = 0.1f;
	currentShip->mtrl.Ambient.a = 0.1f;
*/
}


/* end of file */



