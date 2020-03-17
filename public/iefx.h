//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//===========================================================================//

#if !defined( IEFX_H )
#define IEFX_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"
#include "mathlib/vector.h"

struct model_t;
struct dlight_t;
class IMaterial;

#define	MAX_DLIGHTS		32

//-----------------------------------------------------------------------------
// Purpose: Exposes effects api to client .dll
//-----------------------------------------------------------------------------

#include "compat_iefx.h"

#define VENGINE_EFFECTS_INTERFACE_VERSION "VEngineEffects001"

extern IVEfx *effects;

#endif // IEFX_H
