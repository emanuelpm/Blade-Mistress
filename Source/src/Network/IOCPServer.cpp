// ============================================================================
// 
// IOCPServer.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// local headers.
	// System Headers.
//#include <afxtempl.h>
//#include <afxmt.h>

#include "IOCPServer.h"
#include "../helper/Debug.h"

	// System Headers.
//#include <winsock2.h>

#include "NetWorldMessages.h"

//extern void SetCheckSum(char *data, int size);

using namespace Chronos;


// =============================================
// Default constructor and destructor.
// =============================================

IOCPServer::IOCPServer()
	:	NetWorldRadio("SERV", true), m_port(0), m_maxConnections(0), m_curConnections(0),
		m_listenSocket(NULL), m_context(NULL), m_recvFifo(1000), m_deleteFifo(1000)
{

	lastTimeUpdate = lastMessUpdate = bytesIn = bytesOut = 0;

}

IOCPServer::~IOCPServer()
{
	stopServer();
}

// =============================================
// This function starts the server on a given 
// port.
// =============================================

bool IOCPServer::startServer(int serverPort, int maxConnections, void * context)
{
		// set the parameters.
	m_port				= serverPort;
//	m_callback			= callback;
	m_context			= context;
	m_maxConnections	= maxConnections;

		// Variables.
	IOCPSocket *	tSocket	= NULL;

		// create an IOCP base socket.
	{
		tSocket = new IOCPSocket();

		// startup the socket.
		if(tSocket->Startup(4096, this) == false)
		{
			delete tSocket;
			tSocket = NULL;
			LogMessage("Startup of socket failed.");
			return FALSE;
		}

			// start listening.
		if(tSocket->Listen(m_port, 5) == false)
		{
			delete tSocket;
			tSocket = NULL;
			LogMessage("Listening Failed.");
			return FALSE;
		}

			// set the listen socket.
		m_listenSocket = tSocket;

		LogMessage("Listening Started.");
	}

/*
		// create the listen socket.
	m_listenSocket = new IOCPSocket();

	// startup the socket.
	if(m_listenSocket->startup(10, NetCallback, this) == false)
	{
		stopServer();
		return false;
	}

		// start listening on the port.
	if(m_listenSocket->listen(m_port, 10) == false)
	{
		stopServer();
		return false;
	}
*/

	return true;
}

// =============================================
// This function stops the current server that 
// is running.
// =============================================

bool IOCPServer::stopServer()
{
		// cleanup the socket.
	delete m_listenSocket;
	m_listenSocket = NULL;

		// lock
	m_mutex.Lock();

		// go through all the connected sockets and clean them up too.
	IOCPSocketMap::iterator	sit;

	for(sit = m_socketList.begin(); sit != m_socketList.end(); )
	{
		(*sit).second->Shutdown();
//			pSocket->Shutdown();
 		m_deleteFifo.Push((*sit).second);
		m_socketList.erase(sit);
		sit = m_socketList.begin();
//		delete((*sit).second);

	}

		// clear the whole list.
	m_socketList.clear();

		// set the connection count to 0.
	m_curConnections = 0;

		// unlock.
	m_mutex.Unlock();

	IOCPSocket *	pSocket;

	while(m_deleteFifo.Pop(pSocket) == true)
	{
		delete pSocket;
		pSocket = NULL;
	}

		// go through all the messages still pending and clear them.
	IOCPServerMessage * message = NULL;

	m_mutex.Lock();

	while(m_recvFifo.Pop(message) == true)
	{
		delete message->m_message;
		delete message;
	}

	m_mutex.Unlock();

	return true;
}


