// ============================================================================
// 
// BStream.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_BSTREAM_H
#define CHRONOS_BSTREAM_H

#pragma once

	// Needed Headers.
#include "Types.h"
#include <string>

namespace Chronos
{
	// =============================================
	// These are the Enums for errors.
	// =============================================

	enum BSError
	{
		BS_OK,		// the BStream is ok.
		BS_EOB,		// End of buffer was reached.
	};
	
	// =============================================
	// This class defines the Byte Stream class 
	// which creates a byte buffer that can be 
	// written into.
	// =============================================

	class BStream
	{
		public:
			
				// Creators.
			BStream(int bufSize);
			~BStream();

				// These functions stream from the 
				// buffer into the variables passed in.
			BStream &	operator>>(uchar & val);
			BStream &	operator>>(schar & val);
			BStream &	operator>>(ushort & val);
			BStream &	operator>>(sshort & val);
			BStream &	operator>>(uint & val);
			BStream &	operator>>(sint & val);
			BStream &	operator>>(ulong & val);
			BStream &	operator>>(slong & val);
			BStream &	operator>>(uint64 & val);
			BStream &	operator>>(sint64 & val);
			BStream &	operator>>(f32 & val);
			BStream &	operator>>(f64 & val);
			BStream &	operator>>(std::string & val);
			void		read(void *data, int size);

				// These functions stream data from the variables
				// to the buffer.
			BStream &	operator<<(uchar val);
			BStream &	operator<<(schar val);
			BStream &	operator<<(ushort val);
			BStream &	operator<<(sshort val);
			BStream &	operator<<(uint val);
			BStream &	operator<<(sint val);
			BStream &	operator<<(ulong val);
			BStream &	operator<<(slong val);
			BStream &	operator<<(uint64 val);
			BStream &	operator<<(sint64 val);
			BStream &	operator<<(f32 val);
			BStream &	operator<<(f64 val);
			BStream &	operator<<(const std::string &val);
			void		write(const void *data, int size);
		
				// These functions control the current position of the buffer.
			int			position() const;
			void		setPosition(int pos);

				// These functions get the current sizes of the buffer.
			int			size() const;
			int			used() const;
			int			free() const;

				// This function gets the last error.
			BSError		lastError() const;

				// This function resets the buffer and makes it like a new buffer.
			void		reset();

				// This function gets the pointer to the buffer.
			char *		buffer();

		private:

			char *		m_buffer;
			int			m_size;
			int			m_pos;
			int			m_usedEnd;
			BSError		m_lastError;
	};

	// =============================================
	// This function gets the current position of
	// the buffer.
	// =============================================

	inline int	BStream::position() const
	{
		return m_pos;
	}

	// =============================================
	// This function gets the size of the buffer.
	// =============================================
	
	inline int BStream::size() const
	{
		return m_size;
	}

	// =============================================
	// This function sets the current position in
	// the buffer.
	// =============================================

	inline void BStream::setPosition(int pos)
	{
		m_pos = pos;
	}

	// =============================================
	// This function gets the current used size 
	// of the buffer.
	// =============================================
	
	inline int BStream::used() const
	{
		return m_usedEnd;
	}

	// =============================================
	// This function gets the ammount of free 
	// space left in the buffer.
	// =============================================

	inline int BStream::free() const
	{
		return m_size - m_usedEnd;
	}

	// =============================================
	// This function gets the last error.
	// =============================================

	inline BSError BStream::lastError() const
	{
		return m_lastError;
	}
	
	// =============================================
	// This function resets the buffer.
	// =============================================

	inline void BStream::reset()
	{
		m_pos		= 0;
		m_usedEnd	= 0;
		m_lastError	= BS_OK;
	}

	// =============================================
	// This function gets the pointer to the BSTream
	// buffer.
	// =============================================

	inline char * BStream::buffer()
	{
		return m_buffer;
	}
};

#endif
