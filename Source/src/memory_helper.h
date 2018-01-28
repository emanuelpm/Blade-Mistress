#ifndef __MEMORY_HELPER_H
#define __MEMORY_HELPER_H

	#ifdef _DETECT_LEAKS


		void AddTrack( DWORD addr, DWORD asize, const char *fname, DWORD lnum );
		void RemoveTrack( DWORD addr );
		void DumpUnfreed();
		void DetectMemoryLeaksCRT();


		#undef new
		#undef delete

		inline void * __cdecl operator new( unsigned int size,
												const char *file, int line )
		{
			void *ptr = (void *)malloc(size);
			AddTrack((DWORD)ptr, size, file, line);
			return(ptr);
		}

		inline void __cdecl operator delete( void *p )
		{
			RemoveTrack((DWORD)p);
			free(p);
		}


		#define DEBUG_NEW new( __FILE__, __LINE__ )
	#else
		#define DEBUG_NEW new
	#endif


	#define new DEBUG_NEW

#endif

