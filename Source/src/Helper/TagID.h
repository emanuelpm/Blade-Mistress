// ============================================================================
// 
// TagID.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef TAG_ID_H
#define TAG_ID_H

	// Needed Headers.
#include "BStream.h"

// =============================================
// This class defines a small class that
// represents an ID number in 4 chars form.
// =============================================

class TagID
{
	public:

			// creators.
		TagID(const char *ID);
		TagID(long ID);
		TagID(const TagID & tag);
		~TagID();

			// accessors.
		operator long () const;

	private:

		char	m_ID[5];
};

// =============================================
// Creators.
// =============================================

inline TagID::TagID(const char *ID)
{
	m_ID[0] = ID[0];	
	m_ID[1] = ID[1];	
	m_ID[2] = ID[2];	
	m_ID[3] = ID[3];
	m_ID[4] = 0;	
}

inline TagID::TagID(long ID)
{
	m_ID[0] = *((char *)&ID);	
	m_ID[1] = *(((char *)&ID) + 1);	
	m_ID[2] = *(((char *)&ID) + 2);	
	m_ID[3] = *(((char *)&ID) + 3);	
	m_ID[4] = 0;	
}

inline TagID::TagID(const TagID & tag)
{
	m_ID[0] = tag.m_ID[0];	
	m_ID[1] = tag.m_ID[1];	
	m_ID[2] = tag.m_ID[2];	
	m_ID[3] = tag.m_ID[3];	
	m_ID[4] = 0;	
}

inline TagID::~TagID()
{}

// =============================================
// accessors.
// =============================================

inline TagID::operator long () const
{
	return *((long *)m_ID);
}

#endif
