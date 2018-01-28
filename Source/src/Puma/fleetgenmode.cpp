
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "puma.h"
#include "pumamesh.h"
#include "GeneralUtils.h"
#include "fleetGenmode.h"

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
FleetGenMode::FleetGenMode(int doid, char *doname) : GameMode(doid,doname)
{
	partArray = NULL;
	for (int i = 0; i < 10; ++i)
		currentShip[i] = NULL;
	currentBase = NULL;
	texturePtrArray = NULL;
	numOfTextures   = 0;
	targetVerts     = 2000;
	shipAngle       = 0;
	currentTextureIndex = 0;
   ZeroMemory( &oldKeyState, sizeof(oldKeyState) );
}

//******************************************************************
FleetGenMode::~FleetGenMode()
{

}

//******************************************************************
int FleetGenMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int FleetGenMode::Activate(void) // do this when the mode becomes the forground mode.
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

	for (i = 0; i < numOfTextures; i++)
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

	// init camera position
	puma->pumaCamera->spacePoint.angle = D3DX_PI;
	puma->pumaCamera->spacePoint.azimuth = -.93f;
	puma->pumaCamera->spacePoint.location.x = -50.0f;
	puma->pumaCamera->spacePoint.location.y = -600.0f;
	puma->pumaCamera->spacePoint.location.z = 375.0f;

	puma->pumaCamera->m_fFarPlane = 900.0f; //>spacePoint.location.z = 375.0f;
	puma->pumaCamera->m_fNearPlane= 300.0f; //>spacePoint.location.z = 375.0f;

	return(0);
}

//******************************************************************
int FleetGenMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
//	SAFE_RELEASE(g_pVB);
	for (int i = 0; i < numOfTextures; i++)
	{
	   SAFE_RELEASE(texturePtrArray[i]);
	}

	delete[] texturePtrArray;



	delete[] partArray;

	for (i = 0; i < 10; ++i)
		SAFE_DELETE(currentShip[i]);

	SAFE_DELETE(currentBase);

	return(0);
}

//******************************************************************
int FleetGenMode::Tick(void)
{
   char  shipFileName[128];
   OPENFILENAME ofn;

	puma->StartRenderingFrame();

	shipAngle += 0.04f;

   // Render the vertex buffer contents
	/*
   puma->m_pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
   puma->m_pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
   puma->m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	*/
	puma->m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00000010 );

	D3DXMATRIX matWorld, mat2;
	for (int i = 0; i < 10; ++i)
	{
		int shipZ = i / 5;
		int shipX = i - shipZ * 5;
		float spacing = 140.0f;
		if (8 == i)
			spacing += 30.0f;
		if (9 == i)
			spacing += 60.0f;

		D3DXMatrixIdentity( &matWorld );
		D3DXMatrixRotationY(&matWorld, shipAngle);
	   D3DXMatrixIdentity( &mat2 );
		D3DXMatrixTranslation(&mat2, shipX * spacing - spacing*2.0f, 
			                          0, 
											  shipZ * spacing - spacing/2);
		D3DXMatrixMultiply(&mat2, &matWorld, &mat2);
		puma->m_pd3dDevice->SetTransform( D3DTS_WORLD, &mat2 );

		puma->m_pd3dDevice->SetTexture( 0, texturePtrArray[currentTextureIndex] );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		puma->m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

		if (currentShip[i])
			currentShip[i]->Draw(puma->m_pd3dDevice);
	}
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
   font->DrawText( titleRect, NULL, color, "Thom's Random Spaceship FleetGen V2.0" );

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
	sprintf(tempText, "Faces: %d", currentShip[0]->numOfVertices/3);
	RECT infoRect = {500,1,640,28};
   font->DrawText( infoRect, NULL, color, tempText );
	infoRect.top    += 14;
	infoRect.bottom += 14;
	sprintf(tempText, "Target Faces: %d", targetVerts/3);
   font->DrawText( infoRect, NULL, color, tempText );


