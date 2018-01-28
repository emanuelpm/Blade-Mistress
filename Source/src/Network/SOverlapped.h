// ============================================================================
// 
// SOverlapped.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_SOVERLAPPED_H
#define CHRONOS_SOVERLAPPED_H

	// System Headers.
#include <windows.h>

	// The constants for the IO type.
const int IOCP_SEND		= 1;
const int IOCP_RECV		= 2;
const int IOCP_ACCEPT	= 3;
const int IOCP_CONNECT	= 4;

// =============================================
// SOverlapped
//
// This class adds some extra members to the 
// overlapped structure for overlapped sockets.
// =============================================

class SocketOverlapped: public OVERLAPPED
{
	public:

			// Creators.
		SocketOverlapped(int iIOType = 0, IOCPSocket * pSocket = NULL, int iAcceptSocket = -1, char * pData = NULL, int iSize = 0);
		~SocketOverlapped();

			// This function sets the data in the overlapped structure.
		void Set(int iIOType = 0, IOCPSocket * pSocket = NULL, int iAcceptSocket = -1, char * pData = NULL, int iSize = 0);

			// This function sets the data in the overlapped structure.
		void Reset();

	public:

		IOCPSocket *	m_pSocket;
		int				m_iIOType;
		int				m_iAcceptSocket;
		char *			m_pData;
		int				m_iSize;
		int				m_hSocket;
};

// =============================================
// Inline functions.
// =============================================

inline SocketOverlapped::SocketOverlapped(int iIOType, IOCPSocket * pSocket, int iAcceptSocket, char * pData, int iSize)
	:	m_iIOType(iIOType), m_pSocket(pSocket), m_iAcceptSocket(iAcceptSocket), m_pData(pData), m_iSize(iSize), m_hSocket(0)
{
	Internal		= 0;
	InternalHigh	= 0;
	Offset			= 0;
	OffsetHigh		= 0;
	hEvent			= 0;

	if(pSocket != NULL)
	{
		m_hSocket = pSocket->SocketHandle();
	}
}

inline SocketOverlapped::~SocketOverlapped()
{
	delete[] m_pData;
	m_pData = NULL;
	m_iSize = 0;
}

	// This function sets the data in the overlapped structure.
inline void SocketOverlapped::Set(int iIOType, IOCPSocket * pSocket, int iAcceptSocket, char * pData, int iSize)
{
		// clean up any old buffer data.
	delete[] m_pData;
	m_pData = NULL;

	m_iIOType		= iIOType;
	m_pSocket		= pSocket;
	m_iAcceptSocket	= iAcceptSocket;
	m_pData			= pData;
	m_iSize			= iSize;
	m_hSocket		= 0;

	if(pSocket != NULL)
	{
		m_hSocket = pSocket->SocketHandle();
	}
}

	// This function sets the data in the overlapped structure.
inline void SocketOverlapped::Reset()
{
		// clean up any old buffer data.
	delete[] m_pData;
	m_pData = NULL;

	m_iIOType		= 0;
	m_pSocket		= 0;
	m_iAcceptSocket	= 0;
	m_pData			= 0;
	m_iSize			= 0;
	m_hSocket		= 0;
}

#endif
