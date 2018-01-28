#ifndef FILETOOL_H
#define FILETOOL_H

//***************************************************************
class FileReader
{
public:
	FileReader(char *filename);
	~FileReader();

	int NextWord(char * currentLine, int *linePoint);
	int ReadToken(void);
	int ReadLine(void);

	long    valLong;
	double  valDouble;
	char    currentLine[2048];
	char    valString[2048];

	FILE *file;
    int linePoint, argPoint;

	int ready;

};

//***************************************************************
class FileWriter
{
public:
	FileWriter(char *filename);
	~FileWriter();

	void WriteString(char   *input);
	void WriteLong  (long   input);
	void WriteDouble(double input);
	void NewLine    (void);

	FILE *file;

	int ready;

};



#endif