//	HRESULT hr = font->DrawText("FleetGen string!", strlen("FleetGen string!"), &tempRect,	DT_LEFT, color);

	puma->HandleCamera();

	puma->FinishRenderingFrame();

	// check input
	puma->PumpInput();

	PumaInputEvent *pie = puma->TakeNextInputEvent();
	while (pie)
	{
		switch(pie->WhatAmI())
		{
		case PUMA_INPUT_KEYUP:
			if (32 == pie->asciiValue ) // if SPACE
				GenerateShip(targetVerts);

			if (115 == pie->asciiValue) // if s
			{
				memset(&ofn, 0, sizeof(OPENFILENAME));

				ofn.lStructSize = sizeof(OPENFILENAME);
				shipFileName[0] = 0;
				ofn.lpstrInitialDir = ".\\";
				ofn.lpstrFile= shipFileName;
				ofn.nMaxFile = sizeof(shipFileName);
				ofn.lpstrFilter = "PUMA MESH files\0*.pum\0\0";
				ofn.hwndOwner = (HWND) puma->m_hWnd;
		//	   ofn.Flags = OFN_NOCHANGEDIR;

  				if (GetSaveFileName(&ofn))
				{
//					fileName = shipFileName;

					int len = strlen(shipFileName);
					if (len > 0)
					{
						int tempLen = 0;
						
						while(tempLen < len && shipFileName[tempLen] != '.')
							tempLen++;
						
						char tempString[256];
						
						if (tempLen < len)
							shipFileName[tempLen] = 0;

						for (int j = 0; j < 10; ++j)
						{
							sprintf(tempString,"%s%02d.pum",shipFileName,j);
							currentShip[j]->Save(puma->m_pd3dDevice, tempString);
						}
					}
				}
			}
//				currentShip->Save(puma->m_pd3dDevice);

			if (108 == pie->asciiValue ) // if l
//				currentShip->Load(puma->m_pd3dDevice);

			if (45 == pie->asciiValue ) // if -
				targetVerts -= 200;

			if (43 == pie->asciiValue ) // if +
				targetVerts += 200;

			if (116 == pie->asciiValue ) // if t
			{
				currentTextureIndex++;
				if (currentTextureIndex >= numOfTextures)
					currentTextureIndex = 0;
			}

			if (48 <= pie->asciiValue && 57 >= pie->asciiValue) // if number key
			{
				int val = pie->asciiValue - 49;
				if (val < 0)
					val = 9;
				GenerateShip(targetVerts,val);
			}
			break;
/*
		case PUMA_INPUT_LMBDOWN:
		   fullWindow->Action(UIRECT_MOUSE_LDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_LMBUP:
		   fullWindow->Action(UIRECT_MOUSE_LUP, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBDOWN:
		   fullWindow->Action(UIRECT_MOUSE_RDOWN, pie->mouseX, pie->mouseY);
			break;
		case PUMA_INPUT_RMBUP:
		   fullWindow->Action(UIRECT_MOUSE_RUP, pie->mouseX, pie->mouseY);
			break;
			*/
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

	return(0);
}

//******************************************************************
void FleetGenMode::GenerateShip(int targetVerts, int reDoIndex)
{
	for (int i = 0; i < 10; ++i)
	{
		if (-1 == reDoIndex || i == reDoIndex)
			SAFE_DELETE(currentShip[i]);
	}

	// make common base
	if (-1 == reDoIndex)
	{
		SAFE_DELETE(currentBase);

		currentBase = new PumaMesh(0,"RANDOM_SHIP");

		for (int i = 0; i < 2; i++)
		{
			int partIndex = rand() % numOfMeshes;
			PumaMesh *pm = partArray[partIndex].MakeCopy(puma->m_pd3dDevice);
			pm->Scale(puma->m_pd3dDevice, rnd(0.5f,1.8f), rnd(0.5f,1.8f), rnd(0.5f,1.8f));
			pm->Shift(puma->m_pd3dDevice, rnd(-20.0f,20.0f), rnd(-5.0f,5.0f), rnd(-20.0f,20.0f));
			PumaMesh *pm2 = pm->MakeCopy(puma->m_pd3dDevice);
			pm2->Mirror(puma->m_pd3dDevice);

			currentBase->Assimilate(puma->m_pd3dDevice, pm);
			currentBase->Assimilate(puma->m_pd3dDevice, pm2);

			SAFE_DELETE(pm2);
			SAFE_DELETE(pm);
		}

//		currentBase->Center(puma->m_pd3dDevice);
	}


	for (i = 0; i < 10; ++i)
	{
		if (-1 == reDoIndex || i == reDoIndex)
		{
			currentShip[i] = new PumaMesh(0,"RANDOM_SHIP");
			currentShip[i]->Assimilate(puma->m_pd3dDevice, currentBase);

			int numOfParts = (rand() % 5) + 4;

			int verts = 0;
			int shipTargetVerts = targetVerts;
			if (8 == i)
				shipTargetVerts *= 2;
			if (9 == i)
				shipTargetVerts *= 4;

		//	for (int i = 0; i < numOfParts; i++)
			while (verts < shipTargetVerts)
			{
				int partIndex = rand() % numOfMeshes;
				PumaMesh *pm = partArray[partIndex].MakeCopy(puma->m_pd3dDevice);
				pm->Scale(puma->m_pd3dDevice, rnd(0.5f,1.8f), rnd(0.5f,1.8f), rnd(0.5f,1.8f));
				pm->Shift(puma->m_pd3dDevice, rnd(-20.0f,20.0f), rnd(-5.0f,5.0f), rnd(-20.0f,20.0f));
				PumaMesh *pm2 = pm->MakeCopy(puma->m_pd3dDevice);
				pm2->Mirror(puma->m_pd3dDevice);

				currentShip[i]->Assimilate(puma->m_pd3dDevice, pm);
				currentShip[i]->Assimilate(puma->m_pd3dDevice, pm2);

				SAFE_DELETE(pm2);
				SAFE_DELETE(pm);

				verts = currentShip[i]->numOfVertices;
			}

			currentShip[i]->Center(puma->m_pd3dDevice);
			if (8 == i)
				currentShip[i]->Scale(puma->m_pd3dDevice, 1.4f, 1.4f, 1.4f);
			if (9 == i)
				currentShip[i]->Scale(puma->m_pd3dDevice, 2.0f, 2.0f, 2.0f);
		}
	}


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



