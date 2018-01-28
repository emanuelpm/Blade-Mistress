
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "BBOStatic.h"

//LPDIRECT3DTEXTURE8 BBOStatic::skins[NUM_OF_STATIC_OBJECTS];
PumaMesh *BBOStatic::meshes[NUM_OF_STATIC_OBJECTS];
int BBOStatic::refCount = 0;

//******************************************************************
BBOStatic::BBOStatic(int doid, char *doname) : DataObject(doid,doname)
{
	cellSlot = -1;
	pleaseDelete = FALSE;

	flowerLocList = NULL;
	flowerType = 0;

	if (refCount < 1)
	{
		/*
		HRESULT hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\f_tree.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[0]);
		hr = D3DXCreateTextureFromFileEx( puma->m_pd3dDevice, "dat\\f_tree.png",
							0,0,1,0,	D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT,
							D3DX_DEFAULT, 0xffffffff, NULL, NULL, 
							&skins[1]);
							*/
		meshes[0] = new PumaMesh();
		meshes[0]->LoadFromASC(puma->m_pd3dDevice,"dat\\d-tree1.ase");
		meshes[0]->LoadTexture(puma->m_pd3dDevice,"dat\\d-trees.png",1);
		meshes[0]->Scale(puma->m_pd3dDevice,0.004f, 0.004f, 0.004f);
		meshes[1] = new PumaMesh();
		meshes[1]->LoadFromASC(puma->m_pd3dDevice,"dat\\d-tree2.ase");
		meshes[1]->LoadTexture(puma->m_pd3dDevice,"dat\\d-trees.png",1);
		meshes[1]->Scale(puma->m_pd3dDevice,0.003f, 0.003f, 0.003f);

		meshes[2] = new PumaMesh();
		meshes[2]->LoadFromASC(puma->m_pd3dDevice,"dat\\tree1.ase");
		meshes[2]->LoadTexture(puma->m_pd3dDevice,"dat\\f_tree.png",1);
		meshes[2]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[3] = new PumaMesh();
		meshes[3]->LoadFromASC(puma->m_pd3dDevice,"dat\\tree2.ase");
		meshes[3]->LoadTexture(puma->m_pd3dDevice,"dat\\f_tree.png",1);
		meshes[3]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[4] = new PumaMesh();
		meshes[4]->LoadFromASC(puma->m_pd3dDevice,"dat\\tree3.ase");
		meshes[4]->LoadTexture(puma->m_pd3dDevice,"dat\\f_tree.png",1);
		meshes[4]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[5] = new PumaMesh();
		meshes[5]->LoadFromASC(puma->m_pd3dDevice,"dat\\tree4.ase");
		meshes[5]->LoadTexture(puma->m_pd3dDevice,"dat\\f_tree.png",1);
		meshes[5]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);

		meshes[6] = new PumaMesh();
		meshes[6]->LoadFromASC(puma->m_pd3dDevice,"dat\\t-tree1.ase");
		meshes[6]->LoadTexture(puma->m_pd3dDevice,"dat\\t-tree1.png",1);
		meshes[6]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[7] = new PumaMesh();
		meshes[7]->LoadFromASC(puma->m_pd3dDevice,"dat\\t-tree2.ase");
		meshes[7]->LoadTexture(puma->m_pd3dDevice,"dat\\t-tree2.png",1);
		meshes[7]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);

		meshes[8] = new PumaMesh();
		meshes[8]->LoadFromASC(puma->m_pd3dDevice,"dat\\s-tree1.ase");
		meshes[8]->LoadTexture(puma->m_pd3dDevice,"dat\\s-tree1.png",1);
		meshes[8]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[9] = new PumaMesh();
		meshes[9]->LoadFromASC(puma->m_pd3dDevice,"dat\\s-tree2.ase");
		meshes[9]->LoadTexture(puma->m_pd3dDevice,"dat\\s-tree2.png",1);
		meshes[9]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);

		meshes[10] = new PumaMesh();
		meshes[10]->LoadFromASC(puma->m_pd3dDevice,"dat\\palm.ase");
		meshes[10]->LoadTexture(puma->m_pd3dDevice,"dat\\palm.png",1);
		meshes[10]->Scale(puma->m_pd3dDevice,0.0015f, 0.0015f, 0.0015f);
		meshes[11] = new PumaMesh();
		meshes[11]->LoadFromASC(puma->m_pd3dDevice,"dat\\gmush.ase");
		meshes[11]->LoadTexture(puma->m_pd3dDevice,"dat\\gmush.png",1);
		meshes[11]->Scale(puma->m_pd3dDevice,0.04f, 0.04f, 0.04f);

		meshes[12] = new PumaMesh();
		meshes[12]->LoadFromASC(puma->m_pd3dDevice,"dat\\rocks.ase");
		meshes[12]->LoadTexture(puma->m_pd3dDevice,"dat\\rocks.png",1);
		meshes[12]->Scale(puma->m_pd3dDevice,0.001f, 0.001f, 0.001f);