// =============================================
// This function closes a client socket.
// =============================================
/*
void IOCPServer::closeClientSocket(IOCPSocket * socket)
{
		// lock
	m_mutex.Lock();

		// find the socket block.
	IOCPSocketMap::iterator sbit = m_socketList.find(socket->socketHandle());

	if(sbit != m_socketList.end())
	{
			// delete the socket.
		delete socket;

		while(!(*sbit).second->m_backupSendFifo.IsEmpty())
		{
			Chronos::BStream * stream = NULL;
			(*sbit).second->m_backupSendFifo.Pop(stream);
			delete stream;
		}

		delete(*sbit).second;

			// add it to the socket list.
		m_socketList.erase(sbit);
	}

		// inc the number of connections.
	m_curConnections--;

		// Unlock
	m_mutex.Unlock();

}
*/
// =============================================
// This function is the old way of doing things.
// =============================================
/*
int IOCPServer::NetCallback(IOCPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
						int size, void * context)
{
		// varaibles.
	IOCPSocket *	pSocket		= socket;
	IOCPServer *	lthis		= (IOCPServer *)context;
	int			newHandle	= 0;
	int			bytesParsed = 0;

		// handle accepting new connections.
	if(events.lNetworkEvents & FD_ACCEPT)
	{
		if(m_curConnections < m_maxConnections)
		{
			if(socket->accept(newHandle) == true)
			{
					// create the socket.
				SocketBlock *	sb		= new SocketBlock;
				sb->m_socket			= new IOCPSocket(newHandle);
					// set it up.
				sb->m_socket->startup(65536 * 2, NetCallback, context);
			
				int address = sb->m_socket->clientAddress();
				char *cAddr = (char *) &address;

					// lock
				m_mutex.Lock();

					// add it to the socket list.
				m_socketList.insert(IOCPSocketMap::value_type(newHandle, sb));

					// inc the number of connections.
				m_curConnections++;

					// Unlock
				m_mutex.Unlock();

					// set socket pointer to the new socket.
				pSocket = sb->m_socket;

				// tell my owner about this connection.
				if(m_recvFifo.IsFull() == false)
				{
					MessAccept messageAccept;
					messageAccept.IP[0] = cAddr[0];
					messageAccept.IP[1] = cAddr[1];
					messageAccept.IP[2] = cAddr[2];
					messageAccept.IP[3] = cAddr[3];
//					SetCheckSum((char *) &messageAccept, sizeof(messageAccept));

					IOCPServerMessage *	serverMessage	= new IOCPServerMessage();
					serverMessage->m_message			= new BStream(sizeof(messageAccept) + 20);
					serverMessage->m_socketHandle		= pSocket->socketHandle();

						// add new message tag.
					*(serverMessage->m_message) << (sint)MESSAGE_START_TAG;
						
						// set the total size of the message.
					*(serverMessage->m_message) << (sint)(sizeof(messageAccept) + 20);

						// write out that we are sending this message to a server.
					*(serverMessage->m_message) << (sint)(SOCKET_TARGET_SERVER);

						// stream in the target count.
					*(serverMessage->m_message) << (sint)0;

						// write out the size of the message.
					*(serverMessage->m_message) << (sint)sizeof(messageAccept);

					// copy the data into the stream
					serverMessage->m_message->write(&messageAccept, sizeof(messageAccept));

					m_receiveMutex.Lock();
					m_recvFifo.Push(serverMessage);
					m_receiveMutex.Unlock();
				}

			}
		}
	}

		// handle sending failed messages.
	if(events.lNetworkEvents & FD_WRITE)
	{
		m_mutex.Lock();

			// find the socket in the map.
		IOCPSocketMap::iterator	sit = m_socketList.find(socket->socketHandle());

		if(sit != m_socketList.end())
		{
				// Variables.
			Chronos::BStream *	stream = NULL;

				// Is there anything to send?
			while((*sit).second->m_backupSendFifo.GetTail(stream) == true)
			{
					// attempt to send the message out the socket.
				if(!socket->Send(stream->buffer(), stream->used()))
				{
					break;
				}

					// remove the message.
				if((*sit).second->m_backupSendFifo.Pop(stream) == true)
				{
						// cleanup the memory.
					delete stream;
					stream = NULL;
				}
			}
		}

		m_mutex.Unlock();
	}

		// handle recevie messages.
	if(events.lNetworkEvents & FD_READ)
	{
		bytesParsed = handleReceive(buffer, size, socket->socketHandle());
	}

		// if we have a valid handler, call that too.
//	if(m_callback != NULL)
//	{
//		bytesParsed = m_callback(pSocket, events, buffer, size, m_context);
//	}

		// after we have handled the user callback, we can handle close events.
	if(events.lNetworkEvents & FD_CLOSE)
	{
		if(pSocket != m_listenSocket)
		{
				// lock
			m_mutex.Lock();

				// find the socket in the map.
			IOCPSocketMap::iterator	sit = m_socketList.find(socket->socketHandle());

			if(sit != m_socketList.end())
			{
					// Is there anything to send?
				while(!(*sit).second->m_backupSendFifo.IsEmpty())
				{
						// Variables.
					Chronos::BStream *	stream = NULL;

						// remove the message.
					if((*sit).second->m_backupSendFifo.Pop(stream) == true)
					{
							// cleanup the memory.
						delete stream;
					}
				}

					// set the temp socket handle
				int lsocketHandle = pSocket->socketHandle();

					// delete the socket.
				delete (*sit).second;
					
					// delete the entry.
				delete (*sit).second;

					// remove the socket.
				m_socketList.erase(sit);

					// inc the number of connections.
				m_curConnections--;

					// tell my owner about this connection.
				if(m_recvFifo.IsFull() == false)
				{
					MessClose messageClose;
//					messageClose.checkSum = messageClose.idTag;

					IOCPServerMessage *	serverMessage		= new IOCPServerMessage();
					serverMessage->m_message			= new BStream(sizeof(messageClose) + 20);
					serverMessage->m_socketHandle		= lsocketHandle;

						// add new message tag.
					*(serverMessage->m_message) << (sint)MESSAGE_START_TAG;
						
						// set the total size of the message.
					*(serverMessage->m_message) << (sint)(sizeof(messageClose) + 20);

						// write out that we are sending this message to a server.
					*(serverMessage->m_message) << (sint)(SOCKET_TARGET_SERVER);

						// stream in the target count.
					*(serverMessage->m_message) << (sint)0;

						// write out the size of the message.
					*(serverMessage->m_message) << (sint)sizeof(messageClose);

					// copy the data into the stream
					serverMessage->m_message->write(&messageClose, sizeof(messageClose));

					m_receiveMutex.Lock();
					m_recvFifo.Push(serverMessage);
					m_receiveMutex.Unlock();
				}
			}

				// exit the thread, we are done.
			bytesParsed = -1;			

				// Unlock
			m_mutex.Unlock();
		}
	}

	return bytesParsed;
}
*/


