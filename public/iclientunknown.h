//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef ICLIENTUNKNOWN_H
#define ICLIENTUNKNOWN_H
#ifdef _WIN32
#pragma once
#endif


#include "tier0/platform.h"
#include "ihandleentity.h"

class IClientNetworkable;
class C_BaseEntity;
class IClientRenderable;
class ICollideable;
class IClientEntity;
class IClientThinkable;
class IClientModelRenderable;
class IClientAlphaProperty;


// This is the client's version of IUnknown. We may want to use a QueryInterface-like
// mechanism if this gets big.

#include "compat_iclientunknown.h"


#endif // ICLIENTUNKNOWN_H
