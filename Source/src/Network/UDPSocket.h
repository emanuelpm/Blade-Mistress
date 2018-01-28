// ============================================================================
// 
// UDPSocket.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef UDP_SOCKET_H
#define UDP_SOCKET_H

	// Needed Headers.
#include <string>

	// forward declarations.
struct _WSANETWORKEVENTS;

	// namespace forward declarations.
class UDPSocket;

	// the callback helper.
typedef int (*UDPCALLBACK)(UDPSocket * sock, struct _WSANETWORKEVENTS & events, const char *, 
						int size, void * context);

// =============================================
// This class manages the interface for 
// communicating through a UDP connectionless
// socket.
// =============================================

class UDPSocket
{
	public:

			// Creators.
		UDPSocket(bool isSingleton = false);
		~UDPSocket();

			// These functions handle startup and shutdown 
			// of a socket.
		bool	startup(int port, int recvSize, UDPCALLBACK func, void * context, bool isServer, const char * serverName = "");
		bool	shutdown();
			
			// This function sends a message to address specified.
		bool	send(int address, int port, const char * data, int size);

			// accessors.
		int		socketHandle() const;
		int		socketAddress() const;

	public:

			// these functions manage the singleton object.
		static UDPSocket *			instance();

	private:

			// the singleton pointer.
		static UDPSocket *	m_instance;

			// the socket descriptor.
		int					m_socket;
		int					m_address;

			// this holds the sockets event for threading.
		void *				m_event;
		void *				m_exitEvent;
		void *				m_socketThread;
		unsigned long		m_socketThreadID;
		int					m_recvSize;

			// This holds the callback handler.
		UDPCALLBACK			m_socketHandler;
		void *				m_context;

			// is this socket active as a server.
		bool				m_isServer;
		bool				m_isSingleton;

	private:

			// this is the main function for the socket thread.
		static unsigned long __stdcall ThreadMain(void * context);

};

// =============================================
// This function gets the socket handle.
// =============================================

inline int UDPSocket::socketHandle() const
{
	return m_socket;
}

inline int UDPSocket::socketAddress() const
{
	return m_address;
}

// =============================================
// This function gets the singleton instance
// if it exists.
// =============================================

inline UDPSocket * UDPSocket::instance()
{
	return m_instance;
}

#endif
