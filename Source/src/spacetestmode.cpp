
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "puma.h"
#include "pumamesh.h"
#include "spaceTestmode.h"
#include "GeneralUtils.h"
#include "d3dutil.h"

#include "UIR_TextButton.h"
#include "UIR_TextBox.h"
#include "UIR_ScrollWindow.h"

#include "AFOfleet.h"
#include "AFOplanet.h"
#include "meshRepository.h"
#include "FlockPoints.h"

#include "plasmaTexture.h"
#include "pointSprites.h"


int gCellViewDistance, gGalaxySize, gCellWidth;

MeshRepository *meshRep;
//AFOFleet *fleet;
FlockSet *flockSet;
//PlasmaTexture *plasma;
//AFOPlanet *planet;
DoublyLinkedList planets, fleets;
PumaMesh *sphere, *atmosphere, *shipStandin;

LPDIRECT3DTEXTURE8 m_pParticleTexture; // Our particle texture


CParticleSystem * m_pParticleSystem;

// A structure for our line type.
struct PUMALINESTRUCT
{
    D3DXVECTOR3 position; // The position
    D3DCOLOR    color;    // The color
};

// Our custom FVF, which describes our custom vertex structure
#define PUMA_LINEDESC (D3DFVF_XYZ|D3DFVF_DIFFUSE)

LPDIRECT3DVERTEXBUFFER8 lineSegmentBuffPtr; // Buffer to hold vertices
int gNumOfLines;

const float GAME_SCALE = 1.0f;

// ******** data for old TEST triangle
LPDIRECT3DVERTEXBUFFER8 g_pVB     = NULL; // Buffer to hold vertices

// A structure for our custom vertex type
struct CUSTOMVERTEX
{
   FLOAT x, y, z;    // The untransformed, 3D position for the vertex
   DWORD color;     // The vertex color
};

// Our custom FVF, which describes our custom vertex structure
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)
// ******** end data for spaceTest triangle

PumaMesh *starsphere = NULL;
PumaMesh *billboard  = NULL;
PumaMesh *pylon      = NULL;


enum 
{
	RM_BUTTON_NEW_GAME,
	RM_BUTTON_TEXT,
	RM_BUTTON_STATS,
	RM_BUTTON_QUIT
};

// static pointer that the process function can use
SpaceTestMode *curSpaceTestMode = NULL;

// process function for UIRects in this mode
int FAR PASCAL SpaceTestModeProcess(UIRect *curUIRect, int type, long x, short y)
{
	switch(curUIRect->WhatAmI())	
	{

	case RM_BUTTON_QUIT    :
      if (UIRECT_MOUSE_LUP == type)
      {
//	      StartGameSound(clickSound, FALSE);
//         PostMessage((HWND)DrawLib::GetWindowPtr(), WM_KEYDOWN, VK_F12, 0);
      }
      break;

	case RM_BUTTON_NEW_GAME :
      if (UIRECT_MOUSE_LUP == type)
      {
	      PostQuitMessage( 0 );

			
//	      StartGameSound(clickSound, FALSE);
			
      }
	  break;

	}

   return 0;  // dealt with this message
}



//******************************************************************
SpaceTestMode::SpaceTestMode(int doid, char *doname) : GameMode(doid,doname)
{
}

//******************************************************************
SpaceTestMode::~SpaceTestMode()
{

}

