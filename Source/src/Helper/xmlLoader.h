#ifndef XML_LOADER_H
#define XML_LOADER_H

#include "linklist.h"
#include "xmlparse.h"

//***************************************************************************************
class XMLLoader
{
public:

	XMLLoader(void);
	virtual ~XMLLoader();

	virtual int Process(char *filename);
};


#endif
