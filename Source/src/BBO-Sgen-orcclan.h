#ifndef BBO_SGEN_ORCCLAN_H
#define BBO_SGEN_ORCCLAN_H

//#include "pumamesh.h"
#include "BBO-Sgenerator.h"

class BBOSGenOrcClan : public BBOSGenerator
{
public:

	BBOSGenOrcClan(int x, int y);
	virtual ~BBOSGenOrcClan();
	void Tick(SharedSpace *ss);
//	void FindMonsterPoint(int town, int &mx, int &my, int townDist);

};

#endif
