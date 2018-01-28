// NameRipper.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stdafx.h"
#include "..\src\helper\fileFind.h"

#include "..\src\BBO-SAvatar.h"
#include "..\src\BBO.h"
#include "..\src\helper\GeneralUtils.h"
#include "..\src\helper\UniqueNames.h"
#include "..\src\version.h"

DoublyLinkedList accountList;

DoublyLinkedList dataList, tempList;

//***************************************************************************************
//***************************************************************************************
int main(int argc, char* argv[])
{

	//load EVERY account into memory
	char tempText[1024];

	FileNameList *list = GetNameList("users\\*", "");

	FILE *fp = fopen("nameripperoutput.txt","w");

	for (int i = 0; i < list->numOfFiles; ++i)
	{
		// looking for a period, which would indicate a file
		int isFile = FALSE;
		for (int j = 0; j < (int) strlen(list->nameList[i].name); ++j)
		{
			if ('.' == list->nameList[i].name[j])
				isFile = TRUE;
		}

		if (isFile)
		{
			if (!strncmp(".",list->nameList[i].name,1))
				;
			else if (!strncmp("..",list->nameList[i].name,2))
				;
			else
			{
				sprintf(tempText,"users\\%s", list->nameList[i].name);

				BBOSAvatar *curAvatar = new BBOSAvatar();
				curAvatar->LoadAccount(tempText, NULL, FALSE, TRUE);
				
				// save the names into the unique name system
				for (int i = 0; i < NUM_OF_CHARS_PER_USER; ++i)
				{
					if (curAvatar->charInfoArray[i].topIndex > -1)
					{
						fprintf(fp,"Checking %s..... ", curAvatar->charInfoArray[i].name );
						if( UN_IsNameUnique( curAvatar->charInfoArray[i].name ) )
						{
							fprintf(fp, "%s\n", "IS UNIQUE!! ADDING!" );
							UN_AddName( curAvatar->charInfoArray[i].name );
						}
						else
						{
							fprintf(fp, "%s\n", "Not Unique" );
						}
					}
				}
			}
		}
	}

	fclose( fp );


	return 0;
}
