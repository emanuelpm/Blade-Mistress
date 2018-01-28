#ifndef READINI_H
#define READINI_H

class IniReader
{
public:
	IniReader(char *fileName);
	~IniReader();

	int ReadNextToken(char *label, char* value);

	FILE *fp;


};


#endif