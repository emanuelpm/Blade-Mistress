// ============================================================================
// 
// ISocket.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef ISOCKET_H
#define ISOCKET_H

	// Local headers.
#include "ISocketHandler.h"

	// System Headers.
#include <string>

	// Standard Socket Constants.
const int ISOCKET_NULL_HANDLE	= 0xffffffff;

// =============================================
// ISocket
// 
// This class defines the interface for a 
// socket.  All socket interfaces are built upon
// this interface.
// =============================================

class ISocket
{
	public:

			// Creators.
		ISocket(int iHandle = NULL);
		virtual ~ISocket();

			// This function gets the socket handle.
		int		SocketHandle() const;

	public:

			// These functions handle startup and shutdown 
			// of a socket.
		virtual bool	Startup(int iRecvSize, ISocketHandler * pHandler)	= 0;
		virtual bool	Shutdown()	= 0;

			// These functions start the socket in its 
			// either connect state or its listen state.
		virtual bool	Connect(const std::string & sAddress, int iPort)	= 0;
		virtual bool	Listen(int iPort, int iBacklog = 5)	= 0;

			// These functions send and receive data.
		virtual bool	Send(const char * pData, int iSize)	= 0;

	protected:

			// The socket handle.
		int			m_hSocket;
};

// =============================================
// Inline Functions.
// =============================================

	// Creators.
inline ISocket::ISocket(int iHandle)
				:	m_hSocket(iHandle)
{
}

inline ISocket::~ISocket()
{
}

	// This function gets the socket handle.
inline int ISocket::SocketHandle() const
{
	return m_hSocket;
}

#endif
