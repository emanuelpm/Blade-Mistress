#ifndef MESHREPOSITORY_H
#define MESHREPOSITORY_H

#include "./helper/linklist.h"
#include "./puma/puma.h"
#include "./puma/pumamesh.h"


const int NUM_OF_REP_MESHES = 6;

//******************************************************************
class MeshRecord : public DataObject
{
public:

	MeshRecord(int doid, char *doname);
	virtual ~MeshRecord();

	PumaMesh *pm;

};

//******************************************************************
class MeshRepository
{
public:

	MeshRepository(void);
	virtual ~MeshRepository();

	PumaMesh *GetMesh(int index);

	DoublyLinkedList meshRecords;
};

#endif
