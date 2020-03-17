//========= Copyright Valve Corporation, All rights reserved. ============//
#if !defined(POSIX) && !defined __MINGW32__
#ifndef min
	#define min(a,b)  Min(a, b)
#endif
#ifndef max
	#define max(a,b)  Max(a, b)
#endif
#endif