//******************************************************************
int SpaceTestMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int SpaceTestMode::Activate(void) // do this when the mode becomes the forground mode.
{
	float tweakF;

	gCellViewDistance = gTweakSystem.GetTweak("CELLVIEWDISTANCE", 3,
		                            2, 5,
											 "Furthest sector you can see things in");
	
	gGalaxySize = gTweakSystem.GetTweak("GALAXYSIZE", 20,
		                            5, 30,
											 "Number of sectors wide and long in whole map");

	gCellWidth = gTweakSystem.GetTweak("SECTORSIZE", 250.0f,
		                            100, 1000,
											 "width and length of each sector");

	starsphere = new PumaMesh(0,"STARSPHERE_MESH");

	starsphere->LoadFromASC(puma->pd3dDevice, "dat\\starsphere.ASE");
	starsphere->LoadTexture(puma->pd3dDevice, "dat\\stars.png");
//	starsphere->Scale(puma->pd3dDevice, 0.1f,0.1f,0.1f);

	pylon = new PumaMesh(0,"PYLON_MESH");
	pylon->LoadFromASC(puma->pd3dDevice, "dat\\pylon.ASE");
	pylon->LoadTexture(puma->pd3dDevice, "dat\\pylon.png");
	pylon->Scale(puma->pd3dDevice, 0.1f,0.1f,0.1f);



	// Set up a white, directional light.
   // Note that many lights may be active at a time (but each one slows down
   // the rendering of our scene). However, here we are just using one. Also,
   // we need to set the D3DRS_LIGHTING renderstate to enable lighting
   D3DXVECTOR3 vecDir;
   D3DLIGHT8 light;
   ZeroMemory( &light, sizeof(D3DLIGHT8) );
   light.Type      = D3DLIGHT_DIRECTIONAL;
   light.Diffuse.r  = 1.0f;
   light.Diffuse.g  = 1.0f;
   light.Diffuse.b  = 1.0f;
   light.Ambient.r  = 1.0f;
   light.Ambient.g  = 1.0f;
   light.Ambient.b  = 1.0f;
   vecDir = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
   light.Range      = 1000.0f;
   puma->pd3dDevice->SetLight( 0, &light );
   puma->pd3dDevice->LightEnable( 0, TRUE );
   puma->pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

   vecDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
// puma->pd3dDevice->SetLight( 1, &light );

   // Finally, turn on some ambient light.
   puma->pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(128,128,128,255)  );



	// init camera position
	puma->pumaCamera->spacePoint.angle = -1.0f * D3DX_PI/2;
	puma->pumaCamera->spacePoint.azimuth = -0.37f;
	puma->pumaCamera->spacePoint.location.x = 165.0f;
	puma->pumaCamera->spacePoint.location.y = -66.0f;
	puma->pumaCamera->spacePoint.location.z = -125.0f;


   ZeroMemory( &neutralMaterial, sizeof(D3DMATERIAL8) );
   neutralMaterial.Diffuse.r = 0.5f;
   neutralMaterial.Diffuse.g = 0.5f;
   neutralMaterial.Diffuse.b = 0.5f;
   neutralMaterial.Diffuse.a = 0.5f;
   neutralMaterial.Ambient.r = 0.5f;
   neutralMaterial.Ambient.g = 0.5f;
   neutralMaterial.Ambient.b = 0.5f;
   neutralMaterial.Ambient.a = 0.5f;

	tweakF = gTweakSystem.GetTweak("PLANETAMBIENT", 0.1f, 
		                            0, 0.5f,
											 "Ambient luminace of planet (side in shadow)");
   ZeroMemory( &planetMaterial, sizeof(D3DMATERIAL8) );
   planetMaterial.Diffuse.r = 0.5f;
   planetMaterial.Diffuse.g = 0.5f;
   planetMaterial.Diffuse.b = 0.5f;
   planetMaterial.Diffuse.a = 0.5f;
   planetMaterial.Ambient.r = tweakF;
   planetMaterial.Ambient.g = tweakF;
   planetMaterial.Ambient.b = tweakF;
   planetMaterial.Ambient.a = 0.5f;

//   D3DXCreateTextureFromFile( puma->pd3dDevice, "dat\\mouseart.png", &mouseArt );
//	D3DUtil_SetColorKey( mouseArt, 0x00000000 );
	HRESULT hr = D3DXCreateTextureFromFileEx( puma->pd3dDevice, "dat\\mouseart.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffff00ff,
							NULL, NULL, &mouseArt);



   // ********** start building UIRects
   fullWindow = new UIRectWindow(-1, 0,0,640,480);
   fullWindow->fillStyle = UIRECT_WINDOW_STYLE_NOTHING;
   fullWindow->isActive  = TRUE;

   UIRectTextButton *tButt;

   tButt = new UIRectTextButton(RM_BUTTON_NEW_GAME,
                               2,2  ,100,25);
   tButt->SetText("Quit");
   tButt->process = SpaceTestModeProcess;
   fullWindow->AddChild(tButt);
