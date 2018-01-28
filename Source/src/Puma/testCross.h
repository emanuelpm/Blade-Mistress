#ifndef TEST_CROSS_H
#define TEST_CROSS_H

#include "puma.h"
#include "pumamesh.h"
#include "dataobje.h"
#include "linklist.h"

//***************************************************************************************
class TestCross : public DataObject
{
public:

	TestCross(void);
	virtual ~TestCross();

	void Draw(LPDIRECT3DDEVICE8 pd3dDevice,D3DXVECTOR3 position, float width, float y);
	
	LPDIRECT3DVERTEXBUFFER8 vertexBufferPtr; // Buffer to hold vertices

};

#endif
