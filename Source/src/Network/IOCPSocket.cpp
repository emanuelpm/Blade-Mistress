// ============================================================================
// 
// IOCPSocket.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// System Headers.
#include <winsock2.h>
#include <mswsock.h>

	// Local headers.
#include "IOCPSocket.h"
#include "SOverlapped.h"

// =============================================
// Static declarations.
// =============================================

	// The array of threads for processing the main IOCP requests.
int *						IOCPSocket::m_sahMainThreadPool		= NULL;

	// The size of the thread pools.
int							IOCPSocket::m_siMainPoolSize		= 4;

	// The static IOCP objects for running the main socket callbacks and the accept.
Iocp *						IOCPSocket::m_spMainIocp			= NULL;

	// The number of sockets that exist.
long						IOCPSocket::m_siSocketCount			= 0;

	// The map of socket entries.
std::map<int, ISocket *>	IOCPSocket::m_socketMap;
CRITICAL_SECTION			IOCPSocket::m_socketLock;

	// Accept lock.
CRITICAL_SECTION			IOCPSocket::m_acceptLock;

	// The static fifo of overlapped buffers.
Fifo<SocketOverlapped *>	IOCPSocket::m_soFifo(30000);
Fifo<SocketOverlapped *>	IOCPSocket::m_soUsedFifo(30000);

	// The static fifo of connect socket events.
Fifo<int>					IOCPSocket::m_shConnectEvents(64);

	// this holds the sockets event for threading. This is all for event connect.
void *						IOCPSocket::m_shExitEvent		= NULL;
void *						IOCPSocket::m_shSocketThread	= NULL;

	// This holds a mapping of connecting sockets to their events.
std::map<int, IOCPSocket *>	IOCPSocket::m_sEventSocketList;
CRITICAL_SECTION			IOCPSocket::m_scsListLock;

// =============================================
// This constructor attempts to create the 
// socket.
// =============================================

IOCPSocket::IOCPSocket(int socketHandle)
		:	ISocket(socketHandle), m_pSocketHandler(NULL), m_iClientAddress(0), m_holdingBuffer(4096, 4096, false),
			m_iShuttingDown(0)
{
}

// =============================================
// The destructor cleans up the open socket.
// =============================================

IOCPSocket::~IOCPSocket()
{
		// Shutdown the socket.
	if(m_hSocket != NULL)
	{
		Shutdown();
	}
}

// =============================================
// This function starts up the socket for use.
// This function creates the socket, the two 
// events for signalling and the thread to
// handle receiving data.
// =============================================

bool IOCPSocket::Startup(int iRecvSize, ISocketHandler * pHandler)
{
		// Variables.
	int	retval		= 0;

		// set the receive buffer size.
	m_iRecvSize = iRecvSize;

		// If we have no socket.
	if(m_hSocket == NULL || ISOCKET_NULL_HANDLE == m_hSocket)
	{
			// create the socket.
		m_hSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

			// did we fail.
		if(m_hSocket == INVALID_SOCKET)
		{
			TraceMessage("Startup(): Socket Creation Failure.");
			m_hSocket = NULL;
			return false;
		}

			// Associate the socket with the completion port.
		if(m_spMainIocp->AssociateDevice((HANDLE)m_hSocket, 0) == false)
		{
			TraceMessage("Startup(): Associate Device failed.");
			return false;
		}

			// Add the socket to the map.
		EnterCriticalSection(&m_socketLock);
		m_socketMap.insert(std::map<int, ISocket *>::value_type(m_hSocket, this));
		LeaveCriticalSection(&m_socketLock);

	}

	else
	{
			// Associate the socket with the completion port.
		if(m_spMainIocp->AssociateDevice((HANDLE)m_hSocket, 0) == false)
		{
			TraceMessage("Startup(): Associate Device failed for pre-created socket.");
			return false;
		}

			// Add the socket to the map.
		EnterCriticalSection(&m_socketLock);
		m_socketMap.insert(std::map<int, ISocket *>::value_type(m_hSocket, this));
		LeaveCriticalSection(&m_socketLock);

			// setup a receive buffer.
		if(RegisterReceive() == false)
		{
			TraceMessage("Startup(): Register Receive failed.");
			EnterCriticalSection(&m_socketLock);
			m_socketMap.erase(m_hSocket);
			LeaveCriticalSection(&m_socketLock);
			return false;
		}
	}

		// set the callback function.
	m_pSocketHandler = pHandler;

	if (!m_pSocketHandler)
		m_pSocketHandler = (ISocketHandler *) 5;

	return true;
}


