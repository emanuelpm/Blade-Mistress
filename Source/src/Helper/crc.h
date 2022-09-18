#ifndef CRC_H
#define CRC_H

#include <string>

extern unsigned long GetCRC(char * fileName, unsigned long& crc32Out);
extern unsigned long GetCRCForString(char * string);

extern unsigned long GetCRC(std::string fileName, unsigned long& crc32Out);
extern unsigned long GetCRCForString(std::string string);

#endif
