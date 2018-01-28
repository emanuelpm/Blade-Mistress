// ============================================================================
// 
// Iocp.h
// 
// By: Erik Dieckert
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef IOCP_H
#define IOCP_H

	// Local Headers.
#include <Windows.h>

// =============================================
// Iocp
//
// This class provides a class interface for 
// creating an managing an IO completion port.
// =============================================

class Iocp
{
	public:
		
			// Creators.
		Iocp();
		~Iocp();

			// This function creates the io completion port.
		bool	Create(int iMaxConcurrency = -1);

			// This function associates a device with the io completion port.
		bool	AssociateDevice(HANDLE hDevice, DWORD dwCompKey);

			// This function posts a completion status message to the port.
		bool	PostStatus(DWORD dwCompKey, DWORD ulNumBytes = 0, OVERLAPPED * pOverlap = NULL);

			// This function waits for a completion status to be posted.
		bool	WaitForStatus(DWORD * pdwCompKey, DWORD * pdwNumBytes, OVERLAPPED *& pOverlap, DWORD dwMilliWait = INFINITE);

	private:

			// The IO completion port handle.
		HANDLE		m_hIOCP;
};

#endif