// =============================================
// This function shuts down the socket.  A socket
// can be restarted from shutdown by calling 
// startup again.
// =============================================

bool IOCPSocket::Shutdown()
{
		// Add the socket to the map.
	EnterCriticalSection(&m_socketLock);
	m_socketMap.erase(m_hSocket);
	LeaveCriticalSection(&m_socketLock);

		// Set the status as shutting down.
	InterlockedIncrement(&m_iShuttingDown);

		// close down the socket.
	closesocket(m_hSocket);
	m_hSocket = NULL;

	return true;
}

// =============================================
// This function attempts to connect to a 
// location requested by the caller.
// =============================================

bool IOCPSocket::Connect(const std::string & sAddress, int iPort)
{
		// Variables.
	LPHOSTENT	lpHostEnt;
	SOCKADDR_IN	saIn;
	int			retval;
	int			iTryCount	= 0;

		// get the location of the host.
	lpHostEnt = gethostbyname(sAddress.c_str());
	
	if(lpHostEnt == NULL)
	{
		TraceMessage("Connect(): gethostbyname() failed.");
		return false;
	}

		// get a free event.
	int	tEvent = 0;	

		// if there are no events then return false.
	while(m_shConnectEvents.Pop(tEvent) == false)
	{
		if(iTryCount > 10)
		{
			TraceMessage("Connect(): try count exceeding.");
			return false;	
		}

		iTryCount++;
		Sleep(50);
	}

		// select the events this socket will respond to.
	retval = WSAEventSelect(m_hSocket, (HANDLE)tEvent, FD_CONNECT);

	if(retval == SOCKET_ERROR)
	{
		int lastError = WSAGetLastError();
		TraceMessage("Connect(): WSAEventSelect() failed: error code: %d", lastError);
		return false;
	}

		// set the client address.
	m_iClientAddress	= ((LPIN_ADDR)*lpHostEnt->h_addr_list)->S_un.S_addr;
		
		// build the address.
	saIn.sin_family				= AF_INET;
	saIn.sin_addr.S_un.S_addr	= m_iClientAddress;
	saIn.sin_port				= htons(iPort);

		// lock out the event list.
	EnterCriticalSection(&m_scsListLock);

		// add the item to the list.
	m_sEventSocketList.insert(std::map<int, IOCPSocket *>::value_type(tEvent, this));

		// Unlock it.
	LeaveCriticalSection(&m_scsListLock);

		// attempt to connect to the socket.
	retval = ::connect(m_hSocket, (LPSOCKADDR)&saIn, sizeof(saIn));

	if(retval == SOCKET_ERROR)
	{
		retval = WSAGetLastError();

		if(retval != WSAEWOULDBLOCK)
		{
			TraceMessage("Connect(): connect() failed: error code: %d", retval);
			return false;
		}
	}

	return true;
}

// =============================================
// This function is the threads' main execution
// function.
// =============================================

