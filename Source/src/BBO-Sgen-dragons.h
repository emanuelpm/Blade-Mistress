#ifndef BBO_SGEN_DRAGONS_H
#define BBO_SGEN_DRAGONS_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"

class BBOSGenDragons : public BBOSGenerator
{
public:

	BBOSGenDragons(int x, int y);
	virtual ~BBOSGenDragons();
	void Tick(SharedSpace *ss);
//	void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
