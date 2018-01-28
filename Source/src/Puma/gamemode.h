#ifndef GAMEMODE_H
#define GAMEMODE_H

//***************************************************************
// base class for a game mode which likes to belong to a stack.
//***************************************************************

#include "../helper/dataobje.h"
#include "../helper/linklist.h"

#include "../helper/autolog.h"

#define GMR_NORMAL		0
#define GMR_NEW_MODE	1
#define GMR_POP_ME		2

class GameMode : public DataObject
{
	public:

		GameMode(int doid, char *doname);
		virtual ~GameMode();

		virtual int  Init(void); 		// do this when instantiated.
		virtual int  Activate(void); 	// do this when the mode becomes the forground mode.
		virtual int  Deactivate(void);	// do this when the mode gets pushed behind another mode.
		virtual void SetEnvironment(void);

		virtual int Tick(void);

		// after every call to Tick(), this is checked.  If not GMR_NORMAL,
		// something happens.
		int retState;
};


// each Game Mode creates the next.
extern GameMode * currentGameMode, *newGameMode;

extern Stack gameModeStack;

#endif
