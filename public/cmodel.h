//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//

#ifndef CMODEL_H
#define CMODEL_H
#ifdef _WIN32
#pragma once
#endif

#include "trace.h"
#include "tier0/dbg.h"
#include "basehandle.h"

struct edict_t;
struct model_t;

/*
==============================================================

COLLISION DETECTION

==============================================================
*/

#include "bspflags.h"
//#include "mathlib/vector.h"

// gi.BoxEdicts() can return a list of either solid or trigger entities
// FIXME: eliminate AREA_ distinction?
#define	AREA_SOLID		1
#define	AREA_TRIGGERS	2

#include "vcollide.h"

struct cmodel_t
{
	Vector		mins, maxs;
	Vector		origin;		// for sounds or lights
	int			headnode;

	vcollide_t	vcollisionData;
};

struct csurface_t
{
	const char	*name;
	short		surfaceProps;
	unsigned short	flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------

#include "compat_cmodel.h"

// compute inverse delta
inline Vector Ray_t::InvDelta() const
{
	Vector vecInvDelta;
	for ( int iAxis = 0; iAxis < 3; ++iAxis )
	{
		if ( m_Delta[iAxis] != 0.0f )
		{
			vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
		}
		else
		{
			vecInvDelta[iAxis] = FLT_MAX;
		}
	}
	return vecInvDelta;
}

#endif // CMODEL_H

	
#include "gametrace.h"

