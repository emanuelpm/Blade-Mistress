// ============================================================================
// 
// Mutex.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_MUTEX_H
#define CHRONOS_MUTEX_H

	// forward declarations.
struct _RTL_CRITICAL_SECTION;

// =============================================
// This class creates a mutex for locking out
// thread execution.
// =============================================

class Mutex
{
	public:

			// Creators.
		Mutex();
		~Mutex();

			// these are the control functions.
		void Lock();
		void Unlock();

	private:
	
		struct _RTL_CRITICAL_SECTION *	m_cs;
		
};

#endif
