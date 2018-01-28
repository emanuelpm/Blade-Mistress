// ============================================================================
// 
// TCPSocket.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local headers.
#include "TCPSocket.h"
#include "../helper/Debug.h"

	// System Headers.
#include <winsock2.h>

LPHOSTENT	lpHostEntLast;
char lastIPName[64] = "";

// =============================================
// This constructor attempts to create the 
// socket.
// =============================================

TCPSocket::TCPSocket(int socketHandle)
		:	m_socket(socketHandle), m_event(NULL), m_exitEvent(NULL),
			m_socketThread(NULL), m_socketThreadID(0), m_socketHandler(NULL),
			m_context(NULL), m_clientAddress(0)
{
}

// =============================================
// The destructor cleans up the open socket.
// =============================================

TCPSocket::~TCPSocket()
{
		// Shutdown the socket.
	shutdown();

}

// =============================================
// This function starts up the socket for use.
// This function creates the socket, the two 
// events for signalling and the thread to
// handle receiving data.
// =============================================

bool TCPSocket::startup(int recvSize, NETCALLBACK func, void * context)
{
		// save the old socket.
	int oldSocket	= m_socket;
	int	retval		= 0;

//	lastIPName[0] = 0;

		// set the receive buffer size.
	m_recvSize = recvSize;
	 
	if(m_socket == 0xffffffff)
	{
			// create the socket.
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			// did we fail.
		if(m_socket == INVALID_SOCKET)
		{
			return false;
		}
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

		// if we have a pre made socket, then set the select states.
	if(oldSocket != 0xffffffff)
	{
			// select the events this socket will respond to.
		retval = WSAEventSelect(m_socket, m_event, FD_READ | FD_WRITE | FD_CLOSE);

		if(retval == SOCKET_ERROR)
		{
			return false;
		}

			// a temp sock address.
		struct sockaddr_in	addr;
		int					addrSize = sizeof(addr);

			// get the other side address.
		retval = getpeername(m_socket, (SOCKADDR *)&addr, &addrSize);

		if(retval == SOCKET_ERROR)
		{
			return false;
		}
		
			// set the client address.
		m_clientAddress = addr.sin_addr.S_un.S_addr;

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

	return true;
}

// =============================================
// This function shuts down the socket.  A socket
// can be restarted from shutdown by calling 
// startup again.
// =============================================

bool TCPSocket::shutdown()
{
		// lock the mutex
	m_mutex.Lock();

		// Kill the thread.
	SetEvent(m_exitEvent);
	WaitForSingleObject(m_socketThread, 1000);

		// close down the event.
	WSACloseEvent((HANDLE)m_exitEvent);
	WSACloseEvent((HANDLE)m_event);

		// close down the socket.
	closesocket(m_socket);

		// reset the variables.
	m_socket = 0xffffffff;

	m_mutex.Unlock();

	return true;
}

// =============================================
// This function attempts to connect to a 
// location requested by the caller.
// =============================================

bool TCPSocket::connect(const std::string & name, int port)
{
		// Variables.
	LPHOSTENT	lpHostEnt;
	SOCKADDR_IN	saIn;
	int			retval;

	if (lastIPName[0] && !strcmp(lastIPName, name.c_str()))
	{
		lpHostEnt = lpHostEntLast;
	}
	else
	{
		// get the location of the host.
		lpHostEnt = gethostbyname(name.c_str());
	
		if(lpHostEnt == NULL)
		{
			return false;
		}

		sprintf(lastIPName, name.c_str());
		lpHostEntLast = lpHostEnt;
	}
		// select the events this socket will respond to.
	retval = WSAEventSelect(m_socket, m_event, FD_READ | FD_WRITE | FD_CONNECT | FD_CLOSE);

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

		// set the client address.
	m_clientAddress	= ((LPIN_ADDR)*lpHostEnt->h_addr_list)->S_un.S_addr;
		
		// build the address.
	saIn.sin_family				= AF_INET;
	saIn.sin_addr.S_un.S_addr	= m_clientAddress;
	saIn.sin_port				= htons(port);

		// attempt to connect to the socket.
	retval = ::connect(m_socket, (LPSOCKADDR)&saIn, sizeof(saIn));

	if(retval == SOCKET_ERROR)
	{
		retval = WSAGetLastError();

		if(retval != WSAEWOULDBLOCK)
		{
			return false;
		}
	}

	return true;
}

// =============================================
// This function is the threads' main execution
// function.
// =============================================

unsigned long __stdcall TCPSocket::ThreadMain(void * context)
{
		// set the pointer to the instance.
	TCPSocket *		lthis = (TCPSocket *)context;

		// Other Variables.
	WSANETWORKEVENTS	events;
	char *				buffer				= new char [lthis->m_recvSize];
	int					retval				= 0;
	int					bytesReceived		= 0;
	int					bytesParsed			= 0;
	int					remainder			= 0;
	HANDLE				levents[2];
	int					ret					= 0;

		// set the events int he event array.
	levents[0] = lthis->m_exitEvent;
	levents[1] = lthis->m_event;

		// go into our thread loop.
	while(1)
	{
			// wait for some event to fire.
		retval = WSAWaitForMultipleEvents(2, levents, FALSE, 10000/*INFINITE*/, FALSE);
		
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
			Chronos::InfoMessages::traceMessage("WSAEnumNetworkEvents: Socket Error: %d\n", WSAGetLastError());
			break;
		}

			// if we have a valid handler.
		if(lthis->m_socketHandler != NULL)
		{
				// if we have a receive case, get the data and pass it back to the caller.
			if((events.lNetworkEvents & FD_READ) && (!events.iErrorCode[FD_READ_BIT]))
			{
					// lock out the mutex.
				lthis->m_mutex.Lock();

					// receive the data.
				bytesReceived = recv(lthis->m_socket, buffer + remainder, lthis->m_recvSize - remainder, 0);

					// handle the events
				if(bytesReceived > 0)
				{
						// call the user handler.
					bytesParsed = lthis->m_socketHandler(
															lthis, events, buffer, 
															bytesReceived + remainder, lthis->m_context
														);

						// if the handler returns a -1 then we are suppose to exit.
					if(bytesParsed == -1)
					{
						Chronos::InfoMessages::traceMessage("m_socketHandler: Exiting Thread.\n");
						retval = 1;
						lthis->m_mutex.Unlock();
						break;
					}

						// calculate the remainder in the buffer.
					remainder = (bytesReceived + remainder) - bytesParsed;

						// if we have a remainder, then we need to shift the buffer down to the
						// beginning and initialize the empty space for clarity.
					if(remainder > 0)
					{
							// do a sanity check to make sure the remainder and the bytes parsed don't
							// blow the buffer out of the water.
						if(bytesParsed + remainder > lthis->m_recvSize)
						{
							Chronos::InfoMessages::traceMessage("Socket Error: Remainder and bytesParsed don't add up.  %d\n",
																bytesParsed + remainder);
							remainder = 0;					
						}

						else
						{
							memcpy(buffer, buffer + bytesParsed, remainder);
							//memset(buffer + remainder, 0xFF, lthis->m_recvSize - remainder);
						}
					}

					else
					{
						//if(bytesParsed <= lthis->m_recvSize)
						//{
						//	memset(buffer, 0xFF, lthis->m_recvSize) ;
						//}

						//else
						if(bytesParsed > lthis->m_recvSize)
						{
							Chronos::InfoMessages::traceMessage("Socket Error: bytesParsed is larger than buffer size.  %d",
																bytesParsed);
						}

						remainder = 0;
					}
				}
		
					// Unlock the mutex.
				lthis->m_mutex.Unlock();
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
// This function begins listening on the port 
// specified.
// =============================================

bool TCPSocket::listen(int port, int backlog)
{
		// Variables.
	SOCKADDR_IN		saIn;
	int				retval;

		// select the events this socket will respond to.
	retval = WSAEventSelect(m_socket, m_event, FD_ACCEPT | FD_READ | FD_WRITE | FD_CLOSE);

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

		// setup the address.
	saIn.sin_port			= htons(port);
	saIn.sin_family			= AF_INET;
	saIn.sin_addr.s_addr	= INADDR_ANY;

		// bind to the socket.
	retval = ::bind(m_socket, (LPSOCKADDR)&saIn, sizeof(saIn));

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

		// start listening on the socket.
	retval = ::listen(m_socket, backlog);

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

// =============================================
// This function sends a message on the socket.
// =============================================

bool TCPSocket::send(const char *buffer, int length)
{
		// Variables.
	int retval = 0;

		// send the data down the socket.
	retval = ::send(m_socket, buffer, length, 0);
	
	if(retval == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		return false;
	}

	return true;
}

// =============================================
// This function checks for a new connection
// request on the socket and returns the peer
// socket handle that refers to the new socket.
// =============================================

bool TCPSocket::accept(int & socket)
{
		// Variables.
	SOCKADDR_IN		saIn;
	int				len = sizeof(saIn);

		// accept the connection on the socket.
	socket = ::accept(m_socket, (LPSOCKADDR)&saIn, &len);

	if(socket == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}