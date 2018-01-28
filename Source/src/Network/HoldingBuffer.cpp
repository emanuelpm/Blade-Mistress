// ============================================================================
// 
// HoldingBuffer.cpp
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local Includes.
#include "HoldingBuffer.h"

	// System Headers.
#include <memory.h>

// =============================================
// Creators.
// =============================================

HoldingBuffer::HoldingBuffer(int iSize, int iMaxSize, bool bAutoResize)
			:	m_iSize(iSize), m_iMaxSize(iMaxSize), m_bAutoResize(bAutoResize), m_iStartIndex(0),
				m_pBuffer(new char [iSize])
{
}

HoldingBuffer::~HoldingBuffer()
{
	delete [] m_pBuffer;
}

// =============================================
// This function adds data to the buffer.
// =============================================

bool HoldingBuffer::AddData(const char * pData, int & iSize)
{
		// check the size of the data and make sure we have enough space.
	if(m_iStartIndex + iSize >= m_iSize)
	{
		if(m_bAutoResize == true)
		{
				// double the size.
			int tsize = m_iSize * 2;

				// if the size goes over the max size.
			if(m_iMaxSize != -1)
			{
				if(tsize > m_iMaxSize)
				{
					tsize = m_iMaxSize;
				}
			}

				// make sure we still have enough space after the resize.
			if(m_iStartIndex + iSize >= m_iSize)
			{
				return false;
			}

				// create the temp buffer.
			char * tbuff = new char [tsize];

				// copy the data into the buffer.
			memcpy(tbuff, m_pBuffer, m_iSize);

				// delete the old buffer.
			delete [] m_pBuffer;

				// set the main buffer pointer to the new buffer.
			m_pBuffer = tbuff;

				// increment the size by the block size.
			m_iSize = tsize;
		}

		else
		{
			return false;
		}
	}

		// now that we knoe the buffer is large enough, lets copy in our 
		// new data.
	memcpy(m_pBuffer + m_iStartIndex, pData, iSize);

		// set the start index.
	m_iStartIndex += iSize;

		// return the new size.
	iSize = m_iStartIndex;

	return true;
}

// =============================================
// This function reclaims part of the buffer 
// once it has been used.
// =============================================

bool HoldingBuffer::ReclaimBufferSpace(int iSizeToReclaim)
{
		// if we are trying to reclaim more than we should, then reset to the beginning
	if(iSizeToReclaim > m_iStartIndex)
	{
		m_iStartIndex = 0;
		return true;
	}

		// take the size and increment into the buffer from 0 and copy the buffer after
		// that point to the beginning.
	if(iSizeToReclaim > 0)
	{
		memcpy(m_pBuffer, m_pBuffer + iSizeToReclaim, m_iStartIndex - iSizeToReclaim);
	}

		// reset the index.
	m_iStartIndex -= iSizeToReclaim;

	return true;
}
