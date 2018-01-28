// ============================================================================
// 
// TCPSocket.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_TCP_SOCK_H
#define CHRONOS_TCP_SOCK_H

	// Needed Headers.
#include <string>
#include "../helper/Mutex.h"

	// forward declarations.
struct _WSANETWORKEVENTS;

	// namespace forward declarations.
class TCPSocket;

	// the callback helper.
typedef int (*NETCALLBACK)(TCPSocket * sock, struct _WSANETWORKEVENTS & events, const char *, 
						int size, void * context);

	// the message start tag.
#define MESSAGE_START_TAG	0xDEADBEEF

// =============================================
// This class defines a TCP transport socket.
// =============================================

class TCPSocket
{
	public:

			// Creators.
		TCPSocket(int handle = 0xffffffff);
		~TCPSocket();

			// These functions handle startup and shutdown 
			// of a socket.
		bool	startup(int recvSize, NETCALLBACK func, void * context);
		bool	shutdown();

			// These functions start the socket in its 
			// either connect state or its listen state.
		bool	connect(const std::string & address, int port);
		bool	listen(int port, int backlog = 5);
		bool	accept(int & socketHandle);

			// These functions send and receive data.
		bool	send(const char * data, int size);

			// accessors.
		int		socketHandle() const;
		int		clientAddress() const;
				
	private:

			// the socket descriptor.
		int					m_socket;
		int					m_clientAddress;

			// this holds the sockets event for threading.
		void *				m_event;
		void *				m_exitEvent;
		void *				m_socketThread;
		unsigned long		m_socketThreadID;
		int					m_recvSize;

			// This holds the callback handler.
		NETCALLBACK			m_socketHandler;
		void *				m_context;
		Mutex				m_mutex;

	private:

			// this is the main function for the socket thread.
		static unsigned long __stdcall ThreadMain(void * context);

};

// =============================================
// This function gets the socket handle.
// =============================================

inline int TCPSocket::socketHandle() const
{
	return m_socket;
}

inline int TCPSocket::clientAddress() const
{
	return m_clientAddress;
}

#endif
