//###################################################################################
//#																					#
//#						Socket Object Member Functions								#		
//#																					#
//#						Class to Handle Client/Server Communication					#
//#																					#
//#						Todd Barron, 08/06/2000										#
//#																					#
//###################################################################################

#include "SocketObject.h"

// Constructor
SocketObject::SocketObject()
{
	WSADATA wsaData;
	WORD	wVersionRequested;

	wVersionRequested = MAKEWORD( 2, 0 );

	skSocket = INVALID_SOCKET;
	iStatus = WSAStartup(wVersionRequested,&wsaData);

    dwReceiveHandle = 0;

}

// Destructor
SocketObject::~SocketObject()
{
    if (dwReceiveHandle)
    	CloseHandle(&dwReceiveHandle);
	Disconnect();
}

int SocketObject::vGetPacket(char *szbuffer)
{
	int				iBytesReceived = 0;
	int				iBytesWaiting = 0;
	stPacketHeader	stHeader;

	// Check if write pos moved
	if( stReceive.iWritePos != stReceive.iReadPos ) {
		//
		// Pull packet header from buffer
		//
		iBytesWaiting = (stReceive.iWritePos - stReceive.iReadPos);
		// Make sure a full size header is present
		if( iBytesWaiting < sizeof(stHeader) ) {
			return(0);
		}
		// Copy the header in
		memcpy(&stHeader,&stReceive.szBuffer[stReceive.iReadPos],sizeof(stHeader));
		// Check the checksum
		if( ((stHeader.iType+stHeader.iLength+stHeader.iID)) != stHeader.iCheckSum ) {
			// Skip the first bad byte in an attempt to find a good packet
			stReceive.iReadPos++;
			if( stReceive.iReadPos >= 64000 ) {
				stReceive.iReadPos = 0;
			}
			// Try again for a good packet
			vGetPacket(szbuffer);
		}
		else {
		}

		//
		// Pull the body of the packet according to the size 
		//
		
		// Make sure enough data is waiting, if not leave and try again later
		if( (iBytesWaiting-sizeof(stHeader)) < (unsigned)stHeader.iLength ) {
			return(0);
		}
		// Copy into the return buffer
		memcpy(szbuffer,&stHeader,sizeof(stHeader));
		memcpy(&szbuffer[sizeof(stHeader)],&stReceive.szBuffer[stReceive.iReadPos+sizeof(stHeader)],stHeader.iLength);

		// Update Read Position & Return Values
		stReceive.iReadPos += (stHeader.iLength+sizeof(stHeader));
		iBytesReceived = (stHeader.iLength+sizeof(stHeader));
		
		// Check if reading too far
		if( stReceive.iReadPos >= 64000 ) {
			stReceive.iReadPos = 0;
		}
	}

	return(iBytesReceived);
}

void SocketObject::thrReceiveThread(SocketTransmissionStruct *rs)
{
	int				iBytesReceived;
	char			*szTempBuffer;
	int				iBytesPart1;
	int				iBytesPart2;

	szTempBuffer = new char[32768];

	// Receive data until given notice to terminate
	while( rs->iTerminate != 1 ) {
		// Read from the pipe
		iBytesReceived = recv( rs->skSocket, szTempBuffer, 32768, 0 );
		if( iBytesReceived > 0 ) {
			// Make sure the packet does not overrun the write buffer
			if( (rs->iWritePos+iBytesReceived) >= 64000 ) {
				iBytesPart1 = ((rs->iWritePos+iBytesReceived)-64000);
				iBytesPart2 = (64000 - rs->iWritePos);
				memcpy( &rs->szBuffer[rs->iWritePos], szTempBuffer, iBytesPart1 );
				memcpy( &rs->szBuffer[0], &szTempBuffer[iBytesPart1], iBytesPart2 );
				rs->iWritePos = iBytesPart2;
			}
			else {
				// Write to the permanent buffer
				memcpy( &rs->szBuffer[rs->iWritePos], szTempBuffer, iBytesReceived );
				rs->iWritePos += iBytesReceived;
			}
		}
	}

	delete [] szTempBuffer;
}

void SocketObject::thrSendThread(SocketTransmissionStruct &rs)
{
	int iBytesSent;
	
	iBytesSent = send( rs.skSocket, rs.szBuffer, 128, 0 );
}

