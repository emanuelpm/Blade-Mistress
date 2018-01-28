// IndexMaker.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "stdafx.h"
#include "..\src\helper\fileFind.h"
#include "..\src\helper\crc.h"

#include "../src/helper/PasswordHash.h"
#include "../src/helper/PBKDF2.h"


//*********************************************************************
void ProcessDirectory(char* root, char* dir, FILE * outputFile)
{
    char tempText[1024];

    sprintf_s(tempText, 1024, "%s\\%s\\*",root, dir);

    FileNameList *list = GetNameList(tempText, "");

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
                sprintf_s(tempText,1024,"%s\\%s\\%s",root,dir,list->nameList[i].name);
                HANDLE tempFile = CreateFile(tempText,GENERIC_READ,0, NULL, OPEN_EXISTING,
                             FILE_ATTRIBUTE_NORMAL, NULL);

                if (!tempFile)
                {
                    printf("Unable to open %s to get its time.\n",tempText);
                    exit(-1);
                }
 
                FILETIME created, accessed, written;
                if (!GetFileTime(	tempFile, &created, &accessed, &written))
                {
                    printf("Unable to get time info for %s.\n",tempText);
                    exit(-1);
                }

//				DWORD fileSize = GetFileSize( tempFile, NULL ); // NULL is for high-order size

                // close down file
                CloseHandle(tempFile);

                DWORD dwCrc32;
                DWORD crcResult = GetCRC(tempText, dwCrc32);
                assert(!crcResult);

                sprintf_s(tempText,1024,"%s\\%s",dir,list->nameList[i].name);

                short size = strlen(tempText);
                fwrite(&size, 2,1, outputFile);
                tempText[3] += 1;  // tag offset
                fwrite(tempText, size,1, outputFile);
                fwrite(&written, sizeof(FILETIME),1, outputFile);
//				fwrite(&fileSize, sizeof(DWORD),1, outputFile);
                fwrite(&dwCrc32, sizeof(DWORD),1, outputFile);
                printf("%s\n",tempText);
            }
        }
        else
        {
            sprintf_s(tempText,1024,"%s\\%s",dir,list->nameList[i].name);
            ProcessDirectory(root, tempText, outputFile);
        }

    }

    delete list;

}

void PrintUsage()
{
    printf("Usage: IndexMaker.exe <dir> <out file>\n");
    printf("   <dir> : Path to directory containing the update structure\n");
    printf("   <out file> : Path and name of the output file (usually called index.dat)\n\n");
}

//*********************************************************************
int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        PrintUsage();
        return 0;
    }

    FILE *fp = fopen(argv[2],"wb");
    if (!fp)
    {
        printf("Unable to open output file %s\n\n", argv[2]);
        PrintUsage();
        return -1;
    }

    ProcessDirectory(argv[1], ".", fp);

    short size = -1;
    fwrite(&size, 2, 1, fp);

    fclose(fp);
    
    return 0;
}
