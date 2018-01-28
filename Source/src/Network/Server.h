// ============================================================================
// 
// Server.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_SERVER_H
#define CHRONOS_SERVER_H

	// Needed Headers.
#include "DisableWarnings.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "../helper/Mutex.h"
#include "NetWorldRadio.h"
#include "../helper/Fifo.h"
#include <map>

// =============================================
// These types are the lists.
// =============================================

//typedef Chronos::Fifo<Chronos::BStream *>		FifoBStream;
typedef Fifo<Chronos::BStream *>		FifoBStream;

// =============================================
// This structure defines a mapping of TCP socket
// to a message fifo for failed sends.
// =============================================

class SocketBlock
{
	public:
		SocketBlock() : m_socket(NULL), m_backupSendFifo(10000){}
		TCPSocket *	m_socket;
		FifoBStream	m_backupSendFifo;
};

typedef std::map<TagID, SocketBlock *>			SocketMap;

// =============================================
// This structure contains a single message
// from a client.
// =============================================

class ServerMessage
{
	public:
		ServerMessage() : m_message(NULL), m_socketHandle(0){}
		Chronos::BStream *	m_message;
		int					m_socketHandle;	
};

typedef Fifo<ServerMessage *>			FifoServerMessage;

// =============================================
// This object manages a single server instance
// on a single port.
// =============================================

class Server : public NetWorldRadio
{
	public:

			// Creators.
		Server();
		virtual ~Server();

			// This function starts the server on a given port.
		bool		startServer(int serverPort, int maxConnections, NETCALLBACK callback, void * context);

			// This function stops the current server that is running.
		bool		stopServer();

			// These functions get the list of connected users.
		TCPSocket *	firstSocket();
		TCPSocket * nextSocket();
		TCPSocket * findSocket(int handle);

			// This function closes a client socket.
		void		closeClientSocket(TCPSocket * socket);

			// These functions get the settings.
		int			port() const;
		int			maxConnections() const;
		int			curConnections() const;
	
		virtual bool IsConnected(void);

			// The UDP callback.
		int	UDPNetCallback( UDPSocket *, struct _WSANETWORKEVENTS &, const char *, 
							int size, void * context);

	public:

			// This virtual function provides the interface for sending
			// data.
		virtual void SendMsg(int size, const void *dataPtr, int flags = 0, std::vector<TagID> * receiptList = NULL);
		
			// This function gets the next message in the radio list
			// of messages sent to this radio.
		virtual void GetNextMsg(char *bufferToFill, int & size, int * fromSocket = NULL, std::vector<TagID> * receiptList = NULL);

		void CollectGarbage(void);

	private:

			// This function operates as the network callback for the sockets.
		static int	NetCallback(TCPSocket *, struct _WSANETWORKEVENTS &, const char *, 
								int size, void * context);

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
		int					m_maxConnections;
		int					m_curConnections;
		NETCALLBACK			m_callback;
		void *				m_context;

			// this holds a list of the socket
		SocketMap			m_socketList;
		SocketMap::iterator	m_sit;

			// receive fifo.
		FifoServerMessage	m_recvFifo;

			// this holds the listen socket.
		TCPSocket *			m_listenSocket;

			// the UDP socket.
		UDPSocket *			m_serverUDPSocket;

			// this holds the mutex for locking out access to the socket list.
		Mutex				m_mutex;
		Mutex				m_receiveMutex;
		Mutex				m_closeMutex;
};

// =============================================
// This function returns the current number 
// of connections.
// =============================================

inline int Server::curConnections() const
{
	return m_curConnections;
}

// =============================================
// This function returns the max number of 
// connections for this machine.
// =============================================

inline int Server::maxConnections() const
{
	return m_maxConnections;
}

// =============================================
// This function returns the port.
// =============================================

inline int Server::port() const
{
	return m_port;
}

#endif
