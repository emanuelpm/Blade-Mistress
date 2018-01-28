// ============================================================================
// 
// IOCPServer.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_IOCPSERVER_H
#define CHRONOS_IOCPSERVER_H

	// Needed Headers.
#include "DisableWarnings.h"
#include "IOCPSocket.h"
//#include "UDPSocket.h"
#include "../helper/Mutex.h"
#include "NetWorldRadio.h"
#include "../helper/Fifo.h"
#include <map>
#include "ISocket.h"
#include "ISocketHandler.h"

// =============================================
// These types are the lists.
// =============================================

typedef Fifo<Chronos::BStream *>		FifoBStream;

// =============================================
// This structure defines a mapping of TCP socket
// to a message fifo for failed sends.
// =============================================
/*
class SocketBlock
{
	public:
		SocketBlock() : m_socket(NULL), m_backupSendFifo(10000){}
		IOCPSocket *	m_socket;
		FifoBStream	m_backupSendFifo;
};
*/
typedef std::map<TagID, IOCPSocket *>			IOCPSocketMap;

// =============================================
// This structure contains a single message
// from a client.
// =============================================

class IOCPServerMessage
{
	public:
		IOCPServerMessage() : m_message(NULL), m_socketHandle(0){}
		Chronos::BStream *	m_message;
		int					m_socketHandle;	
};

#ifndef IOCPFifoServerMessage
typedef Fifo<IOCPServerMessage *>			IOCPFifoServerMessage;
#endif

// =============================================
// This object manages a single server instance
// on a single port.
// =============================================

class IOCPServer : public NetWorldRadio , public ISocketHandler
{
	public:

			// Creators.
		IOCPServer();
		virtual ~IOCPServer();

			// This function starts the server on a given port.
		bool		startServer(int serverPort, int maxConnections, void * context);

			// This function stops the current server that is running.
		bool		stopServer();

			// These functions get the list of connected users.
		IOCPSocket *	firstSocket();
		IOCPSocket * nextSocket();
		IOCPSocket * findSocket(int handle);

			// This function closes a client socket.
		void		closeClientSocket(IOCPSocket * socket);

			// These functions get the settings.
		int			port() const;
		int			maxConnections() const;
		int			curConnections() const;
		int			curSockets() const;
	
		virtual bool IsConnected(void);

		void LogMessage(char *);

		void CollectGarbage(void);

	public:

			// This function gets called when a socket completes a connection.
		virtual bool	OnConnect(int hSocket, int iError);

			// This function gets called when a socket accepts a socket connection.
		virtual bool	OnAccept(int hListenSocket, int hNewSocket, int iError);

			// This function gets called when a socket receives data.
		virtual int		OnRecv(int hSocket, const char * pData, int iSize, int iError);

			// This function gets called when a socket receives a disconnect.
		virtual bool	OnDisconnect(int hSocket, int iError);

			// This function gets called when a socket has backed up on sending data and 
			// it can now send.
		virtual void	OnWrite(int hSocket, int iError);

	public:

			// This virtual function provides the interface for sending
			// data.
		virtual void SendMsg(int size, const void *dataPtr, int flags = 0, std::vector<TagID> * receiptList = NULL);
		
			// This function gets the next message in the radio list
			// of messages sent to this radio.
		virtual void GetNextMsg(char *bufferToFill, int & size, int * fromSocket = NULL, std::vector<TagID> * receiptList = NULL);

		DWORD lastTimeUpdate, bytesIn, bytesOut;
		DWORD lastMessUpdate, messSent[256], messBytes[256];

	private:

			// These functions allow the Emulation class to connect
			// and disconnect form/to another NetWorldRadio class.
		virtual void Connect   (NetWorldRadio * other){}
		virtual void Disconnect(NetWorldRadio * other = NULL){}

			// This function appends a BStream to the packet list.
		virtual void Append(const Chronos::BStream * message){}

	private:

			// this function handles processing receive requests.
		int handleReceive(const char * buffer, int size, int socketHandle);

	private:

			// General settings.
		int					m_port;
		long					m_maxConnections;
		long					m_curConnections;
		void *				m_context;

			// this holds a list of the socket
//		CMap<int, int, SocketBlock *, SocketBlock *&>	m_clientList;
		IOCPSocketMap m_socketList;
		IOCPSocketMap::iterator	m_sit;

			// receive fifo.
		IOCPFifoServerMessage	m_recvFifo;

		Fifo<IOCPSocket *>	m_deleteFifo;


			// this holds the listen socket.
		IOCPSocket *			m_listenSocket;

			// this holds the mutex for locking out access to the socket list.
		Mutex				m_clLock;

		Mutex				m_mutex;
		Mutex				m_receiveMutex;
		Mutex				m_closeMutex;

};

// =============================================
// This function returns the current number 
// of connections.
// =============================================

inline int IOCPServer::curConnections() const
{
	return m_curConnections;
}

inline int IOCPServer::curSockets() const
{
	return m_socketList.size();
}

// =============================================
// This function returns the max number of 
// connections for this machine.
// =============================================

inline int IOCPServer::maxConnections() const
{
	return m_maxConnections;
}

// =============================================
// This function returns the port.
// =============================================

inline int IOCPServer::port() const
{
	return m_port;
}

#endif
