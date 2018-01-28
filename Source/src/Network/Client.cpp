// ============================================================================
// 
// Client.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// System Headers.
#include <winsock2.h>
#include <windows.h>

	// local headers.
#include "Client.h"
#include "../helper/Debug.h"

#include "NetWorldMessages.h"

using namespace Chronos;

// =============================================
// Default constructor and destructor.
// =============================================

Client::Client()
	:	NetWorldRadio("CLNT", false), m_port(0), m_socket(NULL), m_backupSendFifo(10000),
		m_recvFifo(10000), m_address(0)
{
}

Client::~Client()
{
	disconnect();
}

// =============================================
// This function attempts to connect to a 
// given server.
// =============================================

bool Client::connect(const std::string & serverName, int port)
{
		// set the parameters.
	m_port				= port;

		// kill the old socket if it is around.
	if(m_socket != NULL)
	{
		disconnect();
	}

		// create the listen socket.
	m_socket = new TCPSocket();
	
		// startup the socket.
	if(m_socket->startup(65536 * 2, NetCallback, this) == false)
	{
		disconnect();
		return false;
	}

		// start listening on the port.
	if(m_socket->connect(serverName, port) == false)
	{
		disconnect();
		return false;
	}

		// set the server address.
	m_address = m_socket->clientAddress();

	return true;
}

// =============================================
// This function stops the current Client that 
// is running.
// =============================================

bool Client::disconnect()
{
	m_closeMutex.Lock();

		// cleanup the socket.
	delete m_socket;
	m_socket = NULL;

		// clean out the backup send queue.
	while(!m_backupSendFifo.IsEmpty())
	{
		Chronos::BStream * stream = NULL;
		m_backupSendFifo.Pop(stream);
		delete stream;
	}

		// clean out the receive queue.
	while(!m_recvFifo.IsEmpty())
	{
		Chronos::BStream * stream = NULL;
		m_recvFifo.Pop(stream);
		delete stream;
	}

	m_closeMutex.Unlock();

	return true;
}

// =============================================
// This function gets called when any of the
// Client managed sockets have an event get 
// signalled.
// =============================================

int Client::NetCallback(TCPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
						int size, void * context)
{
		// varaibles.
	TCPSocket *	pSocket		= socket;
	Client *	lthis		= (Client *)context;
	int			newHandle	= 0;
	int			bytesParsed = 0;

		// lets check the socket and make sure we are not on an invalid object.
	if(socket != lthis->m_socket)
	{
		return -1;
	}

		// lock our close mutex.
	lthis->m_closeMutex.Lock();

		// handle sending failed messages.
	if((events.lNetworkEvents & FD_CONNECT) && (events.iErrorCode[FD_CONNECT_BIT] == 1))
	{
		delete lthis->m_socket;
		lthis->m_socket = NULL;
		lthis->m_closeMutex.Unlock();
		return -1;
	}

		// handle sending failed messages.
	if(events.lNetworkEvents & FD_WRITE)
	{
			// Variables.
		Chronos::BStream *	stream = NULL;

		while(lthis->m_backupSendFifo.GetTail(stream) == true)
		{
				// attempt to send the message out the socket.
			if(!socket->send(stream->buffer(), stream->used()))
			{
				break;
			}

				// remove the message.
			if(lthis->m_backupSendFifo.Pop(stream) == true)
			{
					// cleanup the memory.
				delete stream;
				stream = NULL;
			}
		}
	}

		// handle recevie messages.
	if(events.lNetworkEvents & FD_READ)
	{
		bytesParsed = lthis->handleReceive(buffer, size, socket->socketHandle());
	}

		// after we have handled the user callback, we can handle close events.
	if(events.lNetworkEvents & FD_CLOSE)
	{
		Chronos::BStream * stream = NULL;

			// delete the object.
		delete lthis->m_socket;
		lthis->m_socket = NULL;

			// clean out the backup send queue.
		while(!lthis->m_backupSendFifo.IsEmpty())
		{
			stream = NULL;
			lthis->m_backupSendFifo.Pop(stream);
			delete stream;
		}

			// clean out the receive queue.
		while(!lthis->m_recvFifo.IsEmpty())
		{
			stream = NULL;
			lthis->m_recvFifo.Pop(stream);
			delete stream;
		}

			// exit the thread, we are done.
		bytesParsed = -1;	 

		// tell my owner about this connection.
		{
			MessClose messageClose;
//			messageClose.checkSum = messageClose.idTag;
			stream = new BStream(sizeof(messageClose) + 20);

				// add new message tag.
			*(stream) << (sint)MESSAGE_START_TAG;
				
				// set the total size of the message.
			*(stream) << (sint)(sizeof(messageClose) + 20);

				// write out that we are sending this message to a server.
			*stream << (sint)(SOCKET_TARGET_CLIENT);

				// stream in the target count.
			*(stream) << (sint)0;

				// write out the size of the message.
			*(stream) << (sint)sizeof(messageClose);

			// copy the data into the stream
			stream->write(&messageClose, sizeof(messageClose));

			if (!lthis->m_recvFifo.IsFull())
				lthis->m_recvFifo.Push(stream);
		}

	}

	lthis->m_closeMutex.Unlock();

	return bytesParsed;
}

// =============================================
// This function gets called when any of the
// server managed sockets have an event get 
// signalled.
// =============================================

int Client::UDPNetCallback(UDPSocket *socket, struct _WSANETWORKEVENTS &events, const char *buffer, 
							int size, void * context)
{
		// varaibles.
	Client *	lthis		= (Client *)context;
	int			bytesParsed = 0;

		// handle recevie messages.
	if(events.lNetworkEvents & FD_READ)
	{
		bytesParsed = lthis->handleReceive(buffer, size, socket->socketHandle());
	}

	return bytesParsed;
}

