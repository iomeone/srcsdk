//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#if !defined ( DLIGHTH )
#define DLIGHTH
#ifdef _WIN32
#pragma once
#endif

#include "mathlib/vector.h"

class IClientRenderable;

//-----------------------------------------------------------------------------
// Dynamic light structure
//-----------------------------------------------------------------------------

enum
{
	DLIGHT_NO_WORLD_ILLUMINATION = 0x1,
	DLIGHT_NO_MODEL_ILLUMINATION = 0x2,

	// NOTE: These two features are used to dynamically tweak the alpha on displacements
	// which is a special effect for selecting which texture to use. If 
	// we ever change how alpha is stored for displacements, we'll have to kill this feature
	DLIGHT_ADD_DISPLACEMENT_ALPHA = 0x4,
	DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA = 0x8,
	DLIGHT_DISPLACEMENT_MASK = (DLIGHT_ADD_DISPLACEMENT_ALPHA | DLIGHT_SUBTRACT_DISPLACEMENT_ALPHA),
};

#include "compat_dlight.h"

inline float dlight_t::GetRadius() const
{
	return radius;
}

inline float dlight_t::GetRadiusSquared() const
{
	return radius * radius;
}

inline float dlight_t::IsRadiusGreaterThanZero() const
{
	return radius > 0.0f;
}

#endif
