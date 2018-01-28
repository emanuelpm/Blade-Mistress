//***************************************************************************************
// Mesh object designed for PUMA and spaceship parts.
//***************************************************************************************

#include "puma.h"
#include "pumaRawVerts.h"
#include "..\helper\GeneralUtils.h"
#include "..\helper\autolog.h"


//***************************************************************************************
RawVerts::RawVerts(int doid, char *doname)	 : DataObject(doid,doname)
{

	isReady = FALSE;

	numOfVertices = 0;
	vertexList = NULL;

}

//***************************************************************************************
RawVerts::RawVerts(void) : DataObject(0,"PUMAMESH")
{

	isReady = FALSE;

	numOfVertices = 0;
	vertexList = NULL;

}

//***************************************************************************************
RawVerts::~RawVerts()
{

	if (vertexList)
		delete[] vertexList;

}

//***************************************************************************************
void RawVerts::LoadFromASC(char *fileName)
{

	FILE *fp;

	int numFaces   = 0;
	int numVertPos = 0;
	float *vertPosArray = NULL;
	int numVertUVs = 0;
	float *vertUVArray  = NULL;

	int tempIndex;
	int faceRead = 0;

	fp = fopen(fileName,"r");

	char fileBuffer[1028];
	int done = FALSE;

	while (!done)
	{
		int result = fscanf(fp,"%s",fileBuffer);

		if (!strncmp("*MESH_VERTEX_LIST",fileBuffer,strlen("*MESH_VERTEX_LIST")))
		{
		}
		else if (!strncmp("*MESH_VERTEXNORMAL",fileBuffer,strlen("*MESH_VERTEXNORMAL")))
		{
		}
		else if (!strncmp("*MESH_VERTEX",fileBuffer,strlen("*MESH_VERTEX")))
		{
			result = fscanf(fp,"%d",&tempIndex);
			result = fscanf(fp,"%f",&(vertexList[tempIndex].x));
			result = fscanf(fp,"%f",&(vertexList[tempIndex].z));
			result = fscanf(fp,"%f",&(vertexList[tempIndex].y));
		}
		else if (!strncmp("*MESH_NUMVERTEX",fileBuffer,strlen("*MESH_NUMVERTEX")))
		{
			result = fscanf(fp,"%d",&numOfVertices);
			// Create the vertex buffer.

			vertexList = new D3DXVECTOR3[numOfVertices];

		}

		if (EOF == result)
			done = TRUE;
	}

	fclose(fp);


}

//***************************************************************************************
void RawVerts::Assimilate(RawVerts *pm)
{
	D3DXVECTOR3 *newVerts;
	int totalVerts = pm->numOfVertices + numOfVertices;

	newVerts = new D3DXVECTOR3[totalVerts];


	int i = 0;
	// copy my verts first.
	for (i = 0; i < numOfVertices; ++i)
	{
		newVerts[i] = vertexList[i];
	}

	for (; i < totalVerts; ++i)
	{
		newVerts[i] = pm->vertexList[i-numOfVertices];
	}

	delete vertexList;

	vertexList = newVerts;
	numOfVertices   = totalVerts;

}


//***************************************************************************************
void RawVerts::Scale(float x, float y, float z)
{
	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		vertexList[i].x *= x;
		vertexList[i].y *= y;
		vertexList[i].z *= z;
	}
}

//***************************************************************************************
void RawVerts::Shift(float x, float y, float z)
{
	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		vertexList[i].x += x;
		vertexList[i].y += y;
		vertexList[i].z += z;
	}
}

//***************************************************************************************
void RawVerts::Mirror(void)
{
	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		// flip
		vertexList[i].x *= -1;
	}
}

//***************************************************************************************
void RawVerts::MirrorZ(void)
{
	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		// flip
		vertexList[i].z *= -1;
	}
}

