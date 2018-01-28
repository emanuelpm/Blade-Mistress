#ifndef TESTMODE_H
#define TESTMODE_H

#include "gamemode.h"

class TestMode : public GameMode
{
	public:

		TestMode(int doid, char *doname);
		virtual ~TestMode();

		virtual int Init(void); 		// do this when instantiated.
		virtual int Activate(void); 	// do this when the mode becomes the forground mode.
		virtual int Deactivate(void);	// do this when the mode gets pushed behind another mode.
		virtual int Tick(void);

};

#endif
