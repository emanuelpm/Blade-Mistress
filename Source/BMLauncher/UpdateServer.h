#ifndef __UPDATESERVER_H
#define __UPDATESERVER_H

struct UpdateServer
{
	static const int iServerUrlSize = 1024;
	char pszServerURL[iServerUrlSize];

	static const int iServerNameSize = 256;
	char pszServerName[iServerNameSize];
};

#endif