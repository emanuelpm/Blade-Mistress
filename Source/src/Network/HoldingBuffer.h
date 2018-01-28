// ============================================================================
// 
// HoldingBuffer.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef HOLDING_BUFFER_H
#define HOLDING_BUFFER_H

// =============================================
// HoldingBuffer
// 
// This object creates a variable sized buffer
// and manages partial data message 
// concatenation.
// =============================================

class HoldingBuffer
{
	public:

			// Creators.
		HoldingBuffer(int iSize, int iMaxSize, bool bAutoResize = false);
		~HoldingBuffer();

			// This function adds data to the buffer.
		bool	AddData(const char * pData, int & iSize);

			// This function reclaims part of the buffer once it has been used.
		bool	ReclaimBufferSpace(int iSizeToReclaim);

			// This function gets a pointer to the buffer.
		char *	GetBuffer();

	private:

			// the main holding buffer.
		char *				m_pBuffer;

			// the size of the buffer.
		int					m_iSize;

			// the max size the buffer can grow to.
		int					m_iMaxSize;

			// the beginning index.
		int					m_iStartIndex;

			// the auto resize flag.
		bool				m_bAutoResize;

};

// =============================================
// Inline functions.
// =============================================

	// This function gets a pointer to the buffer.
inline char * HoldingBuffer::GetBuffer()
{
	return m_pBuffer;
}

#endif
