#ifndef NETSYSTEM_H
#define NETSYSTEM_H


#include <iostream.h>
#include <stdio.h>
#include <conio.h>
#include "SocketObject.h"
#include "Packets.h"
#include "linklist.h"



//**********************************************************
class ServerPlayer : public DataObject
{
public:

   ServerPlayer(int doid, char *doname);
   virtual ~ServerPlayer();

   SocketObject	ClientSocketObject;
   long			iID;
   char			szName[24];

};



//**********************************************************
//**********************************************************
extern void NetSystemStartup(void);
extern void NetSystemShutdown(void);

//**********************************************************
//**********************************************************
class NetClient
{
public:

    NetClient(char *szServerIP, int iServerListenPort);
    ~NetClient()                                      ;
    int  CheckForMessage( void )                      ;
    void WaitToGetID( void )                          ;
    void SendMessage(char *dataPtr, int dataLen)      ;

	SocketObject				ClientSocketObject;		// Client Socket Object
	int							iBytesReceived;		// # of Bytes Received
	int							iBytesSent;			// # of Bytes Sent
	char						szCommand[128];			// Chat Buffer
	char						szPacketBuffer[128000];
    ULONG ID;
    int  ready;

};

//**********************************************************
//**********************************************************
class NetServer
{
public:

   void          ServerInit( void );
   void          Cleanup( void );
   ServerPlayer *CheckForMessage( void );
   void          SendMessage(ServerPlayer * target, char *dataPtr, int dataLen);

   char			 szPacketBuffer[32768];
   int	 		 iBytesReceived;
   int	 		 iBytesSent;

   DoublyLinkedList serverPlayers;
};


extern NetServer *netServer;


#endif