//***************************************************************************************
RawVerts *RawVerts::MakeCopy(void)
{
	RawVerts *p = new RawVerts(WhatAmI(),"PUMAMESH_COPY");

	p->vertexList = new D3DXVECTOR3[numOfVertices];
	p->numOfVertices = numOfVertices;

	int i = 0;
	for (i = 0; i < numOfVertices; ++i)
	{
		p->vertexList[i] = vertexList[i];
	}

	return p;
}


//***************************************************************************************
void RawVerts::Center(void)
{
	D3DXVECTOR3 least, most;
	least.x = least.y = least.z =  10000.0f;
	most.x  = most.y  = most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (vertexList[i].x > most.x)
			 most.x  = vertexList[i].x;
		if (vertexList[i].x < least.x)
			 least.x = vertexList[i].x;

		if (vertexList[i].y > most.y)
			 most.y  = vertexList[i].y;
		if (vertexList[i].y < least.y)
			 least.y = vertexList[i].y;

		if (vertexList[i].z > most.z)
			 most.z  = vertexList[i].z;
		if (vertexList[i].z < least.z)
			 least.z = vertexList[i].z;
	}

	// figure out delta to center
	D3DXVECTOR3 center;
	center.x = most.x - ((most.x - least.x) / 2);
	center.y = most.y - ((most.y - least.y) / 2);
	center.z = most.z - ((most.z - least.z) / 2);

	// apply that delta!
	for (int i = 0; i < numOfVertices; ++i)
	{
		vertexList[i].x -= center.x;
		vertexList[i].y -= center.y;
		vertexList[i].z -= center.z;
	}
}

//***************************************************************************************
void RawVerts::GetBounds(D3DXVECTOR3 &least, D3DXVECTOR3 &most)
{
	least.x = least.y = least.z =  10000.0f;
	most.x  = most.y  = most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (vertexList[i].x > most.x)
			 most.x  = vertexList[i].x;
		if (vertexList[i].x < least.x)
			 least.x = vertexList[i].x;

		if (vertexList[i].y > most.y)
			 most.y  = vertexList[i].y;
		if (vertexList[i].y < least.y)
			 least.y = vertexList[i].y;

		if (vertexList[i].z > most.z)
			 most.z  = vertexList[i].z;
		if (vertexList[i].z < least.z)
			 least.z = vertexList[i].z;
	}
}

//***************************************************************************************
void RawVerts::Rotate90(void)
{

	float tempX, tempZ;

	// apply that delta!
	for (int i = 0; i < numOfVertices; ++i)
	{
		tempX = vertexList[i].x;
		tempZ = vertexList[i].z;
		vertexList[i].x = -tempZ;
		vertexList[i].z = tempX;
	}
}


//***************************************************************************************
float RawVerts::SizeX(void)
{
	D3DXVECTOR3 least, most;
	least.x =  10000.0f;
	most.x  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (vertexList[i].x > most.x)
			 most.x  = vertexList[i].x;
		if (vertexList[i].x < least.x)
			 least.x = vertexList[i].x;
	}

	return most.x - least.x;

}

//***************************************************************************************
float RawVerts::SizeY(void)
{
	D3DXVECTOR3 least, most;
	least.y =  10000.0f;
	most.y  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (vertexList[i].y > most.y)
			 most.y  = vertexList[i].y;
		if (vertexList[i].y < least.y)
			 least.y = vertexList[i].y;
	}

	return most.y - least.y;

}

//***************************************************************************************
float RawVerts::SizeZ(void)
{
	D3DXVECTOR3 least, most;
	least.z =  10000.0f;
	most.z  = -10000.0f;

	// find bounds of entire object
	for (int i = 0; i < numOfVertices; ++i)
	{
		if (vertexList[i].z > most.z)
			 most.z  = vertexList[i].z;
		if (vertexList[i].z < least.z)
			 least.z = vertexList[i].z;
	}

	return most.z - least.z;

}








/* end of file */