unsigned long __stdcall IOCPSocket::ConnectThreadMain(void * context)
{
		// Other Variables.
	WSANETWORKEVENTS	events;
	int					retval				= 0;
	HANDLE				levents[64];
	
		// set the events int he event array.
	levents[0] = m_shExitEvent;

		// loop through and add all the events into the array.
	for(int i = 1; i < 64;++i)
	{
		m_shConnectEvents.Pop(retval);
		levents[i] = (HANDLE) retval;
		m_shConnectEvents.Push(retval);
	}

		// loop forever...
	while(1)
	{
			// wait for some event to fire.
		retval = WSAWaitForMultipleEvents(64, levents, FALSE, INFINITE, FALSE);
			
			// see if we 
		if(retval == 0)
		{
			return -1;
		}

			// set the handle.
		HANDLE tEvent = levents[retval];

			// Lock the list.
		EnterCriticalSection(&m_scsListLock);

			// Varaibles.
		std::map<int, IOCPSocket *>::iterator	mit = m_sEventSocketList.find((int)tEvent);

			// lookup the socket from the event handle.
		if(mit == m_sEventSocketList.end())
		{
			LeaveCriticalSection(&m_scsListLock);
			m_shConnectEvents.Push((int)tEvent);
			continue;
		}

			// set the socket pointer.
		IOCPSocket *	pSocket = (*mit).second;

			// remove the element.
		m_sEventSocketList.erase(mit);

			// unlock it.
		LeaveCriticalSection(&m_scsListLock);

			// figure out what events just happened.
		retval = WSAEnumNetworkEvents(pSocket->SocketHandle(), (HANDLE)tEvent, &events);
			
			// kill the thread.
		if(retval == SOCKET_ERROR)
		{
			return -2;
		}

			// handle sending failed messages.
		if(events.lNetworkEvents & FD_CONNECT)
		{
				// Variables.
			SocketOverlapped *	pOver = NULL;

				// post some user overlapped structs to push the threads.
			while(m_soFifo.Pop(pOver) == false)
			{
				GrowOverlappedFifo(1000);
			}

				// push it on.
			//m_soUsedFifo.Push(pOver);

				// set the data.
			pOver->Set(IOCP_CONNECT, pSocket, events.iErrorCode[FD_CONNECT_BIT]);

				// post it.
			m_spMainIocp->PostStatus(0, 0, pOver);
		}

			// release the event.
		m_shConnectEvents.Push((int)tEvent);
	}

	return retval;
}

// =============================================
// This function is the threads' main execution
// function.
// =============================================

unsigned long __stdcall IOCPSocket::MainThreadMain(void * context)
{
		// set the pointer to the instance.
	IOCPSocket *		lthis = (IOCPSocket *)context;

		// Other Variables.
	char *				pBuffer				= new char [4096];
	int					retval				= 0;
	int					bytesReceived		= 0;
	long				numSockets			= 0;
	SocketOverlapped *	pOverlapped			= NULL;
	OVERLAPPED *		ptOverlapped		= NULL;
	DWORD				dwTrans				= 0;
	DWORD				dwKey				= 0;	
	bool				success				= false;

		// get the current number of sockets.
	InterlockedExchange(&numSockets, m_siSocketCount);

		// loop until we have no more sockets.
	while(numSockets > 0)
	{
			// get the completion status.
		success = IOCPSocket::m_spMainIocp->WaitForStatus(&dwKey, &dwTrans, ptOverlapped);

			// set the overlapped pointer.
		pOverlapped = (SocketOverlapped *)ptOverlapped;

			// get the current number of sockets.
		InterlockedExchange(&numSockets, m_siSocketCount);

			// Lets make sure we are not already shutdown.
		if(numSockets < 1)
		{
			TraceMessage("IOCPThread(): no more sockets");

				// clean up the overlapped structure.
			//m_soUsedFifo.Pop(pOverlapped);
			m_soFifo.Push(pOverlapped);
			ptOverlapped = pOverlapped = NULL;
			break;
		}
		
			// if the overlapped pointer is valid then do the parsing.
		if(pOverlapped)
		{
				// Variables.
			EnterCriticalSection(&m_socketLock);
			if(IOCPSocket::m_socketMap.find(pOverlapped->m_hSocket) != IOCPSocket::m_socketMap.end())
			{
				LeaveCriticalSection(&m_socketLock);

					// Make sure we have a valid callback socket pointer.
				if(pOverlapped && pOverlapped->m_pSocket)
				{
						// if we succeeded then process the successful operations.
					if(success == true)
					{
						pOverlapped->m_pSocket->HandleGoodIoCompletions(pOverlapped, dwTrans, pBuffer, 4096);
					}

						// we had a failure.
					else
					{
						pOverlapped->m_pSocket->HandleErroredIoCompletions(pOverlapped, dwTrans);
					}
				}
			}

			else
			{
				LeaveCriticalSection(&m_socketLock);
			}

				// clean up the overlapped structure.
			//m_soUsedFifo.Pop(pOverlapped);
			pOverlapped->Reset();
			m_soFifo.Push(pOverlapped);
			ptOverlapped = pOverlapped = NULL;
		}

		else
		{
			TraceMessage("IOCPThreadMain(): overlapped buffer not valid.");
		}

	}

		// cleanup that buffer.
	delete[] pBuffer;
	pBuffer = NULL;

	return retval;
}

// =============================================
// This function handles processing the data on 
// a receive.
// =============================================

