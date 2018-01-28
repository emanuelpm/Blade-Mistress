#ifndef BBO_SGEN_SPIRITS_H
#define BBO_SGEN_SPIRITS_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"

class BBOSGenSpirits : public BBOSGenerator
{
public:

	BBOSGenSpirits(int x, int y);
	virtual ~BBOSGenSpirits();
	void Tick(SharedSpace *ss);
	void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
