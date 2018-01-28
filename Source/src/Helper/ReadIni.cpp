#include <windows.h>
#include <windowsx.h>
#include <fstream>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "GeneralUtils.h"
#include "ReadIni.h"

//*******************************************************************************
IniReader::IniReader(char *fileName)
{
	fp = fopen(fileName, "r");

}

//*******************************************************************************
IniReader::~IniReader()
{
	if (fp)
		fclose(fp);

}

//*******************************************************************************
int IniReader::ReadNextToken(char *label, char* value)
{
	char fileBuffer[1028];
	char tempText[1028];
	int done = FALSE, result = TRUE;
	int eorFlag;

	while (!done)
	{
//		int result = fscanf(fp,"%s",fileBuffer);
//		int result = 0;
		LoadLineToString(fp, fileBuffer, &eorFlag);

		int start = 0;
		while (start < (int)strlen(fileBuffer) && ' ' == fileBuffer[start])
			++start;
		if (start >= (int)strlen(fileBuffer))
		{
			sprintf(tempText,";");
		}
		else
			sprintf(tempText, &fileBuffer[start]);  // get rid of whitespace

		if ('/' == tempText[0] || ';' == tempText[0] || '[' == tempText[0])
			;
		else
		{
			// look for the = sign
			start = 0;
			while (start < (int)strlen(tempText) && '=' != tempText[start])
				++start;
			if (start >= (int)strlen(tempText))
			{
				sprintf(tempText,";");
			}
			else
			{
				// fill in the label
				sprintf(label, tempText);
				label[start] = 0;

			// fill in the value
				sprintf(value, &tempText[start+1]);

				// cleanup label
				sprintf(tempText, label);
				start = 0;
				while (start < (int)strlen(tempText) && ' ' != tempText[start])
					++start;
				if (start >= (int)strlen(tempText))
					sprintf(tempText,";");
				else
					sprintf(label, tempText);

				// cleanup value
				sprintf(tempText, value);
				start = 0;
				while (start < (int)strlen(tempText) && ' ' != tempText[start])
					++start;
				if (start >= (int)strlen(tempText))
					sprintf(tempText,";");
				else
					sprintf(value, tempText);

			}

			done = TRUE;
			if (!eorFlag)
				result = FALSE;
		}

		if (!eorFlag)
		{
			done = TRUE;
			result = FALSE;
		}
	}

	return result;
}



/* end of file */