void IOCPSocket::ProcessRecv(const char * pData, int iSize)
{
		// if there is no callback then just return.
	if(m_pSocketHandler == NULL)
	{
		TraceMessage("ProcessRecv(): socket handler invalid.");
		return;
	}

		// add the data into the holding buffer.
	if(m_holdingBuffer.AddData(pData, iSize) == false)
	{
		TraceMessage("ProcessRecv(): AddData() failed to add.");
		return;
	}
		
		// get the pointer to the total data.
	char * lpData =	m_holdingBuffer.GetBuffer();

		// now call the callback.
	int iReclaimSize = m_pSocketHandler->OnRecv(m_hSocket, lpData, iSize, 0);

		// now we remove the data that has been processed.
	m_holdingBuffer.ReclaimBufferSpace(iReclaimSize);
}

// =============================================
// This function begins listening on the port 
// specified.
// =============================================

bool IOCPSocket::Listen(int iPort, int iBacklog)
{
		// Variables.
	SOCKADDR_IN		saIn;
	int				retval;

		// setup the address.
	saIn.sin_port			= htons(iPort);
	saIn.sin_family			= AF_INET;
	saIn.sin_addr.s_addr	= INADDR_ANY;

		// bind to the socket.
	retval = ::bind(m_hSocket, (LPSOCKADDR)&saIn, sizeof(saIn));

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

		// start listening on the socket.
	retval = ::listen(m_hSocket, iBacklog);

	if(retval == SOCKET_ERROR)
	{
		return false;
	}

		// based on the back log, loop through and add a number of sockets for accepting.
	for(int i = 0; i < iBacklog; ++i)
	{
			// try to create a new accept socket.
		if(SetupNewAccept() == false)
		{
			return false;
		}
	}

	return true;
}

// =============================================
// This function sets up a new accept socket.
// =============================================

bool IOCPSocket::SetupNewAccept()
{
		// declare the function pointer and the guid to get the interface.
	LPFN_ACCEPTEX	AcceptEx								= NULL;
	GUID			GuidAcceptEx							= WSAID_ACCEPTEX;
	DWORD			dwBytes									= 0;
	int				retval									= 0;
	static char		pBuffer[(sizeof(SOCKADDR_IN) + 16) * 2]	= "";
	
		// Get the function pointer for Accept Ex.
	WSAIoctl(m_hSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx), 
				&AcceptEx, sizeof(AcceptEx), &dwBytes, NULL, NULL);

		// Create a new socket.
	int hSocket	= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

		// did we fail.
	if(hSocket == INVALID_SOCKET)
	{
		TraceMessage("Failed to create socket in IOCP setup.");
		hSocket = NULL;
		return false;
	}

		// get a free overlapped buffer.
	SocketOverlapped *	pOver	= NULL;
		
		// while we are empty grow by 1000.
	while(m_soFifo.Pop(pOver) == false)
	{
		this->GrowOverlappedFifo(1000);
	}

		// setup pOver.
	pOver->Set(IOCP_ACCEPT, this, hSocket);

		// add it to the used.
	//m_soUsedFifo.Push(pOver);

		// Lock that puppy down.
	EnterCriticalSection(&m_acceptLock);

		// register the accept socket.
	retval = AcceptEx(m_hSocket, hSocket, pBuffer, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, pOver);

		// if we have an error, handle it.
	if(retval == FALSE)
	{
			// get the last winsock error.
		int iLastError	= WSAGetLastError();

			// if the last error was pending, then we are of.
		if(iLastError != ERROR_IO_PENDING)
		{
			TraceMessage("AcceptEx() Failed and IO ain't pending.");
	
			// Reset the overlapped object.
			pOver->Reset();

				// add the overlapped structure back into the free fifo
			m_soFifo.Push(pOver);

				// close the socket we tried to create.
			closesocket(hSocket);

				// Catch and release.
			LeaveCriticalSection(&m_acceptLock);

			return false;
		}
	}

		// Catch and release.
	LeaveCriticalSection(&m_acceptLock);

	return true;
}

// =============================================
// This function sends a message on the socket.
// =============================================