//		meshes[2]->LoadFromASC(puma->m_pd3dDevice,"dat\\column.ase");
//		meshes[2]->LoadTexture(puma->m_pd3dDevice,"dat\\column.png",1);
//		meshes[2]->Scale(puma->m_pd3dDevice,0.01f, 0.01f, 0.01f);
		meshes[13] = new PumaMesh();
		meshes[13]->LoadFromASC(puma->m_pd3dDevice,"dat\\cabin.ase");
		meshes[13]->LoadTexture(puma->m_pd3dDevice,"dat\\cabin.png",1);
		meshes[13]->Scale(puma->m_pd3dDevice,0.004f, 0.004f, 0.004f);
		meshes[14] = new PumaMesh();
		meshes[14]->LoadFromASC(puma->m_pd3dDevice,"dat\\cabin2.ase");
		meshes[14]->LoadTexture(puma->m_pd3dDevice,"dat\\cabin.png",1);
		meshes[14]->Scale(puma->m_pd3dDevice,0.004f, 0.004f, 0.004f);

		meshes[15] = new PumaMesh();
//		meshes[15]->LoadFromASC(puma->m_pd3dDevice, "dat\\petHeadStone1.ASE");
		meshes[15]->LoadCompressed(puma->m_pd3dDevice, "dat\\petHeadStone1.MEC");
		meshes[15]->Scale(puma->m_pd3dDevice, 0.0110f, 0.0110f, 0.0110f);
		meshes[15]->LoadTexture(puma->m_pd3dDevice,	"dat\\petHeadStone1.png");
		meshes[16] = new PumaMesh();
//		meshes[16]->LoadFromASC(puma->m_pd3dDevice, "dat\\petHeadStone2.ASE");
		meshes[16]->LoadCompressed(puma->m_pd3dDevice, "dat\\petHeadStone2.MEC");
		meshes[16]->Scale(puma->m_pd3dDevice, 0.0080f, 0.0080f, 0.0080f);
		meshes[16]->LoadTexture(puma->m_pd3dDevice,	"dat\\petHeadStone2.png");

		meshes[17] = new PumaMesh();
		meshes[17]->LoadFromASC(puma->m_pd3dDevice, "dat\\flower1.ASE");
		meshes[17]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
		meshes[17]->LoadTexture(puma->m_pd3dDevice,	"dat\\grassTexture.png");
		meshes[18] = new PumaMesh();
		meshes[18]->LoadFromASC(puma->m_pd3dDevice, "dat\\flower2.ASE");
		meshes[18]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
		meshes[18]->LoadTexture(puma->m_pd3dDevice,	"dat\\grassTexture.png");
		meshes[19] = new PumaMesh();
		meshes[19]->LoadFromASC(puma->m_pd3dDevice, "dat\\flower3.ASE");
		meshes[19]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
		meshes[19]->LoadTexture(puma->m_pd3dDevice,	"dat\\grassTexture.png");
		meshes[20] = new PumaMesh();
		meshes[20]->LoadFromASC(puma->m_pd3dDevice, "dat\\flower4.ASE");
		meshes[20]->Scale(puma->m_pd3dDevice, 0.0020f, 0.0010f, 0.0020f);
		meshes[20]->LoadTexture(puma->m_pd3dDevice,	"dat\\grassTexture.png");
		meshes[21] = new PumaMesh();
		meshes[21]->LoadFromASC(puma->m_pd3dDevice, "dat\\mushroom.ASE");
		meshes[21]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
		meshes[21]->LoadTexture(puma->m_pd3dDevice,	"dat\\mushroom.png");

		meshes[22] = new PumaMesh();
		meshes[22]->LoadFromASC(puma->m_pd3dDevice, "dat\\grass-hut.ASE");
		meshes[22]->Scale(puma->m_pd3dDevice, 0.0040f, 0.0040f, 0.0040f);
		meshes[22]->LoadTexture(puma->m_pd3dDevice,	"dat\\grass-hut.png");

	}
	++refCount;

}

//******************************************************************
BBOStatic::~BBOStatic()
{
	--refCount;
	if (refCount < 1)
	{
		delete meshes[21];
		delete meshes[20];
		delete meshes[19];
		delete meshes[18];
		delete meshes[17];
		delete meshes[16];
		delete meshes[15];
		delete meshes[14];
		delete meshes[13];
		delete meshes[12];
		delete meshes[11];
		delete meshes[10];
		delete meshes[9];
		delete meshes[8];
		delete meshes[7];
		delete meshes[6];
		delete meshes[5];
		delete meshes[4];
		delete meshes[3];
		delete meshes[2];
		delete meshes[1];
		delete meshes[0];
	}

	SAFE_DELETE(flowerLocList);
}



/* end of file */



