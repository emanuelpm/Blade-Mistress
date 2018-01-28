// ============================================================================
// 
// Debug.cpp
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

	// local headers.
#include "Debug.h"

	// system headers.
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

using namespace Chronos;

// =============================================
// This function outputs a trace message to the 
// debugger.
// =============================================

void InfoMessages::traceMessage(const char * fmt, ...)
{
		// variables.
	char buffer[1024];
	va_list		ap;

		// parse out the string.
	va_start(ap, fmt);
    vsprintf(buffer, fmt, ap);
	va_end(ap);
		
		// ouput the message.
	OutputDebugString(buffer);
}
