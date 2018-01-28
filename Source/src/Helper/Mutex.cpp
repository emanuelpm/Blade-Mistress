// ============================================================================
// 
// Mutex.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// Local Headers.
#include "Mutex.h"

	// System Headers.
#include <windows.h>

// =============================================
// Creators.
// =============================================

Mutex::Mutex()
{
	m_cs = new _RTL_CRITICAL_SECTION;
	InitializeCriticalSection(m_cs);
}

Mutex::~Mutex()
{
	DeleteCriticalSection(m_cs);
}

// =============================================
// This function locks the mutex.
// =============================================

void Mutex::Lock()
{
	EnterCriticalSection(m_cs);
}

// =============================================
// This function unlocks the mutex.
// =============================================

void Mutex::Unlock()
{
	LeaveCriticalSection(m_cs);
}