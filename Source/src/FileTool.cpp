
//***************************************************************
//***************************************************************
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "FileTool.h"



//************************************************************************
int FileReader::NextWord(char * currentLine, int *linePoint)
{
	int retVal = *linePoint;

	while (	!isspace(currentLine[(*linePoint)]) )
		(*linePoint)++;
	while (	isspace(currentLine[(*linePoint)]) )
		(*linePoint)++;

	return(retVal);
}

/*
//*************************************************************************
void FileReader::LoadLineToString(FILE *dest, char *string)
{
   fgets(string, 2047, dest);
   int i = strlen(string) - 1;
   char c = string[i];
   while ('\n' == string[i])
   {
      string[i] = 0;
      i = strlen(string) - 1; 
   }
}
*/

//***************************************************************
FileReader::FileReader(char *filename)
{

	ready = FALSE;

    file = fopen(filename,"rb");

    if (file)
		ready = TRUE;

	currentLine[0] = 0;
}

//***************************************************************
FileReader::~FileReader()
{

	if (ready)
		fclose( file );
}

//***************************************************************
int FileReader::ReadToken(void)
{
	if (!ready)
		return FALSE;

	while (!currentLine[0] || argPoint == strlen(currentLine))
	{
        if (NULL == fgets( currentLine, 2047, file ))
        {
            ready = FALSE;
            return FALSE;
        }
        linePoint = 0;

        // skip leading whitespace.
        while (	isspace(currentLine[linePoint]) )
            linePoint++;

        argPoint = NextWord(currentLine,&linePoint);
		if (';' == currentLine[argPoint] || 0 == currentLine[argPoint] || '/' == currentLine[argPoint])
			currentLine[0] = 0;
	}

	sscanf(&(currentLine[argPoint]),"%s", valString);
	valLong   = atol(valString);
	valDouble = atof(valString);

    argPoint = NextWord(currentLine,&linePoint);

	return TRUE;

}


//***************************************************************
int FileReader::ReadLine(void)
{
	if (!ready)
		return FALSE;

	while (!currentLine[0] || argPoint == strlen(currentLine))
	{
        if (NULL == fgets( currentLine, 2047, file ))
        {
            ready = FALSE;
            return FALSE;
        }
        linePoint = 0;

        // skip leading whitespace.
        while (	isspace(currentLine[linePoint]) )
            linePoint++;

        argPoint = NextWord(currentLine,&linePoint);
		if (';' == currentLine[argPoint])
			currentLine[0] = 0;
	}

	sprintf(valString,&(currentLine[argPoint]));
	while(10 == valString[strlen(valString)-1] || 13 == valString[strlen(valString)-1])
		valString[strlen(valString)-1] = 0;
	valLong   = atol(valString);
	valDouble = atof(valString);

	argPoint = strlen(currentLine);

	return TRUE;

}




//***************************************************************
FileWriter::FileWriter(char *filename)
{

	ready = FALSE;

    file = fopen(filename,"w");

    if (file)
		ready = TRUE;

}

//***************************************************************
FileWriter::~FileWriter()
{
	if (ready)
		fclose( file );
}

//***************************************************************
void FileWriter::WriteString(char   *input)
{
	if (!ready)
		return;

	fprintf(file,"%s ",input);
}

//***************************************************************
void FileWriter::WriteLong  (long   input)
{
	if (!ready)
		return;

	fprintf(file,"%ld ",input);
}

//***************************************************************
void FileWriter::WriteDouble(double input)
{
	if (!ready)
		return;

	fprintf(file,"%f ",input);
}

//***************************************************************
void FileWriter::NewLine    (void)
{
	if (!ready)
		return;

	fprintf(file,"\n");
}




/* end of file */



