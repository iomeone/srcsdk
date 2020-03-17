//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//
#if !defined( UTIL_REGISTRY_H )
#define UTIL_REGISTRY_H
#ifdef _WIN32
#pragma once
#endif

#include "tier0/platform.h"


//-----------------------------------------------------------------------------
// Purpose: Interface to registry
//-----------------------------------------------------------------------------
#include "compat_iregistry.h"

extern IRegistry *registry;

// Creates it and calls Init
IRegistry *InstanceRegistry( char const *subDirectoryUnderValve );
// Calls Shutdown and deletes it
void ReleaseInstancedRegistry( IRegistry *reg );

#endif // UTIL_REGISTRY_H
