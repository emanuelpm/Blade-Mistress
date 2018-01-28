
//***************************************************************
// base class for a game mode which likes to belong to a stack.
//***************************************************************
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "gamemode.h"

// each Game Mode creates the next.
GameMode * currentGameMode = NULL, *newGameMode = NULL;

Stack gameModeStack;


//******************************************************************
GameMode::GameMode(int doid, char *doname) : DataObject(doid,doname)
{
	retState = GMR_NORMAL;
}

//******************************************************************
GameMode::~GameMode()
{

}

//******************************************************************
int GameMode::Init(void)  // do this when instantiated.
{
	return(0);
}

//******************************************************************
int GameMode::Activate(void) // do this when the mode becomes the forground mode.
{
	return(0);
}

//******************************************************************
int GameMode::Deactivate(void) // do this when the mode gets pushed behind another mode.
{
	return(0);
}

//******************************************************************
int GameMode::Tick(void)
{
	return(0);
}

//******************************************************************
void GameMode::SetEnvironment(void)
{

}



/* end of file */