//   UIRectStep(0,2);

   UIRectTextBox *tBox;

   tBox = new UIRectTextBox(RM_BUTTON_TEXT,2,puma->screenH - 70,630,puma->screenH-10);
   tBox->SetText("Antares Flame Online --- Test Visualization\n   Use right and left arrow keys to change direction\n   and up arrow to move forward.");
   tBox->process = SpaceTestModeProcess;
   fullWindow->AddChild(tBox);

   tBox = new UIRectTextBox(RM_BUTTON_STATS,puma->screenW - 400,2, puma->screenW-2, 33);
   tBox->SetText("Stats");
   tBox->process = SpaceTestModeProcess;
   fullWindow->AddChild(tBox);

   fullWindow->Arrange();

   // ********** finished building UIRects


   // build game-specific stuff
   flockSet = new FlockSet();
   meshRep = new MeshRepository();

	
   AFOFleet *fleet   = new AFOFleet(1,"PLAYER_FLEET");
	fleets.Append(fleet);

	hr = D3DXCreateTextureFromFileEx( puma->pd3dDevice, "dat\\ship2.png",
							0,0,0,0,
							D3DFMT_A8R8G8B8,
							D3DPOOL_MANAGED,
							D3DX_DEFAULT,
							D3DX_DEFAULT,
							0xffffffff,
							NULL, NULL, &(fleet->pTexture));
	
   fleet->cellX = 0;
   fleet->cellY = 0;

	fleet->angle = D3DX_PI/4;
	fleet->posX = gCellWidth/2;
	fleet->posY = gCellWidth/2;

	tweakF = gTweakSystem.GetTweak("PLAYERFLEETSHIPCOUNT", 14, 
		                            2, 30,
											 "Number of ships in player fleet");
	for (int i = 0; i < (int) tweakF; i++)
	{
		int meshIndex = 0;
		if (i > 2)
			meshIndex = 1;
		if (i > 4)
			meshIndex = 2;
		if (i > 7)
			meshIndex = 3;
		if (i > 10)
			meshIndex = 4;
		if (i > 12)
			meshIndex = 5;

		AFOShip *ship = new AFOShip(meshIndex,"testShip1"); 
		fleet->ships->Append(ship);

		ship->spacePoint.angle   = 0;
		ship->spacePoint.azimuth = 0;
		ship->spacePoint.roll    = 0;

		ship->spacePoint.location = D3DXVECTOR3( rnd(0.0f,gCellWidth),0, rnd(0.0f,gCellWidth));

		ship->flockPointIndex = flockSet->MakePoint();
		FlockPoint *fp = flockSet->GetPoint(ship->flockPointIndex);
		fp->x = ship->spacePoint.location.x;
		fp->y = ship->spacePoint.location.z;
		fp->maxSpeed = 1;
		fp->maxTurn = D3DX_PI / 20;
		fp->angle = ship->spacePoint.angle;
	}

	fleet->GivePositionsToShips(flockSet);

	float fleetCount = gTweakSystem.GetTweak("FLEETCOUNT", (gGalaxySize * gGalaxySize) / 10, 
		                            0, (gGalaxySize * gGalaxySize),
											 "Number of computer-controlled fleets");
	for (i = 0; i < (int) fleetCount; ++i)
	{
		fleet   = new AFOFleet(0,"DUMMY_FLEET");
		fleets.Append(fleet);

		hr = D3DXCreateTextureFromFileEx( puma->pd3dDevice, "dat\\ship2.png",
								0,0,0,0,
								D3DFMT_A8R8G8B8,
								D3DPOOL_MANAGED,
								D3DX_DEFAULT,
								D3DX_DEFAULT,
								0xffffffff,
								NULL, NULL, &(fleet->pTexture));
		
		fleet->cellX = rand() % gGalaxySize;
		fleet->cellY = rand() % gGalaxySize;

		fleet->angle = D3DX_PI/4;
		fleet->posX = fleet->cellX * gCellWidth + gCellWidth/2;
		fleet->posY = fleet->cellY * gCellWidth + gCellWidth/2;

		int shipNumLow = gTweakSystem.GetTweak("COMPFLEETSHIPCOUNTLOW", 2, 
			                            0, 10,
												 "lowest number of ships in comp. fleet");
		int shipNumHigh = (int) gTweakSystem.GetTweak("COMPFLEETSHIPCOUNTHIGH", 10, 
			                            0, 30,
												 "highest number of ships in comp. fleet");

		for (int i = 0; i < (rand() % (int)(shipNumHigh - shipNumLow)) + shipNumLow; i++)
		{
			int meshIndex = rand() % 6;

			AFOShip *ship = new AFOShip(meshIndex,"testShip1"); 
			fleet->ships->Append(ship);

			ship->spacePoint.angle   = 0;
			ship->spacePoint.azimuth = 0;
			ship->spacePoint.roll    = 0;

			ship->spacePoint.location = D3DXVECTOR3( rnd(0.0f,gCellWidth),0, rnd(0.0f,gCellWidth));
			ship->spacePoint.location.x += fleet->posX;
			ship->spacePoint.location.z += fleet->posY;

			ship->flockPointIndex = flockSet->MakePoint();
			FlockPoint *fp = flockSet->GetPoint(ship->flockPointIndex);
			fp->x = ship->spacePoint.location.x;
			fp->y = ship->spacePoint.location.z;
			tweakF = gTweakSystem.GetTweak("SHIPSPEED", 1, 
				                            0, 10,
													 "speed of all ships");
			fp->maxSpeed = tweakF;
			tweakF = gTweakSystem.GetTweak("SHIPTURNRATE", D3DX_PI / 20, 
				                            0.00001f, D3DX_PI / 7,
													 "turn rate of all ships");
			fp->maxTurn = tweakF;
			fp->angle = ship->spacePoint.angle;
			tweakF = gTweakSystem.GetTweak("SHIPFLOCKINGSIZE", 10, 
				                            5, 15,
													 "flocking exclusion radius of all ships");
			fp->size = tweakF;
		}

		fleet->GivePositionsToShips(flockSet);

	}


	sphere = new PumaMesh();
	sphere->LoadFromASC(puma->pd3dDevice, "dat\\sphere.ASE");
	tweakF = gTweakSystem.GetTweak("PLANETSIZESCALE", 0.3f, 
		                            0.1f, 1.0f,
											 "how much to scale planet mesh");
	sphere->Scale(puma->pd3dDevice,tweakF,tweakF,tweakF);
	atmosphere = new PumaMesh();
	atmosphere->LoadFromASC(puma->pd3dDevice, "dat\\sphere.ASE");
	tweakF = gTweakSystem.GetTweak("ATMOSPHERESIZESCALE", 0.31f, 
		                            0.1f, 1.0f,
											 "how much to scale planet atmosphere mesh");
	atmosphere->Scale(puma->pd3dDevice,tweakF,tweakF,tweakF);

	shipStandin = new PumaMesh();
	shipStandin->LoadFromASC(puma->pd3dDevice, "dat\\shipStandin.ASE");
	tweakF = gTweakSystem.GetTweak("SHIPSTANDINSIZESCALE", 0.17f, 
		                            0.1f, 1.0f,
											 "how much to scale low-rez ship mesh");
	shipStandin->Scale(puma->pd3dDevice,tweakF,tweakF,tweakF);


	int planetCount = gTweakSystem.GetTweak("PLANETCOUNT", (gGalaxySize * gGalaxySize) / 5, 
		                            0, (gGalaxySize * gGalaxySize),
											 "Number of planets in galaxy");
	for (i = 0; i < (int) planetCount; ++i)
	{
		int foundSpace, possX, possY;
		do
		{
			possX = rand() % gGalaxySize;
			possY = rand() % gGalaxySize;
			foundSpace = TRUE;

			AFOPlanet *curPlanet = (AFOPlanet *) planets.First();
			while (curPlanet)
			{
				if (curPlanet->cellX == possX && curPlanet->cellY == possY)
					foundSpace = FALSE;

				curPlanet = (AFOPlanet *) planets.Next();
			}

		} while (!foundSpace);

		AFOPlanet *planet = new AFOPlanet(sphere, atmosphere, possX, possY);
		planets.Append(planet);

		int flockIndex = flockSet->MakePoint(); 
		FlockPoint *planetfp = flockSet->GetPoint(flockIndex);
		planetfp->angle = planetfp->targetAngle = 0;
		planetfp->maxSpeed = 1;
		planetfp->maxTurn = 1;
		planetfp->x = planetfp->targetX = planet->posX;
		planetfp->y = planetfp->targetY = planet->posY;
		tweakF = gTweakSystem.GetTweak("PLANETFLOCKINGSIZE", 40, 
			                            20, 55,
												 "flocking exclusion radius of planets");
		planetfp->size = tweakF;
	}

	// find out how many lines are visible
	gNumOfLines = gGalaxySize * gGalaxySize * 2;// + gGalaxySize * 2;

	// set up track line vertex buffer
	lineSegmentBuffPtr = NULL; // Buffer to hold vertices

   if( FAILED( puma->pd3dDevice->CreateVertexBuffer( gNumOfLines*2*sizeof(PUMALINESTRUCT),
                             0, PUMA_LINEDESC,
                             D3DPOOL_MANAGED, &lineSegmentBuffPtr ) ) )
   {
      return 0;
   }

	int lineColor = (int)gTweakSystem.GetTweak("LINEBRIGHTNESS", 55, 
		                            0, 255,
											 "brightness of the sector lines");
	int lineAlpha = (int)gTweakSystem.GetTweak("LINEALPHA", 85, 
		                            0, 255,
											 "alpha translucency of the sector lines");


	// Prepare to fill the line vertex buffer, by locking it.
	PUMALINESTRUCT *pVertices;
	if( FAILED( lineSegmentBuffPtr->Lock( 0, 0, (BYTE**)&pVertices, 0 ) ) )
		return 0;

	int curLine = 0;

	for (int gridY = 0; gridY < 20; ++gridY)
	{
		for (int gridX = 0; gridX < 20; ++gridX)
		{

			pVertices[curLine*2+0].position.x = (float) gridX * gCellWidth;
			pVertices[curLine*2+0].position.z = (float) gridY * gCellWidth;
			pVertices[curLine*2+0].position.y = 0.0f;
			pVertices[curLine*2+0].color = D3DCOLOR_RGBA(lineColor,lineColor,lineColor,lineAlpha);

			pVertices[curLine*2+1].position.x = (float) gridX * gCellWidth;
			pVertices[curLine*2+1].position.z = (float) (gridY + 1) * gCellWidth;
			pVertices[curLine*2+1].position.y = 0.0f;
			pVertices[curLine*2+1].color = D3DCOLOR_RGBA(lineColor,lineColor,lineColor,lineAlpha);

			curLine++;

			pVertices[curLine*2+0].position.x = (float) gridX * gCellWidth;
			pVertices[curLine*2+0].position.z = (float) gridY * gCellWidth;
			pVertices[curLine*2+0].position.y = 0.0f;
			pVertices[curLine*2+0].color = D3DCOLOR_RGBA(lineColor,lineColor,lineColor,lineAlpha);

			pVertices[curLine*2+1].position.x = (float) (gridX + 1) * gCellWidth;
			pVertices[curLine*2+1].position.z = (float) gridY * gCellWidth;
			pVertices[curLine*2+1].position.y = 0.0f;
			pVertices[curLine*2+1].color = D3DCOLOR_RGBA(lineColor,lineColor,lineColor,lineAlpha);

			curLine++;

		}
	}

   lineSegmentBuffPtr->Unlock();



	curCamAngle = realCamAngle = (float)NormalizeAngle(D3DX_PI/2);
	camCenterX  = fleet->cellX * gCellWidth + gCellWidth/2;
	camCenterZ  = fleet->cellY * gCellWidth + gCellWidth/2;


   m_pParticleSystem      = new CParticleSystem( 512, 2048, 500.0f );
   D3DXCreateTextureFromFile( puma->pd3dDevice, "dat\\flare-white.png", &m_pParticleTexture );

   // Initialize the particle system
   if( FAILED( hr = m_pParticleSystem->RestoreDeviceObjects( puma->pd3dDevice ) ) )
       return hr;



