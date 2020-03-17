//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef MINMAX_H
#define MINMAX_H

#if !defined(POSIX) && !defined __MINGW32__
#ifndef min
#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)  (((a) > (b)) ? (a) : (b))
#endif
#else
#if defined( fopen ) && defined( __GNUC__ ) && __GNUC__ < 5
#undef fopen
#endif

#include <algorithm>

using std::min;
using std::max;
#endif

#endif // MINMAX_H
