// ============================================================================
// 
// Stream.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// base includes.
#include "BStream.h"


using namespace Chronos;


// =============================================
// BStream Creators.
// =============================================

BStream::BStream(int bufferSize)
		:	m_size(bufferSize), m_pos(0), m_usedEnd(0)
{
	m_buffer = new char[m_size + 4];
}

BStream::~BStream()
{
	delete m_buffer;
}

// =============================================
// BStream Mutators.
// =============================================

BStream & BStream::operator>>(uchar & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(schar & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(ushort & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(sshort & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(uint & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(sint & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(ulong & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(slong & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(uint64 & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(sint64 & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(f32 & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(f64 & val)
{
	read(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator>>(std::string &val)
{
		// vars.
	int		size	= 0;
	char *	tbuffer	= NULL;

		// read the size.
	read(&size, sizeof(size));
	
		// create a temp buffer;
	tbuffer = new char [size];

		// read the number of bytes.
	read(tbuffer, size);

	val = tbuffer;

	delete tbuffer;

	return *this;
}

BStream & BStream::operator<<(uchar val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(schar val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(ushort val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(sshort val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(uint val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(sint val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(ulong val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(slong val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(uint64 val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(sint64 val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(f32 val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(f64 val)
{
	write(&val, sizeof(val));
	return *this;
}

BStream & BStream::operator<<(const std::string &val)
{
		// get the size.
	int size = val.size();

		// write out the size.
	write(&size, sizeof(size));

		// write out the string.
	write((void *)val.c_str(), size);

	return *this;
}

void BStream::write(const void *data, int size)
{
	if(m_pos + size > m_size)
	{
		m_lastError = BS_EOB;
		return;
	}

	memcpy(m_buffer + m_pos, data, size);
	
	m_pos += size;

	if(m_usedEnd < m_pos)
	{
		m_usedEnd = m_pos;
	}
}

void BStream::read(void *data, int size)
{
	if(m_pos + size > m_usedEnd)
	{
		m_lastError = BS_EOB;
		return;
	}

	memcpy(data, m_buffer + m_pos, size);

	m_pos += size;
}

