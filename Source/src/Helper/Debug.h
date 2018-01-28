// ============================================================================
// 
// Debug.h
// 
// By: Code Shaman
// 
//     Copyright(C) 1998-2001  All Rights Reserved.
// ============================================================================

#ifndef CHRONOS_DEBUG_H
#define CHRONOS_DEBUG_H

#pragma once

// =============================================
// The function for outputting a trace message
// to the devStudio output window.
// =============================================

namespace Chronos
{
	class InfoMessages
	{
		public:

			static void traceMessage(const char * format, ...);
	};
};


#ifdef _DEBUG
	#define TRACE(x) Chronos::InfoMessages::traceMessage(x)
#else
	#define TRACE(x)
#endif


#endif
