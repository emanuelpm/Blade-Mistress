#ifndef GETIP_H
#define GETIP_H

#include <winsock.h>


class GetIPTool
{
public:

   GetIPTool(void);
   virtual ~GetIPTool();

   void GetIPs(void);

   char ipText[10][64];
   int numOfIPs, ready;

};












#endif
