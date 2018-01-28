// ============================================================================
// 
// IOCPSocket.h
// 
// By: Erik Dieckert.h
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_IOCP_SOCK_H
#define CHRONOS_IOCP_SOCK_H

	// Local Headers.
#include "DisableWarnings.h"
#include "ISocket.h"
#include "HoldingBuffer.h"
#include "Iocp.h"
#include "Fifo.h"

	// System Headers.
#include <map>

	// Forward declarations.
class SocketOverlapped;

// =============================================
// This class defines a IOCP transport socket.
// =============================================

class IOCPSocket : public ISocket
{
	public:

			// Creators.
		IOCPSocket(int iHandle = ISOCKET_NULL_HANDLE);
		~IOCPSocket();

			// These functions are used to start and stop the IOCP data
			// whether a socket is created or not.
		static bool	StartIOCP();
		static bool	StopIOCP();

	public:

			// These functions handle startup and shutdown 
			// of a socket.
		virtual bool	Startup(int iRecvSize, ISocketHandler * pHandler);
		virtual bool	Shutdown();

			// These functions start the socket in its 
			// either connect state or its listen state.
		virtual bool	Connect(const std::string & sAddress, int iPort);
		virtual bool	Listen(int iPort, int iBacklog = 5);

			// These functions send and receive data.
		virtual bool	Send(const char * pData, int iSize);

			// the socket descriptor.
		int					m_iClientAddress;

	private:

			// This function prints a debug message to the dev studio console
		static void TraceMessage(const char * fmt, ...);

	private:

			// this holds the sockets event for threading.
		int					m_iRecvSize;

			// This holds the callback handler.
		ISocketHandler *	m_pSocketHandler;

			// The holding buffer.
		HoldingBuffer		m_holdingBuffer;

			// The flag for shutting down.
		long				m_iShuttingDown;
	
	private:
			
			// The array of threads for processing the main IOCP requests.
		static int *						m_sahMainThreadPool;

			// The size of the thread pools.
		static int							m_siMainPoolSize;

			// The static IOCP objects for running the main socket callbacks and the accept.
		static Iocp *						m_spMainIocp;

			// The number of sockets that exist.
		static long							m_siSocketCount;

			// The map of socket entries.
		static std::map<int, ISocket *>		m_socketMap;
		static CRITICAL_SECTION				m_socketLock;
			
			// Accept lock.
		static CRITICAL_SECTION				m_acceptLock;

			// The static fifo of overlapped structures.
		static Fifo<SocketOverlapped *>		m_soFifo;
		static Fifo<SocketOverlapped *>		m_soUsedFifo;

			// The static fifo of connect socket events.
		static Fifo<int>					m_shConnectEvents;

			// this holds the sockets event for threading. This is all for event connect.
		static void *						m_shExitEvent;
		static void *						m_shSocketThread;

			// This holds a mapping of connecting sockets to their events.
		static std::map<int, IOCPSocket *>	m_sEventSocketList;
		static CRITICAL_SECTION				m_scsListLock;

	private:


			// This function registers a receive buffer to complete later.
		bool	RegisterReceive();

			// This function handles processing successful io completions.
		void	HandleGoodIoCompletions(SocketOverlapped * pOverlapped, int length, char * pBuffer = NULL, int iBuffSize = 0);

			// This function handles processing errored io completions.
		void	HandleErroredIoCompletions(SocketOverlapped * pOverlapped, int length);

	private:

			// This is the main function for the socket thread.
		static unsigned long __stdcall MainThreadMain(void * context);
		static unsigned long __stdcall ConnectThreadMain(void * context);

			// This function handles processing the data on a receive.
		void	ProcessRecv(const char * pData, int iSize);

			// This function sets up a new accept socket.
		bool	SetupNewAccept();

			// This function adds overlapped structures to 
			// the fifo.
		static void	GrowOverlappedFifo(int iAmount);
	
			// This function creates the pool of threads for the main IOCP processing.
		static bool	CreateMainThreadPool();

};

	// the message start tag.
#define MESSAGE_START_TAG	0xDEADBEEF

#endif