//	plasma = new PlasmaTexture();
//	plasma->Generate(puma->pd3dDevice, 128, 128);
//	plasma->Colorize();

	return(0);
}

//******************************************************************
int SpaceTestMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{

   SAFE_RELEASE(m_pParticleTexture);


	delete m_pParticleSystem;

//	delete fleet;
	delete meshRep;
	delete flockSet;

	delete fullWindow;

	delete shipStandin;
	delete atmosphere;
	delete sphere;
//	delete planet;
	AFOPlanet *curPlanet = (AFOPlanet *) planets.First();
	while (curPlanet)
	{
		planets.Remove(curPlanet);
		delete curPlanet;

		curPlanet = (AFOPlanet *) planets.First();
	}

	AFOFleet *curFleet = (AFOFleet *) fleets.First();
	while (curFleet)
	{
		fleets.Remove(curFleet);
		delete curFleet;

		curFleet = (AFOFleet *) fleets.First();
	}

   SAFE_RELEASE(mouseArt);

   SAFE_RELEASE(lineSegmentBuffPtr);


//	delete pm;
	delete starsphere;
	delete pylon;
//	delete billboard;


	SAFE_RELEASE(g_pVB);

	return(0);
}

float shipAngle = 0.0f;

//******************************************************************
int SpaceTestMode::Tick(void)
{
   D3DXMATRIX matWorld, mat2;
	int statsNumFleets, statsNumPlanets;
	int statsNumShipsVis, statsNumPlanetsVis;
	int statsNumShipsFull;
	float tweakF;



	WindowServicer();
	// processing

	flockSet->Tick(1.0, 0,0);//gCellWidth/2, gCellWidth/2);

	AFOFleet *playerFleet = (AFOFleet *) fleets.Find(1);
	AFOFleet *curFleet = (AFOFleet *) fleets.First();
	while (curFleet)
	{
		if (abs(curFleet->cellX - playerFleet->cellX) < gCellViewDistance && 
			 abs(curFleet->cellY - playerFleet->cellY) < gCellViewDistance)
		{
			AFOShip *afoShip = (AFOShip *) curFleet->ships->First();
			while (afoShip)
			{
				FlockPoint *fp = flockSet->GetPoint(afoShip->flockPointIndex);
				afoShip->spacePoint.location.x = fp->x;
				afoShip->spacePoint.location.z = fp->y;
				afoShip->spacePoint.angle = fp->angle;

				afoShip = (AFOShip *) curFleet->ships->Next();
			}
		}
		curFleet = (AFOFleet *) fleets.Next();
	}

	AFOPlanet *curPlanet;
	curFleet = (AFOFleet *) fleets.First();
	while (curFleet)
	{
		if (0 == curFleet->WhatAmI())
			curFleet->Process(flockSet);
		else
		{
//			curFleet->Process(flockSet);  // TEST
			curPlanet = (AFOPlanet *) planets.First();
			while (curPlanet)
			{
				curPlanet->Process(curFleet->cellX, curFleet->cellY);
				curPlanet = (AFOPlanet *) planets.Next();
			}
		}
		curFleet = (AFOFleet *) fleets.Next();
	}

	if (curCamAngle != realCamAngle)
	{
		float delta = curCamAngle - realCamAngle;

		tweakF = gTweakSystem.GetTweak("CAMROTATIONCOEFF", 0.17f, 
			                            0.001f, 1.0f,
												 "adjusts speed of camera rotation");
		realCamAngle += delta * tweakF;
	
		tweakF = gTweakSystem.GetTweak("CAMROTATIONSTOP", D3DX_PI/490, 
			                            D3DX_PI/40090, D3DX_PI/2,
												 "finish camera turn when camera angle is this close to target angle");
		if (fabs(curCamAngle - realCamAngle) < tweakF)
			realCamAngle = curCamAngle;

	}

	// start drawing

	puma->StartRenderingFrame();

	D3DXMATRIX matTrans, matRotateX, matRotateY, matRotateZ, matTemp1, matTemp2;


	// render starsphere

   puma->pd3dDevice->SetMaterial( &neutralMaterial );
//   puma->pd3dDevice->LightEnable( 1, TRUE );
//   puma->pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(255,255,255,255)  );


	D3DXMatrixIdentity( &matWorld );
	puma->pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	D3DXMatrixRotationY( &matTemp1, realCamAngle); //camAngle[1]);
	D3DXMatrixRotationX( &matRotateX, puma->pumaCamera->spacePoint.azimuth);
	D3DXMatrixMultiply( &matTemp1, &matTemp1, &matRotateX);
	D3DXMatrixMultiply( &matWorld, &matWorld, &matTemp1 );

	puma->pd3dDevice->SetTransform( D3DTS_VIEW , &matWorld );

	// Turn off the zbuffer
	puma->pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	
//	puma->pd3dDevice->SetMaterial(NULL);
	starsphere->Draw(puma->pd3dDevice);

	// Turn on the zbuffer
	puma->pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

//	puma->HandleCamera();


	// position camera to view current sector
	
	// translate, then rotate, to position relative to sector
	D3DXMatrixRotationY( &matTemp1, realCamAngle);
	tweakF = gTweakSystem.GetTweak("NORMCAMDISTZ", gCellWidth * 0.8f,
		                            gCellWidth * 0.4f, gCellWidth * 1.5f,
											 "Normal Camera moveback from target");
	D3DXMatrixTranslation( &matTemp2, 0,0, tweakF);
	D3DXMatrixMultiply( &matTemp1, &matTemp1, &matTemp2 );

	// build position at sector
	tweakF = gTweakSystem.GetTweak("NORMCAMDISTY", -50.0f,
		                            -10.0f, -100.0f,
											 "Normal Camera moveup from target");
	D3DXMatrixTranslation( &matTrans, -1* camCenterX, tweakF, -1 *camCenterZ );

	tweakF = gTweakSystem.GetTweak("NORMCAMPITCH", -0.37f,
		                            -D3DX_PI, 0,
											 "Normal Camera pitch angle");
	D3DXMatrixRotationX( &matRotateX, tweakF);

	// turn around to face sector
	D3DXMatrixRotationY( &matRotateY, (D3DX_PI));

	// pitch down
	D3DXMatrixMultiply( &matTemp1, &matTemp1, &matRotateX );

	// add the rotate-to-face
	D3DXMatrixMultiply( &matRotateY, &matRotateY, &matTemp1 );

	// and finally translate to the position of the sector itself
	D3DXMatrixMultiply( &matTrans, &matTrans, &matRotateY );


	puma->pd3dDevice->SetTransform( D3DTS_VIEW, &matTrans );


	// render track lines

	puma->pd3dDevice->SetTexture( 0, NULL );

   // Render the line buffer contents
   D3DXMatrixIdentity( &matWorld );
   puma->pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

   puma->pd3dDevice->SetStreamSource( 0, lineSegmentBuffPtr, sizeof(PUMALINESTRUCT) );
   puma->pd3dDevice->SetVertexShader( PUMA_LINEDESC );
   puma->pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0, gNumOfLines );


	// end render of track lines