// =============================================
// This function process data received on the 
// socket and returns the number of bytes 
// processed.
// =============================================

int IOCPServer::handleReceive(const char * buffer, int size, int socketHandle)
{
		// Check to make sure we did not get a negative size.
	if(size < 0)
	{
		Chronos::InfoMessages::traceMessage("IOCPServer Error: Size from socket was negative.  %d\n", size);
		return 0;
	}

		// Varaibles.
	char *			tbuffer			= const_cast<char *>(buffer);
	unsigned long	bytesParsed		= 0;
	unsigned long	lsize			= (unsigned long)size;
	ulong			mSize			= 0;
	ulong			mTarget			= 0;
	bool			done			= false;
	bool			partialPacket	= false;
	
		// check the precursor.
	do
	{
			// do data verification before we stick it in the fifo.
		while(1)
		{
				// inc the bytes parsed.
			bytesParsed += mSize;

				// inc the buffer.
			tbuffer		= const_cast<char *>(buffer + bytesParsed);

				// check to make sure we have enough valid data to get 
				// the eyecatcher, the message size and the client server flag.
			if(lsize - bytesParsed < 16)
			{
				done = true;
				break;
			}

				// loop through the buffer until we find the first eye catcher.
			while((*((ulong *)tbuffer) != MESSAGE_START_TAG) && (tbuffer - buffer < lsize))
			{
				tbuffer++;
			}

			if(tbuffer > buffer + bytesParsed)
			{
				bytesParsed += tbuffer - (buffer + bytesParsed);
			}

				// check and see if we have blown passed the buffer.
			if(tbuffer - buffer >= lsize)
			{
				done = true;
				break;
			}

				// Check the size again just to make sure.
			if(lsize - bytesParsed < 16)
			{
				done = true;
				break;
			}

				// get the size of the message.
			mSize = *(((ulong *)tbuffer) + 1);

				// Find the next buffer end.
			char * walker1	= (tbuffer + 4);
			partialPacket	= false;

				// loop through the buffer until we find the first eye catcher.
			while(*((ulong *)walker1) != MESSAGE_START_TAG)
			{
				if(walker1 - buffer >= lsize)
				{
					partialPacket = true;
					break;
				}

				walker1++;
			}

				// check the size of the buffer against the stated size.
			if(mSize == (walker1 - tbuffer))
			{
				done = false;
				break;
			}

			else if(partialPacket == true)
			{
				done = true;
				break;
			}

			else
			{
				Chronos::InfoMessages::traceMessage("IOCPServer Error: Size Mismatch! packet: %d  calculated: %d\n", 
													mSize, (walker1 - tbuffer));
				mSize = (walker1 - tbuffer);
			}
		}
			
			// check to see if we are done processing.
		if(done == true)
		{
			break;
		}

			// grab the size and the target field.
		mTarget	= *(((ulong *)tbuffer) + 2);
		
			// if we have a valid client message.
		if(mTarget == SOCKET_TARGET_SERVER)
		{
				// check to see if we have a complete message.
			if(mSize <= lsize - bytesParsed)
			{
					// if the receive fifo is not full.
				if(m_recvFifo.IsFull() == false)
				{
					IOCPServerMessage *		serverMessage	= new IOCPServerMessage();
					serverMessage->m_message			= new BStream(mSize);
					serverMessage->m_socketHandle		= socketHandle;
					serverMessage->m_message->write(tbuffer, mSize);
					m_receiveMutex.Lock();
					m_recvFifo.Push(serverMessage);
					m_receiveMutex.Unlock();
				}

					// else we will just drop the message and log it.
				else
				{
					Chronos::InfoMessages::traceMessage("IOCPServer Error: receiveFifo FULL! Dropping Message.  Message Size: %d\n", 
														mSize);
				}
			}
			
			else
			{
					// time to stop parsing and wait for the end of the message.
				break;
			}
		}
	}while(1);

		// check to make sure that the bytes parsed are not greater than the size passed in.
	if(bytesParsed > lsize)
	{
		Chronos::InfoMessages::traceMessage("IOCPServer Error: bytesParsed is greater than the size! bp:%d size:%d\n", 
											bytesParsed, lsize);
		bytesParsed = lsize;
	}

		// return the number of bytes parsed.
	return bytesParsed;
}



