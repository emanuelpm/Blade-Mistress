#ifndef BBO_SNEWBGENERATOR_H
#define BBO_SNEWBGENERATOR_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"


class BBOSNewbGenerator : public BBOSGenerator
{
public:

	BBOSNewbGenerator(int x, int y);
	virtual ~BBOSNewbGenerator();
	virtual void Tick(SharedSpace *ss);
	virtual void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
