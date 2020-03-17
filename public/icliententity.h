//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef ICLIENTENTITY_H
#define ICLIENTENTITY_H
#ifdef _WIN32
#pragma once
#endif


#include "iclientrenderable.h"
#include "iclientnetworkable.h"
#include "iclientthinkable.h"

struct Ray_t;
class CGameTrace;
typedef CGameTrace trace_t;
class CMouthInfo;
class IClientEntityInternal;
struct SpatializationInfo_t;


//-----------------------------------------------------------------------------
// Purpose: All client entities must implement this interface.
//-----------------------------------------------------------------------------

#include "compat_icliententity.h"


#endif // ICLIENTENTITY_H