//   puma->pd3dDevice->LightEnable( 1, FALSE );
//   puma->pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_RGBA(128,128,128,255)  );


	puma->pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00808080 );


   // render AFOShips
	statsNumFleets = statsNumShipsVis = statsNumShipsFull = 0;
	playerFleet = (AFOFleet *) fleets.Find(1);
	curFleet = (AFOFleet *) fleets.First();
	while (curFleet)
	{
		++statsNumFleets;
		if (abs(curFleet->cellX - playerFleet->cellX) < gCellViewDistance && 
			 abs(curFleet->cellY - playerFleet->cellY) < gCellViewDistance)
		{
			puma->pd3dDevice->SetTexture( 0, curFleet->pTexture );
			puma->pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
			puma->pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			puma->pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
			puma->pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

			AFOShip *afoShip = (AFOShip *) curFleet->ships->First();
			int numShips = 0;
			if (1 == curFleet->WhatAmI())
				camCenterX = camCenterZ = 0;
			while (afoShip)
			{
				++statsNumShipsVis;
				D3DXMatrixIdentity( &matWorld );
				D3DXMatrixRotationY(&matWorld, (float) afoShip->spacePoint.angle * -1 + D3DX_PI);

		//  /	   D3DXMATRIX mat2;
				D3DXMatrixIdentity( &mat2 );
				D3DXMatrixTranslation(&mat2, afoShip->spacePoint.location.x,
													 afoShip->spacePoint.location.y,
											 afoShip->spacePoint.location.z);

				D3DXMatrixMultiply(&matWorld, &matWorld, &mat2);

				puma->pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
				PumaMesh *pm	= meshRep->GetMesh(afoShip->WhatAmI());
				if (abs(curFleet->cellX - playerFleet->cellX) > 1 || 
					 abs(curFleet->cellY - playerFleet->cellY) > 1)
					 pm = shipStandin;
				else
					++statsNumShipsFull;

				pm->mtrl = curFleet->mtrl;
		/*
				pm->mtrl.Diffuse.r = 0.5f;
				pm->mtrl.Diffuse.g = 0.0f;
				pm->mtrl.Diffuse.b = 0.0f;
		*/

				pm->Draw(puma->pd3dDevice);

				if (1 == curFleet->WhatAmI())
				{
					camCenterX += afoShip->spacePoint.location.x;
					camCenterZ += afoShip->spacePoint.location.z;
				}
				++numShips;

				afoShip = (AFOShip *) curFleet->ships->Next();
			}

			if (1 == curFleet->WhatAmI())
			{
				camCenterX /= numShips;
				camCenterZ /= numShips;
			}
		}
		curFleet = (AFOFleet *) fleets.Next();
	}

	 

	// end render of AFOShips

	// render planets
