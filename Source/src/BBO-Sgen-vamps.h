#ifndef BBO_SGEN_VAMPS_H
#define BBO_SGEN_VAMPS_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"

class BBOSGenVamps : public BBOSGenerator
{
public:

	BBOSGenVamps(int x, int y);
	virtual ~BBOSGenVamps();
	void Tick(SharedSpace *ss);
//	void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