bool IOCPSocket::Send(const char *pData, int iSize)
{
		// Variables.
	int		retval		= 0;
	DWORD	dwBytesSend	= 0;
	WSABUF	wsaData;

		// fill out the WSA
	wsaData.buf = new char [iSize];
	wsaData.len	= iSize;
	memcpy(wsaData.buf, pData, sizeof(char) * iSize);

		// get a free overlapped buffer.
	SocketOverlapped *	pOver	= NULL;
		
		// while we are empty grow by 1000.
	while(m_soFifo.Pop(pOver) == false)
	{
		this->GrowOverlappedFifo(1000);
	}

		// setup pOver.
	pOver->Set(IOCP_SEND, this, -1, wsaData.buf, wsaData.len);

		// add the overlapped to the used.
	//m_soUsedFifo.Push(pOver);

		// send the data down the socket.
	retval = ::WSASend(m_hSocket, &wsaData, 1, &dwBytesSend, 0, pOver, NULL);
	
	if(retval == SOCKET_ERROR)
	{
		int error = WSAGetLastError();

			// if the last error was pending, then we are of.
		if(error != ERROR_IO_PENDING)
		{
//			TraceMessage("Send(): send failed. Recycling overlapped buffer error code: %d", error);
		
				// cleanup the buffer.
//			delete [] wsaData.buf;

				// Recycle Overlapped Structure.
			pOver->Reset();
			m_soFifo.Push(pOver);

			return false;
		}
	}

	return true;
}

// =============================================
// This function creates the pool of threads for 
// the main IOCP processing.
// =============================================

bool IOCPSocket::CreateMainThreadPool()
{
		// Variables.
	DWORD	threadID	= 0;
	bool	fail		= false;

		// create the array of exit handles and 
	m_sahMainThreadPool	= new int [m_siMainPoolSize];

		// clear the memory.
	memset(m_sahMainThreadPool, 0, sizeof(int) * m_siMainPoolSize);

		// loop through and create the events and the threads for the thread pool.
	for(int i = 0; i < m_siMainPoolSize; ++i)
	{
			// create the thread for this socket.
		m_sahMainThreadPool[i] = (int)CreateThread(NULL, 65536, MainThreadMain, NULL, 0, &threadID);

			// did we fail.
		if(m_sahMainThreadPool[i] == NULL)
		{
			fail = true;
			break;
		}
	}

		// if we failed then cleanup everything and return false.
	if(fail == true)
	{
		return false;
	}

	return true;
}

// =============================================
// This function registers a receive buffer to 
// complete later.
// =============================================

bool IOCPSocket::RegisterReceive()
{
		// Variables.
	DWORD	dwBytesReceived	= 0;
	DWORD	dwFlags			= 0;
	int		retval			= 0;
	WSABUF	wsaBuf;
	
		// setup the WSABUF for a 0 size;
	wsaBuf.buf	= NULL;
	wsaBuf.len	= 0;
			
		// get a free overlapped buffer.
	SocketOverlapped *	pOver	= NULL;
		
		// while we are empty grow by 1000.
	while(m_soFifo.Pop(pOver) == false)
	{
		this->GrowOverlappedFifo(1000);
	}

		// setup pOver.
	pOver->Set(IOCP_RECV, this);

		// add it to the used.
	//m_soUsedFifo.Push(pOver);

		// register a 0 length buffer.  We will be receiving the data at completion time.
	retval = WSARecv(m_hSocket, &wsaBuf, 1, &dwBytesReceived, &dwFlags, pOver, NULL);

		// if there was an error, return that to the caller.
	if(retval != 0)
	{
			// Get the last error.
		int iLastError = WSAGetLastError();

			// if the last error was pending, then we are of.
		if(iLastError != ERROR_IO_PENDING)
		{
			TraceMessage("RegisterRecv(): registration failed: error code: %d", iLastError);
		
				// Recycle the overlapped buffer.
			pOver->Reset();
			m_soFifo.Push(pOver);

			return false;
		}
	}

	return true;
}

// =============================================
// This function handles processing successful 
// io completions.
// =============================================