//   puma->pd3dDevice->SetMaterial( &planetMaterial );
	statsNumPlanets = 0;
	statsNumPlanetsVis = 0;
	curPlanet = (AFOPlanet *) planets.First();
	while (curPlanet)
	{
		++statsNumPlanets;
		if (abs(curPlanet->cellX - playerFleet->cellX) < gCellViewDistance && 
			 abs(curPlanet->cellY - playerFleet->cellY) < gCellViewDistance)
		{
			++statsNumPlanetsVis;
			curPlanet->pm->mtrl  = planetMaterial;
			curPlanet->pm2->mtrl = planetMaterial;
			curPlanet->Draw();
		}
		curPlanet = (AFOPlanet *) planets.Next();
	}


	// render particles
	puma->ClearMaterial();
	D3DXMatrixIdentity( &mat2 );
	puma->pd3dDevice->SetTransform( D3DTS_WORLD, &mat2 );

   D3DXVECTOR3 vEmitterPostion( gCellWidth/2, 0.0f, gCellWidth/2 );

   // Update particle system
   m_pParticleSystem->Update( 1, 1, g_clrColor[2],
                                     g_clrColorFade[0], 1.0f,
                              vEmitterPostion );

   puma->pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );
   puma->pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
   puma->pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
   puma->pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

   puma->pd3dDevice->SetTexture(0, m_pParticleTexture );
   m_pParticleSystem->Render( puma->pd3dDevice );

   puma->pd3dDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE );
   puma->pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );



	// render pylons
	/*
	for (int gridY = 0; gridY < 20; ++gridY)
	{
		for (int gridX = 0; gridX < 20; ++gridX)
		{

			D3DXMatrixTranslation(&mat2, (float) (gridX - 10) * gCellWidth, 
					                             0.0f,
												 (gridY - 10) * gCellWidth);
			puma->pd3dDevice->SetTransform( D3DTS_WORLD, &mat2 );
			pylon->Draw(puma->pd3dDevice);
		}
	}
	*/
	// end of rendering pylons