// Connect
bool SocketObject::Connect(char* szServerAddress, int iPort)
{
	struct		sockaddr_in serv_addr;
	LPHOSTENT	lphost;

	memset(&serv_addr,0,sizeof(sockaddr_in));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(szServerAddress);

	if (serv_addr.sin_addr.s_addr == INADDR_NONE)
	{
		lphost = gethostbyname(szServerAddress);
		if (lphost != NULL)
			serv_addr.sin_addr.s_addr = ((LPIN_ADDR)lphost->h_addr)->s_addr;
		else
		{
			WSASetLastError(WSAEINVAL);
			return FALSE;
		}
	}

	serv_addr.sin_port = htons(iPort);

	// Open the socket
	skSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(skSocket == INVALID_SOCKET)
	{
		return false;
	}

	int err = connect(skSocket, (struct sockaddr*)&serv_addr,sizeof(sockaddr));
	if(err == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}

	
	stReceive.skSocket = skSocket;
	stReceive.szBuffer[0] = NULL;
	stReceive.iReadPos = 0;
	stReceive.iWritePos = 0;
	stReceive.iTerminate = 0;

	// Create the thread to receive data
	CreateThread(
		NULL,  // pointer to security attributes
		NULL,                         // initial thread stack size
		(LPTHREAD_START_ROUTINE ) &thrReceiveThread,     // pointer to thread function
		&stReceive,                        // argument for new thread
		NULL,                     // creation flags
		&dwReceiveHandle          // pointer to receive thread ID
	);

	return true;
}

void SocketObject::Disconnect()
{
	if(skSocket != INVALID_SOCKET)
	{
		closesocket(skSocket);
		skSocket = INVALID_SOCKET;
	}
}

int SocketObject::Bind(int iPort)
{
	sockaddr_in saServerAddress;

	skSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	if(skSocket == INVALID_SOCKET)
	{
		return false;
	}

	memset(&saServerAddress, 0, sizeof(sockaddr_in));

	saServerAddress.sin_family = AF_INET;
	saServerAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	saServerAddress.sin_port = htons(iPort);

	if( bind(skSocket, (sockaddr*) &saServerAddress, sizeof(sockaddr)) == SOCKET_ERROR)
	{
		Disconnect();
		return false;
	}
	else
		return true;
}

int SocketObject::Listen( void )
{
	return listen( skSocket, 32 );
}

bool SocketObject::Accept( SocketObject &skAcceptSocket )
{
	sockaddr_in saClientAddress;
	int			iClientSize = sizeof(sockaddr_in);
	SOCKADDR	IPAddress;

	skAcceptSocket.skSocket = accept( skSocket, (struct sockaddr*)&saClientAddress, &iClientSize );
	
	if( skAcceptSocket.skSocket == INVALID_SOCKET ) 
	{
		return false;
	}
	else 
	{
		memcpy(&IPAddress,&saClientAddress,sizeof(saClientAddress));
		printf("%d.%d.%d.%d is Connecting\n",saClientAddress.sin_addr.S_un.S_un_b.s_b1,saClientAddress.sin_addr.S_un.S_un_b.s_b2,saClientAddress.sin_addr.S_un.S_un_b.s_b3,saClientAddress.sin_addr.S_un.S_un_b.s_b4);

		skAcceptSocket.stReceive.skSocket = skAcceptSocket.skSocket;
		skAcceptSocket.stReceive.szBuffer[0] = NULL;
		skAcceptSocket.stReceive.iReadPos = 0;
		skAcceptSocket.stReceive.iWritePos = 0;
		skAcceptSocket.stReceive.iTerminate = 0;
		
		// Create the thread to receive data
		CreateThread(
			NULL,															// pointer to security attributes
			NULL,															// initial thread stack size
			(LPTHREAD_START_ROUTINE ) &skAcceptSocket.thrReceiveThread,     // pointer to thread function
			&skAcceptSocket.stReceive,              // argument for new thread
			NULL,									// creation flags
			&skAcceptSocket.dwReceiveHandle         // pointer to receive thread ID
		);

		return true;
	}
}

int SocketObject::Recv( char *szBuffer, int iBufLen, int iFlags)
{
	return recv(skSocket, szBuffer, iBufLen, iFlags);
}

int SocketObject::Send(char *szBuffer, int iBufLen, int iFlags)
{
	return send(skSocket,szBuffer,iBufLen,iFlags);
}

int SocketObject::iCalculateChecksum(stPacketHeader stHead)
{
	int	iChecksum = 0;

	iChecksum = (stHead.iID+stHead.iLength+stHead.iType);

	return(iChecksum);
}



