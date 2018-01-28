
#ifndef BBOSquareS_H
#define BBOSquareS_H

#include ".\puma\puma.h"
#include "GroundObjectTiles.h"
#include "RealmObject.h"
#include ".\helper\linklist.h"


//*********************************************************************
class LandSquareManager
{
public:
	LandSquareManager(GroundObjectTiles *go);
	virtual ~LandSquareManager();

	void StartFrame(void);
	void AddSquare(int x, int y);
	void AddDungeonSquare(int x, int y);
	void AddRealmSquare(int x, int y);
	void DrawSquares(void);

	LPDIRECT3DVERTEXBUFFER8 vBuffer;
	int triIndex;

	GroundObjectTiles *go;
	RealmObject *ro;

};


#endif