//	shipAngle += 0.04f;

   // Render the vertex buffer contents
	/*
   puma->pd3dDevice->SetStreamSource( 0, g_pVB, sizeof(CUSTOMVERTEX) );
   puma->pd3dDevice->SetVertexShader( D3DFVF_CUSTOMVERTEX );
   puma->pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 1 );
	*/
	/*
	CD3DFont *font = puma->GetDXFont(0);

	RECT tempRect;
	tempRect.left   = 10;
	tempRect.top   = 10;
	tempRect.right  = 200;
	tempRect.bottom = 40;
	*/
//	D3DCOLOR color;

//	color = D3DCOLOR_RGBA(255,0,0,255);


//   font->DrawText( rect, NULL, color, "SpaceTest String!" );

//	HRESULT hr = font->DrawText("SpaceTest string!", strlen("SpaceTest string!"), &tempRect,	DT_LEFT, color);

//   puma->pd3dDevice->EndScene();
//   puma->pd3dDevice->BeginScene();
	char tempText[1024];
	sprintf(tempText,"Planets %d, Visible %d\nFleets %d, Ships Visible %d, fully visible %d",
				statsNumPlanets, statsNumPlanetsVis, 
				statsNumFleets,  statsNumShipsVis,   statsNumShipsFull
				);
	
   UIRectTextBox *tBox = (UIRectTextBox *) fullWindow->childRectList.Find(RM_BUTTON_STATS);
	tBox->SetText(tempText);

   fullWindow->Draw();

//	RECT rect = {100,100,228,228};
//	puma->DrawRect(plasma->pTexture, rect, 0xffffffff);


	puma->DrawMouse(mouseArt);

