//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef VCOLLIDE_PARSE_H
#define VCOLLIDE_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#include "vphysics_interface.h"

struct fluid_t
{
	int index;
	char surfaceprop[512];

	fluidparams_t params;

	fluid_t() {}
	fluid_t( fluid_t const& src ) : params(src.params)
	{
		index = src.index;
	}
};

struct ragdollcollisionrules_t
{
	void Defaults( IPhysics *pPhysics, IPhysicsCollisionSet *pSetIn )
	{
		pCollisionSet = pSetIn;
		bSelfCollisions = true;
	}
	int	   bSelfCollisions;
	IPhysicsCollisionSet *pCollisionSet;
};

//-----------------------------------------------------------------------------
// Purpose: Pass this into the parser to handle the keys that vphysics does not
// parse.
//-----------------------------------------------------------------------------
class IVPhysicsKeyHandler
{
public:
	virtual void ParseKeyValue( void *pData, const char *pKey, const char *pValue ) = 0;
	virtual void SetDefaults( void *pData ) = 0;
};

#include "compat_vcollide_parse.h"

#endif // VCOLLIDE_PARSE_H
