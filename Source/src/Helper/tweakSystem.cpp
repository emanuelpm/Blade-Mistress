//***************************************************************************************
// tweak system, for adjusting game values thru a text file.
//***************************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "tweakSystem.h"

TweakSystem gTweakSystem;

//***************************************************************************************
TweakObject::TweakObject(int doid, char *doname)	 : DataObject(doid,doname)
{


}

//***************************************************************************************
TweakObject::~TweakObject()
{

}

//***************************************************************************************
//***************************************************************************************
TweakSystem::TweakSystem(void)
{

	// load all the tweaks
	FILE *fp;

	fp = fopen("tweaks.dat","r");
	if (!fp)
		return;

	char fileBuffer[1028];
	int done = FALSE;

	while (!done)
	{
		int result = fscanf(fp,"%s",fileBuffer);

		if (!strncmp("ENTRY",fileBuffer,strlen("ENTRY")))
		{
			result = fscanf(fp,"%s",fileBuffer);
			TweakObject *to = new TweakObject(0,fileBuffer);
			float tempF;
			result = fscanf(fp,"%f %f %f\n", &(to->value), &(to->low), &(to->high));
			fgets( to->description, 127, fp );
			char c = to->description[strlen(to->description)-1];
			while (10 == c)
			{
				to->description[strlen(to->description)-1] = 0;
				c = to->description[strlen(to->description)-1];
			}
			tweakObjects.Append(to);
		}

		if (EOF == result)
			done = TRUE;
	}

	fclose(fp);


}

//***************************************************************************************
TweakSystem::~TweakSystem()
{

	// save all the tweaks
	FILE *fp;
	fp = fopen("tweaks.dat","w");

	if (!fp)
		return;

	fprintf(fp,"**** Aggressive Game Design Tweak system 1.0\n****\n");
	fprintf(fp,"**** This file is read every time the program executes, and\n**** is re-written every time the program exits.\n");
	fprintf(fp,"**** You can change the behavior of the program by changing this file.\n");
	fprintf(fp,"**** Only change the first number after the 'ENTRY' keyword.  This is\n");
	fprintf(fp,"**** the floating point value associated with each tweak.\n");
	fprintf(fp,"**** The two numbers after that are the suggested minimum and maximum values.\n");
	fprintf(fp,"**** The description is for the tweak on the line above the description.\n");
	fprintf(fp,"****\n****\n");


	TweakObject *to = (TweakObject *) tweakObjects.First();
	while (to)
	{
		fprintf(fp,"ENTRY %s %f %f %f\n",
					to->WhoAmI(), to->value, to->low, to->high);
		fprintf(fp,"%s\n", to->description);

		to = (TweakObject *) tweakObjects.Next();
	}

	fclose(fp);


}

//***************************************************************************************
float TweakSystem::GetTweak(char *tag, float def, float low, float high, char *desc)
{
	TweakObject *to = (TweakObject *) tweakObjects.Find(tag);
	if (!to)
	{
		to = new TweakObject(0,tag);
		tweakObjects.Append(to);
		to->value = def;
		to->low = low;
		to->high = high;
		int dLen = strlen(desc);
		if (dLen > 127)
			dLen = 127;
		memcpy(to->description,desc, dLen);
		to->description[dLen] = 0;
	}

	return to->value;
}


/* end of file */
