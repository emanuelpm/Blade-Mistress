
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <time.h>
#include "UniqueNames.h"
#include "GeneralUtils.h"
#include "..\version.h"

#include <Shlobj.h>
#include <direct.h>

char *endOfFileMarker = "************************************";

//*******************************************************************************
void UN_GetNameFile(char *name, char *fileName)
{
	char firstTwo[3];
	firstTwo[2] = 0;

	firstTwo[0] = tolower(name[0]);
	if (!isalpha(firstTwo[0]))
		firstTwo[0] = 'z';

	firstTwo[1] = tolower(name[1]);
	if (!isalpha(firstTwo[1]))
		firstTwo[1] = 'z';

	sprintf(fileName,"names\\%s-names.dat",firstTwo);

	// Make sure folder exists
	// Clean this up
	char cCurrentPath[FILENAME_MAX];
	_getcwd(cCurrentPath, sizeof(cCurrentPath));

	char dirHierarchy[FILENAME_MAX];
	sprintf_s(dirHierarchy, "%s\\names\\", cCurrentPath);
	SHCreateDirectoryEx(NULL, dirHierarchy, NULL);
}

//*******************************************************************************
int UN_IsNameUnique(char *name)
{
	char tempText[1024], fileName[128], compareName[128];
	sprintf(tempText, name);
	CleanString(tempText);

	UN_GetNameFile(tempText,fileName);

	FILE *fp = fopen(fileName,"r");
	if (!fp)
		return TRUE;

	LoadLineToString(fp, compareName);
	while(!IsSame(compareName, endOfFileMarker))
	{
		if (IsCompletelyVisiblySame(compareName, name))
		{
			fclose(fp);
			return FALSE;
		}
		LoadLineToString(fp, compareName);
	}

	fclose(fp);
	return TRUE;
}

//*******************************************************************************
void UN_AddName(char *name)
{
	char tempText[1024], fileName[128], compareName[128];
	sprintf(tempText, name);
	CleanString(tempText);

	UN_GetNameFile(tempText,fileName);

	FILE *fp = fopen(fileName,"r");
	if (!fp)
	{
		fp = fopen(fileName,"w");
		if (fp)
		{
			fprintf(fp,"%s\n",tempText);
			fprintf(fp,"%s\n",endOfFileMarker);
			fclose(fp);
		}
		return;
	}

	DoublyLinkedList list;

	LoadLineToString(fp, compareName);
	while(!IsSame(compareName, endOfFileMarker))
	{
		DataObject *dat = new DataObject(0,compareName);
		list.Append(dat);

		LoadLineToString(fp, compareName);
	}

	fclose(fp);

	DataObject *dat = new DataObject(0,tempText);
	list.Append(dat);

	fp = fopen(fileName,"w");
	if (fp)
	{
		dat = list.First();
		while (dat)
		{
			fprintf(fp,"%s\n",dat->WhoAmI());
			dat = list.Next();
		}

		fprintf(fp,"%s\n",endOfFileMarker);
		fclose(fp);
	}
}

//*******************************************************************************
void UN_RemoveName(char *name)
{
	char tempText[1024], fileName[128], compareName[128];
	sprintf(tempText, name);
	CleanString(tempText);

	UN_GetNameFile(tempText,fileName);

	FILE *fp = fopen(fileName,"r");
	if (!fp)
	{
		return;
	}

	DoublyLinkedList list;

	LoadLineToString(fp, compareName);
	while(!IsSame(compareName, endOfFileMarker))
	{
		if (!IsSame(compareName, tempText))
		{
			DataObject *dat = new DataObject(0,compareName);
			list.Append(dat);
		}
		LoadLineToString(fp, compareName);
	}

	fclose(fp);

	fp = fopen(fileName,"w");
	if (fp)
	{
		DataObject *dat = list.First();
		while (dat)
		{
			fprintf(fp,"%s\n",dat->WhoAmI());
			dat = list.Next();
		}

		fprintf(fp,"%s\n",endOfFileMarker);
		fclose(fp);
	}

}


/* end of file */



