// don't forget to link with wsock32.lib

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "GetIP.h"


//********************************************************************************
GetIPTool::GetIPTool(void)
{

   numOfIPs = 0;
   ready = FALSE;
/*
   WSAData wsaData;
   if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0)
      ready = FALSE;
   else
   */
      ready = TRUE;

}

//********************************************************************************
GetIPTool::~GetIPTool()
{
//   if (ready)
//      WSACleanup();
}

//********************************************************************************
void GetIPTool::GetIPs(void)
{
   if (!ready)
      return;

    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) 
    {
        return;
    }

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) 
    {
        return;
    }

    for (int i = 0; phe->h_addr_list[i] != 0 && i < 10; ++i) 
    {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        sprintf(ipText[numOfIPs],"%s",inet_ntoa(addr));
        numOfIPs++;
    }
    
}

//char ipText[10][64];
//int numOfIPs;

/*

int doit(int, char **)
{
    char ac[80];
    if (gethostname(ac, sizeof(ac)) == SOCKET_ERROR) {
        cerr << "Error " << WSAGetLastError() <<
                " when getting local host name." << endl;
        return 1;
    }
    cout << "Host name is " << ac << "." << endl;

    struct hostent *phe = gethostbyname(ac);
    if (phe == 0) {
        cerr << "Yow! Bad host lookup." << endl;
        return 1;
    }

    for (int i = 0; phe->h_addr_list[i] != 0; ++i) {
        struct in_addr addr;
        memcpy(&addr, phe->h_addr_list[i], sizeof(struct in_addr));
        cout << "Address " << i << ": " << inet_ntoa(addr) << endl;
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(1, 1), &wsaData) != 0) {
        return 255;
    }

    int retval = doit(argc, argv);

    WSACleanup();

    return retval;
}

  */

/* end of file */
