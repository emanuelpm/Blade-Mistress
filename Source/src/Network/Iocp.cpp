// ============================================================================
// 
// Iocp.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local Headers.
#include "Iocp.h"

	// System Headers.
#include <assert.h>

// =============================================
// Creators.
// =============================================

Iocp::Iocp()
	:	m_hIOCP(NULL)
{
}

Iocp::~Iocp()
{
	if(m_hIOCP != NULL)
	{
		CloseHandle(m_hIOCP);
		m_hIOCP = NULL;
	}
}

// =============================================
// This function creates the io completion port.
// =============================================

bool Iocp::Create(int iMaxConcurrency)
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, iMaxConcurrency);
	assert(m_hIOCP != NULL);
	return m_hIOCP != NULL;
}

// =============================================
// This function associates a device with the io 
// completion port.
// =============================================

bool Iocp::AssociateDevice(HANDLE hDevice, DWORD dwCompKey)
{
	bool bOk = (m_hIOCP == CreateIoCompletionPort(hDevice, m_hIOCP, dwCompKey, 0));
	assert(bOk);
	return bOk;
}

// =============================================
// This function posts a completion status 
// message to the port.
// =============================================

bool Iocp::PostStatus(DWORD dwCompKey, DWORD dwNumBytes, OVERLAPPED * pOverlap)
{
	bool bOk	= (PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, dwCompKey, pOverlap) == TRUE);
	assert(bOk);
	return bOk;
}

// =============================================
// This function waits for a completion status 
// to be posted.
// =============================================

bool Iocp::WaitForStatus(DWORD * pdwCompKey, DWORD * pdwNumBytes, OVERLAPPED *& pOverlap, DWORD dwMilliWait)
{
	return GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes, pdwCompKey, &pOverlap, dwMilliWait) == TRUE;
}