void IOCPSocket::HandleGoodIoCompletions(SocketOverlapped * pOverlapped, int length, char * pBuffer, int iBuffSize)
{
		// if the socket is shutting down, just return.
	if(m_iShuttingDown > 0)
	{
		return;
	}

		// switch on the overlapped type.
	switch(pOverlapped->m_iIOType)
	{
		case IOCP_SEND:
		{
			break;
		}

		case IOCP_RECV:
		{
				// Variables.
			int iBytesReceived	= 0;
			int iTempBR			= 0;

			do
			{
					// receive the data.
				iTempBR = recv(m_hSocket, pBuffer, iBuffSize, 0);

				
					// Check for socket error.
				if(iTempBR == SOCKET_ERROR)
				{
					int iLastError = WSAGetLastError();
					TraceMessage("HandleGoodIoCompletions(): recv failed: error code: %d", iLastError);
					break;
				}

					// if we receive nothing then break.
				if(iTempBR == 0)
				{
						// Call the callback and if it returns true, we accept the connection.
					if (m_pSocketHandler)  // added in case there's a valid reason for handler to be 
						                    // NULL TTR 8/11/03
						m_pSocketHandler->OnDisconnect(m_hSocket, 0);
					return;
				}

					// process the receive.
				ProcessRecv(pBuffer, iTempBR);

			}while(iTempBR == iBuffSize);
				
				// register a new receive buffer.
			RegisterReceive();

			break;
		}

		case IOCP_CONNECT:
		{
				// call the callback and if the return is true, we accept the connection.  
				// Otherwise we refuse it and we should close the socket.
			if(m_pSocketHandler->OnConnect(m_hSocket, pOverlapped->m_iAcceptSocket) == false)
			{
				Shutdown();
				break;
			}

				// register the receive buffer.
			RegisterReceive();
			
			break;
		}

		case IOCP_ACCEPT:
		{
				// Call the callback and if it returns true, we accept the connection.
			if(m_pSocketHandler->OnAccept(m_hSocket, pOverlapped->m_iAcceptSocket, 0) == false)
			{
				closesocket(pOverlapped->m_iAcceptSocket);
			}

				// setup a new socket for accepting.
			if(SetupNewAccept() == false)
			{
				TraceMessage("HandleGoodIoCompletions(): setup new accept failed");
				Shutdown();
				break;
			}

			break;
		}
	}
}

// =============================================
// This function handles processing errored io 
// completions.
// =============================================

void IOCPSocket::HandleErroredIoCompletions(SocketOverlapped * pOverlapped, int length)
{
		// if the socket is shutting down, just return.
	if(m_iShuttingDown > 0)
	{
		return;
	}

		// Variables.
	int	iError	= WSAGetLastError();

	
		// switch on the overlapped type.
	switch(pOverlapped->m_iIOType)
	{
		case IOCP_SEND:
		{
			m_pSocketHandler->OnWrite(m_hSocket, iError);
			TraceMessage("HandleErroredIoCompletions(): IOCP_SEND Failed");
			break;
		}

		case IOCP_RECV:
		{
			if(iError == 64)
			{
				m_pSocketHandler->OnDisconnect(m_hSocket, 0);
				return;
			}
			m_pSocketHandler->OnRecv(m_hSocket, NULL, 0, iError);
			TraceMessage("HandleErroredIoCompletions(): IOCP_RECV Failed");
			break;
		}

		case IOCP_CONNECT:
		{
			m_pSocketHandler->OnConnect(m_hSocket, iError);
			TraceMessage("HandleErroredIoCompletions(): IOCP_CONNECT Failed");
			break;
		}

		case IOCP_ACCEPT:
		{
				// Notify the user.
			m_pSocketHandler->OnAccept(m_hSocket, pOverlapped->m_iAcceptSocket, iError);
			
				// close the bad socket
			closesocket(pOverlapped->m_iAcceptSocket);

				// setup a new socket for accepting.
			if(SetupNewAccept() == false)
			{
				TraceMessage("HandleErroredIoCompletions(): setup new accept failed");
				Shutdown();
				break;
			}
			TraceMessage("HandleErroredIoCompletions(): IOCP_ACCEPT Failed. Replaced bad socket.");
			break;
		}
	}
}

// =============================================
// This function adds overlapped structures to 
// the fifo.
// =============================================

void IOCPSocket::GrowOverlappedFifo(int iAmount)
{
	TraceMessage("GrowOverlappedFifo(): Growing the overlapped entries by %d.", iAmount);

		// loop through and add on the structures.
	for(int i = 0; i < iAmount; ++i)
	{
 		m_soFifo.Push(new SocketOverlapped());
	}
}


