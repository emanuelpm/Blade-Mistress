#ifndef TOKENMANAGER_H
#define TOKENMANAGER_H

#include ".\helper\linklist.h"
#include "BBO.h"
#include "version.h"
#include "Tower-Map.h"


//******************************************************************
class TokenManager
{
public:

	TokenManager();
	virtual ~TokenManager();
	void Init(void);
	void Tick(void);
	void Save(void);
	void Load(void);

	int TokenTypeInSquare(SharedSpace *ss, int x, int y);
	int TokenIsInHere(int index, SharedSpace *ss);
	void PlayerEntersSquare(char *playerName, SharedSpace *ss, int x, int y);


	LongTime tokenTimeLeft[MAGIC_MAX];

	char towerName[MAGIC_MAX][64];
	int x[MAGIC_MAX], y[MAGIC_MAX];

};

extern TokenManager tokenMan;


#endif
