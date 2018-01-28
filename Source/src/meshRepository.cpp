
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "MeshRepository.h"

char meshFileNames[NUM_OF_REP_MESHES][128] = 
{
	{"dat\\mid-blob.pum"},
	{"dat\\sleek.pum"},
	{"dat\\destroyer.pum"},
	{"dat\\destroyer2.pum"},
	{"dat\\lancer.pum"},
	{"dat\\big.pum"}
};

//******************************************************************
MeshRecord::MeshRecord(int doid, char *doname) : DataObject(doid,doname)
{
	pm = new PumaMesh();
	pm->Load(puma->m_pd3dDevice,doname);
	float tweakF = gTweakSystem.GetTweak("SHIPMESHSCALE", 0.1f, 
				                            0.05f, 0.4f,
													 "scale value for all ship meshes");
	pm->Scale(puma->m_pd3dDevice, tweakF,tweakF,tweakF);
//	pm->LoadTexture(puma->m_pd3dDevice, "dat\\ship1.bmp");

}

//******************************************************************
MeshRecord::~MeshRecord()
{
	delete pm;

}

//******************************************************************
//******************************************************************
MeshRepository::MeshRepository(void)
{
}

//******************************************************************
MeshRepository::~MeshRepository()
{
	MeshRecord *mr = (MeshRecord *) meshRecords.First();
	while (mr)
	{
		meshRecords.Remove(mr);
		delete mr;

		mr = (MeshRecord *) meshRecords.First();
	}


}

//******************************************************************
PumaMesh *MeshRepository::GetMesh(int index)
{
	MeshRecord *mr = (MeshRecord *) meshRecords.Find(index);

	if (mr)
		return mr->pm;

	mr = new MeshRecord(index, meshFileNames[index]);
	meshRecords.Append(mr);

	return mr->pm;


}




/* end of file */



