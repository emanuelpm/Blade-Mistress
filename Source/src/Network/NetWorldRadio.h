// ============================================================================
// 
// NetWorldRadio.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef NET_WORLD_RADIO_H
#define NET_WORLD_RADIO_H

	// needed headers.
#include "../helper/TagID.h"
#include <vector>

enum
{
	SOCK_FLAG_UNGUARANTEED	= 0x00000001,
};

enum
{
	SOCKET_TARGET_CLIENT	= 1,
	SOCKET_TARGET_SERVER	= 2,
};

// =============================================
// Forward declarations.
// =============================================

namespace Chronos
{
	class BStream;
}

// =============================================
// This class defines the NetWorld Radio 
// interface through which the world can
// comunicate.
// =============================================

class NetWorldRadio
{
	public:

			// Creators.
        NetWorldRadio(const TagID & tagID, bool isServer = false) : m_isServer(isServer), m_ID(tagID){ disconnectionFlag = 0; }
		virtual ~NetWorldRadio(){}

			// This virtual function provides the interface for sending
			// data.
		virtual void SendMsg(	int size, const void *dataPtr, int flags = 0, 
								std::vector<TagID> * receiptList = NULL) = 0;
		
			// This function gets the next message in the radio list
			// of messages sent to this radio.
		virtual void GetNextMsg(char *bufferToFill, int & size, int * fromSocket = NULL, std::vector<TagID> * receiptList = NULL) = 0;

			// These functions allow the Emulation class to connect
			// and disconnect form/to another NetWorldRadio class.
		virtual void Connect   (NetWorldRadio * other)					= 0;
		virtual void Disconnect(NetWorldRadio * other = NULL)			= 0;

			// This function appends a BStream to the packet list.
		virtual void Append(const Chronos::BStream * message)			= 0;

            // This function returns TRUE if the radio is currently connected to another radio
		virtual bool IsConnected(void) = 0;

		virtual void CollectGarbage(void) = 0;

			// server flag.	
		bool	m_isServer;
		TagID	m_ID;
		char	playerHandle[24];
        bool    disconnectionFlag;
};

#endif