// =============================================
// This virtual function provides the interface 
// for sending data.
// =============================================

void IOCPServer::SendMsg(int size, const void *dataPtr, int flags, std::vector<TagID> * receiptList)
{
		// Variables.
	Chronos::BStream *	stream		= NULL;
	int					targetSize	= 20;	
	IOCPSocketMap::iterator	nit;
		
		// check the size to make sure it is not too large or negative.
	if((size < 0) || (size > 4000))
	{
		return;
	}

	unsigned char messIndex = ((unsigned char *)dataPtr)[0];

		// if we are sending to all clients.
	if(receiptList == NULL)
	{
		m_mutex.Lock();
		
		nit = m_socketList.begin();

		while(nit != m_socketList.end())
		{
			if(stream == NULL)
			{
					// create the bstream.
				stream	= new Chronos::BStream(size + targetSize);

					// add new message tag.
				*stream << (sint)MESSAGE_START_TAG;
					
					// set the total size of the message.
				*stream << (sint)(targetSize + size);

					// write out that we are sending this message to a server.
				*stream << (sint)SOCKET_TARGET_CLIENT;

					// stream in the target count.
				*stream << (sint)0;

					// write out the size of the message.
				*stream << (sint)size;

					// copy the data into the stream
				stream->write(dataPtr, size);
			}

				// post  it to the clients.
			{
				bytesOut += stream->used();
				messSent[messIndex] += 1;
				messBytes[messIndex] += size;
				if((*nit).second->Send(stream->buffer(), stream->used()) == false)
				{
					/*
					bytesOut += stream->used();
					messSent[messIndex] += 1;
					messBytes[messIndex] += size;

					
					if((*nit).second->m_backupSendFifo.IsFull() == false)
					{
						(*nit).second->m_backupSendFifo.Push(stream);
						stream = NULL;
					}
					*/
				}
			}
				
			++nit;
		}

		delete stream;

		m_mutex.Unlock();
	}

	else
	{
			// lock it up.
		m_mutex.Lock();

			// loop through the receipt list.
		for(int i = 0; i < receiptList->size(); ++i)
		{
				// find the element.
			nit = m_socketList.find((*receiptList)[i]);

				// check and see if the buffer is null.
			if(stream == NULL)
			{
					// create the bstream.
				stream	= new Chronos::BStream(size + targetSize);
				
					// add new message tag.
				*stream << (sint)MESSAGE_START_TAG;

					// set the total size of the message.
				*stream << (sint)(targetSize + size);

					// write out that we are sending this message to a server.
				*stream << (sint)SOCKET_TARGET_CLIENT;

					// stream in the target count.
				*stream << (sint)0;

					// write out the size of the message.
				*stream << (sint)size;

					// copy the data into the stream
				stream->write(dataPtr, size);
			}

			if(nit != m_socketList.end())
			{
					// post  it to the clients.
				{
					bytesOut += stream->used();
					messSent[messIndex] += 1;
					messBytes[messIndex] += size;
					if((*nit).second->Send(stream->buffer(), stream->used()) == false)
					{
						/*bytesOut += stream->used();
						messSent[messIndex] += 1;
						messBytes[messIndex] += size;
							
						if((*nit).second->m_backupSendFifo.IsFull() == false)
						{
							(*nit).second->m_backupSendFifo.Push(stream);
							stream = NULL;
						} */
					}
				}
			}
		}
	
			// delete the stream if it is still around.
		delete stream;
			
			// unlock the buffer.
		m_mutex.Unlock();
	}
}