// =============================================
// These functions are used to start and stop the IOCP data
// whether a socket is created or not.
// =============================================

bool IOCPSocket::StartIOCP()
{
		// increment the socket count.
	InterlockedIncrement(&m_siSocketCount);

	if(m_siSocketCount == 1)
	{
			// Create the two IOCP Objects.
		m_spMainIocp	= new Iocp();

			// Create the ports.
		m_spMainIocp->Create(m_siMainPoolSize);

			// Create the thread pools.
		if(CreateMainThreadPool() == false)
		{
			delete m_spMainIocp;
			InterlockedDecrement(&m_siSocketCount);
			return false;
		}

			// Grow the fifo.
		GrowOverlappedFifo(30000);

			// Initialize the critical section.
		InitializeCriticalSection(&m_socketLock);

			// Populate the fifo with 63 events used to handle 
			// connection notification.
		for(int i = 0; i < 63; ++i)
		{
				// create an event for this socket.
			int tEvent = (int)WSACreateEvent();

				// did we fail.
			if((HANDLE)tEvent == WSA_INVALID_EVENT)
			{
				return false;
			}

				// add it to the free events.
			m_shConnectEvents.Push(tEvent);
		}

			// create an event for this socket.
		m_shExitEvent = WSACreateEvent();

			// did we fail.
		if(m_shExitEvent == WSA_INVALID_EVENT)
		{
			return false;
		}

			// create the thread for this socket.
		unsigned long socketThreadID = 0;
		
		m_shSocketThread = CreateThread(NULL, 65536, ConnectThreadMain, NULL, 0, &socketThreadID);

			// did we fail.
		if(m_shSocketThread == NULL)
		{
			return false;
		}

			// create the critical section.
		InitializeCriticalSection(&m_scsListLock);
		InitializeCriticalSection(&m_acceptLock);

	}

	return true;
}

// =============================================
// This function stops the IOCP system.
// =============================================

bool IOCPSocket::StopIOCP()
{
		// decrement the socket count.
	InterlockedDecrement(&((LONG)m_siSocketCount));

	if(m_siSocketCount == 0)
	{
			// Variables.
		SocketOverlapped *	pOver = NULL;

			// post some user overlapped structs to push the threads.
		for(int i = 0; i < m_siMainPoolSize; ++i)
		{
			m_soFifo.Pop(pOver);
			//m_soUsedFifo.Push(pOver);
			m_spMainIocp->PostStatus(0, 0, pOver);

				// wait for the threads to close.
			WaitForMultipleObjects(m_siMainPoolSize, (HANDLE *)m_sahMainThreadPool, FALSE, INFINITE);
			pOver = NULL;
		}

			// kill IOCP.
		DeleteCriticalSection(&m_acceptLock);
		DeleteCriticalSection(&m_socketLock);

		delete m_spMainIocp;
		m_spMainIocp = NULL;

			// clean out the free list.
		while(m_soFifo.Pop(pOver) == true)
		{
			delete pOver;
			pOver = NULL;
		}

			// kill all outstanding pOverlapped.
		/*while(m_soUsedFifo.Pop(pOver) == true)
		{
			delete pOver;
			pOver = NULL;
		}*/

			// Kill the thread.
		SetEvent(m_shExitEvent);
		WaitForSingleObject(m_shSocketThread, INFINITE);

			// close down the event.
		if(m_shExitEvent != NULL)
		{
			CloseHandle((HANDLE)m_shExitEvent);
			m_shExitEvent = NULL;
		}

			// loop through and free all the events.
		int tEvent = 0;

		while(m_shConnectEvents.Pop(tEvent) == true)
		{
			CloseHandle((HANDLE)tEvent);
			tEvent = NULL;
		}

			// delete the critical section.
		DeleteCriticalSection(&m_scsListLock);
	}
		
	return true;
}

// =============================================
// This function prints a debug message to the 
// dev studio console
// =============================================

void IOCPSocket::TraceMessage(const char * fmt, ...)
{
		// Variables.
	char buffer[4096];
	char tformat[4096];

	sprintf(tformat, "%s\n", fmt);

	va_list ap;

		// parse out the string
	va_start(ap, fmt);
	vsprintf(buffer, tformat, ap);
	va_end(ap);

		// write the string to the debug window.
	OutputDebugString(buffer);
}
