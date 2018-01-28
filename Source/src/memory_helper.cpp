

#ifdef _DETECT_LEAKS


	#include <windows.h>
	#include <stdlib.h>
	#include <malloc.h>
	#include <memory.h>
	#include <tchar.h>
	#include <stdlib.h>
	#include <crtdbg.h>
	#include <list>
	#include <fstream>

	#define CRTDBG_MAP_ALLOC

	using namespace std;


	typedef struct {
		DWORD	address;
		DWORD	size;
		char	file[64];
		DWORD	line;
	} ALLOC_INFO;


	typedef list<ALLOC_INFO*> AllocList;


	AllocList *allocList;



	void AddTrack( DWORD addr,  DWORD asize,  const char *fname, DWORD lnum ) {
		ALLOC_INFO *info;

		if(!allocList) {
			allocList = new( AllocList );
		}

		info = new( ALLOC_INFO );
		info->address = addr;
		strncpy(info->file, fname, 63);
		info->line = lnum;
		info->size = asize;
		allocList->insert(allocList->begin(), info);
	}


	void RemoveTrack(DWORD addr) {
		AllocList::iterator i;

		if(!allocList)
			return;

		for(i = allocList->begin(); i != allocList->end(); i++) {
			if((*i)->address == addr) {
				allocList->remove((*i));
				break;
			}
		}
	}


	void DumpUnfreed() {
		AllocList::iterator i;
		DWORD totalSize = 0;
		char buf[1024];

		ofstream out;
		out.open( "leaks.txt", ios::out );

		
		if(!allocList) {
			out << "No leaks detected!";	
			return;
		}

		for(i = allocList->begin(); i != allocList->end(); i++) {
			sprintf(buf, "%-50s:\t\tLINE %d,\t\tADDRESS %d\t%d unfreed\n",
				(*i)->file, (*i)->line, (*i)->address, (*i)->size);

			out << buf;

			totalSize += (*i)->size;
		}

		sprintf(buf, "-----------------------------------------------------------\n");
		out << buf;
		
		sprintf(buf, "Total Unfreed: %d bytes\n", totalSize);
		out << buf;

		out.close();
	}


	void DetectMemoryLeaksCRT() {
		_CrtSetDbgFlag( _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF );
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );
		_CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDERR );
	}


#endif