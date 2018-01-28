
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>

#include "NetWorldRadio.h"

//***************************************************************
//***************************************************************
NetWorldRadioConnection::NetWorldRadioConnection( int doid, char *doname)
   : DataObject(doid,doname)
{
    clientConnection = NULL;
}

//***************************************************************
NetWorldRadioConnection::~NetWorldRadioConnection()
{

}

//***************************************************************
//***************************************************************
NetWorldRadioPacket::NetWorldRadioPacket( int s, void *dataToCopy, int doid, char *doname)
   : DataObject(doid,doname)
{
    size = s;
    if (size > 4000)
        size = 4000;
    dataPtr = new char[size];
    if (dataPtr)
    {
        memcpy(dataPtr,dataToCopy,size);
    }
}

//***************************************************************
NetWorldRadioPacket::~NetWorldRadioPacket()
{
    if (dataPtr)
        delete[] dataPtr;

}

//***************************************************************
//***************************************************************
NetWorldRadio::NetWorldRadio( int isServ )
{
    isServer = isServ;
    serverConnection = NULL;
}

//***************************************************************
NetWorldRadio::~NetWorldRadio()
{
   NetWorldRadioConnection *curClient;

   curClient = (NetWorldRadioConnection *)clientList.First();
   while (curClient)
   {
      clientList.Remove(curClient);
      delete curClient;
      curClient = (NetWorldRadioConnection *)clientList.First();
   }

   NetWorldRadioPacket *curPacket;

   curPacket = (NetWorldRadioPacket *)packetList.First();
   while (curPacket)
   {
      packetList.Remove(curPacket);
      delete curPacket;
      curPacket = (NetWorldRadioPacket *)packetList.First();
   }

}

//***************************************************************
void NetWorldRadio::Connect(NetWorldRadio *other)
{
    if (isServer)
    {
        ; // servers don't connect, they get connected to!
    }
    else
    {
        // if I'm already connected to a server...
        if(serverConnection)
        {
            serverConnection->Disconnect(this);
        }

        serverConnection = NULL;

        // DO NOT use Connect() recursively!
        NetWorldRadioConnection *rc = new NetWorldRadioConnection(1,"CLIENT");
        if (rc)
        {
            serverConnection = other;
            rc->clientConnection = this;
            other->clientList.Append(rc);
        }
    }

}

//***************************************************************
void NetWorldRadio::Disconnect(NetWorldRadio *other)
{
    if (isServer)
    {
       NetWorldRadioConnection *curClient;

       curClient = (NetWorldRadioConnection *)clientList.First();
       while (curClient)
       {
          if (curClient->clientConnection == other)
          {
              clientList.Remove(curClient);
              delete curClient;
              other->serverConnection = NULL;
          }
          curClient = (NetWorldRadioConnection *)clientList.Next();
       }

    }
    else
    {
        // other had better be a server!
        other->Disconnect(this);
    }

}

//***************************************************************
void NetWorldRadio::SendMessage(int size, void *dataPtr)
{
    if (isServer)
    {
       NetWorldRadioConnection *curClient;

       curClient = (NetWorldRadioConnection *)clientList.First();
       while (curClient)
       {
           if (curClient->clientConnection)
           {
               NetWorldRadioPacket *curPacket = new NetWorldRadioPacket(size, dataPtr);
               if (curPacket)
               {
                   curClient->clientConnection->packetList.Append(curPacket);
               }
           }
           curClient = (NetWorldRadioConnection *)clientList.Next();
       }

    }
    else
    {
        if (serverConnection)
        {
           NetWorldRadioPacket *curPacket = new NetWorldRadioPacket(size, dataPtr);
           if (curPacket)
           {
               serverConnection->packetList.Append(curPacket);
           }
        }
    }
}

//***************************************************************
// we will ASSUME that the buffer handed in is 4000 in size.
// if no message waiting, this returns 0, else it returns the size of the message put in the buffer.
int NetWorldRadio::GetNextMessage(char *bufferToFill)
{

   NetWorldRadioPacket *curPacket;

   curPacket = (NetWorldRadioPacket *)packetList.First();
   if (curPacket)
   {
       memcpy(bufferToFill, curPacket->dataPtr, curPacket->size);
       int size = curPacket->size;

       // packet is no longer required!
       packetList.Remove(curPacket);
       delete curPacket;

       return size;
   }
   return 0;
}

/* end of file */

