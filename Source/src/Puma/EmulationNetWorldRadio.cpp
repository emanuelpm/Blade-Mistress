// ============================================================================
// 
// EmulationNetWorldRadio.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local Headers.
#include "EmulationNetWorldRadio.h"

// =============================================
// Creators.
// =============================================

EmulationNetWorldRadio::EmulationNetWorldRadio(const TagID & tagID, bool isServer)
					:	NetWorldRadio(tagID, isServer), m_serverConnection(NULL)
{
}

EmulationNetWorldRadio::~EmulationNetWorldRadio()
{
		// clean up the packets.
	std::list<Chronos::BStream *>::iterator	bit = m_packetList.begin();

	while(bit != m_packetList.end())
	{
		delete (*bit);
		++bit;
	}

		// clear out the list.
	m_clientList.clear();
	m_packetList.clear();
}

// =============================================
// These functions allow the Emulation class to 
// connect and disconnect form/to another 
// NetWorldRadio class.
// =============================================

void EmulationNetWorldRadio::Connect(NetWorldRadio *other)
{
	if(m_isServer == false)
    {
			// if I'm already connected to a server...
        if(m_serverConnection)
        {
            m_serverConnection->Disconnect(this);
        }

			// only if we have valid other.
		if(other != NULL)
		{
				// set the new server.
			m_serverConnection = other;

				// connect at the server level.
			m_serverConnection->Connect(this);
		}
	}

	else
	{
			// add the client to the list.
		m_clientList.insert(std::map<TagID, NetWorldRadio *>::value_type(other->m_ID, other));
    }
}

void EmulationNetWorldRadio::Disconnect(NetWorldRadio *other)
{
    if(m_isServer == true)
    {
			// Variables.
		std::map<TagID, NetWorldRadio *>::iterator	nit = m_clientList.begin();

			// loop though and 
		while(nit != m_clientList.end())
		{
				// diconnect the client.
			if((*nit).second == other)
			{
					// remove the client.
				m_clientList.erase(nit);

				break;
			}

				// get the next client
			nit = m_clientList.begin();
		}
    }

    else
    {
			// if other is valid then we are called from t
			// if the other connection is null
		if(m_serverConnection != NULL)
		{
			m_serverConnection->Disconnect(this);
		} 

			// clear the server connection.
		m_serverConnection = NULL;
    }

}

// =============================================
// This virtual function provides the interface 
// for sending data.
// =============================================

void EmulationNetWorldRadio::SendMsg(	int size, const void *dataPtr, 
										std::vector<TagID> * receiptList)
{
		// Variables.
	Chronos::BStream *	stream		= NULL;
	int					targetSize	= 0;	

		// if we are the server.
    if(m_isServer == true)
    {
  			// Variables.
		std::map<TagID, NetWorldRadio *>::iterator	nit;
		
			// set the target size.
		targetSize = 4;

		if(receiptList == NULL)
		{
			nit = m_clientList.begin();

			while(nit != m_clientList.end())
			{
					// create the bstream.
				stream	= new Chronos::BStream(size + targetSize);

				if(stream != NULL)
				{
						// stream in the target count.
					*stream << (long)0;

						// copy the data into the stream
					stream->write(dataPtr, size);

						// post  it to the clients.
					(*nit).second->Append(stream);
				}
				
				++nit;
			}
		}

		else
		{
				// loop through the receipt list.
			for(int i = 0; i < receiptList->size(); ++i)
			{
					// find the element.
				nit = m_clientList.find((*receiptList)[i]);

				if(nit != m_clientList.end())
				{
						// create the bstream.
					stream	= new Chronos::BStream(size + targetSize);

					if(stream != NULL)
					{
							// stream in the target count.
						*stream << (long)0;

							// copy the data into the stream
						stream->write(dataPtr, size);

							// post it to the clients.
						(*nit).second->Append(stream);
					}
				}
			}
		}
	}

	else
	{
			// if we have a list of senders then get the size.
		if(receiptList != NULL)
		{
			targetSize = 4 * receiptList->size();
		}

			// if we have a server connection.
        if (m_serverConnection)
        {
				// create the bstream.
			stream	= new Chronos::BStream(size + targetSize + sizeof(long));

			if(stream != NULL)
			{
					// write out all the targets for the server.
				if(receiptList != NULL)
				{
					*stream << receiptList->size();

					for(int i = 0; i < receiptList->size(); ++i)
					{
						*stream << (*receiptList)[i];
					} 
				}

				else
				{
					*stream << (long)0;
				}

					// copy the data into the stream
				stream->write(dataPtr, size);

					// post it to the clients.
				m_serverConnection->Append(stream);
			}
		}
	}
}

// =============================================
// This function gets the next message in the 
// radio list of messages sent to this radio.
// =============================================

void EmulationNetWorldRadio::GetNextMsg(char *bufferToFill, int & size,
										std::vector<TagID> * receiptList)
{
		// Variables.
	std::list<Chronos::BStream *>::iterator	nit			= m_packetList.begin();
	long									targetCount = 0;
	TagID									tagID		= TagID("NONE");
	long									tmpID		= 0;

		// if it is empty?
	if(nit == m_packetList.end())
	{
		size = 0;
		return;
	}

		// if bufferToFill is null, then just return the size.
	if(bufferToFill == NULL)
	{
			// get the number of target IDs.
		(*nit)->setPosition(0);
		*(*nit) >> targetCount;

			// the size of the message is target count * 4 - used size.
		size = ((*nit)->used() - (targetCount * 4)) - sizeof(long);
		return;
	}
		
		// start at the begining.
	(*nit)->setPosition(0);

		// read in the IDs.
	*(*nit) >> targetCount;

		// clear the array.
	if(receiptList != NULL)
	{
		receiptList->clear();
	}

	for(int i = 0; i < targetCount; ++i)
	{
		*(*nit) >> tmpID;

		if(receiptList != NULL)
		{
			receiptList->push_back(TagID(tmpID));
		}
	}

		// copy the buffer into the one passed in.
	(*nit)->read(bufferToFill, size);

		// delete the packet.
	delete (*nit);

		// remove it from the list.
	m_packetList.erase(nit);
}

// =============================================
// This function appends a BStream to the packet 
// list.
// =============================================

void EmulationNetWorldRadio::Append(const Chronos::BStream * message)
{
		// put the bstream on to the end of the packet list.
	m_packetList.push_back(const_cast<Chronos::BStream *>(message));
}