// =============================================
// This function gets the next message in the 
// radio list of messages sent to this radio.
//
// The format of the Buffer:
// <0xDEADBEEF>(4) <total size>(4) <ID count>(4)
// <ID ..>(4 * count) <data size>(4) <data>(size)
// =============================================

void IOCPServer::GetNextMsg(char *bufferToFill, int & size, int * fromSocket, std::vector<TagID> * receiptList)
{
		// Variables.
	long				targetCount		= 0;
	TagID				tagID			= TagID("NONE");
	IOCPServerMessage *		serverMessage	= NULL;
	int					lsize			= 0;
	int					tmpID			= 0;

		// if it is empty?
	if(m_recvFifo.IsEmpty())
	{
		size = 0;
		return;
	}

		// if bufferToFill is null, then just return the size.
	if(bufferToFill == NULL)
	{
		do
		{
			if(m_recvFifo.GetTail(serverMessage) == false)
			{
				size = 0;
				return;
			}

				// get the number of target IDs.
			serverMessage->m_message->setPosition(12);
			*(serverMessage->m_message) >> targetCount;

				// the size of the message is target count * 4 - used size.
			size = serverMessage->m_message->used() - ((targetCount * 4) + 20);

				// if the stream size and calc size don't match, then delete it and continue.
			if(size != serverMessage->m_message->used() - 20)
			{
				if(m_recvFifo.Pop(serverMessage) == true)
				{
					Chronos::InfoMessages::traceMessage("IOCPServer Error: Invalid message size ... dropping.\n");
					delete serverMessage->m_message;
					delete serverMessage;
				}

			}

			else
			{
				break;
			}

		}while(1);

		return;
	}

		// get the real message.
	m_recvFifo.Pop(serverMessage);
		
	bytesIn += serverMessage->m_message->used();

		// start at the begining.
	serverMessage->m_message->setPosition(12);

		// read in the IDs.
	*(serverMessage->m_message) >> targetCount;

		// clear the array.
	if(receiptList != NULL)
	{
		receiptList->clear();
	}

	for(int i = 0; i < targetCount; ++i)
	{
		*(serverMessage->m_message) >> tmpID;

		if(receiptList != NULL)
		{
			receiptList->push_back(TagID(tmpID));
		}
	}

		// get the size of the buffer.
	*(serverMessage->m_message) >> (sint)lsize;

		// copy the buffer into the one passed in.
	serverMessage->m_message->read(bufferToFill, size);

		// set the from socket if it is passed in.
	if(fromSocket != NULL)
	{
		*fromSocket = serverMessage->m_socketHandle;
	}

		// delete the packet.
	delete serverMessage->m_message;
	delete serverMessage;
}


