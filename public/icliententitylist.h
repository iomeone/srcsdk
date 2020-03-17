//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#if !defined( ICLIENTENTITYLIST_H )
#define ICLIENTENTITYLIST_H

#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

class IClientEntity;
class ClientClass;
class IClientNetworkable;
class CBaseHandle;
class IClientUnknown;

// Cached info for networked entities.
// NOTE: Changing this changes the interface between engine & client
struct EntityCacheInfo_t
{
	// Cached off because GetClientNetworkable is called a *lot*
	IClientNetworkable *m_pNetworkable;
	unsigned short m_BaseEntitiesIndex;	// Index into m_BaseEntities (or m_BaseEntities.InvalidIndex() if none).
	unsigned short m_bDormant;	// cached dormant state - this is only a bit
};

//-----------------------------------------------------------------------------
// Purpose: Exposes IClientEntity's to engine
//-----------------------------------------------------------------------------

#include "compat_icliententitylist.h"

extern IClientEntityList *entitylist;

#define VCLIENTENTITYLIST_INTERFACE_VERSION	"VClientEntityList003"

#endif // ICLIENTENTITYLIST_H
