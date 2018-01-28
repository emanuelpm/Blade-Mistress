// ============================================================================
// 
// ISocketHandler.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef ISOCKET_HANDLER_H
#define ISOCKET_HANDLER_H

// =============================================
// ISocketHandler
// 
// This class defines the interface for 
// receiving messages from the socket layer
// to the user of the socket.
// =============================================

class ISocketHandler
{
	public:

			// This function gets called when a socket completes a connection.
		virtual bool	OnConnect(int hSocket, int iError)	= 0;

			// This function gets called when a socket accepts a socket connection.
		virtual bool	OnAccept(int hListenSocket, int hNewSocket, int iError) = 0;

			// This function gets called when a socket receives data.
		virtual int		OnRecv(int hSocket, const char * pData, int iSize, int iError) = 0;

			// This function gets called when a socket receives a disconnect.
		virtual bool	OnDisconnect(int hSocket, int iError) = 0;

			// This function gets called when a socket has backed up on sending data and 
			// it can now send.
		virtual void	OnWrite(int hSocket, int iError) = 0;
};
#endif