// =============================================
// This function returns TRUE if this server is 
// connected to any clients.
// =============================================

bool IOCPServer::IsConnected(void)
{
    return FALSE;
}

// =============================================
// This function finds a socket by handle.
// =============================================

IOCPSocket * IOCPServer::findSocket(int handle)
{
		// Variables.
	IOCPSocketMap::iterator	sit;

		// find the socket handle.
	sit = m_socketList.find(handle);

		// if the handle is equal to end.
	if(sit == m_socketList.end())
	{
		return NULL;
	}

		// return
	return (*sit).second;
}


// =============================================
void IOCPServer::LogMessage(char *text)
{
	Chronos::InfoMessages::traceMessage(text);
}


// =============================================
// This function gets called when a socket completes a connection.
bool IOCPServer::OnConnect(int hSocket, int iError)
{
		// see if we have an error.
	if(iError == 0)
	{
			// if we are a client then inc the count.
		InterlockedIncrement(&m_curConnections);

			// Post a message that a new socket has been connected.
//		m_logFifo.Push("Connection Established.");

		return true;
	}

	else
	{
			// Post a message that a new socket has been connected.
//		m_logFifo.Push("Connection Attempt Failed.");

			// lock it
		m_clLock.Lock();

			// Variables.
		IOCPSocket *	pSocket = NULL;

			// find the socket.
		pSocket = findSocket(hSocket);
		if(pSocket)
		{
			m_socketList.erase(hSocket);
			pSocket->Shutdown();
			m_deleteFifo.Push(pSocket);
		}

		m_clLock.Unlock();

		return false;
	}
}

// =============================================
// This function gets called when a socket accepts a socket connection.
bool IOCPServer::OnAccept(int hListenSocket, int hNewSocket, int iError)
{
		// see if we have an error.
	if(iError == 0)
	{

		if(m_curConnections < m_maxConnections)
		{
			{
					// create the socket.
				IOCPSocket *	pSocket = new IOCPSocket(hNewSocket);

					// set it up.
				pSocket->Startup(4096, this);
			
				m_clLock.Lock();
				m_socketList.insert(IOCPSocketMap::value_type(hNewSocket, pSocket));
//				m_socketList.SetAt(hNewSocket, pSocket);
				InterlockedIncrement(&m_curConnections);
				m_clLock.Unlock();

				int address = pSocket->m_iClientAddress;
				char *cAddr = (char *) &address;

				// tell my owner about this connection.
				if(m_recvFifo.IsFull() == false)
				{
					MessAccept messageAccept;
					messageAccept.IP[0] = cAddr[0];
					messageAccept.IP[1] = cAddr[1];
					messageAccept.IP[2] = cAddr[2];
					messageAccept.IP[3] = cAddr[3];
//					SetCheckSum((char *) &messageAccept, sizeof(messageAccept));

					IOCPServerMessage *	serverMessage	= new IOCPServerMessage();
					serverMessage->m_message			= new BStream(sizeof(messageAccept) + 20);
					serverMessage->m_socketHandle		= hNewSocket;

						// add new message tag.
					*(serverMessage->m_message) << (sint)MESSAGE_START_TAG;
						
						// set the total size of the message.
					*(serverMessage->m_message) << (sint)(sizeof(messageAccept) + 20);

						// write out that we are sending this message to a server.
					*(serverMessage->m_message) << (sint)(SOCKET_TARGET_SERVER);

						// stream in the target count.
					*(serverMessage->m_message) << (sint)0;

						// write out the size of the message.
					*(serverMessage->m_message) << (sint)sizeof(messageAccept);

					// copy the data into the stream
					serverMessage->m_message->write(&messageAccept, sizeof(messageAccept));

					m_receiveMutex.Lock();
					m_recvFifo.Push(serverMessage);
					m_receiveMutex.Unlock();
				}

			}
		}
	}

	return true;
}