// =============================================
// This function process data received on the 
// socket and returns the number of bytes 
// processed.
// =============================================

int Client::handleReceive(const char * buffer, int size, int socketHandle)
{
		// Check to make sure we did not get a negative size.
	if(size < 0)
	{
		Chronos::InfoMessages::traceMessage("Client Error: Size from socket was negative.  %d\n", size);
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
				if(walker1 - buffer == lsize)
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
				Chronos::InfoMessages::traceMessage("Client Error: Size Mismatch! packet: %d  calculated: %d\n", 
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
		if(mTarget == SOCKET_TARGET_CLIENT)
		{
				// check to see if we have a complete message.
			if(mSize <= lsize - bytesParsed)
			{
					// if the receive fifo is not full.
				if(m_recvFifo.IsFull() == false)
				{
					Chronos::BStream * stream = new BStream(mSize);
					stream->write(tbuffer, mSize);
					m_recvFifo.Push(stream);
				}

					// else we will just drop the message and log it.
				else
				{
					Chronos::InfoMessages::traceMessage("Client Error: receiveFifo FULL! Dropping Message.  Message Size: %d\n", 
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
		Chronos::InfoMessages::traceMessage("Client Error: bytesParsed is greater than the size! bp:%d size:%d\n", 
											bytesParsed, lsize);
		bytesParsed = lsize;
	}

		// return the number of bytes parsed.
	return bytesParsed;
}

// =============================================
// This virtual function provides the interface 
// for sending data.
// The format of the Buffer:
// <0xDEADBEEF>(4) <total size>(4) <ID count>(4)
// <ID ..>(4 * count) <data size>(4) <data>(size)
// =============================================

void Client::SendMsg(int size, const void *dataPtr, int flags, std::vector<TagID> * receiptList)
{
		// check to see if the socket is valid.
	if(m_socket == NULL)
	{
		return;
	}

		// Variables.
	Chronos::BStream *	stream		= NULL;
	int					targetSize	= 20;	

		// adjust the target size.
	if(receiptList != NULL)
	{
		targetSize += 4 * receiptList->size();
	}
		// create the bstream.
	stream	= new Chronos::BStream(size + targetSize);

	if(stream != NULL)
	{
			// add new message tag.
		*stream << (sint)MESSAGE_START_TAG;

			// set the total size of the message.
		*stream << (sint)(targetSize + size);

			// write out that we are sending this message to a server.
		*stream << (sint)(SOCKET_TARGET_SERVER);

			// stream in the target count.

		if(receiptList != NULL)
		{
			*stream << (sint)receiptList->size();

			for(int i = 0; i < receiptList->size(); ++i)
			{
				*stream << (sint)(*receiptList)[i];
			}
		}

		else
		{
			*stream << (sint)0;
		}

			// write out the size of the message.
		*stream << (sint)size;

			// copy the data into the stream
		stream->write(dataPtr, size);

			// post  it to the clients.
		if(flags & SOCK_FLAG_UNGUARANTEED)
		{
			UDPSocket * udp = UDPSocket::instance();

			if(udp != NULL)
			{
				udp->send(m_address, m_port, stream->buffer(), stream->used());
			}

			delete stream;
		}

		else
		{
			if(m_socket->send(stream->buffer(), stream->used()) == false)
			{
				if(m_backupSendFifo.IsFull() == true)
				{
					delete stream;
				}

				else
				{
					m_backupSendFifo.Push(stream);
				}
			}

			else
			{
				delete stream;
			}
		}
	}
}

// =============================================
// This function gets the next message in the 
// radio list of messages sent to this radio.
// =============================================

void Client::GetNextMsg(char *bufferToFill, int & size, int * fromSocket, std::vector<TagID> * receiptList)
{
		// Variables.
	long				targetCount = 0;
	TagID				tagID		= TagID("NONE");
	Chronos::BStream *	stream		= NULL;
	int					lsize		= 0;
	long				tmpID		= 0;

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
			if(m_recvFifo.GetTail(stream) == false)
			{
				size = 0;
				return;
			}

				// get the number of target IDs.
			stream->setPosition(12);
			*stream >> targetCount;

				// the size of the message is target count * 4 - used size.
			size = stream->used() - ((targetCount * 4) + 20);

				// if the stream size and calc size don't match, then delete it and continue.
			if(size != stream->used() - 20)
			{
				if(m_recvFifo.Pop(stream) == true)
				{
					Chronos::InfoMessages::traceMessage("Client Error: Invalid message size ... dropping.\n");
					delete stream;
					stream = NULL;
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
	m_recvFifo.Pop(stream);
		
		// start at the begining.
	stream->setPosition(12);

		// read in the IDs.
	*stream >> targetCount;

		// clear the array.
	if(receiptList != NULL)
	{
		receiptList->clear();
	}

	for(int i = 0; i < targetCount; ++i)
	{
		*stream >> tmpID;

		if(receiptList != NULL)
		{
			receiptList->push_back(TagID(tmpID));
		}
	}

		// get the data size.
	*stream >> (sint)lsize;

		// copy the buffer into the one passed in.
	stream->read(bufferToFill, size);

		// if we want a from socket handle, then give it.
	if(fromSocket != NULL)
	{
		if(m_socket != NULL)
		{
			*fromSocket = m_socket->socketHandle();
		}

		else
		{
			*fromSocket = 0;
		}
	}

		// delete the packet.
	delete stream;
}

// =============================================
// This function returns TRUE if this client is 
// connected to a server.
// =============================================

bool Client::IsConnected(void)
{
	if (m_socket)
	{
		return TRUE;
	}

	return FALSE;
}

void Client::CollectGarbage(void)
{
	;
}

