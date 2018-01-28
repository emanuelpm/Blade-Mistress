// ============================================================================
// 
// EmulationNetWorldRadio.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef EMULATION_NET_WORLD_RADIO_H
#define EMULATION_NET_WORLD_RADIO_H

	// Needed Headers.
#include "../helper/DisableWarnings.h"
#include "../network/NetWorldRadio.h"
#include "../helper/BStream.h"
#include <list>
#include <map>

// =============================================
// This class emulates a loopback NetWorld radio
// that can be used in leiu of an actual
// connection.
// =============================================

class EmulationNetWorldRadio : public NetWorldRadio
{
	public:

			// Creators.
		EmulationNetWorldRadio(const TagID & tagID, bool isServer);
		virtual ~EmulationNetWorldRadio();

			// These functions allow the Emulation class to connect
			// and disconnect form/to another NetWorldRadio class.
		virtual void Connect   (NetWorldRadio * other);
		virtual void Disconnect(NetWorldRadio * other = NULL);
	
			// This virtual function provides the interface for sending
			// data.
		virtual void SendMsg(	int size, const void *dataPtr, 
								std::vector<TagID> * receiptList = NULL);
		
			// This function gets the next message in the radio list
			// of messages sent to this radio.
		virtual void GetNextMsg(char *bufferToFill, int & size, std::vector<TagID> * receiptList = NULL);

			// This function appends a BStream to the packet list.
		virtual void Append(const Chronos::BStream * message);

	private:

			// Server Connections.
		NetWorldRadio *	m_serverConnection;

			// list of client.
		std::map<TagID, NetWorldRadio *>	m_clientList;

			// the list of packets.
		std::list<Chronos::BStream *>		m_packetList;

};

#endif
