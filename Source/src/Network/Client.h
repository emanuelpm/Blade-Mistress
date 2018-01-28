// ============================================================================
// 
// Client.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_CLIENT_H
#define CHRONOS_CLIENT_H

	// Needed Headers.
#include "DisableWarnings.h"
#include "TCPSocket.h"
#include "UDPSocket.h"
#include "../helper/Mutex.h"
#include "NetWorldRadio.h"
#include "../helper/Fifo.h"
#include <map>

// =============================================
// These types are the lists.
// =============================================

typedef Fifo<Chronos::BStream *>		FifoBStream;

// =============================================
// This class handles basic communications for
// a client connecting to a server.
// =============================================

class Client : public NetWorldRadio
{
	public:

			// Creators.
		Client();
		virtual ~Client();

			// This function connects to a remote server.
		bool connect(const std::string & serverName, int port);

			// This function disconnects from the remote server.
		bool disconnect();

		virtual bool IsConnected(void);

			// the UDP callback.
		int	UDPNetCallback(	UDPSocket *, struct _WSANETWORKEVENTS &, const char *, 
							int size, void * context);

	public:

			// This virtual function provides the interface for sending
			// data.
		virtual void SendMsg(int size, const void *dataPtr, int flags = 0, std::vector<TagID> * receiptList = NULL);
		
			// This function gets the next message in the radio list
			// of messages sent to this radio.
		virtual void GetNextMsg(char *bufferToFill, int & size, int * fromSocket = NULL, std::vector<TagID> * receiptList = NULL);

		
		void CollectGarbage(void);

	private:// Disabled functions.

			// This function operates as the network callback for the sockets.
		static int	NetCallback(TCPSocket *, struct _WSANETWORKEVENTS &, const char *, 
								int size, void * context);

			// These functions allow the Emulation class to connect
			// and disconnect form/to another NetWorldRadio class.
		virtual void Connect   (NetWorldRadio * other){}
		virtual void Disconnect(NetWorldRadio * other = NULL){}

			// This function appends a BStream to the packet list.
		virtual void Append(const Chronos::BStream * message){}

	private:

			// this function handles processing receive requests.
		int handleReceive(const char * buffer, int size, int socketHandle);

	private:

			// General settings.
		int					m_port;
		int					m_address;

			// receive fifo.
		FifoBStream			m_recvFifo;

			// this holds the connect socket.
		TCPSocket *			m_socket;
		FifoBStream			m_backupSendFifo;

			// this holds the mutex for locking out access to the socket list.
		Mutex				m_mutex;

			// Close mutex.
		Mutex				m_closeMutex;

};

#endif
