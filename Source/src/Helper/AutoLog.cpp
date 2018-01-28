
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include "AutoLog.h"
#include "../version.h"

//const float VERSION_NUMBER = 0.5f;

AutoLog aLog;

//*******************************************************************************
void AutoLog::Start(char *appName)
{
	char tempText[1024];

	logEnabled = FALSE;

	FILE *fp = fopen("als.dat","r");
	if (fp)
	{
		int i;
		fscanf(fp,"%d",&i);
		fclose(fp);

		if (0 == i)
			logEnabled = TRUE;
	}

	fp = fopen("als.dat","w");
	if (fp)
	{
		fprintf(fp,"0");
		fclose(fp);
	}

	if (logEnabled)
	{
		fp = fopen("autolog.dat","w");
		if (fp)
		{
			_strdate( tempText );
			fprintf(fp, "Autolog for %s v%f,  %s, ", appName, VERSION_NUMBER, tempText );
			_strtime( tempText );
			fprintf(fp, "%s\n\n", tempText );

			fclose(fp);
		}
	}

}

//*******************************************************************************
void AutoLog::Append(void)
{
	char tempText[1024];

	logEnabled = TRUE;

	if (logEnabled)
	{
		FILE *fp = fopen("autolog.dat","a");
		if (fp)
		{
			fprintf(fp, "    ***********  Further info **********\n");

			fclose(fp);
		}
	}

}

//*******************************************************************************
void AutoLog::Finish(void)
{
	FILE *fp = fopen("als.dat","w");
	if (fp)
	{
		fprintf(fp,"1");
		fclose(fp);
	}

	Log("\n\nFINISHED\n\n");

	logEnabled = FALSE;

}

//*******************************************************************************
void AutoLog::Log(char *string)
{
	if (logEnabled)
	{
		FILE *fp = fopen("autolog.dat","a");
		if (fp)
		{
			fprintf(fp,string);
			fclose(fp);
		}
	}
}

//*******************************************************************************
void AutoLog::Log(int i)
{
	if (logEnabled)
	{
		FILE *fp = fopen("autolog.dat","a");
		if (fp)
		{
			fprintf(fp,"%d",i);
			fclose(fp);
		}
	}
}

//*******************************************************************************
void AutoLog::Log(float f)
{
	if (logEnabled)
	{
		FILE *fp = fopen("autolog.dat","a");
		if (fp)
		{
			fprintf(fp,"%f",f);
			fclose(fp);
		}
	}
}




/* end of file */