//	puma->DrawRect(/*pm->pTexture*/NULL,300,300,350,400,D3DCOLOR_RGBA(255,0,0,100)); 


	puma->FinishRenderingFrame();

//	DEBUG_MSG("Debug!");

	return(0);
}


//*******************************************************************************
long SpaceTestMode::WindowServicer(void)
{

	AFOFleet *fleet;

	// check input
	puma->PumpInput();
	int tempX, tempY;

	PumaInputEvent *pie = puma->TakeNextInputEvent();
	while (pie)
	{
		switch(pie->WhatAmI())
		{
		case PUMA_INPUT_KEYDOWN:
			if (200 == pie->scancode ) // if up
				keysPressed |= KEY_ACCEL;

			if (208 == pie->scancode ) // if down
				keysPressed |= KEY_BRAKE;

			if (205 == pie->scancode ) // if right
				keysPressed |= KEY_LEFT;

			if (203 == pie->scancode ) // if left
				keysPressed |= KEY_RIGHT;

         fullWindow->Action(UIRECT_KEY_DOWN, 0, pie->scancode);

			break;

		case PUMA_INPUT_KEYUP:
			if (200 == pie->scancode ) // if up
			{
				keysPressed &= ~(KEY_ACCEL);
				curCamAngle  = (float) NormalizeAngle(curCamAngle);
				realCamAngle = (float) NormalizeAngle(realCamAngle);
				if (fabs(0 - curCamAngle) < 0.1  || fabs(D3DX_PI * 2 - curCamAngle) < 0.1)
				{
					tempX = 0;
					tempY = -1;
				}
				if (fabs(D3DX_PI/2 - curCamAngle) < 0.1)
				{
					tempX = 1;
					tempY = 0;
				}
				if (fabs(D3DX_PI - curCamAngle) < 0.1)
				{
					tempX = 0;
					tempY = 1;
				}
				if (fabs(D3DX_PI*3.0f/2.0f - curCamAngle) < 0.1)
				{
					tempX = -1;
					tempY = 0;
				}

				fleet = (AFOFleet *) fleets.Find(1);

				// if not going out of bounds...
				if (tempX + fleet->cellX < gGalaxySize &&
				    tempX + fleet->cellX >= 0 &&
				    tempY + fleet->cellY < gGalaxySize &&
				    tempY + fleet->cellY >= 0)
				{
					fleet->cellX += tempX;
					fleet->cellY += tempY;
					tempX = (int)(fleet->cellX * gCellWidth + gCellWidth/2);
					tempY = (int)(fleet->cellY * gCellWidth + gCellWidth/2);
					fleet->angle  = TurnTowardsPoint(fleet->posX, fleet->posY, tempX, tempY, 0.0f, 0.0f); 
					fleet->posX   = (float)tempX;
					fleet->posY   = (float)tempY;
					fleet->GivePositionsToShips(flockSet);
				}

			}

			if (208 == pie->scancode ) // if down
				keysPressed &= ~(KEY_BRAKE);

			if (205 == pie->scancode ) // if right
			{
				keysPressed &= ~(KEY_LEFT);
				if (curCamAngle == realCamAngle)
					curCamAngle -= D3DX_PI/2;
			}
			if (203 == pie->scancode ) // if left
			{
				keysPressed &= ~(KEY_RIGHT);
				if (curCamAngle == realCamAngle)
					curCamAngle += D3DX_PI/2;
			}

         fullWindow->Action(UIRECT_KEY_UP, 0, pie->scancode);

			break;

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
		}

		delete pie; // IMPORTANT to delete the message after using it!
		pie = puma->TakeNextInputEvent();
	}

   fullWindow->Action(UIRECT_MOUSE_MOVE, puma->mouseX, puma->mouseY);


/*


	int x,y;

   switch (message)
   {

    	case WM_KEYDOWN:
         firstParent->Action(UIRECT_KEY_DOWN, lParam, wParam);
        	break;

    	case WM_KEYUP:
         firstParent->Action(UIRECT_KEY_UP, lParam, wParam);
        	break;

		case WM_CHAR:
         firstParent->Action(UIRECT_KEY_CHAR, lParam, wParam);
         break;

		case WM_MOUSEMOVE:
			x = LOWORD(lParam);  // horizontal position of cursor  
			y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_MOVE, x,y);
			break;

      case WM_RBUTTONDOWN:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_RDOWN, x,y);
         break;

      case WM_LBUTTONDOWN:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_LDOWN, x,y);
         break;

      case WM_RBUTTONUP:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_RUP, x,y);
         break;

      case WM_LBUTTONUP:
         x = LOWORD(lParam);  // horizontal position of cursor  
         y = HIWORD(lParam);  // vertical position of cursor    
         firstParent->Action(UIRECT_MOUSE_LUP, x,y);
    }
  */
    return 0;
}




/* end of file */



