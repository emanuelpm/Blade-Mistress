//**********************************************************************************
// a file of Thom's new game, "American Superheros", destined to catapult
// Aggressive Game Designs into the big time!
// incept August 19, 1998
//**********************************************************************************
#ifndef FILEFIND_H
#define FILEFIND_H

const int MAX_FILE_NAME_SIZE = 256;

struct nameListElement
{
   char name[MAX_FILE_NAME_SIZE];
};

struct FileNameList
{
   FileNameList(int size) { nameList = new nameListElement[size]; numOfFiles = size; };
   ~FileNameList()  { if (nameList) delete nameList; };

   int numOfFiles;
   nameListElement *nameList;
};


FileNameList *GetNameList(char *wildcard, char*path);

#endif
