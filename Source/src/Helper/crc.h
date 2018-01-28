#ifndef CRC_H
#define CRC_H

#include <windows.h>

extern DWORD GetCRC(char * szFilename, DWORD &dwCrc32);

extern DWORD GetCRCForString(char * string);
#endif