// ============================================================================
// 
// UDPSocket.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local headers.
#include "UDPSocket.h"

	// System Headers.
#include <winsock2.h>

	// initialize the singleton pointer.
UDPSocket * UDPSocket::m_instance = NULL;

// =============================================
// This constructor attempts to create the 
// socket.
// =============================================

UDPSocket::UDPSocket(bool isSingleton)
		:	m_socket(0xffffffff), m_event(NULL), m_exitEvent(NULL),
			m_socketThread(NULL), m_socketThreadID(0), m_socketHandler(NULL),
			m_context(NULL), m_isSingleton(isSingleton)
{
	if(m_isSingleton == true)
	{
		m_instance = this;
	}
}

// =============================================
// The destructor cleans up the open socket.
// =============================================

UDPSocket::~UDPSocket()
{
		// Shutdown the socket.
	shutdown();

	if(m_isSingleton == true)
	{
		m_instance = NULL;
	}

}

// =============================================
// This function starts up the socket for use.
// This function creates the socket, the two 
// events for signalling and the thread to
// handle receiving data.
// =============================================

bool UDPSocket::startup(int port, int recvSize, UDPCALLBACK func, void * context, bool isServer, const char * serverName)
{
		// save the old socket.
	int				retval		= 0;
	SOCKADDR_IN		saIn;

		// set the server flag.
	m_isServer = isServer;

		// set the receive buffer size.
	m_recvSize = recvSize;
	 
	if(m_socket == 0xffffffff)
	{
			// create the socket.
		m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

			// did we fail.
		if(m_socket == INVALID_SOCKET)
		{
			return false;
		}
	}

	if(m_isServer == true)
	{
			// setup the address.
		saIn.sin_port			= htons(port);
		saIn.sin_family			= AF_INET;
		saIn.sin_addr.s_addr	= INADDR_ANY;

			// bind to the socket.
		retval = ::bind(m_socket, (LPSOCKADDR)&saIn, sizeof(saIn));

		if(retval == SOCKET_ERROR)
		{
			int lastError = WSAGetLastError();
			return false;
		}

			// create an event for this socket.
		m_event = WSACreateEvent();

			// did we fail.
		if(m_event == WSA_INVALID_EVENT)
		{
			return false;
		}

			// create an event for this socket.
		m_exitEvent = WSACreateEvent();

			// did we fail.
		if(m_event == WSA_INVALID_EVENT)
		{
			return false;
		}

			// select the events this socket will respond to.
		retval = WSAEventSelect(m_socket, m_event, FD_READ | FD_WRITE);

		if(retval == SOCKET_ERROR)
		{
			return false;
		}

			// create the thread for this socket.
		m_socketThread = CreateThread(NULL, 65536, ThreadMain, this, 0, &m_socketThreadID);

			// did we fail.
		if(m_socketThread == NULL)
		{
			return false;
		}

			// set the callback function.
		m_socketHandler = func;
		m_context		= context;
	}

	if(strlen(serverName) != 0)
	{
		HOSTENT *	lpHostEnt = gethostbyname(serverName);

		m_address = ((LPIN_ADDR)*lpHostEnt->h_addr_list)->S_un.S_addr;
	}

	return true;
}

// =============================================
// This function shuts down the socket.  A socket
// can be restarted from shutdown by calling 
// startup again.
// =============================================

bool UDPSocket::shutdown()
{
	if(m_isServer == true)
	{
			// Kill the thread.
		SetEvent(m_exitEvent);
		WaitForSingleObject(m_socketThread, 1000);

			// close down the event.
		WSACloseEvent((HANDLE)m_exitEvent);
		WSACloseEvent((HANDLE)m_event);
	}

		// close down the socket.
	closesocket(m_socket);

		// reset the variables.
	m_socket = 0xffffffff;

	return true;
}

// =============================================
// This function is the threads' main execution
// function.
// =============================================

unsigned long __stdcall UDPSocket::ThreadMain(void * context)
{
		// set the pointer to the instance.
	UDPSocket *		lthis = (UDPSocket *)context;

		// Other Variables.
	WSANETWORKEVENTS	events;
	char *				buffer				= new char [lthis->m_recvSize];
	int					retval				= 0;
	int					bytesReceived		= 0;
	int					bytesParsed			= 0;
	int					remainder			= 0;
	HANDLE				levents[2];
	int					ret					= 0;
	SOCKADDR_IN			addr;
	int					addrSize			= sizeof(addr);

		// set the events int he event array.
	levents[0] = lthis->m_exitEvent;
	levents[1] = lthis->m_event;

		// go into our thread loop.
	while(1)
	{
			// wait for some event to fire.
		retval = WSAWaitForMultipleEvents(2, levents, FALSE, INFINITE, FALSE);
		
			// see if we 
		if(retval == 0)
		{
			break;
		}

			// figure out what events just happened.
		retval = WSAEnumNetworkEvents(lthis->m_socket, lthis->m_event, &events);
		
			// kill the thread.
		if(retval == SOCKET_ERROR)
		{
			break;
		}

			// if we have a valid handler.
		if(lthis->m_socketHandler != NULL)
		{
				// if we have a receive case, get the data and pass it back to the caller.
			if(events.lNetworkEvents & FD_READ)
			{
					// receive the data.
				bytesReceived = recvfrom(lthis->m_socket, buffer + remainder, lthis->m_recvSize - remainder, 0, 
										(SOCKADDR*)&addr, &addrSize);

					// handle the events
				if(bytesReceived != 0)
				{
					bytesParsed = lthis->m_socketHandler(
															lthis, events, buffer, 
															bytesReceived + remainder, lthis->m_context
														);
					if(bytesParsed == -1)
					{
						retval = 1;
						break;
					}

					remainder = (bytesReceived + remainder) - bytesParsed;

					if(remainder > 0)
					{
						memcpy(buffer, buffer + bytesParsed, remainder);
					}
				}
			}

			else
			{
					// handle the events
				ret = lthis->m_socketHandler(lthis, events, NULL, 0, lthis->m_context);
			
				if(ret == -1)
				{
					retval =  1;
					break;
				}
			}
		}
	}

		// cleanup that buffer.
	delete[] buffer;

	return retval;
}

// =============================================
// This function sends a message on the socket.
// =============================================

bool UDPSocket::send(int address, int port, const char * buffer, int length)
{
		// Variables.
	int			retval = 0;
	SOCKADDR_IN sin;

		// setup the address.
	sin.sin_family				= AF_INET;
	sin.sin_addr.S_un.S_addr	= address;		// Server's address
	sin.sin_port				= htons(port);	// Port number

		// send the data down the socket.
	retval = ::sendto(m_socket, buffer, length, 0, (SOCKADDR *)&sin, sizeof(sin));
	
	if(retval == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}
