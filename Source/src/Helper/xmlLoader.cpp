//***************************************************************************************
// bare XML parser/loader, designed to be overloaded
//***************************************************************************************

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "xmlLoader.h"

//***************************************************************************************
void xmlLoaderStartElement(void *userData, const char *name, const char **atts)
{
  int i;
  int *depthPtr = (int *)userData;
//  for (i = 0; i < *depthPtr; i++)
//    putchar('\t');
//  puts(name);
  *depthPtr += 1;
}

//***************************************************************************************
void xmlLoaderEndElement(void *userData, const char *name)
{
  int *depthPtr = (int *)userData;
  *depthPtr -= 1;
}

//***************************************************************************************
//***************************************************************************************
XMLLoader::XMLLoader(void)
{

}

//***************************************************************************************
XMLLoader::~XMLLoader()
{
}

//***************************************************************************************
int XMLLoader::Process(char *filename)
{
	char buf[BUFSIZ];
	XML_Parser parser = XML_ParserCreate(NULL);
	int done;
	int depth = 0;
	XML_SetUserData(parser, &depth);
	XML_SetElementHandler(parser, xmlLoaderStartElement, xmlLoaderEndElement);

	FILE *fp = fopen(filename, "r");
	if (!fp)
	{
		XML_ParserFree(parser);
		return 0;
	}

	do 
	{
		size_t len = fread(buf, 1, sizeof(buf), fp);
		done = len < sizeof(buf);
		if (!XML_Parse(parser, buf, len, done)) 
		{
			//      printf("%s at line %d\n",
			//	      XML_ErrorString(XML_GetErrorCode(parser)),
			//	      XML_GetCurrentLineNumber(parser));

			fclose(fp);
			XML_ParserFree(parser);
			return 0;
		}
	} while (!done);

	fclose(fp);
	XML_ParserFree(parser);
	return 1;
}


/* end of file */
