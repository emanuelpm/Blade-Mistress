#ifndef BBO_SGEN_NEARVAMPS_H
#define BBO_SGEN_NEARVAMPS_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"

class BBOSGenNearVamps : public BBOSGenerator
{
public:

	BBOSGenNearVamps(int x, int y);
	virtual ~BBOSGenNearVamps();
	void Tick(SharedSpace *ss);
//	void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