// =============================================
// This function gets called when a socket receives data.
int IOCPServer::OnRecv(int hSocket, const char * pData, int iSize, int iError)
{

	if(iError == 0)
		int bytesParsed = handleReceive(pData, iSize, hSocket);


		// Variables.
//	CString	msg;

		// see if we have an error.
	if(iError == 0)
	{
			// make a message.
//		msg.Format("%s", pData);

			// Post a message that a new socket has been connected.
//		m_logFifo.Push(msg);

			// if the auto reply is on then send it back.
		/*
		if((m_appType == AT_SERVER) && (m_bAutoRespond == TRUE))
		{
				// lock the list.
			m_clLock.Lock();

				// variables.
			IOCPSocket * pSocket	= NULL;

				// lookup the socket
			if(m_socketList.Lookup(hSocket, pSocket) == TRUE)
			{
				pSocket->Send(pData, iSize);
			}

			m_clLock.Unlock();
		}
		*/
	}

	else
	{
			// create the message.
//		msg.Format("Receive Socket Error: %d", iError);

			// Post a message that a new socket has been connected.
//		m_logFifo.Push(msg);
	}

	return iSize;
}

// =============================================
// This function gets called when a socket receives a disconnect.
bool IOCPServer::OnDisconnect(int hSocket, int iError)
{
		// see if we have an error.
	if(iError == 0)
	{
		// lock it
		m_clLock.Lock();

			// Variables.
		IOCPSocket *	pSocket = NULL;

			// find the socket.
		IOCPSocketMap::iterator	sit = m_socketList.find(hSocket);
		if(sit != m_socketList.end())
		{

			IOCPSocket *	pSocket = NULL;

			// find the socket.
			pSocket = findSocket(hSocket);
			if(pSocket)
			{
				m_socketList.erase(hSocket);
				pSocket->Shutdown();
				m_deleteFifo.Push(pSocket);
				InterlockedDecrement(&m_curConnections);
			}

		}

		m_clLock.Unlock();


		// tell my owner about this connection.
		
		MessClose messageClose;

		IOCPServerMessage *	serverMessage		= new IOCPServerMessage();
		serverMessage->m_message			= new BStream(sizeof(messageClose) + 20);
		serverMessage->m_socketHandle		= hSocket;

			// add new message tag.
		*(serverMessage->m_message) << (sint)MESSAGE_START_TAG;
			
			// set the total size of the message.
		*(serverMessage->m_message) << (sint)(sizeof(messageClose) + 20);

			// write out that we are sending this message to a server.
		*(serverMessage->m_message) << (sint)(SOCKET_TARGET_SERVER);

			// stream in the target count.
		*(serverMessage->m_message) << (sint)0;

			// write out the size of the message.
		*(serverMessage->m_message) << (sint)sizeof(messageClose);

		// copy the data into the stream
		serverMessage->m_message->write(&messageClose, sizeof(messageClose));

		m_receiveMutex.Lock();
		m_recvFifo.Push(serverMessage);
		m_receiveMutex.Unlock();
	}	
		
	else
	{
			// Post a message that a new socket has been connected.
//		m_logFifo.Push("Client Disconnect Errored.");
	}

	return true;
}

// =============================================
// This function gets called when a socket has backed up on sending data and 
// it can now send.
void IOCPServer::OnWrite(int hSocket, int iError)
{
		// Variables.
//	CString	msg;

		// see if we have an error.
	if(iError == 0)
	{
			// make a message.
		Chronos::InfoMessages::traceMessage("Write Message Received.");

			// Post a message that a new socket has been connected.
//		m_logFifo.Push(msg);
	}

	else
	{
			// create the message.
		Chronos::InfoMessages::traceMessage("Write Socket Error: %d", iError);

			// Post a message that a new socket has been connected.
//		m_logFifo.Push(msg);
	}
}

// =============================================
void IOCPServer::CollectGarbage(void)
{

	IOCPSocket *	pSocket;

	while(m_deleteFifo.Pop(pSocket) == true)
	{
		delete pSocket;
		pSocket = NULL;
	}
}
