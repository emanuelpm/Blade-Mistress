//**********************************************************************************
// a file of Thom's new game, "American Superheros", destined to catapult
// Aggressive Game Designs into the big time!
// incept August 19, 1998
//**********************************************************************************


#include <stdio.h>
#include <io.h>
#include <time.h>
#include "fileFind.h"

struct NameRecord
{
	NameRecord() {next = NULL; };

	NameRecord *next;
   char name[MAX_FILE_NAME_SIZE];
};

FileNameList *GetNameList(char *wildcard, char*path)
{
//   char nameList[500][MAX_FILE_NAME_SIZE];
	NameRecord *first;
   int index = 0;
   
   struct _finddata_t c_file;
   long hFile;
   /* Find first .c file in current directory */
   if( (hFile = _findfirst( wildcard, &c_file )) == -1L )
	  return NULL;
   else   
   {
		NameRecord *nr = new NameRecord();
	  sprintf(nr->name,c_file.name);
		first = nr;
	  index++;
	  while( _findnext( hFile, &c_file ) == 0 )            
	  {
			nr = new NameRecord();
		 sprintf(nr->name,c_file.name);
			nr->next = first;
			first = nr;

		 index++;
	  }
	  _findclose( hFile );   
   }

   FileNameList *ret = new FileNameList(index);
   for (int i = 0; i < index; i++)
	{
	  sprintf(ret->nameList[i].name,"%s%s",path,first->name);
		NameRecord *nr = first;
		first = nr->next;
		delete nr;
	}
   return ret;
}


